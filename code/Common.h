#ifndef  CHATROOM_COMMON_H
#define CHATROOM_COMMON_H
 
#include <iostream>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
using namespace std;


//IP & port
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
 
//maximum number of handles
#define EPOLL_SIZE 5000

//Buffer size
#define BUF_SIZE 0xFFFF  

//new user message
#define SERVER_WELCOME "Welcome!! Your ID is USER #%d"

//who sent the message
#define SERVER_MESSAGE "USER%d say >> %s"
#define SERVER_PRIVATE_MESSAGE "USER%d say to you privately >> %s"
#define SERVER_PRIVATE_ERROR_MESSAGE "USER%d is not in the chat room"

// exit system
#define EXIT "EXIT"

//only user
#define CAUTION "There is only one int the char room!"
 


//add fd into epollfd
static void addfd( int epollfd, int fd, bool enable_et )
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    if( enable_et )
        ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)| O_NONBLOCK);

    cout<<"fd added"<<endl;
}
 

//message
struct Msg
{
    char content[BUF_SIZE];
 
};

#endif 
