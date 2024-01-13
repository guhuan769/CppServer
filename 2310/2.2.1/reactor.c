#include <sys/socket.h>
#include <errno.h>  //perror
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h> //close()

//线程头文件加上111
#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>


//input command  `man fstat` for `ubuntu server`
#include <sys/types.h>
#include <sys/stat.h>

// input command  `man open` for `ubuntu server`
#include <fcntl.h>


//定义一个宏
#define ENABLE_HTTP_RESPONSE	1




#define BUFFER_LENGTH 128

//event
typedef int (*RCALLBACK)(int fd);

//listenfd
//触发EPOLLIN-->
int accept_cb(int fd);
//clientfd
int recv_cb(int fd);
//可写
int send_cb(int fd);

//connection conn,fd,buffer,callback
struct conn_item{

	int fd;


	char rbuffer[BUFFER_LENGTH];
	int rlen;
	char wbuffer[BUFFER_LENGTH];
	int wlen;

	//定义一个资源
	char resource[BUFFER_LENGTH];//index.htm
	
	//做了一个联合
	union {
		RCALLBACK accept_callback;
		RCALLBACK recv_callback;
	}recv_t;
	RCALLBACK send_callback;
};
// libevent -- > 多线程

int epfd = 0;
struct conn_item connlist[1024] = {0};

#if ENABLE_HTTP_RESPONSE

#define ROOT_DIR	"/home/elonsnyder/share/2.1.2-multi-io/"

typedef struct conn_item connection_t;

// http://192.168.101.5:5555/index.htm
int http_request(connection_t *conn){
	//conn->rbuffer
	//conn->wlen
	
	return 0;
}


int http_response(connection_t *conn){
	//connection
#if 1
	conn->wlen = sprintf(conn->wbuffer ,
						"HTTP/1.1 200 OK\r\n"
						"Accept-Ranges: bytes\r\n"
						"Content-Length: 82\r\n"
						"Content-Type: text/html\r\n"
						"Date: Sat, 06 Aug 2022 13:16:46 GMT\r\n\r\n"
						"<html><head><title>0voice.King</title></head><body><h1>King</h1></body></html>\r\n\r\n");
	
	

#else
	int filefd = open("index.html",O_RDONLY);//只读的方式

	struct stat stat_buf;
	fstat(filefd,&stat_buf);
	conn->wlen = sprintf(conn->wbuffer ,
						"HTTP/1.1 200 OK\r\n"
						"Accept-Ranges: bytes\r\n"
						"Content-Length: %ld\r\n"
						"Content-Type: text/html\r\n"
						"Date: Sat, 06 Aug 2022 13:16:46 GMT\r\n\r\n",stat_buf.st_size);
	
	int count = read(filefd,conn->wbuffer + conn->wlen,BUFFER_LENGTH-conn->wlen);
	conn->wlen += count;

	//send fiel
#endif
	return conn->wlen;
}

#endif


//创建一个linux线程
//block
void *client_thread(void *arg)
{
    int clientfd = *(int *)arg;
    
    while (1) {
     char buffer[128] = {0};
     int count = recv(clientfd,buffer,128,0); 
     if(count == 0)
     {
         break;
     }
     /* count : 128 是什么状态*/
     send(clientfd,buffer,count,0);
     printf(" clientfd: %d,count:%d ,buffer:%s \n",clientfd,count,buffer);
    }
    close(clientfd);
}

int set_event(int fd,int event,int flag){
	/*//1 add 0 mod(修改)*/
	if(flag){ 
		
		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
	}else{
		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&ev);
	}
	
}

int accept_cb(int fd){
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int clientfd = accept(fd,(struct sockaddr*)&clientaddr,&len);

	if(clientfd < 0){
		return -1;
	}

	set_event(clientfd,EPOLLIN,1);
		
	connlist[clientfd].fd = clientfd;
	memset(connlist[clientfd].rbuffer,0,BUFFER_LENGTH); // = clientfd;
	connlist[clientfd].rlen = 0;

	memset(connlist[clientfd].wbuffer,0,BUFFER_LENGTH); // = clientfd;
	connlist[clientfd].wlen = 0;
	
	connlist[clientfd].recv_t.recv_callback = recv_cb;
	connlist[clientfd].send_callback = send_cb;

	return clientfd;
}

// 1.recv
// 2.buffer -->
int recv_cb(int fd){ //fd --> EPOLIN
	char *buffer  = connlist[fd].rbuffer;
	int idx = connlist[fd].rlen;

	int count = recv(fd,buffer+idx,BUFFER_LENGTH-idx,0);
	//int count = recv(fd,buffer+idx,BUFFER_LENGTH-idx,0);
	
	if(count == 0){
		printf(" disconnect \n");

		epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL);
		close(fd);
		
		return -1;
	}
	connlist[fd].rlen += count;
	#if 1 // echo:  need to send
	memcpy(connlist[fd].wbuffer,connlist[fd].rbuffer,connlist[fd].rlen);

	connlist[fd].wlen = connlist[fd].rlen;
	connlist[fd].rlen -= connlist[fd].rlen;
	#else 
	//http_request
	http_request(&connlist[fd]);
	http_response(&connlist[fd]);
	#endif
	set_event(fd,EPOLLOUT,0);
	return count;
}


int send_cb(int fd)
{
	
	char *buffer  = connlist[fd].wbuffer;
	int idx = connlist[fd].wlen;

	int count = send(fd,buffer,idx,0);

	set_event(fd,EPOLLIN,0);
	return count;
	
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

	//
	connlist[sockfd].fd = sockfd;
	connlist[sockfd].recv_t.accept_callback = accept_cb;

	//创建一个固定大小
	epfd = epoll_create(1);// int size
	
	/*

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	//添加哪个sockfd，的哪个事件
	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);*/
	
	set_event(sockfd,EPOLLIN,1);
	
	struct epoll_event events[1024] = {0};
 	while (1) { //  mainloop();
		int nready = epoll_wait(epfd,events,1024,-1);
		int i = 0;
		for(i = 0;i < nready;i ++){
			int connfd = events[i].data.fd;//
			
			if (events[i].events & EPOLLIN){
			
				int count = connlist[connfd].recv_t.recv_callback(connfd);
				printf("recv count:%d <-- buffer:%s \n",count,connlist[connfd].rbuffer);
			}else if (events[i].events & EPOLLOUT){
				//int count = send_cb(connfd);
				printf("send --> buffer :%s \n",connlist[connfd].wbuffer);

				int count = connlist[connfd].send_callback(connfd);
				
			}
		}
	}
	
	
    getchar(); 
    //close(clientfd); //关闭通讯
}
