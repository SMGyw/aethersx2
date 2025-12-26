// Hard-fix for iOS/ARM Build
#ifdef __APPLE__
    #define FMT_USE_SSE2 0
    #define FMT_USE_SSE3 0
    #define FMT_USE_SSE4_1 0
    #undef __SSE__
    #undef __SSE2__
#endif

#include "fmt/format.h"
#include "fmt/os.h"

// Your file continues here or we just empty the problematic parts
// This ensures the compiler sees ZERO SSE code in this file.