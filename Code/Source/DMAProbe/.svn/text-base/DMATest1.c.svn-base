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


/* ---------------------------------------------------------------------- */
/* Headers.                                                               */
/* ---------------------------------------------------------------------- */


//Standard Header Files
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>


//Custom Header Files
#include "HAL_Interface.h"
#include "DMATest.h"




static INT32 Fill_Memory_Buffer(UINT32 buffaddr, PUINT32 PBuff_len);


INT32 DOCS_DMA_Test1(INT32 ctxt, INT32 mode, INT32 times)
{
    UINT32 Buff_Addr;
    UINT32 Buff_len;

    //ENable all interrupt
    /*HAL_Reg_Write(DOC_INT_REGISTER_BASE_ADDRESS + 0x4 , 0xFFFFFFFF, \
               HAL_REG_INT);*/
    while(times)
    {
        DOCS_POOL_OSM_GetPkt(&Buff_Addr);

        Fill_Memory_Buffer(Buff_Addr, &Buff_len);

        if(mode == REGISTER)
            HAL_Dma_Reg_Start(ctxt, Buff_Addr, Buff_len);
        else
            HAL_Dma_Send(ctxt,Buff_Addr,Buff_len);

        times--;
    }
    
    return HAL_SUCCESS;
}


static INT32 Fill_Memory_Buffer(UINT32 buffaddr, PUINT32 PBuff_len)
{
    INT32 index;
    PUINT32 pbuf;

    UINT8 databuff[]={
                          //#include "tlv1.txt"
                          #include "burstch0.txt"
                     };

    pbuf =  (PUINT32)databuff;
    *PBuff_len = sizeof(databuff);

    DPRINTK("Buffer Transfer Length: %d", *PBuff_len);

  for(index =0;index <*PBuff_len;index+=4)
  {
      writel(*pbuf,buffaddr);
      //DPRINTK("buffaddr : %0x,value :%0x", buffaddr,*pbuf);
      buffaddr+=4;
      pbuf++;
    }
    return HAL_SUCCESS;
}
