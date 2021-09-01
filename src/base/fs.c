#include <uuki/base/fs.h>

#include <uuki/base/platform.h>

#if W_OS(UNIX)
    #include "fs_unix.c"
#elif W_OS(WINDOWS)
    #include "fs_windows.c"
#else
    #error "not implemented"
#endif
