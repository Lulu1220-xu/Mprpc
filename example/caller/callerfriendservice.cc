#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"

int main(int argc, char **argv)
{
    // 程序启动想用mprpc框架享受rpc服务，一定先初始化框架（只初始化一次）
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendListRpc_Stub stub(new MprpcChannel());

    // Rpc请求：
    fixbug::GetFriendListRequest request;
    request.set_id(1000);

    // Rpc 响应：
    fixbug::GetFriendListRespond response;

    // 定义 状态控制对象
    Mprpccontroller con;
    stub.GetFriendList(&con, &request, &response, nullptr);
    // stub.Login();
    // RpcChannel ->RpcChannel::callMethod
    // 集中做所有的参数序列化、网络发送

    if (con.Failed()) // Rpc调用出错
    {
        std::cout << con.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "Rpc FriendsList response success:" << std::endl;
            int count = response.friends_size();
            for (int i = 0; i < count; i++)
            {
                std::cout << "index: " << i + 1 << " , name: " << response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "Rpc Login response error: " << response.result().errmsg() << std::endl;
        }
    }

    return 0;
}