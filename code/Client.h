#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include "Common.h"
using namespace std;


class Client{
public:
	Client();
	void Connect();
	void Close();
	void Start();

private:
	int sock;
	//현재 프로세스 ID
	int pid;
	int epfd;
	int pipe_fd[2];   //fd[0]은 상위 프로세스 읽기, fd[1]은 하위프로세스 쓰기
	bool isClientwork;

	Msg msg;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];

	struct sockaddr_in serverAddr;
};


#endif
