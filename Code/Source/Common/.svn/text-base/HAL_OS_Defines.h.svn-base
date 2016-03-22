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
*       This header file defines macros for memory related and time related
* system calls used in DOCSIS HAL layer
*---------------------------------------------------------------------------*/

#ifndef _HAL_OS_DEFINES_H_
#define _HAL_OS_DEFINES_H_ (1)

#define timercmp(tvp, uvp, cmp)\
            ((tvp)->tv_sec cmp (uvp)->tv_sec ||\
            (tvp)->tv_sec == (uvp)->tv_sec &&\
            (tvp)->tv_usec cmp (uvp)->tv_usec)

#define OSM_DIFF_TIME(end, start)\
            ((end).tv_usec - (start).tv_usec)


#define OSM_MEM_SET(dst,val,length)     memset( (dst), (val), (length) )
#define OSM_MEM_COPY(dst,src,size)      memcpy( (dst), (src), (size) )
#define OSM_GET_TIME(tvalue)                    gettimeofday( (tvalue),NULL)

#define OSM_MEM_ALLOC           NULL
#define OSM_MEM_RESERVE         request_mem_region
#define OSM_MEM_REMAP               ioremap
#define OSM_MEM_UNMAP               iounmap
#define OSM_MEM_RELEASE         release_mem_region

#endif
