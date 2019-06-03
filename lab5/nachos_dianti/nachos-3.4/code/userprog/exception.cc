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
#include "addrspace.h"

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

void CreateSyscallHandler() {
    // First, get the length of filename
    int fileNameBase = machine->ReadRegister(4);
    int value;
    int length = 0;
    do {
        machine->ReadMem(fileNameBase++, 1, &value);
        length++;
    } while(value != '\0');

    // Copy filename
    char *fileName = new char[length];
    fileNameBase -= length; length--;
    for(int i = 0; i < length; i++) {
        machine->ReadMem(fileNameBase++, 1, &value);
        fileName[i] = char(value);
    }
    fileName[length] = '\0';
    DEBUG('a', "File name: %s\n", fileName);

    bool result = fileSystem->Create(fileName, 0);

    if(result)
        DEBUG('a', "Create file %s done\n", fileName);
    else
        DEBUG('a', "Can not create file %s\n", fileName);
    delete fileName;
}

void OpenSyscallHandler() {
    // First, get the length of filename
    int fileNameBase = machine->ReadRegister(4);
    int value;
    int length = 0;
    do {
        machine->ReadMem(fileNameBase++, 1, &value);
        length++;
    } while(value != '\0');

    // Copy filename
    char *fileName = new char[length];
    fileNameBase -= length; length--;
    for(int i = 0; i < length; i++) {
        machine->ReadMem(fileNameBase++, 1, &value);
        fileName[i] = char(value);
    }
    fileName[length] = '\0';
    DEBUG('a', "File name: %s\n", fileName);

    OpenFile *openFile = fileSystem->Open(fileName);

    if(openFile != NULL)
        DEBUG('a', "Open file %s done\n", fileName);
    else
        DEBUG('a', "Can not open file %s\n", fileName);

    machine->WriteRegister(2, (int)openFile);
}

void CloseSyscallHandler() {
    OpenFile *openFile = (OpenFile *)machine->ReadRegister(4);
    DEBUG('a', "Close File\n");
    delete openFile;
}

void WriteSyscallHandler() {
    int buffer = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    OpenFile *openFile = (OpenFile *)machine->ReadRegister(6);

    // Copy data from user space into kernel space
    char *kernelBuffer = new char[size + 1];
    int value, i;
    for(i = 0; i < size; i++) {
        bool success = machine->ReadMem(buffer++, 1, &value);
        if(!success) {
            buffer--; i--;
            continue;
        }
        kernelBuffer[i] = char(value);
    }
    kernelBuffer[i] = '\0';

    // Write into file
    int result = openFile->Write(kernelBuffer, size);
    machine->WriteRegister(2, result);
    DEBUG('a', "Write %d bytes into file(%d bytes requested)\nContent: %s\n", result, size, kernelBuffer);
    delete kernelBuffer;
}

int ReadSyscallHandler() {
    int buffer = machine->ReadRegister(4);
    int size = machine->ReadRegister(5);
    OpenFile *openFile = (OpenFile *)machine->ReadRegister(6);

    char *kernelBuffer = new char[size];

    // Read from file into kernel space
    int result = openFile->Read(kernelBuffer, size);

    // Write into user space
    for(int i = 0; i < result; i++) {
        bool success = machine->WriteMem(buffer++, 1, (int)kernelBuffer[i]);
        if(!success) {
            buffer--; i--;
        }
    }
    machine->WriteRegister(2, result);

    DEBUG('a', "Read %d bytes from file(%d bytes requested)\nContent: %s\n", size, result, kernelBuffer);
    delete kernelBuffer;
}


void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
        if (type == SC_Halt) {
	        DEBUG('f', "\033[93m Shutdown, initiated by user program.\n\033[0m");
           interrupt->Halt();
        }
        else if(type == SC_Exit) {
            DEBUG('f', "\033[91m Syscall: Exit\n");
            int exitCode = machine->ReadRegister(4);
            printf("\nThread %s finished with exit code %d\n\033[0m", currentThread->getName(), exitCode);
            currentThread->Finish();
        }
        else if(type == SC_Create) {
            DEBUG('f', "\033[92m Syscall: Create\n\033[0m");
            CreateSyscallHandler();
        }
        else if(type == SC_Open) {
            DEBUG('f', "\033[92m Syscall: Open\n\033[0m");
            OpenSyscallHandler();
        }
        else if(type == SC_Close) {
            DEBUG('f', "\033[92m Syscall: Close\n\033[0m");
            CloseSyscallHandler();
        }
        else if(type == SC_Write) {
            DEBUG('f', "\033[93m Syscall: Write\n\033[0m");
            WriteSyscallHandler();
        }
        else if(type == SC_Read) {
            DEBUG('f', "\033[93m Syscall: Read\n\033[0m");
            ReadSyscallHandler();
        }

        // Increase PC
        machine->ReturnFromSyscall();        
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }
}
