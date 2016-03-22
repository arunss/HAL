#ifndef _HAL_APP_INTERFACE_H_
#define _HAL_APP_INTERFACE_H_  (1)

/*
HAL header file
 */

#include "HAL_Interface.h"

#define HAL_VALIDATE            (HAL_ENABLE)
#define DEBUG                   (HAL_DISABLE)
#define DEBUG_ERR               (HAL_ENABLE)


#if DEBUG
    #define DPRINT(fmt, args...) \
            printf("%s(): " fmt "\n",__FUNCTION__, ##args)
#else
    #define DPRINT(fmt, args...)
#endif

/* This is Error debug messages to print variable values or othe info*/
#if DEBUG_ERR
    #define DPRINTE(fmt, args...) \
            printf("%s(): " fmt "\n", __FUNCTION__, ##args)
#else
    #define DPRINTE(fmt, args...)
#endif


typedef enum
{
    DrvReg = 0x00,
    DrvDma = 0x01,
    DrvI2c = 0x02,
    DrvDs = 0x03,
    DrvMsg = 0x04,
    DrvUs = 0x05,

    DrvMax,

}DrvIndex;

#define FT_MUL                (0x1400000)
#define MHZ                   (1000000)
#define KHZ                   (1000)
#define TDMA_POWER_CONST      (307.2)


//Internal Protototype Goes Here
INT32 HAL_I2c_Freq_To_Paramtr(INT32 Freq, PUINT32 Paramtr);
INT32 HAL_I2c_Get_Data_Byte(INT32 Freq, PUINT32 DB);
INT32 HAL_I2c_Select_Band(INT32 Freq, PUINT32 BB);
VOID HAL_App_Init();
INT32 create_server();
INT32 create_clients();
INT32 HAL_Us_Config_Power(INT32 Us_Id, FLOAT Power);


#endif //_HAL_APP_INTERFACE_H_

