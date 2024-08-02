#pragma once
#include"mprpcchannel.h"
#include"mprpccontroller.h"
#include"mprpcconfig.h"
//mprpc 基础类，实现框架的初始化
class MprpcApplication{
    public:
    static void Init(int argc,char**argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();

    private:
    static MprpcConfig m_config;
    MprpcApplication(){}//单例模式
    MprpcApplication(MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
};