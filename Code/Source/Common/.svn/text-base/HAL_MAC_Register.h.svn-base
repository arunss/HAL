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
*       Contains all exported HAL interfaces
*
*----------------------------------------------------------------------------*/

#ifndef _HAL_MAC_REGISTER_H_
#define _HAL_MAC_REGISTER_H_  (1)

//Memory Map

// BASE ADDRESSES
#define DS_MAC_REG_BASE_ADDRESS     (0xf0020000)
#define US_MAC_REG_BASE_ADDRESS     (0xf0028000)
#define MAC_INT_REG_BASE_ADDRESS    (0xf3ff0800)
#define PHY_REG_BASE_ADDRESS        (0xf2000000)
#define DMA_REG_BASE_ADDRESS        (0xf0030000)

//Memory size
#define PHY_REGISTER_SIZE           (0x30000)
#define DS_MAC_REGISTER_SIZE        (0x7FFF)
#define US_MAC_REGISTER_SIZE        (0x7FFF)
#define DMA_REGISTER_SIZE           (0x3FF)
#define INT_REGISTER_SIZE           (0x3FF)

// REGISTER ADDRESSES


//DS Mini-Mac Register Defination

#define REG_DS_IRQ_EN               (DS_MAC_REG_BASE_ADDRESS + 0x10)
#define REG_DS_IRQ_STS              (DS_MAC_REG_BASE_ADDRESS + 0x14)
#define REG_TS_MAP                  (DS_MAC_REG_BASE_ADDRESS + 0x18)
#define REG_TS_FORMAT               (DS_MAC_REG_BASE_ADDRESS + 0x20)
#define REGTS_STATUS                (DS_MAC_REG_BASE_ADDRESS + 0x24)
#define REG_DS_DMA_ASSOC            (DS_MAC_REG_BASE_ADDRESS + 0x40)
#define REG_CAP_CTRL                (DS_MAC_REG_BASE_ADDRESS + 0x80)
#define REG_CAP_READ                (DS_MAC_REG_BASE_ADDRESS + 0x84)
#define REG_DS_MSG_STS              (DS_MAC_REG_BASE_ADDRESS + 0x100)
#define REG_DCSD_DCS_TEST_0         (DS_MAC_REG_BASE_ADDRESS + 0x180)
#define REG_TEST_FIFO_0             (DS_MAC_REG_BASE_ADDRESS + 0x4000)
#define REG_TEST_FIFO_1             (DS_MAC_REG_BASE_ADDRESS + 0x2100)
#define REG_TEST_FIFO_2             (DS_MAC_REG_BASE_ADDRESS + 0x2200)
#define REG_TEST_FIFO_3             (DS_MAC_REG_BASE_ADDRESS + 0x2300)
#define REG_DCSD_TS_TEST            (DS_MAC_REG_BASE_ADDRESS + 0x190)
#define REG_DSMACIPADDRFLTREHN0     (DS_MAC_REG_BASE_ADDRESS + 0x200)
#define REG_DSMACIPADDRFLTREGN1     (DS_MAC_REG_BASE_ADDRESS + 0x204)
#define REG_MAC_DSID                (DS_MAC_REG_BASE_ADDRESS + 0x210)
#define REG_CURR_DSID               (DS_MAC_REG_BASE_ADDRESS + 0x214)
#define REG_BPI_CTRL                (DS_MAC_REG_BASE_ADDRESS + 0x300)

//Not actual value need to be updated -Mar0909
#define REG_DS_FIFO_CTRL            (DS_MAC_REG_BASE_ADDRESS + 0x4000)
#define REG_DS_FIFO_STS             (DS_MAC_REG_BASE_ADDRESS + 0x4000)


//US Mini-Mac Register Defination

#define REG_US_CFG                  (US_MAC_REG_BASE_ADDRESS + 0x0)
#define REG_US_TEK000               (US_MAC_REG_BASE_ADDRESS + 0x20)
#define REG_US_TEK001               (US_MAC_REG_BASE_ADDRESS + 0x24)
#define REG_US_TEK002               (US_MAC_REG_BASE_ADDRESS + 0x28)
#define REG_US_TEK003               (US_MAC_REG_BASE_ADDRESS + 0x2c)
#define REG_US_TEK100               (US_MAC_REG_BASE_ADDRESS + 0x40)
#define REG_US_TEK101               (US_MAC_REG_BASE_ADDRESS + 0x44)
#define REG_US_TEK102               (US_MAC_REG_BASE_ADDRESS + 0x48)
#define REG_US_TEK103               (US_MAC_REG_BASE_ADDRESS + 0x4c)
#define REG_US_IV000                (US_MAC_REG_BASE_ADDRESS + 0x60)
#define REG_US_IV001                (US_MAC_REG_BASE_ADDRESS + 0x64)
#define REG_US_IV002                (US_MAC_REG_BASE_ADDRESS + 0x68)
#define REG_US_IV003                (US_MAC_REG_BASE_ADDRESS + 0x6c)
#define REG_US_IV100                (US_MAC_REG_BASE_ADDRESS + 0x80)
#define REG_US_IV101                (US_MAC_REG_BASE_ADDRESS + 0x84)
#define REG_US_IV102                (US_MAC_REG_BASE_ADDRESS + 0x88)
#define REG_US_IV103                (US_MAC_REG_BASE_ADDRESS + 0x8c)
#define REG_US_ENC_LOC              (US_MAC_REG_BASE_ADDRESS + 0x90)
#define REG_US_INFO                 (US_MAC_REG_BASE_ADDRESS + 0x100)

#define REG_US_PKT_FIFO_0           (US_MAC_REG_BASE_ADDRESS + 0x120)
#define REG_US_PKT_FIFO_1           (US_MAC_REG_BASE_ADDRESS + 0x124)
#define REG_US_PKT_FIFO_2           (US_MAC_REG_BASE_ADDRESS + 0x128)
#define REG_US_PKT_FIFO_3           (US_MAC_REG_BASE_ADDRESS + 0x12c)


//DOCSIS Mini-Mac Interrupt

#define REG_GLBL_INT_ENABLE         (MAC_INT_REG_BASE_ADDRESS + 0x04)
#define REG_GLBL_INT_STATUS         (MAC_INT_REG_BASE_ADDRESS + 0x00)


//DOCSIS Phy Registers
#define REG_PHY_GRP2AREG44          (0x0110)
#define REG_PHY_LOCKSTATUS          (0x0680)
#define REG_PHY_GRP1QAMSTVITFDBK    (0x03F0)
//32 Bit Register
#define REG_PHY_RFDATA_GAIN         (0x08CF)

//32 Bit Register
#define REG_NYQ_ADDR_PORT           (0x894)
#define REG_NYQ_DATA_PORT_INC       (0x898)

//External Clock From the CMTS
#define REG_PHY_TB_EXTCLK_SEL       (0xE160)
#define REG_PHY_TB_FRQ_OFF_0        (0xE164)
#define REG_PHY_TB_FRQ_OFF_1        (0xE168)


// REGISTER BIT MASK


//PHY Register Bit Defination
#define DS_LOCK_RE_ACQUIRE          (0x01)


//Interrrupt Register Bit/Mask Defination
#define MASK_INT_DMA                (0x0007FFFF)
#define MASK_INT_DS_DMA             (0x00003000)
#define MASK_INT_DS_DMA12           (0x00001000)
#define MASK_INT_DS_DMA13           (0x00002000)
#define MASK_INT_DS_FIFO            (0x00100000)

#define MASK_INT_US_DMA             (0x00000F00)

//DMA Register Bit/Mask Defination
#define DMA_REGISTER_BLOCK_SIZE           (0x20)
#define DMA_NEXT_DESC_REGISTER_OFFSET     (0x0)
#define DMA_START_ADDR_REGISTER_OFFSET    (0x4)
#define DMA_PARAMETERS_REGISTER_OFFSET    (0x8)
#define DMA_MODE_CTRL_REGISTER_OFFSET     (0xc)
#define DMA_CURR_DESCR_REGISTER_OFFSET    (0x10)


#define DMA_CONTEXT_DONE_BIT              (0x80000000)
#define DMA_CONTEXT_REGISTER_MODE_BIT     (0x40000000)
#define DMA_DESCRIPTOR_MODE_BIT           (0x20000000)
#define DMA_HOLD_BIT                      (0x10000000)
#define DMA_GENERATE_EOP_BIT              (0x08000000)
#define DMA_LAST_DESCRIPTOR_BIT           (0x02000000)
#define DMA_DESCR_WBACK_BIT               (0x01000000)
#define DMA_IRQ_EN_BIT                    (0x00800000)
#define DMA_IRQ_ON_PROT_ERR_EN_BIT        (0x00200000)
#define DMA_IRQ_ON_AXI_ERR_EN_BIT         (0x00100000)
#define DMA_IRQ_BIT                       (0x00080000)
#define DMA_PROT_ERR_BIT                  (0x00020000)
#define DMA_AXI_ERR_BIT                   (0x00010000)
#define DMA_WORDS_TRANSFERRED_MASK        (0x0000FFFF)
#define DMA_BLACKOUT_MASK                 (0xFF000000)
#define DMA_BLACKOUT_SHIFT                (24)
#define DMA_BURST_LENGTH_MASK             (0x000F0000)
#define DMA_BURST_LENGTH_SHIFT            (16)
#define DMA_BLOCK_LENGTH_MASK             (0x0000FFFF)

#define DMA_BURST_8BYTE                 (0x00 << 16)
#define DMA_BURST_16BYTE                (0x01 << 16)
#define DMA_BURST_32BYTE                (0x03 << 16)
#define DMA_BURST_64BYTE                (0x07 << 16)
#define DMA_BURST_128BYTE               (15 << 16)


#endif //__DOCS_MAC_REGISTER_H_


