#pragma once
#include <google/protobuf/service.h>

#include <string>

/**
 * @brief 此类主要是用来控制rpc调用过程中出现的错误信息
 * 
 */
class MprpcController : public google::protobuf::RpcController {
 public:
  MprpcController();
  void Reset() override;
  bool Failed() const override;
  std::string ErrorText() const override;
  void SetFailed(const std::string& reason) override;
  // No implement
  void StartCancel() override;
  bool IsCanceled() const override;
  void NotifyOnCancel(google::protobuf::Closure* callback) override;

 private:
  bool m_failed;          // rpc执行状态
  std::string m_errText;  //错误信息
};