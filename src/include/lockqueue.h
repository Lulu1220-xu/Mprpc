#pragma once
#include<queue>
#include<thread>
#include<mutex>//互斥锁
#include<condition_variable>//条件变量


//异步写入日志队列
template<typename T>
class LockQueue
{
    public:
    void Push(const T& data){
        std::lock_guard<std::mutex>lock(m_mutex);//lock_guard上锁,生命周期在该大括号内，出了括号自动析构该对象，解锁
        m_queue.push(data);
        //写入数据后，唤醒写出线程，将数据写入日志文件
        m_condvariable.notify_one();
    }
    T Pop()
    {
        std::unique_lock<std::mutex>lock(m_mutex);
        while(m_queue.empty())
        {
            //消息队列空，线程进入wait 状态
            m_condvariable.wait(lock);
        }
        T data=m_queue.front();
        m_queue.pop();
        return data;
    }
    private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
};