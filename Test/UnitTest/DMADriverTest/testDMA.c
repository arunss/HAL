/*
Test code : Added by AShah
 */



#include <linux/mman.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define DMA_TEST1    (0x01)
#define DMA_TEST2    (0x02)
#define DMA_TEST3    (0x03)
#define DMA_TEST4    (0x04)

#ifndef MSDOS
int getch()
{
    int charact, ch;
    while ((ch=getchar()) != '\n') {charact=ch;}
    return charact;
}
#endif

int main(void)
{

  int fd;
  int valid = 0x00;
    int ioctl_command;
    int data = 0x00;
    char ch;

  fd = open("/var/DMAProb", O_RDWR);
  if (fd == -1 ) { printf("err in open /var/DMAProb \n "); return;}

    valid = 0;

    while (!valid) {


      fprintf(stderr, "\n   -------------- M E N U ------------ \n");
      fprintf(stderr, "\n    Test Case  0. ********EXIT*******.");
      fprintf(stderr, "\n    Test Case  1: Register Mode DMA 8.");
      fprintf(stderr, "\n    Test Case  2: Descriptor Mode DMA 8.");
      fprintf(stderr, "\n    Test Case  3: Register Mode DMA 9.");
      fprintf(stderr, "\n    Test Case  4: Descriptor Mode DMA 9.");
      fprintf(stderr, "\n    Test Case  5: Multiple Descriptor DMA 8.");
      fprintf(stderr, "\n    Test Case  6: Multiple Descriptor DMA 9.");
      fprintf(stderr, "\n    Test Case  7: Register 8 - 9 - 8.");
      fprintf(stderr, "\n    Test Case  8: Descriptor 8 - 9 - 8.");
      fprintf(stderr, "\n    Test Case  9: Multiple Register DMA 8.");
      fprintf(stderr, "\n    Test Case 10: Continuous Desc US DMA 8.");
      fprintf(stderr, "\n    Test Case 11: Continuous Desc US DMA 9.");
      fprintf(stderr, "\n    Choose above one of options [ ]\b\b");

      //ch = getch();
      scanf("%d",&ioctl_command);

      //printf("choice is %c\n", ch);
      //ioctl_command = ch - '0';

      switch(ioctl_command) {

            case 0 :
                valid = 1;
                break;


            default :
              if( (ioctl_command < 1) || (ioctl_command > 13))
          {

              printf("out of range\n");
              break;
          }
                ioctl(fd,ioctl_command, &data, 0x01);
                break;
      } // end of switch
    }  // end of while

  close(fd);
  return 0;
}
