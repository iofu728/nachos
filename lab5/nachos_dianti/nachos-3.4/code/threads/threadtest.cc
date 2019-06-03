// threadtest.cc
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield,
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "elevatortest.h"
#include "synch.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
//----------------------------------------------------------------------

void SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++)
    {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
//  Set up a ping-pong between two threads, by forking a thread
//  to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, (void *)1);
    SimpleThread(0);
}

//----------lab1-Test-Begin--------------------------------------------

void Lab1Thread()
{
    for (int i = 0; i <= 3; ++i)
    {
        printf("threadname: %s tid: %d uid: %d  looped %d times\n",
               currentThread->getName(), currentThread->getTid(),
               currentThread->getUid(), i);
        currentThread->Yield();
    }
}

Thread *createThreadTest(int num, char *threadNameList)
{
    Thread *temp = new Thread(threadNameList);
    temp->setUid(num);
    temp->Fork(Lab1Thread, (void *)1);
    return temp;
}

void PrintThreadInfo()
{
    printf("threadname-----tid-----uid-----status\n");
    for (int i = 0; i < MaxThreadNum; ++i)
    {
        if (thread[i] != NULL)
        {
            thread[i]->Print();
        }
    }
}

void Lab1Test()
{
    printf("Write by Jiang Huiqiang 1801210840\n");
    char threadNameList[MaxThreadNum][20] = {};

    for (int i = 0; i < MaxThreadNum - 1; ++i)
    {
        char str[20];
        sprintf(str, "%d", i);
        strcat(threadNameList[i], "Thread");
        strcat(threadNameList[i], str);
        createThreadTest(i, threadNameList[i]);
    }
    Lab1Thread();
}

void TestThreadNumExceed()
{
    for (int i = 1; i <= 128; ++i)
    {
        Thread *t = new Thread("testThread");
        printf("creat thread %d\n", i);
    }
}

//----------lab1-Test-End-----------------------------------------------

//----------lab2-Test-Begin---------------------------------------------

void Lab2Thread(int n)
{
    for (int i = 0; i <= n; ++i)
    {
        printf("threadname: %s tid: %d uid: %d priority: %d looped %d times\n",
               currentThread->getName(), currentThread->getTid(),
               currentThread->getUid(), currentThread->getPriority(), i);
        currentThread->Yield();
    }
}

void Challenge1Thread(int n)
{
    for (int i = 1; i <= n; ++i)
    {
        printf("threadname: %s tid: %d uid: %d priority: %d looped %d times\n",
               currentThread->getName(), currentThread->getTid(),
               currentThread->getUid(), currentThread->getPriority(), i);
        // interrupt->SetLevel(IntOn);
        currentThread->Yield();
        // interrupt->SetLevel(IntOff);
    }
}

Thread *createThreadLab2Test(int num, int priority, char *threadNameList,
                             int loop, int type)
{
    Thread *temp = new Thread(threadNameList);
    temp->setUid(num);
    temp->setPriority(priority);
    if (type)
    {
        temp->Fork(Lab2Thread, (void *)loop);
    }
    else
    {
        temp->Fork(Challenge1Thread, (void *)loop);
    }

    return temp;
}

void Lab2Test()
{
    printf("Write by Jiang Huiqiang 1801210840 in 2019-03-08\n");
    char threadNameList[MaxThreadNum][20] = {};
    int priorityList[MaxThreadNum] = {3, 1, 5, 4};

    for (int i = 0; i < 4; ++i)
    {
        char str[20];
        sprintf(str, "%d", i);
        strcat(threadNameList[i], "Thread");
        strcat(threadNameList[i], str);
        createThreadLab2Test(i, priorityList[i], threadNameList[i], 3, 1);
    }

    Lab2Thread(3);
}

void Challenge1Test()
{
    printf("Write by Jiang Huiqiang 1801210840 in 2019-03-08\n");
    char threadNameList[MaxThreadNum][20] = {};
    int priorityList[MaxThreadNum] = {3, 3, 5, 4};

    for (int i = 0; i < 4; ++i)
    {
        char str[20];
        sprintf(str, "%d", i);
        strcat(threadNameList[i], "Thread");
        strcat(threadNameList[i], str);
        createThreadLab2Test(i, priorityList[i], threadNameList[i], 10, 0);
    }
    Challenge1Thread(10);
}

//----------lab2-Test-End-----------------------------------------------

//----------lab3-Test-Begin---------------------------------------------
#define Buffersize 4
Semaphore *Mutex = new Semaphore("Mutex", 1);
Semaphore *Empty = new Semaphore("Empty", Buffersize);
Semaphore *Full = new Semaphore("Full", 0);
int itemnum = 0;

Lock *CLock = new Lock("Condition Lock");
Condition *ConditionFull = new Condition("Condition Full");
Condition *ConditionEmpty = new Condition("Condition Empty");

Barrier *barrier = new Barrier("barrier", 5);

ReadWrite *rwLock = new ReadWrite("Read Write");

void Producer(int n)
{
    for (int i = 0; i < n; ++i)
    {
        Empty->P();
        Mutex->P();
        ++itemnum;
        printf(
            "Thread %s tid: %d uid: %d priority: %d, PPPProduce the "
            "item, there are %d items in buffer\n",
            currentThread->getName(), currentThread->getTid(),
            currentThread->getUid(), currentThread->getPriority(), itemnum);
        Mutex->V();
        Full->V();
        currentThread->Yield();
    }
}

void Comsumer(int n)
{
    for (int i = 0; i < n; ++i)
    {
        Full->P();
        Mutex->P();
        --itemnum;
        printf(
            "Thread %s tid: %d uid: %d priority: %d, CCCComsumer the "
            "item, there are %d items in buffer\n",
            currentThread->getName(), currentThread->getTid(),
            currentThread->getUid(), currentThread->getPriority(), itemnum);
        Mutex->V();
        Empty->V();
        currentThread->Yield();
    }
}

void ProducerCondition(int n)
{
    for (int i = 0; i < n; ++i)
    {
        CLock->Acquire();
        if (itemnum == Buffersize)
        {
            printf("-----------------Thread %s Waiting-----------------%s\n",
                   currentThread->getName(), ConditionFull->getName());
            ConditionFull->Wait(CLock);
        }
        ++itemnum;
        printf(
            "Thread %s tid: %d uid: %d priority: %d, PPPProduceCondition the "
            "item, there are %d items in buffer\n",
            currentThread->getName(), currentThread->getTid(),
            currentThread->getUid(), currentThread->getPriority(), itemnum);
        if (!itemnum)
        {
            printf("-----------------Thread %s Wakeup-----------------%s\n",
                   currentThread->getName(), ConditionEmpty->getName());
            ConditionEmpty->Signal(CLock);
        }
        CLock->Release();
        currentThread->Yield();
    }
}

void ComsumerCondiction(int n)
{
    for (int i = 0; i < n; ++i)
    {
        CLock->Acquire();
        if (!itemnum)
        {
            printf("-----------------Thread %s Waiting-----------------%s\n",
                   currentThread->getName(), ConditionEmpty->getName());
            ConditionEmpty->Wait(CLock);
        }
        --itemnum;

        printf(
            "Thread %s tid: %d uid: %d priority: %d, CCCComsumerCondition the "
            "item, there are %d items in buffer\n",
            currentThread->getName(), currentThread->getTid(),
            currentThread->getUid(), currentThread->getPriority(), itemnum);

        if (itemnum == Buffersize)
        {
            printf("-----------------Thread %s Wakeup-----------------%s\n",
                   currentThread->getName(), ConditionFull->getName());
            ConditionFull->Signal(CLock);
        }
        CLock->Release();
        currentThread->Yield();
    }
}

void BarrierTestThread() { barrier->setBarrier(); }

void Reader(int n)
{
    rwLock->ReadAcquire();
    for (int i = 0; i < n; ++i)
        printf("***** Thread %s **** Read \n", currentThread->getName());
    rwLock->ReadRelease();
}

void Writer(int n)
{
    rwLock->WriteAcquire();
    for (int i = 0; i < n; ++i)
        printf("***** Thread %s **** Write \n", currentThread->getName());
    rwLock->WriteRelease();
}

Thread *createThreadLab3Test(int num, int priority, char *threadNameList,
                             int loop, int type, int method)
{
    Thread *temp = new Thread(threadNameList);
    temp->setUid(num);
    temp->setPriority(priority);

    if (!method && type)
    {
        temp->Fork(Producer, (void *)loop);
    }
    else if (!method)
    {
        temp->Fork(Comsumer, (void *)loop);
    }
    else if (method == 1 && type)
    {
        temp->Fork(ProducerCondition, (void *)loop);
    }
    else if (method == 1)
    {
        temp->Fork(ComsumerCondiction, (void *)loop);
    }
    else if (method == 2)
    {
        temp->Fork(BarrierTestThread, (void *)loop);
    }
    else if (method == 3 && type)
    {
        temp->Fork(Reader, (void *)loop);
    }
    else
    {
        temp->Fork(Writer, (void *)loop);
    }

    return temp;
}

void Lab3Test()
{
    printf("Write by Jiang Huiqiang 1801210840 in 2019-03-17\n");
    char threadNameList[MaxThreadNum][20] = {};
    int priorityList[MaxThreadNum] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    int readWriteList[MaxThreadNum] = {1, 1, 0, 1, 0, 1, 0, 1, 1, 0};
    int model = 3;

    if (model == 2)
        printf("............ Two Phase Protocol begin ............\n");

    for (int i = 0; i < 10; ++i)
    {
        char str[20];
        sprintf(str, "%d", i);
        strcat(threadNameList[i], "Thread");
        strcat(threadNameList[i], str);
        if (model == 3)
            createThreadLab3Test(i, priorityList[i], threadNameList[i], 5,
                                 readWriteList[i], model);
        else
            createThreadLab3Test(i, priorityList[i], threadNameList[i], 5,
                                 i < 5 ? 1 : 0, model);
    }
    if (model == 2)
        printf("-------***------- Consumer Phase End -------***-------\n");
}
//----------lab3-Test-End-----------------------------------------------

//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest()
{
    switch (testnum)
    {
    case 1:
        Lab3Test();
        break;
    default:
        printf("No test specified.\n");
        break;
    }
}
