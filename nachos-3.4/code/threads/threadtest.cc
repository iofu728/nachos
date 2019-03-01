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
#include "elevatortest.h"
#include "system.h"

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
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
// 	Set up a ping-pong between two threads, by forking a thread
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void ThreadTest1() {
  DEBUG('t', "Entering ThreadTest1");

  Thread *t = new Thread("forked thread");

  t->Fork(SimpleThread, (void *)1);
  SimpleThread(0);
}

//----------lab1-Test-Begin--------------------------------------------

void Lab1Thread(int someone) {
  printf("Write by Jiang Huiqiang 1801210840\n");
  for (int i = 0; i <= 6; ++i) {
    printf("threadname: %s tid: %d uid: %d  looped %d times\n",
           currentThread->getName(), currentThread->getTid(),
           currentThread->getUid(), i);
    currentThread->Yield();
  }
}

void Lab1Test() {
  Thread *t1 = new Thread("thread1");
  Thread *t2 = new Thread("thread2");
  Thread *t3 = new Thread("thread3");
  t1->setUid(1);
  t2->setUid(2);
  t3->setUid(3);
  t1->Fork(Lab1Thread, (void *)1);
  t2->Fork(Lab1Thread, (void *)1);
  t3->Fork(Lab1Thread, (void *)1);
  Lab1Thread(0);
}

void TestThreadNumExceed() {
  for (int i = 1; i <= 128; ++i) {
    Thread *t = new Thread("testThread");
    printf("creat thread %d\n", i);
  }
}

void PrintThreadInfo() {
  printf("threadname-----tid-----uid-----status\n");
  for (int i = 0; i < MaxThreadNum; ++i) {
    if (thread[i] != NULL) {
      thread[i]->Print();
    }
  }
}
//----------lab1-Test-End-----------------------------------------------

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest() {
  switch (testnum) {
    case 1:
      Lab1Test();
      break;
    default:
      printf("No test specified.\n");
      break;
  }
}
