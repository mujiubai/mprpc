#include "rpcprovider.h"

#include <functional>
#include <string>

#include "logger.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

void RpcProvider::NotiFyService(google::protobuf::Service *service) {
  ServiceInfo service_info;

  //获取对象描述信息
  const google::protobuf::ServiceDescriptor *pserverDesc =
      service->GetDescriptor();
  //获取服务名字
  std::string service_name = pserverDesc->name();
  //获取方法数量
  int methodCnt = pserverDesc->method_count();

  LOG_INFO("service_name:%s", service_name.c_str());

  //将service中的所有方法保存到service_info中
  for (int i = 0; i < methodCnt; ++i) {
    //获取服务对象指定下标的服务方法描述
    const google::protobuf::MethodDescriptor *pmethodDesc =
        pserverDesc->method(i);
    std::string method_name = pmethodDesc->name();
    service_info.m_methodMap.insert({method_name, pmethodDesc});
    LOG_INFO("method_name:%s", method_name.c_str());
  }

  service_info.m_service = service;
  m_serviceMap.insert({service_name, service_info});  //保存service
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run() {
  std::string ip =
      MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
  uint16_t port = atoi(MprpcApplication::GetInstance()
                           .GetConfig()
                           .Load("rpcserverport")
                           .c_str());
  muduo::InetAddress address(port, ip);

  //创建Tcpserver对象
  muduo::TcpServer server(&m_eventLoop, address, "rpcProvider");

  //绑定连接回调和消息读写回调
  server.setConnectionCallback(
      std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
  server.setMessageCallback(
      std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,
                std::placeholders::_2, std::placeholders::_3));

  //设置muduo库线程数
  //   server.setThreadNum(4);
  LOG_INFO("RpcProvider start service at ip:%s port:%d", ip.c_str(), port);

  server.start();
  m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::TcpConnectionPtr &conn) {
  if (!conn->connected()) {
    //连接断开
    conn->shutdown();
  }
}

/**
 * 在框架内部，rpcprovider和rpcconsumer协商好通信的protobuf数据类型
 * service_name method_name args
 * 定义proto的message类型，进行数据的序列化和反序列化
 * header_size+header_str+args_str
 * 将远程传过来的字符串反序列化生成新的request对象，然后调用相应的方法，最后将response结果序列化字符串发送回去
 *
 * 读写事件回调  远程有rpc服务请求 onmessage会响应
 */
void RpcProvider::OnMessage(const muduo::TcpConnectionPtr &conn,
                            muduo::Buffer *buffer, muduo::Timestamp) {
  // rpc调用的字符流，包括Login args
  std::string recv_buf = buffer->retrieveAllAsString();

  //从字符流中读取前4字节
  uint32_t header_size = 0;
  recv_buf.copy((char *)&header_size, 4, 0);

  //读取headersize大小的字符流,反序列化数据，得到rpc请求详细信息
  std::string rpc_header_str = recv_buf.substr(4, header_size);
  mprpc::RpcHeader rpcHeader;
  std::string service_name;
  std::string method_name;
  uint32_t args_size;
  if (rpcHeader.ParseFromString(rpc_header_str)) {
    //反序列化成功
    service_name = rpcHeader.service_name();
    method_name = rpcHeader.method_name();
    args_size = rpcHeader.args_size();
  } else {
    //反序列化失败
    LOG_INFO("rpc_header_str:%s parse error!", rpc_header_str.c_str());
  }
  std::string args_str = recv_buf.substr(4 + header_size, args_size);

  // std::cout << "==========================================" << std::endl;
  // std::cout << "header_size:" << header_size << std::endl;
  // std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
  // std::cout << "service_name:" << service_name << std::endl;
  // std::cout << "method_name:" << method_name << std::endl;
  // std::cout << "args_str:" << args_str << std::endl;
  // std::cout << "==========================================" << std::endl;

  //获取service对象和method对象
  const auto &it = m_serviceMap.find(service_name);
  if (it == m_serviceMap.end()) {
    std::cout << service_name << " is not exist!" << std::endl;
    LOG_ERROR("%s is not exist!", service_name.c_str());
    return;
  }

  const auto &mit = it->second.m_methodMap.find(method_name);
  if (mit == it->second.m_methodMap.end()) {
    LOG_INFO("%s:%s is not exist!", service_name.c_str(), method_name.c_str());
    return;
  }

  //获取service对象
  google::protobuf::Service *service = it->second.m_service;
  //获取method方法
  const google::protobuf::MethodDescriptor *method = mit->second;

  //生成rpc方法调用的请求request和响应response参数
  google::protobuf::Message *request =
      service->GetRequestPrototype(method).New();
  if (!request->ParseFromString(args_str)) {
    LOG_INFO("request parse error! content:%s", args_str.c_str());
  }
  google::protobuf::Message *response =
      service->GetResponsePrototype(method).New();

  // method方法绑定一个closure的回调函数
  //相当于使用SendRpcResponse函数重写了closure类中的Run方法,在用户自己写得服务中最后会执行done->run(),最终会执行SendRpcResponse方法
  google::protobuf::Closure *done =
      google::protobuf::NewCallback<RpcProvider,
                                    const muduo::TcpConnectionPtr &,
                                    google::protobuf::Message *>(
          this, &RpcProvider::SendRpcResponse, conn, response);

  //调用rpc方法
  service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::TcpConnectionPtr &conn,
                                  google::protobuf::Message *response) {
  std::string response_str;
  // response序列化
  if (response->SerializeToString(&response_str)) {
    conn->send(response_str);
  } else {
    LOG_INFO("serialize response_str error!");
  }
  //模拟http短链接服务，由rpcprovider主动断开连接
  conn->shutdown();
}