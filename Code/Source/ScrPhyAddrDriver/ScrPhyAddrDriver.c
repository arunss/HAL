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




/* -------------------------------------------------------------------------- */
/* Headers.                                                                   */
/* -------------------------------------------------------------------------- */

//Standard Header Files
#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/io.h>

//Custom Header Files
#include "HAL_Types.h"
#include "DOCSBSPDefines.h"
#include "CrePeHAL.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("Char Driver To Access Mac Register");

/* -------------------------------------------------------------------------- */
/* Macros.                                                                    */
/* -------------------------------------------------------------------------- */
#define MACIO_REG_READ 0x00
#define MACIO_REG_WRITE 0x01

/* -------------------------------------------------------------------------- */
/* Internal globals.                                                          */
/* -------------------------------------------------------------------------- */

INT32 result;
UINT32 Addr;
UINT32 Data;
UINT32 base_address=DOC_DS_MAC_REGISTER_BASE_ADDRESS;
UINT32 io_size = 0xffff;
//UINT32 io_size = 0x2FD1000;
UINT32 test_address = TEST_BUFFER_BASE_ADDRESS;
UINT32 buf_size =TEST_BUFFER_SIZE *NUMBER_OF_TEST_BUFFER;
UINT32 io_addr;
UINT32 buf_addr;
UINT32 drv_addr;
UINT32 phy_address = DOC_PHY_REGISTER_BASE_ADDRESS; //ashah
UINT32 phy_size = 0x1ffff ;//ashah
UINT32 phy_addr; //ashah

#ifdef dma
UINT32 dma_address = DMA_REGISTER_BASE_ADDRESS;
UINT32 dma_size = 0xFFFF ;
UINT32 dma_addr;
#endif
/* -------------------------------------------------------------------------- */
/* Type definitions.                                                          */
/* -------------------------------------------------------------------------- */

typedef struct t_addr_value{
    unsigned int addr;
    unsigned int value;
}addr_value;


/* -------------------------------------------------------------------------- */
/* Internal prototypes.                                                       */
/* -------------------------------------------------------------------------- */

INT32  MacRegOpen(struct inode*,struct file*);
INT32  MacRegRelease(struct inode*,struct file*);
static INT32 MacRegIoctl(struct inode*,struct file*,unsigned int,unsigned long);
INT32  MacRegInit(void);
VOID   MacRegCleanup(void);

/* -------------------------------------------------------------------------- */
/* Functions.                                                                 */
/* -------------------------------------------------------------------------- */

/* Device Open Call */
INT32 MacRegOpen(struct inode* inode,struct file* pt_file)
{
    /* Nothing to Do Now */
    return HAL_SUCCESS;
}

/* Device Close Call */
INT32 MacRegRelease(struct inode* inode,struct file* pt_file)
{
    /* Nothing to Do Now */
    return HAL_SUCCESS;
}

/* Device Ioctl Call */
static INT32 MacRegIoctl(struct inode* inode,struct file* pt_file,\
                                   unsigned int cmd,unsigned long arg)
{
    //    DPRINTK("\n Entered Ioctl routine \n");
    unsigned int* pt;
    UINT32 offset;
    pt = (unsigned int*)arg;

    /* Firsr Argument Contains the Address */
    if(copy_from_user(&Addr,(unsigned int*)arg,sizeof(int)))
    {
        DPRINTK("Unable to copy from user space");
        return HAL_FAILED;
    }

    /* Validate the Address for MAC Register Range */
    if((Addr>=base_address)&&(Addr<(base_address+io_size)))
    {
        offset = Addr - DOC_DS_MAC_REGISTER_BASE_ADDRESS;
        drv_addr = io_addr;
    }
    else if((Addr>=test_address)&&(Addr<(test_address+buf_size)))
    {
        offset = Addr - TEST_BUFFER_BASE_ADDRESS;
        drv_addr = buf_addr;
    }
    //ashah
    else if((Addr>=phy_address)&&(Addr<(phy_address+phy_size)))
    {
        offset = Addr - DOC_PHY_REGISTER_BASE_ADDRESS;
        drv_addr = phy_addr;
    }
    //ashah
    #ifdef dma
    else if((Addr>=dma_address)&&(Addr<(dma_address+dma_size)))
    {
        offset = Addr - DMA_REGISTER_BASE_ADDRESS;
        drv_addr = dma_addr;
    }
    #endif
    //ashah
    else
    {
        DPRINTK("Error in address value transformed, Address %x",Addr);
        return HAL_FAILED;
    }

    /* Check Command */
    switch(cmd)
    {
        case MACIO_REG_READ: /* Do Mac Register Read */
           Data = readl(drv_addr + offset);
           arg+=4;// change
           if(copy_to_user((unsigned int*)arg,&Data,sizeof(int)))
           {
                DPRINTK("Unable to copy to user space");
                return HAL_FAILED;
           }
           break;

        case MACIO_REG_WRITE: /* Do Mac Register Write */
           arg+=4;
           if(copy_from_user(&Data,(unsigned int*)arg,sizeof(int)))
           {
                DPRINTK("Unable to copy from user space");
                return HAL_FAILED;
           }
           writel(Data,(drv_addr + offset));
           break;
    }
//    DPRINTK("\nExiting IOCTL \n");
    return HAL_SUCCESS;
}

/* File Operations Supported From MacReg Driver */
struct file_operations char_ops = {
    .owner   = THIS_MODULE,
    .ioctl   = MacRegIoctl,
    .open       = MacRegOpen,
    .release = MacRegRelease,
};

/* Called When Driver Inserted */
INT32 MacRegInit(VOID)
{
       /*
        * 1. Register as Char Driver
        * 2. Request Mac Register Space
        * 3. Map Physical Space to Logical Space
        */

    PRINT_DRV_INFO();
    DPRINTK("create the device entry in /var/ ");
    DPRINTK("mknod -m 666 /var/%s c %d 0", DRIVER_NAME, DRIVER_MAJOR);

    /* 1. Register as Char Driver */
    result=register_chrdev(DRIVER_MAJOR,DRIVER_NAME,&char_ops);
    if(result<0)
    {
        DPRINTK("Could not register device\n");
        return result;
    }

    /* 2. Request Mac Register Space */
    if(!(request_mem_region(base_address,io_size,DRIVER_NAME)))
    {
        DPRINTK("Cant access region %x\n", base_address);
        unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
        return HAL_FAILED;
    }

    if(!(request_mem_region(test_address,buf_size,DRIVER_NAME)))
    {
        DPRINTK("Cant access test region %x\n",test_address);
        unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
        return HAL_FAILED;
    }
    //ashah
    if(!(request_mem_region(phy_address,phy_size,DRIVER_NAME)))
    {
        DPRINTK("Cant access test region %x\n",phy_address);
        unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
        return HAL_FAILED;
    }
    //ashah
    //ashah
    #ifdef dma
    if(!(request_mem_region(dma_address,dma_size,DRIVER_NAME)))
    {
        DPRINTK("Cant access test region %x\n",dma_address);
        unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
        return HAL_FAILED;
    }
    #endif
    //ashah

    /* 3. Map Physical Space to Logical Space */
    io_addr=(unsigned long)ioremap(base_address,io_size);
    buf_addr=(unsigned long)ioremap(test_address,buf_size);
    phy_addr =(unsigned long)ioremap(phy_address,phy_size); //ashah
    #ifdef dma
    dma_addr =(unsigned long)ioremap(dma_address,dma_size); //ashah
    #endif
    return HAL_SUCCESS;
}

/* Called When Driver Removed */
VOID MacRegCleanup(VOID)
{
    /*
     * 1. UnRegister the Driver
     * 2. Un Map Memory
     * 3. Release Memory
     */

    /* 1. UnRegister the Driver */
    unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);

    /* 2. Un Map Memory */
    iounmap((void *)io_addr);
    iounmap((void *)buf_addr);
    iounmap((void *)phy_addr);//ashah
    //iounmap((void *)dma_addr);//ashah

     /* 3. Release Memory */
    release_mem_region(base_address,io_size);
    release_mem_region(test_address,buf_size);
    release_mem_region(phy_address,phy_size);//ashah
    #ifdef dma
    release_mem_region(dma_address,dma_size);//ashah
    #endif
    return;
}

/* Register Driver Init and Exit Calls */
module_init(MacRegInit);
module_exit(MacRegCleanup);
