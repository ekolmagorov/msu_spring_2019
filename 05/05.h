#include <iostream>
#include <mutex>
#include <thread>

#define N 10000

std::mutex m1;
std::mutex m2;

void ping()
{
    for(int i = 0; i < N; i++)
    {
        m1.lock();
        std::cout<<"ping\n";
        m2.unlock();
    }
}

void pong()
{
    for(int i = 0; i < N; i++)
    {
        m2.lock();
        std::cout<<"pong\n";
        m1.unlock();
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
