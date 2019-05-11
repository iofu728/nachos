// synch.cc
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

int nowReadListLen = 0;
int tempReadListLen = 0;

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore() { delete queue; }

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff); // disable interrupts

    while (value == 0)
    {                                         // semaphore not available
        queue->Append((void *)currentThread); // so go to sleep
        currentThread->Sleep();
    }
    value--; // semaphore available,
             // consume its value

    (void)interrupt->SetLevel(oldLevel); // re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL) // make thread ready, consuming the V immediately
        scheduler->ReadyToRun(thread);
    value++;
    (void)interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments
// Note -- without a correct implementation of Condition::Wait(),
// the test case in the network assignment won't work!

// ----------------------------------------------------------------------
// Lab 3 Lock
// ----------------------------------------------------------------------

Lock::Lock(char *debugName)
{
    name = debugName;
    mutex = new Semaphore(name, 1);
    thread = NULL;
}

Lock::~Lock() { delete mutex; }

void Lock::Acquire()
{
    mutex->P();
    thread = currentThread;
}

void Lock::Release()
{
    ASSERT(thread == currentThread);
    thread = NULL;
    mutex->V();
}

bool Lock::isHeldByCurrentThread() { return currentThread == thread; }

// ----------------------------------------------------------------------
// Lab 3 Condition
// ----------------------------------------------------------------------

Condition::Condition(char *debugName)
{
    name = debugName;
    waitQueue = new List();
    phase = new Lock("Two Phase Lock"); // lab 3 Challenge 1
}

Condition::~Condition()
{
    delete waitQueue;
    delete phase;
}

void Condition::Wait(Lock *conditionLock)
{
    // ASSERT(FALSE);
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    conditionLock->Release();
    waitQueue->Append((Thread *)currentThread);
    currentThread->Sleep();
    conditionLock->Acquire();
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock *conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    if (conditionLock->isHeldByCurrentThread())
    {
        thread = (Thread *)waitQueue->Remove();
        if (thread != NULL)
        {
            scheduler->ReadyToRun(thread);
        }
    }

    (void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock *conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    if (conditionLock->isHeldByCurrentThread())
    {
        thread = (Thread *)waitQueue->Remove();
        while (thread != NULL)
        {
            scheduler->ReadyToRun(thread);
            thread = (Thread *)waitQueue->Remove();
        }
    }
    (void)interrupt->SetLevel(oldLevel);
}

void Condition::BroadcastPhase(Lock *conditionLock)
{ // lab 3 Challenge 1
    IntStatus oldLevel = interrupt->SetLevel(IntOff);
    Thread *thread;
    nowReadListLen = scheduler->getReadyListLen();

    printf("Now ReadyList len is %d\n", nowReadListLen);
    printf("wait Queue len is%d\n", waitQueue->NumInList());
    if (conditionLock->isHeldByCurrentThread())
    {
        thread = (Thread *)waitQueue->Remove();
        while (thread != NULL)
        {
            thread->setPriority(1);
            scheduler->ReadyToRun(thread);
            thread = (Thread *)waitQueue->Remove();
        }
    }
    (void)interrupt->SetLevel(oldLevel);
    phase->Release();
}

void Condition::AcquirePhase() { phase->Acquire(); }

void Condition::ReleasePhase() { phase->Release(); }

// ----------------------------------------------------------------------
// Lab 3 Challenge 1 Barrier Two Phase Protocol
// ----------------------------------------------------------------------

Barrier::Barrier(char *debugName, int num)
{
    name = debugName;
    waitNum = num;
    totalNum = num;
    ASSERT(waitNum > 0); // waitNum must > 0
    bl = new Lock("Barrier Lock");
    bc = new Condition("Barrier Condition");
}

Barrier::~Barrier()
{
    delete bl;
    delete bc;
}

void Barrier::setBarrier()
{
    bl->Acquire();
    if (!waitNum)
    {
        bc->AcquirePhase();
        if (!waitNum)
        {
            waitNum = totalNum;
            printf("-------***------- Consumer Phase End -------***-------\n");
            printf("-------***------- Queue Phase Begin -------***-------\n");
        }
        bc->ReleasePhase();
    }
    printf("### Thread %s enter Barrier! ###\n", currentThread->getName());

    --waitNum;

    if (!waitNum)
    {
        bc->AcquirePhase();
        printf("-------***------- Queue Phase End -------***-------\n");
        printf("-------***------- Consumer Phase Begin -------***-------\n");
        bc->BroadcastPhase(bl);
    }
    else
    {
        bc->Wait(bl);
    }
    printf("### Thread %s quit Barrier! ###\n", currentThread->getName());
    bl->Release();
}

// ----------------------------------------------------------------------
// Lab 3 Challenge 2 ReadWrite Lock
// ----------------------------------------------------------------------

ReadWrite::ReadWrite(char *debugName)
{
    name = debugName;
    readNum = 0;
    mutex = new Lock("Mutex Lock");
    rlock = new Lock("Read Lock");
    wlock = new Lock("Write Lock");
}

ReadWrite::~ReadWrite()
{
    delete mutex;
    delete rlock;
    delete wlock;
}

void ReadWrite::ReadAcquire()
{
    rlock->Acquire();
    if (!readNum)
        mutex->Acquire();
    ++readNum;
    rlock->Release();
}

void ReadWrite::ReadRelease()
{
    rlock->Acquire();
    --readNum;
    if (!readNum)
        mutex->Release();
    rlock->Release();
}

void ReadWrite::WriteAcquire()
{
    wlock->Acquire();
    mutex->Acquire();
}

void ReadWrite::WriteRelease()
{
    mutex->Release();
    wlock->Release();
}
