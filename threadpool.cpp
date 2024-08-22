#include "threadpool.h"

Threadpool::Threadpool()
{
    m_bflag = true;
    m_hSemaphore = NULL;
    m_lCreateThreadNum = 0;  //创建的线程个数
    m_lRunThreadNum  =0;     //运行的线程个数
    m_lMaxThreadNum = 0;      //最大的线程个数
    m_hMutex = NULL;
}

Threadpool::~Threadpool()
{

}

bool Threadpool::createThreadpool(long lMinThreadNum, long lMaxThreadNum)
{
    //1.校验参数
    if(lMinThreadNum <=0 || lMaxThreadNum < lMinThreadNum){
       // qDebug()<<"createthreadpool err";
        return false;
    }
    //初始化线程资源
    m_hSemaphore = CreateSemaphore(0,0,MAXITASK,0);
    m_hMutex = CreateMutex(0,0,0);
     //2.创建线程
    for(long i =0; i < lMinThreadNum;i++){

        HANDLE hThread = CreateThread(0,0,&threadProc,this,0,0);
        if(hThread){
            m_lstThread.push_back(hThread);
        }
    }
    m_lCreateThreadNum = lMinThreadNum;
    m_lMaxThreadNum = lMaxThreadNum;
    return true;
}
bool Threadpool::push(task *ptask)
{
    if(!ptask)return false;

    //将任务加入队列
    WaitForSingleObject(m_hMutex,INFINITE);
    m_qtask.push(ptask);
    ReleaseMutex(m_hMutex);
    //1.有空闲线程  ---释放信号量
    if(m_lRunThreadNum == m_lCreateThreadNum && m_lCreateThreadNum < m_lMaxThreadNum ){
          //2.没有空闲线程，但是没有达到上限--创建线程-释放信号量
         HANDLE hThread = CreateThread(0,0,&threadProc,this,0,0);
         if(hThread){
            m_lstThread.push_back(hThread);
         }
         m_lCreateThreadNum++;

    }
      //3.爆满
     //释放信号量
     ReleaseSemaphore(m_hSemaphore,1,0);
    return true;

}

DWORD Threadpool::threadProc(LPVOID lpvoid)
{
    Threadpool *pthis = (Threadpool*)lpvoid;
    task *ptask = NULL;
    while(pthis->m_bflag)
    {
        //等信号
        WaitForSingleObject(pthis->m_hSemaphore,INFINITE);
         //从阻塞态---运行态
        // pthis->m_lRunThreadNum++;
        InterlockedIncrement(&pthis->m_lRunThreadNum);
        //从队列中取任务
        if(!pthis->m_qtask.empty()){

            WaitForSingleObject(pthis->m_hMutex,INFINITE);
            if(pthis->m_qtask.empty()){
                ReleaseMutex(pthis->m_hMutex);
                break;
            }
            ptask = pthis->m_qtask.front(); //获取队列的头
            pthis->m_qtask.pop(); //从队列中移除
            ReleaseMutex(pthis->m_hMutex);

            ptask->run(); //执行子类的任务

            delete ptask;
            ptask = NULL;
        }
        //运行态--阻塞态

        InterlockedDecrement(&pthis->m_lRunThreadNum);
    }
    return 0;
}

void Threadpool::destroyThreadpool()
{
    m_bflag  =false;
    auto ite = m_lstThread.begin();
    while(ite != m_lstThread.end())
    {
        //判断线程是否退出---无信号代表线程存在
        if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
            TerminateThread(*ite,-1);

        CloseHandle(*ite);
        *ite = NULL;
        ite++;
    }
    m_lstThread.clear();

    if(m_hSemaphore){
        CloseHandle(m_hSemaphore);
        m_hSemaphore = NULL;
    }
    if(m_hMutex){
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
    }
}


