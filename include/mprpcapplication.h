#pragma once

#include "mprpcconfig.h"

// mprpc框架基础类
class MprpcApplication {
 public:
  static void Init(int argc, char **argv);
  //获取单例对象
  static MprpcApplication &GetInstance();
  //获取配置文件对象地址
  static MprpcConfig &GetConfig();

 private:
  MprpcApplication(){};
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;

  static MprpcConfig config_;
};