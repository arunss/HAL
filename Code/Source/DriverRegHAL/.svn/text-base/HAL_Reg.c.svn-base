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
*     Has memory request/map and unmap for PHY SPI register region.
*----------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------*/
/* Headers.                                                                  */
/* --------------------------------------------------------------------------*/

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
#include "HAL_Reg.h"

//Include External Interfaces




/* --------------------------------------------------------------------------*/
/* Global Variables.                                                         */
/* --------------------------------------------------------------------------*/

PUINT32 phy_reg_base;
PUINT32 ds_mac_reg_base;
PUINT32 us_mac_reg_base;
PUINT32 dma_docsis_reg_base;
PUINT32 int_reg_base;

#ifdef _DEBUG_REG_BASED_
PUINT32 debug_reg_base;
PUINT32 debug_reg_base1;
PUINT32 debug_reg_base2;
#endif

/* --------------------------------------------------------------------------*/
/* Internal prototypes.                                                      */
/* --------------------------------------------------------------------------*/



/*----   DOCS_SPI_OSM_Setup()    ----
* 1.Reserve memory for
*  1.1 PHY_REG_BASE_ADDRESS
*  1.2 DS_MAC_REG_BASE_ADDRESS
*  1.3 US_MAC_REG_BASE_ADDRESS
*  1.4 DOC_ARM_REGISTER_BASE_ADDRESS
*  1.5 DMA_REG_BASE_ADDRESS
*  1.6 MAC_INT_REG_BASE_ADDRESS
*  1.7 UART_REGISTER_BASE_ADDRESS
*  1.8 RGMII0_REGISTER_BASE_ADDRESS
*  1.9 RGMII1_REGISTER_BASE_ADDRESS
*  1.10 GPIO_REGISTER_BASE_ADDRESS
*  1.11 ADDR_XLAT_REGISTER_BASE_ADDRESS
*  1.12 EXP_BUS_REGISTER_BASE_ADDRESS
*
* 2. Remap Memory for
*  2.1 PHY_REG_BASE_ADDRESS
*  2.2 DS_MAC_REG_BASE_ADDRESS
*  2.3 US_MAC_REG_BASE_ADDRESS
*  2.4 DOC_ARM_REGISTER_BASE_ADDRESS
*  2.5 DMA_REG_BASE_ADDRESS
*  2.6 MAC_INT_REG_BASE_ADDRESS
*  2.7 UART_REGISTER_BASE_ADDRESS
*  2.8 RGMII0_REGISTER_BASE_ADDRESS
*  2.9 RGMII1_REGISTER_BASE_ADDRESS
*  2.10 GPIO_REGISTER_BASE_ADDRESS
*  2.11 ADDR_XLAT_REGISTER_BASE_ADDRESS
*  2.12 EXP_BUS_REGISTER_BASE_ADDRESS
*--------------------------------*/
INT32 DOCS_HAL_REG_OSM_Setup()
{
    /* 1.1 Reserver PHY_REG_BASE_ADDRESS */
    if(!(OSM_MEM_RESERVE(PHY_REG_BASE_ADDRESS,PHY_REGISTER_SIZE,DRIVER_NAME)))
    {
        DPRINTE("Cant access region %x\n", PHY_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }
    //1.2 Reserver DS_MAC_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(DS_MAC_REG_BASE_ADDRESS, \
                                   DS_MAC_REGISTER_SIZE, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", DS_MAC_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }
    //1.3 Reserver US_MAC_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(US_MAC_REG_BASE_ADDRESS,\
                                  US_MAC_REGISTER_SIZE, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", US_MAC_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }

#ifdef _DEBUG_REG_BASED_
//Raghu, May 9th

    //1.3 Reserver US_MAC_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(0xF7FC0000,\
                                  0x04, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", US_MAC_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }



    //1.3 Reserver US_MAC_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(0xF7FC0004,\
                                  0x04, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", US_MAC_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }


    //1.3 Reserver US_MAC_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(0xF7FC0028,\
                                  0x04, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", US_MAC_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }

//Raghu, May 9th
#endif


    //1.5 Reserver DMA_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(DMA_REG_BASE_ADDRESS,\
                                  DMA_REGISTER_SIZE, DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", DMA_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }
    //1.6 Reserver MAC_INT_REG_BASE_ADDRESS
    if(!(OSM_MEM_RESERVE(MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE,\
                                                 DRIVER_NAME)) )
    {
        DPRINTE("Cant access region %x\n", MAC_INT_REG_BASE_ADDRESS);
        return HAL_FAILED;
    }



    /* 2. Remap Memory */

    //2.1 Remap PHY_REG_BASE_ADDRESS
    phy_reg_base=(PUINT32)OSM_MEM_REMAP(PHY_REG_BASE_ADDRESS,\
                                               PHY_REGISTER_SIZE);
    DPRINTK(" phy base addr %x :size %x \n logical addr %x\n ",\
               PHY_REG_BASE_ADDRESS, PHY_REGISTER_SIZE, \
                                         (UINT32)phy_reg_base );

    //2.2 Remap DS_MAC_REG_BASE_ADDRESS
  ds_mac_reg_base=(PUINT32)OSM_MEM_REMAP(DS_MAC_REG_BASE_ADDRESS,\
                                              DS_MAC_REGISTER_SIZE);
    DPRINTK(" ds_mac_reg base addr %x :size %x \n logical addr %x\n ",\
              DS_MAC_REG_BASE_ADDRESS, DS_MAC_REGISTER_SIZE, \
                                             (UINT32)ds_mac_reg_base );

  //2.3 Remap US_MAC_REG_BASE_ADDRESS
  us_mac_reg_base=(PUINT32)OSM_MEM_REMAP(US_MAC_REG_BASE_ADDRESS,\
                                            US_MAC_REGISTER_SIZE);
    DPRINTK(" us_mac_reg_base addr %x :size %x \n logical addr %x\n ",\
              US_MAC_REG_BASE_ADDRESS, US_MAC_REGISTER_SIZE,\
                                                 (UINT32)us_mac_reg_base );

  //2.5 Remap DMA_REG_BASE_ADDRESS
  dma_docsis_reg_base=(PUINT32)OSM_MEM_REMAP(DMA_REG_BASE_ADDRESS,\
                                              DMA_REGISTER_SIZE);
    DPRINTK(" dma_docsis_reg_base addr %x :size %x \n logical addr  %x\n ",\
              DMA_REG_BASE_ADDRESS, DMA_REGISTER_SIZE, \
                                        (UINT32)dma_docsis_reg_base );

  //2.6 Remap MAC_INT_REG_BASE_ADDRESS
  int_reg_base=(PUINT32)OSM_MEM_REMAP(MAC_INT_REG_BASE_ADDRESS, \
                                    INT_REGISTER_SIZE);
    DPRINTK(" int_reg_base addr %x :size %x \n logical addr  %x\n ",\
              MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE, \
                                          (UINT32)int_reg_base );

  
#ifdef _DEBUG_REG_BASED_
//Raghu, May 9th

  //2.7 Remap MAC_INT_REG_BASE_ADDRESS
  debug_reg_base=(PUINT32)OSM_MEM_REMAP(0xF7FC0000, \
                                    0x04);
    DPRINTK(" int_reg_base addr %x :size %x \n logical addr  %x\n ",\
              MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE, \
                                          (UINT32)int_reg_base );


  //2.7 Remap MAC_INT_REG_BASE_ADDRESS
  debug_reg_base1=(PUINT32)OSM_MEM_REMAP(0xF7FC0004, \
                                    0x04);
    DPRINTK(" int_reg_base addr %x :size %x \n logical addr  %x\n ",\
              MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE, \
                                          (UINT32)int_reg_base );

  //2.7 Remap MAC_INT_REG_BASE_ADDRESS
  debug_reg_base2=(PUINT32)OSM_MEM_REMAP(0xF7FC0028, \
                                    0x04);
    DPRINTK(" int_reg_base addr %x :size %x \n logical addr  %x\n ",\
              MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE, \
                                          (UINT32)int_reg_base );


//Raghu, May 9th
#endif

    return HAL_SUCCESS;
}


/*
 *Return the logical base address according to value of "Type"
 * It can MAC base , PHY base etc..
 */
UINT32 HAL_Reg_Get_Base(UINT32 Type)
{
    switch(Type)
    {
        case HAL_REG_PHY:
            return (UINT32) phy_reg_base;

        break;

        case HAL_REG_MAC_DS:
            return (UINT32) ds_mac_reg_base;
        break;

        case HAL_REG_MAC_US:
            return (UINT32)us_mac_reg_base;
        break;

        case HAL_REG_MAC_DMA:
            return (UINT32)dma_docsis_reg_base;
        break;

        case HAL_REG_INT:
            return (UINT32)int_reg_base; //I2C Reg changed to INT Reg
        break;


    }

    return 0;
}

EXPORT_SYMBOL(HAL_Reg_Get_Base);

INT32 HAL_Reg_Write(UINT32 Addr,UINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, Offset=0x00;

    switch(Type)
    {
        case HAL_REG_PHY:
                Offset = Addr - 0 ;
        break;

        case HAL_REG_MAC_DS:
            Offset = Addr - DS_MAC_REG_BASE_ADDRESS;
        break;

        case HAL_REG_MAC_US:
            Offset = Addr - US_MAC_REG_BASE_ADDRESS;
        break;

        case HAL_REG_MAC_DMA:
          Offset = Addr - DMA_REG_BASE_ADDRESS;
        break;

        case HAL_REG_INT:
           Offset = Addr - MAC_INT_REG_BASE_ADDRESS;
        break;
    }


    BaseAddr = HAL_Reg_Get_Base(Type);
    DPRINTK("BaseAddr 0x%08x, Offset 0x%08x",BaseAddr,Offset);

    Addr = (BaseAddr +  Offset); //& (0xFFFFFFF0) ;
    DPRINTK("value = 0x%08x , Addr = 0x%08x",Value,Addr);

    writel(Value,Addr);

    return HAL_SUCCESS;
}

EXPORT_SYMBOL(HAL_Reg_Write);

#ifdef _DEBUG_REG_BASED_
//Raghu, May 9th

INT32 HAL_Reg_Write_Debug(UINT32 Addr,UINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, New_Addr, Offset=0x00;


    Offset = Addr - 0xF7FC0000;

    New_Addr = *debug_reg_base + Offset;
   
    if (Addr == 0xf7fc0028) 
      printk("value = 0x%08x , Addr = 0x%08x, Nw_addr: 0x%08x\n",Value,Addr,
            New_Addr);

    writel(Value,debug_reg_base);
    //writel(Value,New_Addr);

    return HAL_SUCCESS;
}



INT32 HAL_Reg_Write_Debug1(UINT32 Addr,UINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, New_Addr, Offset=0x00;

    writel(Value,debug_reg_base1);

    return HAL_SUCCESS;
}


INT32 HAL_Reg_Write_Debug2(UINT32 Addr,UINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, New_Addr, Offset=0x00;

    writel(Value,debug_reg_base2);

    return HAL_SUCCESS;
}






EXPORT_SYMBOL(HAL_Reg_Write_Debug);
EXPORT_SYMBOL(HAL_Reg_Write_Debug1);
EXPORT_SYMBOL(HAL_Reg_Write_Debug2);
//Raghu, May 9th
#endif





INT32 HAL_Reg_Read(UINT32 Addr,PUINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr,Offset=0x00;

    BaseAddr = HAL_Reg_Get_Base(Type);
    //DPRINTK("BaseAddr %08x",BaseAddr);

    switch(Type)
    {
        case HAL_REG_PHY:
                Offset = Addr - 0 ;
        break;

        case HAL_REG_MAC_DS:
            Offset = Addr - DS_MAC_REG_BASE_ADDRESS;
        break;

        case HAL_REG_MAC_US:
            Offset = Addr - US_MAC_REG_BASE_ADDRESS;
        break;

        case HAL_REG_MAC_DMA:
          Offset = Addr - DMA_REG_BASE_ADDRESS;
        break;

        case HAL_REG_INT:
           Offset = Addr - MAC_INT_REG_BASE_ADDRESS;
        break;
    }

    Addr = (BaseAddr + Offset);// & (0xFFFFFFF0);

    *Value = readl(Addr);

    DPRINTK("Addr = %08x , value = %08x",Addr,*Value);

  return HAL_SUCCESS;
}

EXPORT_SYMBOL(HAL_Reg_Read);


#ifdef _DEBUG_REG_BASED_
//Raghu, May 9th

INT32 HAL_Reg_Read_Debug(UINT32 Addr,PUINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, Offset=0x00;
    PUINT32 New_Addr;  
 

    *Value = readl(debug_reg_base);

  return HAL_SUCCESS;
}


INT32 HAL_Reg_Read_Debug1(UINT32 Addr,PUINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, Offset=0x00;
    PUINT32 New_Addr;
 

    *Value = readl(debug_reg_base1);

  return HAL_SUCCESS;
}


INT32 HAL_Reg_Read_Debug2(UINT32 Addr,PUINT32 Value,UINT32 Type)
{
    UINT32 BaseAddr, Offset=0x00;
    PUINT32 New_Addr;
 

    *Value = readl(debug_reg_base2);

  return HAL_SUCCESS;
}


EXPORT_SYMBOL(HAL_Reg_Read_Debug);
EXPORT_SYMBOL(HAL_Reg_Read_Debug1);
EXPORT_SYMBOL(HAL_Reg_Read_Debug2);
//Raghu, May 9th
#endif

/*---- DOCS_ HAL_REG_OSM_Release() ---
 * 1.UNMAP for
*  1.1 PHY_REG_BASE_ADDRESS
*  1.2 DS_MAC_REG_BASE_ADDRESS
*  1.3 US_MAC_REG_BASE_ADDRESS
*  1.4 DOC_ARM_REGISTER_BASE_ADDRESS
*  1.5 DMA_REG_BASE_ADDRESS
*  1.6 MAC_INT_REG_BASE_ADDRESS
*  1.7 UART_REGISTER_BASE_ADDRESS
*  1.8 RGMII0_REGISTER_BASE_ADDRESS
*  1.9 RGMII1_REGISTER_BASE_ADDRESS
*  1.10 GPIO_REGISTER_BASE_ADDRESS
*  1.11 ADDR_XLAT_REGISTER_BASE_ADDRESS
*  1.12 EXP_BUS_REGISTER_BASE_ADDRESS
*
* 2. Release Memory for
*  2.1 PHY_REG_BASE_ADDRESS
*  2.2 DS_MAC_REG_BASE_ADDRESS
*  2.3 US_MAC_REG_BASE_ADDRESS
*  2.4 DOC_ARM_REGISTER_BASE_ADDRESS
*  2.5 DMA_REG_BASE_ADDRESS
*  2.6 MAC_INT_REG_BASE_ADDRESS
*  2.7 UART_REGISTER_BASE_ADDRESS
*  2.8 RGMII0_REGISTER_BASE_ADDRESS
*  2.9 RGMII1_REGISTER_BASE_ADDRESS
*  2.10 GPIO_REGISTER_BASE_ADDRESS
*  2.11 ADDR_XLAT_REGISTER_BASE_ADDRESS
*  2.12 EXP_BUS_REGISTER_BASE_ADDRESS
 *--------------------------------*/
INT32 DOCS_HAL_REG_OSM_Release()
{
    // 1.1  Unmap DOC_PHY_REGISTER  region
    OSM_MEM_UNMAP((void *)phy_reg_base);
    //1.2 Unmap DOC_DS_MAC_REGISTER region
    OSM_MEM_UNMAP((void *)ds_mac_reg_base);
    //1.3 Unmap DOC_US_MAC_REGISTER region
    OSM_MEM_UNMAP((void *)us_mac_reg_base);
    //1.5 Unmap DMA_DOCSIS_REGISTER  region
    OSM_MEM_UNMAP((void *)dma_docsis_reg_base);
    //1.6 Unmap I2C_REGISTER region
    OSM_MEM_UNMAP((void *)int_reg_base);

    // 2. Release Memory
    // 2.1 Release DOC_PHY_REGISTER  region
    OSM_MEM_RELEASE(PHY_REG_BASE_ADDRESS, PHY_REGISTER_SIZE);
    //2.2 Release DOC_DS_MAC_REGISTER region
    OSM_MEM_RELEASE(DS_MAC_REG_BASE_ADDRESS, DS_MAC_REGISTER_SIZE);
    //2.3 Release DOC_US_MAC_REGISTER region
    OSM_MEM_RELEASE(US_MAC_REG_BASE_ADDRESS, US_MAC_REGISTER_SIZE);
    //2.5 Release DMA_DOCSIS_REGISTER region
    OSM_MEM_RELEASE(DMA_REG_BASE_ADDRESS, DMA_REGISTER_SIZE);
    //2.6 Release I2C_REGISTER region
    OSM_MEM_RELEASE(MAC_INT_REG_BASE_ADDRESS, INT_REGISTER_SIZE);

    return HAL_SUCCESS;
}


#define DIV64CLK 0
#define CORE_FREQ 210e6

INT32 startClk(void);
INT32 stopClk(void);

struct em_function_data {
  int (*fn)(void);
  int ret;
};

static void em_func(void *data)
{
  struct em_function_data *d = data;
  int ret = d->fn();
  if (ret)
    d->ret = ret;
}

static int em_call_function(int (*fn)(void))
{
  struct em_function_data data;
  int cpu;

  data.fn = fn;
  data.ret = 0;

  preempt_disable();

  for_each_online_cpu(cpu)
    smp_call_function_single(cpu, em_func, &data, 0,1);

  //smp_call_function(em_func, &data, 1,1);

  //em_func(&data);
  preempt_enable();

  return data.ret;
}

INT32 startClk(VOID)
{
  // Clock divider: 400Mhz clk / 64, overflows the counter in 11 minutes
  const unsigned pmnc = (DIV64CLK << 3) | 0x7;
  arm11_write_pmnc(pmnc);
  return HAL_SUCCESS;
}

INT32 HAL_Pmu_Start_Clk(VOID)
{
  em_call_function(startClk);
  return HAL_SUCCESS;
}

EXPORT_SYMBOL(HAL_Pmu_Start_Clk);

INT32 stopClk(VOID)
{
  const unsigned pmnc = 0x0;
  arm11_write_pmnc(pmnc);
  return HAL_SUCCESS;
}

INT32 HAL_Pmu_Stop_Clk(VOID)
{
  em_call_function(stopClk);
  return HAL_SUCCESS;
}

EXPORT_SYMBOL(HAL_Pmu_Stop_Clk);

UINT32 getCCNT(VOID)
{
#ifdef _CCNT_ENABLED_
  unsigned ret;
  ret = arm11_get_counter(0x00);
#if DIV64CLK
  return (ret * 64);
#else
  return ret;
#endif
#else
  UINT32 CM_Time_Value;
  HAL_Reg_Write(0x0000e000, 0x00000041 ,HAL_REG_PHY);
  HAL_Reg_Read(0x0000e118, &CM_Time_Value, HAL_REG_PHY);

  //printk("\nStart CM Time Value = %x",CM_Time_Value);
  //printk("\nValue = %x",CM_Time_Value);
  return CM_Time_Value;
#endif
}

EXPORT_SYMBOL(getCCNT);
