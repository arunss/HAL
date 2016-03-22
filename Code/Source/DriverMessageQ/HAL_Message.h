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


/*-----------------------------------------------------------------------------
 * PURPOSE
 *       This header file defines data types and prototype for DMA
 *
 *----------------------------------------------------------------------------*/
#ifndef DOCS_HAL_MSG_H_
#define DOCS_HAL_MSG_H_

#include "HAL_Interface.h"
#include "HAL_Message_Defines.h"

#define MSGGetSocket(MsgId)     MsgQ[MsgId].ComSocket
#define MSGGetPort(MsgId)       MsgQ[MsgId].PortNo

/* ------------------------------------------------------------------------ */
/* Type definitions.                                                        */
/* ------------------------------------------------------------------------ */

typedef struct _MessageQ_t
{
  struct socket *ComSocket;
  INT32 PortNo;

}MessageQ_t, *PMessageQ_t;



INT32 DOCSMessageComInit(VOID);
VOID DOCSMessageComRelease(VOID);


struct socket* set_up_server_socket(int port_no);
struct socket* server_accept_connection(struct socket *sock);
struct socket * set_up_client_socket(unsigned int IP_addr, int port_no);




#endif /*DOCS_HAL_MSG_H_*/
