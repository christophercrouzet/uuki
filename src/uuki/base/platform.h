#ifndef UUKI_BASE_PLATFORM_H
#define UUKI_BASE_PLATFORM_H

#if defined(_WIN32)
    #define W_PLATFORM_WINDOWS
#elif defined(__unix__) || defined(__APPLE__)
    #define W_PLATFORM_UNIX
    #if defined(__APPLE__)
        #define W_PLATFORM_DARWIN
    #elif defined(__linux__)
        #define W_PLATFORM_LINUX
    #endif
#endif

#endif // UUKI_BASE_PLATFORM_H
