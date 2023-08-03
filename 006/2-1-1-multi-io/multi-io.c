
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(){
	//open
	int sockfd = socket(AF_INET,SOCK_STREAM,0);//io
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(struct sockaddr_in)); // 192.168.2.123
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
	servaddr.sin_port = htons(9999);

	if(-1 == bind(sockfd,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)))
	{
		printf("bind failed:%s",strerror(errno));
		return -1;  
	}
	listen(sockfd,10);
	#if 0
	//非阻塞IO
	fcntl(sockfd,F_GETFL,0);
	//accept 相当于一个服务员   等待客户端的连接再进去
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
	
	#endif
	getchar(); //空格卡住方便查询端口号 输入一个字符再往下走
	//此处延生到一个新问题  阻塞IO 非阻塞IO      
	//阻塞就是等待某一条件的满足
}

