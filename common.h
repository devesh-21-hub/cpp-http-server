#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SERVER_PORT 8080   
#define BACKLOG 20         
#define BUF_SIZE 65536

using namespace std;


const char server_dir[] = "/home/deves/Temp/www";
