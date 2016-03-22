#ifndef _HAL_SPI_H_
#define _HAL_SPI_H_   (1)

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
*       This header file defines data types and prototype for SPI
*
*----------------------------------------------------------------------------*/

//Custom Header Files
#include "HAL_Interface.h"
#include "HAL_Spi_Defines.h"



#define SPI_BASE                        0xe800
#define SPI_CONFIG                      0x00
#define SPI_RX_DATA                     0x04
#define SPI_TX_DATA                     0x18

#define PHY_REG_SPI_CONFIG              (SPI_BASE + SPI_CONFIG)
#define PHY_REG_SPI_RX_DATA             (SPI_BASE + SPI_RX_DATA)
#define PHY_REG_SPI_TX_DATA             (SPI_BASE + SPI_TX_DATA)


#define SPI_CS0_SELECT                  0x40000000
#define SPI_CS1_SELECT                  0x80000000

#define SPI_SIZE_16BIT                  0x20000000
#define SPI_SIZE_8BIT                   0x10000000
#define SPI_BUSY                        0x20000000

#define SPI_RESET                       0x80
#define SPI_CLOCK                       0x01

#define HAL_Spi_Sleep(ms)               msleep(ms)



#endif /*_HAL_SPI_H_*/
