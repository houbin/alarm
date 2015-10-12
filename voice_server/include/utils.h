/**
 * @created:               2012/05/16
 *
 * @file                utils.h
 *
 * @author              wei yao <yaocoder@gmail.com>
 *
 * @version             1.0
 *
 * @LICENSE
 *
 * @brief                通用工具方法
 *
 */
#ifndef UTILS_H__
#define UTILS_H__

#include <netinet/in.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <uuid/uuid.h>
using namespace std;

namespace utils
{
#define CRLF "\r\n"

static inline void ShellCommand(const char *command, char *buf, int length)
{
    FILE   *stream;
    stream = popen(command,"r");
    fread( buf, sizeof(char), length,  stream);
    pclose( stream );
    return;
}

static inline void DeleteElementFrom(const std::string& value, std::vector<std::string>& vec)
{
    std::vector<std::string>::iterator iter;
    for(iter = vec.begin(); iter != vec.end();)
    {
        if((*iter).compare(value) == 0)
            iter = vec.erase(iter);
        else
            ++iter;
    }
}

static inline void SplitData(const std::string& str, const std::string& delimiter, std::vector<std::string>& vec_data)
{
    std::string s = str;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        vec_data.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
}

static inline std::string ReplaceString(const std::string& str, const std::string& flag, const std::string& replaceFlag)
{
    boost::regex expressionReplace(flag);
    std::string strTemp = boost::regex_replace(str, expressionReplace, replaceFlag);
    return strTemp;
}

static inline bool FindCRLF(const std::string& s)
{
    if(s.find("\r\n") != std::string::npos)
        return true;
    else
        return false;
}

static inline std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> vec_temp;
    boost::algorithm::split(vec_temp, str, boost::algorithm::is_any_of(delimiter));
    DeleteElementFrom("", vec_temp);
    return vec_temp;
}

static inline std::string JoinListByDelimiter(const std::vector<std::string>&vec, const std::string& delimiter)
{
    std::string str;
    for(unsigned int i = 0; i < vec.size(); ++i)
    {
        str = str + vec.at(i) + delimiter;
    }

    return str;
}

static inline std::string JoinSetByDelimiter(const std::set<std::string>& set_string, const std::string& delimiter)
{
    std::string str;
    std::set<std::string>::iterator iter;
    for(iter = set_string.begin(); iter != set_string.end(); ++iter)
    {
        str = str + *iter + delimiter;
    }

    return str;
}

template<typename K, typename V>
inline void MapKeyToSet(const std::map<K, V>& map_temp, std::set<K>& set_temp)
{
    typedef typename std::map<K, V>::const_iterator iter_temp;
    iter_temp iter;
    for(iter = map_temp.begin(); iter != map_temp.end(); ++iter)
    {
        set_temp.insert(iter->first);
    }
}


template<typename T> std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

static inline std::string int2str(int v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

static inline std::string NowtimeString()
{
    std::time_t time_now = std::time(NULL);
    tm* tm_now = localtime(&time_now);
    char time_str[sizeof("yyyymmddhhmmss")] = {0};
    std::strftime(time_str, sizeof(time_str), "%Y%m%d%H%M%S", tm_now);
    return time_str;
}

static inline std::string GetCurrentDayString()
{
    time_t time_;
    struct tm *ptm;
    time_ = time(NULL);
    ptm = localtime(&time_);
    char temp[100] =
    { 0 };
    snprintf(temp, sizeof(temp), "%d-%d-%d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
    std::string str;
    str.assign(temp);
    return str;
}

static inline std::string GetCurrentTime()
{
    std::time_t time_now = std::time(NULL);
    tm* tm_now = localtime(&time_now);
    char time_str[100] ={ 0 };
    std::strftime(time_str, sizeof(time_str), "%Y年%m月%d日 %H:%M:%S", tm_now);
    return time_str;
}

static inline std::string GetMonthString(const std::string& month)
{
    int mon = atoi(month.c_str());
    switch(mon)
    {
    case 1:
        return "January";
    case 2:
        return "February";
    case 3:
        return "March";
    case 4:
        return "April";
    case 5:
        return "May";
    case 6:
        return "June";
    case 7:
        return "July";
    case 8:
        return "August";
    case 9:
        return "September";
    case 10:
        return "October";
    case 11:
        return "November";
    case 12:
        return "December";
    default:
        break;
    }
    return NULL;
}

static inline unsigned long GetTickCount()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        //error
    }

    return (ts.tv_sec*1000 + ts.tv_nsec/(1000*1000));
}

template<class T>
class Singleton: private T
{
public:
    static T &Instance()
    {
        static Singleton<T> _instance;
        return _instance;
    }
private:
    Singleton()
    {
    }
    ~Singleton()
    {
    }
};

template<typename T>
T& G()
{
    return Singleton<T>::Instance();
}

template<typename T> inline void SafeDeleteArray(T*& p)
{
    if (NULL != p)
    {
        delete[] p;
        p = 0;
    }
}

template<typename T> inline void SafeDelete(T*& p)
{
    if (NULL != p)
    {
        delete p;
        p = 0;
    }
}

static inline void Unicode2UTF8OneWord(std::string& obj)
{
    int count = 0;
    int index = 0;
    while((index = int(obj.find("\\u00", index))) != int(std::string::npos))
    {
        count++;
        index++;
    }

    char ch[count+1];
    index = 0;
    std::string substr;
    for(int i = 0; i < count; i++)
    {
        index = obj.find("\\u00", index);
        index += 4;
        substr = obj.substr(index, 2); 
        ch[i] = strtol(substr.c_str(), NULL, 16);
    }

    ch[count] = '\0';
    obj = ch; 
}

static inline void Unicode2UTF8(std::string& obj)
{
    int beg = 0;
    std::string substr;

    while((beg = int(obj.find("\\u00", 0))) != int(std::string::npos))
    {
        substr = obj.substr(beg, 18);
        Unicode2UTF8OneWord(substr);
        obj.replace(beg, 18, substr);
    }
}

static inline std::string encrypt(const char* ch)
{
    char* p = (char*)ch;
    int i = 0;
    while(*p != '\0')
        *p++ ^= i++;

    std::string  str = ch;
    return str;
}

static inline std::string decrypt(const char* ch)
{
    char* p = (char*)ch;
    int i = 0;
    while(*p != '\0')
        *p++ ^= i++;

    std::string  str = ch;
    return str;
}

}
#endif
