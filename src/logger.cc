#include "logger.h"

#include <iostream>

#include "time.h"

Logger::Logger() {
  //启动写日志线程
  std::thread writeLogTack([&]() {
    for (;;) {
      //获取当前日期，然后取日志信息，写入相应文件中
      time_t now = time(nullptr);
      tm *nowtm = localtime(&now);

      char file_name[128];
      sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900,
              nowtm->tm_mon + 1, nowtm->tm_mday);

      FILE *pf = fopen(file_name, "a+");
      if (pf == nullptr) {
        std::cout << "logger file:" << file_name << " open error!" << std::endl;
        exit(EXIT_FAILURE);
      }

      std::string msg = m_lockQue.Pop();
      char time_buf[128] = {0};
      sprintf(time_buf, "[%s]%d:%d:%d => ",
              (m_loglevel == INFO ? "INFO" : "ERROR"), nowtm->tm_hour,
              nowtm->tm_min, nowtm->tm_sec);
      msg.insert(0, time_buf);
      msg.append("\n");

      fputs(msg.c_str(), pf);
      fclose(pf);
    }
  });
  //设置分离线程
  writeLogTack.detach();
}

//获取日志单例
Logger &Logger::GetInstance() {
  static Logger logger;
  return logger;
}

//设置日志级别
void Logger::SetLogLevel(LogLevel level) { m_loglevel = level; }
//写日志 把日志信息写入lockQueue中
void Logger::Log(const std::string &msg) { m_lockQue.Push(msg); }
