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


#ifndef _DMA_Test3_H
#define _DMA_Test3_H (1)

#include "DOCSTestDefines.h"
#include "HAL_Interface.h"



INT32 DOCS_DMA_Test3(INT32 ctxt, INT32 mode, INT32 times);

typedef struct Dma_Buff_t{
    UINT32 Addr;
    UINT32 Len;
}Dma_Buff_t,* PDma_Buff_t;
/* ---------------------------------------------------------------------- */
/* extern.                                                                */
/* ---------------------------------------------------------------------- */

extern INT32 HAL_Reg_Read(UINT32 Offset,PUINT32 Value,UINT32 Type);
extern INT32 HAL_Reg_Write(UINT32 Offset,UINT32 Value,UINT32 Type);
extern INT32 HAL_Dma_Send_Buffer(INT32 Context,PDma_Buff_t Buff_Det,UINT32 Len);

#endif //_DMA_Test3_H
