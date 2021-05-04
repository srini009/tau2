/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/tau	           **
*****************************************************************************
**    Copyright 1997-2006                                                  **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/****************************************************************************
**	File 		: LikwidLayer.h                                      **
**	Description 	: TAU Profiling Package			           **
**	Contact		: tau-team@cs.uoregon.edu 		 	   **
**	Documentation	: See http://www.cs.uoregon.edu/research/tau       **
****************************************************************************/

#ifndef _LIKWID_LAYER_H_
#define _LIKWID_LAYER_H_

#ifdef TAU_LIKWID
struct ThreadValue {
  int ThreadID; 
  long long *CounterValues;
};


class LikwidLayer {
public:
  static int initializeLikwidLayer();//bool lock = true);
  static long long getSingleCounter(int tid);
  static long long *getAllCounters(int tid, int *numValues);
  static int addEvents(const char *name); 
  static ThreadValue *ThreadList[TAU_MAX_THREADS];
  static int numCounters;
  static int counterList[TAU_MAX_COUNTERS];
  static int* cpus;
  static int gid;
  static int err;
  static bool likwidInitialized;
private:
  static int initializeSingleCounter();
  static int initializeThread(int tid);
  static double scalingFactor;
};

#endif /* TAU_LIKWID */
#endif /* _LIKWID_LAYER_H_ */
