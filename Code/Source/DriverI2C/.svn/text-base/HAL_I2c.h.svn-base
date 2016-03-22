#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_     (1)

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
*       This header file defines data types and prototype for I2C Driver
*
*----------------------------------------------------------------------------*/
//Include Custom Header Files

#include "HAL_Interface.h"
#include "HAL_I2c_Defines.h"


//Frequently used in the progamming
#define I2C_BASE             (0xe900)

//Available I2C Interfaces
#define I2C_MASTER_BASE     (I2C_BASE + IF_MASTER)
#define I2C_SLAVE_BASE      (I2C_BASE + IF_SLAVE)

//Offset from the I2C Interface Base Address
#define I2C_ICR             (0x00)
#define I2C_ISR             (0x04)
#define I2C_ISAR            (0x08)
#define I2C_IDBR            (0x0C)
#define I2C_IBMR            (0x14)
#define I2C_IWCR            (0x18)
#define I2C_ISMSCR          (0x1C)
#define I2C_ISMLCR          (0x20)
#define I2C_IFMSCR          (0x24)
#define I2C_IFMLCR          (0x28)

//I2C Operational Bit Combinations

#define I2C_NULL            (0x0000)

#define I2C_RESET           (0x01<<14)

#define I2C_INT_ALL         (0x2F00)

#define I2C_ENABLE          (0x0040)
#define I2C_MASTER          (0x0020)
#define I2C_ABORT           (0x0010)

#define I2C_START           (0x0001)
#define I2C_TRANSFER        (0x0008)

#define I2C_WR_STOP         (0x0002)
#define I2C_RD_STOP         (0x0006)


//I2C Status Bit Combinations
#define I2C_TX_OK           (0x0040)
#define I2C_RX_OK           (0x0080)


#define I2C_Get_Addr(Interface, Reg_Offset) (I2C_BASE + Interface + Reg_Offset)

#define HAL_I2c_Sleep(ms)               msleep(ms)





INT32 HAL_I2c_Setup (VOID);




#endif /*_HAL_I2C_H_*/
