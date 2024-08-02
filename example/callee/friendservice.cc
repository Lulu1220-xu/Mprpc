#include <iostream>
#include <string>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include"logger.h"
class FriendService:public fixbug::FriendListRpc
{
    public:
    std::vector<std::string>GetFriendList(uint32_t id)
    {
        std::cout<<" do GetFriendList service:"<<std::endl;
        std::vector<std::string>v;
        v.push_back("lili");
        v.push_back("lulu");
        v.push_back("lala");
        return v;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListRespond* response,
                       ::google::protobuf::Closure* done)
    {
        uint32_t id=request->id();

        std::vector<std::string> list=GetFriendList(id);
        fixbug::ResultCode *res=response->mutable_result();
        res->set_errcode(0);
        res->set_errmsg("");
        for(auto it:list)
        {
            std::string*p=response->add_friends();
            *p=it;
        }
        done->Run();
    }                   
};

int main(int argc,char**argv)
{
    //调用框架的初始化操作
    MprpcApplication::Init(argc,argv);

    //provider是一个rpc网络服务对象
    //把Service对象发布到网络结点上
    PrcProvider provider;
    provider.NotifyService(new FriendService());

    //启动rpc服务的发布节点，Run以后进入阻塞状态，等待远程rpc请求
    provider.Run();
}