// progtest.cc
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------

void StartProcess(char *filename)
{
    printf("\033[01;34m Write by Jiang Huiqiang 1801210840 in 2019-05-05 \033[0m\n");
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL)
    {
        printf("Unable to open file %s\n", filename);
        return;
    }
    space = new AddrSpace(executable);
    currentThread->space = space;

    delete executable; // close file

    space->InitRegisters(); // set the initial register values
    space->RestoreState();  // load page table register

    machine->Run(); // jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

//----------------------------------------------------------------------
// Multi memory lab 4 exercise 5
//----------------------------------------------------------------------

// void printPageTable(AddrSpace *addr){
//     TranslationEntry *pageTable = addr->pageTable;
    
//     for (unsigned int i = 0; i < addr->numPages; i++)
//     {
//         pageTable[i].virtualPage = i; // for now, virtual page # = phys page #s
//         pageTable[i].physicalPage = machine->AllocationMemory();
//         pageTable[i].valid = TRUE;
//         pageTable[i].use = FALSE;
//         pageTable[i].dirty = FALSE;
//         pageTable[i].readOnly = FALSE; // if the code segment was entirely on
//                                        // a separate page, we could set its
//                                        // pages to be read-only
//     }
    
// }

void ForkThread(int num){
    printf("\033[95m No.%d Thread Start \033[0m\n", num);
    machine->Run();
}

void StartMultiProcess(char *filename, int threadNum){
    printf("\033[01;34m Write by Jiang Huiqiang 1801210840 in 2019-05-05 \033[0m\n");
    if (!threadNum) {
        printf("\033[95m ThreadNum should be a num \033[0m\n");
        return;
    }
    OpenFile *executable[threadNum] = {};
    AddrSpace *space[threadNum] = {};
    Thread *thread[threadNum] = {};
    for (int i = 0; i < threadNum; ++i) executable[i] = fileSystem->Open(filename);
    for (int i = 1; i < threadNum; ++i) thread[i] = new Thread("Thread");

    if (executable[0] == NULL){
        printf("Unable to open file %s\n", filename);
        return;
    }
    for (int i = 0; i < threadNum; ++i) {
        printf("\033[95m No.%d Thread init address space \033[0m\n", i);
        space[i] = new AddrSpace(executable[i]);
    }

    currentThread->space = space[0];
    for (int i = 1; i < threadNum; ++i){
        space[i]->InitRegisters();
        space[i]->RestoreState();
        thread[i]->space = space[i];
        thread[i]->Fork(ForkThread, (void *)i);
        currentThread->Yield();
    }
    for (int i = 0; i < threadNum; ++i) delete executable[i];
    space[0]->InitRegisters();
    space[0]->RestoreState();
    printf("\033[95m No.0 Thread Start \033[0m\n"); 

    machine->Run(); // jump to the user progam
    ASSERT(FALSE);  // machine->Run never returns;
                    // the address space exits
                    // by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void ConsoleTest(char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);

    for (;;)
    {
        readAvail->P(); // wait for character to arrive
        ch = console->GetChar();
        console->PutChar(ch); // echo it!
        writeDone->P();       // wait for write to finish
        if (ch == 'q')
            return; // if q, quit
    }
}
