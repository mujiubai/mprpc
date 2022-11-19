#include <iostream>

#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"

int main(int argc, char **argv) {
  //需要先调用框架的初始化函数
  MprpcApplication::Init(argc, argv);

  //调用远程发布的rpc方法Login
  fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
  //相当于调用RpcChannel->RpcChannel::callMethod
  // stub.Login();
  fixbug::LoginRequest request;
  request.set_name("zhang wang");
  request.set_pwd("123456");

  fixbug::LoginResponse response;
  stub.Login(nullptr, &request, &response, nullptr);

  if (response.result().errcode() == 0) {
    std::cout << "rpc login response: " << response.success() << std::endl;
  } else {
    std::cout << "rpc login response error:" << response.result().errmsg()
              << std::endl;
  }

  fixbug::RegisterRequest req;
  req.set_id(2000);
  req.set_name("mprpc");
  req.set_pwd("123");
  fixbug::RegisterResponse rsp;

  stub.Register(nullptr,&req,&rsp,nullptr);

    if (rsp.result().errcode() == 0) {
    std::cout << "rpc register response: " << rsp.success() << std::endl;
  } else {
    std::cout << "rpc register response error: reg" << rsp.result().errmsg()
              << std::endl;
  }

  return 0;
}