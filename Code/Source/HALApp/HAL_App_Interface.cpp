/*
Test code : Added by AShah
 */



/*-----------------------------------------------------------------------------
* PURPOSE
*     Has memory request/map and unmap for PHY SPI register region.
*----------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------*/
/* Headers.                                                                  */
/* --------------------------------------------------------------------------*/

//Standard Header Files
#include <linux/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

//Custom Header Files
#include "HAL_App_Interface.h"



//Global Variable Goes Here
INT32 driver_fd[DrvMax];


VOID HAL_App_Init()
{
  INT32 idx = 0x00;

  for(idx=0; idx<DrvMax; idx++)
    driver_fd[idx] = (INT32)NULL;

  driver_fd[DrvReg] = open("/var/REGDriver", O_RDWR);

  if (driver_fd[DrvReg] == -1 )
  {
      DPRINTE("err in open /var/REGDriver \n ");
  }

  driver_fd[DrvDs] = open("/var/DsDriver", O_RDWR);

  if (driver_fd[DrvDs] == -1 )
  {
      DPRINTE("err in open /var/DsDriver \n ");
  }

  driver_fd[DrvMsg] = open("/var/MsgDriver", O_RDWR);

  if (driver_fd[DrvMsg] == -1 )
  {
      DPRINTE("err in open /var/MsgDriver \n ");
  }

  driver_fd[DrvUs] = open("/var/UsDriver", O_RDWR);

  if (driver_fd[DrvUs] == -1 )
  {
      DPRINTE("err in open /var/UsDriver \n ");
  }

}

INT32 HAL_Reg_Write(UINT32 Addr,UINT32 Value,UINT32 Type)
{
    Reg_RD_WR_t tmpRegRDWR;

    tmpRegRDWR.Addr = Addr;
    tmpRegRDWR.Value= Value;
    tmpRegRDWR.Type = Type;

    ioctl(driver_fd[DrvReg], CMD_REG_WRITE, &tmpRegRDWR);

    return HAL_SUCCESS;

}

INT32 HAL_Reg_Read(UINT32 Addr,PUINT32 Value,UINT32 Type)
{
    Reg_RD_WR_t tmpRegRDWR;

    tmpRegRDWR.Addr = Addr;
    tmpRegRDWR.Value= 0x00;
    tmpRegRDWR.Type = Type;

    ioctl(driver_fd[DrvReg], CMD_REG_READ, &tmpRegRDWR);

    *Value = tmpRegRDWR.Value;

    return HAL_SUCCESS;
}

/******************************************************************************
 *
 * Name: HAL_Ds_Enable
 *
 * Description:
 *
 *
 *
 * Arguments:
 *
 *
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


//DSChannel Corresponds to StreamId Starts from 0..3
INT32 HAL_Ds_Enable(INT32 DS_Channel)
{
    UINT32 Addr, Value;

#if HAL_VALIDATE
    if(DS_Channel >= US_Id_Max)
        return HAL_FAILED;
#endif

    HAL_Reg_Read(REG_TS_MAP, &Value, HAL_REG_MAC_DS);
    Value |= ((0x01) << DS_Channel) ;
    HAL_Reg_Write(REG_TS_MAP, Value, HAL_REG_MAC_DS);


    return HAL_SUCCESS;

}


/******************************************************************************
 *
 * Name: HAL_Ds_Disable
 *
 * Description:
 *
 *
 *
 * Arguments:
 *
 *
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


//DSChannel Corresponds to StreamId Starts from 0..3
INT32 HAL_Ds_Disable(INT32 DS_Channel)
{
    UINT32 Addr, Value;

#if HAL_VALIDATE
    if(DS_Channel >= DS_Id_Max)
        return HAL_FAILED;
#endif

    HAL_Reg_Read(REG_TS_MAP, &Value, HAL_REG_MAC_DS);
    Value &= (~((0x01) << DS_Channel));
    HAL_Reg_Write(REG_TS_MAP, Value, HAL_REG_MAC_DS);


    return HAL_SUCCESS;

}

//USChannel Corresponds to StreamId Starts from 0..3
INT32 HAL_Us_Enable(INT32 USChannel)
{
    UINT32 Addr, Value;

    if(USChannel >= US_Id_Max)
        return HAL_FAILED;

    HAL_Reg_Read(REG_US_CFG, &Value, HAL_REG_MAC_US);
    Value |= ((0x01) << USChannel) ;
    HAL_Reg_Write(REG_US_CFG, Value, HAL_REG_MAC_US);


    return HAL_SUCCESS;
}

//USChannel Corresponds to StreamId Starts from 0..3
INT32 HAL_Us_Disable(INT32 USChannel)
{
    UINT32 Addr, Value;

    if(USChannel >= US_Id_Max)
        return HAL_FAILED;

    HAL_Reg_Read(REG_US_CFG, &Value, HAL_REG_MAC_US);
    Value &= (~((0x01) << USChannel));
    HAL_Reg_Write(REG_US_CFG, Value, HAL_REG_MAC_US);

    return HAL_SUCCESS;
}

INT32 HAL_Us_Config_Fifo(void)
{
   //1. Configure US FIFO mode .
   //Need to check any other value could be send.
    UINT32 REG_US_CFG_Val;

    HAL_Reg_Read(REG_US_CFG, &REG_US_CFG_Val, HAL_REG_MAC_US);
#if ENABLE_MAC_PHY_MODE
    REG_US_CFG_Val |=  ( (MAC_PHY_MODE << FIFO_MODE_BIT) & FIFO_MODE_MASK );
#else
    REG_US_CFG_Val |=  ( (FIFO_DBG_MODE << FIFO_MODE_BIT) & FIFO_MODE_MASK );
#endif
    HAL_Reg_Write(REG_US_CFG, REG_US_CFG_Val, HAL_REG_MAC_US);
}





//Returns the SNR Value
INT32 HAL_Get_SNR(INT32 DSChannel)
{
    UINT32 SnrValue;
    UINT32 SnrAddr;

    if(DSChannel >= (UINT32)DS_Id_Max)
        return HAL_FALSE;

    SnrAddr = GET_CHANNEL_ADDR(REG_PHY_GRP2AREG44, DSChannel);

    HAL_Reg_Read(SnrAddr, &SnrValue, HAL_REG_PHY);

    return (SnrValue & 0xFF);
}


//ChannelNo: DS StreamId starts from 0..3
//Returns: Bit Map of the Status
// bit-0: QAM Lock
// bit-1: FEC Syn Lock
// bit-2: Mpeg Frame Sync Lock
// bit-3: Phy Lock
// Phy Lock need to be 1 in order to recieve the data from the channel

INT32 HAL_Get_Lock_Status(INT32 DSChannel)
{
    INT32 LockStatus;
    UINT32 LockAddr, Value;

    if(DSChannel >= (UINT32)DS_Id_Max)
        return HAL_FALSE;

    LockAddr = GET_CHANNEL_ADDR(REG_PHY_LOCKSTATUS, DSChannel);

    HAL_Reg_Read(LockAddr, &Value, HAL_REG_PHY);

    LockStatus = (Value & 0x0F);
    LockStatus |= (((Value & 0x1F) && 0x1F) << 0x03);

    return LockStatus;
}

/******************************************************************************
 *
 * Name: HAL_Ds_Lock_Channel
 *
 * Description:
 *
 *
 *
 * Arguments:
 *
 *
 *
 * Return Value:
 *   Status: HAL_SUCCESS
 *           HAL_FAILED :
 *
 *****************************************************************************/


//DSChannel Corresponds to StreamId Starts from 0..3
INT32 HAL_Ds_Lock_Channel(INT32 DS_Channel)
{
    INT32 QAM_Start_Addr;

#if HAL_VALIDATE
    if(DS_Channel >= DS_Id_Max)
        return HAL_FAILED;
#endif

    QAM_Start_Addr = GET_CHANNEL_ADDR(REG_PHY_GRP1QAMSTVITFDBK, DS_Channel);

    HAL_Reg_Write(QAM_Start_Addr, DS_LOCK_RE_ACQUIRE, HAL_REG_PHY);


    return HAL_SUCCESS;

}


//Does DS and US Path Hardware Initializtion
INT32 HAL_Ds_Hardware_Init(void)
{
    INT32 Status;

    ioctl(driver_fd[DrvDs], CMD_DS_HW_INIT, &Status);

    return HAL_SUCCESS;

}

//Does DS and US Path Hardware Initializtion
INT32 HAL_Us_Hardware_Init(void)
{
    INT32 Status;

    ioctl(driver_fd[DrvUs], CMD_US_HW_INIT, &Status);

    //Configure the Power
    HAL_Us_Config_Power(US_Id1, US_GET_INIT_POWER());

    return HAL_SUCCESS;

}


//Freq in HZ
//Tuner corresponds to DS StreamId starts from 0..4
//Returns: Success, Failure
INT32 HAL_Ds_Tune_Freq(INT32 Tuner, UINT32 Freq)
{
    INT32 Status;
    UINT32 Cw;
    INT32 tuneParam[0x02];

    HAL_I2c_Freq_To_Paramtr(Freq,  &Cw);

    tuneParam[0x00] = Tuner; //1st argument is TunerID
    tuneParam[0x01] = Cw; //2nd argument is Frequency CodeWord

    ioctl(driver_fd[DrvDs], CMD_DS_TUNE_FREQ, &tuneParam);

    return HAL_SUCCESS;
}

//Freq in HZ
//Tuner corresponds to DS StreamId starts from 0..4
//Returns: Success, Failure
INT32 HAL_Us_Tune_Freq(INT32 US_Id, UINT32 Freq)
{
    INT32 Status;
    UINT32 Cw;
    INT32 tuneParam[0x02];
    FLOAT frequency = Freq/MHZ;


    //FTWORD = floor(Aout * 2^32)/SYSCLK
    //FTWORD, the hex value to program the DDS for specified frequency.
    //Aout, the desired US frequency in Hz.
    //SYSCLK, use default value of 204.8 MHz.

    //FT_MUL = 2^32/SYSCLK
    //FT_WORD = floor(FT_MUL * Freq) //(--Units in MHZ)
    Cw = floor(FT_MUL * frequency);
    DPRINTE("US Frequenc Control Word 0x%08x", Cw);

    tuneParam[0x00] = US_Id; //1st argument is TunerID
    tuneParam[0x01] = Cw; //2nd argument is Frequency CodeWord

    ioctl(driver_fd[DrvUs], CMD_US_TUNE_FREQ, &tuneParam);

    return HAL_SUCCESS;
}


INT32 HAL_I2c_Freq_To_Paramtr(INT32 Freq, PUINT32 Paramtr)
{
  UINT32 TmpValue,TmpDB;
  UINT32 DB[2];
  UINT32 BB = 0;
  HAL_I2c_Get_Data_Byte(Freq, (PUINT32)&DB);
  if( HAL_I2c_Select_Band(Freq, &BB) != HAL_SUCCESS){
    DPRINTE(" BB error!!");
    return HAL_FAILED ;
  }

  TmpValue = 0;
  TmpValue  =  (DB[0]) & 0xFF ;
  TmpValue |=  ((DB[1] & 0xFF) << 8 );
  TmpValue |= 0x860000 ; // Control Byte defualt value
  TmpValue |= ( ( BB & 0xFF ) << 24 ) ;

  *Paramtr = TmpValue ;

  DPRINT(" parameter %08x \n", TmpValue);
  return HAL_SUCCESS;

}


INT32 HAL_I2c_Get_Data_Byte(INT32 Freq, PUINT32 DB)
{
  /*1.Caculate Data Byte from freq by formula
  * DataByte = ( (Freq + Intermediate Freq )/Step size
  *2.Assign upper data byte DB1 from DataByte
  *3.Assign lower data byte DB2 from DataByte
  */
  INT32 ScaleFactor;
  FLOAT frequency = Freq/1000;

  /*1.Caculate Data Byte from freq by formula
  * DataByte = ( (Freq + Intermediate Freq )/Step size
  */

  DPRINT("Freq = %d \n",Freq);
  ScaleFactor = ((frequency + 44000)/62.5);
  DPRINT("StepSize = %f \n",ScaleFactor);

  //2.Assign lower data byte DB2 from DataByte
  DB[1] = ScaleFactor & 0xFF;

  //3.Assign upper data byte DB1 from DataByte
  DB[0] = (ScaleFactor >>8) & 0xFF;

  DPRINT("DB1 = %x\n",DB[0]);
  DPRINT("DB2 = %x\n",DB[1]);

  return HAL_SUCCESS;
}


INT32 HAL_I2c_Select_Band(INT32 Frequency, PUINT32 BB)
{
  INT32 Freq = (INT32)(Frequency/(1000*1000));
  /*
  *1.If Freq is in range of 57 MHz to 159 MHz assign Band Select Byte as 41
  *2.If Freq is in range of 165 MHz to 441 MHz assign Band Select Byte as 42
  *3.If Freq is in range of 447 MHz to 861 MHz assign Band Select Byte as 44
  *4.If Freq is not in above ranges return error
  */

  //1.If Freq is in range of 57 MHz to 159 MHz assign Band Select Byte as 41

  if( (Freq >= 57) && (Freq <= 159) ){
    *BB = 0x41;
    return HAL_SUCCESS;
  }

  //2.If Freq is in range of 165 MHz to 441 MHz assign Band Select Byte as 42
  if( (Freq >= 165) && (Freq <= 441) ){
    *BB = 0x42;
    return HAL_SUCCESS;
  }

  //3.If Freq is in range of 447 MHz to 861 MHz assign Band Select Byte as 44
  if( (Freq >= 447) && (Freq <= 861) ){
    *BB = 0x44;
    DPRINT("BB : %x \n",*BB);
    return HAL_SUCCESS;
  }


  //4.If Freq is not in above ranges return error
  return HAL_FAILED;
}


INT32 HAL_Main_Control(void)
{
    // Initialize the HAL at User Level
    HAL_App_Init();

    //Create Scoket Server
    create_server();

    //Create Clients
    create_clients();

    // intiallze the DS Hardware
    HAL_Ds_Hardware_Init();

    // intiallze the US Hardware
    HAL_Us_Hardware_Init();
}


// added by ravij@sasken.com
INT32 HAL_Config_Inversion(UINT32 DSChannel)
{
    return HAL_SUCCESS;
}

INT32 HAL_Config_QAM(UINT32 DSChannel, INT32 QAM)
{
    return HAL_SUCCESS;
}


/*
*Whenever FIFO Overflow or Underflow occurs in the miniMAC FIFO
*    contents need to be cleared
*After clearing the FIFOs by setting the suitable bits, FIFO
*   need to be enabled by clearing the corresponding same Bits
*
*/

INT32  HAL_Ds_Fifo_Clear( FIFOTYPE FIFOType, FIFONO FIFONo)
{
  UINT32 REG_DS_FIFO_CTRL_Val;
  //read DS_FIFO_CTRL
  //1. Check input is correct
  if(FIFOType > FIFOTYPEMax)
  {
   DPRINTE("Out of range FIFOTYPE %x",FIFOType);
   return HAL_FAILED;
  }
  if(FIFONo > FIFONOMax)
  {
   DPRINTE("Out of range FIFONo %x",FIFONo);
   return HAL_FAILED;
  }

  //2. Read DS_FIFO_CTRL control register value
  HAL_Reg_Read(REG_DS_FIFO_CTRL, &REG_DS_FIFO_CTRL_Val, \
                                       HAL_REG_MAC_DS);
  //3. Set 1 on the corresponding FIFONumber's bit
  REG_DS_FIFO_CTRL_Val |= (0x1 << FIFONo);
  //4. Write the modfied value to DS_FIFO_CTRL register
  HAL_Reg_Write(REG_DS_FIFO_CTRL , REG_DS_FIFO_CTRL_Val, \
                                         HAL_REG_MAC_DS);
  //5.  Make a vlue which is  0 in the bit of corresponing FIFONumber .
  REG_DS_FIFO_CTRL_Val &= (~(0x1 << FIFONo));
  //6.  Write 0 in the bit of corresponing FIFONumber .
  HAL_Reg_Write(REG_DS_FIFO_CTRL , REG_DS_FIFO_CTRL_Val,\
                                         HAL_REG_MAC_DS);

  return HAL_SUCCESS;
}


INT32 HAL_Ds_Fifo_Get_Status(FIFOTYPE FIFOType, FIFONO FIFONo)
{
  UINT32 REG_DS_FIFO_STS_Val;


  //1. Check input is correct
  if(FIFOType > FIFOTYPEMax)
  {
   DPRINTE("Out of range FIFOTYPE %x",FIFOType);
   return HAL_FAILED;
  }
  if(FIFONo > FIFONOMax)
  {
   DPRINTE("Out of range FIFONo %x",FIFONo);
   return HAL_FAILED;
  }

  //2. Read DS_FIFO_CTRL control register value
  HAL_Reg_Read(REG_DS_FIFO_CTRL, &REG_DS_FIFO_STS_Val,\
                                       HAL_REG_MAC_DS);
  REG_DS_FIFO_STS_Val &= (0x1 << FIFONo);
  return REG_DS_FIFO_STS_Val;

}

INT32 HAL_Us_Config_Power(INT32 Us_Id, FLOAT Power)
{
  FLOAT Power_Digital,Power_Analog;
  INT32 Analog_Cw;
  FLOAT Pd_Result, Pd_Power_val;
  INT32 Power_Param[0x03];

  if(Power<-18.16)
  {
    Analog_Cw = 0x0;
    Power_Analog = -18.16;
  }
  else if(Power>=-18.16 && Power<-12.14)
  {
    Analog_Cw = 0x1;
    Power_Analog = -12.14;
  }
  else if(Power>=-12.14 && Power<-6.12)
  {
    Analog_Cw = 0x2;
    Power_Analog = -6.12;
  }
  else if(Power>=-6.12 && Power<-0.10)
  {
    Analog_Cw = 0x4;
    Power_Analog = -0.1;
  }
  else if(Power>=-0.10 && Power<5.92)
  {
    Analog_Cw = 0x8;
    Power_Analog = 5.92;
  }
  else if(Power>=5.92 && Power<11.94)
  {
    Analog_Cw = 0x10;
    Power_Analog = 11.94;
  }
  else if(Power>=11.94 && Power<17.96)
  {
    Analog_Cw = 0x20;
    Power_Analog = 17.96;
  }
  else if(Power>=17.96 && Power<23.98)
  {
    Analog_Cw = 0x40;
    Power_Analog = 23.98;
  }
  else if(Power>=23.98 && Power<30)
  {
    Analog_Cw = 0x80;
    Power_Analog = 30;
  }
  else
  {
    DPRINTE("Invalid Power Range");
    return HAL_FAILED;
  }

  //Pd_Result = round(TDMA_POWER_CONST * 10^(dig/20))
  //TDMA_POWER_CONST = 0.6 * 2^9

  Power_Digital = Power - Power_Analog;
  Pd_Power_val = pow(10,(Power_Digital/20));
  Pd_Result = floor((TDMA_POWER_CONST * Pd_Power_val) + 0.5);

  Power_Param[0x00] = Us_Id;
  Power_Param[0x01] = (INT32)Analog_Cw;
  Power_Param[0x02] = (INT32)Pd_Result;

  ioctl(driver_fd[DrvUs], CMD_US_PRG_POWER, &Power_Param);
  return HAL_SUCCESS;
}


