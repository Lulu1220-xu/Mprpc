#include"mprpcprovider.h"
#include"mprpcapplication.h"
#include"rpcheader.pb.h"
#include"logger.h"
#include"zookeeperutil.h"
// 外部使用，供框架发布rpc方法的函数接口
/*

struct Service {
        服务对象： service*
        map< name,  method 方法> 该服务对象的所有  方法集合
        }

map< service_name , Service类型信息 > 服务对象集合
*/
void PrcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info; // 创建服务类型的实例

    // 获取服务对象的描述
    const google::protobuf::ServiceDescriptor *pserviceDes = service->GetDescriptor();
    // 服务   对象的名字
    std::string service_name = pserviceDes->name();
    // 服务对象的所有方法数量
    int methodCnt = pserviceDes->method_count();
    for (int i = 0; i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDes->method(i); // 获取方法（描述）
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc}); // 将方法及方法名插入方法集合
    }
    service_info.m_service = service;                  // big bug
    m_serviceMap.insert({service_name, service_info}); // 将服务名，服务信息集合 插入所有服务的集合
}

// 启动rpc服务结点，开启提供rpc网络调用
void PrcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str()); // c_str string ->char*  atoi char*->int
    muduo::net::InetAddress address(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    // 绑定连接回调和消息读写回调方法，通过muduo实现网路与业务代码分离
    // 连接回调：

    // 绑定连接回调和消息读写回调方法  分离了网络代码和业务代码
    server.setConnectionCallback(std::bind(&PrcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&PrcProvider::OnMessage, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3));
    // 设置muduo 库的线程数量
    server.setThreadNum(4);

    // 把当前的服务注册到zk,caller 从zk实时获取当前的服务方法的ip:port
    //session timeout 默认30 s  ,zkCli 建立建立时创建的网络I/O线程，每间隔1/3timeout 时间发送一个心跳消息
    ZkClient zkCli;
    zkCli.Star(); // 建立于zk_sever的连接
    // zkCli 为永久性结点 不可删除 结点的服务方法为临时结点，由心跳机制验证是否删除
    for (auto &sp : m_serviceMap)
    {
        // service_name服务对象名 / UseServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0); // 创建服务对象结点
        for (auto &mp : sp.second.m_methodMap)
        {
            // service_name/method_name   /UserServiceRpc/Login 结点存储data数据：ip:port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL 表示znode为临时结点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    //四个线程，一个i/o线程，三个工作线程
    LOG_INFO("RpcProvider start, ip : %s , port: %d",ip.c_str(),port);
    std::cout<<"RpcProvider start, ip::"<<ip<<" ,port: "<<port<<std::endl;
    //开启服务：
    server.start();
    m_eventLoop.loop();//开启epoll wait 等待接收
    //std::cout<<"Server Rpc 结点开启成功！"<<std::endl;
}

//连接的方法
void PrcProvider::OnConnection(const muduo::net::TcpConnectionPtr &con)
{
    if(!con->connected())
    {
        
        std::cout<<"client断开了"<<std::endl;
        con->shutdown();
    }
}
//读写方法：
/*
在RpcProvide 和 Rpcconsumer 协商通信的 protobuf 数据类型、结构
server_name 、method_name、 args ,为所需要的重要参数
但是由于TCP 连接为面向字节流的流式传输，以及粘包问题
我们定义头部 server_name 、method_name 的长度（占用4字节，二进制存储，（不用担心整数存储超范围，超字节））
以 header_size + header_str + arg_size 的形式传输

以string 的insert 和copy 可以有效控制读取和写入的字节长度
*/
void PrcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp receiveTime)
{
    //接收远程RPC 请求字符流 
    std::string recv_buf=buffer->retrieveAllAsString();
    //std::cout<<"接收远程RPC 请求字符流 "<<recv_buf<<std::endl;
    //读取前4个字节的首部字段内容
    uint32_t header_size=0;
    recv_buf.copy((char*)&header_size,4,0);//bug1

    //根据header_size读取的字节流，反序列化得到rpc详细信息
    std::string rpc_header_str=recv_buf.substr(4,header_size);
    mprpc::Rpcheader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    std::string args_str;
    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        //头部反序列化成功
        service_name=rpcHeader.service_name();
        method_name=rpcHeader.method_name();
        args_size=rpcHeader.args_size();
        args_str=recv_buf.substr(4+header_size,args_size);//未反序列化
    }
    else
    {
        //头部反序列化失败
        LOG_ERROR("rpc_header_str:%s ,parse error!",rpc_header_str.c_str());
       
        return;
    }
    //std::cout<<"头部反序列化成功"<<std::endl;
    //打印
    std::cout<<"________________________________________________"<<std::endl;
    std::cout<<"head_size:"<<header_size<<std::endl;
    std::cout<<"rpc_header_str:"<<rpc_header_str<<std::endl;
    std::cout<<"service_name:"<<service_name<<std::endl;
    std::cout<<"method_name:"<<method_name<<std::endl;
    std::cout<<"args_str:"<<args_str<<std::endl;
    std::cout<<"________________________________________________"<<std::endl;
    
//获取service 对象以及 method 方法

    auto it = m_serviceMap.find(service_name);

    if (it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        return;
    }

    google::protobuf::Service* service=it->second.m_service;//对象
    const google::protobuf::MethodDescriptor*method=mit->second;//方法

    //生成rpc 请求的request response
    google::protobuf::Message*request=service->GetRequestPrototype(method).New();
   
    if(!request->ParseFromString(args_str))
    {
        //std::cout<<<<std::endl;
        LOG_ERROR(" request parse error! content is: %s",args_str.c_str());
        return ;
    }

    google::protobuf::Message*response=service->GetResponsePrototype(method).New();
    //std::cout<<"method 方法的调用，绑定Closure 回调"<<std::endl;
    google::protobuf::Closure *done = google::protobuf::NewCallback<PrcProvider, 
                                                                    const muduo::net::TcpConnectionPtr&, 
                                                                    google::protobuf::Message*>
                                                                    (this, 
                                                                    &PrcProvider::SendRpcResponse, 
                                                                    conn, response);
    //std::cout<<"框架远端调用Rpc 请求执行当前结点发布（重写）的方法";
    service->CallMethod(method,nullptr,request,response,done);
}

    //重写的回调函数 ，用于序列化 rpc 响应 和网络发送
void PrcProvider:: SendRpcResponse(const muduo::net::TcpConnectionPtr & conn,google::protobuf::Message* response)
{
   std::string response_str;
    if (response->SerializeToString(&response_str)) // response进行序列化
    {
        // 序列化成功后，通过网络把rpc方法执行的结果发送会rpc的调用方
        conn->send(response_str);
    }
    else
    {
        LOG_ERROR( "serialize response_str error!" );
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}