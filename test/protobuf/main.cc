#include <iostream>
#include <string>

#include "test.pb.h"

using namespace fixbug;

int main() {
  //   LoginResponse rsp;
  //   ResultCode *rc = rsp.mutable_result();//类成员设置
  //   rc->set_errcode(1);
  //   rc->set_errmsg("登录处理失败");

  GetFriendListsResponse rsp;
  ResultCode *rc = rsp.mutable_result();
  rc->set_errcode(0);

  User *user1 = rsp.add_friend_list();
  user1->set_name("zhang san");
  user1->set_age(10);
  user1->set_sex(User::MAN);//设置枚举

  std::cout << rsp.friend_list_size() << std::endl;


  return 0;
}

int main1() {
  //封装login请求对象数据
  LoginRequest req;
  req.set_name("wang");
  req.set_pwd("123434");
  //对象序列化
  std::string send_str;
  if (req.SerializeToString(&send_str)) {
    std::cout << send_str << std::endl;
  }

  // send_str反序列化
  LoginRequest reqB;
  if (reqB.ParseFromString(send_str)) {
    std::cout << reqB.name() << std::endl;
    std::cout << reqB.pwd() << std::endl;
  }

  return 0;
}