#include"mprpcapplication.h"
#include<iostream>
#include<unistd.h>
#include<string>

//初始化静态成员变量，配置文件类型的成员
MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

//初始化框架
void MprpcApplication::Init(int argc, char **argv)
{
    if(argc<=2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file=optarg;
            //将 -i 选项后面跟随的参数
            //（即配置文件的路径）赋值给 config_file 变量
            break;
        case '?':
        //遇到了一个未知的选项字符
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
        //表示一个需要参数的选项后面没有跟随参数
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件 rpcserver_ip rpcserver_port 
    // zookeeperip    zookeeperport
    m_config.LoadConfigFile(config_file.c_str());

    /*std::cout<<"rpcserverip: "<<m_config.Load("rpcserverip")<<std::endl;
    std::cout<<"rpcserverport: "<<m_config.Load("rpcserverport")<<std::endl;
    std::cout<<"zookeeperip: "<<m_config.Load("zookeeperip")<<std::endl;
    std::cout<<"zookeeperport: "<<m_config.Load("zookeeperport")<<std::endl;*/
}

//获取框架的实例对象
MprpcApplication &MprpcApplication ::GetInstance()
{
    static MprpcApplication app;
    return app;
}

//获取框架的配置文件信息的实例
MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}