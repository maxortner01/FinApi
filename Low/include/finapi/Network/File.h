#pragma once

#include "CClient.h"

namespace finapi
{
namespace Cloud
{
    class File : public FinStream
    {
        friend class FileFriend;

        uint   fsize;
        char*  _buffer;

    public:
        File();
        File(const char* filename, const char* address);
        File(const char* filename, Cloud::Address address);

        ReadStatus read(char* ptr, c_uint size) override;
        void       close();

        Status status()   const;
        c_uint filesize() const;

        ~File();
    };

    class FileFriend : public FinStream
    {
        File* const _file;

    public:
        FileFriend(File& file);

        ReadStatus read(char* ptr, c_uint size) override;

        const bool good() const override;
    };
}
}