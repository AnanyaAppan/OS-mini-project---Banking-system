#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "constants.h"

int del_user(char* username){
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    char filename[BUF_SIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    strncat(filename,extension,sizeof(extension));
    int fd = open(filename,O_RDWR,0644);
    if(fd == -1){ perror("open");}
    if(fcntl(fd, F_SETLKW, &lock)==-1) {perror("fcntl");}

    return unlink(filename);
}

int modify_user(char* username, char* new_username, char* password) {
    static struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    char filename[BUF_SIZE];
    strcpy(filename,username);
    char extension[5] = ".txt";
    int fd;
    strncat(filename,extension,sizeof(extension));
    fd = open(filename,O_RDWR,0644);
    if(fd == -1){
        perror("mod user"); return -1;
    }
    struct user u;
    if(fcntl(fd, F_SETLKW, &lock)==-1) {perror("fcntl");}
    // start of critical section
    lseek(fd,0,SEEK_SET);
    if(read(fd,&u,sizeof(struct user))==-1) { perror("read"); return -1; }
    strcpy(u.username,new_username);
    strcpy(u.password,password);
    lseek(fd,0,SEEK_SET);
    if(write(fd,&u,sizeof(struct user))==-1) { perror("write"); return -1; }
    // end of critical section
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    close(fd);
    return 0;
}