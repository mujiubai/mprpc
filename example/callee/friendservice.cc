#include <iostream>
#include <string>

#include "friend.pb.h"
#include "logger.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class FriendService : public fixbug::FriendServiceRpc {
 public:
  std::vector<std::string> GetFriendList(uint32_t userid) {
    LOG_INFO(" do GetFriendList service");
    std::vector<std::string> vec;
    vec.push_back("li");
    vec.push_back("zhang");
    vec.push_back("wang");
    return vec;
  }

  void getFriendList(google::protobuf::RpcController* controller,
                     const ::fixbug::getFriendListRequest* request,
                     ::fixbug::getFriendListResponse* response,
                     ::google::protobuf::Closure* done) {
    uint32_t id = request->id();
    std::vector<std::string> friendList = GetFriendList(id);
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    for (auto& fri : friendList) {
      std::string* t = response->add_friends();
      *t = fri;
    }
    done->Run();
  }
};

int main(int argc, char** argv) {
  LOG_INFO("first log message test");
  LOG_ERROR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
  //框架初始化
  MprpcApplication::Init(argc, argv);

  RpcProvider provider;
  provider.NotiFyService(new FriendService());

  provider.Run();

  return 0;
}