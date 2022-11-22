#include <iostream>

#include "friend.pb.h"
#include "logger.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

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
  MprpcController controller;

  stub.getFriendList(&controller, &request, &response, nullptr);

  if (controller.Failed()) {
    LOG_INFO("%s", controller.ErrorText().c_str());
  } else {
    if (response.result().errcode() == 0) {
      LOG_INFO("rpc login response: ");
      for (auto name : response.friends()) {
        LOG_INFO("%s", name.c_str());
      }
    } else {
      LOG_INFO("rpc login response error:%s",
               response.result().errmsg().c_str());
    }
  }

  return 0;
}