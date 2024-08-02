#include "mprpcconfig.h"
#include<iostream>
#include"logger.h"
// 负责解析加载的配置文件
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE* pf =fopen(config_file,"r");
    if(nullptr==pf)
    {
        LOG_ERROR("%s is not exist!",config_file);
        std::cout<<config_file<<"is not exist!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[512]={0};
        fgets(buf,512,pf);

        //去掉多余空格
        std::string read_buf(buf);
        Trim(read_buf);

        //判断# 注释,或者去掉空格后为空
        if(read_buf[0]=='#'||read_buf.empty())
            continue;

        //解析配置内容
        int idx=read_buf.find('=');
        if(idx==-1)
        {
            //不合法配置
            continue;
        }

        std::string key;
        std::string val;
        key=read_buf.substr(0,idx);
        Trim(key);//去掉key 的空格

        //rpcserverip=127.0.0.1
        int endidx=read_buf.find('\n',idx);
        val=read_buf.substr(idx+1,endidx-idx-1);
        Trim(val);
        m_configMap.insert({key,val});
    }
    fclose(pf);
}
// 查询配置文件信息
std::string MprpcConfig::Load(const std::string &key)
{
    auto it=m_configMap.find(key);
    if(it==m_configMap.end())
        return "";
    return it->second;
}

//去除前后空格
void MprpcConfig::Trim(std::string &buf)
{
    //去前面的
    int idx=buf.find_first_not_of(' ');
    if(idx!=-1)
    {
        buf=buf.substr(idx,buf.size()-idx);
    }
    //去后面的
    idx=buf.find_last_not_of(' ');
    if(idx!=-1)
    {
        buf=buf.substr(0,idx+1);
    }
}