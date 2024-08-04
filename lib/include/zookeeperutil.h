#pragma  once

#include<semaphore.h>
#include<zookeeper/zookeeper.h>
#include<string>

//封装zk客户端类
class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    //zkCli 启动连接zkServer
    void Star();
    //在zkServer 根据指定路径（/Servicename/functionname : ip+port)创建znode结点
    void Create (const char *path,const char*data ,int datalen,int state=0 );
    //根据参数指定路径参数、值
    std::string GetData(const char*path);
private:
    //客户端句柄
    zhandle_t *m_zhandle;
};