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

/* =============================================================================
 * Contains I2C Driver init , exit functions and file oprations functions.
 * =============================================================================
 */

/* --------------------------------------------------------------------------*/
/* Headers.                                                                  */
/* --------------------------------------------------------------------------*/

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
#include "HAL_I2c.h"



/* --------------------------------------------------------------------------*/
/* Macros.                                                                   */
/* --------------------------------------------------------------------------*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("I2C Driver");





/* --------------------------------------------------------------------------*/
/* Internal prototypes.                                                      */
/* --------------------------------------------------------------------------*/

static INT32 HAL_I2c_Drv_Open (struct inode *inode, struct file *file);
static INT32 HAL_I2c_Drv_Release (struct inode *inode, struct file *file);
static ssize_t HAL_I2c_Drv_Read (struct file *file, char *buf,
        size_t count, loff_t *ppos);
static ssize_t HAL_I2c_Drv_Write (struct file *file, const char *buf,
        size_t count, loff_t *ppos);


static INT32 __init HAL_I2c_Drv_Init (VOID);
static VOID __exit HAL_I2c_Drv_Exit (VOID);


/* -------------------------------------------------------------------------- */
/* Internal globals.                                                          */
/* -------------------------------------------------------------------------- */


// define which file operations are supported
struct file_operations G_HAL_I2c_Drv_Fops = {
                .owner      =   THIS_MODULE,
                .llseek     =   NULL, /* AShah: TBD  */
                .read       =   HAL_I2c_Drv_Read,
                .write      =   HAL_I2c_Drv_Write,
                .ioctl      =   NULL, //HAL_I2c_Drv_Ioctl,
                .open       =   HAL_I2c_Drv_Open,
                .release    =   HAL_I2c_Drv_Release,
};


/* --------------------------------------------------------------------------*/
/* Function Definations.                                                     */
/* --------------------------------------------------------------------------*/




// open function - called when the "file" /dev/I2CDriver is opened in userspace
static INT32 HAL_I2c_Drv_Open (struct inode *inode, struct file *file)
{
    INT32 Ret_Val = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return Ret_Val;
}

// close function - called when the "file" /dev/I2CDriver is
//  closed in userspace
static INT32 HAL_I2c_Drv_Release (struct inode *inode, struct file *file)
{
    INT32 Ret_Val = HAL_SUCCESS;

    DPRINTK("Entering.....");


    return Ret_Val;
}

// read function called when from /dev/I2CDriver is read
static ssize_t HAL_I2c_Drv_Read (struct file *file, char *buf,
        size_t count, loff_t *ppos)
{
    INT32 Ret_Val = HAL_SUCCESS;

    return Ret_Val;
}

// write function called when to /dev/I2CDriver is written
static ssize_t HAL_I2c_Drv_Write (struct file *file, const char *buf,
        size_t count, loff_t *ppos)
{
    INT32 Ret_Val = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return Ret_Val;
}






static INT32 __init HAL_I2c_Drv_Init (VOID)
{
    INT32 Ret_Val = HAL_SUCCESS;

    DPRINTK("Entering .....");

    /*
     * 1. Regsister as Char Driver
     * 2. Setup I2C
     */

    Ret_Val = register_chrdev (DRIVER_MAJOR, DRIVER_NAME, &G_HAL_I2c_Drv_Fops);
    if (Ret_Val != 0) return - EIO;

    PRINT_DRV_INFO();
    DPRINTK("create the device entry in /var ");
    DPRINTK("mknod -m 666 /var/%s c %d 0", DRIVER_NAME, DRIVER_MAJOR);

    HAL_I2c_Setup();

    return HAL_SUCCESS;
}


// close and cleanup module
static VOID __exit HAL_I2c_Drv_Exit (VOID)
{

    DPRINTK("Entering .....");

    /*
     * 1. Unregister as Char Driver
     * 2. Release All DMA Context
     */

    //1.Unregister driver
    unregister_chrdev(DRIVER_MAJOR, DRIVER_NAME);

    //2.unmap and release all mapped and requested region
    //DOCS_I2C_OSM_Release();

}


module_init(HAL_I2c_Drv_Init);
module_exit(HAL_I2c_Drv_Exit);
