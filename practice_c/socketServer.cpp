#include<winsock2.h>
#include<iostream>

#define LISTEN_PORT 6000
#define RECV_BUFFER_LEN 1024

using namespace std;

//socket server
int server() {
	SOCKADDR_IN listenInfo;
	listenInfo.sin_addr.s_addr = htonl(INADDR_ANY);
	listenInfo.sin_family = AF_INET;
	listenInfo.sin_port = htons(LISTEN_PORT);
	SOCKET clientfd;
	SOCKADDR_IN clientInfo;

	SOCKET sockfd=socket(AF_INET,SOCK_STREAM,0);
	if (-1==bind(sockfd,(SOCKADDR*)&listenInfo,sizeof(SOCKADDR))) {
		cout << "error: bind wrong!" << endl;
		return 0;
	}
	listen(sockfd,10);
	int addrLen = sizeof(SOCKADDR);
	if ((clientfd=accept(sockfd,(SOCKADDR*)&clientInfo,&addrLen))<0) {
		cout << "error: accept error!" << endl;
	}
	char buffer[1024];
	recv(clientfd,buffer, RECV_BUFFER_LEN,0);
	cout << "Receive content: " << buffer << endl;

	closesocket(sockfd);
	closesocket(clientfd);

	return 1;
}

int main() {
	server();
	return 0;
}