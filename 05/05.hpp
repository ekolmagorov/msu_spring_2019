#include <iostream>
#include <mutex>
#include <thread>

const static unsigned int N = 10;

std::mutex m1;
std::mutex m2;

void ping()
{
    for(size_t i = 0; i < N; i++)
    {
        m2.lock();
        std::cout<<"ping"<<std::endl;
        m1.unlock();
    }
}

void pong()
{
    for(size_t i = 0; i < N; i++)
    {
        m1.lock();
        std::cout<<"pong"<<std::endl;
        m2.unlock();
    }
}

int main()
{
    std::thread th1(ping);
    std::thread th2(pong);
    m1.unlock();
    m2.unlock();
    th1.join();
    th2.join();
    return 0;
}
