#include <sys/socket.h>
#include <errno.h>  //perror
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h> //close()

//线程头文件加上
#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>

#define BUFFER_LENGTH 128

struct conn_item{

	int fd;
	char buffer[BUFFER_LENGTH];
	int idx;

};

struct conn_item connlist[1024] = {0};



//创建一个linux线程
//block
void *client_thread(void *arg)
{
    int clientfd = *(int *)arg;
    
    while (1) {
     char buffer[BUFFER_LENGTH] = {0};
     int count = recv(clientfd,buffer,128,0); 
     if(count == 0)
     {
         break;
     }
     /* count : 128 是什么状态*/
     send(clientfd,buffer,128,0);
     printf(" clientfd: %d,count:%d ,buffer:%s \n",clientfd,count,buffer);
    }
    close(clientfd);
}

//TCP SERVER 
int main(){
  	int sockfd = socket(AF_INET,SOCK_STREAM,0);
  	struct sockaddr_in serveraddr;
  	memset(&serveraddr,0,sizeof(struct sockaddr_in));
  	//绑定本地
  	serveraddr.sin_family = AF_INET;
  	//本地地址  ANY任意地址
  	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	// 1024以前的端口是系统在用，
    serveraddr.sin_port = htons(2048);
    //绑定  
    //
	if(-1 == bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr)))
    {
      perror("bind");
      return -1;
    }
    
    listen(sockfd,10);

	//创建一个固定大小
	
	int epfd = epoll_create(1);// int size

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;

	//添加哪个sockfd，的哪个事件
	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);

	struct epoll_event events[1024] = {0};
 	while (1) {
		int nready = epoll_wait(epfd,events,1024,-1);
		int i = 0;
		for(i = 0;i < nready;i ++){
			int connfd = events[i].data.fd;//
			if(sockfd == connfd){
				struct sockaddr_in clientaddr;
				socklen_t len = sizeof(clientaddr);
				int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
				
				ev.events = EPOLLIN ;//边沿触发在EPOLLIN | EPOLLET
				ev.data.fd = clientfd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,clientfd,&ev);

				connlist[clientfd].fd = clientfd;
				memset(connlist[clientfd].buffer,0,BUFFER_LENGTH); // = clientfd;
				connlist[clientfd].idx = 0;
				
				printf("clientfd:%d",clientfd);
			}else if (events[i].events & EPOLLIN){

				char *buffer  = connlist[connfd].buffer;
				int idx = connlist[connfd].idx;
				
				int count = recv(connfd,buffer,BUFFER_LENGTH - idx,0);
				printf("clientfd:%d,count:%d,buffer:%s\n",connfd,count,buffer);

				if(count == 0){
					printf(" disconnect \n");
						
					epoll_ctl(epfd,EPOLL_CTL_DEL,connfd,NULL);
					close(i);

					continue;
				}
				connlist[connfd].idx += count;
				send(connfd,buffer,count,0);
				printf("clientfd:%d,count:%d,buffer:%s\n",connfd,count,buffer);
			}
		}
	}
	
	
    getchar(); 
    //close(clientfd); //关闭通讯
}
