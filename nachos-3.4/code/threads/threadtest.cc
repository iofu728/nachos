// threadtest.cc
//  Simple test case for the threads assignment.
//
//  Create two threads, and have them context switch
//  back and forth between themselves by calling Thread::Yield,
//  to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "elevatortest.h"
#include "system.h"

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

void SimpleThread(int which) {
  int num;

  for (num = 0; num < 5; num++) {
    printf("*** thread %d looped %d times\n", which, num);
    currentThread->Yield();
  }
}

//----------------------------------------------------------------------
// ThreadTest1
//  Set up a ping-pong between two threads, by forking a thread
//  to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void ThreadTest1() {
  DEBUG('t', "Entering ThreadTest1");

  Thread *t = new Thread("forked thread");

  t->Fork(SimpleThread, (void *)1);
  SimpleThread(0);
}

//----------lab1-Test-Begin--------------------------------------------

void Lab1Thread() {
  for (int i = 0; i <= 3; ++i) {
    printf("threadname: %s tid: %d uid: %d  looped %d times\n",
           currentThread->getName(), currentThread->getTid(),
           currentThread->getUid(), i);
    currentThread->Yield();
  }
}

Thread *createThreadTest(int num, char *threadNameList) {
  Thread *temp = new Thread(threadNameList);
  temp->setUid(num);
  temp->Fork(Lab1Thread, (void *)1);
  return temp;
}

void PrintThreadInfo() {
  printf("threadname-----tid-----uid-----status\n");
  for (int i = 0; i < MaxThreadNum; ++i) {
    if (thread[i] != NULL) {
      thread[i]->Print();
    }
  }
}

void Lab1Test() {
  printf("Write by Jiang Huiqiang 1801210840\n");
  char threadNameList[MaxThreadNum][20] = {};

  for (int i = 0; i < MaxThreadNum - 1; ++i) {
    char str[20];
    sprintf(str, "%d", i);
    strcat(threadNameList[i], "Thread");
    strcat(threadNameList[i], str);
    createThreadTest(i, threadNameList[i]);
  }
  Lab1Thread();
}

void TestThreadNumExceed() {
  for (int i = 1; i <= 128; ++i) {
    Thread *t = new Thread("testThread");
    printf("creat thread %d\n", i);
  }
}

//----------lab1-Test-End-----------------------------------------------

//----------lab2-Test-Begin---------------------------------------------

void Lab2Thread(int n) {
  for (int i = 0; i <= n; ++i) {
    printf("threadname: %s tid: %d uid: %d priority: %d looped %d times\n",
           currentThread->getName(), currentThread->getTid(),
           currentThread->getUid(), currentThread->getPriority(), i);
    currentThread->Yield();
  }
}

void Challenge1Thread(int n) {
  for (int i = 1; i <= n; ++i) {
    printf("threadname: %s tid: %d uid: %d priority: %d looped %d times\n",
           currentThread->getName(), currentThread->getTid(),
           currentThread->getUid(), currentThread->getPriority(), i);
    // interrupt->SetLevel(IntOn);
    currentThread->Yield();
    // interrupt->SetLevel(IntOff);
  }
}

Thread *createThreadLab2Test(int num, int priority, char *threadNameList,
                             int loop, int type) {
  Thread *temp = new Thread(threadNameList);
  temp->setUid(num);
  temp->setPriority(priority);
  if (type) {
    temp->Fork(Lab2Thread, (void *)loop);
  } else {
    temp->Fork(Challenge1Thread, (void *)loop);
  }

  return temp;
}

void Lab2Test() {
  printf("Write by Jiang Huiqiang 1801210840 in 2019-03-08\n");
  char threadNameList[MaxThreadNum][20] = {};
  int priorityList[MaxThreadNum] = {3, 1, 5, 4};

  for (int i = 0; i < 4; ++i) {
    char str[20];
    sprintf(str, "%d", i);
    strcat(threadNameList[i], "Thread");
    strcat(threadNameList[i], str);
    createThreadLab2Test(i, priorityList[i], threadNameList[i], 3, 1);
  }

  Lab2Thread(3);
}

void Challenge1Test() {
  printf("Write by Jiang Huiqiang 1801210840 in 2019-03-08\n");
  char threadNameList[MaxThreadNum][20] = {};
  int priorityList[MaxThreadNum] = {3, 3, 5, 4};

  for (int i = 0; i < 4; ++i) {
    char str[20];
    sprintf(str, "%d", i);
    strcat(threadNameList[i], "Thread");
    strcat(threadNameList[i], str);
    createThreadLab2Test(i, priorityList[i], threadNameList[i], 10, 0);
  }
  Challenge1Thread(10);
}

//----------lab2-Test-End-----------------------------------------------

//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest() {
  switch (testnum) {
    case 1:
      Challenge1Test();
      break;
    default:
      printf("No test specified.\n");
      break;
  }
}
