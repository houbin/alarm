#include <stdlib.h>
#include "logger.h"
#include "../include/errcode.h"

namespace util
{

LogLevel Logger::g_log_level = INFO;

Logger::Logger(const string& log_dir, size_t roll_size, bool thread_safe, int check_every_n)
: log_dir_(log_dir), 
roll_size_(roll_size),
check_every_n_(check_every_n),
count_(0), 
last_roll_(0),
mutex_("logger::mutex")
{
    RollFile();
}

Logger::~Logger()
{

}

bool Logger::RollFile()
{
    time_t now;

    string filename = GetLogFileName(log_dir_, &now);
    if (now > last_roll_)
    {
        last_roll_ = now;
        file_.reset(new PosixWritableFile(filename));
        return true;
    }

    return false;
}

string Logger::GetLogFileName(const string& log_dir, time_t *now)
{
    char buffer[32] = {0};
    string filename = log_dir;

    *now = time(NULL);
    struct tm t;
    localtime_r(now, &t);

    strftime(buffer, sizeof(buffer), ".%Y%m%d-%H%M%S.", &t);
    
    filename += "log";
    filename += buffer;

    char pidbuffer[32] = {0};
    snprintf(pidbuffer, sizeof pidbuffer, "%d", getpid());

    filename += pidbuffer;
    filename += ".log";

    return filename;
}

void Logger::Logv(const char* format, va_list ap)
{
    char buffer[500];
    char *base;
    int32_t size;

    const uint64_t thread_id = pthread_self();
    for (int iter = 0; iter < 2; iter++)
    {
        if (iter == 0)
        {
            size = sizeof(buffer);
            base = buffer;
        }
        else
        {
            size = 30000;
            base = new char[30000];
        }

        char *p = base;
        char *limit = p + size;
        
        struct timeval now;
        gettimeofday(&now, NULL);
        time_t sec = now.tv_sec;
        struct tm t;
        localtime_r(&sec, &t);
        
        p += snprintf(p, limit - p, 
                     "%04d/%02d/%02d %02d:%02d:%02d.%06d %llx ",
                     t.tm_year + 1900,
                     t.tm_mon + 1,
                     t.tm_mday,
                     t.tm_hour,
                     t.tm_min,
                     t.tm_sec,
                     static_cast<int>(now.tv_usec),
                     static_cast<long long unsigned int>(thread_id)
                    );

        if (p < limit)
        {
            va_list backup_ap;
            va_copy(backup_ap, ap);
            p += vsnprintf(p, limit - p, format, backup_ap);
            va_end(backup_ap);
        }

        if (p >= limit)
        {
            if (iter == 0)
            {
                continue;
            }
            else
            {
                p = limit - 1;
            }
        }
        
        if (p == base || p[-1] != '\n')
        {
            *p++ = '\n';
        }

        assert(p <= limit);
        Slice s(base, p - base);
        Append(s);
        
        if (base != buffer)
        {
            delete[] base;
        }
        
        break;
    }

    return;
}

void Logger::Append(const Slice & data)
{
    Mutex::Locker lock(mutex_);
    file_->Append(data);
    file_->Flush();

    // check if need roll file
    if (file_->WrittenBytes() > roll_size_)
    {
        RollFile();
    }
    else
    {
        ++count_;
        if (count_ >= check_every_n_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            
            time_t last_period = last_roll_ / kRollPerSeconds_ * kRollPerSeconds_;
            time_t this_period = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (this_period != last_period)
            {
                RollFile();
            }
        }
    }

    return;
}

int32_t Logger::SetLogLevel(LogLevel level)
{
    g_log_level = level;
    return 0;
}

LogLevel Logger::GetLogLevel()
{
    return g_log_level;
}

int32_t NewLogger(const char* name, Logger **logger)
{
    *logger = new Logger(name);
    return 0;
}

void LogFunc(Logger *info_log, const char* format, ...)
{
    if (NULL != info_log)
    {
        va_list ap;
        va_start(ap, format);
        info_log->Logv(format, ap);
        va_end(ap);
    }

    return;
}

}

