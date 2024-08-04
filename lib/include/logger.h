#pragma once
#include"lockqueue.h"
#include<string>

enum Loglevel
{
    INFO,//普通消息
    ERROR,//错误信息
};

//定义宏，直接调用具体的实现日志记录，不需要用户自行调用函数接口
#define LOG_INFO(logmsgformat, ...) \
    do \
    {  \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while(0)

#define LOG_ERROR(logmsgformat, ... ) \
    do \
    {  \
        Logger &logger=Logger::GetInstance();\
        logger.SetLogLevel(ERROR);\
        char c[1024]={0};\
        snprintf(c,1024,logmsgformat,##__VA_ARGS__);\
        logger.Log(c);\
    } while(0)
// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")

class Logger{
public:
    //获取单例对象
    static Logger& GetInstance();
    //设置日志级别
    void SetLogLevel(Loglevel level);
    //写日志
    void Log(std::string msg);
private:
    int m_loglevel;//日志级别
    LockQueue<std::string>m_locQue;//日志队列缓冲区

    Logger();//单例模式只初始化一次
    Logger(const Logger&)=delete;
    Logger(const Logger&&)=delete;
};
