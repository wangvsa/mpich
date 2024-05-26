#!/usr/bin/bash


mpicc=/p/lustre2/wang116/corona/sources/mpich-chen-20240522/install-romio-flush/bin/mpicc

$mpicc ./hdf5-meta-test.c
$mpicc ./large-io.c -o ./large-io
