#!/bin/bash -e

source sourceme_mpi.sh

old_method() {
    rm -f core.*
    rm -rf profile.* MULTI__* tauprofile-matmult.bp tauprofile.xml
    # launch the program with the plugin
    echo "With PAPI component plugin environment variables"
    T="$(date +%s)"
    export TAU_PLUGINS=libTAU-papi-components-plugin.so
    export TAU_PLUGINS_PATH=../../x86_64/lib/shared-papi-mpi-pthread
    mpirun -np 4 ./matmult
    A="$(($(date +%s)-T))"
    printf "Time to run tau: %02d hours %02d minutes %02d seconds.\n" "$((A/3600))" "$((A/60%60))" "$((A%60))"
    unset TAU_PLUGINS
    unset TAU_PLUGINS_PATH
}

new_method() {
    rm -f core.*
    rm -rf profile.* MULTI__* tauprofile-matmult.bp tauprofile.xml
    # launch the program with tau_exec
    echo "With PAPI component plugin tau_exec"
    T="$(date +%s)"
    mpirun -np 2 tau_exec -T papi,mpi,pthread,adios2 -monitoring ./matmult
    A="$(($(date +%s)-T))"
    printf "Time to run tau: %02d hours %02d minutes %02d seconds.\n" "$((A/3600))" "$((A/60%60))" "$((A%60))"
}

new_method

