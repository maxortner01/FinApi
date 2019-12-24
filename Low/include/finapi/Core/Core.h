#pragma once

#include <string>
#include <vector>

/*     Type Definitions     */
#define uint unsigned int
#define c_uint const unsigned int

#define CLEAN_OBJ(obj) if (obj) delete obj
#define _ALLOC(size, type) (type)std::calloc(size, 1)
#define CHAR_ALLOC(size) _ALLOC(size, char*) 