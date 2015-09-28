#ifndef UTIL_ENV_H_
#define UTIL_ENV_H_

#include <stdint.h>
#include "slice.h"

namespace util
{

// copy from leveldb
// A file abstraction for sequentail writing. The implementation
// must provide buffering since callers may append fragments 
// at a time to the file
class WritableFile
{
public:
    WritableFile() {};
    virtual ~WritableFile() { }

    virtual int32_t Append(const Slice& data) = 0;
    virtual int32_t Close() = 0;
    virtual int32_t Flush() = 0;
    virtual int32_t Sync() = 0;

private:
    WritableFile(const WritableFile&);
    void operator=(const WritableFile&);
};

}

#endif

