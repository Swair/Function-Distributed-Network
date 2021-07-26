#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <curl/curl.h>
#include <signal.h>
#include <execinfo.h>
#include <iconv.h>
#include <dirent.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <tuple>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <functional>
#include <memory>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <random>
#include <thread>
#include "json.hpp"
using json = nlohmann::json;
using std::default_random_engine;
using std::uniform_int_distribution;
using namespace std::placeholders;


void log_write(const char* fmt, ...);
int getContent(std::string& content, const std::string& inputText, const std::string& bsig, int offset, const std::string& esig);
std::string getIp();
void sighandler(int sig);
extern jmp_buf envbuf;



template <typename type> std::string dtos(type para)
{
    std::string ret;
    if(std::is_same<type, int>::value)
    {
        ret = std::to_string(para);
    }
    else if(std::is_same<type, unsigned int>::value)
    {
        ret = std::to_string(para);
    }
    else if(std::is_same<type, double>::value)
    {
        ret = std::to_string(para);
    }
    return ret;

}

class ExceptionPanic
{
    public:
        ExceptionPanic(const std::string& msg)
        {
            log_write("%s\n", msg.c_str());
            msg_ = msg;
        }

        const char* what()
        {
            return msg_.c_str();
        }

        std::string get()
        {
            return msg_;
        }

    private:
        std::string msg_;
};


    template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

std::string setRes(int code, const std::string& msg);





#endif
