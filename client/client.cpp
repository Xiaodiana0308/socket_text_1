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

int client(const char *ip_1,const int port_1,char *text)
{
    char text_out[Length];//每次发送数据最大大小
    memset(&text_out,0,sizeof(text_out));//开辟空间
    if(strlen(text)>Length){//数据超过1024字节则不执行
        std::cout<<"The data length exceeds "<<Length<<"B"<<std::endl;
        return -1;
    }
    else memcpy(&text_out,text,strlen(text));

    //创建客户端socket
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){//socket[固定参数]
        perror("socket");//输出错误格式
        return -1;
    }

    //缓冲区大小确认
    socklen_t optlen;    //选项值长度
    int snd_size = 0;    //输出缓冲区
    int rcv_size = 0;    //输入缓冲区
    int err;
    optlen = sizeof(snd_size);
    err = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,&snd_size, &optlen);
    if(err<0){
        printf("Get send buffer size error\n");
    }   
    optlen = sizeof(rcv_size);
    err = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);//[本机socket，SOL_SOCKET，SO_SNDBUF，缓冲区大小地址，缓冲区大小的大小的地址]
    if(err<0){
        printf("Get receive buffer size error\n");
    }
    std::cout<<"send buffer:"<<snd_size<<"B"<<std::endl;
    std::cout<<"receive buffer:"<<rcv_size<<"B"<<std::endl;
    //改变snd_size、rcv_size的值，然后调用getsocketopt函数，来改变缓冲区大小
    
    //创建服务端网络地址
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));//开辟空间并初始化，注意该函数第一个参数需要地址类型，故结构体类型需要&
    struct hostent *h;//服务端hostent结构体
    h=gethostbyname(ip_1);//直接获得服务端网络相关信息
    servaddr.sin_port=htons(port_1);//端口信息
    servaddr.sin_family=AF_INET;//协议族，规定好的
    memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);//注意：servaddr.sin_addr与h->h_addr数据类型不一样，但可以互相转化。h->h_addr等价于h->h_addr_list[0]

    //发起连接请求
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0){//connect[本机socket，对方网络地址(sockaddr *),地址大小]
        perror("connect");
        close(sockfd);
        return -1;
    }
    else std::cout<<"Connection Establishment"<<std::endl;

    //与服务端通信，发送数据
    int iret;//数据长度
    if((iret=send(sockfd,text_out,strlen(text_out),0))<=0){//send[本机socket，文件地址，文件大小]
        printf("iret=%d\n",iret);
        perror("send");//使用strlen是因为sizeof必须为固定大小数据，而前者可以接收不确定大小数据，但类型必须为char *
        close(sockfd);
        return -1;
    }

    //接收数据
    memset(text_out,0,sizeof(text_out));
    if((iret=recv(sockfd,text_out,sizeof(text_out),0))<=0){//recv[本机socket，文件地址，文件大小]
        printf("iret=%d\n",iret);
        perror("recv");
        close(sockfd);
        return -1;
    }
    std::cout<<text_out<<std::endl;

    //关闭通信，释放资源
    close(sockfd);
    return 0;
}

int main()
{
    const char *ip1="10.203.172.241";
    const int port1=5005;
    char *text=(char *)"hello,network1";
    client(ip1,port1,text);
    return 0;
}