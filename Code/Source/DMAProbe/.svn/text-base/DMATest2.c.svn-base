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


/* --------------------------------------------------------------------------*/
/* Headers.                                                                  */
/* ------------------------------------------------------------------------- */


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

#include "DMATest2.h"

//Include External Interfaces



INT32 DOCS_DMA_Test2(INT32 ctxt, INT32 mode, INT32 times)
{
   /* 1. Start DMA ( in Desc mode)
   * 2. Fillfifo corresponding to context
   * 3. Dump buffer
   *
   */

    //ENable all interrupt
    HAL_Reg_Write(REG_GLBL_INT_ENABLE , MASK_INT_DS_DMA, HAL_REG_INT);

   // HAL_Dma_Fill_All_Desc_Nodes(ctxt);

}






#if 0

//Fill_Fifo(ds_fifo_docsis_0,0xaaaaaaaa,0x5555,0,\
//"./datfiles/fr_hcs_bad_01.txt");
static INT32 FillFifo(UINT32 FifoNum)
{
    INT32 index;
    PUINT32 pbuf;

    UINT8 databuff[]={
                                            #include <tlv1.txt>
                                            //#include <us.txt>
                                        };

    pbuf =  databuff;
    *PBuff_len = sizeof(databuff);

    DPRINTK("Buffer Transfer Length: %d", *PBuff_len);

  for(index =0;index <*PBuff_len;index+=4)
  {
      writel(*pbuf,buffaddr);
      //DPRINTK("buffaddr : %0x,value :%0x", buffaddr,*pbuf);
      pbuf++;
    }
    return HAL_SUCCESS;
}

/* 1.Fill the fifo with value read from the file.
2. In case of TS fifos, 188 bytes or one full TS packet is processed.
3.In docsis fifo, the timestamps and bytecount are prepended to the data.
4. Only one docsis fifo is currently implemented.
5. The values read from the files are converted to long and
    inserted to the fifo locations starting from the base address
6. The appropriate bits in the test registers are set to initiate
     the transfer */
INT32 Fill_Fifo(UINT32 AddrVar,UINT32 Timestamp_lo,UINT32 Timestamp_hi,
UINT32 ByteOffset,const INT8* FileName)
{
  INT8 NextByte[32];
  INT8* EndPtr;
  UINT32 next_value;
  UINT32 fifo_value;
  UINT32 InsertCount;
  INT32 i,errcode;
  INT32 mod_count;
  UINT32 ByteCount;
  UINT32 TS_BYTES[256];
  UINT32 WriteAddr;
  UINT32 reg_value;
  UINT32 Timeout;
  UINT32 status_value;
  //FILE* FillFile;

  /*FillFile = fopen(FileName,"r");
  if(FillFile==0)
  {
      printf("fopen Failed File: %s\n", FileName);
  }*/


  next_value = 0x0;
  i = 0;
  errcode = 0x0;
  printf("**Fill_Fifo Started ..... \n");
 // printf("from file %s \n",FileName);
  switch(AddrVar)
  {
    case DS_FIFO_TS_0_ENABLE:
    case DS_FIFO_TS_1_ENABLE:
    case DS_FIFO_TS_2_ENABLE:
    case DS_FIFO_TS_3_ENABLE:
      InsertCount = 0;
      while (fgets(NextByte,32,FillFile) != NULL)
      {
        next_value = strtoul(NextByte,&EndPtr,16); //get byte from file
        //printf(" nextval = %x\n",next_value);
        TS_BYTES[InsertCount] = (next_value & 0xFF); // limit size to byte
        //printf(" TS_Byte %d = %x\n",InsertCount,TS_BYTES[InsertCount]);
        InsertCount++;
        if(InsertCount == 188)
        {
          switch(AddrVar)
          {
            case DS_FIFO_TS_0_ENABLE:
              WriteAddr = DOCSIS_TS_TEST_FIFO_0_BASE_ADDRESS;
              reg_value = 0x1;
            break;
            case DS_FIFO_TS_1_ENABLE:
              WriteAddr = DOCSIS_TS_TEST_FIFO_1_BASE_ADDRESS;
              reg_value = 0x2;
            break;
            case DS_FIFO_TS_2_ENABLE:
              WriteAddr = DOCSIS_TS_TEST_FIFO_2_BASE_ADDRESS;
              reg_value = 0x4;
            break;
            case DS_FIFO_TS_3_ENABLE:
              WriteAddr = DOCSIS_TS_TEST_FIFO_3_BASE_ADDRESS;
              reg_value = 0x8;
            break;
          } //end swicth AddrVar
          fifo_value=0x0;
          for(i = 0;i < 188;i++)
          {
            mod_count = i%4;
            switch(mod_count)
            {
              case 0:
                fifo_value |= TS_BYTES[i];
              break;
              case 1:
                fifo_value |= (TS_BYTES[i] << 8);
              break;
              case 2:
                fifo_value |= (TS_BYTES[i] << 16);
              break;
              case 3:
                fifo_value |= (TS_BYTES[i] << 24);
              break;
            } //end switch
            if( (i != 0) && (mod_count == 0x3) )
            {
              if(MW(WriteAddr,fifo_value) != 0x0)
              {
                errcode = 1;
                goto errexit;
              }
              WriteAddr += 0x4;
              fifo_value = 0x0;
            }
          } //end for 188
          if(MW(WriteAddr,fifo_value) != 0x0)
          {
            errcode = 1;
            goto errexit;
          }
          if(MW(DOCSIS_DCSD_TS_TEST_REGISTER,reg_value) != 0x0)
          {
            errcode = 1;
            goto errexit;
          }
          Timeout = 0xFFF;
          if(Axi_MR(DOCSIS_DCSD_TS_TEST_REGISTER,&status_value) != 0x0)
          {
            errcode = 1;
            goto errexit;
          }
          while (Timeout != 0 && (status_value & reg_value))
          {
            if(Axi_MR(DOCSIS_DCSD_TS_TEST_REGISTER,&status_value) != 0x0)
            {
              errcode = 1;
              goto errexit;
            }
            Timeout--;
          }
          if(Timeout == 0)
          {
            errcode = 1;
            goto errexit;
          }
          InsertCount = 0;
        } //end if 188 --- one full TS has been processed
      } //end while
    break;
    case DS_FIFO_OC_ENABLE:
      while (fgets(NextByte,32,FillFile) != NULL)
      {
        next_value = strtoul(NextByte,&EndPtr,16); //get byte from file
        if(MW(MAC_REG_DS_TEST_WR,next_value) != 0x0)
        {
          errcode = 1;
          goto errexit;
        }
      } //end while
    break;
    case DS_FIFO_DOCSIS_0_ENABLE:
      WriteAddr = DOCSIS_DOCSIS_TEST_FIFO_0_BASE_ADDRESS;
      ByteCount = 0x0;
      next_value = (Timestamp_hi & 0xFFFF);
      next_value |= (ByteOffset & 0xFFFF) << 16;
      if(MW(WriteAddr,next_value) != 0x0)
      {
        errcode = 1;
        goto errexit;
      }
      WriteAddr += 0x4;
      ByteCount += 0x4;
      next_value = Timestamp_lo;
      if(MW(WriteAddr,next_value) != 0x0)
      {
        errcode = 1;
        goto errexit;
      }
      WriteAddr += 0x4;
      ByteCount += 0x4;
      i = 0;
      fifo_value = 0x0;
      while (fgets(NextByte,32,FillFile) != NULL)
      {
        next_value = strtoul(NextByte,&EndPtr,16); //get byte from file
        ByteCount++;
        mod_count = i%4;
        switch(mod_count)
        {
          case 0:
            fifo_value |= next_value;
          break;
          case 1:
            fifo_value |= (next_value << 8);
          break;
          case 2:
            fifo_value |= (next_value << 16);
          break;
          case 3:
            fifo_value |= (next_value << 24);
            if(MW(WriteAddr,fifo_value) != 0x0)
            {
              errcode = 1;
              goto errexit;
            }
            WriteAddr += 0x4;
            fifo_value = 0x0;
          break;
        } //end switch
        i++;
      } //end while
      mod_count = i%4;
      if(mod_count != 0x0)
      {
        if(MW(WriteAddr,fifo_value) != 0x0)
        {
          errcode = 1;
          goto errexit;
        }
      }
      reg_value =  (ByteCount & 0x7FF) | 0x8000;
      if(MW(DOCSIS_DCSD_DCS_TEST_0_REGISTER,reg_value) != 0x0)
      {
        errcode = 1;
        goto errexit;
      }
    break;
    default:
      errcode = 1;
    goto errexit;
  }
  errexit:
    fclose(FillFile);
  printf("**Fill_Fifo Completed .....\n");
  return errcode;
}/* Fill_Fifo() */

#endif
