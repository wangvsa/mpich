/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#include "adio.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

void ADIOI_GEN_ConsistencyFetch(ADIO_File fd, int *error_code)
{
    int err;
    static char myname[] = "ADIOI_GEN_CONSISTENCYFETCH";

    *error_code = MPI_SUCCESS;

    /* CHEN:
     * this is a no-op on POSIX file systems,
     * since POSIX write is already strongly
     * consistent.
     */
}
