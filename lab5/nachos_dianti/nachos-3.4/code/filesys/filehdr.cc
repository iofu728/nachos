// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"
#include <time.h>

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
	return FALSE;		// not enough space

    if (numSectors < NumDirect) {
        DEBUG('f', "Allocating using direct indexing only\n");
        for (int i = 0; i < numSectors; i++)
            dataSectors[i] = freeMap->Find();
    } else {
        ASSERT(FALSE);
    }
    return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(BitMap *freeMap)
{
    for (int i = 0; i < numSectors; i++) {
	ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
	freeMap->Clear((int) dataSectors[i]);
    }
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    return(dataSectors[offset / SectorSize]);
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d. \033[93m File type %s \033[0m File blocks:\n", numBytes, type);
    printf("\033[92m sectorPos: %d\n\033[0m", SectorPos);
    printf("\033[92m CreateTime: %s\n\033[0m", createTime);
    printf("\033[92m LastVisterTime: %s\n\033[0m", lastVisterTime);
    printf("\033[92m LastModifyTime: %s\n\033[0m", lastModifiedTime);
    for (i = 0; i < numSectors; i++)
	printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
	synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
            if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
                printf("%c", data[j]);
            else
		        printf("\\%x", (unsigned char)data[j]);
        }
        printf("\n"); 
    }
    delete [] data;
}

//----------------------------------------------------------------------
// init header when create
// 	init header lab5 exercise 2
//----------------------------------------------------------------------

void FileHeader::HeaderInit(char *fileType){
    setFileType(fileType);
    SetCreateTime();
    SetLastModifyTime();
    SetLastVisterTime();
}

//----------------------------------------------------------------------
// set time
// 	set time lab5 exercise 2
//----------------------------------------------------------------------

void setTime(char *paramName, char *name){
    time_t timep;
    time (&timep);
    struct tm *timeinfo = localtime(&timep);
    strncpy(paramName, asctime(timeinfo), 25);
    paramName[24] = '\0';
    printf("\033[92m %s: %s \n \033[0m", name, paramName);
}

//----------------------------------------------------------------------
// FileHeader::SetCreateTime
// 	set create time lab5 exercise 2
//----------------------------------------------------------------------

void FileHeader::SetCreateTime(){
    setTime(createTime, "Create Time");
    // time_t timep;
    // time (&timep);
    // struct tm *timeinfo = localtime(&timep);
    // strncpy(createTime, asctime(timeinfo), 25);
    // createTime[24] = '\0';
    // printf("\033[92m Create Time: %s \n \033[0m", createTime);
}

//----------------------------------------------------------------------
// FileHeader::SetLastVisterTime
// 	set last vister time lab5 exercise 2
//----------------------------------------------------------------------

void FileHeader::SetLastVisterTime(){
    setTime(lastVisterTime, "Last Vister Time");
    // time_t timep;
    // time (&timep);
    // struct tm *timeinfo = localtime(&timep);
    // strncpy(lastVisterTime, asctime(timeinfo), 25);
    // lastVisterTime[24] = '\0';
    // printf("\033[92m Last Vsiter Time: %s \n \033[0m", createTime);
}

//----------------------------------------------------------------------
// FileHeader::SetLastModifyTime
// 	set last modify time lab5 exercise 2
//----------------------------------------------------------------------

void FileHeader::SetLastModifyTime(){
    setTime(lastModifiedTime, "Last Modified Time");
    // time_t timep;
    // time (&timep);
    // struct tm *timeinfo = localtime(&timep);
    // strncpy(lastModifiedTime, asctime(timeinfo), 25);
    // lastModifiedTime[24] = '\0';
    // printf("\033[92m Last Modified Time: %s \n \033[0m", lastModifiedTime);
}

//----------------------------------------------------------------------
// get file type from file name
//----------------------------------------------------------------------

char* getFileType(char *name){
    char *dot = strrchr(name, '.');
    if (!dot || dot == name) return "";
    return dot + 1;

}