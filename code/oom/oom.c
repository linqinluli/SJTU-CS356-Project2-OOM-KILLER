/*
Filename: oom.c

Student name: Hanzhang Yang
Student ID: 518030910022

This file is the daemon process to call oom killer
*/
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int sleeptime=atoi(argv[1]);//time parameter
    printf("oom begin to work\n");
    daemon(0,0);//create daemon functions
    printf("daemon load\n");
    while(1)
    {
        syscall(382,sleeptime);
        sleep(sleeptime);
    }

}

