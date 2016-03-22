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

/*---------------------------------------------------------------------------
 * PURPOSE
 * I2C Probe driver used to call I2C driver functions and serve as interface
 * for I2C driver calls/ tests from user applications.
 * Temporary it calls SPI functions too.
 *---------------------------------------------------------------------------*/


/* ------------------------------------------------------------------------ */
/* Headers.                       */
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
#include "HAL_Interface.h"

#include "HAL_I2c_Probe_Defines.h"
#include "HAL_I2c_Probe_Drv.h"


//Include External Interfaces


/* ------------------------------------------------------------------------ */
/* Macros.                                                                  */
/* ------------------------------------------------------------------------ */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("I2C Prob module");



#define DRV_TEST     HAL_ENABLE
#define REG_LEN      PAGE_SIZE


/* ------------------------------------------------------------------------ */
/* Internal prototypes.                                                     */
/* ------------------------------------------------------------------------ */

static INT32 HAL_I2c_Probe_Drv_Open(struct inode *inode, struct file *file);
static INT32 HAL_I2c_Probe_Drv_Release(struct inode *inode,struct file *file);
static INT32 HAL_I2c_Probe_Drv_Ioctl(struct inode *inode,
                                     struct file *file,
                                     UINT32 cmd,
                                     ULONG arg);
static INT32
  HAL_I2c_Probe_Drv_Mmap(struct file * filp, struct vm_area_struct* vma);

static INT32 __init HAL_I2c_Probe_Drv_Init (VOID);
static VOID __exit HAL_I2c_Probe_Drv_Exit (VOID);


/* ------------------------------------------------------------------------ */
/* Internal globals.                                                         */
/* ------------------------------------------------------------------------- */

// define which file operations are supported
struct file_operations G_HAL_I2c_Probe_Drv_Fops = {
  .owner  =  THIS_MODULE,
  .ioctl  =  HAL_I2c_Probe_Drv_Ioctl,
  .mmap   =  HAL_I2c_Probe_Drv_Mmap,
  .open   =  HAL_I2c_Probe_Drv_Open,
  .release    = HAL_I2c_Probe_Drv_Release,
};


  INT32 G_Data[2048]; //Placing Data[2048] as local gives prob-hangs.
/* ------------------------------------------------------------------------ */
/* Functions.                                                              */
/* ------------------------------------------------------------------------ */


// open function - called when the "file" /dev/TestMap is opened in userspace
static INT32 HAL_I2c_Probe_Drv_Open(struct inode *inode, struct file *file){

  INT32 Ret_Val = HAL_SUCCESS;

  DPRINTK("Entering.....");

  return Ret_Val;
}//HAL_I2c_Probe_Drv_Open()

// close function - called when the "file" /dev/TestMap is closed
// in userspace
static INT32 HAL_I2c_Probe_Drv_Release(struct inode *inode, struct file *file){

  INT32 Ret_Val = HAL_SUCCESS;

  DPRINTK("Entering.....");

  return Ret_Val;
}//HAL_I2c_Probe_Drv_Release()

//for mmap from user space
static INT32
HAL_I2c_Probe_Drv_Mmap(struct file * filp, struct vm_area_struct *vma){

  INT32 Ret_Val = HAL_SUCCESS;
  DPRINTK("Entering.....");
  return Ret_Val;

}//HAL_I2c_Probe_Drv_Mmap()



// initialize module
static INT32 __init HAL_I2c_Probe_Drv_Init (VOID){

  DPRINTK("Entering .....");

  if(register_chrdev(DRIVER_MAJOR,DRIVER_NAME, &G_HAL_I2c_Probe_Drv_Fops)< 0){
    DPRINTK("unable to get major for map module");
    return -EIO;
  }//if

  PRINT_DRV_INFO();
  DPRINTK("create the device entry in /dev ");
  DPRINTK("mknod -m 666 /var/%s c %d 0", DRIVER_NAME, DRIVER_MAJOR);

  HAL_Ds_Hardware_Init();

  return HAL_SUCCESS;
}//HAL_I2c_Probe_Drv_Init()


// close and cleanup module

static VOID __exit HAL_I2c_Probe_Drv_Exit (VOID){

  DPRINTK("Exit DMAProb .....");
  unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
}//HAL_I2c_Probe_Drv_Exit()



module_init(HAL_I2c_Probe_Drv_Init);
module_exit(HAL_I2c_Probe_Drv_Exit);


// ioctl - I/O control

static INT32 HAL_I2c_Probe_Drv_Ioctl(struct inode *inode, struct file *file,
   UINT32 cmd, ULONG arg)
{

  INT32 Ret_Val = HAL_SUCCESS;
  INT32 Paramtr, Size, Idx;
  ULONG Arg_Addr = arg;

  DPRINTK("Entering..... Command %d \n", cmd);

  if (copy_from_user(&Size, (INT32*)Arg_Addr, sizeof(INT32))){

   return -EFAULT;
  }//if

  DPRINTK("Size %x", Size);

  Arg_Addr+=4;
  for(Idx = 0x00; Idx < Size; Idx++,Arg_Addr+=4) {
    if (copy_from_user(&G_Data[Idx], (INT32*)Arg_Addr, sizeof(INT32))){
       return -EFAULT;
    }//if
    DPRINTK("G_Data[%d] %08x",Idx, G_Data[Idx]);
  }//for


  switch (cmd)
  {
    case I2C_TEST1:
      Paramtr = G_Data[0x00] ;
      HAL_Ds_Tune_Frequency(DS_Id1, Paramtr);
      break;

    case I2C_TEST2:
      Paramtr = G_Data[0x00] ;
      HAL_Ds_Tune_Frequency(DS_Id2, Paramtr);
      break;

    case SPI_TEST1: //Tx SpiDDS
      Paramtr = G_Data[0x00] ;
      HAL_Spi_Transmit(Spi_DDS, US_Id1, (PUINT32)&G_Data,7);

      break;

    case SPI_TEST2: //Rx SpiDDS

      HAL_Spi_Receive(Spi_DDS, US_Id1, (PUINT32)&G_Data, 7);

      Arg_Addr = arg+4;
      for(Idx = 0x00; Idx < Size; Idx++,Arg_Addr+=4){
        if (copy_to_user((INT32*)Arg_Addr, &G_Data[Idx], sizeof(INT32))){
           return -EFAULT;
        }//if
      }//for

      break;

      case SPI_TEST3: //Tx SpiPA
        Paramtr = G_Data[0x00] ;
        HAL_Spi_Transmit(Spi_PA, US_Id1, (PUINT32)&G_Data, 1);

      break;
  }

  return Ret_Val;
}
