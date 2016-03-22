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

/* ============================================================================
 * Contains the core functions definitions of I2C driver.
 * ============================================================================
 */

/* ------------------------------------------------------------------------ */
/* Headers.                                                                  */
/* ------------------------------------------------------------------------- */

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
#include "HAL_I2c.h"

/* ------------------------------------------------------------------------- */
/* Internal globals.                                                         */
/* ------------------------------------------------------------------------- */
static UINT32 G_Phy_Reg_Base= (UINT32)NULL;

/* --------------------------------------------------------------------------*/
/* Internal prototypes.                                                      */
/* --------------------------------------------------------------------------*/
VOID HAL_I2c_Reg_Write(INT32 Interface,
                            INT32 Channel,
                            INT32 Offset,
                            INT32 Value);

VOID HAL_I2c_Reg_Read(INT32 Interface,
                            INT32 Channel,
                            INT32 Offset,
                            PINT32 Value);

VOID HAL_I2c_Reset(INT32 Interface, INT32 Channel);

VOID HAL_I2c_Wait_On_Status(INT32 Interface, INT32 Channel);

VOID HAL_I2c_Send_Data(INT32 Channel, INT32 Data, INT32 Type);

/* ------------------------------------------------------------------------- */
/* Functions.                                                                */
/* ------------------------------------------------------------------------- */

/******************************************************************************
 *
 * Name: HAL_I2c_Reg_Write
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Interface  - IF_MASTER or IF_SLAVE (0 or 1)
 *   Channel     -  Channel number
 *   Offset     - offset to calculate address
 *   Value       - to be written
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


VOID HAL_I2c_Reg_Write(INT32 Interface,
                            INT32 Channel,
                            INT32 Offset,
                            INT32 Value)
{
    UINT32 Phy_Addr, I2C_Chan_Addr;


    Phy_Addr = I2C_Get_Addr(Interface, Offset);

    I2C_Chan_Addr = GET_CHANNEL_ADDR(Phy_Addr, Channel);

    HAL_Reg_Write(I2C_Chan_Addr, Value, HAL_REG_PHY);

    DPRINTK("I2C_Chan_Addr 0x%08x, Value 0x%04x, 0x%08x",\
             I2C_Chan_Addr, Value,(G_Phy_Reg_Base + I2C_Chan_Addr));

    DPRINTK("I2C_Chan_Addr %08x, Value %08x", I2C_Chan_Addr, Value);

    return;
}

/******************************************************************************
 *
 * Name: HAL_I2c_Reg_Read
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Interface - IF_MASTER or IF_SLAVE (0 or 1)
 *   Channel     -  Channel number
 *   Offset     - offset to calculate address
 *   Value        - address to where read value gets stored
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

VOID HAL_I2c_Reg_Read(INT32 Interface,
                            INT32 Channel,
                            INT32 Offset,
                            PINT32 Value)
{
    UINT32 Phy_Addr, I2C_Chan_Addr;


    Phy_Addr = I2C_Get_Addr(Interface, Offset);
    I2C_Chan_Addr = GET_CHANNEL_ADDR(Phy_Addr, Channel);

    HAL_Reg_Read(I2C_Chan_Addr, Value, HAL_REG_PHY);

}


/******************************************************************************
 *
 * Name: HAL_I2c_Setup
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   VOID
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

INT32 HAL_I2c_Setup (VOID)
{
    /*
     * 1. Init all I2C Channel in Master Interface
     * 2. Init all I2C Channel in Slave Interface
     */

    INT32 channel;

    G_Phy_Reg_Base = HAL_Reg_Get_Base(HAL_REG_PHY);
    DPRINTK("G_Phy_Reg_Base 0x%08x, ",G_Phy_Reg_Base);


    //1. Init all I2C Channel in Master Interface
    for(channel=0 ; channel < DS_GET_MAX_CHANNEL(); channel++)
    {
        HAL_I2c_Init(IF_MASTER ,channel);
    }

    //2. Init all I2C Channel in Slave Interface
    //TBD



    return HAL_SUCCESS;

}


/******************************************************************************
 *
 * Name: HAL_I2c_Init
 *
 * Description:
 *
 *
 *
 * Arguments:
 *  Interface - IF_MASTER or IF_SLAVE (0 or 1)
 *   Channel     -  Channel number
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


VOID HAL_I2c_Init(INT32 Interface, INT32 Channel)
{
    INT32 Icr_Value = 0x00;

    HAL_I2c_Reset(Interface, Channel);

    Icr_Value = (I2C_ENABLE | I2C_MASTER | I2C_INT_ALL);

    HAL_I2c_Reg_Write(Interface, Channel, I2C_ICR, Icr_Value);
}


/******************************************************************************
 *
 * Name: HAL_I2c_Reset
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Interface - IF_MASTER or IF_SLAVE (0 or 1)
 *   Channel     -  Channel number
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


VOID HAL_I2c_Reset(INT32 Interface, INT32 Channel)
{
    /*
     * 1. Reset
     * 2. Wait for some time
     * 3. Clear reset
     */
    //INT32 Idx,Icr_Value;

    //1. Reset
    HAL_I2c_Reg_Write(Interface, Channel, I2C_ICR, I2C_RESET);

    //2. Wait for some time
    HAL_I2c_Sleep(3);

    //3. Clear Reset
    HAL_I2c_Reg_Write(Interface, Channel, I2C_ICR, I2C_NULL);

    return;
}

/******************************************************************************
 *
 * Name: HAL_I2c_Wait_On_Status
 *
 * Description:
 *
 *
 *
 * Arguments:
 *  Interface - IF_MASTER or IF_SLAVE (0 or 1)
 *   Channel     -  Channel number
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

VOID HAL_I2c_Wait_On_Status(INT32 Interface, INT32 Channel)
{
    volatile INT32 Icr_Value;
    volatile INT32 Isr_Value;
    INT32 retry = 1000;
    //DPRINTK("Entering .....");
    //Poll till Byte Get transmitted
    do
    {
        HAL_I2c_Sleep(3);

        HAL_I2c_Reg_Read(Interface, Channel, I2C_ISR, (PINT32)&Isr_Value);
        if(Isr_Value & I2C_TX_OK)
            break;

        HAL_I2c_Reg_Read(Interface, Channel, I2C_ICR, (PINT32)&Icr_Value);
        if(!(Icr_Value & I2C_TRANSFER))
            break;

    } while(retry--);

    if(retry<=0)
    {
        DPRINTE("I2C Wait Failed %08x",Isr_Value);
        DPRINTE("I2C Wait Failed %08x",Icr_Value);
        //HAL_I2c_Reg_Write(Interface, Channel, I2C_ISR, Isr_Value);
    }

    //Clear Status
    Isr_Value &= (0x7F0);
    HAL_I2c_Reg_Write(Interface, Channel, I2C_ISR, Isr_Value);
    //HAL_I2c_Reg_Write(Interface, Channel, I2C_ISR, 0x40);
    Icr_Value = (I2C_ENABLE | I2C_MASTER | I2C_INT_ALL);
    HAL_I2c_Reg_Write(Interface, Channel, I2C_ICR, Icr_Value);

}

/******************************************************************************
 *
 * Name: HAL_I2c_Send_Data
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Channel     -  Channel number
 *   Data        -  Data to be transferd
 *   Type        - I2C operational bit combinations
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

VOID HAL_I2c_Send_Data(INT32 Channel, INT32 Data, INT32 Type)
{
    INT32 Icr_Value;


    //Program IDBR with Device Address
    HAL_I2c_Reg_Write(IF_MASTER, Channel, I2C_IDBR, Data);

    //Transmit DevAddr and Indicate Data Transfer Start to the Device
    Icr_Value = (I2C_ENABLE|I2C_MASTER|I2C_INT_ALL|I2C_TRANSFER|Type);

    HAL_I2c_Reg_Write(IF_MASTER, Channel, I2C_ICR, Icr_Value);

    //Block Call: Wait till data get transmitted
    HAL_I2c_Wait_On_Status(IF_MASTER, Channel);

    DPRINTK("I2C_IDBR %08x", Data);
    DPRINTK("I2C_ICR %08x", Icr_Value);


}

/******************************************************************************
 *
 * Name: HAL_I2c_Write
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Interface     - Master or Slave 0 or 1
 *   Dev_Id        - like Ds tuner0 to tuner3 ( 0 to 3)
 *   Data            - Address where data stored ( ma be more than 1)
 *   Length         - length of data to be transfered from given location
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

INT32 HAL_I2c_Write(INT32 Interface, INT32 Dev_Id, PINT32 Data, INT32 Length)
{
    INT32 Cnt;

    //Validate
    if(Length < 2)
        return HAL_FAILED;

    for(Cnt=0; Cnt<Length; Cnt++)
        DPRINTE("Data[%d]   %08x", Cnt,Data[Cnt]);

    //1. Slect the Device and Start Data Transfer
    HAL_I2c_Send_Data(Dev_Id, Data[0], I2C_START);

    //Send All Data (Except Last Byte)
    for(Cnt=0x01; Cnt<(Length-1); Cnt++)
    {
        HAL_I2c_Sleep(3);
        HAL_I2c_Send_Data(Dev_Id, Data[Cnt], I2C_NULL);
    }

    HAL_I2c_Sleep(3);

    //Send Last Byte and Stop Data Trasnfer
    HAL_I2c_Send_Data(Dev_Id, Data[Cnt], I2C_WR_STOP);

    return HAL_SUCCESS;
}



/******************************************************************************
 *
 * Name: HAL_I2c_Read
 *
 * Description:
 *
 *
 *
 * Arguments:
 *   Interface     - Master or Slave 0 or 1
 *   Dev_Id        - like Ds tuner0 to tuner3 ( 0 to 3)
 *   Data            - Address where data stored ( ma be more than 1)
 *   Length         - length of data to be transfered from given location
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/

INT32 HAL_I2c_Read(INT32 Interface, INT32 Dev_Id, PINT32 Data, INT32 Length)
{
    INT32 Cnt;

    //Validate
    if(Length < 2)
        return HAL_FAILED;

    //1. Slect the Device and Start Data Transfer
    HAL_I2c_Send_Data(Dev_Id, Data[0], I2C_START);

    //Recieve All Data (Except Last Byte)
    for(Cnt=0x01; Cnt<(Length-1); Cnt++)
    {
        HAL_I2c_Sleep(3);

        //Indicate Data Recieve Start Ready to device
        HAL_I2c_Send_Data(Dev_Id, Data[Cnt], I2C_NULL);

        HAL_I2c_Sleep(3);

        //Read the Data
        HAL_I2c_Reg_Read(Interface, Dev_Id, I2C_ICR, &Data[Cnt]);
    }
    HAL_I2c_Sleep(3);

    //Recieve Last Byte and Stop Data Recieve
    HAL_I2c_Send_Data(Dev_Id, Data[Cnt], I2C_RD_STOP);

    HAL_I2c_Sleep(3);
    //Read the Data
    HAL_I2c_Reg_Read(Interface, Dev_Id, I2C_ICR, &Data[Cnt]);

    return HAL_SUCCESS;
}


/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */

EXPORT_SYMBOL(HAL_I2c_Init);
EXPORT_SYMBOL(HAL_I2c_Write);
EXPORT_SYMBOL(HAL_I2c_Read);




