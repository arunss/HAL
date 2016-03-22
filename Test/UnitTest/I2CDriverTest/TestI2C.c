/*
Test code : Added by AShah
 */



/*------------------------------------------------------------------------------
* PURPOSE
*  Brief Description of the overall implementation strategy used in the module.
*  Platform specific implementation details and optimization strategies (if any)
*  should be described.
*
* HISTORY (Version, Date, Author, Note)
*------------------------------------------------------------------------------
*   Ver.0.01   13/5/08  AShah     Intial Draft
*
******************************************************************************/

#include <linux/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "HAL_Types.h"

#define I2C_TEST1   (0x01)
#define I2C_TEST2   (0x02)
#define SPI_TEST1   (0x03)
#define SPI_TEST2   (0x04)
#define SPI_TEST3   (0x05)


INT32 Data[2048];
INT32 Data_2[12];
INT32 Data_3[12];

#ifndef MSDOS
int getch()
{
  int charact, ch;
  while ((ch=getchar()) != '\n') {charact=ch;}
  return charact;
}
#endif

INT32 DOCS_HAL_I2C_FreqToParamtr(float Freq, PUINT32 Paramtr);
INT32 DOCS_HAL_I2C_GetDataByte(float Freq, PUINT32 DB);
INT32 DOCS_HAL_I2C_SelectBand(float Freq, PUINT32 BB);

int main(void)
{

  int fd;
  int valid = 0x00;
  int ioctl_command;
  int data = 0x00;
  char ch;
  float freq;
  int idx=0;

  fd = open("/var/I2C_Probe", O_RDWR);
  if (fd == -1 ) { printf("err in open /var/I2CProbe \n "); return;}

  valid = 0;

  while (!valid) {

    fprintf(stderr, "\n --------------- M E N U ------------ \n");
    fprintf(stderr, "\n Test Case 0. ********EXIT*******.");
    fprintf(stderr, "\n Test Case 1: I2C - tunner1 Prgming .");
    fprintf(stderr, "\n Test Case 2: I2C - tunner2 Prgming .");
    fprintf(stderr, "\n Test Case 3: SPI - Tx DDS .");
    fprintf(stderr, "\n Test Case 4: SPI - Rx DDS .");
    fprintf(stderr, "\n Test Case 5: SPI - Tx SpiPA .");
    fprintf(stderr, "\n Choose above one of options [ ]\b\b");

    //ch = getch();
    scanf ("%d", &ch);

    //printf("choice is %c\n", ch);
    //ioctl_command = ch - '0';
    ioctl_command = ch;

    switch(ch) {

      case 0 :
        valid = 1;
        break;

      case I2C_TEST1:
      case I2C_TEST2:
        printf("Enter Frequency in MHz:  ");
        scanf ("%f", &freq);
        DOCS_HAL_I2C_FreqToParamtr(freq , &data);
        printf("\ndata %x \n", data );
            Data[0x00] = 0x01; //Size
            Data[0x01] = data;

        ioctl(fd,ioctl_command, &Data, Data[0x00]+1);
        break;

      case SPI_TEST1: //SPI Write
        //25Mhz part1
        Data[0x00] = 0x07; //Argument Size
        Data[0x01] = 0x0091;
        Data[0x02] = 0x0102;
        Data[0x03] = 0x0200;
        Data[0x04] = 0x0300;
        Data[0x05] = 0x0440;
        Data[0x06] = 0x051f;
        Data[0x07] = 0x0609;
        ioctl(fd,ioctl_command, &Data, Data[0x00]);
        //part2
        Data_2[0] = 0x01;
        Data_2[1] = 0x0142;
        ioctl(fd,ioctl_command, &Data_2, Data_2[0x00]);


        break;

      case SPI_TEST2: //SPI Read
        Data[0x00] = 0x07; //Argument Size
        Data[0x01] = 0x00;
        Data[0x02] = 0x00;
        Data[0x03] = 0x00;
        Data[0x04] = 0x00;
        Data[0x05] = 0x00;
        Data[0x06] = 0x00;
        Data[0x07] = 0x00;
        ioctl(fd,ioctl_command, &Data, Data[0x00]+1);
        for(idx=0; idx<Data[0x00]; idx++)
          printf("Data %d is 0x%08x\n", idx, Data[idx+1]);
        break;

      case SPI_TEST3:
        /*Data[0x00] = 0x08; //Size
        Data[0x01] = 0x0091;
        Data[0x01] = 0x0102;
        Data[0x01] = 0x0200;
        Data[0x01] = 0x0300;
        Data[0x01] = 0x0440;
        Data[0x01] = 0x051f;
        Data[0x01] = 0x0609;

        ioctl(fd,ioctl_command, &Data, Data[0x00]+1);*/
        //Channel-0 power amplifier value
        Data_3[0] = 0x01;
        Data_3[1] = 0x04;//PowerVal
        ioctl(fd,ioctl_command, &Data_3, Data_3[0x00]+1);

        break;

    } // end of switch
  } // end of while


  close(fd);
  return 0;
}


INT32 DOCS_HAL_I2C_FreqToParamtr(float Freq, PUINT32 Paramtr)
{
  UINT32 TmpValue,TmpDB;
  UINT32 DB[2];
  UINT32 BB = 0;
  DOCS_HAL_I2C_GetDataByte(Freq, &DB[0]);
  if( DOCS_HAL_I2C_SelectBand(Freq, &BB) != HAL_SUCCESS){
  printf(" BB error!!");
  return HAL_FAILED ;
  }

  TmpValue = 0;
  TmpValue  =  (DB[0]) & 0xFF ;
  TmpValue |=  ((DB[1] & 0xFF) << 8 );
  TmpValue |= 0x860000 ; // Control Byte defualt value
  TmpValue |= ( ( BB & 0xFF ) << 24 ) ;

  printf(" parameter %08x \n", TmpValue);

  *Paramtr = TmpValue ;
  return HAL_SUCCESS;

}


INT32 DOCS_HAL_I2C_GetDataByte(float Freq, PUINT32 DB)
{
  /*1.Caculate Data Byte from freq by formula
  * DataByte = ( (Freq + Intermediate Freq )/Step size
  *2.Assign upper data byte DB1 from DataByte
  *3.Assign lower data byte DB2 from DataByte
  */
  float DataByte;

  /*1.Caculate Data Byte from freq by formula
  * DataByte = ( (Freq + Intermediate Freq )/Step size
  */

  printf("Freq = %f \n",Freq);
  DataByte = (Freq*1000+44000)/62.5;
  //DataByte = (Freq*1000+44000)/62;
  printf("StepSize = %f \n",DataByte);

  //2.Assign upper data byte DB1 from DataByte
  DB[0]=( (INT32)DataByte / 0x100 );
  printf("DB1 = %x\n",DB[0]);

  //3.Assign lower data byte DB2 from DataByte
  DB[1]= ( DataByte - ( DB[0] * 0x100)) ;
  printf("DB2 = %x\n",DB[1]);

  return HAL_SUCCESS;
}


INT32 DOCS_HAL_I2C_SelectBand(float Freq, PUINT32 BB)
{

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
    printf("BB : %x \n",*BB);
    return HAL_SUCCESS;
  }


  //4.If Freq is not in above ranges return error
  return HAL_FAILED;
}



