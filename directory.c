/*
 * Program name: directory.c
 * Programmer:   Alex St.Clair
 * Program Desc: (logical) directory structure and functions
 */
#include <time.h>
#include "disk.h"

typedef struct{
    char* created;  //time of creation 
    char* owner;    //name of owner
    int size;       //size of file in bytes
}metadata;

typedef struct{
    char* name; //name of file/directory (15 char max)
    int start;  //location of starting block for file
    int sub;    //location of subdirectory, -1 if file
    metadata m; //metadata for the file
    size_t offset; //location of pointer inside file (-1 for directories)
}dirent;

typedef struct{
    dirent entries[256];//dir is list of 256 entries
    int length; //logical size of array
}dir;

typedef struct{
    size_t disksize;
    int bootptr;
    int dirptr;
    int fatptr;
    int datptr;
}BootBlock;

/**DIRECTORY STORAGE
root
17
18
4090
12/04/19 - 03:37PM
alexstclair
4096
 */
