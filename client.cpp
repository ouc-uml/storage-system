#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define SERVER_IP "47.93.218.237"
#define PORT 12345
#define MAXDATASIZE 512

unsigned trans_block_to_int(unsigned char block[],int start,int len){
    unsigned ret = 0;
    for (int i = start;i<start+len;i++)
        ret = (ret<<8)+block[i];
    return ret;
}

int main()
{

    int fd;
    int num;

    unsigned char recebuf[MAXDATASIZE];
    unsigned char sendbuff[MAXDATASIZE];
    char tmp[MAXDATASIZE];
    int number = 1;

    struct hostent *he;   //the struct that will get information about remote host
    struct sockaddr_in  server;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("create socket error!\n");
        exit(1);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    //server.sin_addr = *((struct in_addr *)he->h_addr);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    //start to conncet 
    if(connect(fd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        printf("connect error\n");
        exit(1);
    }


    //start to send message

    while (1){
        //start to receive message 
        //not blocking mode
        //int flags = fcntl(fd, F_GETFL, 0);   
        //fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        while (1){
        num = recv(fd, recebuf, MAXDATASIZE,0);
        if (num <= 0) exit(1);

        unsigned nn = trans_block_to_int(recebuf,0,4);
        unsigned nnn = nn>=MAXDATASIZE?(nn-MAXDATASIZE):nn;
        for (int i = 0;i<nnn;i++){
            putchar(recebuf[4+i]);
        }
        if (nn<MAXDATASIZE) break;
        }

        //start to send message
        fgets(tmp,MAXDATASIZE,stdin);
        memcpy(sendbuff,tmp,MAXDATASIZE);
        if (sendbuff[strlen(tmp)-1] == '\n')
            sendbuff[strlen(tmp)-1] = 0;
        if (!strcmp((const char *) sendbuff,"exit")) break;

        //printf("%s\n", sendbuff);
        num = send(fd, sendbuff, MAXDATASIZE, 0);

        if(num == -1)
        {
            printf("send message error\n");
        }
    }

    return 0;
}
