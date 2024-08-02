#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    GetFriendListsResponse rsp;
    ResultCode*result= rsp.mutable_result();
    result->set_errcode(0);
    result->set_errmsg("");


    User*friend1=rsp.add_friend_list();
    friend1->set_age(20);
    friend1->set_name("lulu");
    friend1->set_sex(User::WOMAN);

    User*friend2=rsp.add_friend_list();
    friend2->set_age(22);
    friend2->set_name("ksda");
    friend2->set_sex(User::WOMAN);

    int num=rsp.friend_list_size();
    //std::cout<<num<<std::endl;
    int i=0;
    while(i<num)
    {
        const fixbug::User user=rsp.friend_list(i++);
        std::cout<<user.age()<<std::endl;
        std::cout<<user.name()<<std::endl;
        std::cout<<user.sex()<<std::endl;
    }


    return 0;
}

// int main()
// {
//     LoginResponse rsp;
//     ResultCode *rsult = rsp.mutable_result();
//     rsult->set_errcode(1);
//     rsult->set_errmsg("登录处理失败，原因如下：");

//     // 类型内的类型嵌套处理：
//     // 获取嵌套类型的指针对象
//     // 对该对象进行赋值
// }
// int main1()
// {
//     // 定义登录消息的对象
//     LoginRequest req;
//     // 向对象写值  .set_("XXX");
//     req.set_name("lulu");
//     req.set_pwd("123456");

//     // 序列化->char*
//     std::string send_str;

//     if (req.SerializePartialToString(&send_str))
//     {
//         std::cout << send_str.c_str() << std::endl; // 展示序列化的内容
//     }

//     // 反序列化
//     // 定义接收的登录消息对象
//     LoginRequest req2;
//     if (req2.ParseFromString(send_str)) // 返回值为 bool类型
//     {
//         std::cout << req2.name() << std::endl;
//         std::cout << req2.pwd() << std::endl;
//     }

//     return 0;
// }