#ifndef __DOCS_HAL_INTERFACE_H_
#define __DOCS_HAL_INTERFACE_H_         (1)

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

#include "HAL_Types.h"
#include "HAL_OS_Defines.h"
#include "HAL_MAC_Register.h"


/*----------------------------------------------------------------------------
* PURPOSE
*       Contains all exported HAL interfaces
*
*----------------------------------------------------------------------------*/


//----------------------------------------------------------------------------
//HAL Fixup Macros
//----------------------------------------------------------------------------
#define FPGAR_VER_01            1
#define FPGAR_VER_02            2

//Define For Which Version Are we Compiling the BSP
#define FPGA_VERSION            FPGAR_VER_02



//Use Buffers from Buffer Pool for Creating Messages
#define RBU_FIX_000             (0x00)

//Drop 1 out of MAP_DROP_CNT Map Messages From Tasklet/Thread
#define RBU_FIX_001             (0x01)

//Drop 1 out of MAP_DROP_CNT Map Messages From Interrupt Handler
#define RBU_FIX_002             (0x01)

//<RBU Fix, Date � 17-APR-2009, AShah, RBU_EXT_CLK_FIX is Enabled Default>
//Enables the External Clock Configuration
#define RBU_EXT_CLK_FIX         (0x01)

//<RBU Fix, Date � 16-APR-2009, AShah, To Support Both Version FPGA>
#if (FPGA_VERSION==FPGAR_VER_01)
  //Disables the DS Overflow Interrupt Handling
  #define RBU_DS_OVF_INT_FIX      (0x00)
#else
  //Enables the DS Overflow Interrupt Handling
  #define RBU_DS_OVF_INT_FIX      (0x01)
#endif

//----------------------------------------------------------------------------
//HAL Defination Macros
//----------------------------------------------------------------------------

#define IS_SNR_IN_RANGE(snrValue) (snrValue<=0x14)? HAL_TRUE: HAL_FALSE;

//BitMap of the Active Channel
#define DS_GET_ACTIVE_CHANNEL()             (0x02) //(0x03)

#define DS_GET_CHANNEL()                    (0x01) //  (0x02)
#define DS_GET_MAX_CHANNEL()                   (0x02)
#define GET_CHANNEL_ADDR(PhyAddr, Channel)    (PhyAddr                \
                                            | ((Channel) << 13)     \
                                            | ((Channel) << 16));

#define US_GET_INIT_POWER()                   (-0.1)

//----------------------------------------------------------------------------
//HAL IOCTL Commands
//----------------------------------------------------------------------------
//Register Driver IOCTL Commands
#define CMD_REG_WRITE                       (0x01)
#define CMD_REG_READ                        (0x02)

//DS IOCTL Commands
#define CMD_DS_HW_INIT                      (0x01)
#define CMD_DS_TUNE_FREQ                    (0x02)

//US IOCTL Commands
#define CMD_US_HW_INIT                      (0x01)
#define CMD_US_TUNE_FREQ                    (0x02)
#define CMD_US_PRG_POWER                    (0x03)


//Message Driver IOCTL Command
#define CMD_MSG_SET_PORT                    (0x01)
#define CMD_MSG_CLR_PORT                    (0x02)


//----------------------------------------------------------------------------
//HAL Register Interface
//----------------------------------------------------------------------------
#define     HAL_REG_MAC_DS             (0x00)
#define     HAL_REG_MAC_US             (0x01)
#define     HAL_REG_MAC_DMA            (0x02)
#define     HAL_REG_PHY                (0x20)
#define     HAL_REG_INT                (0x30)



typedef struct {
  UINT32 Addr;
  UINT32 Value;
  UINT32 Type;
}Reg_RD_WR_t;



INT32 HAL_Reg_Read(UINT32 Offset,PUINT32 Value,UINT32 Type);
INT32 HAL_Reg_Write(UINT32 Offset,UINT32 Value,UINT32 Type);
UINT32 HAL_Reg_Get_Base(UINT32 Type);

//Raghu, May 9th
#ifdef _DEBUG_REG_BASED_
INT32 HAL_Reg_Write_Debug(UINT32 Addr,UINT32 Value,UINT32 Type);
INT32 HAL_Reg_Write_Debug1(UINT32 Addr,UINT32 Value,UINT32 Type);
INT32 HAL_Reg_Write_Debug2(UINT32 Addr,UINT32 Value,UINT32 Type);
INT32 HAL_Reg_Read_Debug(UINT32 Addr,PUINT32 Value,UINT32 Type);
INT32 HAL_Reg_Read_Debug1(UINT32 Addr,PUINT32 Value,UINT32 Type);
INT32 HAL_Reg_Read_Debug2(UINT32 Addr,PUINT32 Value,UINT32 Type);
#endif
//Raghu, May 9th


INT32 HAL_Pmu_Start_Clk(void);
INT32 HAL_Pmu_Stop_Clk(void);
UINT32 getCCNT(void);
//----------------------------------------------------------------------------
//Hardware Init Interface
//----------------------------------------------------------------------------



INT32 HAL_Ds_Hardware_Init(void);
VOID HAL_Ds_Phy_Init(INT32 PhyNo);
VOID HAL_I2c_Init(INT32 Interface, INT32 Channel);
INT32 HAL_Us_Hardware_Init(void);

enum DocsPacketType
{
    DATA_PDU = 0x00,
    AUX_TIME = 0x01,
    ISOLATE = 0x2,
    MAC_MANAGE = 0x3,

    DocsPacketMax
};

//----------------------------------------------------------------------------
//Enable/Disable Hardware Interface
//----------------------------------------------------------------------------

typedef enum DS_Id_t
{
    DS_Id1 = 0x00,
    DS_Id2 = 0x01,
    DS_Id3 = 0x02,
    DS_Id4 = 0x03,

    DS_Id_Max
}DS_Id_t;

#ifndef _MAC_COMMON_DEFS_H_
typedef enum US_Id_t
{
    US_Id1 = 0x00,
    US_Id2 = 0x01,
    US_Id3 = 0x02,
    US_Id4 = 0x03,

    US_Id_Max
}US_Id_t;
#endif

INT32 HAL_Ds_Enable(INT32 Channel_No);
INT32 HAL_Ds_Disable(INT32 Channel_No);

INT32 HAL_Us_Enable(INT32 Channel_No);
INT32 HAL_Us_Disable(INT32 Channel_No);

//----------------------------------------------------------------------------
//Hardware Configuration Interface
//----------------------------------------------------------------------------

INT32 HAL_Config_Inversion(UINT32 DS_Channel);
INT32 HAL_Config_QAM(UINT32 DSChannel, INT32 QAM);

INT32 HAL_Us_Config_Fifo(void);

INT32 HAL_Get_SNR(INT32 DS_Channel);
INT32 HAL_Get_Lock_Status(INT32 DS_Channel);
INT32 HAL_Ds_Lock_Channel(INT32 DS_Channel);
INT32 HAL_Get_Current_QAM(INT32 DS_Channel);

//----------------------------------------------------------------------------
//I2C Programming Interface
//----------------------------------------------------------------------------

typedef enum I2CInterface
{
    IF_MASTER = 0x000,
    IF_SLAVE  = 0x100,

    I2CInterfaceMax
}I2c_Interface_t;


INT32 HAL_I2c_Write(INT32 Interface, INT32 DevId, PINT32 Data, INT32 Length);
INT32 HAL_I2c_Read(INT32 Interface, INT32 DevId, PINT32 Data, INT32 Length);


//----------------------------------------------------------------------------
//SPI Programming Interface
//----------------------------------------------------------------------------

typedef enum
{
    Spi_DDS = 0x40000000, //SPI_CS0_SELECT
    Spi_PA  = 0x80000000, //SPI_CS1_SELECT

    Spi_Interface_Max
}Spi_Interface_t;


INT32 HAL_Spi_Transmit(INT32 Interface,
                       INT32 Channel,
                       PUINT32 Buff_Addr,
                       INT32 length);

INT32 HAL_Spi_Receive(INT32 Interface,
                      INT32 Channel,
                      PUINT32 Buff_Addr,
                      UINT32 length);


//----------------------------------------------------------------------------
// Tuner Programming Interface
//----------------------------------------------------------------------------

INT32 HAL_Ds_Tune_Frequency(INT32 Ds_id, UINT32 FreqCw);
INT32 HAL_Ds_Tune_Freq(INT32 Ds_Id, UINT32 Freq);
//INT32 HAL_Set_Ds_Frequency(INT32 Channel, INT32 FreqCw);

INT32 HAL_Us_Tune_Frequency(INT32 US_Id, UINT32 FreqCw);
INT32 HAL_Us_Tune_Freq(INT32 US_Id, UINT32 Freq);

//----------------------------------------------------------------------------
// Power Programming Interface
//----------------------------------------------------------------------------

INT32 HAL_Us_Program_Power(INT32 US_Id, UINT32 Pa, UINT32 Pd);


//----------------------------------------------------------------------------
// DMA Programming Interface
//----------------------------------------------------------------------------


INT32 HAL_Dma_Send(INT32 context,UINT32 Buff_Addr,UINT32 len);
VOID HAL_Dma_Reg_Start(INT32 context, UINT32 Buff_Addr, UINT32 Len);
VOID HAL_Dma_Init_CallBack(INT32 idx, UINT32 Ptr_Function);
VOID HAL_Dma_Init_CallBack_MAP(INT32 idx, UINT32 Ptr_Function);
INT32 HAL_Dma_OverFlow(INT32 context);

//----------------------------------------------------------------------------
// Driver Interfaces
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//Message Queue Interface

//Message Types

enum DocsMsgType
{
    MsgDmaDs = 0x00,
    MsgDmaUs = 0x01,

    MsgMax
};

typedef struct _msgArg
{
    INT32 msgIdx;
    INT32 PortNo;
}msgArg;

//TCP/UDP Based Communication
#define TCP_SOCKET      0x00


//INT32 HAL_Msg_Send(INT32 MsgId, const char *Buffer, size_t Length);
INT32 HAL_Msg_Recv(INT32 MsgId, const char *Buffer, INT32 Length);
INT32 HAL_Msg_Send(INT32 Msg_Id, char *Buffer, INT32 Length);


//----------------------------------------------------------------------------


INT32 DOCS_POOL_OSM_Setup(void);
INT32 DOCS_POOL_OSM_Init(void);
INT32 DOCS_POOL_OSM_FreePkt(INT32 Packet);
INT32 DOCS_POOL_OSM_GetPkt(PINT32 Packet);
INT32 DOCS_POOL_OSM_Release(void);


INT32 DOCS_Q_OSM_GetQueue(PUINT32 element);
INT32 DOCS_Q_OSM_PutQueue(UINT32 element);
INT32 DOCS_Q_OSM_ResetQueue(void);
INT32 DOCS_Q_OSM_Setup(void);


INT32 DOCS_PROF_Setup(INT32 dbgAddr, INT32 size);
INT32 DOCS_PROF_Start(INT32 PktId);
INT32 DOCS_PROF_Stop(INT32 PktId);
INT32 DOCS_PROF_Init(void);



//----------------------------------------------------------------------------
//HAL Application Intrerface

INT32 HAL_Main_Control(void);
INT32 listen_clients(void);
//----------------------------------------------------------------------------

typedef enum  {

                       TSFIFO          = 0,
                       DocsDSFIFO      = 1,
                       DocsOutputFIFO  = 2,
                       OpenCableFIFO   = 3,

                       FIFOTYPEMax
               }FIFOTYPE;


    typedef enum  {
                       FIFO_TS0   = 0,
                       FIFO_TS1   = 1,
                       FIFO_TS2   = 2,
                       FIFO_TS3   = 3,
                       FIFO_TS4   = 4,
                       FIFO_TS5   = 5,



                       FIFO_DOC0  = 6,
                       FIFO_DOC1  = 7,
                       FIFO_DOC2  = 8,
                       FIFO_DOC3  = 9,

                       FIFO_DMA   = 17,



                       FIFO_OC    = 24,
                       FIFONOMax



       }FIFONO ;//enum FIFONO

INT32 HAL_Ds_Fifo_Clear(FIFOTYPE FIFOType, FIFONO FIFONo);
INT32 HAL_Ds_Fifo_Get_Status(FIFOTYPE FIFOType, FIFONO FIFONo);


//argumnets for FIFOMode
#define ENABLE_MAC_PHY_MODE         (0x1)
#define FIFO_DBG_MODE               (0x1)
#define MAC_PHY_MODE                (0x0)
#define FIFO_MODE_BIT               (0xe) //14
#define FIFO_MODE_MASK              (0x4000) //only 14 bit enabled


#endif //__DOCS_HAL_INTERFACE_H_


