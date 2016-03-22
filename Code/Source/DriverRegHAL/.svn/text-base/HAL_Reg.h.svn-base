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
*       This header file defines data types and prototype for DMA
*
*----------------------------------------------------------------------------*/
#ifndef DOCS_HAL_REG_H_
#define DOCS_HAL_REG_H_

#include "HAL_Interface.h"
#include "HAL_Reg_Defines.h"

INT32 DOCS_HAL_REG_OSM_Setup (void);
INT32 DOCS_HAL_REG_OSM_Release (void);


/*
 * ARM11 PMU support
 */
static inline void arm11_write_pmnc(u32 val)
{
   /* upper 4bits and 7, 11 are write-as-0 */
   val &= 0x0ffff77f;
   asm volatile("mcr p15, 0, %0, c15, c12, 0" : : "r" (val));
}

static inline u32 arm11_read_pmnc(void)
{
  u32 val;
  asm volatile("mrc p15, 0, %0, c15, c12, 0" : "=r" (val));
  return val;
}

/* cnt ==> pass zero only */
static inline u32 arm11_get_counter(unsigned int cnt)
{
  u32 val;
  asm volatile("mrc p15, 0, %0, c15, c12, 1" : "=r" (val));
  return val;
}

#endif /*DOCS_HAL_REG_H_*/
