#include "mprpcconfig.h"

#include <iostream>
#include <string>
#include "logger.h"

//加载配置项文件
void MprpcConfig::LoadConfigFile(const char *configFile) {
  FILE *pf = fopen(configFile, "r");
  if (pf == nullptr) {
    LOG_ERROR("%s is not exist!",configFile);
    exit(EXIT_FAILURE);
  }

  // 1.注释  2.正确的配置项=   3.去掉开头空格
  while (!feof(pf)) {
    char buf[128] = {0};
    fgets(buf, 128, pf);
    //删除前面空格
    std::string srcBuf(buf);
    Trim(srcBuf);

    //判断#号
    if (srcBuf[0] == '#' || srcBuf.empty()) continue;

    //解析配置项
    int idx = srcBuf.find('=');
    if (idx == -1) {
      continue;
    }

    std::string key;
    std::string value;
    key = srcBuf.substr(0, idx);
    Trim(key);
    int endidx=srcBuf.find('\n',idx);
    value = srcBuf.substr(idx + 1, endidx - idx-1);
    Trim(value);
    configMap_.insert({key, value});
  }
}

//查询配置项信息
std::string MprpcConfig::Load(const std::string &key) {
  auto it = configMap_.find(key);
  if (it == configMap_.end()) {
    return "";
  }
  return it->second;
}

//去掉字符串前后空格
void MprpcConfig::Trim(std::string &srcBuf) {
  int idx = srcBuf.find_first_not_of(' ');
  if (idx != -1) {
    srcBuf = srcBuf.substr(idx, srcBuf.size() - idx);
  }
  //删除后面空格
  idx = srcBuf.find_last_not_of(' ');
  if (idx != -1) {
    srcBuf = srcBuf.substr(0, idx + 1);
  }
}