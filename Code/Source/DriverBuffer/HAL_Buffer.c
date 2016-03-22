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
*  Brief Description of the overall implementation strategy used in the module.
*  Platform specific implementation details and optimization s
*  trategies (if any)
*  should be described.
*
*---------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------- */
/* Headers.                                                                */
/* ----------------------------------------------------------------------- */

//Standard Header Files
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>

//Custom Header Files
#include "HAL_Buffer.h"

//Include External Interface


/* ----------------------------------------------------------------------- */
/* Macros.                                                                 */
/* ----------------------------------------------------------------------- */


/* ----------------------------------------------------------------------- */
/* Global Variable.                                                         */
/* ----------------------------------------------------------------------- */

PktPool_t PacketPool;

UINT32 PacketId=0x00;
extern PUINT32 buffer_pool_base_addr;
extern PUINT32 buffer_base_addr;
extern PUINT32 buffer_last_addr;
/* ----------------------------------------------------------------------- */
/* Internal prototypes.                                                    */
/* ---------------------------------------------------------------------- */

//==========================================================================

    /*----    DOCS_POOL_OSM_Setup    -------------------------
     * 1. Create the Buffer pool.
     * 2. Create the free pkt list.
     *
     *----------------------------------------------------*/
INT32 DOCS_POOL_OSM_Setup()
{
    UINT32 pkt;

        /* 1. Reserve testbuffer memory */

    if(!(OSM_MEM_RESERVE(POOL_BUFFER_BASE_ADDRESS, POOL_BUFFER_SIZE,
         DRIVER_NAME)) )
    {
      DPRINTE("Cant access region %x\n", POOL_BUFFER_BASE_ADDRESS);
      unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
      return HAL_FAILED;
    }

    /* 2. Remap testbuffer region  */
    buffer_pool_base_addr=(PUINT32)OSM_MEM_REMAP(POOL_BUFFER_BASE_ADDRESS,\
                                                POOL_BUFFER_SIZE);
    DPRINTK("PP phy base addr %x :size %x \n log base addr %x\n ",\
            POOL_BUFFER_BASE_ADDRESS, POOL_BUFFER_SIZE,
            (UINT32)buffer_pool_base_addr );

    buffer_base_addr =  buffer_pool_base_addr;

    /* start of the free list. */

    PacketPool.free_list_head = PacketPool.free_list_tail = \
                                      (Packet_t *)buffer_pool_base_addr;

    DPRINTK("free_list_tail addr %8x :free_list_head %8x", \
                   PacketPool.free_list_tail, \
                   PacketPool.free_list_head);


    /* Initializing the pointer to packet */
    for (pkt = 0; pkt < (PACKET_NO - 1); pkt++)
    {
      DPRINTK("PacketPool.free_list_tail addr %8x : Nxt addr %8x", \
                     PacketPool.free_list_tail, \
                     PacketPool.free_list_tail->next_packet);

      PacketPool.free_list_tail->next_packet =
           (UINT32 *)((UINT32)(PacketPool.free_list_tail) + (sizeof(Packet_t)));

      PacketPool.free_list_tail =
           (Packet_t *)((UINT32)(PacketPool.free_list_tail)+(sizeof(Packet_t)));

      PacketPool.free_list_count++;
    }
    PacketPool.free_list_tail->next_packet = (UINT32 *)NULL;

    buffer_last_addr =  (PUINT32)(PacketPool.free_list_tail);

    spin_lock_init(&PacketPool.int_lock);

    return HAL_SUCCESS;
}

//==========================================================================

    /*----    DOCS_POOL_OSM_GetPkt    -------------------------
     * 1. Fetch the immediate free pkt available from the free list head.
     * 2. Error in not available free pkt in list.
     *----------------------------------------------------*/

INT32 DOCS_POOL_OSM_GetPkt(PINT32 Packet)
{
    ULONG Flags;
    INT32 retval = HAL_SUCCESS;


    if(PacketPool.free_list_head == NULL)
    {
      retval = HAL_FAILED;
      *Packet = (INT32)NULL;
      DPRINTE(" EMPTY list %u",PacketPool.free_list_count);
      return retval;
   }
    spin_lock_irqsave(&PacketPool.int_lock, Flags);

    if(PacketPool.free_list_head == PacketPool.free_list_tail)
    {
        *Packet=(INT32)PacketPool.free_list_head;
        // last packet in the list.
        PacketPool.free_list_head = PacketPool.free_list_tail=(Packet_t *)NULL;
    }
    else
    {
      *Packet=(INT32)PacketPool.free_list_head;
      PacketPool.free_list_head =
            (Packet_t *)(PacketPool.free_list_head)->next_packet;
    }

    spin_unlock_irqrestore(&PacketPool.int_lock, Flags);

    PacketPool.free_list_count--;

   DPRINTK(" Packet %08x", *Packet);
   return retval;
}

EXPORT_SYMBOL(DOCS_POOL_OSM_GetPkt);

//==========================================================================

    /*----    DOCS_POOL_OSM_FreePkt    -------------------------
     * 1. Add the free pkt available into the tail in the free list.
     *----------------------------------------------------*/

INT32 DOCS_POOL_OSM_FreePkt(INT32 Packet)
{
  INT32 retval = HAL_SUCCESS;
  ULONG Flags;

  DPRINTK(" Packet %08x", Packet);
  // If the free list is empty then add to the head.
  spin_lock_irqsave(&PacketPool.int_lock, Flags);

  if(PacketPool.free_list_head == NULL)
        {
          PacketPool.free_list_head = PacketPool.free_list_tail =
            (Packet_t *)Packet;
        }
        else
        {
          // Update the previous tail packet with the "next" packet.
          PacketPool.free_list_tail->next_packet = (UINT32 *)Packet;
          // Update tail to the new packet.
          PacketPool.free_list_tail = (Packet_t *)Packet;
          PacketPool.free_list_tail->next_packet = (UINT32 *)NULL;
        }
  spin_unlock_irqrestore(&PacketPool.int_lock, Flags);

        PacketPool.free_list_count++;
    return retval;

}

EXPORT_SYMBOL(DOCS_POOL_OSM_FreePkt);

//==========================================================================

   /* ----    DOCS_POOL_OSM_Release()    ----
   *  1. Un Map Memory  packet pool
   *  2. Release Memory  packet pool
   *----------------------------------*/
INT32 DOCS_POOL_OSM_Release()
{


    // 1. Un Map Memory packet pool
  OSM_MEM_UNMAP((void *)buffer_pool_base_addr);

  // 2. Release Memory  packet pool
  OSM_MEM_RELEASE(POOL_BUFFER_BASE_ADDRESS, POOL_BUFFER_SIZE);

  return HAL_SUCCESS;
}




