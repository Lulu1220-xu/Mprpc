#include"mprpcchannel.h"
#include"rpcheader.pb.h"
#include<errno.h>
#include<sys/socket.h>
#include<string>
#include<arpa/inet.h>
#include"mprpcapplication.h"
#include<sys/types.h>
#include <netinet/in.h>
#include<mprpccontroller.h>
#include"zookeeperutil.h"
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done)
{
    //header_size(二进制)+header_str(service_name method_name args_size)+args_str
    const google::protobuf::ServiceDescriptor *sd=method->service();
    std::string service_name=sd->name();
    std:: string method_name=method->name();

    //获取参数的序列化长度：args_size
    uint32_t args_size=0;
    std::string args_str;
    if(request->SerializeToString(&args_str))//序列化到args_str 中
    {
        args_size=args_str.size();
    }
    else
    {
       controller->SetFailed("serialize request error1!");
        return ;
    }

    //定义rpc header
    mprpc::Rpcheader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size=0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str))//序列化
    {
        header_size=rpc_header_str.size();//获取对头长度
    }
    else
    {
        controller->SetFailed("serialize rpc_header_str error2!");
        return ;
    }
    //组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4));
    send_rpc_str+=rpc_header_str;
    send_rpc_str+=args_str;

    //打印
    std::cout<<"________________________________________________"<<std::endl;
    std::cout<<"head_size: "<<header_size<<std::endl;
    std::cout<<"rpc_header_str: "<<rpc_header_str<<std::endl;
    std::cout<<"service_name: "<<service_name<<std::endl;
    std::cout<<"method_name: "<<method_name<<std::endl;
    std::cout<<"args_str: "<<args_str<<std::endl;
    std::cout<<"================================================"<<std::endl<<std::endl;


    //发送：无并发要求 
    int clientfd=socket(AF_INET,SOCK_STREAM,0);
    if(clientfd==-1)
    {
        char errtxt[512]={0};
        sprintf(errtxt,"create socket error3 ! errno: %d",errno);
        controller->SetFailed(errtxt);
        return;
    }
    // std::string ip =MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port=atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());//c_str string ->char*  atoi char*->int  
    //不再从配置的文件map表查询
    ZkClient zkCli;
    zkCli.Star();//建立与zk服务器的连接
    //查询方法的data信息  :
    //先构造方法路径
    //UserService / login
    std::string method_path="/"+service_name+"/"+method_name;
    std::string host_data=zkCli.GetData(method_path.c_str());
    if(host_data=="")
    {
        controller->SetFailed(method_path+"is not exist!");
        return;
    }
    int idx=host_data.find(":");
    if(idx==-1)
    {
        controller->SetFailed(method_path+"address is invalid!");
        return;
    }
    std::string ip=host_data.substr(0,idx);
    // u_int16_t port=atoi((host_data.substr(idx,host_data.size()-idx)).c_str());
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 

    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    server_addr.sin_addr.s_addr=inet_addr(ip.c_str());
    
    //连接服务节点
    if(-1==connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        char txt[512]={0};
        sprintf(txt,"connect error4 !errrno: %d",errno);
        controller->SetFailed(txt);
        close(clientfd);
        return;
    }
    //send 发出rpc请求//bug send 后服务器就断掉了 未执行方法调用
    if(-1==send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0))
    {
        char txt[512]={0};
        sprintf(txt,"send error5 !errrno:%d ",errno);
        controller->SetFailed(txt);
        close(clientfd);
        return ;
    }
    //receve 接收response 返回值
    char recv_buf[1024]={0};
    int recv_size=0;
    if(-1==(recv_size=recv(clientfd,recv_buf,1024,0)))//bug:recv=0
    {
        char txt[512]={0};
        sprintf(txt,"recv error6 !errrno: %d",errno);
        controller->SetFailed(txt);
        close(clientfd);
        return ;
    }
    //std::string response_str(recv_buf,0,recv_size);//bug: 接收
    //if(!response->ParseFromString(response_str))
    if(!response->ParseFromArray(recv_buf,recv_size))
    {
        char txt[1200]={0};
        snprintf(txt, sizeof(txt), "parse error7! response_str: %s", recv_buf);
        controller->SetFailed(txt);
        close(clientfd);
        return ;
    }
    close(clientfd);

    
}