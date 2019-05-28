// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "bitmap.h"

#define HeaderIntNum 2
#define HeaderTimeNum 3
#define MaxFileTimeLen 26
#define MaxFileNameLen 5
#define HeaderStringLen MaxFileNameLen + HeaderTimeNum * MaxFileTimeLen

#define NumDirect 	((SectorSize - (HeaderIntNum * sizeof(int) + HeaderStringLen * sizeof(char))) / sizeof(int))
#define MaxFileSize 	(NumDirect * SectorSize)

// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:
    bool Allocate(BitMap *bitMap, int fileSize);// Initialize a file header, 
						//  including allocating space 
						//  on disk for the file data
    void Deallocate(BitMap *bitMap);  		// De-allocate this file's 
						//  data blocks

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk

    int ByteToSector(int offset);	// Convert a byte offset into the file
					// to the disk sector containing
					// the byte

    int FileLength();			// Return the length of the file 
					// in bytes

    void Print();			// Print the contents of the file.
    

    void SetCreateTime();       // lab5 set create time 
    void SetLastVisterTime();   // lab5 set last visiter time 
    void SetLastModifyTime();   // lab5 set last modified time 
    void HeaderInit(char *type);// lab5 init header set
    void setFileType(char* fileType) { strcmp(fileType, "") ? strcpy(type, fileType) : strcpy(type, "None"); }
                                // lab5 set file type
    int SectorPos;              // lab5 sector position

  private:
    int numBytes;			// Number of bytes in the file
    int numSectors;			// Number of data sectors in the file
    int dataSectors[NumDirect];		// Disk sector numbers for each data 
					                        // block in the file
    char createTime[MaxFileTimeLen];      // lab5 create time
    char lastVisterTime[MaxFileTimeLen];  // lab5 last vister time
    char lastModifiedTime[MaxFileTimeLen];// lab5 last modified time
    char type[MaxFileNameLen];            // lab5 file type
};

extern char* getFileType(char *name);
#endif // FILEHDR_H
