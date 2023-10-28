
#include <sys/socket.h>
#include <errno.h>  //perror
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h> //close()

//线程头文件加上
#include <pthread.h>



//创建一个linux线程
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
    #if 0
    struct sockaddr_in clientaddr;//定义一个客户端
    socklen_t len = sizeof(clientaddr);
    int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
    
    printf("accept \n ");
    
   #if 0
     //定义buffer
    char buffer[128] = {0};
    int count = recv(clientfd,buffer,128,0); 
     /* count : 128 是什么状态*/
    send(clientfd,buffer,128,0);
     printf("sockfd:%d clientfd: %d,count:%d ,buffer:%s \n",sockfd,clientfd,count,buffer);
   #else
        while (1) {
             char buffer[128] = {0};
             int count = recv(clientfd,buffer,128,0); 
             if(count == 0)
             {
                 break;
             }
             /* count : 128 是什么状态*/
             send(clientfd,buffer,128,0);
             printf("sockfd:%d clientfd: %d,count:%d ,buffer:%s \n",sockfd,clientfd,count,buffer);
        }
   #endif
   #else
    while (1){
         struct sockaddr_in clientaddr;//定义一个客户端
    	 socklen_t len = sizeof(clientaddr);
    	 int clientfd = accept(sockfd,(struct sockaddr*)&clientaddr,&len);
         
        //定义一个线程ID
         pthread_t thid;
         pthread_create(&thid,NULL,client_thread,&clientfd);
    }
    
   #endif
    
    getchar(); 
    //close(clientfd); //关闭通讯
}

