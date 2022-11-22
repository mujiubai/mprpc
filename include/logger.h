#pragma once
#include <lockqueue.h>

#include <string>

enum LogLevel {
  INFO,
  ERROR,
};

/**
 * @brief 日志类
 *
 */
class Logger {
 public:
  //获取日志单例
  static Logger &GetInstance();
  //设置日志级别
  void SetLogLevel(LogLevel level);
  //写日志
  void Log(const std::string &msg);

 private:
 //在构造函数中开启新线程，不断写日志
  Logger();
  Logger(const Logger &) = delete;
  Logger(Logger &&) = delete;

  int m_loglevel;
  LockQueue<std::string> m_lockQue;  //日志缓冲队列
};

//定义宏
#define LOG_INFO(logmsgformat, ...)                 \
  do {                                              \
    Logger &logger = Logger::GetInstance();         \
    logger.SetLogLevel(INFO);                       \
    char c[1024] = {0};                             \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c);                                  \
  } while (0);

#define LOG_ERROR(logmsgformat, ...)                \
  do {                                              \
    Logger &logger = Logger::GetInstance();         \
    logger.SetLogLevel(ERROR);                      \
    char c[1024] = {0};                             \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c);                                  \
  } while (0);
