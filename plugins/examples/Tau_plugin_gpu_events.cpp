/************************************************************************************************
 * *   Plugin Testing
 * *   Tests basic functionality of a plugin for function registration event
 * *
 * *********************************************************************************************/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <Profile/Profiler.h>
#include <Profile/TauSampling.h>
#include <Profile/TauMetrics.h>
#include <Profile/TauAPI.h>
#include <Profile/TauPlugin.h>

int Tau_plugin_gpu_event_init(Tau_plugin_event_gpu_init_data_t* data) {
    fprintf(stderr, "TAU Plugin Event: GPU Profiling Initialized.\n");
    return 0;
}

int Tau_plugin_gpu_event_finalize(Tau_plugin_event_gpu_finalize_data_t* data) {
    fprintf(stderr, "TAU Plugin Event: GPU Profiling Finalized.\n");
    return 0;
}

int Tau_plugin_gpu_event_kernel_exec(Tau_plugin_event_gpu_kernel_exec_data_t* data) {
    fprintf(stderr, "TAU Plugin Event: GPU Kernel time: %lu\n", data->time);
    return 0;
}

int Tau_plugin_gpu_event_memcpy(Tau_plugin_event_gpu_memcpy_data_t* data) {
    fprintf(stderr, "TAU Plugin Event: GPU Memcpy time, data, kind: %lu, %lu, %d\n", data->time, data->size, data->kind);
    return 0;
}

/*This is the init function that gets invoked by the plugin mechanism inside TAU.
 * Every plugin MUST implement this function to register callbacks for various events 
 * that the plugin is interested in listening to*/
extern "C" int Tau_plugin_init_func(int argc, char **argv, int id) {
    Tau_plugin_callbacks_t cb;
    TAU_VERBOSE("TAU PLUGIN GPU EVENT Init\n"); fflush(stdout);
    /* Create the callback object */
    TAU_UTIL_INIT_TAU_PLUGIN_CALLBACKS(&cb);
    /* Required event support */
    cb.GpuInit = Tau_plugin_gpu_event_init;
    cb.GpuFinalize = Tau_plugin_gpu_event_finalize;
    cb.GpuKernelExec = Tau_plugin_gpu_event_kernel_exec;
    cb.GpuMemcpy = Tau_plugin_gpu_event_memcpy;

    /* Register the callback object */
    TAU_UTIL_PLUGIN_REGISTER_CALLBACKS(&cb, id);
    return 0;
}

