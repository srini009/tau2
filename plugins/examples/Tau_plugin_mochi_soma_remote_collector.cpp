/***************************************************************************
 * *   Plugin Testing
 * *   This plugin will provide iterative output of TAU profile data to an
 * *   MOCHI service.
 * *
 * *************************************************************************/

/* Have the TAU configure set -DTAU_MOCHI when you want this plugin */
#if defined(TAU_MOCHI)

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <mutex>

#include <Profile/Profiler.h>
#include <Profile/TauSampling.h>
#include <Profile/TauMetrics.h>
#include <Profile/TauAPI.h>
#include <Profile/TauPlugin.h>
#include <Profile/TauMetaData.h>

#ifdef TAU_MPI
#include<mpi.h>
#endif

#include <margo.h>
#include <soma/Client.hpp>
#include <conduit/conduit.hpp>

/* So that we can keep track of how long this plugin takes */
#include "Tau_scoped_timer.h"

/* Globals -- Yikes, I know. */
static bool enabled{false};
static bool initialized{false};
static bool opened{false};
static bool done{false};
static int my_rank = 0;
static int size = 0;
static std::string g_address_file;
static std::string g_address;
static std::string g_protocol = "na+sm";
static std::string g_node;
static unsigned    g_provider_id;
static std::string g_log_level = "info";
int reduction_frequency = 1;
int num_server = 1;
static tl::engine *engine;
soma::Client *client;
soma::CollectorHandle soma_client;
int server_instance_id = 0;

#define RESERVE(container, size) container.reserve(size)
#define UPDATE_KEY(container, val) container.push_back(val)
#define UPDATE_VAL(container, val) container.push_back(val)

pthread_mutex_t _my_mutex; // for initialization, termination

static void parse_command_line();

/* Helper function to read and parse input args */
static std::string read_nth_line(const std::string& filename, int n)
{
   std::ifstream in(filename.c_str());

   std::string s;
   //for performance
   s.reserve(200);

   //skip N lines
   for(int i = 0; i < n; ++i)
       std::getline(in, s);

   std::getline(in,s);
   return s;
}

void parse_command_line() {

    char *addr_file_name = getenv("SOMA_SERVER_ADDR_FILE");
    char *node_file_name = getenv("SOMA_NODE_ADDR_FILE");
    num_server = std::stoi(std::string(getenv("SOMA_NUM_SERVERS_PER_INSTANCE")));
    server_instance_id = std::stoi(std::string(getenv("SOMA_SERVER_INSTANCE_ID")));
    int my_server_offset = num_server % my_rank;
    std::string l = read_nth_line(g_address_file, server_instance_id*num_server + my_server_offset);
    pos = l.find(delimiter);
    std::string server_rank_str = l.substr(0, pos);
    std::stringstream s_(server_rank_str);
    int server_rank;
    s_ >> server_rank;
    l.erase(0, pos + delimiter.length());
    g_address = l;
    g_provider_id = 0;
    g_node = read_nth_line(std::string(node_file_name), server_instance_id*num_server + my_server_offset);
    g_protocol = g_address.substr(0, g_address.find(":"));

}

/* These are useful if you want to tell Mochi what the name of the program
 * is or if you want to get the path to the executable. */

char *_program_path()
{
#if defined(__APPLE__)
    return NULL;
#else
    char path[PATH_MAX] = {0};
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
        return(NULL);
    }
    std::string tmp(path);
    size_t i = tmp.rfind('/', tmp.length());
    if (i != string::npos) {
        //sprintf(path, "%s", tmp.substr(i+1, tmp.length() - i).c_str());
        path[i] = '\0';
    }
    return strdup(path);
#endif
}

char *_program_name()
{
#if defined(__APPLE__)
    return NULL;
#else
    char path[PATH_MAX] = {0};
    if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
        return(NULL);
    }
    std::string tmp(path);
    size_t i = tmp.rfind('/', tmp.length());
    if (i != string::npos) {
        sprintf(path, "%s", tmp.substr(i+1, tmp.length() - i).c_str());
    }
    return strdup(path);
#endif
}

void Tau_dump_mochi_metadata() {
    tau::plugins::ScopedTimer(__func__);
    int tid = RtsLayer::myThread();
    int nodeid = TAU_PROFILE_GET_NODE();
    for (MetaDataRepo::iterator it = Tau_metadata_getMetaData(tid).begin();
         it != Tau_metadata_getMetaData(tid).end(); it++) {
        std::stringstream ss;
        /* Build a metadata key */
        ss << "TAU:" << nodeid << ":" << tid << ":MetaData:" << it->first.name;
        switch(it->second->type) {
            case TAU_METADATA_TYPE_STRING:
                /* do something with  it->second->data.cval */
                break;
            case TAU_METADATA_TYPE_INTEGER:
                /* do something with  it->second->data.ival */
                break;
            case TAU_METADATA_TYPE_DOUBLE:
                /* do something with  it->second->data.dval */
                break;
            case TAU_METADATA_TYPE_TRUE:
                /* do something with  "true" */
                break;
            case TAU_METADATA_TYPE_FALSE:
                /* do something with  "false" */
                break;
            case TAU_METADATA_TYPE_NULL:
                /* do something with  "null" */
                break;
            default:
                break;
        }
    }
}

void Tau_plugin_mochi_init_mochi(void) {
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);

    /* Grab my server instance address and other deets */
    parse_command_line();

    // Initialize the thallium server
    engine = new tl::engine(g_protocol, THALLIUM_CLIENT_MODE);

    // Initialize a Client
    client = new soma::Client(*engine);

    // Create a handle from provider 0
    collector = (*client).makeCollectorHandle(g_address, g_provider_id,
                    soma::UUID::from_string(g_collector.c_str()));

    initialized = true;
}

void Tau_plugin_mochi_open_file(void) {
    /* open mochi connection */

    int ret = 0;
    if (ret == 0) { opened = true;}
    assert(ret == 0);
}

/* Convenience function if you want to strip the source info */
void shorten_timer_name(std::string& name) {
    std::size_t index = name.find(" [{");
    if (index != std::string::npos) {
        name = name.substr(0,index);
    }
}

/* Iterates over the FunctionInfo DB and EventDB to compile a list of 
 * per-thread metrics and counters. */
void Tau_plugin_mochi_write_variables() {
    // Send a dummy Conduit node for "pipe" cleaning
    Node n;
    n["test"] = "test_value" + " for rank: " + rank;
    collector.soma_publish(n);
}

int Tau_plugin_mochi_dump(Tau_plugin_event_dump_data_t* data) {
    if (!enabled) return 0;
    TAU_VERBOSE("TAU PLUGIN MOCHI: dump\n");
    tau::plugins::ScopedTimer(__func__);

    if (!initialized) {
        Tau_plugin_mochi_init_mochi();
    }

    Tau_global_incr_insideTAU();
    // get the most up-to-date profile information
    TauProfiler_updateAllIntermediateStatistics();
    Tau_global_decr_insideTAU();

    if (!opened) {
       Tau_plugin_mochi_open_file();
    }

    if (opened) {
        Tau_plugin_mochi_write_variables();
    }

    return 0;
}

/* This is a weird event, not sure what for */
int Tau_plugin_finalize(Tau_plugin_event_function_finalize_data_t* data) {
    return 0;
}

/* This happens from MPI_Finalize, before MPI is torn down. */
int Tau_plugin_mochi_pre_end_of_execution(Tau_plugin_event_pre_end_of_execution_data_t* data) {
    if (!enabled) return 0;
    TAU_VERBOSE("TAU PLUGIN MOCHI Pre-Finalize\n"); fflush(stdout);
#ifdef TAU_MPI
    Tau_plugin_event_dump_data_t dummy;
    dummy.tid = 0;
    /* write final data */
    Tau_plugin_mochi_dump(&dummy);
    if (opened) {
        /* close mochi if it has to close before MPI_Finalize */
        opened = false;
    }
    enabled = false;
#endif
    margo_finalize(mid);
    return 0;
}

/* This happens after MPI_Init, and after all TAU metadata variables have been
 * read */
int Tau_plugin_mochi_post_init(Tau_plugin_event_post_init_data_t* data) {
    if (!enabled) return 0;
    Tau_plugin_mochi_init_mochi();
    Tau_plugin_mochi_open_file();
    return 0;
}

/* This happens from Profiler.cpp, when data is written out. */
int Tau_plugin_mochi_end_of_execution(Tau_plugin_event_end_of_execution_data_t* data) {
    if (!enabled || data->tid != 0) return 0;
    TAU_VERBOSE("TAU PLUGIN MOCHI Finalize\n"); fflush(stdout);
#ifndef TAU_MPI
    if (opened) {
        Tau_plugin_event_dump_data_t dummy;
        dummy.tid = 0;
        /* write final data */
        Tau_plugin_mochi_dump(&dummy);
        bpWriter.Close();
        opened = false;
    }
    enabled = false;
#endif
    margo_finalize(mid);
    return 0;
}

/* Function to remove all spaces from a given string */
void removeSpaces(char *str) {
     // To keep track of non-space character count
     int count = 0;
     for (int i = 0; str[i]; i++)
         if (str[i] != ' ') str[count++] = str[i];
             str[count] = '\0';
}

/*This is the init function that gets invoked by the plugin mechanism inside TAU.
 * Every plugin MUST implement this function to register callbacks for various events
 * that the plugin is interested in listening to*/
extern "C" int Tau_plugin_init_func(int argc, char **argv, int id) {
    Tau_plugin_callbacks_t cb;
    TAU_VERBOSE("TAU PLUGIN MOCHI Init\n"); fflush(stdout);
    /* Create the callback object */
    TAU_UTIL_INIT_TAU_PLUGIN_CALLBACKS(&cb);
    /* Required event support */
    cb.Dump = Tau_plugin_mochi_dump;
    cb.PostInit = Tau_plugin_mochi_post_init;
    cb.PreEndOfExecution = Tau_plugin_mochi_pre_end_of_execution;
    cb.EndOfExecution = Tau_plugin_mochi_end_of_execution;

    /* Register the callback object */
    TAU_UTIL_PLUGIN_REGISTER_CALLBACKS(&cb, id);
    enabled = true;
    char * red_freq_str = getenv("TAU_SOMA_MONITORING_FREQUENCY");
    if(red_freq_str != NULL) {
	    reduction_frequency = atoi(red_freq_str);
	    fprintf(stderr, "Reduction frequency is: %d\n", reduction_frequency);
    } else {
	    reduction_frequency = 1;
	    fprintf(stderr, "Reduction frequency is: %d\n", reduction_frequency);
    }
    return 0;
}


#endif // TAU_MOCHI
