/*
Test code : Added by AShah : Upstream/downstream test paths
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

//Custom Header Files
#include "HAL_Interface.h"





INT32 Test_Main(void);
INT32 Test_Ds_Path(void);
INT32 Test_Us_Path(void);
INT32 HAL_Us_Config_Power(INT32 Us_Id, FLOAT Power);

main()
{
    HAL_Main_Control();

    Test_Main();

    listen_clients();

}



INT32 Test_Main(void)
{
  INT32 TestNo;
  // Insert the Test Case Code Here
  printf("\n");
  printf(" BSP HAL Test\n");
  printf("---------------------------------\n");
  printf("1.  DS Path Test \n");
  printf("2.  US Path Test\n");
  printf("3.  DS and US Both Path Test\n");
  printf("\n");

  printf("Enter the TestCase  No : ");
  scanf("%d",&TestNo);
  printf("\n");

  if (TestNo<0||TestNo>3)
  {
    printf("Invalid TestNo");
    return 0;
  }

  switch(TestNo)
  {
    case 1:
      Test_Ds_Path();
      break;

    case 2:
      Test_Us_Path();
      break;

    case 3:
      Test_Us_Path();
      Test_Ds_Path();
      break;
  }
}


INT32 Test_Ds_Path(void)
{
  INT32 Ds_Freq=0x00;

  printf("Enter the DownStream Frequency for All Channels in HZ: ");
  scanf("%d",&Ds_Freq);
  printf("\n");
  printf("Ds_Freq : %d\n",&Ds_Freq);

  HAL_Ds_Disable(DS_Id2);
  HAL_Ds_Disable(DS_Id1);

  HAL_Ds_Tune_Freq(DS_Id2, Ds_Freq);
  HAL_Ds_Tune_Freq(DS_Id1, Ds_Freq);

  HAL_Ds_Enable(DS_Id2);
  HAL_Ds_Enable(DS_Id1);

  return 0x00;
}

INT32 Test_Us_Path(void)
{
  INT32 Us_Freq=0x00;

  printf("Enter the UpStream Frequency for All Channels in HZ: ");
  scanf("%d",&Us_Freq);
  printf("\n");
  printf("Us_Freq : %d\n",&Us_Freq);

  HAL_Us_Tune_Freq(US_Id1, Us_Freq);
  HAL_Us_Tune_Freq(US_Id2, Us_Freq);

  HAL_Us_Enable(US_Id2);
  HAL_Us_Enable(US_Id1);

  return 0x00;
}
