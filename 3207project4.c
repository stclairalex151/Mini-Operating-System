/*
 * Program name: 3207project4.c
 * Programmer:   Alex St.Clair
 * Program Desc: main driver for project 4
 */
//https://templeu.instructure.com/courses/63780/files/folder/Lab%20Slides/Fall%202019%20(Chenglong)/Lab4?preview=6572535

//need: 4 source files, screenshots, checklist doc, readme, makefile, disk file

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "directory.c"
#include "disk.c"
#include "disk.h"

int make_fs(char* diskname);
int mount_fs(char *disk_name);
int unmount_fs(char *disk_name);
int fs_open(char *name);
int fs_close(int fd);
int fs_create(char *name);
int fs_delete(char *name);
int fs_read(char* name, void *buf, size_t nbyte);
int fs_write(char* name, void *buf, size_t nbyte);
int fs_get_filesize(char* name);
int find_block();

//global variables
int fat[16384]; //fat array
dir directory;  //directory struct (contains list)
BootBlock boot;


int main(int argc, char** argv) {
    int option;     //stores user choice
    char* diskname = malloc(32*sizeof(char));
    char * filename = malloc(sizeof(char)* 15);
    
    printf("Welcome.");
    while(option != -1){
        printf("\nSelect an option:\n"
                " 1: Create disk\n"
                " 2: Mount disk\n"
                " 3: Unmount disk\n"
                " 4: Create a file\n"
                " 5: Delete a file\n"
                " 6: Write to a file\n"
                " 7. Read from a file\n"
                " 8. Close a file(does nothing)\n"
                " 9. Open a file (does nothing)\n"
                "10. Get size of a file\n"
                "-1. Quit the program\n");

        printf("Enter your choice: ");
        scanf("%d" , &option);
        
        switch(option){
            case 1:
                printf("\nEnter the name of the disk: ");
                scanf("%s" , diskname);

                if(make_fs(diskname) == -1)
                    printf("\nmain: disk creation failed. \n");
                break;
            case 2:
                printf("\nEnter the name of the disk: ");
                scanf("%s" , diskname);

                if(mount_fs(diskname) == -1)
                    printf("\nmain: disk failed to mount. \n");
                break;
            case 3:
                printf("Enter the name of the disk: ");
                scanf("%s" , diskname);
                
                if(unmount_fs(diskname) == -1)
                    printf("\nmain: Unmounting failed. \n");
                break;
            case 4:
                printf("Enter the name of the file/directory: ");
                scanf("%s" , filename);
                
                if(fs_create(filename) == -1)
                    printf("\nmain: fs_create failed. \n");
                break;
            case 5:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                if(fs_delete(filename) == -1)
                    printf("\nmain: file failed to open. \n");
                break;
            case 6:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                int size;
                printf("\nHow many chars will you be using?: ");
                scanf("%d", &size);
                
                void * buf = malloc(8192*4096);//buffer for file data
                printf("\nEnter the data here: \n");
                scanf("%p", &buf);

                if(fs_write(filename, buf, size) == -1)
                    printf("\nmain: file failed to read. \n");
                break;
            case 7:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                size_t bytes;
                printf("\nEnter the number of bytes to read: ");
                scanf("%zu" , &bytes);
                buf = malloc(8192*4096);//buffer for file data

                if(fs_read(filename, buf, bytes) == -1)
                    printf("\nmain: file failed to read. \n");
                break;
            case 8:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                //if(fs_close(filename) == -1)
                //    printf("\nmain: file failed to close. \n");
                printf("The file has been closed.");
                break;
            case 9:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                if(fs_open(filename) == -1)
                    printf("\nmain: file failed to open. \n");
                break;
            case 10:
                printf("\nEnter the name of the file: ");
                scanf("%s" , filename);

                int ans = fs_get_filesize(filename);
                if(ans == -1)
                    printf("\nmain: file failed to open. \n");
                else
                    printf("Size of %s is %zu bytes.\n",filename, bytes);
                break;
            default:
                break;
        }
    }
    return 0;
}

int make_fs(char* diskname){    
    
    //creates disk
    if(make_disk(diskname) == -1){
        printf("\nmake_fs: make_disk failed.\n");
        return -1;
    }
    
    //now open the disk
    if(open_disk(diskname) == -1){
        printf("\nmake_fs: open_disk failed.\n");
        return -1;
    }
    printf("The drive has been created.\n");

    /*  define disk metadata
     *  disk format: [boot][fat][dirlist][data]
     *      block 0 is boot block
     *      blocks 1-16 is fat table
     *      blocks 17-8191 is directory list
     *      block 8192-16384 is data region 
     */

    char * bootbuf = calloc(22, sizeof(char));
    if(bootbuf == NULL)
        printf("bootbuf is null.\n");
    int val = sprintf(bootbuf, "%d\n%d\n%d\n%d\n%d\n", DISK_BLOCKS * BLOCK_SIZE, 0, 1, 17, 8192);

    
    block_write(0, bootbuf);
    //block 0 now contains locations of sections of disk  
    
    int j;
    int fatsize = strlen("-1 ") * 8192 + strlen("0 ") * 8192;
    char fatlist[fatsize];  //split halfway, -1 and 1
    
    for(j=0;j<8192;j++){
        strcat(fatlist, "-1 ");
    }//fatlist now contains -1s for first half of disk
    
    
    for(j=8192;j<strlen(fatlist); j++){
        strcat(fatlist, "0 ");
    }//fatlist is now a complete buffer of inital fat table
        
    lseek(handle, 4096, SEEK_SET); //sets pointer to the start of the second block
    write(handle, fatlist, strlen(fatlist)*sizeof(char));//write the fat data to the virtual disk
    lseek(handle, 0, SEEK_SET);
    printf("the fat table has been written to disk.\n");
    
    //get the current time
    time_t t;
    struct tm *tmp; 
    char datestring[50]; 
    time(&t); 
    tmp = localtime(&t); 
    // using strftime to write formatted time/date to datestring
    strftime(datestring, sizeof(datestring), "%x - %I:%M%p", tmp); 
    
    //fill buffer with directory info for the root
    char dirbuf[55];
    sprintf(dirbuf, "%s\n%d\n%d\n%d\n%s\n%s\n%d\n", "root", 17, 18, -1,
            datestring, getenv("USER"), 4096);
    
    //dirbuf now contains to_string of root directory, now write string to root block (17)
    
    lseek(handle, 4096*17, SEEK_SET); //sets pointer to the start of the 17th block, the root directory block
    write(handle, dirbuf, strlen(dirbuf)*sizeof(char));//write the fat data to the virtual disk
    lseek(handle, 0, SEEK_SET);
    printf("root directory has been written to disk.\n");
    
    close_disk();
    printf("The disk has been completed and will now close until mounted.\n");
    return 0;
}

int mount_fs(char *disk_name){
    if(open_disk(disk_name) == -1){
        printf("\nmount_fs: open_disk failed.\n");
        return -1;
    }
    
    //buffer to load in boot block
    char * bootbuf = calloc(22, sizeof(char));
    if(bootbuf == NULL)
        printf("bootbuf is null.\n");
    
    block_read(0, bootbuf); //bootbuf now contains the data from boot block
    char token[9];  //buffer for each number grabbed (reused)
    
    //gets disksize
    memcpy(token, &bootbuf[0], 8);  //token now contains size
    token[8] = '\0';
    boot.disksize = (unsigned long)atoi(token);
    memset(token, '\0', 8);
    
    //gets boot block
    memcpy(token, &bootbuf[9], 1);  //token now contains boot block
    token[8] = '\0';
    boot.bootptr = atoi(token);
    memcpy(token, "", sizeof(""));
    
    //gets fat block
    memcpy(token, &bootbuf[11], 1);  //token now contains fat block
    token[8] = '\0';
    boot.fatptr = atoi(token);
    memcpy(token, "", sizeof(""));
    
    //gets directory block
    memcpy(token, &bootbuf[13], 2);  //token now contains directory block
    token[8] = '\0';
    boot.dirptr = atoi(token);
    memcpy(token, "", sizeof(""));

    //gets data block
    memcpy(token, &bootbuf[16], 4);  //token now contains data block no
    token[8] = '\0';
    boot.datptr = atoi(token);
    memcpy(token, "", sizeof(""));
    printf("boot block has been successfully loaded.\n");
    
    //the directory is a list of 256 entries, an entry contains metadata, etc. 
    dirent root;
    root.offset = 0;
    //buffer to load in root directory
    char * dirbuf = calloc(46, sizeof(char));
    block_read(17, dirbuf); //dirbuf now contains data from block
    if(dirbuf == NULL)
        printf("dirbuf is null.\n");
    char dirtoken[18];
    
    memcpy(dirtoken, &dirbuf[0], 4);  //token now contains name of root directory
    dirtoken[17] = '\0';
    root.name = dirtoken;
    memset(dirtoken, '\0', 17);
    
    memcpy(dirtoken, &dirbuf[5], 2);  //token now contains root block
    dirtoken[17] = '\0';
    root.start = atoi(dirtoken);
    memcpy(dirtoken, "", sizeof(""));
    
    memcpy(dirtoken, &dirbuf[8], 2);  //token now contains subdirectory block
    dirtoken[17] = '\0';
    root.sub = atoi(dirtoken);
    memcpy(dirtoken, "", sizeof(""));
    
    memcpy(dirtoken, &dirbuf[11], 18);  //token now contains date
    dirtoken[17] = '\0';
    root.m.created = dirtoken;
    memcpy(dirtoken, "", sizeof(""));
    
    memcpy(dirtoken, &dirbuf[30], 11);  //token now contains owner
    dirtoken[17] = '\0';
    root.m.owner = dirtoken;
    memcpy(dirtoken, "", sizeof(""));
    
    memcpy(dirtoken, &dirbuf[42], 4);  //token now contains size of root dir (bytes)
    dirtoken[17] = '\0';
    root.m.size = atoi(dirtoken);
    memcpy(dirtoken, "", sizeof(""));
    
    printf("root directory block has been successfully loaded.\n");
    directory.entries[0] = root;    //root is now in directory list
    directory.length++;
    printf("%s is now ready for use.\n", disk_name);
    
    return 0;
}

int unmount_fs(char *disk_name){
    //store the list of directories in a buffer
    //wipe old data region
    lseek(handle, BLOCK_SIZE * 18, SEEK_SET); //sets pointer to just inside root directory
    write(handle, 0, (8191-18)*BLOCK_SIZE*sizeof(char));//write 0s into the directory space
    lseek(handle, 0, SEEK_SET); //rewind back to the beginning

    //then write the local directory list to the disk starting at block 18
    
    char* buf = calloc(73 , sizeof(char)); //73 is max chars for directory entry info

    lseek(handle, BLOCK_SIZE * 18, SEEK_SET); //sets pointer to 18th block (inside root)
    int i;
    for(i = 1;i < directory.length; i++){ //while entries exist
        //we need to throw the info from entry into the buffer
        sprintf(buf, "%s\n%d\n%d\n%zu\n%s\n%s\n%d\n\n", directory.entries[i].name,
                directory.entries[i].start, directory.entries[i].sub, 
                directory.entries[i].offset, directory.entries[i].m.created,
                directory.entries[i].m.owner, directory.entries[i].m.size);
        printf("\nbuf:|%s|\n",buf);
        write(handle, buf, strlen(buf) * sizeof(char));    //writes the entry to the disk
        memset(buf, '\0', 72);
    }
    
    printf("data has been written back to disk, you may now remove the drive.\n");
    lseek(handle,0,SEEK_SET);//done writing, rewind pointer
    return 0;
}

int fs_open(char *name){
    int value = 1;
    while(value < 256){
        if(strcmp(directory.entries[value].name, name) != 0)
            value++;
    }
    
    if(value == 255){
        printf("fs_open: file not found.\n");
        return -1;
    }
    else
        return value;
}

int fs_close(int fd){
    if(0 >= fd && fd >= 255)
        return -1;
    
    return 0;
}

int fs_create(char* name){
    if(strlen(name) > 15){
        printf("fs_create: name too long.\n");
        return -1;
    }
    int i;
    for(i = 1; i < directory.length; i++){
        if(strcmp(directory.entries[i].name , name) == 0){
            printf("fs_create: name %s already exists in directory.",name);
            return -1;
        }
    }
    if(directory.length == 256){
        printf("fs_create: directory full");
        return -1;
    }
    getchar();
    char isdir;
    printf("Press f for file or d for directory: ");
    scanf("%c", &isdir);
    
    
    dirent newfile;
    newfile.name = name;
    newfile.offset = 0;
    newfile.start = find_block();
    fat[newfile.start] = -1;//reserve block for directory

    if(strcmp(&isdir, "d"))
        newfile.sub = find_block();
    else if(strcmp(&isdir, "f"))
        newfile.sub = -1;
    
    //get the current time
    time_t t;
    struct tm *tmp; 
    char datestring[50]; 
    time(&t); 
    tmp = localtime(&t); 
    // using strftime to write formatted time/date to datestring
    strftime(datestring, sizeof(datestring), "%x - %I:%M%p", tmp); 
    
    newfile.m.created = datestring;
    newfile.m.owner = getenv("USER");
    newfile.m.size = 4096;
    
    directory.entries[directory.length] = newfile;
    directory.length++;
    
    printf("file %s has been created.\n" , newfile.name);
    return 0;
}

int fs_delete(char *name){
    //find the location of the file in the directory 
    int i;
    for(i=1;i<directory.length;i++){
        if(directory.entries[i].name == name)
            break;
    }
    if(directory.entries[i].name != name){//if the last entry doesnt match, not found
        printf("file not found.\n");
        return -1;
    }//i is now the location of the file in the directory
        
    int start = directory.entries[i].start; //grab the start block of the file before deleting
        
    int blocks[8192];//list of blocks used by file
    blocks[0] = start;
    
    //fill blocks[] with block numbers
    int next;
    int j=1;
    while(start != -1){
        next = fat[start];
        blocks[j] = next;
        start = next;
        j++;
    }//blocks also contains a -1 at the end, decrement j to exclude
    j--;//j is the number of blocks used
    
    //free fat blocks taken by file
    char* buf = calloc(4096, sizeof(char));
    for(i=0; i< j; i++){//for every block used
        block_write(blocks[i], buf);
    }
    
    for(j = i; j < directory.length;j++)
        directory.entries[j] = directory.entries[j+1];
    //brings all files past this one down one in the list 
    directory.length --;//file is now deleted from directory list
    
    printf("the file has been deleted. \n");
    
    return 0;
}

int fs_read(char* name, void *buf, size_t nbyte){
    //find the location of the file in the directory 
    int i;
    for(i=1;i<directory.length;i++){
        if(directory.entries[i].name == name)
            break;
    }
    if(directory.entries[i].name != name){//if the last entry doesnt match, not found
        printf("file not found.\n");
        return -1;
    }//i is now the location of the file in the directory
        
    int blocks[8192];//list of blocks used by file
    int start = directory.entries[i].start;
    blocks[0] = start; 
    
    //fill blocks[] with block numbers
    int next;
    int j=1;
    while(start != -1){
        next = fat[start];
        blocks[j] = next;
        start = next;
        j++;
    }//blocks also contains a -1 at the end, decrement j to exclude
    j--;//j is the number of blocks used
    
    
    int k;
    for(int k = 0; (k <= j) && (directory.entries[i].offset <= nbyte); k++){
        block_read(k, buf);//reads block by block and throws into buf
        printf("fp for that file is now at: %zu\n", directory.entries[i].offset);
    }//buf now contains file data
    
    
    printf("data read was:\n|%p|\n", buf);
    
    return directory.entries[i].offset;
}

int fs_write(char* name, void *buf, size_t nbyte){
    //find the location of the file in the directory 
    int i;
    for(i=1;i<directory.length;i++){
        if(directory.entries[i].name == name)
            break;
    }
    if(directory.entries[i].name != name){//if the last entry doesnt match, not found
        printf("file not found.\n");
        return -1;
    }//i is now the location of the file in the directory
    printf("location in dirlist is %d\n",i); 
    //find the final block of the file
    int start = directory.entries[i].start;
    int next;
    while(start != -1){
        next = fat[start];
        start = next;
    }//start is now the location of the final block
    
    
    block_write(start, buf);
    if(nbyte > 4096){
        int new = find_block();
        fat[start] = find_block();
        fat[new] = -1;
    }
    printf("data has been written.\n");
    getchar();
    getchar();
    return directory.entries[i].offset;
}

int fs_get_filesize(char* name){
    int fd = -1;
    int i = 0;
    for(i=0;i<directory.length;i++){
        if(strcmp(name, directory.entries[i].name) == 0)
            fd = i;
    }
    if(fd==-1){
        printf("File could not be found.\n");
        return 0;
    }
           
    return directory.entries[fd].m.size;
}

int find_block(){
    int startblock; //to get the start block of the new file
    for(startblock = 8192;startblock <= 16384; startblock++){
        if(fat[startblock] == 0)
            break;
    }//startblock now contains the value of the first block to be used
    
    if(startblock == 16384) //last block can not be written to
        return -1;
    
    return startblock;
}