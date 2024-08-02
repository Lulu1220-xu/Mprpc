#include"mprpccontroller.h"


  Mprpccontroller::Mprpccontroller()//初始化
  {
    m_errText="";
    m_failed=false;
  }

  void Mprpccontroller::Reset()//重置
  {
    m_errText="";
    m_failed=false;
  }

  bool Mprpccontroller::Failed() const//查看状态
  {
    return m_failed;
  }

  std::string Mprpccontroller::ErrorText() const//错误信息
  {
    return m_errText;
  }

  void Mprpccontroller::SetFailed(const std::string& reason) //设置错误
  {
    m_failed=true;//true 为产生了错误
    m_errText=reason; 
  }


//目前未实现的具体功能：
  void Mprpccontroller::StartCancel(){};

  bool Mprpccontroller::IsCanceled() const{return 0;}

  void Mprpccontroller::NotifyOnCancel(google::protobuf::Closure* callback){};