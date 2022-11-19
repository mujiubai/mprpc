#pragma once

#include <google/protobuf/descriptor.h>
#include <mymuduo/Buffer.h>
#include <mymuduo/TcpConnection.h>
#include <mymuduo/TcpServer.h>
#include <mymuduo/Timestamp.h>

#include <unordered_map>

#include "google/protobuf/service.h"

/**
 * @brief 框架提供专用于发布rpc服务的网络对象类
 *
 */
class RpcProvider {
 public:
  //发布Rpc方法的函数
  void NotiFyService(google::protobuf::Service *service);
  //启动rpc服务节点，开始提供rpc远程网络调用服务
  void Run();

 private:
  //组合eventloop
  muduo::EventLoop m_eventLoop;

  // service服务类型信息
  struct ServiceInfo {
    google::protobuf::Service *m_service;
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor *>
        m_methodMap;  //保存服务方法
  };

  //存储服务对象和其服务方法的所有信息
  std::unordered_map<std::string, ServiceInfo> m_serviceMap;

  void OnConnection(const muduo::TcpConnectionPtr &);
  void OnMessage(const muduo::TcpConnectionPtr &, muduo::Buffer *,
                 muduo::Timestamp);
  // closure回调操作，用于序列号rpc的响应和网络发送
  void SendRpcResponse(const muduo::TcpConnectionPtr &,
                       google::protobuf::Message *);
};