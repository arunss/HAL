#ifndef CREPEHAL
#define CREPEHAL

#define DOC_DS_MAC_REGISTER_BASE_ADDRESS    0xf0020000
#define TEST_BUFFER_BASE_ADDRESS   0x8000d000 //0x80000000
#define TEST_BUFFER_SIZE    4096
#define NUMBER_OF_TEST_BUFFER    128//1024
#define DOC_PHY_REGISTER_BASE_ADDRESS 0xf2000000
#ifndef dma
//#define dma
#endif
#ifdef dma
#define DMA_REGISTER_BASE_ADDRESS     0xf0030000
#endif


#endif
