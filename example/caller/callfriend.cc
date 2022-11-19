#include <iostream>

#include "friend.pb.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

int main(int argc, char **argv) {
  //需要先调用框架的初始化函数
  MprpcApplication::Init(argc, argv);

  //调用远程发布的rpc方法Login
  fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
  //相当于调用RpcChannel->RpcChannel::callMethod
  // stub.Login();
  fixbug::getFriendListRequest request;
  request.set_id(1);

  fixbug::getFriendListResponse response;
  stub.getFriendList(nullptr, &request, &response, nullptr);

  if (response.result().errcode() == 0) {
    std::cout << "rpc login response: " << std::endl;
    for (auto name : response.friends()) {
      std::cout << name << std::endl;
    }
  } else {
    std::cout << "rpc login response error:" << response.result().errmsg()
              << std::endl;
  }

  return 0;
}