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
#include <linux/delay.h>
#include <linux/kthread.h>

//Custom Header Files
#include "HAL_Ds.h"


/* ----------------------------------------------------------------------- */
/* Internal prototypes.                                                    */
/* ----------------------------------------------------------------------- */

static irqreturn_t HAL_Ds_Interrupt(INT32 irq,
                                     VOID *dev_id,
                                     struct pt_regs *regs);

INT32 HAL_Dma_Kick_Start(INT32 context);

/******************************************************************************
 *
 * Name: HAL_Ds_Lock_Channel
 *
 * Description:
 *    Starts the QAM Lock ReAcquisition
 *
 *
 * Arguments:
 *    DS_Channel : Enum Values from DS_Id_t (Starts from 0..3)
 *
 *
 * Return Value:
 *  HAL_SUCCESS : QAM Lock ReAcquisition is Success
 *                        It doesn't indicate QAM has been Locked
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/


INT32 HAL_Ds_Lock_Channel(INT32 DS_Channel)
{
  INT32 QAM_Start_Addr;

#if HAL_VALIDATE
  if(DS_Channel >= DS_Id_Max)
      return HAL_FAILED;
#endif

  QAM_Start_Addr = GET_CHANNEL_ADDR(REG_PHY_GRP1QAMSTVITFDBK, DS_Channel);

  HAL_Reg_Write(QAM_Start_Addr, DS_LOCK_RE_ACQUIRE, HAL_REG_PHY);

  return HAL_SUCCESS;

}


/******************************************************************************
 *
 * Name: HAL_Ds_Enable
 *
 * Description:
 *    Enables the given DS Channel Path
 *    This need to be called after Locking the Channel to Requried Ferquency
 *
 * Arguments:
 *    DS_Channel : Enum Values from DS_Id_t (Starts from 0..3)
 *
 *
 * Return Value:
 *  HAL_SUCCESS : DS Path is Enabled Successfully
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/


INT32 HAL_Ds_Enable(INT32 DS_Channel)
{
  UINT32 Value;

#if HAL_VALIDATE
  if(DS_Channel >= US_Id_Max)
      return HAL_FAILED;
#endif

  HAL_Reg_Read(REG_TS_MAP, &Value, HAL_REG_MAC_DS);
  Value |= ((0x01) << DS_Channel) ;
  HAL_Reg_Write(REG_TS_MAP, Value, HAL_REG_MAC_DS);


  return HAL_SUCCESS;

}


/******************************************************************************
 *
 * Name: HAL_Ds_Disable
 *
 * Description:
 *    Disabled the given DS Channel Path
 *    Disabling the DS Path will also clear the DS FIFO Content
 *
 * Arguments:
 *    DS_Channel : Enum Values from DS_Id_t (Starts from 0..3)
 *
 *
 * Return Value:
 *  HAL_SUCCESS : DS Path is Disabled Successfully
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/


INT32 HAL_Ds_Disable(INT32 DS_Channel)
{
  UINT32 Value;

#if HAL_VALIDATE
  if(DS_Channel >= (INT32)DS_Id_Max)
      return HAL_FAILED;
#endif

  HAL_Reg_Read(REG_TS_MAP, &Value, HAL_REG_MAC_DS);
  Value &= (~((0x01) << DS_Channel));
  HAL_Reg_Write(REG_TS_MAP, Value, HAL_REG_MAC_DS);


  return HAL_SUCCESS;

}


/******************************************************************************
 *
 * Name: HAL_Ds_Tune_Frequency
 *
 * Description:
 *    Tunes the Given DS Channel Tuner to Supplied Frequency.
 *    Frequency will be supplied as Frequency Control Word.
 *    QAM Lock ReAcquistion will be automatically started.
 *
 * Arguments:
 *    Tuner : Enum Values from DS_Id_t (Starts from 0..3)
 *    FreqCw: Frequency Control Word
 *
 * Return Value:
 *  HAL_SUCCESS : DS Path is Disabled Successfully
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/


INT32 HAL_Ds_Tune_Frequency(INT32 Tuner, UINT32 FreqCw)
{
  UINT32 Data[5], Length=5;

  DPRINTK("FreqCw %x ",FreqCw);

  // 1. Set the Tuner Device Write Address
  Data[0] = 0xC2;

  //2. Genrate the Data to be written
  Data[2] = ((FreqCw & 0x0000FF00) >> 8);
  Data[1] = ((FreqCw & 0x000000FF) >> 0);
  Data[3] = ((FreqCw & 0x00FF0000) >> 16);
  Data[4] = ((FreqCw & 0xFF000000) >> 24);

  //3. Send I2C Command to Tune to Frequency
  HAL_I2c_Write(IF_MASTER, Tuner, Data, Length);

  //4. Set the Tuner Device Read Address
  Data[0] = 0xC3;
  Data[1] = 0x00;
  Length=2;

  //5. Read the Status
  HAL_I2c_Read(IF_MASTER, Tuner, Data, Length);
  DPRINTK("Status 0x%x ",Data[1]);

  //5. Start the ReAcquistion Of the QAM Lock after Frequency Programming
  HAL_Ds_Lock_Channel(Tuner);

  return HAL_SUCCESS;
}




/******************************************************************************
 *
 * Name: HAL_Ds_Hardware_Init
 *
 * Description:
 *    Does DS One Time Initializations.
 *
 *
 * Arguments:
 *    NONE
 *
 *
 * Return Value:
 *   HAL_SUCCESS : Always Success
 *
 *
 *****************************************************************************/


INT32 HAL_Ds_Hardware_Init(VOID)
{
  INT32 Channel;
  INT32 Value;
  INT32 BitMask;

  //1. Init all DS Phy Register

  BitMask = DS_GET_ACTIVE_CHANNEL();
  for(Channel=0; Channel < DS_GET_MAX_CHANNEL(); Channel++)
  {
    //Check for Active Channel
    if((BitMask >> Channel) & 0x01)
    {
      DPRINTK("PHY Init - %d",Channel);
      HAL_Ds_Phy_Init(Channel);
    }
  }

  //2. Init all I2C Channel in Master Interface

  BitMask = DS_GET_ACTIVE_CHANNEL();
  for(Channel=0; Channel < DS_GET_MAX_CHANNEL(); Channel++)
  {
    //Check for Active Channel
    if((BitMask >> Channel) & 0x01)
    {
      DPRINTK("I2C Init - %d",Channel);
      HAL_I2c_Init(IF_MASTER ,Channel);
    }
  }

  //3.Disable All DS Channel
  HAL_Reg_Read(REG_TS_MAP, &Value, HAL_REG_MAC_DS);
  Value &= 0xF0;
  HAL_Reg_Write(REG_TS_MAP, Value, HAL_REG_MAC_DS);


  //4.Configure and do Assocoate the DS DMA
  //DATA_PDU      = DMA -DS Context 0x00,
  //AUX_TIME       = DMA -DS Context 0x00,
  //ISOLATE         = DMA -DS Context 0x01,
  //MAC_MANAGE = DMA -DS Context 0x01,
  Value = 0x1100;
  HAL_Reg_Write(REG_DS_DMA_ASSOC, Value, HAL_REG_MAC_DS);


  //5.Configure Perfect Filter Array
  Value = 0x0068;

  //Override DA
  //Override SA
  //Override DSID
  //Eanble MDF
  Value |= 0x0F0000;
  HAL_Reg_Write(REG_MAC_DSID, Value, HAL_REG_MAC_DS);



  //6.Disable Imperfect Filter
  //TBD

  //7. Clear all DS Interrupt Status
  HAL_Reg_Write(REG_DS_IRQ_STS, 0x0F, HAL_REG_MAC_DS);
  HAL_Reg_Write(REG_GLBL_INT_STATUS, 0xFFFFFFFF, HAL_REG_INT);

  //8. Select the External Clock From the CMTS
#if RBU_EXT_CLK_FIX    
  HAL_Reg_Write(REG_PHY_TB_EXTCLK_SEL, 0x03, HAL_REG_PHY);
  HAL_Reg_Write(REG_PHY_TB_FRQ_OFF_0, 0x19999999, HAL_REG_PHY);
  HAL_Reg_Write(REG_PHY_TB_FRQ_OFF_1, 0x1999999A, HAL_REG_PHY);
#endif


  //9. Request IRQ for interrupt (INT7)
#if DS_INT_ENABLE
  Value = request_irq(101, (PVOID)HAL_Ds_Interrupt, IRQF_DISABLED, \
              DRIVER_NAME, (PVOID)NULL); //IRQF_DISABLED,IRQF_SHARED
#endif

  //10. Initilalize the Debug Profile Counters
  HAL_Pmu_Start_Clk();

  //11. Enable all Interrupts

  //11.a. Enable DS Overflow Interrupt
#if RBU_DS_OVF_INT_FIX
  HAL_Reg_Write(REG_DS_IRQ_EN, 0x0F, HAL_REG_MAC_DS);
#else
  HAL_Reg_Write(REG_DS_IRQ_EN, 0x00, HAL_REG_MAC_DS);
#endif


  //11.b. Enable Global Interrupt for 
  //        DS DMA
  //        US DMA 
  //        DS Overflow Interrupt
  HAL_Reg_Write(REG_GLBL_INT_ENABLE,                \
                MASK_INT_DS_DMA | MASK_INT_US_DMA | MASK_INT_DS_FIFO,  \
                HAL_REG_INT);

  return HAL_SUCCESS;
}

/******************************************************************************
 *
 * Name: HAL_Ds_Phy_Init
 *
 * Description:
 *  Does the DS Phy One time Initialization for the givne PHY Channel
 *
 *
 * Arguments:
 *    PhyNo : Enum Values from DS_Id_t (Starts from 0..3)
 *
 *
 *
 * Return Value:
 *   NONE : Always Success
 *
 *****************************************************************************/


VOID HAL_Ds_Phy_Init(INT32 PhyNo)
{
  INT32 idx=0x00;
  INT32 PhyAddr;

  INT32 RegValue[] = {
      0x000400,0x00000003,
      0x000514,0x00000001,
      0x000518,0x00000000,
      0x00054c,0x00000008,
      0x000550,0x00000001,
      0x000554,0x00000002,
      0x000558,0x000000ff,
      0x000534,0x00000047,
      0x00055c,0x00000047,
      0x0005c0,0x00000080,
      0x0005c4,0x00000008,
      0x00052c,0x00000002,
      0x000530,0x000000ff,
      0x000598,0x00000000,
      0x0003f0,0x00000000,
      0x0003f4,0x00000001,
      0x0003f8,0x00000015,
      0x000000,0x000000ea,
      0x0003c4,0x00000000,
      0x000008,0x00000044,
      0x00000c,0x00000004,
      0x000010,0x00000005,
      0x000014,0x00000001,
      0x000018,0x00000001,
      0x00001c,0x00000001,
      0x000020,0x00000005,
      0x000024,0x00000005,
      0x000028,0x00000005,
      0x00002c,0x00000032,
      0x000030,0x00000096,
      0x000034,0x00000054,
      0x000038,0x000000ff,
      0x00003c,0x000000d2,
      0x000040,0x00000003,
      0x000044,0x000000b8,
      0x000048,0x00000028,
      0x000054,0x000000ca,
      0x000058,0x0000002b,
      0x00005c,0x00000037,
      0x000060,0x00000052,
      0x000064,0x0000000e,
      0x000068,0x0000000e,
      0x000070,0x00000014,
      0x000074,0x00000083,
      0x000078,0x000000fd,
      0x00007c,0x00000043,
      0x000080,0x00000042,
      0x000084,0x00000036,
      0x000088,0x0000001d,
      0x00008c,0x00000094,
      0x000090,0x00000045,
      0x000094,0x000000d5,
      0x000098,0x0000007f,
      0x00009c,0x00000064,
      0x0000a0,0x0000002a,
      0x0000a4,0x000000f4,
      0x0000a8,0x000000d6,
      0x0000b0,0x0000002a,
      0x0000b4,0x000000f7,
      0x0000b8,0x000000d6,
      0x0000bc,0x00000065,
      0x0000c0,0x0000002a,
      0x0000c4,0x000000f5,
      0x0000c8,0x000000d6,
      0x0000cc,0x00000012,
      0x0000d0,0x00000059,
      0x0000d4,0x00000080,
      0x0000dc,0x00000012,
      0x0000e0,0x000000ff,
      0x0000e4,0x000000ff,
      0x0000e8,0x00000088,
      0x0000ec,0x00000014,
      0x0000f0,0x00000088,
      0x0000f4,0x00000014,
      0x0000f8,0x00000088,
      0x0000fc,0x00000014,
      0x000324,0x000000e0,
      0x000328,0x00000003,
      0x000340,0x00000060,
      0x000348,0x00000080,
      0x00034c,0x00000008,
      0x0005d0,0x00000004,
      0x0003f4,0x00000040,
      0x000004,0x000000a4,
      0x0003f0,0x00000001,
      0x0005ac,0x00000001,
      0x00000000
  };

#if HAL_VALIDATE
  if(PhyNo >= (INT32)DS_Id_Max)
      return;
#endif

  for(idx=0x00; idx<(sizeof(RegValue)/sizeof(INT32))-1; idx+=2)
  {
      PhyAddr = GET_CHANNEL_ADDR(RegValue[idx+0], PhyNo);
      DPRINTK("PhyAddr 0x%08x, Value 0x%04x",PhyAddr, RegValue[idx+1]);
      HAL_Reg_Write(PhyAddr, RegValue[idx+1], HAL_REG_PHY);
  }

  return;
}



/******************************************************************************
 *
 * Name: HAL_Ds_Fifo_Clear
 *
 * Description:
 *    Whenever FIFO Overflow or Underflow occurs in the miniMAC FIFO
 *    contents need to be cleared
 *    After clearing the FIFOs by setting the suitable bits, FIFO
 *    need to be enabled by clearing the corresponding same Bits Arguments:
 *
 * Arguments:
 *    NONE
 *
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


INT32  HAL_Ds_Fifo_Clear( FIFOTYPE FIFOType, FIFONO FIFONo)
{
  UINT32 REG_DS_FIFO_CTRL_Val;

#if HAL_VALIDATE
  //read DS_FIFO_CTRL
  //1. Check input is correct
  if(FIFOType > FIFOTYPEMax)
  {
   DPRINTE("Out of range FIFOTYPE %x",FIFOType);
   return HAL_FAILED;
  }
  if(FIFONo > FIFONOMax)
  {
   DPRINTE("Out of range FIFONo %x",FIFONo);
   return HAL_FAILED;
  }
#endif

  //2. Read DS_FIFO_CTRL control register value
  HAL_Reg_Read(REG_DS_FIFO_CTRL, &REG_DS_FIFO_CTRL_Val, \
                                       HAL_REG_MAC_DS);
  //3. Set 1 on the corresponding FIFONumber's bit
  REG_DS_FIFO_CTRL_Val |= (0x1 << FIFONo);
  //4. Write the modfied value to DS_FIFO_CTRL register
  HAL_Reg_Write(REG_DS_FIFO_CTRL , REG_DS_FIFO_CTRL_Val, \
                                         HAL_REG_MAC_DS);
  //5.  Make a vlue which is  0 in the bit of corresponing FIFONumber .
  REG_DS_FIFO_CTRL_Val &= (~(0x1 << FIFONo));
  //6.  Write 0 in the bit of corresponing FIFONumber .
  HAL_Reg_Write(REG_DS_FIFO_CTRL , REG_DS_FIFO_CTRL_Val,\
                                         HAL_REG_MAC_DS);

  return HAL_SUCCESS;
}



/******************************************************************************
 *
 * Name: HAL_Ds_Fifo_Get_Status
 *
 * Description:
 *
 *
 *
 * Arguments:
 *
 *
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


INT32 HAL_Ds_Fifo_Get_Status(FIFOTYPE FIFOType, FIFONO FIFONo)
{
  UINT32 REG_DS_FIFO_STS_Val;

#if HAL_VALIDATE
  //1. Check input is correct
  if(FIFOType > FIFOTYPEMax)
  {
   DPRINTE("Out of range FIFOTYPE %x",FIFOType);
   return HAL_FAILED;
  }
  if(FIFONo > FIFONOMax)
  {
   DPRINTE("Out of range FIFONo %x",FIFONo);
   return HAL_FAILED;
  }
#endif

  //2. Read DS_FIFO_CTRL control register value
  HAL_Reg_Read(REG_DS_FIFO_CTRL, &REG_DS_FIFO_STS_Val,\
                                       HAL_REG_MAC_DS);
  REG_DS_FIFO_STS_Val &= (0x1 << FIFONo);
  return REG_DS_FIFO_STS_Val;

}


/******************************************************************************
 *
 * Name: HAL_Ds_Interrupt
 *
 * Description:
 *    Handles the DS Overflow Interrupt
 *
 *
 * Arguments:
 *    irq : Irq Number
 *    dev_id : 
 *    regs : 
 *
 * Return Value:
 *  IRQ_HANDLED : Always IRQ Needs to be Handled
 *
 *
 *****************************************************************************/

static irqreturn_t HAL_Ds_Interrupt(INT32 irq,
                                     VOID *dev_id,
                                     struct pt_regs *regs)
{
  INT32 Irq_Status;
  INT32 intGblStat;

  //1. Check the interrupt- status
  HAL_Reg_Read(REG_GLBL_INT_STATUS, &intGblStat, HAL_REG_INT);

  //2. Mask Only DS Interrupt Status
  intGblStat &= MASK_INT_DS_FIFO;

  //3. Check DS Overflow Cause
  HAL_Reg_Read(REG_DS_IRQ_STS, &Irq_Status, HAL_REG_MAC_DS);

  //4. Handle DS Overflow
  if(Irq_Status)
  {
    DPRINTK("%08x", Irq_Status);
    HAL_Dma_OverFlow(13);
  }

  //5. Clear the DS Overflow Status
  HAL_Reg_Write(REG_DS_IRQ_STS, Irq_Status, HAL_REG_MAC_DS);
  HAL_Reg_Write(REG_GLBL_INT_STATUS, intGblStat, HAL_REG_INT);

  return IRQ_HANDLED;
}

/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */
EXPORT_SYMBOL(HAL_Ds_Tune_Frequency);
EXPORT_SYMBOL(HAL_Ds_Hardware_Init);
