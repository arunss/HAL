/*
Test code : Added by AShah : to set up streaming server
 */



/*****************************************************************************/
/*** simple-server.c                                                       ***/
/***                                                                       ***/
/*****************************************************************************/

/**************************************************************************
*   This is a simple echo server.  This demonstrates the steps to set up
*   a streaming server.
**************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <iostream>


//Include External Interfaces
#include "HAL_App_Interface.h"

#define MY_PORT     9999
#define MAXBUF      1024*2


//Global Variable Goes Here
extern INT32 driver_fd[DrvMax];

int sockfd=NULL;



INT32 create_server()
{

    struct sockaddr_in self;

    //Creating Server Socket
#if TCP_SOCKET
    /*---Create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("Socket");
        return HAL_FAILED;
    }
#else
    /*---Create UDP socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 )
    {
        perror("Socket");
        return HAL_FAILED;
    }
#endif
    /*---Initialize address/port structure---*/
    bzero(&self, sizeof(self));

    self.sin_family = AF_INET;
    self.sin_port = htons(MY_PORT);
    self.sin_addr.s_addr = INADDR_ANY;

    printf("Binding to socket\n");
    /*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
    {
        perror("socket--bind");
        return HAL_FAILED;
    }

#if TCP_SOCKET
    printf("Listening to socket\n");

    /*---Make it a "listening socket"---*/
    if ( listen(sockfd, 20) != 0 )
    {
        perror("socket--listen");
        return HAL_FAILED;
    }
    printf("Connected From the Application\n");
    fflush(stdout);
#endif
}

INT32 create_clients()
{
    msgArg MsgArg;

    MsgArg.msgIdx = 0x00;
    MsgArg.PortNo = 9999;

    ioctl(driver_fd[DrvMsg], CMD_MSG_SET_PORT, &MsgArg);

}


INT32 listen_clients()
{
    char buffer[MAXBUF];
    int *pBuffer;
    int idx=0;

    while (1)
    {
      int clientfd;
      struct sockaddr_in client_addr;
      int addrlen=sizeof(client_addr);

      int len=0;
      int recvLoop=0;


#if TCP_SOCKET
      /*---accept a connection (creating a data pipe)---*/
      clientfd = accept(sockfd, (struct sockaddr*)&client_addr,
                          (socklen_t*)&addrlen);
      printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr),
                                  ntohs(client_addr.sin_port));
#else
      clientfd = sockfd;
#endif

      while(1)
      {

        /*---Echo back anything sent---*/
#if TCP_SOCKET
        len = recv(clientfd, buffer, MAXBUF, 0);
#else
        len = recvfrom(clientfd, buffer, MAXBUF, 0, (sockaddr*)&client_addr, \
                                                      (socklen_t*)&addrlen);
#endif
        if (len<=0)
            break;
        pBuffer =  (PINT32)buffer;

        if(len >= 0)
          printf("Recieved  %d Length  %d\n", recvLoop, len);

        recvLoop++;
#if 0
        for(idx=0;idx<len;idx++)
        {
          printf("Buffer %d Content %x\n", idx, pBuffer[idx]);
        }
#endif
      }
        /*---Close data connection---*/
      close(clientfd);
    }
}

