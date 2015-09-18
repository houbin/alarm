#include <stdio.h>
#include <unistd.h>
#include <string>
#include <errno.h>
#include "env_posix.h"

using namespace std;

namespace util
{

PosixWritableFile::PosixWritableFile(string filename)
: filename_(filename), file_(NULL), written_bytes_(0)
{
    file_ = fopen(filename_.c_str(), "ae");
    assert(file_ != NULL);
}

PosixWritableFile::~PosixWritableFile()
{
    if (file_ != NULL)
    {
        fclose(file_);
        file_ = NULL;
    }
}

int32_t PosixWritableFile::Append(const Slice& data)
{
    size_t r = fwrite(data.data(), 1, data.size(), file_);
    if (r != data.size())
    {
        return -errno;
    }

    written_bytes_ += data.size();

    return 0;
}

int32_t PosixWritableFile::Close()
{
    int32_t ret = 0;

    if (fclose(file_) != 0)
    {
        ret = -errno;
    }

    file_ = NULL;
    return ret;
}

int32_t PosixWritableFile::Flush()
{
    if (fflush(file_) != 0)
    {
        return -errno;
    }

    return 0;
}

int32_t PosixWritableFile::Sync()
{
    if (fdatasync(fileno(file_)) != 0)
    {
        return -errno;
    }

    return 0;
}

}

