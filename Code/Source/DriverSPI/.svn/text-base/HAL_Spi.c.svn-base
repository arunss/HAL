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
* Contains Spi driver core functions
*   Has memory request/map and unmap for PHY SPI register region.
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
#include <linux/delay.h>

//Custom Header Files
#include "HAL_Spi.h"


/* --------------------------------------------------------------------------*/
/* Internal prototypes.                                                      */
/* --------------------------------------------------------------------------*/
VOID HAL_Spi_Prepare_Data(INT32 Interface, PINT32 databuff, INT32 Length);


/******************************************************************************
 *
 * Name: HAL_Spi_Wait_On_Status
 *
 * Description:
 *   Wait till SPI is BUSY.
 *   SPI BUSY => Data is either Transmitting or Recieving
 *
 * Arguments:
 *   Interface: Which Interfce PA or Tuner, Spi_Interface_t
 *   Channel: Which Phy Channel, US_Id_t
 *
 * Return Value:
 *   HAL_SUCCESS : Current SPI Operation is Success
 *   HAL_FAILED : Transmit/Recieve Failed due to time out
 *
 *****************************************************************************/

INT32 HAL_Spi_Wait_On_Status(INT32 Interface, INT32 Channel)
{

    INT32 Reg_Value,Not_Done,Timeout;
    UINT32 Addr_Spi_Rx_Ctrl;

    /*
     * 1. Start TimeOut Timer
     * 2. Read SPI RX Ctrl Register
     * 3. Wait untill BUSY Bit is cleared
     * 4. If time out occurs then exit with timout status
     */

    Addr_Spi_Rx_Ctrl = GET_CHANNEL_ADDR(PHY_REG_SPI_RX_DATA, Channel);

    //1. Start TimeOut Timer
    Not_Done = 1;
    Timeout = 500;

    //2. Read SPI RX Ctrl Register
    //3. Wait untill BUSY Bit is cleared
    while(Not_Done && Timeout) {
        HAL_Reg_Read( Addr_Spi_Rx_Ctrl, &Reg_Value, HAL_REG_PHY);
        Not_Done = Reg_Value & SPI_BUSY;
        Timeout--;
    }//while

    //4. If time out occurs then exit with timout status
    if(Timeout == 0x0) {
        return HAL_FAILED ;
    }//if

    return HAL_SUCCESS;
}//HAL_Spi_Wait_On_Status()


/******************************************************************************
 *
 * Name: HAL_Spi_Reset
 *
 * Description:
 *   Reset SPI Bus.
 *
 * Arguments:
 *   Interface: Which Interfce PA or Tuner, Spi_Interface_t
 *   Channek: Which Phy Channel, US_Id_t
 *
 * Return Value:
 *   HAL_SUCCESS : SPI Reset is Done
 *   HAL_FAILED : Reset Failed
 *
 *****************************************************************************/

//Reset Spi
INT32 HAL_Spi_Reset(INT32 Interface, INT32 Channel)
{
    UINT32 Addr_Spi_Cfg, Spi_Cfg_Value;

    /*
     * 1. Reset
     * 2. Wait for some time
     * 3. Check Reset Bit
     */

    Addr_Spi_Cfg = GET_CHANNEL_ADDR(PHY_REG_SPI_CONFIG, Channel);

    //1. Reset SPI and set clock
    HAL_Reg_Write( Addr_Spi_Cfg,(SPI_RESET|SPI_CLOCK),HAL_REG_PHY);

    //2. Wait for some time
    HAL_Spi_Sleep(3);

    //3. Check Reset Bit
    HAL_Reg_Read( Addr_Spi_Cfg, &Spi_Cfg_Value, HAL_REG_PHY);

    if(Spi_Cfg_Value & SPI_RESET){ //If bit is set not cleared then failed
        DPRINTE("Spi Reset FAILED!");
        return HAL_FAILED;
    }//if

    return HAL_SUCCESS;
}//HAL_Spi_Reset()


/******************************************************************************
 *
 * Name: HAL_Spi_Transmit
 *
 * Description:
 *   Transmit the multiple data to given Interface Device.
 *
 * Arguments:
 *   Interface: Which Interfce PA or Tuner, Spi_Interface_t
 *   Channek: Which Phy Channel, US_Id_t
 *   Buff_Addr: Pointer to the Buffer
 *   Length: Length of the buffer
 *
 * Return Value:
 *   HAL_SUCCESS : SPI Transmit is Successfull
 *   HAL_FAILED : Reset Failed, Transmit Time Out occured
 *
 *****************************************************************************/

 INT32 HAL_Spi_Transmit(INT32 Interface,
                        INT32 Channel,
                        PUINT32 Buff_Addr,
                        INT32 Length)
{

    UINT32 Addr_Spi_Cfg,Addr_Spi_Tx_Ctrl,Addr_Spi_Rx_Ctrl;
    INT32 Count;

    /*
     * 1. Get Corresponding SPI Register Address
     * 2. Reset SPI
     * 3. Preapare the Data for Transmit
     * 4. Write buffer content to the TX register of corresponding channel
     * 5. Wait till data is transmitted
     */

    Count = 0;

    //1. Check channel number and assign corresponding PHY's SPI register
    Addr_Spi_Cfg = GET_CHANNEL_ADDR(PHY_REG_SPI_CONFIG, Channel);
    Addr_Spi_Tx_Ctrl = GET_CHANNEL_ADDR(PHY_REG_SPI_TX_DATA, Channel);
    Addr_Spi_Rx_Ctrl = GET_CHANNEL_ADDR(PHY_REG_SPI_RX_DATA, Channel);


    //2. Reset SPI and set clock
    if(HAL_Spi_Reset(Interface ,Channel) != HAL_SUCCESS)
    {
        return HAL_FAILED;
    }

    //3. Prepare the data for sending to corresponding interface
    HAL_Spi_Prepare_Data(Interface, Buff_Addr, Length);

    //4. Write buffer content to the TX register of corresponding channel
    for(Count=0;Count< Length;Count++)
    {
        HAL_Reg_Write( Addr_Spi_Tx_Ctrl,
                            Buff_Addr[Count],
                            HAL_REG_PHY);

        //5. Wait till data is transmitted
        if(HAL_Spi_Wait_On_Status(Interface ,Channel) != HAL_SUCCESS)
        {
            DPRINTE("Timed Out waiting for SPI Tx to finish!");
            return HAL_FAILED ;
        }
    }
    return HAL_SUCCESS;

 }//HAL_Spi_Transmit

 /******************************************************************************
 *
 * Name: HAL_Spi_Receive
 *
 * Description:
 *   Recieve the required multiple register data from given Interface Device.
 *
 * Arguments:
 *   Interface: Which Interfce PA or Tuner, Spi_Interface_t
 *   Channek: Which Phy Channel, US_Id_t
 *   Buff_Addr: Pointer to the Buffer
 *   Length: Length of the buffer
 *
 * Return Value:
 *   HAL_SUCCESS : SPI Recieve is Successfull
 *   HAL_FAILED : Reset Failed, Recieve Time Out occured
 *
 *****************************************************************************/
 INT32 HAL_Spi_Receive(INT32 Interface,
                       INT32 Channel,
                       PUINT32 Buff_Addr,
                       UINT32 Length)
{

    UINT32 Cs_Select,Reg_Value;
    UINT32 Addr_Spi_Cfg,Addr_Spi_Tx_Ctrl,Addr_Spi_Rx_Ctrl;
    INT32 Reg_Idx;

    /*
     * 1. Get Corresponding SPI Register Address
     * 2. Select the proper interface for which data need to be sent
     * 3. Reset SPI and set clock
     * 4. configure DDS for SDIO
     * 5. Wait till DDS is Configured
     * 6. Configure which register to read
     * 7. Request Transmitter to send the register value
     * 8. Wait till Transmitter sends the register value
     * 9. Read the SPI Recieve data once register value is available
     */

    //1. Check channel number and assign corresponding PHY's SPI register
    Addr_Spi_Cfg = GET_CHANNEL_ADDR(PHY_REG_SPI_CONFIG, Channel);
    Addr_Spi_Tx_Ctrl = GET_CHANNEL_ADDR(PHY_REG_SPI_TX_DATA, Channel);
    Addr_Spi_Rx_Ctrl = GET_CHANNEL_ADDR(PHY_REG_SPI_RX_DATA, Channel);


    //2. Select the proper interface for which data need to be sent
    Cs_Select = Interface;

    //3. Reset SPI and set clock
    if(HAL_Spi_Reset(Interface ,Channel) != HAL_SUCCESS) {
        return HAL_FAILED;
    }//if

    //4. configure DDS for SDIO, Need to first write 0x91 to addr 0.
    Reg_Value = (0x00 & 0xFF) << 8;
    Reg_Value |= 0x91 & 0xFF;
    Reg_Value |= Cs_Select;
    Reg_Value |= SPI_SIZE_16BIT;

    HAL_Reg_Write( Addr_Spi_Tx_Ctrl, Reg_Value, HAL_REG_PHY);

    //5. Wait till DDS is Configured
    if(HAL_Spi_Wait_On_Status(Interface ,Channel) != HAL_SUCCESS) {
        DPRINTE("Timed Out waiting for SPI Tx to finish!");
        return HAL_FAILED;
    }//if

    for(Reg_Idx=0; Reg_Idx < Length; Reg_Idx++) {

        //6. Configure which register to read
        Reg_Value = (Reg_Idx | 0x80) << 8;
        Reg_Value |= 0x0 & 0xFF;
        Reg_Value |= Cs_Select;
        Reg_Value |= SPI_SIZE_16BIT;

        //7. Request Transmitter to send the register value
        HAL_Reg_Write( Addr_Spi_Tx_Ctrl, Reg_Value, HAL_REG_PHY);

        //8. Wait till Transmitter sends the register value
        if(HAL_Spi_Wait_On_Status(Interface ,Channel) != HAL_SUCCESS) {
            DPRINTE("Timed Out waiting for SPI Tx to finish!");
            return HAL_FAILED;
        }//if

        //9. Read the SPI Recieve data once register value is available
        HAL_Reg_Read( Addr_Spi_Rx_Ctrl, &Reg_Value, HAL_REG_PHY);

        *Buff_Addr = (Reg_Value & 0xFF);
         Buff_Addr++;
    } //end for loop - read all the register values

    return HAL_SUCCESS;
}//HAL_Spi_Receive()


 /******************************************************************************
 *
 * Name: HAL_Spi_Prepare_Data
 *
 * Description:
 *   Prepare data according to the format needed by SPI to transmit the same
 *
 * Arguments:
 *   Interface: Which Interfce PA or Tuner, Spi_Interface_t
 *   Channek: Which Phy Channel, US_Id_t
 *   Buff_Addr: Pointer to the Buffer
 *   Length: Length of the buffer
 *
 * Return Value:
 *   None
 *
 *****************************************************************************/

//Prepare data according to format spi needs
VOID HAL_Spi_Prepare_Data(INT32 Interface, PINT32 Data_Buff, INT32 Length)
{
  INT32 Index;

  for(Index =0;Index <Length;Index++) {
    Data_Buff[Index] |= (Data_Buff[Index] & 0xFF00) << 8; //RegAddr
    Data_Buff[Index] |= (Data_Buff[Index] & 0xFF) ;       //FIFOData
    Data_Buff[Index] &= 0xFF00FFFF ;
    Data_Buff[Index] |= Interface;                   //Cs_Select (Interface)
    if(Interface == Spi_DDS){
      Data_Buff[Index] |= SPI_SIZE_16BIT;   //SPI_SIZE
    }else{
      Data_Buff[Index] |= SPI_SIZE_8BIT;   //SPI_SIZE
    } //if-else
    DPRINTK("Data_Buff[%d] = %08x",Index,Data_Buff[Index]);
  }//for
}//HAL_Spi_Prepare_Data


/* ------------------------------------------------------------------------- */
/* Exported globals.                                                         */
/* ------------------------------------------------------------------------- */
EXPORT_SYMBOL(HAL_Spi_Transmit);
EXPORT_SYMBOL(HAL_Spi_Receive);

