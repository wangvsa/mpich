#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define KB (1*1024)
#define MB (1024*1024)

#define DATA_BLOCK_SIZE  (1*MB)
#define DATA_NUM_BLOCK   1

static int mpi_size;
static int mpi_rank;
static MPI_File mpi_fh;

void write_data(int step) {

    long int chunk_size = DATA_NUM_BLOCK * DATA_BLOCK_SIZE;
    long int offset = step * chunk_size * mpi_size + mpi_rank * chunk_size;
    MPI_File_seek(mpi_fh, offset, MPI_SEEK_SET);

    for(int i = 0; i < DATA_NUM_BLOCK; i++) {
        void* buf = malloc(DATA_BLOCK_SIZE);
        MPI_File_write_at_all(mpi_fh, offset, buf, DATA_BLOCK_SIZE, MPI_BYTE, MPI_STATUS_IGNORE);
        offset += DATA_BLOCK_SIZE;
    }
}

static double t_barr = 0, t_sync = 0;

void sync_data(int method) {
    if (method == 0) {
        double t1, t2;
        t1 = MPI_Wtime();
        MPI_File_sync(mpi_fh);
        t2 = MPI_Wtime();
        t_sync += t2 - t1;

        t1 = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        t2 = MPI_Wtime();
        t_barr += (t2-t1);

        t1 = MPI_Wtime();
        MPI_File_sync(mpi_fh);
        t2 = MPI_Wtime();
        t_sync += t2 - t1;
    } else if (method == 1) {
        //MPI_File_flush(mpi_fh);
        MPI_Barrier(MPI_COMM_WORLD);
        //MPI_File_flush(mpi_fh);
    }
}

void read_data(int step) {

    long int chunk_size = DATA_NUM_BLOCK * DATA_BLOCK_SIZE;
    long int offset = chunk_size * mpi_size + mpi_rank * chunk_size;
    MPI_File_seek(mpi_fh, offset, MPI_SEEK_SET);

    for(int i = 0; i < DATA_NUM_BLOCK; i++) {
        void* buf = malloc(DATA_BLOCK_SIZE);
        MPI_File_read(mpi_fh, buf, DATA_BLOCK_SIZE, MPI_BYTE, MPI_STATUS_IGNORE);
    }
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    MPI_Barrier(MPI_COMM_WORLD);
    double total_time = 0, sync_time = 0;
    double t;
    total_time = MPI_Wtime();

    MPI_Info info;
    MPI_Info_create(&info);
    //MPI_Info_set(info, "romio_visibility_immediate", "false");
    //MPI_Info_set(info, "cb_nodes", "1");
    MPI_File_open(MPI_COMM_WORLD, "workfile.out", MPI_MODE_RDWR | MPI_MODE_CREATE, info, &mpi_fh);

    int steps = 10;
    for(int i = 0; i < steps; i++) {

        write_data(i);

        t = MPI_Wtime();
        sync_data(atoi(argv[1]));
        sync_time += (MPI_Wtime() - t);

        read_data(i);
    }

    MPI_File_close(&mpi_fh);
    MPI_Barrier(MPI_COMM_WORLD);
    total_time = MPI_Wtime() - total_time;

    if (mpi_rank == 0) {
        printf("total time: %.3f seconds, sync time: %.3f seconds\n", total_time, sync_time);
        printf("t_sync: %.3f seconds, t_barrier: %.3f seconds\n", t_sync, t_barr);
    }


    MPI_Finalize();
    return 0;
}
