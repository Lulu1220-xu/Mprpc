#pragma once
#include<google/protobuf/service.h>
#include<google/protobuf/descriptor.h>
#include<google/protobuf/message.h>

class MprpcChannel :public google::protobuf::RpcChannel{
    public:
    //所有stub执行的Rpc服务均通过此类的成员函数：序列化、网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done);
};