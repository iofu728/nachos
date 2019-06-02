// directory.cc 
//	Routines to manage a directory of file names.
//
//	The directory is a table of fixed length entries; each
//	entry represents a single file, and contains the file name,
//	and the location of the file header on disk.  The fixed size
//	of each directory entry means that we have the restriction
//	of a fixed maximum size for file names.
//
//	The constructor initializes an empty directory of a certain size;
//	we use ReadFrom/WriteBack to fetch the contents of the directory
//	from disk, and to write back any modifications back to disk.
//
//	Also, this implementation has the restriction that the size
//	of the directory cannot expand.  In other words, once all the
//	entries in the directory are used, no more files can be created.
//	Fixing this is one of the parts to the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

#define NumDirEntries 		10

//----------------------------------------------------------------------
// Directory::Directory
// 	Initialize a directory; initially, the directory is completely
//	empty.  If the disk is being formatted, an empty directory
//	is all we need, but otherwise, we need to call FetchFrom in order
//	to initialize it from disk.
//
//	"size" is the number of entries in the directory
//----------------------------------------------------------------------

Directory::Directory(int size)
{
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++){
	    table[i].inUse = FALSE;
        // table[i].sector = 0;
    }
    for (int i = 0; i < tableSize; i++)
        printf("Id: %d, name: %s, inUse: %d, sector: %d\n", i, table[i].name, table[i].inUse, table[i].sector);
}

//----------------------------------------------------------------------
// Directory::~Directory
// 	De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{ 
    delete [] table;
} 

//----------------------------------------------------------------------
// Directory::FetchFrom
// 	Read the contents of the directory from disk.
//
//	"file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    // printf("file name");
    Print();

    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
    Print();
    // for (int i = 0; i < tableSize; i++)
	//     printf("Id: %d, name: %s, inUse: %d, sector: %d\n", i, table[i].name, table[i].inUse, table[i].sector);
}

//----------------------------------------------------------------------
// Directory::WriteBack
// 	Write any modifications to the directory back to disk
//
//	"file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    Print();
    // for (int i = 0; i < tableSize; i++)
	//     printf("Id: %d, name: %s, inUse: %d, sector: %d\n", i, table[i].name, table[i].inUse, table[i].sector);
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
    Print();
    // for (int i = 0; i < tableSize; i++)
	//     printf("Id: %d, name: %s, inUse: %d, sector: %d\n", i, table[i].name, table[i].inUse, table[i].sector);
}

//----------------------------------------------------------------------
// Directory::FindIndex
// 	Look up file name in directory, and return its location in the table of
//	directory entries.  Return -1 if the name isn't in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(char *name)
{
    printf("Need Name: %s\n", name);
    for (int i = 0; i < tableSize; i++) {
        printf("%s %d %d\n", table[i].name, table[i].inUse, table[i].sector);
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen)) 
            return i;
            // char *tempName;
            // nameFile->ReadAt(tempName, table[i].name_len, table[i].name_pos);
            // if (!strncmp(tempName, name, FileNameMaxLen))
                
        // }
	        
    }
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
// 	Look up file name in directory, and return the disk sector number
//	where the file's header is stored. Return -1 if the name isn't 
//	in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::Find(char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].sector;
    return -1;
}

//----------------------------------------------------------------------
// Directory::Add
// 	Add a file into the directory.  Return TRUE if successful;
//	return FALSE if the file name is already in the directory, or if
//	the directory is completely full, and has no more space for
//	additional file names.
//
//	"name" -- the name of the file being added
//	"newSector" -- the disk sector containing the added file's header
//----------------------------------------------------------------------

bool
Directory::Add(char *name, int newSector, int type)
{ 
    char fileName[FileNameMaxLen + 1];
    strncpy(fileName, FindName(name), FileNameMaxLen);
    if (FindIndex(fileName) != -1)
	    return FALSE;

    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = TRUE;
            strncpy(table[i].path, name, 20);
            // int nameLen = 10;
            // table[i].name_len = nameLen;
            // table[i].name_pos = nowNamePos;
            // nameFile->WriteAt(name, nameLen, nowNamePos);     // lab 5 more file name
            strncpy(table[i].name, fileName, FileNameMaxLen); 
            table[i].sector = newSector;
            table[i].type = type;
            // nowNamePos += nameLen;
            // printf("Directory Id: %d %d %d % d %d\n", i, table[i].inUse, table[i].name_len, table[i].name_pos, table[i].sector);
            printf("Directory Id: %d %d %s %d %s %d\n", i, table[i].inUse, table[i].name, table[i].sector, table[i].path, table[i].type); 
            return TRUE;
	    }
    return FALSE;	// no space.  Fix when we have extensible files.
}

//----------------------------------------------------------------------
// Directory::Remove
// 	Remove a file name from the directory.  Return TRUE if successful;
//	return FALSE if the file isn't in the directory. 
//
//	"name" -- the file name to be removed
//----------------------------------------------------------------------

bool
Directory::Remove(char *name)
{ 
    int i = FindIndex(name);

    if (i == -1)
	return FALSE; 		// name not in directory
    table[i].inUse = FALSE;
    return TRUE;	
}

//----------------------------------------------------------------------
// Directory::List
// 	List all the file names in the directory. 
//----------------------------------------------------------------------

void
Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	if (table[i].inUse)
	    printf("Name: %s\n", table[i].name);
        // printf("Name Len: %d, Name pos: %d\n", table[i].name_len, table[i].name_pos);
}

//----------------------------------------------------------------------
// Directory::Print
// 	List all the file names in the directory, their FileHeader locations,
//	and the contents of each file.  For debugging.
//----------------------------------------------------------------------

void
Directory::Print()
{ 
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse) {
            // printf("Id: %d, Name Len: %d, Name pos: %d, InUse: %d, Sector: %d\n", i, table[i].name_len, table[i].name_pos, table[i].inUse, table[i].sector);
            printf("Id: %d, \033[91m Name: %s \033[0m, InUse: %d, Sector: %d,\033[91m Type: %d, Path: %s \033[0m\n", i, table[i].name, table[i].inUse, table[i].sector, table[i].type, table[i].path);
            hdr->FetchFrom(table[i].sector);
            hdr->Print();
        }
    printf("\n");
    delete hdr;
}


//----------------------------------------------------------------------
// Directory::InitNameFile
// 	init name file
//----------------------------------------------------------------------

// void Directory::InitNameFile(int sector){
//     nameFile = new OpenFile(sector);
//     nowNamePos = 0;
//     WriteBack(nameFile);
// }

//----------------------------------------------------------------------
// Directory::FindDir
// 	Multi Dir to find path
//----------------------------------------------------------------------

int Directory::FindDir(char *name){
    int sector = 1;
    OpenFile *directoryFile = new OpenFile(sector);
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(directoryFile);
    int strPos = 0;
    int subStrPos = 0;
    char subStr[10];
    while (strPos < strlen(name)){
        subStr[subStrPos++] = name[strPos++];
        if (name[strPos] == '/') {
            subStr[subStrPos] = '\0';
            sector = directory->Find(subStr);
            directoryFile = new OpenFile(sector);
            directory = new Directory(NumDirEntries);
            directory->FetchFrom(directoryFile);
            ++strPos;
            subStrPos = 0;
        }
    }
    return sector;
}

//----------------------------------------------------------------------
// Directory::FindName
// 	Multi Dir to find path
//----------------------------------------------------------------------


char *Directory::FindName(char *name){
    printf("1111112\n");
    
    char fileName[FileNameMaxLen + 1];
    printf("1111113, %s %d\n", name, strlen(name));
    int pos = -1;
    for (int i = strlen(name) - 1; i >= 0; --i)
        if (name[i] == '/') {
            pos = i + 1;
            break;
        }
    printf("1111114\n");
    if (pos == -1) pos = 0;
    int j = 0;
    printf("1111115\n");
    for (int i = pos; i < strlen(name); ++i)
        fileName[j++] = name[i];
    printf("1111116\n");
    fileName[j] = '\0';
    printf("1111117, %s\n", fileName);
    char *nameFile = fileName;
    printf("%s\n", nameFile);
    return nameFile;
}

//----------------------------------------------------------------------
// Directory::GetType
// 	Multi Dir get file type
//----------------------------------------------------------------------

int Directory::GetType(char *name){
  for (int i = 0; i < tableSize; i++) {
        printf("%s %d %d\n", table[i].name, table[i].inUse, table[i].sector);
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen)) 
            return table[i].type;
    }
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::IsEmpty
// 	Multi Dir is empty
//----------------------------------------------------------------------

bool Directory::IsEmpty() {
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse)
           return FALSE;
    return TRUE;
}