#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <windows.h>
#include <queue>
#include <list>
//#include <QDebug>
using namespace std;
#define MAXITASK   100000
class task
{
public:
    task()
    { }
    virtual ~task()
    { }
public:
    virtual void run() = 0;
};

class Threadpool
{
public:
    Threadpool();
    ~Threadpool();
public:
    bool createThreadpool(long lMinThreadNum,long lMaxThreadNum);
    static DWORD WINAPI threadProc(LPVOID lpvoid);
    void destroyThreadpool();
    bool push(task*);
private:
    queue<task*> m_qtask;
    list<HANDLE> m_lstThread;
    bool   m_bflag;
    HANDLE m_hSemaphore;
    long   m_lCreateThreadNum;
    long   m_lRunThreadNum;
    long   m_lMaxThreadNum;
    HANDLE m_hMutex;

};

#endif // THREADPOOL_H
