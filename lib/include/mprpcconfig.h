#pragma once

#include <unordered_map>
#include <string>
//读取配置文件
//rpcserver_ip rpcserver_port 
//zookeeper_ip zookeeper_port 
class MprpcConfig
{
    public:
    //负责解析加载的配置文件
    void LoadConfigFile(const char*config_file);
    //查询配置文件信息
    std::string Load(const std::string & key);
    //去除空格
    void Trim(std::string &buf);
    private:
    std::unordered_map<std::string,std::string>m_configMap;
};
