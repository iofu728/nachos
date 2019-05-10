// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Lab4 in 19.5.5
//----------------------------------------------------------------------

int time = 0, replace = 1;
int tlbTime[TLBSize] = {0, 0, 0, 0};
// PageTable Fault Hander
void PageTableFaultHandler(unsigned int vpn)
{

  DEBUG('a', "\033[91mPage Table Fault \033[0m\n");
  OpenFile *openfile = fileSystem->Open("virtual_memory");
  int pos = machine->AllocationMemory();
  if (pos == -1) {
    pos = 0;
    for (int i = 0; i < machine->pageTableSize; ++i){
      if (machine->pageTable[i].physicalPage == 0) {
        if (machine->pageTable[i].dirty == TRUE) {
          openfile->WriteAt(&(machine->mainMemory[pos * PageSize]),
            PageSize, machine->pageTable[i].virtualPage * PageSize);
          machine->pageTable[i].valid = FALSE;
          break;
        }
      }
    }
    
  }
  openfile->ReadAt(&(machine->mainMemory[pos * PageSize]), PageSize, vpn * PageSize);
  machine->PageTable[vpn].valid = TRUE;
  machine->PageTable[vpn].physicalPage = pos;
  machine->PageTable[vpn].use = FALSE;
  machine->PageTable[vpn].dirty = FALSE;
  machine->PageTable[vpn].readOnly = FALSE;
  delete openfile;
  
  

  ASSERT(FALSE);
}

// TLB Miss Fault Handler + PageTable Fault Handler
// PageTable Fault Hander
void PageFaultHandler()
{
  unsigned int vpn, offset;
  int virtAddr, emptyTLBIndex = 0;

  virtAddr = machine->ReadRegister(BadVAddrReg);
  vpn = (unsigned)virtAddr / PageSize;
  offset = (unsigned)virtAddr % PageSize;
  DEBUG('a', "\033[92mVPN: 0x%x Offset:0x%x\033[0m\n", vpn, offset);

  if (machine->tlb == NULL)
  {
    PageTableFaultHandler(vpn);
  }
  else
  {
    while (emptyTLBIndex < TLBSize && machine->tlb[emptyTLBIndex].valid)
      ++emptyTLBIndex;

    if (emptyTLBIndex == TLBSize)
    {
      if (!replace)
      {
        int minTime = 0x3fffffff;
        for (int i = 0; i < TLBSize; ++i)
        {
          DEBUG('a', "\033[92m TLBTime: %d \033[0m\n", tlbTime[i]);
          if (tlbTime[i] < minTime)
          {
            minTime = tlbTime[i];
            emptyTLBIndex = i;
          }
        }
      }
      else
      {
        int maxTime = -1;
        for (int i = 0; i < TLBSize; ++i)
          if (machine->LRUTLB[i] > maxTime)
          {
            maxTime = machine->LRUTLB[i];
            emptyTLBIndex = i;
          }
        int lruNUm = machine->LRUTLB[emptyTLBIndex];
        for (int i = 0; i < TLBSize; ++i)
        {
          if (machine->LRUTLB[i] <= lruNUm && machine->LRUTLB[i] < TLBSize)
            ++machine->LRUTLB[i];
          if (emptyTLBIndex == i)
            machine->LRUTLB[i] = 1;
          DEBUG('a', "\033[92m LRUTLB: %d \n", machine->LRUTLB[i]);
        }
      }
    }

    if (machine->pageTable[vpn].valid)
    {
      printf("\033[92m TLB Index:%d \033[0m\n", emptyTLBIndex);
      machine->tlb[emptyTLBIndex] = machine->pageTable[vpn];
      ++tlbTime[emptyTLBIndex];
      ++TLBMiss;
      ++time;
    }
    else
    {
      PageTableFaultHandler(vpn);
    }
  }
}

// Exception Handler entry function
void ExceptionHandler(ExceptionType which)
{
  int type = machine->ReadRegister(2);
  if (which == PageFaultException)
  {
    PageFaultHandler();
  }
  else if ((which == SyscallException) && (type == SC_Halt))
  {
    DEBUG('a', "Shutdown, initiated by user program.\n");
    printf("TLB MIss Time: %d, Ratio TLB Miss:%.2f%\n", TLBMiss,
           TLBMiss * 100.0 / totalRun);
    machine->ClearMemory();
    currentThread->Finish();
    
    int nextPC = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, nextPC);
    interrupt->Halt();
  }
  else
  {
    printf("Unexpected user mode exception %d %d\n", which, type);
    ASSERT(FALSE);
  }
}
