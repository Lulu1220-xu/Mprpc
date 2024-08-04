#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "mprpcprovider.h"
#include"logger.h"
/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/
class UserService : public fixbug::UserServiceRpc // 使用在rpc服务发布端（rpc服务提供者）
{
public:
    bool Login(std::string name, std::string pwd)
    {
        LOG_INFO("进入远程Login 服务：\n doing local service: Login");
        std::cout<<"进入远程Login 服务："<<std::endl;
        std::cout << "doing local service: Login" << std::endl;
        LOG_INFO("name:%s ,pwd:%s ",name.c_str(),pwd.c_str());
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;  
        return true;
    }
    bool Register(uint32_t id,std::string name,std::string pwd)
    {
        LOG_INFO("进入远程Register 服务：\n doing Register service: Register");
        std::cout<<"进入远程Register 服务："<<std::endl;
        std::cout << "doing Register service: Register" << std::endl;
        LOG_INFO("id:%d , name:%s , pwd:%s ",id,name.c_str(),pwd.c_str());
        std::cout <<" id:"<<id<< " name:" << name << " pwd:" << pwd << std::endl;  
        return true;
    }
    //重写UserServiceRpc 基类的虚函数
    //作为服务的执行者
    //函数的使用背景：
    /*
    caller 调用RPC  通过muduo网络库发送到callee    callee获取到服务的请求后
    通过protobuf 反序列化  到下面的函数：执行
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,//服务请求
                       ::fixbug::LoginResponse* response,//返回值
                       ::google::protobuf::Closure* done)//回调
    {       
        //框架给业务上报了参数LoginRequest ，
        //1：获取到函数的参数，做本地业务
        std::string name=request->name();
        std::string pwd=request->pwd();

        //实现本地业务，远端的调用
        bool login_result=Login(name,pwd);

        //写入响应、返回值、错误参数、错误消息
        fixbug::ResultCode*code=response->mutable_result();
        code->set_errcode(0);//错误码
        code->set_errmsg("");//错误消息
        response->set_sucess(login_result);//返回值

        //执行回调
        done->Run();
    }
    void Register(google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
         //1：获取到函数的参数，做本地业务
        uint32_t id=request->id();
        std::string name=request->name();
        std::string pwd=request->pwd();

        bool register_result=Register(id,name,pwd);

        fixbug::ResultCode*code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(register_result);

        done->Run();
    }
};

int main(int argc,char**argv)
{
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    //provider是一个rpc网络服务对象
    //把UserService对象发布到网络结点上
    PrcProvider provider;
    provider.NotifyService(new UserService());

    //启动rpc服务的发布节点，Run以后进入阻塞状态，等待远程rpc请求
    provider.Run();
}