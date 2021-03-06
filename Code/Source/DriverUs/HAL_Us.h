#ifndef _HAL_US_H_
#define _HAL_US_H_  (1)

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
*       This header file defines data types and prototype for DriverUs
*
*----------------------------------------------------------------------------*/

#include "HAL_Interface.h"
#include "HAL_Us_Defines.h"


void HAL_Us_Phy_Init(void);

//Nyquist co-efficient init
INT32 HAL_Us_Nyquist_Coefficient_Init(VOID);


#endif /*_HAL_US_H_*/
