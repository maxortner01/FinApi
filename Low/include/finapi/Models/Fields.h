#pragma once

#include "../Core/Core.h"

namespace finapi
{
namespace models
{
    template<typename T>
    struct Field
    {
        uint size = sizeof(T);
        T value;

        void set_size(c_uint s)
        {
            size = s;
        }

        T* ptr()
        {
            return &value;
        }

        const T& operator ()() const
        {
            return value;
        }
    };

    template<typename _Model>
    struct FinFields
    {
        static c_uint            count;
        static const std::string fields[];
    };

    template<typename T, typename _Model>
    const T& get_field(const _Model& model, c_uint& index)
    {
        const char* ptr = (const char*)&model;
        for (int i = 0; i < index; i++)
        {
            c_uint size = (c_uint)*(c_uint*)ptr;
            ptr += size + sizeof(uint);
        }
        ptr += sizeof(uint);
        
        const T& r = (const T&)*(T*&)ptr;
        return r;
    }

    template<typename T, typename _Model>
    const T& get_field(const _Model& model, const std::string& field)
    {
        int index = -1;
        for (int i = 0; i < FinFields<_Model>::count; i++)
            if (FinFields<_Model>::fields[i] == field)
            {
                index = i;
                break;
            }

        assert(index > -1);

        return get_field<T>(model, (c_uint)index);
    }
}
}