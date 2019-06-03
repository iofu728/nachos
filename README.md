## Operation System Lab Result by Jianpan Gun in 19.3-19.6


## Lab4

### Exercise 2

Shoudld be

```plaintext
Initializing address space, num pages 10, size 1280
Initializing code segment, at 0x0, size 256
Initializing stack register to 1264
Starting thread "main" at time 10
Reading VA 0x0, size 4
	Translate 0x0, read: phys addr = 0x0
	value read = 0c000034
At PC = 0x0: JAL 52
Reading VA 0x4, size 4
	Translate 0x4, read: phys addr = 0x4
	value read = 00000000
At PC = 0x4: SLL r0,r0,0
Reading VA 0xd0, size 4
	Translate 0xd0, read: phys addr = 0xd0
	value read = 27bdffe8
At PC = 0xd0: ADDIU r29,r29,-24
Reading VA 0xd4, size 4
	Translate 0xd4, read: phys addr = 0xd4
	value read = afbf0014
At PC = 0xd4: SW r31,20(r29)
Writing VA 0x4ec, size 4, value 0x8
	Translate 0x4ec, write: phys addr = 0x4ec
Reading VA 0xd8, size 4
	Translate 0xd8, read: phys addr = 0xd8
	value read = afbe0010
At PC = 0xd8: SW r30,16(r29)
Writing VA 0x4e8, size 4, value 0x0
	Translate 0x4e8, write: phys addr = 0x4e8
Reading VA 0xdc, size 4
	Translate 0xdc, read: phys addr = 0xdc
	value read = 0c000030
At PC = 0xdc: JAL 48
Reading VA 0xe0, size 4
	Translate 0xe0, read: phys addr = 0xe0
	value read = 03a0f021
At PC = 0xe0: ADDU r30,r29,r0
Reading VA 0xc0, size 4
	Translate 0xc0, read: phys addr = 0xc0
	value read = 03e00008
At PC = 0xc0: JR r0,r31
Reading VA 0xc4, size 4
	Translate 0xc4, read: phys addr = 0xc4
	value read = 00000000
At PC = 0xc4: SLL r0,r0,0
Reading VA 0xe4, size 4
	Translate 0xe4, read: phys addr = 0xe4
	value read = 0c000004
At PC = 0xe4: JAL 4
Reading VA 0xe8, size 4
	Translate 0xe8, read: phys addr = 0xe8
	value read = 00000000
At PC = 0xe8: SLL r0,r0,0
Reading VA 0x10, size 4
	Translate 0x10, read: phys addr = 0x10
	value read = 24020000
At PC = 0x10: ADDIU r2,r0,0
Reading VA 0x14, size 4
	Translate 0x14, read: phys addr = 0x14
	value read = 0000000c
At PC = 0x14: SYSCALL
Exception: syscall
Shutdown, initiated by user program.
Machine halting!

Ticks: total 22, idle 0, system 10, user 12
Disk I/O: reads 0, writes 0
Console I/O: reads 0, writes 0
Paging: faults 0
Network I/O: packets received 0, sent 0

Cleaning up...
```

One

```
 Write by Jiang Huiqiang 1801210840 in 2019-05-05
Initializing address space, num pages 10, size 1280
Initializing code segment, at 0x0, size 256
Initializing stack register to 1264
Starting thread "main" at time 10
Reading VA 0x0, size 4
	Translate 0x0, read: *** no valid TLB entry found for this virtual page!
Exception: page fault/no TLB entry
Unexpected user mode exception 2 0
Assertion failed: line 110, file "../userprog/exception.cc"
[1]    2622 abort      ./nachos -x ../test/halt -d am
```

TLB MIss Time: 66, Ratio TLB Miss:5.64%
TLB MIss Time: 50, Ratio TLB Miss:4.31%

## Trouble

### 'TranslationEntry' does not name a type

```
In file included from ../threads/thread.h:44:0,
                 from ../threads/scheduler.h:14,
                 from ../threads/system.h:13,
                 from ../threads/main.cc:53:
../userprog/addrspace.h:35:3: error: 'TranslationEntry' does not name a type
   TranslationEntry *pageTable;  // Assume linear page table translation
```

The order of 'include' is very import.

## Bit Calculations

```cpp
int have_many_1(int n ){
	int count = 0;

	while(n){
		count++;
		n = (n-1) & n;
	}
	return count;
}

// find last 1 position -> to 10 scale
_bit = n & ((n - 1)^-1);
f_bit = n & (~(n - 1))
```

## Lab 5

### Trouble Shooting

#### Segment fault when use time_h

Until now, I have not find problem in where. But I know it's cause by lab4 code.

#### fsttest.cc abort

```cpp
openFile = fileSystem->Open(to);
ASSERT(openFile != NULL);
```

you must run `./nachos -f` before other

#### copy char* to char []

you should use `strncpy` rather than `strcpy`
```
strncpy(type, fileType, sizeof type - 1);
type[sizeof type - 1] = '\0';
```

- [如何在C中将char * str复制到char c []？](https://codeday.me/bug/20180921/256704.html)