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


/*----------------------------------------------------------------------------
* PURPOSE
*  This header file defines data types and prototype for Packet pool .
*
*--------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------- */
/* Headers.                                                                */
/* ----------------------------------------------------------------------- */
#ifndef __DOCS_HAL_PACKET_H_
#define __DOCS_HAL_PACKET_H_

//Custom Header Files
#include "HAL_Interface.h"
#include "HAL_Buffer_Defines.h"


/* ----------------------------------------------------------------------- */
/* Macros.                                                                 */
/* ----------------------------------------------------------------------- */

#define PKT_CTRL_RELEASE                    (0x01)
#define PKT_CTRL_RESERVE                    (0x02)
#define PKT_STAT_GET_BASE                   (0x03)
#define PKT_STAT_GET_PACKET                 (0x04)
#define PKT_CTRL_SET_PID                    (0x05)
#define PKT_CTRL_INIT                       (0x06)
#define PKT_STAT_GET_DBGBASE                (0x07)

#ifndef DESC_BUFFER_BASE_ADDRESS
  #define DESC_BUFFER_BASE_ADDRESS          (0x80000000)
  #define DESC_BUFFER_POOL_SIZE             (0x00)
#endif  


#define POOL_BUFFER_BASE_ADDRESS            DESC_BUFFER_BASE_ADDRESS + \
                                            DESC_BUFFER_POOL_SIZE
//#define POOL_BUFFER_SIZE                    (PACKET_NO * sizeof(Packet_t))
#define POOL_BUFFER_SIZE                    (0xFFFFFF)


#define PACKET_NO                           (4000)
#define PACKET_SIZE                         (2*1024)
//#define PACKET_SIZE                         (1*1024)
#define POOL_SIZE                           (PACKET_SIZE * PACKET_NO)

#define BUFFER_SIZE                         (1 * PAGE_SIZE)

//#define POOL_BUFFER_SIZE                    (POOL_SIZE + BUFFER_SIZE)

#define DRV_TEST                            HAL_ENABLE
#define INT8_BIT_MAPPING                    HAL_DISABLE


#define GET_BASE_ADDR()                     buffer_base_addr
#define GET_POOL_SIZE()                     POOL_SIZE
//#define GET_DBG_BASE()                      PacketPool.packet[PACKET_NO]

/* Packet Structure */
typedef struct _Packet_t{
  UINT8 packet[PACKET_SIZE - 4]; 
  UINT32 *next_packet;
}Packet_t,*PPacket_t;

/* Packet Pool list*/
typedef struct _PktPool_t
{
    Packet_t *free_list_head;
    Packet_t *free_list_tail;
    UINT32 free_list_count;
    spinlock_t int_lock;
}PktPool_t, *PPktPool_t;



/* Packet Signature */
typedef struct _PacketSign_t{
  UINT32 PktAddr;
  UINT32 PktSize;
  UINT32 PktId;
  UINT32 s_tv_sec;
  UINT32 s_tv_usec;
  UINT32 s_jiffies;
  UINT32 e_tv_sec;
  UINT32 e_tv_usec;
  UINT32 e_jiffies;
  UINT32 status_rsv;    
}PacketSign_t,*PPacketSign_t;

/* Packet Status */
typedef enum {
    PKT_FREE = 0x00,
    PKT_RESV = 0x01,        
}Pkt_Status_t;



#endif //DOCS_HAL_PACKET_H
/* ----------------------------------------------------------------------- */
/* Internal prototypes.                                                    */
/* ----------------------------------------------------------------------- */




