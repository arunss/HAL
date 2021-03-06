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
*--------------------------------------------------------------------------*/
#ifndef DOCS_HAL_DMA_H_
#define DOCS_HAL_DMA_H_

#include "HAL_Interface.h"
#include "CrePeHAL.h"
#include "HAL_Dma_Defines.h"

#include "Msg_Mod_Ids.h"


#define MAX_BUF_NO                                 (0x04)


//RBU-Fix-27Apr, Franklin, This value was tested for 50/250
#define MAP_DROP_CNT                                (10)

#define DMA_SIZE                                     (0x400)
#define MAC_SIZE                                     (0xfff)
#define MAX_DMA_CONTEXT                              (0x13)

#define US_DMA_CONTEXT_MIN                           (8)
#define US_DMA_CONTEXT_MAX                           (11)
#define DS_DMA_CONTEXT_MIN                           (12)
#define DS_DMA_CONTEXT_MAX                           (13)


#define PACKET_SIZE                                 (2*1024)
#define DMA_QEUE_SIZE_RX_MGMT                       (2*1024)
#define DMA_QEUE_SIZE_RX_DATA                       (2*1024)

#define DMA_FIFO_SIZE                               (2*1024)


typedef struct _DMADesc_t
{
    UINT32 NextDescr;
    UINT32 StartAddr;
    UINT32 ContextParam;
    UINT32 ModeControl;
    UINT32 CurrDescr;
    UINT32 Reserved1;
    UINT32 Reserved2;
    UINT32 Reserved3;

}DMADesc_t, *PDMADesc_t;

typedef struct _DescrValue_t
{
    UINT32 addr;
    UINT32 len;
    //UINT32 CntrlVal;
}DescrValue_t,*PDescrValue_t;

//<RBU Fix, Date 17-APR-2009, AShah, DMA Internal FIFO Overflow Handle
typedef struct _DMAPrivate
{
  struct tasklet_struct   rx_tasklet_data;
  struct tasklet_struct   rx_tasklet_manage;
  INT32 DataCount;
  INT32 ManageCount;
  INT32 Map_Count;
  INT32 DmaRegBase;
  INT32 IntRegBase;
  INT32 rx_count;

  spinlock_t rx_lock;
  spinlock_t int_lock;


  struct kfifo *Dma_Fifo[MAX_DMA_CONTEXT];
  spinlock_t Dma_Fifo_Lock[MAX_DMA_CONTEXT];
  INT8 Fifo_Full[MAX_DMA_CONTEXT];

  struct task_struct *Thread_Info[2];
  INT32 Ds_Path_Mask;
}DMAPrivate, *PDMAPrivate;

typedef struct Dma_Buff_t{
    UINT32 Addr;
    UINT32 Len;
}Dma_Buff_t,* PDma_Buff_t;

INT32 HAL_Dma_Setup (VOID);
INT32 HAL_Dma_Release (VOID);

VOID HAL_Dma_Desc_Init (INT32 context);
VOID HAL_Dma_Desc_Create_Chain(UINT32  Descr_Addr);

INT32 HAL_Dma_Update_Prev_Desc_Node(INT32 context,UINT32 DescNodeAddr);
UINT32 HAL_Dma_Get_Desc_Node( UINT32 CurrDescAddr );
INT32 HAL_Dma_Fill_Desc_Node(INT32 Context,
                             UINT32 DescNodeAddr,
                             UINT32 BuffAddr,
                             UINT32 len);


INT32 HAL_Dma_Trigger_Dma(INT32 context,UINT32 DescNodeAddr);
VOID HAL_Dma_Desc_Start(INT32 context, UINT32 Descr_Addr);
VOID HAL_Dma_Reg_Start(INT32 context, UINT32 BuffAddr, UINT32 Len);
//INT32 HAL_Dma_Send(INT32 context,UINT32 Buff_Addr,UINT32 len);

INT32 HAL_Dma_Late_Bind(INT32 context);
INT32 HAL_Dma_Reset(INT32 context);
INT32 HAL_Dma_Is_Busy(INT32 context);
INT32 HAL_Dma_Hold(INT32 context);
INT32 HAL_Dma_Unhold(INT32 context);
INT32 HAL_Dma_Int_Clear(INT32 context);

VOID HAL_Dma_Init_Private(VOID);

INT32 HAL_Dma_Fill_All_Desc_Nodes(INT32 context);
INT32 HAL_Dma_Kick_Start(INT32 context);
VOID HAL_Dma_Get_Buffer(INT32 context, PUINT32 BufferAddr, PINT32 BuffLen);
INT32 HAL_Dma_Clear(INT32 context);

INT32 HAL_Dma_Int_Mask(INT32 Int_Mask);

/* -------------------------------------------------------------------------- */
/* Prototypes.                                                                */
/* -------------------------------------------------------------------------- */

//INT32 Send_Message (Mod_Id_t Dest_Mod, Msg_Struct_t* pMsg);
INT32 HAL_EnQueue_Buffer(INT32 Buffer, INT32 Length);

#endif /*DOCS_HAL_DMA_H_*/
