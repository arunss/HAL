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
* Contains Usdriver core functions definitions
*   Has functions HAL_Us_Hardware_Init, HAL_Us_Phy_Init, HAL_Us_Enable,
*   HAL_Us_Disable andHAL_Us_Config_Fifo
*----------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------ */
/* Headers.                                                                 */
/* ------------------------------------------------------------------------ */


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

//Custom Header Files
#include "HAL_Us.h"



/* ------------------------------------------------------------------------ */
/* Functions.                                                               */
/* ------------------------------------------------------------------------ */



/******************************************************************************
 *
 * Name: HAL_Us_Hardware_Init
 *
 * Description:
 *  Does US Mac Hardware Initialization.
 *  TBD: SCDMA Related Initialiation is TBD
 *
 * Arguments:
 *    NONE
 *
 *
 * Return Value:
 *   HAL_SUCCESS : Returns Always HAL_SUCCESS
 *           
 *
 *****************************************************************************/

INT32 HAL_Us_Hardware_Init()
{
  //1.US PHy init
  HAL_Us_Phy_Init();

  //2. Configure US for PHY Mode
  HAL_Us_Config_Fifo();

  //3. Nyquist coefficient
  HAL_Us_Nyquist_Coefficient_Init();

  //4. US TDMA Related Configuration;

  HAL_Reg_Write(0x0000e40c,0x00000010,HAL_REG_PHY);
  HAL_Reg_Write(0x00000b94,0x0000008c,HAL_REG_PHY);

  //5. US SCDMA Related Configuration;
  //TBD

  return HAL_SUCCESS;
}//HAL_Us_Hardware_Init()


/******************************************************************************
 *
 * Name: HAL_Us_Phy_Init
 *
 * Description:
 *  Does US Mac Generic PHY Initialization. (Common to Both TDMA and SCDMA)
 *  
 *
 * Arguments:
 *    NONE
 *
 *
 * Return Value:
 *   HAL_SUCCESS : Returns Always HAL_SUCCESS
 *           
 *
 *****************************************************************************/
void HAL_Us_Phy_Init()
{
    HAL_Reg_Write(0x0000e00c,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e100,0x0000007d,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e104,0x00000322,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e108,0x000000d5,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e14c,0x00000051,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e10c,0x0000001f,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e110,0x00000017,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e12c,0x1999999a,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e164,0x19999999,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e168,0x1999999a,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e154,0x00004589,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e158,0x000080d7,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e140,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e150,0x00000005,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e160,0x00000003,HAL_REG_PHY);
    HAL_Reg_Write(0x0000e16c,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x0001e00c,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e100,0x0000007d,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e104,0x00000322,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e108,0x000000d5,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e14c,0x00000051,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e10c,0x0000001f,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e110,0x00000017,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e12c,0x1999999a,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e164,0x19999999,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e168,0x1999999a,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e154,0x00004589,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e158,0x000080d7,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e140,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e150,0x00000005,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e160,0x00000003,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e16c,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x00000800,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00012800,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00004800,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00006800,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x00000828,0x0000005b,HAL_REG_PHY);
    HAL_Reg_Write(0x00012828,0x000000b6,HAL_REG_PHY);
    HAL_Reg_Write(0x00004828,0x0000005b,HAL_REG_PHY);
    HAL_Reg_Write(0x00006828,0x0000005b,HAL_REG_PHY);

    HAL_Reg_Write(0x00000b80,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00012b80,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00004b80,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00006b80,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x00000b84,0x00000001,HAL_REG_PHY);
    HAL_Reg_Write(0x00012b84,0x00000001,HAL_REG_PHY);
    HAL_Reg_Write(0x00004b84,0x00000001,HAL_REG_PHY);
    HAL_Reg_Write(0x00006b84,0x00000001,HAL_REG_PHY);

    HAL_Reg_Write(0x00000bf0,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00012bf0,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00004bf0,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00006bf0,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x00000b60,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00012b60,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00004b60,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x00006b60,0x00000000,HAL_REG_PHY);

    HAL_Reg_Write(0x0000e008,0x00000000,HAL_REG_PHY);
    HAL_Reg_Write(0x0001e008,0x00000001,HAL_REG_PHY);

}//HAL_Us_Phy_Init()

/******************************************************************************
 *
 * Name: HAL_Us_Enable
 *
 * Description:
 * Enable the Upstream transfer by setting 1 at
 * corresponding bits in REG_US_CFG for channel 0 to 3
 * USChannel Corresponds to StreamId Starts from 0..3
 *
 * Arguments:
 *    US_Channel : Enum Values from US_Id_t (Starts from 0..3)
 *
 *
 * Return Value:
 *  HAL_SUCCESS : Returns if USStream is Enabled
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/

/*-----------------------------------------------------
 * Enable the Upstream transfer by setting 1 at
 * corresponding bits in REG_US_CFG for channel 0 to 3
 * USChannel Corresponds to StreamId Starts from 0..3
 *----------------------------------------------------*/

INT32 HAL_Us_Enable(INT32 US_Channel)
{
  UINT32 Reg_Value;

#if HAL_VALIDATE
  if(US_Channel >= US_Id_Max){
    return HAL_FAILED;
  }//if
#endif

  HAL_Reg_Read(REG_US_CFG, &Reg_Value, HAL_REG_MAC_US);
  Reg_Value |= ((0x01) << US_Channel) ;
  HAL_Reg_Write(REG_US_CFG, Reg_Value, HAL_REG_MAC_US);

  return HAL_SUCCESS;
}//HAL_Us_Enable()



/******************************************************************************
 *
 * Name: HAL_Us_Disable
 *
 * Description:
 * Disable the Upstream transfer by setting 0 at
 * corresponding bits in REG_US_CFG for channel 0 to 3
 * USChannel Corresponds to StreamId Starts from 0..3
 *
 * Arguments:
 *    US_Channel : Enum Values from US_Id_t (Starts from 0..3)
 *
 *
 * Return Value:
 *  HAL_SUCCESS : Returns if USStream is Disabled
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/

INT32 HAL_Us_Disable(INT32 US_Channel)
{
  UINT32 Reg_Value;

#if HAL_VALIDATE
  if(US_Channel >= US_Id_Max){
    return HAL_FAILED;
  }//if
#endif

  HAL_Reg_Read(REG_US_CFG, &Reg_Value, HAL_REG_MAC_US);
  Reg_Value &= (~((0x01) << US_Channel));
  HAL_Reg_Write(REG_US_CFG, Reg_Value, HAL_REG_MAC_US);

  return HAL_SUCCESS;
}//HAL_Us_Disable


/******************************************************************************
 *
 * Name: HAL_Us_Config_Fifo
 *
 * Description:
 * Configuring Upstream FIFO for 
 * - FIFO debug mode
 * - MAC PHY mode.
 *
 * Arguments:
 *    NONE
 *
 *
 * Return Value:
 *  HAL_SUCCESS : Returns Always HAL_SUCCESS
 *  
 *
 *****************************************************************************/

INT32 HAL_Us_Config_Fifo(void)
{
  //1. Configure US FIFO mode .
  //Need to check any other value could be send
  UINT32 Reg_Value;

  HAL_Reg_Read(REG_US_CFG, &Reg_Value, HAL_REG_MAC_US);
#if ENABLE_MAC_PHY_MODE
  Reg_Value |=  ( (MAC_PHY_MODE << FIFO_MODE_BIT) & FIFO_MODE_MASK );
#else
  Reg_Value |=  ( (FIFO_DBG_MODE << FIFO_MODE_BIT) & FIFO_MODE_MASK );
#endif
  HAL_Reg_Write(REG_US_CFG, Reg_Value, HAL_REG_MAC_US);

  return HAL_SUCCESS;
}//HAL_Us_Config_Fifo



/******************************************************************************
 *
 * Name: HAL_Us_Tune_Frequency
 *
 * Description:
 * Tune US Channel for the Supplied Frequency
 * - Frequency Control Word Contains the 
 * - MAC PHY mode.
 *
 * Arguments:
 *    US_Channel : Enum Values from US_Id_t (Starts from 0..3)
 *    FreqCw : US Frequency Control Word
 *
 * Return Value:
 *  HAL_SUCCESS : Returns if US Frequency is Programmed
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/

INT32 HAL_Us_Tune_Frequency(INT32 US_Id, UINT32 FreqCw)
{
  INT32 Data[7];

#if HAL_VALIDATE
  if(US_Id >= US_Id_Max){
    return HAL_FAILED;
  }//if
#endif

  //NOTE:
  //During Run Time (Not in the Intialiazation) only 1.2 and 2 is required.

  //1. Genrate the FTWORD to be Sent to DDS

  // 1.1 Progmram Control Register
  Data[0x00] = (0x0091);
  Data[0x01] = (0x0102);

  // 1.2 Modify DDS Frequency
  Data[0x02] = (0x0200 | ((FreqCw & 0x000000FF) >> 0));
  Data[0x03] = (0x0300 | ((FreqCw & 0x0000FF00) >> 8));
  Data[0x04] = (0x0400 | ((FreqCw & 0x00FF0000) >> 16));
  Data[0x05] = (0x0500 | ((FreqCw & 0xFF000000) >> 24));

  // 1.3 Program Control Register
  Data[0x06] = (0x0609);

  //2. Program the US DDS for the Required Frequency
  HAL_Spi_Transmit(Spi_DDS, US_Id, (PUINT32)&Data,7);

  //3. Sleep For 1ms, Required During Initialization Only
  msleep(1);

  //4. Enable
  Data[0x00] = (0x0142);
  HAL_Spi_Transmit(Spi_DDS, US_Id, (PUINT32)&Data,1);

  return HAL_SUCCESS;
}


/******************************************************************************
 *
 * Name: HAL_Us_Program_Power
 *
 * Description:
 * Program US Channel Power, It contains 2 Component/Stream ID
 * - Analog Power
 * - Digital Power
 *
 * Arguments:
 *    US_Channel : Enum Values from US_Id_t (Starts from 0..3)
 *    Pa : Analog Power Component
 *    Pb : Digital Power Component 
 *
 * Return Value:
 *  HAL_SUCCESS : Returns If Power is Successfully Programed.
 *  HAL_FAILED : If Invalid Channel
 *
 *****************************************************************************/

INT32 HAL_Us_Program_Power(INT32 US_Id, UINT32 Pa, UINT32 Pd)
{
  UINT32 Data[2];

#if HAL_VALIDATE
  if(US_Id >= US_Id_Max){
    return HAL_FAILED;
  }//if
#endif

  // 1. Program Analog Power Component
  Data[0x00] = Pa;
  HAL_Spi_Transmit(Spi_PA, US_Id, (PUINT32)&Data,1);

  // 2. Program Digital Power Component
  HAL_Reg_Write(REG_PHY_RFDATA_GAIN, Pd, HAL_REG_PHY);

  return HAL_SUCCESS;
} //HAL_Us_Program_Power


/******************************************************************************
 *
 * Name: HAL_Us_Nyquist_Coefficient_Init
 *
 * Description:
 * Initialze the US Nyquist Coefficient
 *
 *
 *
 * Arguments:
 *    NONE
 *
 *
 *
 * Return Value:
 *  HAL_SUCCESS : Returns Always HAL_SUCCESS
 *  
 *
 *****************************************************************************/

INT32 HAL_Us_Nyquist_Coefficient_Init(VOID)
{
  UINT32 Idx;
  UINT32 Nyquist_Co_Eff_Data[20]={0x00090004,0x000a000d,
                                0x0ff50001,0x0ff30fee,
                                0x00120002,0x000b0016,
                                0x0fe70ff6,0x0ffe0fea,
                                0x001b0015,0x0fee000a,
                                0x0fef0fe0,0x00320014,
                                0x0ff20028,0x0f9f0fb2,
                                0x00560fdd,0x00a500b7,
                                0x0f090ffe,0x0ED60e6c,
                                0x035A0098,0x07F7062E,
                                };

  //1. Initilize the Nyquist Port
  HAL_Reg_Write(REG_NYQ_ADDR_PORT, 0, HAL_REG_PHY);

  //2. Incrementatlly Updated the Nyquist Coefficient Values
  for(Idx =0 ; Idx<20 ; Idx++){
    DPRINTK("Write Nyq data ");
    HAL_Reg_Write(REG_NYQ_DATA_PORT_INC,Nyquist_Co_Eff_Data[Idx],HAL_REG_PHY);
  }//for
  return HAL_SUCCESS;
}//HAL_Us_Set_Nyquist_Coefficient


/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */

EXPORT_SYMBOL(HAL_Us_Enable);
EXPORT_SYMBOL(HAL_Us_Config_Fifo);
EXPORT_SYMBOL(HAL_Us_Disable);


