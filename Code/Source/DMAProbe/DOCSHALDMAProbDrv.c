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

 *---------------------------------------------------------------------------*/


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
#include "HAL_Interface.h"

#include "DOCSTestDefines.h"
//#include "CrePeHAL.h"
#include "DMATest.h"
#include "DMATest2.h"
#include "DMATest3.h"



//Include External Interfaces


/* ----------------------------------------------------------------------- */
/* Macros.                                                                 */
/* ---------------------------------------------------------------------- */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AShah");
MODULE_DESCRIPTION("DMA Prob module");



#define DRV_TEST            HAL_ENABLE
#define REG_LEN             PAGE_SIZE
#define REG_BASE            (UINT32)0x70000000 //ashah

#define DMA_TEST1     (0x01)
#define DMA_TEST2     (0x02)
#define DMA_TEST3     (0x03)
#define DMA_TEST4     (0x04)
#define DMA_TEST5     (0x05)
#define DMA_TEST6     (0x06)
#define DMA_TEST7     (0x07)
#define DMA_TEST8     (0x08)
#define DMA_TEST9     (0x09)
#define DMA_TEST10    (0x0a)
#define DMA_TEST11    (0x0b)
#define DMA_TEST12    (0x0c)
#define DMA_TEST13    (0x0d)


#define     HAL_REG_MAC_DS                     (0x00)
#define     HAL_REG_MAC_US                     (0x01)
#define     HAL_REG_MAC_DMA                 (0x02)
#define     HAL_REG_PHY                     (0x20)
#define   HAL_REG_INT                 (0x30)


/* ---------------------------------------------------------------------- */
/* extern.                                                       */
/* ---------------------------------------------------------------------- */

extern INT32 HAL_Reg_Read(UINT32 Offset,PUINT32 Value,UINT32 Type);
extern INT32 HAL_Reg_Write(UINT32 Offset,UINT32 Value,UINT32 Type);
extern VOID HAL_Dma_Reg_Start(INT32 context, UINT32 BuffAddr, UINT32 Len);
/* ----------------------------------------------------------------------- */
/* Internal prototypes.                                                    */
/* ----------------------------------------------------------------------- */

static INT32 DOCS_REGDRV_OSM_Open (struct inode *inode, struct file *file);
static INT32 DOCS_REGDRV_OSM_Release (struct inode *inode, struct file *file);
static INT32 DOCS_REGDRV_OSM_Ioctl(struct inode *inode, \
                      struct file *file,UINT32 cmd, ULONG arg);
static INT32 DOCS_REGDRV_OSM_Mmap(struct file * filp, \
                            struct vm_area_struct * vma);

static INT32 __init RegDrvInit (VOID);
static VOID __exit RegDrvExit (VOID);



UINT32 Data;
UINT32 SIZE;

/* ------------------------------------------------------------------------- */
/* Functions.                                                                */
/* ------------------------------------------------------------------------- */


// open function - called when the "file" /dev/RegMap is opened in userspace
static INT32 DOCS_REGDRV_OSM_Open (struct inode *inode, struct file *file)
{
    INT32 retval = HAL_SUCCESS;

    DPRINTK("Entering.....");

    return retval;
}

// close function - called when the "file" /dev/RegMap is closed in userspace
static INT32 DOCS_REGDRV_OSM_Release (struct inode *inode, struct file *file)
{
    INT32 retval = HAL_SUCCESS;


    DPRINTK("Entering.....");

    return retval;
}


static INT32 DOCS_REGDRV_OSM_Mmap(struct file * filp, \
                               struct vm_area_struct * vma)
{
  INT32 retval = HAL_SUCCESS;


  DPRINTK("Entering.....");
#if 0
    //vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    if (remap_pfn_range(vma, vma->vm_start,__phys_to_pfn(RegBase),
                                                  // chaged RegBase to Data
                /*SIZE*/REG_LEN, vma->vm_page_prot)) {
                return -EAGAIN;
    }


    DPRINTK(" User Space Address MAP %lx", vma->vm_start);
    DPRINTK(" Size 0x%lx\n", SIZE);
    DPRINTK("Mapped Physical address is 0x%x\n ", \
             (INT32)__io_address((ULONG)REG_BASE));
#endif
    return retval;

}

// define which file operations are supported
struct file_operations regmap_drv_fops = {
  .owner      =   THIS_MODULE,
  .ioctl      =   DOCS_REGDRV_OSM_Ioctl,
  .mmap       =   DOCS_REGDRV_OSM_Mmap,
  .open       =   DOCS_REGDRV_OSM_Open,
  .release    =   DOCS_REGDRV_OSM_Release,
};

// initialize module
static INT32 __init RegDrvInit (VOID)
{
  DPRINTK("Entering .....");

  if (register_chrdev(DRIVER_MAJOR,DRIVER_NAME, &regmap_drv_fops) <0 )
  {
    DPRINTK("unable to get major for map module");
    return -EIO;
  }

  PRINT_DRV_INFO();
  DPRINTK("create the device entry in /dev ");
  DPRINTK("mknod -m 666 /var/%s c %d 0", DRIVER_NAME, DRIVER_MAJOR);

  return HAL_SUCCESS;
}





// close and cleanup module

static VOID __exit RegDrvExit (VOID)
{
  DPRINTK("Exit DMAProb .....");
  unregister_chrdev(DRIVER_MAJOR,DRIVER_NAME);
}



module_init(RegDrvInit);
module_exit(RegDrvExit);

// ioctl - I/O control
// ashah added a arg size
static INT32 DOCS_REGDRV_OSM_Ioctl(struct inode *inode, struct file *file,
        UINT32 cmd, ULONG arg)
{
  INT32 retval = HAL_SUCCESS;
  UINT32 Data;

  DPRINTK("Entering..... Command %d \n", cmd);

    if (copy_from_user(&Data, (UINT32*)arg, sizeof(INT32)))
                         return -EFAULT;

    switch (cmd)
    {
        case DMA_TEST1:
       DOCS_DMA_Test1(8, REGISTER, 1);
      break;

      case DMA_TEST2:
       DOCS_DMA_Test1(8, DESCRIPTOR, 1);
      break;

      case DMA_TEST3:
       DOCS_DMA_Test1(9, REGISTER, 1);
      break;

      case DMA_TEST4:
       DOCS_DMA_Test1(9, DESCRIPTOR, 1);
      break;

      case DMA_TEST5:
       DOCS_DMA_Test1(8, DESCRIPTOR, 10);
      break;

      case DMA_TEST6:
       DOCS_DMA_Test1(9, DESCRIPTOR, 10);
      break;

      case DMA_TEST7:
       DOCS_DMA_Test1(8, REGISTER, 1);
       DOCS_DMA_Test1(9, REGISTER, 1);
       DOCS_DMA_Test1(8, REGISTER, 1);
      break;

      case DMA_TEST8:
       DOCS_DMA_Test1(8, DESCRIPTOR, 1);
       DOCS_DMA_Test1(9, DESCRIPTOR, 1);
       DOCS_DMA_Test1(8, DESCRIPTOR, 1);
      break;

      case DMA_TEST9:
       DOCS_DMA_Test1(8, REGISTER, 10);
      break;

      case DMA_TEST10:
       DOCS_DMA_Test3(8, DESCRIPTOR, 64);
       //DOCS_DMA_Test2(12, DESCRIPTOR, 1);
      break;

      case DMA_TEST11:
       DOCS_DMA_Test3(9, DESCRIPTOR, 64);
       //DOCS_DMA_Test2(13, DESCRIPTOR, 1);
      break;

      case DMA_TEST12:
       //DOCS_DMA_Test3(8, DESCRIPTOR, 64);
      break;

      case DMA_TEST13:
       //DOCS_DMA_Test3(9, DESCRIPTOR, 64);
      break;

      default:
        DPRINTK("default DMA test");
      break;

  }

    return retval;
}
