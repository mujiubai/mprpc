#include <iostream>
#include <string>

#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "user.pb.h"

/**
 * @brief 是一个本地服务，提供两个进程内的本地方法
 *
 */
class UserService : public fixbug::UserServiceRpc {
 public:
  bool Login(std::string name, std::string pwd) {
    std::cout << "doing local service: Login" << std::endl;
    std::cout << "name:" << name << " pwd:" << pwd << std::endl;
    return false;
  }

  bool Register(uint32_t id, std::string name, std::string pwd) {
    std::cout << "doing local service: Register" << std::endl;
    std::cout << "id:" << id << " pwd:" << pwd << std::endl;
    return true;
  }

  //重写UserServiceRpc的虚函数，
  void Login(::google::protobuf::RpcController* controller,
             const ::fixbug::LoginRequest* request,
             ::fixbug::LoginResponse* response,
             ::google::protobuf::Closure* done) {
    //框架给业务上报了请求参数LoginRequest，业务获取相应数据做本地业务
    const std::string& name = request->name();
    const std::string& pwd = request->pwd();
    //做本地业务
    bool loginResult = Login(name, pwd);
    //把响应写入
    fixbug::ResultCode* code = response->mutable_result();
    response->set_success(loginResult);
    code->set_errcode(1);
    code->set_errmsg("err");

    //执行回调 做响应数据的序列化和网络发送（框架完成）
    done->Run();
  }

  void Register(google::protobuf::RpcController* controller,
                const ::fixbug::RegisterRequest* request,
                ::fixbug::RegisterResponse* response,
                ::google::protobuf::Closure* done) {
    uint32_t id = request->id();
    std::string name = request->name();
    std::string pwd = request->pwd();
    bool ret = Register(id, name, pwd);
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    response->set_success(ret);

    done->Run();
  }
};

int main(int argc, char** argv) {
  //框架初始化
  MprpcApplication::Init(argc, argv);

  RpcProvider provider;
  provider.NotiFyService(new UserService());

  provider.Run();

  return 0;
}