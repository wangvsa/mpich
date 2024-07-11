
#include "ad_unify.h"
#include "ad_unify_common.h"

// internal implementation for MPI_File_sync()
void ADIOI_UNIFY_Flush(ADIO_File fd, int *error_code)
{
    int ret;
    *error_code = MPI_SUCCESS;
    ADIOI_UNIFY_fs *unifyfs_blob = fd->fs_ptr;

    unifyfs_io_request sync_reqs[2];
    sync_reqs[0].op = UNIFYFS_IOREQ_OP_SYNC_META;
    sync_reqs[0].gfid = unifyfs_blob->gfid;
    sync_reqs[0].result.error = 0;
    sync_reqs[1].op = UNIFYFS_IOREQ_OP_SYNC_DATA;
    sync_reqs[1].gfid = unifyfs_blob->gfid;
    sync_reqs[1].result.error = 0;

    ret = unifyfs_dispatch_io(unifyfs_blob->fshdl, 2, sync_reqs);
    if (ret == UNIFYFS_SUCCESS) {
        ret = unifyfs_wait_io(unifyfs_blob->fshdl, 2, sync_reqs, 2);
        if (ret == UNIFYFS_SUCCESS)
            if (sync_reqs[0].result.error != 0 || 
                sync_reqs[1].result.error != 0 ) {
                *error_code = MPIO_Err_create_code(MPI_SUCCESS,
                                                   MPIR_ERR_RECOVERABLE, __func__, __LINE__,
                                                   MPI_ERR_IO, "**io", "**io %s",
                                                   strerror(sync_reqs[0].result.error));
                return;
            }
    }

    *error_code = MPI_SUCCESS;
    return;
}
