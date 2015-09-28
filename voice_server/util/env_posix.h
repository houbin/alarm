#ifndef UTIL_ENV_POSIX_H_
#define UTIL_ENV_POSIX_H_

#include <string>
#include "env.h"

using namespace std;

namespace util
{

class PosixWritableFile : public WritableFile
{
private:
    string filename_;
    FILE *file_;
    size_t written_bytes_;

public:
    PosixWritableFile(string filename);
    ~PosixWritableFile();

    int32_t Append(const Slice& data);

    int32_t Close();
    int32_t Flush();
    int32_t Sync();

    size_t WrittenBytes() const { return written_bytes_; }
};

}

#endif

