
#include "ad_unify.h"
#include "ad_unify_common.h"

void ADIOI_UNIFY_ConsistencyFlush(ADIO_File fd, int *error_code)
{
    int ret;
    *error_code = MPI_SUCCESS;
    ADIOI_UNIFY_fs *unifyfs_blob = fd->fs_ptr;

    unifyfs_io_request sync_req;
    sync_req.op = UNIFYFS_IOREQ_OP_SYNC_META;
    sync_req.gfid = unifyfs_blob->gfid;
    sync_req.result.error = 0;

    ret = unifyfs_dispatch_io(unifyfs_blob->fshdl, 1, &sync_req);
    if (ret == UNIFYFS_SUCCESS) {
        ret = unifyfs_wait_io(unifyfs_blob->fshdl, 1, &sync_req, 1);
        if (ret == UNIFYFS_SUCCESS)
            if (sync_req.result.error != 0) {
                *error_code = MPIO_Err_create_code(MPI_SUCCESS,
                                                   MPIR_ERR_RECOVERABLE, __func__, __LINE__,
                                                   MPI_ERR_IO, "**io", "**io %s",
                                                   strerror(sync_req.result.error));
                return;
            }
    }

    *error_code = MPI_SUCCESS;
    return;
}
