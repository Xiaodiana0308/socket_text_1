#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <assert.h>
#define Length 1024

int server(int port_2,char *text)
{
    char text_in[Length];//每次接收数据大小
    memset(text_in,0,sizeof(text_in));//初始化

    //创建服务端
    int listenfd;
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){//socket[固定参数]
        perror("socket");
        return -1;
    }

    //创建服务端网络地址
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));//开辟空间并初始化[类型指针，0，类型大小]
    servaddr.sin_family=AF_INET;//协议族
    servaddr.sin_port=htons(port_2);//服务端端口，相关函数：htonl，ntohs，ntohl
    // std::cout<<"网络端口为"<<ntohs(servaddr.sin_port)<<std::endl;//【测试函数-输出】
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);//服务端网络地址，任意，只有这个函数参数对应最里层参数。
    // servaddr.sin_addr.s_addr=inet_addr("192.168.1.1");//指定地址【测试函数】注意：仅inet_addr()函数直接对应.s_addr，其余到结构体一层即可
    // inet_aton("192.168.1.2",&servaddr.sin_addr);//【测试函数】
    // std::cout<<"网络地址ip为"<<inet_ntoa(servaddr.sin_addr)<<std::endl;//【测试函数-输出】

    //绑定端口
    if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){//bind[本机socket，本机网络地址(sockaddr *)，地址大小]
        perror("bind");
        close(listenfd);
        return -1;
    }

    //设置为监听状态
    if(listen(listenfd,5)!=0){//[本机socket，0-50]
        perror("listen");
        close(listenfd);
        return -1;
    }

    //接收客户端的连接
    int clientfd;//客户端的socket
    int socklen=sizeof(struct sockaddr_in);//存储struct sockaddr_in的大小，accept函数需要
    struct sockaddr_in clientaddr;//客户端网络地址，未指定，由之后的accept来确定。
    if((clientfd=accept(listenfd,(struct sockaddr *)&clientaddr,(socklen_t *)&socklen))==-1){//accept[本机socket，对方网络地址，网址长度]
        perror("accept error!");
        close(listenfd);
        close(clientfd);
        return -1;
    }
    std::cout<<"Connection Establishment"<<std::endl;

    //接收数据
    int iret;//接收数据大小
    if((iret=recv(clientfd,text_in,sizeof(text_in),0))<=0){//recv[对方socket，文件地址，文件大小]
        printf("iret=%d\n",iret);
        perror("recv");
        close(listenfd);
        close(clientfd);
        return -1;
    }
    std::cout<<"Receive data:"<<text_in<<std::endl;
    memcpy(&text,text_in,sizeof(text_in));

    //返回确认
    memset(text_in,0,sizeof(text_in));
    strcpy(text_in,"ACK1");//数据写入空字符串
    if((iret=send(clientfd,text_in,strlen(text_in),0))<=0){//send[对方socket，文件地址，文件大小]
        printf("iret=%d\n",iret);
        perror("send");
        close(listenfd);
        close(clientfd);
        return -1;
    } 
    close(listenfd);
    close(clientfd);
    return 0;
}

int main()
{
    const int port1=5005;
    char *text;
    server(port1,text);
    std::cout<<text<<std::endl;
    return 0;
}