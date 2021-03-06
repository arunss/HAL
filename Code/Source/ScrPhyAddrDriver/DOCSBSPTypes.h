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
*       This header file defines basic data types used in DOCSIS HAL layer
******************************************************************************/

#ifndef __DOCS_HAL_TYPES_H_
#define __DOCS_HAL_TYPES_H_

/* ------------------------------------------------------------------------- */
/* Type definitions.                                                          */
/* ------------------------------------------------------------------------- */

// Basic Data types
typedef void                VOID;
typedef char                INT8;
typedef unsigned char       UINT8;
typedef short               INT16;
typedef unsigned short      UINT16;
typedef int                 INT32;
typedef unsigned int        UINT32;
typedef long long           INT64;
typedef unsigned long long  UINT64;
typedef long                SLONG;
typedef unsigned long       ULONG;

// Basic Pointer types
typedef VOID*               PVOID;
typedef INT8*               PINT8;
typedef UINT8*              PUINT8;
typedef INT16*              PINT16;
typedef UINT16*             PUINT16;
typedef INT32*              PINT32;
typedef UINT32*             PUINT32;
typedef INT64*              PINT64;
typedef UINT64*             PUINT64;
typedef SLONG*              PLONG;
typedef ULONG*              PULONG;

typedef unsigned char       BOOL;


#define HAL_TRUE            (0x01)
#define HAL_FALSE           (0x00)
#define HAL_ENABLE          (0x01)
#define HAL_DISABLE         (0x00)
#define HAL_SUCCESS         (0x00)
#define HAL_FAILED          (-1)

#endif //__DOC_HAL_TYPES_H_


