#ifndef UUKI_BASE_STATUS_H
#define UUKI_BASE_STATUS_H

enum w_status {
    W_SUCCESS                 =  0,
    W_ERROR                   = -1,
    W_ERROR_ALLOCATION_FAILED = -2,
    W_ERROR_COPY_FAILED       = -3,
    W_ERROR_MAX_SIZE_EXCEEDED = -4,
    W_STREAM_IO_FAILED        = -5,
};

#endif // UUKI_BASE_STATUS_H
