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

/******************************************************************************
* PURPOSE
*  Dma core that implements the DMA descriptor mode of operation
*  This core is called by the driver for each specific context.
*  The core contains the functions to allocate memory for descriptor chain and
*  data buffers used for dma transfers
******************************************************************************/

/* -------------------------------------------------------------------------- */
/* Headers.                                                                   */
/* -------------------------------------------------------------------------- */

//Standard Header Files
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>

//Custom Header Files
#include "HAL_Types.h"
#include "DOCSBSPDefines.h"
#include "DOCSHALDMAInterface.h"
#include "DOCSHALDMADefines.h"

//Licensing and module descriptions
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("DMA driver");

/* -------------------------------------------------------------------------- */
/* Macros.     NA                                                             */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Internal globals. Variables and arrays                                     */
/* -------------------------------------------------------------------------- */
UINT32 Dma_Addr;
UINT32 Descr_Addr;
UINT32 Descr_Context[NUMBER_OF_DMA_CONTEXTS];
UINT32 Descr_Phys_Address[NUMBER_OF_DMA_CONTEXTS];
UINT32 Buffer_Address[NUMBER_OF_DMA_CONTEXTS][MAX_BUF_NO];
UINT32 Phys_Buff_Address[NUMBER_OF_DMA_CONTEXTS][MAX_BUF_NO];
UINT32 Temp_AddrOffset;

/* -------------------------------------------------------------------------- */
/* Internal prototypes.  -NA                                                  */
/* -------------------------------------------------------------------------- */

/* Accessing the DMA registers.
 *
 * 1. Request DMA register area for all contexts
 * 2. Map physical space to logical space
 * 3. Return Success or Failure
 * parameters passsed - null
 * Return type - 0/1 (success/failure)
 */
INT32 DOCS_DMA_REG_Access(VOID)
{

  /* 1. Request DMA register area for all contexts */
  if(!(request_mem_region(DMA_REGISTER_BASE,DMA_SIZE,DRIVER_NAME)))
    return HAL_FAILED;

    /* Request memory area to write the DMA descriptor chain */
  if(!(request_mem_region(DMA_DESCRIPTOR_BASE,DESCR_SIZE,DRIVER_NAME)))
    return HAL_FAILED;

  /* 2. Map Physical Space to Logical Space */
  Dma_Addr=(unsigned long)ioremap(DMA_REGISTER_BASE,DMA_SIZE);
  Descr_Addr=(unsigned long)ioremap(DMA_DESCRIPTOR_BASE,DESCR_SIZE);
  return HAL_SUCCESS;
}

/* Request space in memory for storing the descriptor chain of each context and
 * buffer space to store the data transferred.The maximum size of each buffer is
 * 4096 bytes. After filling a buffer the DMA descriptor chain is programmed to
 * transfer to the next buffer
 *
  * 1. Request Memory Space for all 12 DOCSIS DMA descriptor contexts.
  *    Max Space = 4096 bytes
  * 2. Store pointers to these logical locations in an array
  * 3. Convert the kernel logical addresses in array to physical addresses
  *   (needed later to program the descriptor chain)
  * 4. Store these values in another array.
  * 5. Initialize the allocated descriptor memory to 0
  * 6. Request memory space for 128 buffers each for each descriptor context
  *    (4096/32)allocDescr_Contextated
  * 7. Repeat steps 2 to 5 for the allocated buffers
 * parameters passsed - null
 * Return type - 0/1 (success/failure)
 */
INT32 DOCS_DMA_MEM_Request(VOID)
{

  INT32 array_pos,buf_no;
  UINT32 Print_Value;
  Temp_AddrOffset = 0;
  /* 1. Request Memory Space for all 12 DOCSIS DMA descriptor contexts. */

  for(array_pos = 0; array_pos < NUMBER_OF_DMA_CONTEXTS; array_pos++)
  {

    /* 2. Store pointers to these logical locations in an array */
    Descr_Context[array_pos] = Temp_AddrOffset;

    /*if(!Descr_Context[array_pos])
    {
      DPRINTK("Unable to allocate memory region for descriptor context %d",  \
               array_pos);
      return HAL_FAILED;
    }*/
    Print_Value = Descr_Context[array_pos] + MIRROR_BASE;
    /* 3. Convert the kernel logical addresses in array to physical addresses */
    /* 4. Store these values in another array. */
  //  Descr_Phys_Address[array_pos]=virt_to_phys(Descr_Context[array_pos]);
    DPRINTK("Desc Context %02d Physical Address 0x%08x",  \
    array_pos,Print_Value);

    /* 5. Initialize the allocated descriptor memory to 0 */

    //memset((char *)Descr_Context[array_pos], 0, MAX_DESCR_SIZE);
    Temp_AddrOffset += MAX_DESCR_SIZE;
  }

  Temp_AddrOffset = Print_Value - MIRROR_BASE;
  Temp_AddrOffset += MAX_DESCR_SIZE;
  /* 6. Request memory space for 128 buffers each for each descriptor context
   *    (4096/32) */
  for(array_pos = 8; array_pos < 10/*NUMBER_OF_DMA_CONTEXTS*/; array_pos++)
  {
    for(buf_no = 0; buf_no < MAX_BUF_NO; buf_no++)
    {
      /* 6.a. Store pointers to these logical locations in an array */
      //Buffer_Address[array_pos][buf_no] = kmalloc(MAX_BUF_SIZE, GFP_KERNEL);
      Buffer_Address[array_pos][buf_no] = Temp_AddrOffset;
      Print_Value = MIRROR_BASE + Buffer_Address[array_pos][buf_no];
      if(!Buffer_Address[array_pos][buf_no])
      {
        DPRINTK("Unable to allocate memory region for buffer %d in descriptor \
               context %d",  buf_no,array_pos);
        return HAL_FAILED;
      }

      /* 6.b. Convert kernel logical addresses in array to physical addresses */
      /* 6.iii. Store these values in another array. */
      //Phys_Buff_Address[array_pos][buf_no] =
                      //      virt_to_phys(Buffer_Address[array_pos][buf_no]);

        //DPRINTK("Desc Context %02d array no %03d phy_addr %08x offset %x",  \
             //array_pos,buf_no,Print_Value,Buffer_Address[array_pos][buf_no]);

      /* 6.iv. Initialize the allocated descriptor memory to 0 */
      //memset(&(Buffer_Address[array_pos][buf_no]), 0, MAX_BUF_SIZE);
      Temp_AddrOffset += MAX_DESCR_SIZE;
    }
  }
  return HAL_SUCCESS;
}

/* Freeing the DMA registers.
 *
 * 1. Unmap the DMA register area
 * 2. Free the DMA register area
 * parameters passsed - nuallocDescr_Contextatedll
 * Return type - void
 */
VOID DOCS_DMA_REG_Free(VOID)
{

  /* 1. Unmap the DMA register area */
  iounmap((void *)Dma_Addr);

  /* Unmap the descriptor area */
  iounmap((void*)Descr_Addr);

  /* 2. Free the DMA register area */
  release_mem_region(DMA_REGISTER_BASE,DMA_SIZE);

    /* 3. Free the descriptor area */
  release_mem_region(DMA_DESCRIPTOR_BASE,DESCR_SIZE);
  return;
}

/* Release the memory allocated for descriptor chain and data buffers
 *
 * 1. Release memory allocDescr_Contextated for descriptor contexts
 * 2. Release memory allocated for data buffers
 * parameters passsed - null
 * Return type - void
 */
VOID DOCS_DMA_MEM_Free(VOID)
{
  //INT32 array_pos, buf_no;
  /* 1. Release memory allocated for descriptor contexts */
//  for(array_pos=0;array_pos<NUMBER_OF_DMA_CONTEXTS;array_pos++)
  //  kfree(Descr_Context[array_pos]);

  /* 2. Release memory allocated for data buffers */
/*  for(array_pos = 0; array_pos < NUMBER_OF_DMA_CONTEXTS; array_pos++)
  {
    for(buf_no = 0; buf_no < MAX_BUF_NO; buf_no++)
    {
      kfree(Buffer_Address[array_pos][buf_no]);
    }
  } */
  return;
}

/* Build the DMA descriptor chain. Each descriptor block is 8 bytes wide. The
 * first field is the starting address of the next descriptor block. Second
 * field is the address of data buffer in memory. Third field is the parameter
 * field for setting the length of transfer, delay between successive transfers
 * etc. Fourth field is the control field. The remaining four fields can be
 * ignored at present.
 *
 * 1. Program the descriptDescr_Contextor chain as follows.
 * The parameter register is programmed to transfer
 *  a total block size of 4096 bytes with a burst size of 1
 * 2. The mode control register is programmed in descriptor mode with dma
 *    writeback and end of packet asserted. Interrupts, error checking etc are
 *    not enabled currently
 * 3. The remaing four fields are filled with 0xdeadbeef at present
 * 4. The above fields are repeated for the first 2 descriptor blocks
 * 5. The last descriptor bit in the mode control register field of the 3rd
 *    descriptor block is set to 1.
 * 6. Currently each descriptor context is programmed seperately
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_DESC_Build(INT32 context)
{

  UINT32 Next_Descr,Descriptor_Location, Transfer_Location;
  INT32 buf_no,value,Curr_Descr;
  Next_Descr = Descr_Context[context];
  DPRINTK("next Desc %8x \n",Next_Descr);
//  DPRINTK("Descr_Addr : %8x",Descr_Addr);
  Descriptor_Location = context * MAX_DESCR_SIZE + MIRROR_BASE;
  DPRINTK("Descriptor chain start address  : %8x",Descriptor_Location);
  Curr_Descr = Descriptor_Location - MIRROR_BASE + Descr_Addr;
//  DPRINTK("Curr_addr  : %8x",Curr_Descr);
  DPRINTK("");
  for(buf_no = 0; buf_no < MAX_BUF_NO; buf_no++)
  {
    /*1. First location stores the next descriptor block address */
    Next_Descr += 0x20;
    Descriptor_Location = Next_Descr + MIRROR_BASE;
    //DPRINTK("Location of next descriptor %8x",Descriptor_Location);
    writel(Next_Descr,Curr_Descr);
    /*DPRINTK("Value %08x Address %x",Next_Descr,Curr_Descr); */
    /*2. Second location stores the address of data buffer */
    Curr_Descr+=0x4;
    writel(Buffer_Address[context][buf_no],Curr_Descr);
    Transfer_Location = Buffer_Address[context][buf_no] + MIRROR_BASE;
    //DPRINTK("Location of transfer %d is %x",buf_no, \
        //Transfer_Location);
    /*3. Third location stores the parameter register contents */
    Curr_Descr+=0x4;
    writel(0x00000fff,Curr_Descr);
    //DPRINTK("Value %08x Address %x",0x00000fff,Curr_Descr);
    /*4. Fourth location stores the control register contents */
    Curr_Descr+=0x4;
    /*5. Check if last block. If so program last descriptor bit */
    if(buf_no == (MAX_BUF_NO-1))
      value = 0x0a000000;
    else
      value = 0x08000000;
    writel(value,Curr_Descr);
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /*6. Fifth location is the current descriptor register */
  //  Curr_Descr+=0x4;
    /*7. Check if last block */
  /*  if(buf_no == (MAX_BUF_NO-1))
      value = 0x0a000000;
    else
      value = 0x08000000;
    writel(value,Curr_Descr); */
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /* Ignore the remaining three registers */
    Curr_Descr+=0x14;
    //DPRINTK("");
  }
  DPRINTK("\nRetuned from building descriptor\n");
  return ;
}

/* DMA reset routine
 *
 * 1. Write 1 to bits Desc mode and Reg mode in the control register
 * 2. Read these locations. 0 implies reset done
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_RST_Routine(context)
{

  UINT32 Control_Addr,Status;
  Status = 1;
  /* 1. Write 1 to bits Desc mode and Reg mode in the control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  writel(0x60000000,Control_Addr);

  /* 2. Read these locations. 0 implies reset done */
  while(Status!=0)
  {
    Status = readl(Control_Addr);
    Status = Status&0x60000000;
  }
  return;
}

/* DMA descriptor mode start routine
 *
 * Copy the address of the DMA descriptor chain to the next descriptor address
 * of the current context
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_START_Routine(context)
{
  UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr;
  UINT32 Print_Addr;
  DPRINTK("");
  DPRINTK("Entered DMA Descriptor Start Routine");
  Next_Descr_Addr = Dma_Addr + 0x20*context;
  Parameter_Addr = Next_Descr_Addr + 0x8;
  Control_Addr = Parameter_Addr + 0x4;
  Print_Addr = Next_Descr_Addr - Dma_Addr + DMA_REGISTER_BASE;
  DPRINTK("Dma Context base :%0x Value at first location %0x",   \
              Print_Addr,Descr_Context[context]);
  writel(Descr_Context[context],Next_Descr_Addr);
  writel(0x00000100,Parameter_Addr);
  writel(0x28000000,Control_Addr);
  return;
}

/* DMA register mode start routine
 *
 * Start register mode transfer
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_REG_START_Routine(context)
{
  UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr,Start_Addr;
  UINT32 Print_Addr;
  DPRINTK("");
  DPRINTK("Entered DMA Register Start Routine");
  Next_Descr_Addr = Dma_Addr + 0x20*context;
  Start_Addr = Next_Descr_Addr + 0x4;
  Parameter_Addr = Next_Descr_Addr + 0x8;
  Control_Addr = Parameter_Addr + 0x4;
  Print_Addr = Next_Descr_Addr - Dma_Addr + DMA_REGISTER_BASE;
  DPRINTK("Dma Context base :%0x Value at first location %0x",   \
              Print_Addr,(0xd000+((context-4)*4096) ));
  /*DPRINTK("Next Desc : %0x, Cntrl Addr :%0x, Param addr :%0x", \
           Next_Descr_Addr, Parameter_Addr,Control_Addr); */
  //Trans_Location = Phys_Buff_Address[context][8]+0x70000000;
  //DPRINTK("Location of register transfer : %0x",Trans_Location);
  writel((0xd000+((context-4)*4096) ),Start_Addr);
  writel(0x000003cf,Parameter_Addr);
  writel(0x48000000,Control_Addr);
  return;
}

/* DMA write back mode start routine
 *
 * Copy the address of the DMA descriptor chain to the next descriptor address
 * of the current context
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_WBSTART_Routine(context)
{
  UINT32 Next_Descr_Addr,Control_Addr,Parameter_Addr;
  DPRINTK("");
  DPRINTK("Entered DMA Start Routine");
  Next_Descr_Addr = Dma_Addr + 0x20*context;
  Parameter_Addr = Next_Descr_Addr + 0x8;
  Control_Addr = Parameter_Addr + 0x4;
  DPRINTK("Next Desc : %0x, Cntrl Addr :%0x, Param addr :%0x",Next_Descr_Addr, \
          Parameter_Addr,Control_Addr);
  writel(Descr_Phys_Address[context],Next_Descr_Addr);
  writel(0x00000100,Parameter_Addr);
  writel(0x29000000,Control_Addr);
  return;
}

/* Build the DMA descriptor chain for writeback mode.
 * Each descriptor block is 8 bytes wide. The first field is the starting
 * address of the next descriptor block. Second field is the address of data
 * buffer in memory. Third field is the parameter field for setting the length
 * of transfer, delay between successive transfers etc. Fourth field is the
 * control field. The remaining four fields can be ignored at present.
 *
 * 1. Program the descriptor chain as follows. The parameter register is
 *    programmed to transfer a total block size of 4096 bytes with a burst size
 *    of 1
 * 2. The mode control register is programmed in descriptor mode with dma
 *    writeback and end of packet asserted. Interrupts, error checking etc are
 *    not enabled currently
 * 3. The remaing four fields are filled with 0xdeadbeef at present
 * 4. The above fields are repeated for the first 2 descriptor blocks
 * 5. The last descriptor bit in the mode control register field of the 3rd
 *    descriptor block is set to 1.
 * 6. Currently each descriptor context is programmed seperately
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_WBDESC_Build(INT32 context)
{

  UINT32 Next_Descr;
  INT32 buf_no,value,Curr_Descr;
  Next_Descr = Descr_Phys_Address[context];
  DPRINTK("");
  DPRINTK("Descr_Addr : %8x",Descr_Addr);
  DPRINTK("phys_addr  : %8x",Descr_Phys_Address[context]);
  Curr_Descr = (Descr_Phys_Address[context]-(DMA_DESCRIPTOR_BASE-MIRROR_BASE)) \
                + Descr_Addr;
  DPRINTK("Curr_addr  : %8x",Curr_Descr);
  DPRINTK("");
  for(buf_no = 0; buf_no < 3/*MAX_BUF_NO*/; buf_no++)
  {
    /*1. First location stores the next descriptor block address */
    Next_Descr += 0x20;
    DPRINTK("Next_Descr : %8x",Next_Descr);
    writel(Next_Descr,Curr_Descr);
    DPRINTK("Value %08x Address %x",Next_Descr,Curr_Descr);
    /*2. Second location stores the address of data buffer */
    Curr_Descr+=0x4;
    writel(Phys_Buff_Address[context][buf_no],Curr_Descr);
    DPRINTK("Value %08x Address %x",Phys_Buff_Address[context][buf_no],  \
             Curr_Descr);
    /*3. Third location stores the parameter register contents */
    Curr_Descr+=0x4;
    writel(0x00000fff,Curr_Descr);
    DPRINTK("Value %08x Address %x",0x00000fff,Curr_Descr);
    /*4. Fourth location stores the control register contents */
    Curr_Descr+=0x4;
    /*5. Check if last block. If so program last descriptor bit */
    if(buf_no == (2/*MAX_BUF_NO-1*/))
      value = 0x2b000000;
    else
      value = 0x29000000;
    writel(value,Curr_Descr);
    DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /*6. Fifth location is the current descriptor register */
  //  Curr_Descr+=0x4;
    /*7. Check if last block */
    //if(buf_no == (2/*MAX_BUF_NO-1*/))
  /*    value = 0x2b000000;
    else
      value = 0x29000000;
    writel(value,Curr_Descr);
    DPRINTK("Value %08x Address %x",value,Curr_Descr); */
    /* Ignore the remaining three registers */
    Curr_Descr+=0x14;
    DPRINTK("");
  }
  return ;
}

/* Build the DMA descriptor chain for different transfer sizes.
 * Each descriptor block is 8 bytes wide. The first field is the starting
 * address of the next descriptor block. Second field is the address of data
 * buffer in memory. Third field is the parameter field for setting the length
 * of transfer, delay between successive transfers etc. Fourth field is the
 * control field. The remaining four fields can be ignored at present.
 *
 * 1. Program the descriptor chain as follows. The parameter register is
 *    programmed to transfer a total block size of 4096 bytes with a burst size
 *    of 1
 * 2. The mode control register is programmed in descriptor mode with dma
 *    writeback and end of packet asserted. Interrupts, error checking etc are
 *    not enabled currently
 * 3. The remaing four fields are filled with 0xdeadbeef at present
 * 4. The above fields are repeated for the first 2 descriptor blocks
 * 5. The last descriptor bit in the mode control register field of the 3rd
 *    descriptor block is set to 1.
 * 6. Currently each descriptor context is programmed seperately
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_DESCR_VAR_Build(INT32 context)
{

  UINT32 Next_Descr;
  INT32 buf_no,value,Curr_Descr;
  Next_Descr = Descr_Phys_Address[context];
  DPRINTK("");
  DPRINTK("Descr_Addr : %8x",Descr_Addr);
  DPRINTK("phys_addr  : %8x",Descr_Phys_Address[context]);
  Curr_Descr = (Descr_Phys_Address[context]-(DMA_DESCRIPTOR_BASE-MIRROR_BASE)) \
                + Descr_Addr;
  DPRINTK("Curr_addr  : %8x",Curr_Descr);
  DPRINTK("");
  for(buf_no = 0; buf_no < 3/*MAX_BUF_NO*/; buf_no++)
  {
    /*1. First location stores the next descriptor block address */
    Next_Descr += 0x20;
    DPRINTK("Next_Descr : %8x",Next_Descr);
    writel(Next_Descr,Curr_Descr);
    DPRINTK("Value %08x Address %x",Next_Descr,Curr_Descr);
    /*2. Second location stores the address of data buffer */
    Curr_Descr+=0x4;
    writel(Phys_Buff_Address[context][buf_no],Curr_Descr);
    DPRINTK("Value %08x Address %x",Phys_Buff_Address[context][buf_no],  \
             Curr_Descr);
    /*3. Third location stores the parameter register contents */
    Curr_Descr+=0x4;
    if(buf_no == 0)
    {
      writel(0x00000001,Curr_Descr);
      DPRINTK("Value %08x Address %x",0x00000001,Curr_Descr);
    }
    else if(buf_no == 1)
    {
      writel(0x00000002,Curr_Descr);
      DPRINTK("Value %08x Address %x",0x00000002,Curr_Descr);
    }
    else
    {
      writel(0x00000003,Curr_Descr);
      DPRINTK("Value %08x Address %x",0x00000003,Curr_Descr);
    }
    /*4. Fourth location stores the control register contents */
    Curr_Descr+=0x4;
    /*5. Check if last block. If so program last descriptor bit */
    if(buf_no == (2/*MAX_BUF_NO-1*/))
      value = 0x0a000000;
    else
      value = 0x08000000;
    writel(value,Curr_Descr);
    DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /*6. Fifth location is the current descriptor register */
  //  Curr_Descr+=0x4;
    /*7. Check if last block */
    //if(buf_no == (2/*MAX_BUF_NO-1*/))
  /*    value = 0x2b000000;
    else
      value = 0x29000000;
    writel(value,Curr_Descr);
    DPRINTK("Value %08x Address %x",value,Curr_Descr); */
    /* Ignore the remaining three registers */
    Curr_Descr+=0x14;
    DPRINTK("");
  }
  return ;
}

/* DMA terminate routine
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_TERM_Routine(context)
{

  UINT32 Control_Addr,Status,Term_Value;
  Status = 0;
  /* 1. Write 1 to bits Desc mode and Reg mode in the control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Term_Value = readl(Control_Addr);
  DPRINTK("Value %08x",Term_Value);
  Term_Value&=0x9FFFFFFF;
  DPRINTK("Value to write to register %08x",Term_Value);
  writel(Term_Value,Control_Addr);

  return;
}

/* DMA read routine
 *
 * 1. Find DMA context
 * 2. Pass the address of corresponding first data buffer to user space
 * parameters passsed - dma context, argument address
 * Return type - void
 */
INT32 DOCS_DMA_READ_Routine(context,arg)
{
  UINT32 Dma_Buffer;
  /* location of buffer address */
  arg+=4;
  Dma_Buffer = Phys_Buff_Address[context][0];
  if(copy_to_user((unsigned int*)arg,&Dma_Buffer,sizeof(int)))
  {
    DPRINTK("Unable to copy to user space");
    return HAL_FAILED;
  }
  return HAL_SUCCESS;
}

/* DMA suspend routine 1
 * 
 * 1. Write 1 to HOLD bit in the control register
 *    (mode control register value)|0x10000000
 * 2. Read the bit. Hold bit should be set to 1
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_SUSP_Routine_1(context)
{
  UINT32 Control_Addr,Status,Susp_value;
  DPRINTK("Entered suspend routine");
  Status = 0;

  /* 1. Set hold bit to 1 in mode control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Susp_value = readl(Control_Addr);
  DPRINTK("Value at mode control register %08x",Susp_value);
  Susp_value|= 0x10000000;
  DPRINTK("Value to write to register %08x",Susp_value);
  writel(Susp_value,Control_Addr);

  /* 2. Read these locations. The hold bit should be set*/
  //for(i=0;i<100;i++)
  //{
    Status = readl(Control_Addr);
    DPRINTK("Value at mode register %08x",Status);
  //}

  return;
}

/* DMA suspend routine 2
 *
 * 1. Write 1 to HOLD bit in the control register
 *    (0x10000000)
 * 2. Read the bit. Hold bit should be set to 1
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_SUSP_Routine_2(context)
{
  UINT32 Control_Addr,Status,Susp_value;
  DPRINTK("Entered suspend routine");
  Status = 0;

  /* 1. Set hold bit to 1 in mode control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Susp_value = readl(Control_Addr);
  DPRINTK("Value at mode control register %08x",Susp_value);
  Susp_value = 0x10000000;
  DPRINTK("Value to write to register %08x",Susp_value);
  writel(Susp_value,Control_Addr);

  /* 2. Read these locations. The hold bit should be set*/
  //for(i=0;i<100;i++)
  //{
  Status = readl(Control_Addr);
  DPRINTK("Value at mode register %08x",Status);
  //}

  return;
}

/* Build the DMA descriptor chain to test hold sequence. There are 5 descriptor
 * blocks in the descriptor chain. The second, third, fourth and fifth blocks
 * have their hold bit set. The decriptor bit is also set.
 *
 * 1. Program the descriptor chain as follows. The parameter register is
 *    programmed to transfer a total block size of 4096 bytes with a burst size
 *    of 1
 * 2. The mode control register is programmed in descriptor mode with dma
 *    hold and end of packet asserted. Interrupts, error checking etc are
 *    not enabled currently. 2nd, 3rd, 4th and 5th blocks have hold bit set.
 * 3. The remaing four fields are filled with 0xdeadbeef at present
 * 4. The above fields are repeated for the first 2 descriptor blocks
 * 5. The last descriptor bit in the mode control register field of the 3rd
 *    descriptor block is set to 1.
 * 6. Currently each descriptor context is programmed seperately
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_DESC_Hold(INT32 context)
{

  UINT32 Next_Descr,Descriptor_Location, Transfer_Location, Temp_Value, \
         Control_Addr;
  INT32 buf_no,value,Curr_Descr;
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Next_Descr = Descr_Phys_Address[context];
  DPRINTK("");
//  DPRINTK("Descr_Addr : %8x",Descr_Addr);
  Descriptor_Location = Descr_Phys_Address[context] + 0x70000000;
  DPRINTK("Descriptor chain start address  : %8x",Descriptor_Location);
  Curr_Descr = (Descr_Phys_Address[context]-(DMA_DESCRIPTOR_BASE-MIRROR_BASE)) \
                + Descr_Addr;
//  DPRINTK("Curr_addr  : %8x",Curr_Descr);
  DPRINTK("");
  for(buf_no = 0; buf_no < 5/*MAX_BUF_NO*/; buf_no++)
  {
    /*1. First location stores the next descriptor block address */
    Next_Descr += 0x20;
    Descriptor_Location = Next_Descr + 0x70000000;
    DPRINTK("Location of next descriptor %8x",Descriptor_Location);
    writel(Next_Descr,Curr_Descr);
    /*DPRINTK("Value %08x Address %x",Next_Descr,Curr_Descr); */
    /*2. Second location stores the address of data buffer */
    Curr_Descr+=0x4;
    writel(Phys_Buff_Address[context][buf_no],Curr_Descr);
    Transfer_Location = Phys_Buff_Address[context][buf_no] + 0x70000000;
    DPRINTK("Location of transfer %d is %x",buf_no, \
        Transfer_Location);
    /*3. Third location stores the parameter register contents */
    Curr_Descr+=0x4;
    writel(0x00000fff,Curr_Descr);
    //DPRINTK("Value %08x Address %x",0x00000fff,Curr_Descr);
    /*4. Fourth location stores the control register contents */
    Curr_Descr+=0x4;
    if(buf_no == 0)
      value = 0x28000000;
    /*5. Check if last block. If so program last descriptor bit */
    else if(buf_no == (4/*MAX_BUF_NO-1*/))
      value = 0x3a000000;
    else
      value = 0x38000000;
    writel(value,Curr_Descr);
    Temp_Value = readl(Curr_Descr);
    DPRINTK("Value written to mode contol register : %08x",Temp_Value);
    //Temp_Value = readl(Control_Addr);
  /*  DPRINTK("Value at register location : %08x",Temp_Value);
    mdelay(500);
    Temp_Value = readl(Control_Addr);
    DPRINTK("Value after 0.5 seconds : %08x",Temp_Value); */
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /*6. Fifth location is the current descriptor register */
    //Curr_Descr+=0x4;
    /*7. Check if last block */
    //if(buf_no == (2/*MAX_BUF_NO-1*/))
    //  value = 0x0a000000;
    //else
      //value = 0x08000000;
    //writel(value,Curr_Descr);
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /* Ignore the remaining three registers */
    Curr_Descr+=0x14;
    DPRINTK("");
  }
  return ;
}

/* Build the DMA descriptor chain to test hold sequence. There are 5 descriptor
 * blocks in the descriptor chain. The second, third, fourth and fifth blocks
 * have their hold bit set. Here the descriptor bit is not set
 *
 * 1. Program the descriptor chain as follows. The parameter register is
 *    programmed to transfer a total block size of 4096 bytes with a burst size
 *    of 1
 * 2. The mode control register is programmed in descriptor mode with dma
 *    hold and end of packet asserted. Interrupts, error checking etc are
 *    not enabled currently. 2nd, 3rd, 4th and 5th blocks have hold bit set.
 * 3. The remaing four fields are filled with 0xdeadbeef at present
 * 4. The above fields are repeated for the first 2 descriptor blocks
 * 5. The last descriptor bit in the mode control register field of the 3rd
 *    descriptor block is set to 1.
 * 6. Currently each descriptor context is programmed seperately
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_DESC_Hold_1(INT32 context)
{

  UINT32 Next_Descr,Descriptor_Location, Transfer_Location, Control_Addr;
  INT32 buf_no,value,Curr_Descr;

  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Next_Descr = Descr_Phys_Address[context];
  DPRINTK("");

//  DPRINTK("Descr_Addr : %8x",Descr_Addr);
  Descriptor_Location = Descr_Phys_Address[context] + 0x70000000;
  DPRINTK("Descriptor chain start address  : %8x",Descriptor_Location);

  Curr_Descr = (Descr_Phys_Address[context]-(DMA_DESCRIPTOR_BASE-MIRROR_BASE)) \
                + Descr_Addr;
//  DPRINTK("Curr_addr  : %8x",Curr_Descr);
  DPRINTK("");
  for(buf_no = 0; buf_no < 5/*MAX_BUF_NO*/; buf_no++)
  {
    /*1. First location stores the next descriptor block address */
    Next_Descr += 0x20;
    Descriptor_Location = Next_Descr + 0x70000000;
    DPRINTK("Location of next descriptor %8x",Descriptor_Location);
    writel(Next_Descr,Curr_Descr);
    /*DPRINTK("Value %08x Address %x",Next_Descr,Curr_Descr); */

    /*2. Second location stores the address of data buffer */
    Curr_Descr+=0x4;
    writel(Phys_Buff_Address[context][buf_no],Curr_Descr);
    Transfer_Location = Phys_Buff_Address[context][buf_no] + 0x70000000;
    DPRINTK("Location of transfer %d is %x",buf_no, \
        Transfer_Location);

    /*3. Third location stores the parameter register contents */
    Curr_Descr+=0x4;
    writel(0x00000fff,Curr_Descr);
    //DPRINTK("Value %08x Address %x",0x00000fff,Curr_Descr);

    /*4. Fourth location stores the control register contents */
    Curr_Descr+=0x4;
    /*5. Check if last block. If so program last descriptor bit */
    if(buf_no == (4/*MAX_BUF_NO-1*/))
      value = 0x0a000000;
    else
      value = 0x08000000;
    writel(value,Curr_Descr);
  /*  Temp_Value = readl(Curr_Descr);
    DPRINTK("Value written to mode contol register : %08x",Temp_Value);

    Temp_Value = readl(Control_Addr);
    DPRINTK("Value at register location : %08x",Temp_Value);

    mdelay(500);

    Temp_Value = readl(Control_Addr);
    DPRINTK("Value after 0.5 seconds : %08x",Temp_Value); */
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /*6. Fifth location is the current descriptor register */
    //Curr_Descr+=0x4;
    /*7. Check if last block */
    //if(buf_no == (2/*MAX_BUF_NO-1*/))
    //  value = 0x0a000000;
    //else
      //value = 0x08000000;
    //writel(value,Curr_Descr);
  //  DPRINTK("Value %08x Address %x",value,Curr_Descr);
    /* Ignore the remaining three registers */
    Curr_Descr+=0x14;
    DPRINTK("");
  }
  return ;
}

/* read DMA descriptor
 * parameters - context
 * return type - void */
VOID DOCS_DMA_DESC_Read(INT32 context)
{
  UINT32 Control_Addr, Temp_Value;
  DPRINTK("Entered read routine");
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Temp_Value = readl(Control_Addr);
  DPRINTK("Value at register location : %08x",Temp_Value);
  mdelay(500);
  Temp_Value = readl(Control_Addr);
  DPRINTK("Value after 0.5 seconds : %08x",Temp_Value);
  mdelay(500);
  Temp_Value = readl(Control_Addr);
  DPRINTK("Value after 1 second : %08x",Temp_Value);
  mdelay(500);
  Temp_Value = readl(Control_Addr);
  DPRINTK("Value after 1.5 seconds : %08x",Temp_Value);
  mdelay(500);
  Temp_Value = readl(Control_Addr);
  DPRINTK("Value after 2 seconds : %08x",Temp_Value);
}

/* DMA suspend routine 3
 *
 * 1. Write 1 to HOLD bit in the control register
 *    (mode control register value)|0x30000000
 * 2. Read the bit. Hold bit should be set to 1
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_SUSP_Routine_3(context)
{
  UINT32 Control_Addr,Status,Susp_value;
  DPRINTK("Entered suspend routine");
  Status = 0;

  /* 1. Set hold bit to 1 in mode control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Susp_value = readl(Control_Addr);
  DPRINTK("Value at mode control register %08x",Susp_value);
  Susp_value|= 0x30000000;
  DPRINTK("Value to write to register %08x",Susp_value);
  writel(Susp_value,Control_Addr);

  /* 2. Read these locations. The hold bit should be set*/
  //for(i=0;i<100;i++)
  //{
    Status = readl(Control_Addr);
    DPRINTK("Value at mode register %08x",Status);
  //}

  return;
}

/* DMA suspend routine 1
 *
 * 1. Write 1 to HOLD bit in the control register
 *    (mode control register value)|0x50000000
 * 2. Read the bit. Hold bit should be set to 1
 * parameters passsed - dma context
 * Return type - void
 */
VOID DOCS_DMA_SUSP_Routine_4(context)
{
  UINT32 Control_Addr,Status,Susp_value;
  DPRINTK("Entered suspend routine");
  Status = 0;

  /* 1. Set hold bit to 1 in mode control register */
  Control_Addr = Dma_Addr + 0x20*context + 0xc;
  Susp_value = readl(Control_Addr);
  DPRINTK("Value at mode control register %08x",Susp_value);
  Susp_value|= 0x50000000;
  DPRINTK("Value to write to register %08x",Susp_value);
  writel(Susp_value,Control_Addr);

  /* 2. Read these locations. The hold bit should be set*/
  //for(i=0;i<100;i++)
  //{
    Status = readl(Control_Addr);
    DPRINTK("Value at mode register %08x",Status);
  //}

  return;
}
//-----------------DOCS_DMA_REG_READ---------------------
//Read the regisers/ memory region mapped by DMA drviver
//Copy to user space
//------------------------------------------------------
INT32 DOCS_DMA_REG_READ(UINT32 Addr,UINT32 arg)
{
  UINT32 Data,drv_addr,offset;
  //Validate the address paassed
  //Calculate offset from DMA base address need to be added
  //with logical address
  //Assign drv_addr for region Addr is mapped
    if((Addr >= DMA_REGISTER_BASE) && ( Addr < (DMA_REGISTER_BASE + DMA_SIZE)))
    {
        offset = Addr - DMA_REGISTER_BASE ;
        drv_addr = Dma_Addr;
    }
    else if((Addr>= DMA_DESCRIPTOR_BASE )&& (Addr < \
                        (DMA_DESCRIPTOR_BASE + DESCR_SIZE)))
    {
      offset = Addr - DMA_DESCRIPTOR_BASE ;
      drv_addr = Descr_Addr;
    }

    else
    {
        DPRINTK("DMA -Error in address value transformed, Address %x",Addr);
        return HAL_FAILED;
    }
    //read from location
    Data = readl(drv_addr + offset);
    arg+=4;// change
    //Copy to user space ( arg )
    if( copy_to_user((unsigned int*)arg,&Data,sizeof(int)) )
    {
       DPRINTK("Unable to copy to user space");
       return HAL_FAILED;
    }
    return HAL_SUCCESS;
}

//---------------DOCS_DMA_REG_WRITE-------------------------------
//Get the Value from User space
//Write to the given address which is also copied from user space
//----------------------------------------------------------------
INT32 DOCS_DMA_REG_WRITE(UINT32 Addr,UINT32 arg)
{
  UINT32 Data,drv_addr,offset;

  //Validate the address paassed
  //Calculate offset from DMA base address need to be added
  //with logical address
  //Assign drv_addr for region Addr is mapped
    if((Addr >= DMA_REGISTER_BASE) && ( Addr < (DMA_REGISTER_BASE + DMA_SIZE)))
    {
        offset = Addr - DMA_REGISTER_BASE ;
        drv_addr = Dma_Addr;
    }
    else if((Addr>= DMA_DESCRIPTOR_BASE )&& (Addr < \
                                (DMA_DESCRIPTOR_BASE + DESCR_SIZE)))
    {
      offset = Addr - DMA_DESCRIPTOR_BASE ;
      drv_addr = Descr_Addr;
    }

    else
    {
        DPRINTK("DMA -Error in address value transformed, Address %x",Addr);
        return HAL_FAILED;
    }

    arg+=4;// change
    //copy the Data from user space which is to be written to given adddress
    if(copy_from_user(&Data,(unsigned int*)arg,sizeof(int)))
    {
                DPRINTK("Unable to copy from user space");
                return HAL_FAILED;
    }
    //write the data to given address
    writel(Data,(drv_addr + offset));
    return HAL_SUCCESS;
}


