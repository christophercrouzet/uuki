#ifndef UUKI_BASE_STATUS_H
#define UUKI_BASE_STATUS_H

enum w_status {
    W_SUCCESS                     =   0,
    W_ERROR                       =  -1,
    W_ERROR_ALLOC_FAILED          =  -2,
    W_ERROR_BUF_FULL              =  -3,
    W_ERROR_FILE_CLOSE_FAILED     =  -4,
    W_ERROR_FILE_OPEN_FAILED      =  -5,
    W_ERROR_FILE_READ_FAILED      =  -6,
    W_ERROR_IO_FAILED             =  -7,
    W_ERROR_LOCK_FAILED           =  -8,
    W_ERROR_MAX_SIZE_EXCEEDED     =  -9,
    W_ERROR_NOT_FOUND             = -10,
    W_ERROR_STR_FORMATTING_FAILED = -11,
    W_ERROR_UNLOCK_FAILED         = -12,
};

#endif // UUKI_BASE_STATUS_H
