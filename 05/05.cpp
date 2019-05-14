#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

const static size_t N = 10;

mutex m1;
mutex m2;

void ping()
{
    for(size_t i = 0; i < N; i++){
        m1.lock();
        cout << "ping" << endl;
        m2.unlock();
    }
}

void pong()
{
    for(size_t i = 0; i < N; i++){
        m2.lock();
        cout << "pong" << endl;
        m1.unlock();
    }
}

int main()
{

	m2.lock();
    thread th1(ping);
    thread th2(pong);

    th1.join();
    th2.join();

    return 0;
}
