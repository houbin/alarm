#ifndef DISPATCH_QUEUE_H_
#define DISPATCH_QUEUE_H_

#include "../util/logger.h"
#include "../util/thread.h"

using namespace util;

enum
{
    PRIORITY_LOW = 0,
    PRIORITY_DEFAULT = 128,
    PRIORITY_HIGH = 255
};

class QueueItem
{
private:
    int priority_;
    string msg_;
public:
    QueueItem(int priority, string msg) : priority_(priority), msg_(msg) {}
    ~QueueItem() {}
};

class DispatchQueue
{
private:
    Logger *logger_;

public:
    DispatchQueue(Logger *logger);
    ~DispatchQueue();

    int Enqueue()

public:
    class DispatchThread : public 
};

#endif

