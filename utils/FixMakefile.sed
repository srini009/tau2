s/\(.*\)#ENDIF#\(.*\)/\2\1#ENDIF#/g
s/^CONFIG_ARCH=\(.*\)/CONFIG_ARCH=x86_64/g
s/^TAU_ARCH=\(.*\)/TAU_ARCH=x86_64/g
s@^CONFIG_CC=\(.*\)@CONFIG_CC=gcc@g
s@^CONFIG_CXX=\(.*\)@CONFIG_CXX=g++@g
s@^USER_OPT=\(.*\)@USER_OPT=-O2 -g@g
s@^EXTRADIR=\(.*\)@EXTRADIR=@g
s@^EXTRADIRCXX=\(.*\)@EXTRADIRCXX=@g
s@^TAUEXTRASHLIBOPTS=\(.*\)@TAUEXTRASHLIBOPTS=@g
s;^TAU_PREFIX_INSTALL_DIR=\(.*\);TAU_PREFIX_INSTALL_DIR=/home/sramesh/AUTOPERF/TAU_INSTALL;g
s@^CONF_ENV_FILE=\(.*\)@CONF_ENV_FILE=/home/sramesh/AUTOPERF/tau2/.configure_env/4dc79288636e527592de3fef40f74ee7@g
s@^TAUROOT=\(.*\)@TAUROOT=/home/sramesh/AUTOPERF/tau2@g
s/#ARCH_WIDTH_64#\(.*\)/\1#ARCH_WIDTH_64#/g
s/#PROFILE#\(.*\)/\1#PROFILE#/g
s@^TAU_MPI_LIB=\(.*\)@TAU_MPI_LIB=-L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi@g
s@^TAU_MPI_FLIB=\(.*\)@TAU_MPI_FLIB=-L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh -lmpi@g
s@^TAU_MPI_INC=\(.*\)@TAU_MPI_INC=-I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi@g
s@^FULL_CXX=.*@FULL_CXX=g++@g
s@^FULL_CC=.*@FULL_CC=gcc@g
s,^TAUGFORTRANLIBDIR=.*$,TAUGFORTRANLIBDIR=/usr/lib/gcc/x86_64-linux-gnu/7/,g
s/#GNU_GFORTRAN#\(.*\)/\1#GNU_GFORTRAN#/g
s@^FULL_CXX=.*@FULL_CXX=g++@g
s@^FULL_CC=.*@FULL_CC=gcc@g
s/#GNU46PLUS#\(.*\)/\1#GNU46PLUS#/g
s/#GNU#\(.*\)/\1#GNU#/g
s/#COMPINST_GNU#\(.*\)/\1#COMPINST_GNU#/g
s,^TAUGCCLIBDIR=.*$,TAUGCCLIBDIR=/usr/lib/gcc/x86_64-linux-gnu/7/,g
s,^TAUGCCSTDCXXLIBDIR=.*$,TAUGCCSTDCXXLIBDIR=/usr/lib/gcc/x86_64-linux-gnu/7/,g
s/#LD_AUDITOR_AVAILABLE#\(.*\)/\1#LD_AUDITOR_AVAILABLE#/g
s/#TAU_USELM#\(.*\)/\1#TAU_USELM#/g
s/#LINUXTIMERS#\(.*\)/\1#LINUXTIMERS#/g
s/#MPI#\(.*\)/\1#MPI#/g
s/#MPI_THREADED#\(.*\)/\1#MPI_THREADED#/g
s/#MPICH3_CONST#\(.*\)/\1#MPICH3_CONST#/g
s/#MPI2#\(.*\)/\1#MPI2#/g
s/#MPIGREQUEST#\(.*\)/\1#MPIGREQUEST#/g
s/#MPIDATAREP#\(.*\)/\1#MPIDATAREP#/g
s/#MPIERRHANDLER#\(.*\)/\1#MPIERRHANDLER#/g
s/#MPICONSTCHAR#\(.*\)/\1#MPICONSTCHAR#/g
s/#MPIATTR#\(.*\)/\1#MPIATTR#/g
s/#MPIFILE#\(.*\)/\1#MPIFILE#/g
s/#MPITYPEEX#\(.*\)/\1#MPITYPEEX#/g
s/#MPIADDERROR#\(.*\)/\1#MPIADDERROR#/g
s@^TAU_MPI_LIB=\(.*\)@TAU_MPI_LIB=-L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -Wl,-rpath,$(TAU_MPILIB_DIR)@g
s@^TAU_MPI_FLIB=\(.*\)@TAU_MPI_FLIB=-L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh -lmpi -L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -Wl,-rpath,$(TAU_MPILIB_DIR)@g
s@^TAU_MPILIB_DIR=\(.*\)@TAU_MPILIB_DIR=/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib@g
s@^TAU_MPILIB_DIRLIB=\(.*\)@TAU_MPILIB_DIRLIB=-L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib@g
s@^TAU_MPI_NOWRAP_LIB=\(.*\)@TAU_MPI_NOWRAP_LIB= -L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -Wl,-rpath,$(TAU_MPILIB_DIR)@g
s@^TAU_MPI_NOWRAP_FLIB=\(.*\)@TAU_MPI_NOWRAP_FLIB= -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/include -pthread -I/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,-rpath -Wl,/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -Wl,--enable-new-dtags -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh -lmpi -L/home/sramesh/AUTOPERF/TAU_INSTALL/x86_64/lib -lTauMpi$(TAU_CONFIG) -L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib -lmpi -Wl,-rpath,$(TAU_MPILIB_DIR)@g
s@^TAU_MPILIB_DIR=\(.*\)@TAU_MPILIB_DIR=/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib@g
s@^TAU_MPILIB_DIRLIB=\(.*\)@TAU_MPILIB_DIRLIB=-L/lus/theta-fs0/software/thetagpu/openmpi-4.0.5/lib@g
s/#TAU_STRSIGNAL_OK#\(.*\)/\1#TAU_STRSIGNAL_OK#/g
s/#TAU_LARGEFILE#\(.*\)/\1#TAU_LARGEFILE#/g
s/#TAU_WEAK_SUPPORTED#\(.*\)/\1#TAU_WEAK_SUPPORTED#/g
s,^BFDINCLUDE=.*$,BFDINCLUDE=,g
s,^BFDLINK=.*$,BFDLINK=,g
s,^BFDLIBS=.*$,BFDLIBS=-lbfd -liberty -lz,g
s/#TAU_SS_ALLOC_SUPPORT#\(.*\)/\1#TAU_SS_ALLOC_SUPPORT#/g
s/#TAU_LINKS_RT#\(.*\)/\1#TAU_LINKS_RT#/g
s/#TAU_TR1_HASH_MAP#\(.*\)/\1#TAU_TR1_HASH_MAP#/g
s/#TAU_PTHREAD_WRAP#\(.*\)/\1#TAU_PTHREAD_WRAP#/g
s/#TLS_AVAILABLE#\(.*\)/\1#TLS_AVAILABLE#/g
s/#PTHREAD_AVAILABLE#\(.*\)/\1#PTHREAD_AVAILABLE#/g
s,^PTDIR=.*$,PTDIR=,g
s/#CUDA#\(.*\)/\1#CUDA#/g
s/#TAU_USE_GPU#\(.*\)/\1#TAU_USE_GPU#/g
s@^TAU_CUDA_INC=\(.*\)@TAU_CUDA_INC=/lus/theta-fs0/software/thetagpu/hpc-sdk/Linux_x86_64/20.9/cuda/11.0//include@g
s/#OPENCL#\(.*\)/\1#OPENCL#/g
s/#TAU_USE_GPU#\(.*\)/\1#TAU_USE_GPU#/g
s@^TAU_OPENCL_INC=\(.*\)@TAU_OPENCL_INC=/lus/theta-fs0/software/thetagpu/hpc-sdk/Linux_x86_64/20.9/cuda/11.0//include@g
s/#CUPTI#\(.*\)/\1#CUPTI#/g
s@^TAU_CUPTI_INC=\(.*\)@TAU_CUPTI_INC=/lus/theta-fs0/software/thetagpu/hpc-sdk/Linux_x86_64/20.9/cuda/11.0/extras/CUPTI/include@g
s/#CUPTI_ACTIVITY#\(.*\)/\1#CUPTI_ACTIVITY#/g
s/#EBS_HAS_RT#\(.*\)/\1#EBS_HAS_RT#/g
s,^EBS_CLOCK_RES=.*$,EBS_CLOCK_RES=1,g
s@^EXTRA_LINKER_ARGS=.*$@EXTRA_LINKER_ARGS= -Wl,--export-dynamic@g
s@^TAU_CONFIG=\(.*\)@TAU_CONFIG=-mpi-pthread-cupti@g
