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
*  Has memory request/map and unmap for DMA register region.
*   DMA ISR handler, descriptor build and start routines.
*--------------------------------------------------------------------------*/


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
#include <linux/spinlock.h>
#include <linux/kfifo.h>
#include <linux/delay.h>
#include <linux/kthread.h>

//Custom Header Files
#include "HAL_Dma.h"

#define HTONL(A) ((((UINT32)(A) & 0xff000000) >> 24) | \
    (((UINT32)(A) & 0x00ff0000) >> 8)  | \
    (((UINT32)(A) & 0x0000ff00) << 8)  | \
    (((UINT32)(A) & 0x000000ff) << 24))

#define NTOHL  HTONL

UINT32 MAP_Count = 0;
UINT32 MAP_Data_Count = 0;

//Include External Interfaces


/* ---------------------------------------------------------------------- */
/* Extern Variables.                                                   */
/* -------------------------------------------------------------------- */
extern PUINT32 buffer_base_addr;
extern PUINT32 buffer_last_addr;

/* --------------------------------------------------------------------- */
/* Macros.                                                               */
/* --------------------------------------------------------------------- */

#define DOCS_DMA_GET_BUFF_BASE()        buffer_base_addr
#define DOCS_DMA_RESET_BITS             (0x60000000)

/* Minimum Number correponds to number of context available */
#define DESC_CHAIN_MAX_NO               (0x10)

//Type of DMA Transfer Enabled
#define DMA_REG_MODE            0x01
#define DMA_DESC_MODE           0x00

//Interrupt Related Settings
#define DMA_INT_ENABLE           0x01
#define INT_DS_DISABLE           0x00

//0x01 => Thread is Spawned
//0x00 => Tasklet is Sheduled
#define THREAD_EN               0x01


/* --------------------------------------------------------------------- */
/* Global Variables.                                                     */
/* --------------------------------------------------------------------- */

//Static Variables
static UINT32 mac_reg_base_dma;
static PDMAPrivate pdmaPrivate=NULL;
static INT32 intNo = 0x00;
typedef INT32 (*ptr_Send_Message)(Mod_Id_t, Msg_Struct_t*);
typedef INT32 (*ptr_Process_Map_Main)(Msg_Struct_t*);

ptr_Send_Message _Send_Message;
ptr_Process_Map_Main _Process_Map_Main;

UINT32 Descr_base_Addr[DESC_CHAIN_MAX_NO];
UINT32 DMA_CONTEXT_BASE_ADDR[]={ DMA_WCONTEXT00_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT01_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT02_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT03_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT04_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT05_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT06_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT07_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT08_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT09_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT10_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT11_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT12_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT13_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT14_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT15_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT16_NEXT_DESC_REGISTER,
                                DMA_RCONTEXT17_NEXT_DESC_REGISTER,
                                DMA_WCONTEXT18_NEXT_DESC_REGISTER
                                                                };






/* ----------------------------------------------------------------------- */
/* Internal prototypes.                                                    */
/* ----------------------------------------------------------------------- */
#if !THREAD_EN
static VOID HAL_DMA_Process_Rx_Data(unsigned long dmaPrivate);
static VOID HAL_Dma_Process_Rx_Manage(unsigned long dmaPrivate);
#else
static INT32 HAL_Dma_Rx_DeQueue(PVOID dmaPrivate);
#endif

static irqreturn_t HAL_Dma_Interrupt(INT32 irq,
                                     VOID *dev_id,
                                     struct pt_regs *regs);

static VOID HAL_Dma_Process_Rx(INT32 context);
static VOID HAL_Dma_Process_Tx(INT32 context);
static VOID HAL_Dma_Int_Write(INT32 RegAddr, INT32 Value);
static VOID HAL_Dma_Int_Read(INT32 RegAddr, PINT32 Value);


/* ----------------------------------------------------------------------- */
/* Functions Definations.                                                  */
/* ----------------------------------------------------------------------- */



//Read interrupt register using spinlock
static VOID HAL_Dma_Int_Read(INT32 RegAddr, PINT32 Value)
{
    ULONG flags;
    INT32 Addr;
    spin_lock_irqsave(&pdmaPrivate->rx_lock, flags);
    Addr = pdmaPrivate->IntRegBase + (RegAddr - DOC_INT_REGISTER_BASE_ADDRESS);
    *Value = readl(Addr);
    spin_unlock_irqrestore(&pdmaPrivate->rx_lock, flags);
}//HAL_Dma_Int_Read()

//Write interrupt register using spinlock
static VOID HAL_Dma_Int_Write(INT32 RegAddr, INT32 Value)
{
    ULONG flags;
    INT32 Addr;
    spin_lock_irqsave(&pdmaPrivate->rx_lock, flags);
    Addr = pdmaPrivate->IntRegBase + (RegAddr - DOC_INT_REGISTER_BASE_ADDRESS);
    writel(Value,Addr);
    spin_unlock_irqrestore(&pdmaPrivate->rx_lock, flags);
}//HAL_Dma_Int_Write()

volatile UINT32 ISR_Time1=0x00, ISR_Time2=0x00;
volatile UINT32 Map_Time1=0x00, Map_Time2=0x00, IT_Time=0x00;
volatile UINT32 deq_Time1=0x00, deq_Time2=0x00, deq_Time3=0x00;
volatile INT32 Map_Update=0x00;

volatile UINT32 Start_Time = 0;
volatile UINT32 MAP_T1A = 0;
UINT8 Time_Mes = 0x01;

volatile UINT32 MAP_Start = 0;
volatile UINT32 MAP_Start1 = 0;
volatile UINT32 MAP_Start2 = 0;
//Interrupt functions called when interrupt occurs
static irqreturn_t HAL_Dma_Interrupt(INT32 irq,
                                     VOID *dev_id,
                                     struct pt_regs *regs)
{
    //INT32 retval = HAL_SUCCESS;
    INT32 intGblStat;
    //INT32 intGblEn;
    INT32 serviced = IRQ_NONE;
    //ULONG flags;
    INT32 ContextNo;
    INT32 BitMask;
#ifdef _DEBUG_REG_BASED_
    UINT32 Value;
#endif
    //UINT32 Time1, Time2;

#if 0
    if(intNo == 0x00)
    {
      startClk();
      intNo++;
    }
#endif

    MAP_Start2 = getCCNT();
    //Do Validation
    if(pdmaPrivate == NULL)
    {
        DPRINTE("Invalid pdmaPrivate");
        return serviced;
    }
    DPRINTK("Interrupt %d.....",intNo++);

    ISR_Time1 = getCCNT();

#if 0
    //Disable all interrupt
    spin_lock_irqsave(&pdmaPrivate->rx_lock, flags);
    HAL_Reg_Read(REG_GLBL_INT_ENABLE, &intGblEn, HAL_REG_INT);
    HAL_Reg_Write(REG_GLBL_INT_ENABLE, HAL_DISABLE, HAL_REG_INT);
    spin_unlock_irqrestore(&pdmaPrivate->rx_lock, flags);
#endif
    //Check the interrupt- status
    HAL_Dma_Int_Read(REG_GLBL_INT_STATUS, &intGblStat);

    //Consider only DMA Causes in this ISR
    intGblStat = intGblStat & MASK_INT_DMA;

#ifdef _DEBUG_REG_BASED_    
    HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);
    //Value = Value | 0x02;
    Value = 0x01;
    HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
#endif
    for(ContextNo = 0; ContextNo < MAX_DMA_CONTEXT; ContextNo++)
    {
        //Get Bit Mask
        BitMask = 0x01<<ContextNo;

        //is current context generated the interrupt
        if(!(intGblStat & BitMask))
            continue; //Not then continue to check for next context

        //If true then check which Path DS or US?
        DPRINTK("Conetxt %d Interrupt %d Enable %08x Status %08x",ContextNo, \
                    intNo++, intGblEn, intGblStat);

        //Is it DS DMA
        if(BitMask & MASK_INT_DS_DMA)
        {
            HAL_Dma_Process_Rx(ContextNo);
            serviced = IRQ_HANDLED;
        }
        else
        {
          HAL_Dma_Process_Tx(ContextNo);
          serviced = IRQ_HANDLED;
        }

    }

    //Clear the particular interrupt which occured recently
    HAL_Dma_Int_Write(REG_GLBL_INT_STATUS, intGblStat);


#if 0
    //Enable all interrupt
    HAL_Dma_Int_Write(REG_GLBL_INT_ENABLE, intGblEn);
#endif

    DPRINTK("Leaving .....");

    ISR_Time2 = getCCNT();

    DPRINTK("ISR Time .....%dus",(Time2 - Time1)/210);
#ifdef _DEBUG_REG_BASED_
    HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);
    Value = Value & 0x00;

    HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
#endif
    return IRQ_HANDLED;

}//HAL_Dma_Interrupt()

/*---------------------------
*
* 1. Get DMA Memory Base
* 2. Reset the DMA Context
* 3. Init the DMA Descriptor Chain
* 4. Allocate DMA Private Memory
* 5. Init DMA Private Memory
* 6. Setup the Interrupt
*--------------------------------*/

INT32 HAL_Dma_Setup()
{
    INT32  context;

    //1. Get DMA Memory Base
    mac_reg_base_dma = HAL_Reg_Get_Base(HAL_REG_MAC_DMA);

    /* 2. Reset the DMA Context */
    /* 3. Init the DMA Descriptor Chain */
    for(context=8; context < MAX_DMA_CONTEXT; context++)
    {
        HAL_Dma_Reset(context);
        HAL_Dma_Desc_Init(context);
    }

    //4. Allocate DMA Private Memory
    pdmaPrivate = (PDMAPrivate)kmalloc(sizeof(DMAPrivate),GFP_KERNEL);

    if ( pdmaPrivate == NULL)
    {
        DPRINTE("Not Enough Memory");
        return HAL_FAILED;
    }

    //5. Init DMA Private Stucture
    HAL_Dma_Init_Private();

#if DMA_INT_ENABLE
    //6.Request IRQ for interrupt (INT7)
    context = request_irq(103, (PVOID)HAL_Dma_Interrupt, IRQF_DISABLED, \
                DRIVER_NAME, (PVOID)DRIVER_MAJOR); //IRQF_DISABLED,IRQF_SHARED
#endif

    HAL_Dma_Kick_Start(12);
    HAL_Dma_Kick_Start(13);
/*
    HAL_Reg_Write(REG_GLBL_INT_ENABLE,                \
                  MASK_INT_DS_DMA | MASK_INT_US_DMA,  \
                  HAL_REG_INT);
*/
    return HAL_SUCCESS;
}//HAL_Dma_Setup()

//==========================================================================

/*--------------------------------------
 * 1. Initializae all DMA Private strucuture
 *-------------------------------------*/

VOID HAL_Dma_Init_Private()
{
  INT32 ctxt;

  //Private Initialization

  //1. Initialize Data Packet RX Tasklet
#if !THREAD_EN
  tasklet_init(&pdmaPrivate->rx_tasklet_data, \
               HAL_DMA_Process_Rx_Data,      \
               (unsigned long)pdmaPrivate);

  tasklet_init(&pdmaPrivate->rx_tasklet_manage, \
               HAL_Dma_Process_Rx_Manage,        \
               (unsigned long)pdmaPrivate);
#else
  pdmaPrivate->Thread_Info[0]= kthread_create(HAL_Dma_Rx_DeQueue,
                                              pdmaPrivate,
                                              "Dma Rx Irq CPU/%d",
                                              0x00);
  kthread_bind(pdmaPrivate->Thread_Info[0], 0x00);
  wake_up_process(pdmaPrivate->Thread_Info[0]);

  //  pdmaPrivate->Thread_Info[1]=
#endif

  //2.  Global Initialization
  intNo = 0x00;
  pdmaPrivate->DataCount = 0x00;
  pdmaPrivate->ManageCount = 0x00;
  pdmaPrivate->Map_Count = 0x00;

  pdmaPrivate->DmaRegBase = HAL_Reg_Get_Base(HAL_REG_MAC_DMA);
  pdmaPrivate->IntRegBase = HAL_Reg_Get_Base(HAL_REG_INT);

  pdmaPrivate->rx_count = 0x00;

  //3. Init RX Spinlock
  spin_lock_init(&pdmaPrivate->rx_lock);
  spin_lock_init(&pdmaPrivate->int_lock);




  //4. Setup the US Fifo
  //5. Setup the DS Fifo
  for(ctxt = US_DMA_CONTEXT_MIN; ctxt <= DS_DMA_CONTEXT_MAX; ctxt++){

    //a. Init DMA FIFO SpinLock
    spin_lock_init(&pdmaPrivate->Dma_Fifo_Lock[ctxt]);

    //b. Allocate DMA Fifo's
    pdmaPrivate->Dma_Fifo[ctxt] = kfifo_alloc(DMA_FIFO_SIZE, \
                              GFP_KERNEL, &pdmaPrivate->Dma_Fifo_Lock[ctxt]);
    //c. Internal Fifo Is Free
    pdmaPrivate->Fifo_Full[ctxt] = HAL_FALSE;

  }//for

  _Send_Message = NULL;
  _Process_Map_Main = NULL;




}//HAL_Dma_Init_Private()

#if !THREAD_EN

static VOID HAL_DMA_Process_Rx_Data(unsigned long dmaPrivate)
{
    PDMAPrivate ptdmaPrivate = (PDMAPrivate)dmaPrivate;
    PUINT32 pBufferAddr;
    INT32  buffLen;
    DescrValue_t QueueValue = {0x00};

    if(ptdmaPrivate == NULL) return;

    ptdmaPrivate->DataCount++;

    kfifo_get(pdmaPrivate->Dma_Fifo[12], (UINT8*)&QueueValue,\
              sizeof(QueueValue));

    pBufferAddr = (PUINT32)QueueValue.addr;
    buffLen = QueueValue.len;

#if 0
    HAL_Msg_Send(NULL, (PINT8)pBufferAddr, buffLen);
#endif

    //Free Transferred packet
    DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);

    DPRINTE("Data Packet %d, Pkt Len %d",ptdmaPrivate->DataCount, buffLen);

#if 0
    //DPRINTE("BuffAddr 0x%08x", QueueValue.addr);
    for(idx = 0x00; idx< buffLen; idx++)
        DPRINTK("DMA Data 0x%08x", *(pBufferAddr + idx));

#endif
}//HAL_DMA_Process_Rx_Data()


static VOID HAL_Dma_Process_Rx_Manage(unsigned long dmaPrivate)
{
    PDMAPrivate ptdmaPrivate = (PDMAPrivate)dmaPrivate;
    PUINT8 pBufferAddr;
    INT32  buffLen;
    DescrValue_t QueueValue = {0x00};
    UINT32 temp_Element;

    if(ptdmaPrivate == NULL) return;

    kfifo_get(pdmaPrivate->Dma_Fifo[13], (UINT8*)&QueueValue, \
              sizeof(QueueValue));

    pBufferAddr = (PUINT8)QueueValue.addr;
    buffLen = QueueValue.len;

#if 1
    if(pBufferAddr[36]==0x03){

      deq_Time1 = getCCNT();
      HAL_EnQueue_Buffer((INT32)pBufferAddr, buffLen);
      deq_Time2 = getCCNT();
      printk("MAP SENT M-T %uus ",(Map_Time1-deq_Time1)/210);
      printk(":: P-T %uus ",(deq_Time2 - deq_Time1)/210);
      printk(":: I-T %uus\n",(ISR_Time2-ISR_Time1)/210);
    }
    else{
      HAL_Msg_Send((INT32)NULL, (PINT8)pBufferAddr, buffLen);

      if(buffLen >= 100)
      {
        DPRINTK("Manage Packet %d, Len %d",ptdmaPrivate->ManageCount,buffLen);
        ptdmaPrivate->ManageCount = ptdmaPrivate->ManageCount;
      }
      ptdmaPrivate->ManageCount++;
    }

    //Free Transferred packet
    DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);
#endif



#if 0
    //DPRINTE("BuffAddr 0x%08x", QueueValue.addr);
    for(idx = 0x00; idx< buffLen; idx++)
        DPRINTK("DMA Data 0x%08x", *(pBufferAddr + idx));

#endif
}

#else


static INT32 HAL_Dma_Rx_DeQueue(PVOID dmaPrivate)
{
    PDMAPrivate ptdmaPrivate = (PDMAPrivate)dmaPrivate;
    PUINT8 pBufferAddr;
    INT32  buffLen;
    DescrValue_t QueueValue = {0x00};
    INT32 fifo_status;
    INT32 value;
#ifdef _DEBUG_REG_BASED_
    UINT32 Register_Value;
  
    //HAL_Reg_Write_Debug(0xf7fc0000, 0x0020 ,HAL_REG_MAC_DS);

#endif
    if(ptdmaPrivate == NULL) return HAL_FAILED;

    //<RBU Fix, Date 17-APR-2009, AShah, DMA Internal FIFO Overflow Handle
    set_user_nice(current, -12);
    set_current_state(TASK_INTERRUPTIBLE);
    HAL_Reg_Read(REG_TS_MAP, &pdmaPrivate->Ds_Path_Mask, HAL_REG_MAC_DS);
    //startClk();
    //stopClk();
    //HAL_Pmu_Start_Clk();
    schedule();

  while(!kthread_should_stop()){

    deq_Time3 = getCCNT();

    fifo_status = kfifo_get(pdmaPrivate->Dma_Fifo[13],
                            (UINT8*)&QueueValue,
                            sizeof(QueueValue));

    if(!fifo_status){
      //<RBU Fix, Date 17-APR-2009, AShah, DMA Internal FIFO Overflow Handle
      //If Fifo Was Full Previously, then enable the DS Path
      if(pdmaPrivate->Fifo_Full[13] == HAL_TRUE){
        HAL_Reg_Write(REG_TS_MAP, pdmaPrivate->Ds_Path_Mask, HAL_REG_MAC_DS);
        DPRINTE("Internal Fifo is Now Empty %08x",pdmaPrivate->Ds_Path_Mask);
        pdmaPrivate->Fifo_Full[13] = HAL_FALSE;
        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
      }else{
        //msleep(1);
        set_current_state(TASK_INTERRUPTIBLE);
        //stopClk();
        schedule();
        //startClk();
      }
      continue;
    }
    __set_current_state(TASK_RUNNING);

    pBufferAddr = (PUINT8)QueueValue.addr;
    buffLen = QueueValue.len;
#if 1
    if(pBufferAddr[36]==0x03){
      //startClk();
#ifdef _DEBUG_REG_BASED_
      HAL_Reg_Read_Debug1(0xf7fc0004,&Register_Value, HAL_REG_MAC_DS);
      //Register_Value = Register_Value | 0x08;
      Register_Value = 0x01;

      HAL_Reg_Write_Debug1(0xf7fc0004, Register_Value, HAL_REG_MAC_DS);
#endif

      deq_Time1 = getCCNT();
      Map_Time2 = (deq_Time1 - Map_Time1)/210;
      //Now ISR Can Update Map_Time1
      Map_Update=0x00;
      IT_Time = (deq_Time1-ISR_Time2)/210;
      MAP_T1A = getCCNT();
      DPRINTK("MAP T1 A %u\n",MAP_T1A);
      HAL_EnQueue_Buffer((INT32)pBufferAddr, buffLen);

      preempt_disable();
      deq_Time2 = getCCNT();

      DPRINTK("T %u - %u - %u - %u ",deq_Time1, deq_Time2,ISR_Time2,deq_Time3);
      DPRINTK("T1: DMA Isr %u us\n",(ISR_Time2-ISR_Time1)/210);
      DPRINTK("T3: Dma Thread to Map Enqueue %u us ",(deq_Time2-deq_Time1)/210);
      deq_Time2 = getCCNT();
      value = (deq_Time2-ISR_Time2)/210;
      DPRINTK("T2: Isr to Thread = max(%u us; %u us)",IT_Time, value);
      DPRINTK("T %u - %u - %u - %u ",deq_Time1, deq_Time2,ISR_Time2,deq_Time3);
      DPRINTK("Map Interrupt to Thread %u us == Near To (T1+T2)",Map_Time2);

      preempt_enable();
    }
    else{
#ifdef _DEBUG_REG_BASED_
      HAL_Reg_Read_Debug1(0xf7fc0004,&Register_Value, HAL_REG_MAC_DS);
      //Register_Value = Register_Value | 0x08;
      Register_Value = 0x02;

      HAL_Reg_Write_Debug1(0xf7fc0004, Register_Value, HAL_REG_MAC_DS);
#endif
      HAL_Msg_Send((INT32)NULL, (PINT8)pBufferAddr, buffLen);

      if(buffLen >= 100)
      {
        DPRINTK("Manage Packet %d, Len %d",ptdmaPrivate->ManageCount,buffLen);
        ptdmaPrivate->ManageCount = ptdmaPrivate->ManageCount;
      }
      ptdmaPrivate->ManageCount++;
    }

    //Free Transferred packet
    DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);
#endif
    //printk("%d :: %d\n",Time2, ISR_Time2);

#ifdef _DEBUG_REG_BASED_
    HAL_Reg_Read_Debug1(0xf7fc0004,&Register_Value, HAL_REG_MAC_DS);
    //Register_Value = Register_Value & 0xf7;
    Register_Value = 0x00;

    HAL_Reg_Write_Debug1(0xf7fc0004, Register_Value, HAL_REG_MAC_DS);
#endif
    set_current_state(TASK_INTERRUPTIBLE);
  }

#ifdef _DEBUG_REG_BASED_

  //HAL_Reg_Write_Debug(0xf7fc0000, 0x002f ,HAL_REG_MAC_DS);
#endif

  return HAL_SUCCESS;
}

#endif

VOID HAL_Dma_Get_Buffer(INT32 context, PUINT32 BufferAddr, PINT32 BuffLen)
{
    UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr, Start_Addr;
    INT32 Length;
    UINT32 offset;

    Next_Descr_Addr= DMA_CONTEXT_BASE_ADDR[context] + \
                              DMA_NEXT_DESC_REGISTER_OFFSET;
    Start_Addr = DMA_CONTEXT_BASE_ADDR[context] +    \
                              DMA_START_ADDR_REGISTER_OFFSET;
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] +\
                              DMA_MODE_CTRL_REGISTER_OFFSET;
    Parameter_Addr = DMA_CONTEXT_BASE_ADDR[context] +\
                              DMA_PARAMETERS_REGISTER_OFFSET;


    HAL_Reg_Read(Start_Addr,&offset ,HAL_REG_MAC_DMA);
    *BufferAddr =  offset + (UINT32) DOCS_DMA_GET_BUFF_BASE();


    HAL_Reg_Read(Control_Addr,&Length ,HAL_REG_MAC_DMA);
    *BuffLen = (Length&0xFFFF)*8;

    return;
}

static VOID HAL_Dma_Process_Tx(INT32 context)
{
  UINT32 addr;
  INT32 len;
  INT32 fifo_len;
  Dma_Buff_t Dma_Buffer_Pkt = {0x00};
  struct kfifo *Fifo;

  //Get Transferred Buffer Address
  HAL_Dma_Get_Buffer(context, &addr, &len);

  DPRINTK("Us Interrupt Buffer Addr 0x%08x", addr);

  //printk("us %u", jiffies_to_usecs(jiffies-Map_Jiff));
  //Free Transferred packet
  DOCS_POOL_OSM_FreePkt((INT32)addr);




  //Clear the DMA (Context Done Bit)
  HAL_Dma_Clear(context);

  //Clear Device Interrupt
  HAL_Dma_Int_Clear(context);

#if 1
  //Get Fifo
  Fifo = pdmaPrivate->Dma_Fifo[context];

  //De Queue Packet
  fifo_len = sizeof(Dma_Buff_t);
  len = kfifo_get(Fifo, (UINT8*)&Dma_Buffer_Pkt, fifo_len);

  if(len >= fifo_len){ //Interrupt is not tx this queued buffer, tx here
    // Start the DMA
    HAL_Dma_Reg_Start(context, Dma_Buffer_Pkt.Addr, Dma_Buffer_Pkt.Len);
  }
#endif
}

static INT32 Sync_Count = 0;
static VOID HAL_Dma_Process_Rx(INT32 context)
{
    DescrValue_t QueueValue = {0x00};
    INT32 fifo_status=0x0;
#if RBU_FIX_002
    PUINT8 pBufferAddr;
    //INT32 Map_Count;
#endif
    UINT32 CM_Time_Value;
    UINT32 Temp_Val;
#ifdef _DEBUG_REG_BASED_
    UINT32 Value, Register_Value;
#endif
    HAL_Dma_Get_Buffer(context, &QueueValue.addr, &QueueValue.len);

#if RBU_FIX_002
    pBufferAddr = (PINT8)QueueValue.addr;

     if(MAP_Data_Count == 0)
      {

        HAL_Reg_Write(0x0001e000, 0x00000041 ,HAL_REG_PHY);
        HAL_Reg_Read(0x0001e118, &CM_Time_Value, HAL_REG_PHY);

        HAL_Reg_Write(0xf0020400, CM_Time_Value ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[4];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020414, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[5];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020418, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[6];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002041c, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[7];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020420, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[8];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020424, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[9];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020428, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[10];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002042c, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[11];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020430, Temp_Val ,HAL_REG_MAC_DS);

        MAP_Data_Count++;
      }

//Included for testing in Bangalore
#if 1
    // Sync filter for testing
    if(pBufferAddr[36] == 0x01) {
      //DPRINTR("SP RCVD");
      Sync_Count++;
      if (Sync_Count>100) {
        // drop sync
        DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);
        HAL_Dma_Int_Clear(context);
        HAL_Dma_Kick_Start(context);
        //DPRINTR("SD");
        return;
      }
      //Sync_Count = 0;
      //DPRINTR("SP");
    }
#endif
    if (pBufferAddr[36]==0x05)
    {
      HAL_Reg_Write(0xf0020450, 0x01, HAL_REG_MAC_DS);
    
      HAL_Reg_Read(0xf0020454, &CM_Time_Value, HAL_REG_MAC_DS);
      
      if (CM_Time_Value == 1)
      {
        Temp_Val = pBufferAddr[37];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020458, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[38];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002045c, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[39];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020460, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[40];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020464, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[41];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020468, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[42];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002046c, Temp_Val ,HAL_REG_MAC_DS);
      }
    }

    if(pBufferAddr[36]==0x03){
        pdmaPrivate->Map_Count++;

#if 1
        if(pdmaPrivate->Map_Count < 2000)
#else
        if(!((pBufferAddr[15] == 0x54) && (Time_Mes == 0x00)))
#endif
        {
          //Free Transferred packet
          DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);

          //Clear Device Interrupt
          HAL_Dma_Int_Clear(context);

          //Kick Start the DMA
          HAL_Dma_Kick_Start(context);
#ifdef _DEBUG_REG_BASED_
          HAL_Reg_Read_Debug(0xf7fc0000,&Register_Value, HAL_REG_MAC_DS);
          //Register_Value = Register_Value & 0xF9;          
          Register_Value = 0x00;

          HAL_Reg_Write_Debug(0xf7fc0000, Register_Value, HAL_REG_MAC_DS);
#endif

          return;
        }

        //Map_Count = (pdmaPrivate->Map_Count) % MAP_DROP_CNT;
        //if(Map_Count!=0)
        //{
        Time_Mes = 0x01;
        DPRINTK("Flag set count %d\n",pdmaPrivate->Map_Count);

        //MAP_Start = getCCNT();
        MAP_Start = MAP_Start2;
        //DPRINTK("\nMAP T1 = %u\n",MAP_Start);
        DPRINTK("*\n");
#if 0
        if(pBufferAddr[15] != 0x54)
        {
          //Free Transferred packet
          DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);

          //Clear Device Interrupt
          HAL_Dma_Int_Clear(context);

          //Kick Start the DMA
          HAL_Dma_Kick_Start(context);

          return;
        }
#endif

#if 1
         //printk("%x %x",pBufferAddr[58], pBufferAddr[59]);
         //if( !(pBufferAddr[58]==0xff && pBufferAddr[59]==0xfc))
         if( pBufferAddr[56] != 0xC0)
         {
          //Free Transferred packet
          DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);

          //Clear Device Interrupt
          HAL_Dma_Int_Clear(context);

          //Kick Start the DMA
          HAL_Dma_Kick_Start(context);
#ifdef _DEBUG_REG_BASED_
           HAL_Reg_Read_Debug(0xf7fc0000,&Register_Value, HAL_REG_MAC_DS);
           //Register_Value = Register_Value & 0xF9;
           Register_Value = 0x00;

           HAL_Reg_Write_Debug(0xf7fc0000, Register_Value, HAL_REG_MAC_DS);
#endif
          return;
        }
#endif

#if 0
      Map_Count++;

      if((Map_Count % 3) !=0)
      {
          //Free Transferred packet
          DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);

          //Clear Device Interrupt
          HAL_Dma_Int_Clear(context);

          //Kick Start the DMA
          HAL_Dma_Kick_Start(context);

          return;
      }
#endif
      if(MAP_Data_Count == 0)
      {

        HAL_Reg_Write(0x0000e000, 0x00000041 ,HAL_REG_PHY);
        HAL_Reg_Read(0x0000e118, &CM_Time_Value, HAL_REG_PHY);

        HAL_Reg_Write(0xf0020400, CM_Time_Value ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[4];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020414, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[5];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020418, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[6];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002041c, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[7];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020420, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[8];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020424, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[9];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020428, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[10];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf002042c, Temp_Val ,HAL_REG_MAC_DS);

        Temp_Val = pBufferAddr[11];
        Temp_Val = Temp_Val & 0x000000FF;
        HAL_Reg_Write(0xf0020430, Temp_Val ,HAL_REG_MAC_DS);

#ifdef _DEBUG_REG_BASED_
//      HAL_Reg_Write_Debug(0xf7fc0000, 0xabcd ,HAL_REG_MAC_DS);
/*      Value = 0x01;
        HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);
        
        printk("\nRG = %x\n",Value);


        Value = 0x02;
        HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);

        printk("\nRG = %x\n",Value);

        Value = 0x04;
        HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);

        printk("\nRG = %x\n",Value);

        Value = 0x08;
        HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);

        printk("\nRG = %x\n",Value);
*/
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);
        Value = Value | 0x04;
       
        //printk("\nRG = %x\n",Value);
 
        //HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);

#endif
        MAP_Data_Count++;
      }
#ifdef _DEBUG_REG_BASED_
        HAL_Reg_Read_Debug(0xf7fc0000,&Value, HAL_REG_MAC_DS);
        //Value = Value | 0x04;
        Value = pBufferAddr[45];
        //printk("\nRG = %x\n",Value);

        HAL_Reg_Write_Debug(0xf7fc0000, Value, HAL_REG_MAC_DS);
#endif


      if(!Map_Update)
        Map_Time1 = getCCNT();

      Map_Update = 0x01;
    }
    else
    {
       // HAL_Reg_Write_Debug(0xf7fc0000, 0x0019 ,HAL_REG_MAC_DS);
    }
#endif

  fifo_status = kfifo_put(pdmaPrivate->Dma_Fifo[context],(UINT8*)&QueueValue,\
                                    sizeof(QueueValue));
#if !THREAD_EN

    switch(context)
    {
        case 12:
            tasklet_schedule(&pdmaPrivate->rx_tasklet_data);
            break;

        case 13:
            tasklet_schedule(&pdmaPrivate->rx_tasklet_manage);
            break;
    }
#endif


    if(!fifo_status)
    {
      //Free Transferred packet
      DOCS_POOL_OSM_FreePkt((INT32)QueueValue.addr);
      //<RBU Fix, Date 17-APR-2009, AShah, DMA Internal FIFO Overflow Handle
      //Stop DS Path
      if(pdmaPrivate->Fifo_Full[context] == HAL_FALSE)
      {
        HAL_Reg_Read(REG_TS_MAP, &pdmaPrivate->Ds_Path_Mask, HAL_REG_MAC_DS);
        HAL_Reg_Write(REG_TS_MAP, 0x00, HAL_REG_MAC_DS);
        pdmaPrivate->Fifo_Full[context] = HAL_TRUE;
        DPRINTE("DS Internal Fifo Full");
      }
    }

    if (pdmaPrivate->Thread_Info[0] &&
        pdmaPrivate->Thread_Info[0]->state != TASK_RUNNING)
    {
      wake_up_process(pdmaPrivate->Thread_Info[0]);
    }

#ifdef _DEBUG_REG_BASED_
    HAL_Reg_Read_Debug(0xf7fc0000,&Register_Value, HAL_REG_MAC_DS);
    //Register_Value = Register_Value & 0xF9;
    //Register_Value = 0x00;

    HAL_Reg_Write_Debug(0xf7fc0000, Register_Value, HAL_REG_MAC_DS);
#endif

    //Clear Device Interrupt
    HAL_Dma_Int_Clear(context);

    //Kick Start the DMA
    HAL_Dma_Kick_Start(context);
}
//==========================================================================

/*---- HAL_Dma_Release() ---
 * 1. Free DMA Common ISR
 * 2. Free DMA Private Memory
 *--------------------------------*/
INT32 HAL_Dma_Release()
{
  INT32 ctxt;
    //TST
    HAL_Reg_Write(DOCSIS_US_CFG_REGISTER,0x00 ,HAL_REG_MAC_US);

#if DMA_INT_ENABLE
    //Free Interrupt ISR
    //disable_irq(103);
    free_irq(103, (PVOID)DRIVER_MAJOR);
#endif

  //Stop All Threads
  if(pdmaPrivate->Thread_Info[0])
    kthread_stop(pdmaPrivate->Thread_Info[0]);

  //Free All DMA Fifo's
  for(ctxt = US_DMA_CONTEXT_MIN; ctxt <= DS_DMA_CONTEXT_MAX; ctxt++){
    kfifo_free(pdmaPrivate->Dma_Fifo[ctxt]);
  }//for


  if ( pdmaPrivate != NULL)
  {
      kfree(pdmaPrivate);
  }


    return HAL_SUCCESS;
}

//=========================================================================

  /*----    HAL_Dma_Desc_Init    -------------------------
     * 1. Allocate Descriptors
     * 2. Create a Descriptor Chain
     *----------------------------------------------------*/

VOID HAL_Dma_Desc_Init(INT32 context)
{
    /*
     * 1. Allocate Descriptors
     * 2. Create a Descriptor Chain
     */

    //1. Allocate Descriptors
    DOCS_POOL_OSM_GetPkt(&Descr_base_Addr[context]);

    //2. Create Descriptor Chain
    HAL_Dma_Desc_Create_Chain(Descr_base_Addr[context]);

    return ;
}

//==========================================================================

    /*----    HAL_Dma_Desc_Create_Chain    -------------------------
     * 1. Create a Descriptor Chain
     * 2. Initialzie the Descriptor with default value
     *         a. Write Back Mode (if supported)
     *         b. last descriptor
     *    c. Enable Interrupt For every transfer
     *----------------------------------------------------*/

VOID HAL_Dma_Desc_Create_Chain(UINT32  Descr_Addr)
{
    PDMADesc_t pDMADesc;
    UINT32 DescAddrOffset;

    DPRINTK("Descr_Addr : %8x",(UINT32)Descr_Addr);
    DescAddrOffset = (UINT32)Descr_Addr - (UINT32)DOCS_DMA_GET_BUFF_BASE();

    //1. Create a Descriptor Chain
    for( pDMADesc = (PDMADesc_t)Descr_Addr; \
        (UINT32) pDMADesc < (UINT32)(Descr_Addr + PACKET_SIZE); \
        pDMADesc= (PDMADesc_t)((UINT32)pDMADesc +(sizeof ( DMADesc_t ))) )
    {
    /*
         * 2. Initialzie the Descriptor with default value
         *         a. Write Back Mode (if supported)
         *         b. last descriptor
         *    c. Enable Interrupt For every transfer
         */

      pDMADesc->NextDescr = ((UINT32)pDMADesc+(sizeof(DMADesc_t)))-\
                                                     (UINT32)Descr_Addr ;
      pDMADesc->NextDescr +=  DescAddrOffset ; //added Jan 22 2009
      pDMADesc->StartAddr = (UINT32)NULL;

      pDMADesc->ModeControl = (   DMA_DESCRIPTOR_MODE_BIT     | \
                                  DMA_IRQ_EN_BIT              | \
                                  DMA_IRQ_ON_PROT_ERR_EN_BIT  | \
                                  DMA_IRQ_ON_AXI_ERR_EN_BIT   );
                                  //DMA_LAST_DESCRIPTOR_BIT     |
                                  //DMA_DESCR_WBACK_BIT         |

      pDMADesc->ContextParam = (DMA_BURST_8BYTE);

    }//for


  //2. Create a Descriptor Ring Buffer Chain or List (Fuse last)
  //pDMADesc->NextDescr = (UINT32)NULL;
  pDMADesc->NextDescr = (UINT32)Descr_Addr - (UINT32)DOCS_DMA_GET_BUFF_BASE();

    return ;
}

//============================================================================

INT32 HAL_Dma_Reset(INT32 context)
{
    UINT32 Control_Addr;
    INT32 value, ret;

    DPRINTK("Resetting DMA Context %02d",context);

    if( context<0 || context> 18 ){
         DPRINTE("context out of range %d",context);
         return HAL_FAILED;
    }

    // Get Context Control Register
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] +\
                   DMA_MODE_CTRL_REGISTER_OFFSET;

    //Reset DMA Context
    HAL_Reg_Read( Control_Addr, &value, HAL_REG_MAC_DMA);
    value |= DOCS_DMA_RESET_BITS;
    HAL_Reg_Write(Control_Addr,value,HAL_REG_MAC_DMA);

    //Wait till DMA get Reseted
    value = 0x02000;
    while (value--)
    {
        HAL_Reg_Read( Control_Addr, &ret, HAL_REG_MAC_DMA);
        if ((ret & DOCS_DMA_RESET_BITS) == 0x00)
        {
            return HAL_SUCCESS;
        }
    }

    DPRINTE("DMA Context %d Reset Failed !!!!!!!!!!!! 0x%08x",context, ret);
    return HAL_FAILED;
}

//===========================================================================


//HAL_SUCCESS = Not Busy, DMA is Done
//HAL_FAILED = its Busy
INT32 HAL_Dma_Is_Busy(INT32 context)
{
    UINT32 Control_Addr;
    UINT32 value;

    // Get Context Control Register
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] + \
                   DMA_MODE_CTRL_REGISTER_OFFSET;

    // Get Context Done Bit
    HAL_Reg_Read(Control_Addr,&value,HAL_REG_MAC_DMA);

    // Check for Context Done or Busy
    if( ((value & DMA_CONTEXT_DONE_BIT) == DMA_CONTEXT_DONE_BIT) || \
            ((value & (DOCS_DMA_RESET_BITS | DMA_CONTEXT_DONE_BIT)) == 0 ))
    {
        // Context Done Bit is Set
        DPRINTK("DMA is Free ********** %x", value);
        return HAL_SUCCESS;
    }

    // Context Done Bit is not Set
    DPRINTK("DMA Context %d is Busy !!!!!!!!!!!! %x", context, value);
    return HAL_FAILED;

}

//=========================================================================

//HAL_SUCCESS = DMA is Cleared
//HAL_FAILED = DMA is Not Cleared
INT32 HAL_Dma_Clear(INT32 context)
{
    UINT32 Control_Addr;
    UINT32  ret;
    INT32 value;

    DPRINTK("Clearing DMA Context %02d",context);

    // Get Context Control Register
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] + \
                      DMA_MODE_CTRL_REGISTER_OFFSET;

    // Get Context Done Bit
    HAL_Reg_Read(Control_Addr,&value,HAL_REG_MAC_DMA);

    // Clear DMA Done
    //value |= DMA_CONTEXT_DONE_BIT;
    value &= DMA_CONTEXT_DONE_BIT;
    HAL_Reg_Write(Control_Addr,value,HAL_REG_MAC_DMA);

    //Wait till DMA get Reseted
    value = 0x02000;
    while (value--)
    {
        HAL_Reg_Read( Control_Addr, &ret, HAL_REG_MAC_DMA);
        if ((ret & DMA_CONTEXT_DONE_BIT) == 0x00)
        {
            return HAL_SUCCESS;
        }
    }

    DPRINTE("DMA Clear Failed !!!!!!!!!!!! %d", ret);
    return HAL_FAILED;
}

//==========================================================================

//DMA REG START

VOID HAL_Dma_Reg_Start(INT32 context, UINT32 BuffAddr, UINT32 Len)
{
    UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr, Start_Addr;
    UINT32 CtrlRegValue;
    UINT32 offset;

    DPRINTK("Programming DMA Context %02d",context);

    //HAL_Dma_Reset(context);
    //HAL_Dma_Clear(context);
    if(HAL_Dma_Is_Busy(context)== HAL_FAILED){
      DPRINTE("DMA Context %d is Busy !!!!!!!!!!!!", context);
      return;
    }


    Next_Descr_Addr= DMA_CONTEXT_BASE_ADDR[context] + \
                              DMA_NEXT_DESC_REGISTER_OFFSET;
    Start_Addr = DMA_CONTEXT_BASE_ADDR[context] +    \
                              DMA_START_ADDR_REGISTER_OFFSET;
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] +\
                              DMA_MODE_CTRL_REGISTER_OFFSET;
    Parameter_Addr = DMA_CONTEXT_BASE_ADDR[context] +\
                              DMA_PARAMETERS_REGISTER_OFFSET;



    //always pass logical addr of descr after hold pass latest desc addr
    offset = BuffAddr - (UINT32) DOCS_DMA_GET_BUFF_BASE();

    if(BuffAddr < (UINT32)DOCS_DMA_GET_BUFF_BASE() || \
       BuffAddr > (UINT32) buffer_last_addr)
    {
      DPRINTE("Error BuffAddr  0x%08x, Pool Base 0x%08x ", \
                          BuffAddr,(UINT32)DOCS_DMA_GET_BUFF_BASE());
      return;
    }
    HAL_Reg_Write(Start_Addr,offset ,HAL_REG_MAC_DMA);

    //Update Data Transfer Length
    HAL_Reg_Write(Parameter_Addr,Len ,HAL_REG_MAC_DMA);

    //Start the DMA
    CtrlRegValue =  DMA_CONTEXT_REGISTER_MODE_BIT | \
                    DMA_IRQ_EN_BIT                | \
                    DMA_IRQ_ON_PROT_ERR_EN_BIT    | \
                    DMA_IRQ_ON_AXI_ERR_EN_BIT;//    | DMA_GENERATE_EOP_BIT;

    if((context == 13) || (context == 12)) //TBD Function Required
    {
      CtrlRegValue |= DMA_GENERATE_EOP_BIT ;
    }

    HAL_Reg_Write(Control_Addr,CtrlRegValue ,HAL_REG_MAC_DMA);
    if (context == 8)
    {
    DPRINTK("Control_Addr  %08x",CtrlRegValue);
    DPRINTK("Start_Addr  %08x",offset);
    DPRINTK("Parameter_Addr  %08x",Len);
    }
#if !DMA_INT_ENABLE
    value =0;
    while ( (value & DMA_CONTEXT_DONE_BIT )!= DMA_CONTEXT_DONE_BIT)
        HAL_Reg_Read(Control_Addr,&value ,HAL_REG_MAC_DMA);
#endif

    return;
}



//========================================================================

INT32 HAL_Dma_Late_Bind(INT32 context)
{
    UINT32 Next_Descr_Addr,Control_Addr;
    INT32 value,ret;

    Next_Descr_Addr= DMA_CONTEXT_BASE_ADDR[context] +\
                      DMA_NEXT_DESC_REGISTER_OFFSET;
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] + \
                  DMA_MODE_CTRL_REGISTER_OFFSET;

    //Hold DMA Context
    HAL_Reg_Read(Control_Addr,&value,HAL_REG_MAC_DMA);
    value |= DMA_HOLD_BIT;
    HAL_Reg_Write(Control_Addr,value,HAL_REG_MAC_DMA);


    //Wait till DMA get Holded
    value = 0xFFFF;
    while (value--)
    {
        HAL_Reg_Read(Control_Addr,&ret,HAL_REG_MAC_DMA);
        if ((ret & DMA_HOLD_BIT) == DMA_HOLD_BIT)
        {
            return HAL_SUCCESS;
        }
    }

    DPRINTE("DMA Bind Failed !!!!!!!!!!!! %d", ret);
    return HAL_FAILED;
}



//===========================================================================

/* DMA descriptor mode start routine
 *
 * Copy the address of the DMA descriptor chain to the next descriptor address
 * of the current context
 * parameters passsed - dma context
 * Return type - VOID
 */
VOID HAL_Dma_Desc_Start(INT32 context, UINT32 Descr_Addr)
{
    UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr;

    Next_Descr_Addr= DMA_CONTEXT_BASE_ADDR[context] + \
                     DMA_NEXT_DESC_REGISTER_OFFSET;
    Control_Addr = DMA_CONTEXT_BASE_ADDR[context] + \
                  DMA_MODE_CTRL_REGISTER_OFFSET;
    Parameter_Addr = DMA_CONTEXT_BASE_ADDR[context] + \
                      DMA_PARAMETERS_REGISTER_OFFSET;

    DPRINTK("Starting DMA Context %02d \t Descr_Addr %08x",\
                             context,Descr_Addr);

    //Get Logical Address of Descriptor
    //offset = Descr_Addr - (UINT32) DOCS_DMA_GET_BUFF_BASE();
    Descr_Addr &= 0x00FFFFFF;

    //Updated Next Descriptor with Logical address of descriptor chain
    //HAL_Reg_Write(Next_Descr_Addr,offset ,HAL_REG_MAC_DMA);
    HAL_Reg_Write(Next_Descr_Addr,Descr_Addr ,HAL_REG_MAC_DMA);

    //Start the DMA
    HAL_Reg_Write(Control_Addr,DMA_DESCRIPTOR_MODE_BIT ,\
                            HAL_REG_MAC_DMA);


#if !DMA_INT_ENABLE
    value =0;
    while ( (value & DMA_CONTEXT_DONE_BIT )!= DMA_CONTEXT_DONE_BIT)
        HAL_Reg_Read(Control_Addr,&value ,HAL_REG_MAC_DMA);
#endif

    return;
}


UINT32 HAL_Dma_Get_Desc_Chain(INT32 context)
{
    return Descr_base_Addr[context];
}

UINT32 HAL_Dma_Get_Desc_Node( UINT32 CurrDescAddr )
{
    PDMADesc_t pDMADesc;

    DPRINTK("currDescAddr %08x ",CurrDescAddr);

    //1. Parse the current descriptor chain to get node whose Buffer is Null.

    for( pDMADesc = (PDMADesc_t)CurrDescAddr; \
        (UINT32) pDMADesc < (UINT32)(CurrDescAddr + PACKET_SIZE); \
        pDMADesc= (PDMADesc_t)((UINT32)pDMADesc +(sizeof ( DMADesc_t ))) )
    {
        if( pDMADesc->StartAddr == (UINT32)NULL){break;}
    }//for

    //2. Check for validity of the Node.
    if( (UINT32) pDMADesc < (UINT32)(CurrDescAddr + PACKET_SIZE) )
    {
        DPRINTK("Got free Desc : %08x", (UINT32)pDMADesc);
        return ((UINT32)pDMADesc);
    }
    else
    {
        DPRINTE("NO FREE DESC @ Desc Chain: %08x", (UINT32)CurrDescAddr);
        return (HAL_FALSE);
    }

}

INT32 HAL_Dma_Fill_Desc_Node(INT32 Context,
                             UINT32 DescNodeAddr,
                             UINT32 BuffAddr,
                             UINT32 len)
{
    PDMADesc_t pDMADesc;
    UINT32 ContextParam = 0x00;
    UINT32 ModeCtrl = 0x00;
    UINT32 offset = 0x00;
    INT32 cnt = 0;
    DPRINTK("DescNodeAddr %08x \t BuffAddr %08x \t len %08x \t ",\
                             DescNodeAddr,BuffAddr,len);

    pDMADesc = (PDMADesc_t)DescNodeAddr;


    //1. Calculate offset of logical address
    offset = BuffAddr - (UINT32 )DOCS_DMA_GET_BUFF_BASE();

    //2. Attach the Buffer to the node in the Chain.
    writel(0, (UINT32)&pDMADesc->StartAddr);
    writel(offset, (UINT32)&pDMADesc->StartAddr);

  //3. update data length to transfer and Context Parameters
    ContextParam |= (len & 0xFFFF);
    ContextParam |= DMA_BURST_8BYTE;
    writel(0, (UINT32) &pDMADesc->ContextParam );
    writel(ContextParam, (UINT32) &pDMADesc->ContextParam );

    //4. Update the Dma Modes

    if( (Context >= US_DMA_CONTEXT_MIN) || (Context <= US_DMA_CONTEXT_MAX) ){
      ModeCtrl = /*DMA_LAST_DESCRIPTOR_BIT     |*/
                 DMA_IRQ_EN_BIT              |
                 DMA_IRQ_ON_PROT_ERR_EN_BIT  |
                 DMA_IRQ_ON_AXI_ERR_EN_BIT   |
                 DMA_IRQ_BIT;
    }
    else{
         ModeCtrl = DMA_GENERATE_EOP_BIT     |
               DMA_IRQ_EN_BIT              |
               DMA_IRQ_ON_PROT_ERR_EN_BIT  |
               DMA_IRQ_ON_AXI_ERR_EN_BIT   |
               DMA_IRQ_BIT;
    }//if-else


                /*DMA_DESCRIPTOR_MODE_BIT     | \ */
                /*DMA_LAST_DESCRIPTOR_BIT     | \ */
                /*DMA_DESCR_WBACK_BIT         | \ */
                /*DMA_IRQ_EN_BIT              | \ */
                /*DMA_IRQ_ON_PROT_ERR_EN_BIT  | \
                DMA_IRQ_ON_AXI_ERR_EN_BIT     | \ */
#if 1
        cnt++;
        if( cnt == 20  )
        {
           ModeCtrl |= DMA_LAST_DESCRIPTOR_BIT ;
        }
#endif



    writel(0, (UINT32)&pDMADesc->ModeControl);
    writel(ModeCtrl, (UINT32)&pDMADesc->ModeControl);



    DPRINTK("NextDes %08x \t Addr %08x", \
        pDMADesc->NextDescr,pDMADesc->StartAddr);



    return (HAL_SUCCESS);
}

INT32 HAL_Dma_Trigger_Dma(INT32 context,UINT32 DescNodeAddr)
{
    /*1. check DMA_IsBusy
     *2. If DMA not Busy
     *   a. reset the DMADescr_Addr context.
     *   b. Start the DMA context.
     *3. If DMA is Busy
     *   a. hold the DMA
     *   b. change the previous expecteddescriptor node value if that node is
     *      not running currently.
     *      Mode_cntrl last bit set to 0 and
     *      disable interrupt.
     */


    //1. check DMA_IsBusy
    if(HAL_Dma_Is_Busy(context)== HAL_SUCCESS){

        /*
         * 2. If DMA not Busy
         *   a. reset the DMADescr_Addr context.
         *   b. Start the DMA context.
         */
        HAL_Dma_Reset(context);
        HAL_Dma_Desc_Start(context,DescNodeAddr);
    }
    else{
    /*
     * 3. If DMA is Busy
     *    a. hold the DMA
     *    b. change the previous expected descriptor node value if
          *        that node is
     *      not running currently. Mode_contrl last bit set to 0
     *    c. UnHold the DMA
     */
        HAL_Dma_Hold(context);
        HAL_Dma_Update_Prev_Desc_Node(context,DescNodeAddr);
        HAL_Dma_Unhold(context);
    }
    return HAL_SUCCESS;
}

INT32 HAL_Dma_Hold(INT32 context)
{
  if(HAL_Dma_Late_Bind(context) == HAL_SUCCESS )
  {
     return HAL_SUCCESS;
  }
  return HAL_FAILED;
}

INT32 HAL_Dma_Update_Prev_Desc_Node(INT32 context,UINT32 DescNodeAddr)
{
    /*1.Read current desciptor value of DMA register
     *2.Check if previous node of curr node
     *3.If previous node is not equal to ( not running ) current
     *   desciptor value of DMA register then
     *   Make Last descriptor bit as zero of previous node
     *4.Else do nothing
     *
     */
    PDMADesc_t pDMADesc;
    UINT32 Next_Descr_Addr,CurrDesc_Addr;
    INT32 value;

    DPRINTK("");
    DPRINTK("passed values :");
    DPRINTK("DescNodeAddr %08x ",DescNodeAddr);

    Next_Descr_Addr = (UINT32)mac_reg_base_dma + \
                              DMA_REGISTER_BLOCK_SIZE*context;
    CurrDesc_Addr = Next_Descr_Addr + DMA_CURR_DESCR_REGISTER_OFFSET;

    //Read current descriptor of DMA register
    value = readl(CurrDesc_Addr);
    DPRINTK("CurrDesc_Addr %08x ",value);



    pDMADesc = (PDMADesc_t) DescNodeAddr;
    pDMADesc--;
    //2.Check if previous node of curr node
    /*3.If previous node is not equal to ( not running ) current
        desciptor value of DMA register then */
    if( ((UINT32)pDMADesc > (UINT32 )DOCS_DMA_GET_BUFF_BASE() ) && \
        ( value != (UINT32)pDMADesc) ) {

        DPRINTK("PreNodeAddr %08x ",(UINT32)pDMADesc);


        //Make Last descriptor bit as zero of previous node
        DPRINTK("Cntrl Value bfr %08x",pDMADesc->ModeControl);
        pDMADesc->ModeControl &= ~DMA_LAST_DESCRIPTOR_BIT;

      DPRINTK("Cntrl Value unset last desc bit %08x",pDMADesc->ModeControl);


      DPRINTK("nextdesc %08x \t Address %08x", pDMADesc->NextDescr,\
                  pDMADesc->StartAddr);

        DPRINTK("");
        return HAL_SUCCESS;
    }
    else
      return HAL_FAILED;

}


INT32 HAL_Dma_Unhold(INT32 context)
{
    UINT32 Next_Descr_Addr,Control_Addr;
    INT32 value,ret;

    Next_Descr_Addr = (UINT32)mac_reg_base_dma +\
                   DMA_REGISTER_BLOCK_SIZE*context;
    Control_Addr = Next_Descr_Addr + DMA_MODE_CTRL_REGISTER_OFFSET;

    //Hold DMA Context
    value = readl(Control_Addr);
    writel((value &(~ DMA_HOLD_BIT) ), Control_Addr);

    //Wait till DMA unhold
    value = 0xFFFF;
    while (value--)
    {
        ret =  readl(Control_Addr);
        if ((ret & DMA_HOLD_BIT) != DMA_HOLD_BIT)
        {
            return HAL_SUCCESS;
        }
    }

    DPRINTE("DMA Unhold Failed !!!!!!!!!!!! %d", ret);
    return HAL_FAILED;
}

//Convert to Register Transfer and Have a Queue to Q the In Comming Packet and
// then check the same Q in the Interrupt and Restart the DMA


INT32 HAL_Dma_Send(INT32 context,UINT32 Buff_Addr,UINT32 len)
{
#if 0 //Enable and Debug this Code Later
  UINT32 DescNodeAddr;
  UINT32 Descr_Chain;

  //1. Get Descriptor Chain
  Descr_Chain = HAL_Dma_Get_Desc_Chain(context);

  DPRINTK(" Descr_Chain %x ",Descr_Chain);

  //2. Get Descriptor Node
  DescNodeAddr= HAL_Dma_Get_Desc_Node(Descr_Chain);
  if(DescNodeAddr == 0 ){
      DPRINTE(" Failed Get Desc Node!!!!!!!!!!!! ");
      return HAL_FAILED;
  }

  // Fill the descriptor
  HAL_Dma_Fill_Desc_Node(context,DescNodeAddr,Buff_Addr,len);

  // Start the DMA
  HAL_Dma_Trigger_Dma(context, DescNodeAddr);
#else //Using Register Transfer Method

  struct kfifo *Fifo;
  INT32 fifo_len;
  Dma_Buff_t Dma_Buffer_Pkt = {0x00};


  //Get Fifo
  Fifo = pdmaPrivate->Dma_Fifo[context];

  //En-Queue DMA Packet, Required if Interrupt Prempts this function
  Dma_Buffer_Pkt.Addr = Buff_Addr;
  Dma_Buffer_Pkt.Len = len;
  kfifo_put(Fifo, (UINT8*)&Dma_Buffer_Pkt, sizeof(Dma_Buff_t));

  // check DMA_IsBusy
  if(HAL_Dma_Is_Busy(context)== HAL_SUCCESS){ //If DMA not Busy

    // Mask Context Interrupt
    //Int_Mask = HAL_Dma_Int_Mask(~(0x01 << context));

    //De Queue Packet
    fifo_len = sizeof(Dma_Buff_t);
    len = kfifo_get(Fifo, (UINT8*)&Dma_Buffer_Pkt, fifo_len);

    if(len >= fifo_len){ //Interrupt is not tx this queued buffer, tx here
      // Start the DMA
      HAL_Dma_Reg_Start(context, Dma_Buffer_Pkt.Addr, Dma_Buffer_Pkt.Len);
    }else{
      DPRINTE("DMA Context %d is Busy !!!!!!!!!!!!", context);
    }

    //Un-Mask the Interrupt
    //HAL_Dma_Int_Mask(Int_Mask);
  }

#endif

    return HAL_SUCCESS;

}





INT32 HAL_Dma_Fill_All_Desc_Nodes(INT32 context)
{
    UINT32 DescNodeAddr;
    UINT32 Descr_Chain, Buff_Addr;
    UINT32 Buffer;

    PDMADesc_t pDMADesc;


    /*
     *For DS 12 and 13 make ready all nodes filled in corresponding descchain
     *1.Get Descriptor Chain
     *2.Get Desc node
     *3.Get packet from buffer
     *4.Fill the address of packet and len in node
     *5.Repeat step 2 to 4 for all nodes in descriptor.
     *
     */

    if( (context != 12) && (context != 13) ){
        DPRINTE("context out of range in DS");
        return HAL_FAILED;
    }
    //1. Get Descriptor Chain
    Descr_Chain = HAL_Dma_Get_Desc_Chain(context);

    DPRINTK(" Descr_Chain %x ",Descr_Chain);

    //2. Get Descriptor Node
    DescNodeAddr= HAL_Dma_Get_Desc_Node(Descr_Chain);

    if(DescNodeAddr == 0 )
    {
        DPRINTE(" Failed Get Desc Node!!!!!!!!!!!! ");
        return HAL_FAILED;
    }

    //File Descriptor with the Buffer
    while( DescNodeAddr != 0 )
    {
        DOCS_POOL_OSM_GetPkt(&Buff_Addr);
        DPRINTK(" Buff_Addr 0x%08x", Buff_Addr);

        // Fill the descriptor
        HAL_Dma_Fill_Desc_Node(context,DescNodeAddr,Buff_Addr,0xFFF);

        // Get Descriptor Node
        DescNodeAddr= HAL_Dma_Get_Desc_Node(Descr_Chain);
    } //while( DescNodeAddr != 0 )


    // Start the DMA
    //1. Calculate offset of logical address
    //Descr_Chain = Descr_Chain - (UINT32 )DOCS_DMA_GET_BUFF_BASE();

    //2. Trigger the DMA
    //HAL_Dma_Trigger_Dma(context, Descr_Chain);

    pDMADesc = (PDMADesc_t)Descr_Chain;
    Buffer = pDMADesc->StartAddr + (UINT32 )DOCS_DMA_GET_BUFF_BASE();
    HAL_Dma_Reg_Start(context, Buffer, 0xFFFF);

    return HAL_SUCCESS;

}




INT32 HAL_Dma_Kick_Start(INT32 context)
{
    //PDMADesc_t pDMADesc, pCurrDMADesc;
    //INT32 idxDesc;
    UINT32 Buffer;
    INT32 Ret_Val=HAL_SUCCESS;

    if(HAL_Dma_Is_Busy(context)== HAL_FAILED){
      DPRINTE("DMA Context %d is Busy !!!!!!!!!!!!", context);
      return HAL_FAILED;
    }

    /*
     * Should be get called when fifo triggers interrupt
     *
     */
#if 0
    DPRINTK("Entering ..%d", pdmaPrivate->rx_count);

    pDMADesc = (PDMADesc_t)HAL_Dma_Get_Desc_Chain(context);

    pdmaPrivate->rx_count = (pdmaPrivate->rx_count + 1) % 63;

    for(idxDesc=0; idxDesc<pdmaPrivate->rx_count; pDMADesc++,idxDesc++)
     {DPRINTK("Loop ..%d",idxDesc);}

    pCurrDMADesc = pDMADesc;



    Buffer = pCurrDMADesc->StartAddr + (UINT32 )DOCS_DMA_GET_BUFF_BASE();

    DPRINTK(" Buff_Addr 0x%08x", Buffer);
#else
    Ret_Val = DOCS_POOL_OSM_GetPkt(&Buffer);
    if(Ret_Val==HAL_FAILED)
    {
      return HAL_FAILED;
    }
#endif
    //HAL_Dma_Reset(context);
    HAL_Dma_Clear(context);
    HAL_Dma_Reg_Start(context, Buffer, 0xFFFF);

#ifdef _DEBUG_REG_BASED_
    if (context == 13)
    {
      //HAL_Reg_Write_Debug(0xf7fc0000, 0x0000 ,HAL_REG_MAC_DS);
    }
#endif

    return HAL_SUCCESS;
}


INT32 HAL_Dma_Int_Clear(INT32 context)
{
    INT32 CntrlAddr,Value;
    ULONG flags;

    spin_lock_irqsave(&pdmaPrivate->rx_lock, flags);

    //Get DMA Context
    CntrlAddr = DMA_CONTEXT_BASE_ADDR[context] + DMA_MODE_CTRL_REGISTER_OFFSET;

    HAL_Reg_Read(CntrlAddr, &Value, HAL_REG_MAC_DMA);

    //clear the INT bit of DMA context
    Value = Value & (DMA_IRQ_BIT | DMA_PROT_ERR_BIT | DMA_AXI_ERR_BIT);
    HAL_Reg_Write(CntrlAddr, Value, HAL_REG_MAC_DMA);

    spin_unlock_irqrestore(&pdmaPrivate->rx_lock, flags);

    DPRINTK("INT Value 0x%08x", Value);
    return HAL_SUCCESS;
}


INT32 HAL_Dma_Int_Mask(INT32 Int_Mask)
{
  ULONG Flags;
  //INT32 Int_Mask;
  INT32 Glbl_Int_En;
  spin_lock_irqsave(&pdmaPrivate->int_lock, Flags);

  //Int_Mask = ~(0x01 << context);
  HAL_Reg_Read(REG_GLBL_INT_ENABLE, &Glbl_Int_En, HAL_REG_INT);
  HAL_Reg_Write(REG_GLBL_INT_ENABLE, Int_Mask, HAL_REG_INT);

  spin_unlock_irqrestore(&pdmaPrivate->int_lock, Flags);

  return Glbl_Int_En;
}


INT32 HAL_Dma_Send_Buffer(INT32 Context,PDma_Buff_t Buff_Det,UINT32 Len)
{
    UINT32 Desc_Node_Addr;
    UINT32 Descr_Chain;
    UINT32 Buff_Addr;
    UINT32 Buff_Len;
     if( (Context < US_DMA_CONTEXT_MIN) || (Context > US_DMA_CONTEXT_MAX) ){

         DPRINTE("Not us context");
         return HAL_FAILED;
    }

    //1. Get Descriptor Chain
    Descr_Chain = HAL_Dma_Get_Desc_Chain(Context);

    DPRINTK(" Descr_Chain %x ",Descr_Chain);
    while(Len--){
        //2. Get Descriptor Node
        Desc_Node_Addr= HAL_Dma_Get_Desc_Node(Descr_Chain);
        if(Desc_Node_Addr == 0 ){
            DPRINTE(" Failed Get Desc Node!!!!!!!!!!!! ");
            return HAL_FAILED;
        }//if

        Buff_Addr = Buff_Det->Addr;
        Buff_Len  = Buff_Det->Len;
        // Fill the descriptor
        HAL_Dma_Fill_Desc_Node(Context,Desc_Node_Addr,Buff_Addr,Buff_Len);
    }//while

    // Start the DMA
    HAL_Dma_Trigger_Dma(Context, Descr_Chain);

    return HAL_SUCCESS;

}//HAL_Dma_Send_Buffer()


INT32 HAL_EnQueue_Buffer(INT32 Buffer, INT32 Length)
{
  Msg_Struct_t* TestMsg;
  //UINT32 Msg_Buffer;
  //INT32 Data_Buffer;
  //INT32 Map_Count;
  UINT32 CM_Time_Value;

  //Drop This message if CallBack FUnction is Not Available
  if(_Send_Message==NULL){
    DPRINTK("MAP Dropped");
    return HAL_FAILED;
  }

#if (RBU_FIX_001 && (!RBU_FIX_002))
  pdmaPrivate->Map_Count++;
  Map_Count = (pdmaPrivate->Map_Count) % MAP_DROP_CNT;
  if(Map_Count!=0)
    return HAL_SUCCESS;
#endif

   //HAL_Reg_Write(0x0000e000, 0x00000041 ,HAL_REG_PHY);
   HAL_Reg_Read(0x0000e118, &CM_Time_Value, HAL_REG_PHY);

   //printk("\nStart CM Time Value = %x",CM_Time_Value);



  TestMsg = (Msg_Struct_t*) kmalloc(sizeof(Msg_Struct_t), GFP_KERNEL);


  TestMsg->Src_Mod_Id = MOD_DIST;
  TestMsg->Dest_Mod_Id = MOD_MAP;
  TestMsg->Msg_Id = MSG_ID_MAC_MAP_MGMT_MSG;

  TestMsg->Local_Data = NULL;


  TestMsg->Pkt_Struct = (Packet_Struct_t *)kmalloc(sizeof(Packet_Struct_t),\
                                                    GFP_KERNEL);



  TestMsg->Pkt_Struct->Data = kmalloc(sizeof(UINT8)*Length, GFP_KERNEL);
  memcpy(TestMsg->Pkt_Struct->Data, (PVOID)Buffer, Length);


  TestMsg->Pkt_Struct->Length = Length;

  if(_Send_Message!=NULL){

#if 0
    if(pdmaPrivate->Map_Count <= MAP_DROP_CNT*2)
    {
      DPRINTE("TestMsg 0x%08x",(INT32)TestMsg);
      DPRINTE("Pkt_Struct 0x%08x",(INT32)TestMsg->Pkt_Struct);
      DPRINTE("Data 0x%08x",(INT32)TestMsg->Pkt_Struct->Data);
    }
#endif

   //HAL_Reg_Write(0x0000e000, 0x00000041 ,HAL_REG_PHY);
   HAL_Reg_Read(0x0000e118, &CM_Time_Value, HAL_REG_PHY);

   //printk("\Before Send CM Time Value = %x\n",CM_Time_Value);
    //preempt_disable();
    _Send_Message(MOD_MAP, TestMsg);
    //_Process_Map_Main(TestMsg);
    //preempt_enable();

   //HAL_Reg_Write(0x0000e000, 0x00000041 ,HAL_REG_PHY);
   HAL_Reg_Read(0x0000e118, &CM_Time_Value, HAL_REG_PHY);

   //printk("\nAfter Send CM Time Value = %x\n",CM_Time_Value);
    //preempt_disable();
    DPRINTK("MAP Send");
  }

  return HAL_SUCCESS;
}

VOID HAL_Dma_Init_CallBack(INT32 idx, UINT32 Ptr_Function)
{
  DPRINTE("Initialized with Address 0x%08x",Ptr_Function);
  _Send_Message = (ptr_Send_Message) Ptr_Function;
}


VOID HAL_Dma_Init_CallBack_MAP(INT32 idx, UINT32 Ptr_Function)
{
  DPRINTE("Initialized with Address 0x%08x",Ptr_Function);
  _Process_Map_Main = (ptr_Process_Map_Main) Ptr_Function;
}

INT32 HAL_Dma_OverFlow(INT32 context)
{
  UINT32 addr;
  INT32 len;

  //Get Transferred Buffer Address
  HAL_Dma_Get_Buffer(context, &addr, &len);

  //Free Transferred packet
  DOCS_POOL_OSM_FreePkt((INT32)addr);

  HAL_Dma_Reset(13);
  HAL_Dma_Kick_Start(13);
  
  return HAL_SUCCESS;
}
/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */


EXPORT_SYMBOL(HAL_Dma_Reg_Start);
EXPORT_SYMBOL(HAL_Dma_Send);
EXPORT_SYMBOL(HAL_Dma_OverFlow);
EXPORT_SYMBOL(HAL_Dma_Kick_Start);
EXPORT_SYMBOL(HAL_Dma_Send_Buffer);
EXPORT_SYMBOL(HAL_Dma_Init_CallBack);
EXPORT_SYMBOL(HAL_Dma_Init_CallBack_MAP);
EXPORT_SYMBOL(MAP_Start);
EXPORT_SYMBOL(Time_Mes);

