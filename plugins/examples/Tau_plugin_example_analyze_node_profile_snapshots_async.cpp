/************************************************************************************************
 * *   Plugin Testing
 * *   Tests basic functionality of a plugin for function registration event
 * *
 * *********************************************************************************************/

#include <TAU.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <utility>

#ifdef TAU_USE_STDCXX11

#include <Profile/TauEnv.h>
#include <Profile/TauMetrics.h>
#include <Profile/TauCollate.h>
#include <Profile/TauUtil.h>
#include <Profile/TauXML.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>

#ifdef TAU_MPI
#include <mpi.h>

#include <list>
#include <vector>

#include <Profile/TauPlugin.h>

#include <Profile/TauTrace.h>

MPI_Comm comm;
MPI_Comm newcomm;
pthread_t worker_thread;
pthread_mutex_t _my_mutex; // for initialization, termination
pthread_cond_t _my_cond; // for timer
int period_microseconds = 2000000;
bool _threaded = true;

int analytics_complete = 1;
sem_t snapshot_mutex;

typedef struct snapshot_buffer {
  double ***gExcl, ***gIncl;
  double_int **gExcl_min, **gIncl_min;
  double_int **gExcl_max, **gIncl_max;
  double **gNumCalls, **gNumSubr;
  double ***sExcl, ***sIncl;
  double **sNumCalls, **sNumSubr;
  double **gAtomicMin, **gAtomicMax;
  double_int *gAtomicMin_min, *gAtomicMax_max;
  double **gAtomicCalls, **gAtomicMean;
  double **gAtomicSumSqr;
  double **sAtomicMin, **sAtomicMax;
  double **sAtomicCalls, **sAtomicMean;
  double **sAtomicSumSqr;
  Tau_unify_object_t *functionUnifier;
  Tau_unify_object_t *atomicUnifier;
  int *numEventThreads;
  int *globalEventMap;
  int *numAtomicEventThreads;
  int *globalAtomicEventMap;
  std::vector <int> top_5_excl_time_mean;
} snapshot_buffer_t;

#define N_SNAPSHOTS 2000
snapshot_buffer_t s_buffer[N_SNAPSHOTS]; //Store upto N_SNAPSHOTS snapshots

int done = 0;

void init_lock(void) {
    if (!_threaded) return;
    pthread_mutexattr_t Attr;
    pthread_mutexattr_init(&Attr);
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_ERRORCHECK);
    int rc;
    if ((rc = pthread_mutex_init(&_my_mutex, &Attr)) != 0) {
        errno = rc;
        perror("pthread_mutex_init error");
        exit(1);
    }
    if ((rc = pthread_cond_init(&_my_cond, NULL)) != 0) {
        errno = rc;
        perror("pthread_cond_init error");
        exit(1);
    }
}

void Tau_stop_worker(void) {

    fprintf(stderr, "Entering stop worker routine...\n");
    pthread_mutex_lock(&_my_mutex);
    done = true;
    pthread_mutex_unlock(&_my_mutex);
        pthread_cond_signal(&_my_cond);
        int ret = pthread_join(worker_thread, NULL);
        if (ret != 0) {
            switch (ret) {
                case ESRCH:
                    // already exited.
                    break;
                case EINVAL:
                    // Didn't exist?
                    break;
                case EDEADLK:
                    // trying to join with itself?
                    break;
                default:
                    errno = ret;
                    perror("Warning: pthread_join failed\n");
                    break;
            }
        }
        pthread_cond_destroy(&_my_cond);
        pthread_mutex_destroy(&_my_mutex);
}

int Tau_plugin_event_end_of_execution(Tau_plugin_event_end_of_execution_data_t *data) {

  //sem_destroy(&snapshot_mutex);

  return 0;
}

void * Tau_plugin_threaded_analytics_(void* data) {
    /* Set the wakeup time (ts) to 2 seconds in the future. */
    struct timespec ts;
    struct timeval  tp;

    while (!done) {
        // wait x microseconds for the next batch.
        gettimeofday(&tp, NULL);
        fprintf(stderr, "Inside thread...\n");
        const int one_second = 1000000;
        // first, add the period to the current microseconds
        int tmp_usec = tp.tv_usec + period_microseconds;
        int flow_sec = 0;
        if (tmp_usec > one_second) { // did we overflow?
            flow_sec = tmp_usec / one_second; // how many seconds?
            tmp_usec = tmp_usec % one_second; // get the remainder
        }
        ts.tv_sec  = (tp.tv_sec + flow_sec);
        ts.tv_nsec = (1000 * tmp_usec);
        pthread_mutex_lock(&_my_mutex);
        int rc = pthread_cond_timedwait(&_my_cond, &_my_mutex, &ts);
        if (rc == ETIMEDOUT) {
        } else if (rc == EINVAL) {
            TAU_VERBOSE("Invalid timeout!\n"); fflush(stderr);
        } else if (rc == EPERM) {
            TAU_VERBOSE("Mutex not locked!\n"); fflush(stderr);
        }
    }
    // unlock after being signalled.
    pthread_mutex_unlock(&_my_mutex);
    pthread_exit((void*)0L);
	return(NULL);
}

void * Tau_plugin_threaded_analytics(void* data) {

  // Protect TAU from itself
  TauInternalFunctionGuard protects_this_function;
  int flag;
  static int index = 0;

#ifdef TAU_MPI
  PMPI_Initialized(&flag);
#endif

 while(!done && flag) {

  sem_wait(&snapshot_mutex); //Block on semaphore
  fprintf(stderr, "Performing analytics...\n");
  //PMPI_Barrier(newcomm);

  analytics_complete = 0;

  //Update the profile!
  TauProfiler_updateAllIntermediateStatistics();

  int rank = 0;
  int world_rank = 0;

#ifdef TAU_MPI

  if(!index)
    PMPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &comm);

   PMPI_Comm_rank(comm, &rank);
   PMPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

#endif


  int numEvents = 0;
  int globalNumThreads;

  int numAtomicEvents = 0;

  if (TauEnv_get_stat_precompute() == 1) {
    // Unification must already be called.
    s_buffer[index].functionUnifier = Tau_unify_getFunctionUnifier();
    numEvents = s_buffer[index].functionUnifier->globalNumItems;
    s_buffer[index].numEventThreads = (int*)TAU_UTIL_MALLOC(numEvents*sizeof(int));
    s_buffer[index].globalEventMap = (int*)TAU_UTIL_MALLOC(numEvents*sizeof(int));
    // initialize all to -1
    for (int i=0; i<s_buffer[index].functionUnifier->globalNumItems; i++) {
      // -1 indicates that the event did not occur for this rank
      s_buffer[index].globalEventMap[i] = -1;
    }
    for (int i=0; i<s_buffer[index].functionUnifier->localNumItems; i++) {
      s_buffer[index].globalEventMap[s_buffer[index].functionUnifier->mapping[i]] = i; // set reverse mapping
    }
    Tau_collate_get_total_threads_MPI(s_buffer[index].functionUnifier, &globalNumThreads, &(s_buffer[index].numEventThreads),
				  numEvents, s_buffer[index].globalEventMap,false);

    Tau_collate_allocateFunctionBuffers(&(s_buffer[index].gExcl), &(s_buffer[index].gIncl),
					&(s_buffer[index].gNumCalls), &(s_buffer[index].gNumSubr),
					numEvents,
					Tau_Global_numCounters,
					COLLATE_OP_BASIC);

    s_buffer[index].gExcl_min = (double_int **)TAU_UTIL_MALLOC(sizeof(double_int *)*Tau_Global_numCounters);
    s_buffer[index].gIncl_min = (double_int **)TAU_UTIL_MALLOC(sizeof(double_int *)*Tau_Global_numCounters);
    s_buffer[index].gExcl_max = (double_int **)TAU_UTIL_MALLOC(sizeof(double_int *)*Tau_Global_numCounters);
    s_buffer[index].gIncl_max = (double_int **)TAU_UTIL_MALLOC(sizeof(double_int *)*Tau_Global_numCounters);

    // Please note the use of Calloc
    for (int m=0; m<Tau_Global_numCounters; m++) {
      s_buffer[index].gExcl_min[m] = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numEvents);
      s_buffer[index].gIncl_min[m] = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numEvents);
      s_buffer[index].gExcl_max[m] = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numEvents);
      s_buffer[index].gIncl_max[m] = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numEvents);

    }

    if (rank == 0) {
      Tau_collate_allocateFunctionBuffers(&(s_buffer[index].sExcl), &(s_buffer[index].sIncl),
					  &(s_buffer[index].sNumCalls), &(s_buffer[index].sNumSubr),
					  numEvents,
					  Tau_Global_numCounters,
					  COLLATE_OP_DERIVED);
    }

    Tau_collate_compute_statistics_MPI_with_minmaxloc(s_buffer[index].functionUnifier, s_buffer[index].globalEventMap,
				   numEvents,
				   globalNumThreads, s_buffer[index].numEventThreads,
				   &(s_buffer[index].gExcl), &(s_buffer[index].gIncl),
				   &(s_buffer[index].gExcl_min), &(s_buffer[index].gIncl_min),
				   &(s_buffer[index].gExcl_max), &(s_buffer[index].gIncl_max),
                                   &(s_buffer[index].gNumCalls), &(s_buffer[index].gNumSubr),
				   &(s_buffer[index].sExcl), &(s_buffer[index].sIncl),
                                   &(s_buffer[index].sNumCalls), &(s_buffer[index].sNumSubr), newcomm);

   /* if(rank == 0) {
      for (int m=0; m<Tau_Global_numCounters; m++)  {
        for(int n=0; n<numEvents; n++) {
          fprintf(stderr, "Counter %d: The min exclusive, max exclusive, min inclusive, max inclusive values for event %d are located on processes %d, %d, %d and %d with values %f, %f, %f, %f AND %d\n", m, n, s_buffer[index].gExcl_min[m][n].index, s_buffer[index].gExcl_max[m][n].index, s_buffer[index].gIncl_min[m][n].index, s_buffer[index].gIncl_max[m][n].index, s_buffer[index].gExcl_min[m][n].value, s_buffer[index].gExcl_max[m][n].value, s_buffer[index].gIncl_min[m][n].value, s_buffer[index].gIncl_max[m][n].value, world_rank);
        }
      }
    }*/

    /* End  interval event calculations */
    /* Start atomic statistic calculations */

    s_buffer[index].atomicUnifier = Tau_unify_getAtomicUnifier();
    numAtomicEvents = s_buffer[index].atomicUnifier->globalNumItems;

    s_buffer[index].numAtomicEventThreads =
      (int*)TAU_UTIL_MALLOC(numAtomicEvents*sizeof(int));
    s_buffer[index].globalAtomicEventMap = (int*)TAU_UTIL_MALLOC(numAtomicEvents*sizeof(int));

    // initialize all to -1
    for (int i=0; i<numAtomicEvents; i++) {
      // -1 indicates that the event did not occur for this rank
      s_buffer[index].globalAtomicEventMap[i] = -1;
    }
    for (int i=0; i<s_buffer[index].atomicUnifier->localNumItems; i++) {
      // set reverse mapping
      s_buffer[index].globalAtomicEventMap[s_buffer[index].atomicUnifier->mapping[i]] = i;
    }

    Tau_collate_get_total_threads_MPI(s_buffer[index].atomicUnifier, &globalNumThreads, &(s_buffer[index].numAtomicEventThreads),
				  numAtomicEvents, s_buffer[index].globalAtomicEventMap,true);

    Tau_collate_allocateAtomicBuffers(&(s_buffer[index].gAtomicMin), &(s_buffer[index].gAtomicMax),
				      &(s_buffer[index].gAtomicCalls), &(s_buffer[index].gAtomicMean),
				      &(s_buffer[index].gAtomicSumSqr),
				      numAtomicEvents,
				      COLLATE_OP_BASIC);
   s_buffer[index].gAtomicMin_min = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numAtomicEvents);
   s_buffer[index].gAtomicMax_max = (double_int *)TAU_UTIL_CALLOC(sizeof(double_int)*numAtomicEvents);

    if (rank == 0) {
      Tau_collate_allocateAtomicBuffers(&(s_buffer[index].sAtomicMin), &(s_buffer[index].sAtomicMax),
					&(s_buffer[index].sAtomicCalls), &(s_buffer[index].sAtomicMean),
					&(s_buffer[index].sAtomicSumSqr),
					numAtomicEvents,
					COLLATE_OP_DERIVED);
    }

    Tau_collate_compute_atomicStatistics_MPI_with_minmaxloc(s_buffer[index].atomicUnifier, s_buffer[index].globalAtomicEventMap,
					 numAtomicEvents,
					 globalNumThreads,
					 s_buffer[index].numAtomicEventThreads,
					 &(s_buffer[index].gAtomicMin), &(s_buffer[index].gAtomicMax),
					 &(s_buffer[index].gAtomicMin_min), &(s_buffer[index].gAtomicMax_max),
					 &(s_buffer[index].gAtomicCalls), &(s_buffer[index].gAtomicMean),
					 &(s_buffer[index].gAtomicSumSqr),
					 &(s_buffer[index].sAtomicMin), &(s_buffer[index].sAtomicMax),
					 &(s_buffer[index].sAtomicCalls), &(s_buffer[index].sAtomicMean),
					 &(s_buffer[index].sAtomicSumSqr), newcomm);


   /* if(rank == 0) {
      for(int i=0; i<numAtomicEvents; i++)
        fprintf(stderr, "The min and max for atomic event %d lies with processes %d and %d with values %f and %f\n", i, s_buffer[index].gAtomicMin_min[i].index, s_buffer[index].gAtomicMax_max[i].index, s_buffer[index].gAtomicMin_min[i].value, s_buffer[index].gAtomicMax_max[i].value);
    }*/
  }

  index++;
  analytics_complete = 1;

}
  return(NULL);
}

int Tau_plugin_event_trigger(Tau_plugin_event_trigger_data_t* data) {

   if(analytics_complete) {
      Tau_unify_unifyDefinitions_MPI();
      sem_post(&snapshot_mutex); //release semaphore
   }

   return 0;
}

/*This is the init function that gets invoked by the plugin mechanism inside TAU.
 * Every plugin MUST implement this function to register callbacks for various events
 * that the plugin is interested in listening to*/
extern "C" int Tau_plugin_init_func(int argc, char **argv, int id) {
  Tau_plugin_callbacks * cb = (Tau_plugin_callbacks*)malloc(sizeof(Tau_plugin_callbacks));
  TAU_UTIL_INIT_TAU_PLUGIN_CALLBACKS(cb);

  sem_init(&snapshot_mutex, 0, 0);

  cb->Trigger = Tau_plugin_event_trigger;
  cb->EndOfExecution = Tau_plugin_event_end_of_execution;

#ifdef TAU_MPI
  PMPI_Comm_dup(MPI_COMM_WORLD, &newcomm);
#endif

  init_lock();

  int ret = pthread_create(&worker_thread, NULL, &Tau_plugin_threaded_analytics, NULL);
            if (ret != 0) {
                errno = ret;
                perror("Error: pthread_create (1) fails\n");
                exit(1);
  }


  TAU_UTIL_PLUGIN_REGISTER_CALLBACKS(cb, id);

  return 0;
}

#endif
#endif
