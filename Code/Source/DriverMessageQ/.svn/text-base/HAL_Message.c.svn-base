/*
 *  GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2009-2010 Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *    Intel Corporation
 *    2200 Mission College Blvd.
 *    Santa Clara, CA  97052
 *
 */



#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/skbuff.h>
#include <linux/timer.h>

#include <linux/connector.h>
#include <linux/net.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/kref.h>
#include <linux/net.h>
#include <net/tcp.h>

//Custom Header Files
#include "HAL_Message.h"


#define SHUTDOWN_SIGS   (sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGTERM))


/*
 * Sendbuffer sends "Length" bytes from "Buffer" through the socket "sock".
 */

extern struct socket *sock;

MessageQ_t MsgQ[MsgMax];


/* ------------------------------------------------------------------------- */
/* Functions.                                                                */
/* ------------------------------------------------------------------------- */

/******************************************************************************
 *
 * Name: HAL_Msg_Send
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Msg_Id  -
 *   Buffer     -
 *   Length     -
 *
 * Return Value:
 *   Status: Number of Bytes sent
 *
 *
 *****************************************************************************/


INT32 HAL_Msg_Send(INT32 Msg_Id, char *Buffer, INT32 Length)
{
  struct msghdr msg;
  struct kvec iov; // structure containing a base addr. and length
  sigset_t oldset;

  INT32 len;
  INT32 total;
  ULONG flags;
  PINT8 pBuffer = Buffer;

#if !TCP_SOCKET
  INT32 Port_No;
  struct sockaddr_in target;
#endif

  //struct socket *sock;


  //DPRINTK("Message pipe no %d ", MsgId);
  //sock = MSGGetSocket(MsgId);

  if(sock == NULL)
    return HAL_FAILED;

#if 1
  spin_lock_irqsave(&current->sighand->siglock, flags);
  oldset = current->blocked;
  sigfillset(&current->blocked);
  siginitsetinv(&current->blocked, SHUTDOWN_SIGS);
  recalc_sigpending();
  spin_unlock_irqrestore(&current->sighand->siglock, flags);
#endif

  total = 0x00;
  do{
    sock->sk->sk_allocation = GFP_NOIO;

    iov.iov_base = pBuffer;
    iov.iov_len = Length;

#if TCP_SOCKET
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
#else
    Port_No = MSGGetPort(Msg_Id);

    /* Now send to the destination socket */
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = htonl(0);
    target.sin_port = htons(Port_No);

    msg.msg_name = &target;
    msg.msg_namelen = sizeof(target);
#endif

    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = MSG_NOSIGNAL; //0/*MSG_DONTWAIT*/;


    /* Actual Sending of the Message */
    len = kernel_sendmsg(sock, &msg, &iov, 1, iov.iov_len);
    if(len<=0)
    {
      //TBD: DPRINTE
      DPRINTK("Error Server Socket Closed !!!!!!");
      break;
    }
    //Update the Index Pointers
    Length -= len;
    pBuffer += len;
    total += len;

  } while (Length > 0);

#if 1
  spin_lock_irqsave(&current->sighand->siglock, flags);
  current->blocked = oldset;
  recalc_sigpending();
  spin_unlock_irqrestore(&current->sighand->siglock, flags);
#endif

  DPRINTK("Sent %d bytes to socket 0x%08x", total, sock);
  return total;
}



struct socket * set_up_client_socket(unsigned int IP_addr, int port_no)
{
  struct socket * clientsock;
  int error;

#if TCP_SOCKET
  struct sockaddr_in sin;
  int i;
#endif

  /* First create a socket */
#if TCP_SOCKET
  error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&clientsock);
#else
  error = sock_create(PF_INET,SOCK_DGRAM,IPPROTO_UDP,&clientsock);
#endif
  if (error<0) {
    DPRINTE("Error during creation of socket %d", error);
    return 0;
  }

#if TCP_SOCKET
  /* Now bind and connect the socket */
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(IP_addr);
  sin.sin_port = htons(port_no);

#define MAX_RETRIES     10

  for(i=0;i<MAX_RETRIES;i++) {
    error = clientsock->ops->connect(clientsock,
        (struct sockaddr*)&sin,sizeof(sin),0);
    if (error<0) {
      DPRINTE("Error connecting to server: %i, retrying .. %d \n",error, i);
      if(i==MAX_RETRIES-1) {
        DPRINTE("Giving Up!\n"); return 0;
      }
    }
    else {break;} //connected
  }

#endif


  return clientsock;

}


INT32 DOCSMessageComInit(VOID)
{
  INT32 idx;

  for(idx=0; idx<MsgMax; idx++)
  {
    MSGGetSocket(idx) = NULL;
    MSGGetPort(idx) = (INT32)NULL;
  }

  return HAL_SUCCESS;
}

VOID DOCSMessageComRelease(VOID)
{
  INT32 idx;

  for(idx=0; idx<MsgMax; idx++)
  {
    if(MSGGetSocket(idx) != NULL)
    {
      sock_release(MSGGetSocket(idx));
      MSGGetPort(idx) = (INT32)NULL;
    }
    else
    {
      DPRINTE("NULL !!!!!!");
    }
  }

}

/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */

EXPORT_SYMBOL(HAL_Msg_Send);
//EXPORT_SYMBOL(HAL_Msg_Recv);



