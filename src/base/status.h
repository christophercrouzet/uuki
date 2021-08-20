#ifndef UUKI_BASE_STATUS_H
#define UUKI_BASE_STATUS_H

enum w_status {
    W_SUCCESS                     =  0,
    W_ERROR                       = -1,
    W_ERROR_ALLOC_FAILED          = -2,
    W_ERROR_BUF_SIZE_EXCEEDED     = -3,
    W_ERROR_IO_FAILED             = -4,
    W_ERROR_LOCK_FAILED           = -5,
    W_ERROR_MAX_SIZE_EXCEEDED     = -6,
    W_ERROR_NOT_FOUND             = -7,
    W_ERROR_STR_FORMATTING_FAILED = -8,
    W_ERROR_STREAM_IO_FAILED      = -9,
};

#endif // UUKI_BASE_STATUS_H
