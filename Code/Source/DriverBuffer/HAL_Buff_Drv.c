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
*  Brief Description of the overall implementation strategy used in the module.
*  Platform specific implementation details and optimization
*  strategies (if any)
*  should be described.
*
* HISTORY (Version, Date, Author, Note)
*------------------------------------------------------------------------------
*   Ver.0.01   13/5/08  AShah     Intial Draft
*
******************************************************************************/

/* ------------------------------------------------------------------------ */
/* Headers.                                                                 */
/* ------------------------------------------------------------------------ */

//Standard Header Files
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>

//Custom Header Files
#include "HAL_Buffer.h"


/* ----------------------------------------------------------------------- */
/* Macros.                                                                 */
/* ----------------------------------------------------------------------- */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("Packet Driver");

/* ----------------------------------------------------------------------- */
/* Global Variable.                                                        */
/* ----------------------------------------------------------------------- */

PUINT32 buffer_base_addr;
EXPORT_SYMBOL(buffer_base_addr);

PUINT32 buffer_last_addr;
EXPORT_SYMBOL(buffer_last_addr);

PUINT32 buffer_pool_base_addr;
EXPORT_SYMBOL(buffer_pool_base_addr);
extern PktPool_t PacketPool;
extern UINT32 PacketId;



/* ---------------------------------------------------------------------- */
/* Internal prototypes.                                                   */
/* ---------------------------------------------------------------------- */
static INT32 DOCS_PKTDRV_OSM_Open (struct inode *inode, struct file *file);
static INT32 DOCS_PKTDRV_OSM_Release (struct inode *inode, struct file *file);
static ssize_t DOCS_PKTDRV_OSM_Read (struct file *file, \
                         char *buf,size_t count, loff_t *ppos);
static ssize_t DOCS_PKTDRV_OSM_Write (struct file *file, \
                       const char *buf,size_t count, loff_t *ppos);
static INT32 DOCS_PKTDRV_OSM_Ioctl(struct inode *inode, \
struct file *file,UINT32 cmd, ULONG arg);
static int DOCS_PKTDRV_OSM_Mmap(struct file * filp,\
                                 struct vm_area_struct *vma);


static INT32 __init ModuleInit (VOID);
static VOID __exit ModuleExit (VOID);


// open function - called when /dev/PacketPool is opened in userspace
static INT32 DOCS_PKTDRV_OSM_Open (struct inode *inode, struct file *file)
{
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return retval;
}

// close function - called when /dev/PacketPool is closed in userspace
static INT32 DOCS_PKTDRV_OSM_Release (struct inode *inode, struct file *file)
{
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return retval;
}



// read function called when from /dev/PacketPool is read
static ssize_t DOCS_PKTDRV_OSM_Read (struct file *file, char *buf, \
        size_t count, loff_t *ppos)
{
    INT32 retval = HAL_SUCCESS;

    //DPRINTK("Entering.....");

    //1. Get which Packet
    //2. copy packet contents to user space "copy_to_user"
    //3. Return number of bytes copied
    //4. If driver is configured for auto clear mode,
    //   a. Release the packet to the pool.

    return retval;
}

// write function called when to /dev/PacketPool is written
static ssize_t DOCS_PKTDRV_OSM_Write (struct file *file, const char *buf, \
                                        size_t count, loff_t *ppos)
{
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering.....");

    //1. Get free Packet
    //2. copy packet contents from user space "copy_from_user"
    //3. Return number of bytes copied


    return retval;
}

// ioctl - I/O control
static INT32 DOCS_PKTDRV_OSM_Ioctl(struct inode *inode, struct file *file,
                                                    UINT32 cmd, ULONG arg)
{
    INT32 retval = HAL_SUCCESS;
    INT32 Data;

    DPRINTK("Entering..... Command: %d", cmd);

    switch (cmd)
    {
        case PKT_CTRL_RELEASE:
        case PKT_CTRL_RESERVE:
            if (copy_from_user(&Data, (int *)arg, sizeof(INT32)))
                return -EFAULT;
            break;
    }

    switch (cmd)
    {
        case PKT_STAT_GET_BASE:
            Data = (UINT32)buffer_base_addr;
            break;

        case PKT_STAT_GET_PACKET:
            break;

        case PKT_CTRL_RELEASE:
            retval = DOCS_POOL_OSM_FreePkt(Data);
            break;

        case PKT_CTRL_RESERVE:
            break;

        case PKT_CTRL_INIT:
            //DOCS_POOL_OSM_Init();        
            break;

        case PKT_STAT_GET_DBGBASE:
            //Data = GET_DBG_BASE();
            break;

        default:
            retval = -EINVAL;
            break;
    }


    switch (cmd)
    {
        case PKT_STAT_GET_BASE:
        case PKT_STAT_GET_PACKET:
        case PKT_CTRL_SET_PID:
        case PKT_STAT_GET_DBGBASE:
            if (copy_to_user((int *)arg, &Data, sizeof(INT32)))
                return -EFAULT;
            break;
    }
      return retval;
}

// mmap function called when  /dev/PacketPool is memory mapped
static int DOCS_PKTDRV_OSM_Mmap(struct file * filp,\
                              struct vm_area_struct * vma) {
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return retval;
}

// define which file operations are supported
struct file_operations packet_drv_fops = {
    .owner      =   THIS_MODULE,
    .llseek     =   NULL, /* AShah: TBD  */
    .read       =   DOCS_PKTDRV_OSM_Read,
    .write      =   DOCS_PKTDRV_OSM_Write,
    .ioctl      =   DOCS_PKTDRV_OSM_Ioctl,
    .mmap       =   DOCS_PKTDRV_OSM_Mmap,
    .open       =   DOCS_PKTDRV_OSM_Open,
    .release    =   DOCS_PKTDRV_OSM_Release,
};




// initialize module
static INT32 __init ModuleInit (VOID)
{
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering .....");

    /*
     * 1. Regsister as Char Driver
     * 2. Setup a Packet Pool
     * 3. Reserve Pool Memory ensure not to Swap
     */

    retval = register_chrdev (DRIVER_MAJOR, DRIVER_NAME, &packet_drv_fops);
    if (retval != 0) return - EIO;

    PRINT_DRV_INFO();
    DPRINTK("create the device entry in /dev ");
    DPRINTK("mknod -m 666 /dev/%s c %d 0", DRIVER_NAME, DRIVER_MAJOR);


    DOCS_POOL_OSM_Setup();

    return HAL_SUCCESS;
}


// close and cleanup module
static VOID __exit ModuleExit (VOID)
{
    DPRINTK("Entering .....");


   //1.Unregister driver

    unregister_chrdev(DRIVER_MAJOR, DRIVER_NAME);
    //2.unmap and release all mapped and requested region
     DOCS_POOL_OSM_Release();


}



module_init(ModuleInit);
module_exit(ModuleExit);
