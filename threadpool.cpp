#include "threadpool.h"

Threadpool::Threadpool()
{
    m_bflag = true;
    m_hSemaphore = NULL;
    m_lCreateThreadNum = 0;  //�������̸߳���
    m_lRunThreadNum  =0;     //���е��̸߳���
    m_lMaxThreadNum = 0;      //�����̸߳���
    m_hMutex = NULL;
}

Threadpool::~Threadpool()
{

}

bool Threadpool::createThreadpool(long lMinThreadNum, long lMaxThreadNum)
{
    //1.У�����
    if(lMinThreadNum <=0 || lMaxThreadNum < lMinThreadNum){
       // qDebug()<<"createthreadpool err";
        return false;
    }
    //��ʼ���߳���Դ
    m_hSemaphore = CreateSemaphore(0,0,MAXITASK,0);
    m_hMutex = CreateMutex(0,0,0);
     //2.�����߳�
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

    //������������
    WaitForSingleObject(m_hMutex,INFINITE);
    m_qtask.push(ptask);
    ReleaseMutex(m_hMutex);
    //1.�п����߳�  ---�ͷ��ź���
    if(m_lRunThreadNum == m_lCreateThreadNum && m_lCreateThreadNum < m_lMaxThreadNum ){
          //2.û�п����̣߳�����û�дﵽ����--�����߳�-�ͷ��ź���
         HANDLE hThread = CreateThread(0,0,&threadProc,this,0,0);
         if(hThread){
            m_lstThread.push_back(hThread);
         }
         m_lCreateThreadNum++;

    }
      //3.����
     //�ͷ��ź���
     ReleaseSemaphore(m_hSemaphore,1,0);
    return true;

}

DWORD Threadpool::threadProc(LPVOID lpvoid)
{
    Threadpool *pthis = (Threadpool*)lpvoid;
    task *ptask = NULL;
    while(pthis->m_bflag)
    {
        //���ź�
        WaitForSingleObject(pthis->m_hSemaphore,INFINITE);
         //������̬---����̬
        // pthis->m_lRunThreadNum++;
        InterlockedIncrement(&pthis->m_lRunThreadNum);
        //�Ӷ�����ȡ����
        if(!pthis->m_qtask.empty()){

            WaitForSingleObject(pthis->m_hMutex,INFINITE);
            if(pthis->m_qtask.empty()){
                ReleaseMutex(pthis->m_hMutex);
                break;
            }
            ptask = pthis->m_qtask.front(); //��ȡ���е�ͷ
            pthis->m_qtask.pop(); //�Ӷ������Ƴ�
            ReleaseMutex(pthis->m_hMutex);

            ptask->run(); //ִ�����������

            delete ptask;
            ptask = NULL;
        }
        //����̬--����̬

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
        //�ж��߳��Ƿ��˳�---���źŴ����̴߳���
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


