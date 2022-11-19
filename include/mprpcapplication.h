#pragma once

#include "mprpcconfig.h"

// mprpc框架基础类
class MprpcApplication {
 public:
  static void Init(int argc, char **argv);
  static MprpcApplication &GetInstance();
  static MprpcConfig &GetConfig();

 private:
  MprpcApplication(){};
  MprpcApplication(const MprpcApplication &) = delete;
  MprpcApplication(MprpcApplication &&) = delete;

  static MprpcConfig config_;
};