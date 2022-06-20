#include "Server.h"
using namespace std;

Server::Server(){
	serverAddr.sin_family = PF_INET;   //타입 : IPv4
	serverAddr.sin_port = htons(SERVER_PORT);   //port
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);   //ip주소


	listener = 0;
	epfd = 0;
}


void Server::Init(){
	cout<<"...Init Server..."<<endl;

	//tcp 연결지향형이고 ipv4 도메인을 위한 소켓을 생성
	listener = socket(PF_INET, SOCK_STREAM, 0);
	if(listener < 0){
		perror("listener");
		exit(-1);
	}

	
	// 서버의 ip주소와 port를 소켓에 할당
	if(bind(listener, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
		perror("bind error");
		exit(-1);
	}



	//대기상태로 만들기
	int ret = listen(listener, 5);
	if(ret < 0){
		perror("listen error");
		exit(-1);
	}
	


	cout<<"Start : "<<SERVER_IP<<endl;


	//epfd는 커널에서 이벤트 테이블을 생성하는 핸들
	epfd = epoll_create(EPOLL_SIZE);

	if(epfd < 0){
		perror("epfd error");
		exit(-1);
	}
	
	//이벤트 테이블에 추가
	addfd(epfd, listener, true);
}



void Server::Close(){
	close(listener);
	close(epfd);
}



int Server::SendBroadcastMessage(int clientfd){
	char recv_buf[BUF_SIZE];
	bzero(recv_buf, BUF_SIZE);
	char send_buf[BUF_SIZE];
	Msg msg;
	

	cout<<"Read from Client(USER = "<<clientfd<<")"<<endl;
	int len = recv(clientfd, recv_buf, BUF_SIZE, 0);
	memset(&msg,0,sizeof(msg));
	memcpy(&msg, recv_buf,sizeof(msg));


	//클라이언트 나감
	if(len == 0){
		close(clientfd);
		
		clients_list.remove(clientfd);
		cout<<"USR = "<<clientfd<<" closed"<<endl<<"Now there are "<<clients_list.size()<<" client in the char room."<<endl;
	}
	else{
		//클라이언트가 1명 남아있다면
		if(clients_list.size() == 1){
			memcpy(&msg.content, CAUTION, sizeof(msg.content));
			bzero(send_buf, BUF_SIZE);
			memcpy(send_buf, &msg, sizeof(msg));
			send(clientfd, send_buf, sizeof(send_buf), 0);
			return len;
		}


		char format_message[BUF_SIZE];
		//전송된 메시지 내용 포맷
		sprintf(format_message, SERVER_MESSAGE, clientfd, msg.content);
		memcpy(msg.content, format_message, BUF_SIZE);
		
		//클라이언트들에게 메시지 전송
		list<int>::iterator it;
		for(it = clients_list.begin(); it != clients_list.end(); ++it){
			if(*it != clientfd){
				bzero(send_buf, BUF_SIZE);
				memcpy(send_buf, &msg, sizeof(msg));
				if(send(*it, send_buf, sizeof(send_buf),0) < 0){
					return -1;
				}
			}
		}
	}
	return len;
}



void Server::Start(){
	//epoll 이벤트 큐
	static struct epoll_event events[EPOLL_SIZE];
	
	Init();

	while(1){
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
	
		if(epoll_events_count < 0){
			perror("epoll failure");
			break;
		}



		cout<<"epoll_events_counts = "<<epoll_events_count<<endl;
		for(int i=0;i<epoll_events_count;++i){
			int sockfd = events[i].data.fd;

			//새로운 유저 연결			
			if(sockfd == listener){
				struct sockaddr_in client_address;
				socklen_t client_addrLength = sizeof(struct sockaddr_in);
				int clientfd = accept(listener, (struct sockaddr*)&client_address, &client_addrLength);
				

				cout<<"Client connection from : "<<inet_ntoa(client_address.sin_addr)<<", clientfd = "<<clientfd<<endl;


				addfd(epfd, clientfd, true);
				clients_list.push_back(clientfd);
				
				cout<<"Add new clientfd = "<<clientfd<<" to epoll"<<endl;
				cout<<"Now there are "<<clients_list.size()<<" clients in the chat room."<<endl;

			

				//welcome 메시지 보내기
				cout<<"welcome message"<<endl;
				char message[BUF_SIZE];
				bzero(message, BUF_SIZE);
				sprintf(message, SERVER_WELCOME, clientfd);
				int ret = send(clientfd, message, BUF_SIZE, 0);
				if(ret < 0){
					perror("send error");
					Close();
					exit(-1);
				}
			}
			else{
				int ret = SendBroadcastMessage(sockfd);
				if(ret < 0){
					perror("error");
					Close();
					exit(-1);
				}			
			}
		}
	}
	Close();
}
