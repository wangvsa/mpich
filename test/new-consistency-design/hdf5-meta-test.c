#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define KB (1*1024)
#define MB (1024*1024)

#define METADATA_BLOCK_SIZE  (1*KB)
#define METADATA_NUM_BLOCK   100

static int mpi_size;
static int mpi_rank;
static MPI_File mpi_fh;

void write_metadata() {
    if (mpi_rank == 0) {
        MPI_File_seek(mpi_fh, 0, MPI_SEEK_SET);
        for(int i = 0; i < METADATA_NUM_BLOCK; i++) {
            void* buf = malloc(METADATA_BLOCK_SIZE);
            MPI_File_write(mpi_fh, buf, METADATA_BLOCK_SIZE, MPI_BYTE, MPI_STATUS_IGNORE);
        }
    }
}

void sync_metadata(int method) {
    if (method == 0) {
        MPI_File_sync(mpi_fh);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_File_sync(mpi_fh);
    } else if (method == 1) {
        MPI_File_flush(mpi_fh);
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_File_flush(mpi_fh);
    }
}

void read_metadata() {

    MPI_File_seek(mpi_fh, 0, MPI_SEEK_SET);

    for(int i = 0; i < METADATA_NUM_BLOCK; i++) {
        void* buf = malloc(METADATA_BLOCK_SIZE);
        MPI_File_read(mpi_fh, buf, METADATA_BLOCK_SIZE, MPI_BYTE, MPI_STATUS_IGNORE);
    }
}

void write_data() {
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    MPI_Info info = MPI_INFO_NULL;
    MPI_File_open(MPI_COMM_WORLD, "workfile.out", MPI_MODE_RDWR | MPI_MODE_CREATE, info, &mpi_fh);

    MPI_Barrier(MPI_COMM_WORLD);
    double t;
    t = MPI_Wtime();

    int steps = 100;
    for(int i = 0; i < steps; i++) {

        write_data();

        write_metadata();

        sync_metadata(atoi(argv[1]));

        read_metadata();
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t = MPI_Wtime() - t;
    if (mpi_rank == 0)
        printf("time: %.3f seconds\n", t);


    MPI_File_close(&mpi_fh);

    MPI_Finalize();
    return 0;
}
