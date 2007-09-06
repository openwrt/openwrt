#ifndef DMA_CORE_H 
#define DMA_CORE_H 

#define AMAZON_DMA_REG32(reg_num)  *((volatile u32*)(reg_num))
#define AMAZON_DMA_CH_STEP            4

#define COMB_ISR_RX_MASK 0xfe000000
#define COMB_ISR_TX_MASK 0x01f00000


#define DMA_OWN   1
#define CPU_OWN   0   
#define DMA_MAJOR 250

//Descriptors
#define DMA_DESC_OWN_CPU		0x0
#define DMA_DESC_OWN_DMA		0x80000000
#define DMA_DESC_CPT_SET		0x40000000
#define DMA_DESC_SOP_SET		0x20000000
#define DMA_DESC_EOP_SET		0x10000000

#define switch_rx_chan_base 0
#define switch_tx_chan_base 7
#define switch2_rx_chan_base 2
#define switch2_tx_chan_base 8
#define TPE_rx_chan_base    4
#define TPE_tx_chan_base    9
#define DPLus2FPI_rx_chan_base  6
#define DPLus2FPI_tx_chan_base  11 

#define RX_CHAN_NUM 7
#define TX_CHAN_NUM 5
#define CHAN_TOTAL_NUM       (RX_CHAN_NUM+TX_CHAN_NUM)
#define DEFAULT_OFFSET 20
#define DESCRIPTOR_SIZE 8

typedef struct dev_list{
   struct dma_device_info* dev;
   int weight;
   struct dev_list* prev;
   struct dev_list* next;
}dev_list; 

typedef struct channel_info{
   char device_name[16];
   int occupied;
   enum attr_t attr;  
   int current_desc;
   int weight;
   int default_weight;
   int desc_num;
   int burst_len;
   int desc_len;
   int desc_ofst;
   int packet_size;
   int offset_from_base;
   int control;
   void* opt[DEFAULT_OFFSET];
   u8* (*buffer_alloc)(int len,int* offset, void** opt);
   int (*buffer_free)(u8* dataptr,void* opt);
   int (*intr_handler)(struct dma_device_info* info,int status);

   struct dma_device_info* dma_dev;
}channel_info;



#endif

