#include<iostream>
#include"mprpcapplication.h"
#include"user.pb.h"
#include"mprpcchannel.h"
#include"logger.h"
int main(int argc,char**argv)
{
    //程序启动想用mprpc框架享受rpc服务，一定先初始化框架（只初始化一次）
    MprpcApplication::Init(argc,argv);

    //演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    //Rpc请求：
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    //Rpc 响应：
    fixbug::LoginResponse response;
    stub.Login(nullptr,&request,&response,nullptr);
    //stub.Login();  
    //RpcChannel ->RpcChannel::callMethod 
    //集中做所有的参数序列化、网络发送

    if(0==response.result().errcode())
    {
        LOG_INFO("Rpc Login response success ");
        std::cout<<"Rpc Login response success:"<<response.sucess()<<std::endl;
    }
    else
    {
        LOG_ERROR("Rpc Login response error: %s ",response.result().errmsg().c_str());
        std::cout<<"Rpc Login response error: "<<response.result().errmsg()<<std::endl;
    }
    //Rpc Registwe请求：
    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("zhang san");
    req.set_pwd("123456");

    //Rpc 响应：
    fixbug::RegisterResponse res;
    stub.Register(nullptr,&req,&res,nullptr);
    //stub.Resgister();  
    //RpcChannel ->RpcChannel::callMethod 
    //集中做所有的参数序列化、网络发送

    if(0==res.result().errcode())
    {
        LOG_INFO("Rpc Register response success ");
        std::cout<<"Rpc Register response success:"<<res.sucess()<<std::endl;
    }
    else
    {
        LOG_ERROR("Rpc Register response error: %s ",res.result().errmsg().c_str());
        std::cout<<"Rpc Register response error: "<<res.result().errmsg()<<std::endl;
    }

    return 0;
}