#pragma once

#include <cassert>
#include <fstream>
#include <vector>

#define CLEAN_OBJ(obj) if (obj) delete obj

namespace finapi
{
    template<typename T>
    static void clean_list(std::vector<T*>& list)
    {
        for (int i = 0; i < list.size(); i++)
            CLEAN_OBJ(list[i]);
    }
}