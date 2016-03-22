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
*       Contains all exported DMA HAL interfaces
******************************************************************************/

#ifndef __DOCS_HAL_DMA_INTERFACE_H_
#define __DOCS_HAL_DMA_INTERFACE_H_

INT32 DOCS_DMA_REG_Access(VOID);
INT32 DOCS_DMA_MEM_Request(VOID);
VOID DOCS_DMA_REG_Free(VOID);
VOID DOCS_DMA_MEM_Free(VOID);
VOID DOCS_DMA_DESC_Build(INT32);
VOID DOCS_DMA_RST_Routine(INT32);
VOID DOCS_DMA_START_Routine(INT32);
VOID DOCS_DMA_REG_START_Routine(INT32);
VOID DOCS_DMA_WBDESC_Build(INT32);
VOID DOCS_DMA_WBSTART_Routine(INT32);
VOID DOCS_DMA_DESCR_VAR_Build(INT32);
VOID DOCS_DMA_SUSP_Routine_1(INT32);
INT32 DOCS_DMA_READ_Routine(INT32,INT32);
VOID DOCS_DMA_TERM_Routine(INT32);
VOID DOCS_DMA_SUSP_Routine_2(INT32);
VOID DOCS_DMA_DESC_Hold(INT32);
VOID DOCS_DMA_DESC_Hold_1(INT32);
VOID DOCS_DMA_DESC_Read(INT32);
VOID DOCS_DMA_SUSP_Routine_3(INT32);
VOID DOCS_DMA_SUSP_Routine_4(INT32);
INT32 DOCS_DMA_REG_READ(UINT32 ,UINT32 );
INT32 DOCS_DMA_REG_WRITE(UINT32 ,UINT32 );


#endif //__DOCS_HAL_INTERFACE_H_


