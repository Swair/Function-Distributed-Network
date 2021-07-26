#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "Common.h"

jmp_buf envbuf;
void callbacktrace()
{
    void *array[100];
    char **strings;
    int size, i;

    size = backtrace(array, 100);
    strings = backtrace_symbols(array, size);

    printf("%p\n", strings);
    for(i = 0; i < size; i++)
        printf("sigsegv at :%p:%s\n", array[i], strings[i]);

    free(strings);
}

void sighandler(int sig)
{
    printf("algError, catch sig id %d\n", sig);
    callbacktrace();
    siglongjmp(envbuf, 1);
}

std::string getTimeStamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm current_time;
    localtime_r(&tv.tv_sec, &current_time);

    int year = (1900 + current_time.tm_year);
    int month = (1 + current_time.tm_mon);
    int day = current_time.tm_mday;
    int hour = current_time.tm_hour;
    int minute = current_time.tm_min;
    int sec = current_time.tm_sec;
    int msec = (int) (tv.tv_usec / 1000);

    char timeStamp[128] = {0};
    snprintf(timeStamp, sizeof(timeStamp) / sizeof(char), "%d-%02d-%02d %02d:%02d:%02d.%03d", year, month, day,
            hour, minute, sec, msec);

    return timeStamp;
}

void log_write(const char* fmt, ...)
{
    char logBuf[10240] = {0};
    memset(logBuf, 0, 10240);
    va_list vp; 
    std::string timeStamp = getTimeStamp(); 
    std::string logMsg = timeStamp + " [" + std::to_string(pthread_self()) + ", " + std::to_string(getpid()) + "]: ";
    va_start(vp, fmt);
    vsnprintf(logBuf, sizeof(logBuf) - 1, fmt, vp); 
    va_end(vp);
    logMsg.insert(logMsg.end(), logBuf, logBuf + strlen(logBuf));

    printf("%s", logMsg.c_str()); 
    fflush(stdout);
}

int getContent(std::string& content, const std::string& inputText, const std::string& bsig, int offset, const std::string& esig)
{
    auto bi = inputText.find(bsig);
    if(bi == std::string::npos)
        return 0;

    auto bText = inputText.substr(bi + bsig.size() + offset); 

    auto ei = bText.find(esig);
    if(ei == std::string::npos)
        return 0;

    content = bText.substr(0, ei);

    return 1;
}

bool exeCmd(const std::string& cmd)
{
    if(-1 == system(cmd.c_str()))
    {
        log_write("exeCmd, %s failed\n", cmd.c_str());
        return false; 
    }
    return true;
}

std::string setRes(int code, const std::string& msg)
{
    std::string result = "{\"code\": " + std::to_string(code) + ", \"msg\": \"" + msg + "\"}";
    return result;
}














