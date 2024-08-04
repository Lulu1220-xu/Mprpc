#pragma once
#include"google/protobuf/service.h"
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<google/protobuf/descriptor.h>
#include<unordered_map>
#include<string>
#include<functional>

//框架提供专门发布rpc服务的网络对象类
class PrcProvider{
    public:
    //外部使用，供框架发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service*service);
    //启动rpc服务结点，开启提供rpc网络调用
    void Run();



    private:
    //组合：EventLoop epoll
    muduo::net::EventLoop m_eventLoop;

    //service 服务 类型信息
    struct ServiceInfo
    {
        google::protobuf::Service* m_service;   //对象
        std::unordered_map<std::string ,const google::protobuf::MethodDescriptor*>m_methodMap; //服务  <名字,方法>
    };
    //存注册成功的服务对象以及服务方法的所有信息
    std::unordered_map<std::string ,ServiceInfo>m_serviceMap;
    
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr& ,muduo::net::Buffer* buffer,muduo::Timestamp receiveTime);

    //重写的回调函数 ，用于序列化 rpc 响应 和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);

};