#!/bin/bash
rm -rf CMakeCache.txt C* 
cmake \
-D Trilinos_ENABLE_Fortran:BOOL=ON \
      -D CMAKE_INSTALL_PREFIX="/home/rppawlo/tpls/trilinos/empire" \
      -D Trilinos_ENABLE_DEBUG=OFF \
      -D Trilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
      -D Trilinos_ENABLE_CXX11:BOOL=ON \
-D Trilinos_TPL_SYSTEM_INCLUDE_DIRS=TRUE \
      -D Panzer_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
      -D Trilinos_ENABLE_CHECKED_STL:BOOL=OFF \
      -D Trilinos_ENABLE_TEUCHOS_TIME_MONITOR:BOOL=OFF \
      -D Panzer_ENABLE_TEUCHOS_TIME_MONITOR:BOOL=ON \
      -D NOX_ENABLE_TEUCHOS_TIME_MONITOR:BOOL=ON \
      -D Trilinos_ENABLE_INSTALL_CMAKE_CONFIG_FILES:BOOL=ON \
      -D Trilinos_ENABLE_EXPORT_MAKEFILES:BOOL=OFF \
      -D Trilinos_ENABLE_ALL_PACKAGES:BOOL=OFF \
      -D Trilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF \
      -D Trilinos_ENABLE_EXAMPLES:BOOL=OFF \
      -D Trilinos_ENABLE_TESTS:BOOL=OFF \
-D Trilinos_ENABLE_FEI:BOOL=ON \
      -D EpetraExt_ENABLE_HDF5:BOOL=OFF \
      -D Teuchos_ENABLE_FLOAT:BOOL=OFF \
      -D Teuchos_ENABLE_COMPLEX:BOOL=OFF \
-D Tpetra_ENABLE_OpenMP:BOOL=ON \
-D Tpetra_INST_FLOAT:BOOL=OFF \
-D Tpetra_INST_COMPLEX_FLOAT:BOOL=OFF \
-D Tpetra_INST_COMPLEX_DOUBLE:BOOL=OFF \
-D Tpetra_INST_UNSIGNED_LONG:BOOL=OFF \
-D Tpetra_INST_INT:BOOL=OFF \
-D Trilinos_ENABLE_Intrepid:BOOL=OFF \
      -D Intrepid2_ENABLE_DEBUG_INF_CHECK:BOOL=OFF \
-D Intrepid2_ENABLE_TESTS:BOOL=ON \
-D Intrepid2_ENABLE_EXAMPLES:BOOL=ON \
-D Sacado_ENABLE_TESTS:BOOL=OFF \
-D Sacado_ENABLE_EXAMPLES:BOOL=OFF \
      -D SEACAS_ENABLE_NETCDF4_SUPPORT:BOOL=ON \
      -D SEACASExodus_ENABLE_MPI:BOOL=OFF \
      -D Trilinos_ENABLE_KokkosCore:BOOL=ON \
      -D Trilinos_ENABLE_KokkosAlgorithms:BOOL=ON \
      -D Trilinos_ENABLE_STKClassic:BOOL=OFF \
      -D Trilinos_ENABLE_STKMesh:BOOL=OFF \
      -D Trilinos_ENABLE_STKUtil:BOOL=OFF \
      -D Trilinos_ENABLE_STKTopology:BOOL=OFF \
      -D Trilinos_ENABLE_STKSearch:BOOL=OFF \
      -D Trilinos_ENABLE_STKTransfer:BOOL=OFF \
-D Trilinos_ENABLE_Zoltan2:BOOL=OFF \
-D Trilinos_ENABLE_ROL:BOOL=OFF \
      -D Trilinos_ENABLE_MueLu:BOOL=OFF \
      -D Trilinos_ENABLE_Ifpack2:BOOL=ON \
      -D Trilinos_ENABLE_Amesos2:BOOL=ON \
      -D MueLu_INST_DOUBLE_INT_LONGLONGINT=ON \
-D Amesos2_ENABLE_LAPACK:BOOL=ON \
-D Amesos2_ENABLE_KLU2:BOOL=ON \
      -D Ifpack2_ENABLE_EXPLICIT_INSTANTIATION:BOOL=OFF \
      -D Amesos2_ENABLE_EXPLICIT_INSTANTIATION:BOOL=OFF \
      -D Trilinos_ENABLE_ThreadPool:BOOL=OFF \
      -D Trilinos_ENABLE_Stokhos:BOOL=OFF \
-D Phalanx_INDEX_SIZE_TYPE:STRING="INT" \
-D Phalanx_SHOW_DEPRECATED_WARNINGS:BOOL=OFF \
-D Phalanx_ENABLE_KOKKOS_DYN_RANK_VIEW:BOOL=ON \
-D Phalanx_ENABLE_TEUCHOS_TIME_MONITOR:BOOL=ON \
      -D Trilinos_ENABLE_Panzer:BOOL=ON \
      -D Phalanx_ENABLE_TESTS:BOOL=ON \
      -D Phalanx_ENABLE_EXAMPLES:BOOL=ON \
      -D Panzer_ENABLE_FADTYPE:STRING="Sacado::Fad::DFad<RealType>" \
      -D Panzer_ENABLE_TESTS:BOOL=ON \
      -D Panzer_ENABLE_EXAMPLES:BOOL=ON \
      -D Trilinos_ENABLE_SECONDARY_TESTED_CODE:BOOL=ON \
      -D Trilinos_ENABLE_TriKota:BOOL=OFF \
      -D TPL_ENABLE_MPI:BOOL=ON \
      -D MPI_BASE_DIR:PATH="$ROGER_MPICH_INCLUDE_PATH" \
      -D TPL_ENABLE_Boost:BOOL=OFF \
      -D Boost_INCLUDE_DIRS:FILEPATH="$ROGER_BOOST_INCLUDE_PATH" \
      -D TPL_ENABLE_BoostLib:BOOL=OFF \
      -D BoostLib_INCLUDE_DIRS:FILEPATH="$ROGER_BOOST_INCLUDE_PATH" \
      -D BoostLib_LIBRARY_DIRS:FILEPATH="$ROGER_BOOST_LIBRARY_PATH" \
      -D TPL_ENABLE_HDF5:BOOL=ON \
      -D HDF5_INCLUDE_DIRS:FILEPATH="$ROGER_HDF5_INCLUDE_PATH" \
      -D HDF5_LIBRARY_DIRS:FILEPATH="$ROGER_HDF5_LIBRARY_PATH" \
      -D TPL_ENABLE_Netcdf:BOOL=ON \
      -D Netcdf_INCLUDE_DIRS:FILEPATH="$ROGER_NETCDF_INCLUDE_PATH" \
      -D Netcdf_LIBRARY_DIRS:FILEPATH="$ROGER_NETCDF_LIBRARY_PATH" \
-DTPL_ENABLE_Matio=OFF \
-DTPL_ENABLE_X11=OFF \
      -D TPL_BLAS_LIBRARIES:PATH="$ROGER_BLAS_LIBRARY" \
      -D TPL_LAPACK_LIBRARIES:PATH="$ROGER_LAPACK_LIBRARY" \
      -D CMAKE_CXX_COMPILER:FILEPATH="mpicxx" \
      -D CMAKE_C_COMPILER:FILEPATH="mpicc" \
      -D CMAKE_Fortran_COMPILER:FILEPATH="mpifort" \
      -D CMAKE_CXX_FLAGS:STRING="-g -pipe -Wshadow -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -fPIC" \
      -D CMAKE_C_FLAGS:STRING="-g" \
      -D CMAKE_Fortran_FLAGS:STRING="-g" \
      -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
      -D Trilinos_VERBOSE_CONFIGURE:BOOL=OFF \
      -D CMAKE_SKIP_RULE_DEPENDENCY=ON \
      -D CMAKE_BUILD_TYPE:STRING=DEBUG \
      -D Trilinos_ENABLE_OpenMP:BOOL=ON \
      -D Kokkos_ENABLE_OpenMP:BOOL=ON \
      -D Kokkos_ENABLE_Serial:BOOL=ON \
       ../Trilinos

##-D MPI_EXEC_POST_NUMPROCS_FLAGS="-bind-to;core;-map-by;core" \

##      -D TPL_ENABLE_HWLOC:BOOL=OFF \
##      -D HWLOC_INCLUDE_DIRS:FILEPATH="$ROGER_HWLOC_INCLUDE_PATH" \
##      -D HWLOC_LIBRARY_DIRS:FILEPATH="$ROGER_HWLOC_LIBRARY_PATH" \

###-D MPI_EXEC_POST_NUMPROCS_FLAGS="-bind-to;socket;-map-by;socket" \
###      -D CMAKE_EXE_LINKER_FLAGS:STRING="-Wl,-fuse-ld=gold -L/usr/lib -lgfortran" \
###      -D CMAKE_CXX_FLAGS:STRING="-DKOKKOS_USING_EXPERIMENTAL_VIEW -g -Wno-unused-local-typedefs -Wno-sign-compare" \

##      -D CMAKE_Fortran_COMPILER:FILEPATH="mpifort" \
##      -D CMAKE_Fortran_FLAGS:STRING="-g" \
##      -D CMAKE_EXE_LINKER_FLAGS:STRING="-Wl,-fuse-ld=gold -L/usr/lib -lgfortran" \
##      -D CMAKE_CXX_FLAGS:STRING="-g -Wno-unused-local-typedefs -Wno-sign-compare -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_NO_AUTO_PTR -D_GLIBCXX_USE_DEPRECATED=0" \
