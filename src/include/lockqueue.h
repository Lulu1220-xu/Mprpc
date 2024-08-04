#pragma once
#include<queue>
#include<thread>
#include<mutex>//互斥锁
#include<condition_variable>//条件变量
#include"time.h"
#include<iostream>
//异步写入日志
template<typename T>
class LockQueue
{
    public:
    void Push(const T& data)
    {
        std::lock_guard<std::mutex>lock(m_mutex);//lock_guard上锁,生命周期在该大括号内，出了括号自动析构该对象，解锁
        m_queue.push(data);
        //std::cout<<"push"<<std::endl;
        //写入数据后，唤醒写出线程，将数据写入日志文件
        m_condvariable.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex>lock(m_mutex);
        //std::cout<<"pop"<<std::endl;
        while(m_queue.empty())
        {
            //消息队列空，线程进入wait 状态
            //std::cout<<"pop 阻塞"<<std::endl;
            m_condvariable.wait(lock);

        }
        //std::cout<<"pop 输出"<<std::endl;
        T data=m_queue.front();
        m_queue.pop();
        return data;
    }
    private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};