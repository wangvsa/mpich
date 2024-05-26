#!/usr/bin/bash
#flux: -N2 -t 02m
#flux: --job-name="newio"

source ~/.bash_profile
module restore
module list

#[Performance Suggestion]: Application has requested for multi-thread capability.
#If allocating memory from different pthreads/OpenMP threads, please consider
#setting MV2_USE_ALIGNED_ALLOC=1 for improved performance.
export MV2_USE_ALIGNED_ALLOC=1


ppn=16
export nnodes=`flux resource list --states=free -no {nnodes}`
export nprocs=$((ppn * nnodes))

echo "nodes:" $nnodes "procs:" $nprocs


mpirun=/p/lustre2/wang116/corona/sources/mpich-chen-20240522/install-romio-flush/bin/mpirun

export ROMIO_PRINT_HINTS=1
$mpirun -ppn $ppn -n $nprocs ./large-io 0
$mpirun -ppn $ppn -n $nprocs ./large-io 0
$mpirun -ppn $ppn -n $nprocs ./large-io 0
$mpirun -ppn $ppn -n $nprocs ./large-io 0

#$mpirun -ppn $ppn -n $nprocs ./large-io 1
#export ROMIO_PRINT_HINTS=0

echo "use sync-barrier-sync"
for i in {1..1}
do
    flux run -N $nnodes -n $nprocs ./large-io 0
done

#echo "use flush-barrier-flush"
#for i in {1..1}
#do
#    flux run -N $nnodes -n $nprocs ./large-io 1
#done
