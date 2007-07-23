#ifndef AMAZON_DMA_H
#define AMAZON_DMA_H

#define RCV_INT          1
#define TX_BUF_FULL_INT  2
#define TRANSMIT_CPT_INT 4

#define QOS_DEFAULT_WGT 0x7fffffffUL;


enum attr_t{
   TX=0,
   RX=1,
   RESERVED=2,
   DEFAULT=3,
   
};

#ifdef CONFIG_CPU_LITTLE_ENDIAN
typedef struct rx_desc{
   u32 data_length:16;
   volatile u32 reserved:7;
   volatile u32 byte_offset:2; 
   volatile u32 Burst_length_offset:3;
   volatile u32 EoP:1;
   volatile u32 Res:1;
   volatile u32 C:1;
   volatile u32 OWN:1;
   volatile u32 Data_Pointer;
   /*fix me:should be 28 bits here, 32 bits just for host simulatiuon purpose*/ 
}_rx_desc; 


typedef struct tx_desc{
   volatile u32 data_length:16;
   volatile u32 reserved1:7;
   volatile u32 byte_offset:5;   
   volatile u32 EoP:1;
   volatile u32 SoP:1;
   volatile u32 C:1;
   volatile u32 OWN:1;
   volatile u32 Data_Pointer;//fix me:should be 28 bits here
}_tx_desc;
#else //BIG
typedef struct rx_desc{
	union
	{
		struct
		{
			volatile u32 OWN                 	:1;
			volatile u32 C	                 	:1;
			volatile u32 SoP                 	:1;
			volatile u32 EoP	         	:1;
			volatile u32 Burst_length_offset	:3;
			volatile u32 byte_offset	 	:2; 
			volatile u32 reserve             	:7;
			volatile u32 data_length             	:16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 Data_Pointer;
}_rx_desc; 


typedef struct tx_desc{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 SoP                 :1;
			volatile u32 EoP	         :1;
			volatile u32 byte_offset	 :5; 
			volatile u32 reserved            :7;
			volatile u32 data_length         :16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 Data_Pointer;
}_tx_desc;

#endif //ENDIAN

struct dma_channel_info{
  /*filled by driver, optional*/  
    enum attr_t attr;/*TX or RX*/
    int weight;
    int desc_num;
    int packet_size;
    int control;/*on or off*/ 
   
    int desc_base;
    int status;
};

typedef struct dma_channel_info _dma_channel_info;

struct dma_device_info{
   /*variables*/ 
   /*filled by driver, compulsary*/
    char device_name[15];
    enum attr_t attr;/*default or else*/
    int tx_burst_len;
    int rx_burst_len;   
 
    int logic_rx_chan_base;
    int logic_tx_chan_base;
    u8 on_ch_bit;
   /*filled by driver, optional*/
    int  weight;
    int  current_tx_chan;
    int  current_rx_chan;
    int  num_tx_chan;
    int  num_rx_chan; 
    struct dma_channel_info tx_chan[2];
    struct dma_channel_info rx_chan[4]; 
    
    /*functions, optional*/
    u8* (*buffer_alloc)(int len,int* offset, void** opt);
    int (*buffer_free)(u8* dataptr, void* opt);
    int (*intr_handler)(struct dma_device_info* info, int status);
    /*set by device, clear by dma*/
    int ack;
    void * priv;		/* used by peripheral driver only */
};
typedef struct dma_device_info _dma_device_info;

int dma_device_register(struct dma_device_info* info);

int dma_device_unregister(struct dma_device_info* info);

int dma_device_read(struct dma_device_info* info, u8** dataptr, void** opt);

int dma_device_write(struct dma_device_info* info, u8* dataptr, int len, void* opt);

int dma_device_update(struct dma_device_info* info);

void dma_device_update_rx(struct dma_device_info* dma_dev);

void dma_device_update_tx(struct dma_device_info* dma_dev);

void register_handler_sim(int (*handler)(int));
#endif /* AMAZON_DMA_H */
