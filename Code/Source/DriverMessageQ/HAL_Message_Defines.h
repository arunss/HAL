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
 *       This header file contains macros "define" used in DOCSIS HAL layer
 *
 *----------------------------------------------------------------------------*/

#ifndef __DOCS_BSP_DEFINES_H_
#define __DOCS_BSP_DEFINES_H_

#define DRV_INFO                    (HAL_ENABLE)
#define DRIVER_MAJOR                (G_drv_major)
#define DRIVER_NAME                 "MsgDriver"
#define DRIVER_VERSION              "V0.01"

#define DEBUG                       (HAL_DISABLE)
#define DEBUG_ERR                   (HAL_ENABLE)

//#undef DEBUG

#define PRINTK(fmt, args...) \
printk(KERN_INFO "%s(): " fmt "\n",__FUNCTION__, ##args)

/* This is general/Normal debug messages to print variable values or
   othe info*/
#if DEBUG
#define DPRINTK(fmt, args...) \
printk(KERN_INFO "%s(): " fmt "\n",__FUNCTION__, ##args)
#else
#define DPRINTK(fmt, args...)
#endif

/* This is Error debug messages to print variable values or othe info*/
#if DEBUG_ERR
#define DPRINTE(fmt, args...) \
printk(KERN_ERR "%s: %s %s(): " fmt "\n", DRIVER_NAME, \
    DRIVER_VERSION, __FUNCTION__, ##args)
#else
#define DPRINTE(fmt, args...)
#endif

/* This is general/Normal debug messages to print variable
   values or othe info*/
#if DRV_INFO
#define PRINT_DRV_INFO() \
printk(KERN_INFO "Module Name => %s : Module Version => %s \n",\
    DRIVER_NAME, DRIVER_VERSION)
#else
#define PRINT_DRV_INFO()
#endif

#endif //__DOC_HAL_TYPES_H_


