#pragma once

#include <string>
#include <vector>

#pragma region VERSIONING

#define FIN_API_VERSION_MAJOR  "0"
#define FIN_API_VERSION_MINOR  "0"
#define FIN_API_VERSION_FIX    "1"
#define FIN_API_VERSION_BRANCH "maxBranch"

#define FIN_API_VERSION_STR\
    FIN_API_VERSION_MAJOR "." FIN_API_VERSION_MINOR "." FIN_API_VERSION_FIX\

#ifdef FIN_API_VERSION_BRANCH
#   undef  FIN_API_VERSION_STR
#   define FIN_API_VERSION_STR \
        FIN_API_VERSION_MAJOR "." FIN_API_VERSION_MINOR "." FIN_API_VERSION_FIX "." FIN_API_VERSION_BRANCH
#endif

#pragma endregion

#define assert(expr) if (!(expr)) {\
    printf("Assertion failed in file %s on expression\n\n%s\n\non line %d\n", __FILE__, #expr, __LINE__);\
    exit(1); }

/*     Type Definitions     */
#define uint unsigned int
#define c_uint const uint

#define CLEAN_OBJ(obj) if (obj) delete obj
#define _ALLOC(size, type) (type)std::calloc(size, 1)
#define CHAR_ALLOC(size) _ALLOC(size, char*) 