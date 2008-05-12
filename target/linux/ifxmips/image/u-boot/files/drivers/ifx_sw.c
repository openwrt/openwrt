/*
 * DANUBE internal switch ethernet driver.
 *
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */


#include <common.h>

#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_NET_MULTI) \
    && defined(CONFIG_DANUBE_SWITCH)

#include <malloc.h>
#include <net.h>
#include <asm/danube.h>
#include <asm/addrspace.h>
#include <asm/pinstrap.h>

#define MII_MODE 1
#define REV_MII_MODE 2

#define TX_CHAN_NO   7
#define RX_CHAN_NO   6

#define NUM_RX_DESC	PKTBUFSRX                  
#define NUM_TX_DESC	8
#define MAX_PACKET_SIZE 	1536
#define TOUT_LOOP	100
#define PHY0_ADDR       1 /*fixme: set the correct value here*/ 

#define DMA_WRITE_REG(reg, value) *((volatile u32 *)reg) = (u32)value
#define DMA_READ_REG(reg, value)    value = (u32)*((volatile u32*)reg)

#define SW_WRITE_REG(reg, value)  *((volatile u32*)reg) = (u32)value  	
#define SW_READ_REG(reg, value)   value = (u32)*((volatile u32*)reg)

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 reserved		 :3;
			volatile u32 Byteoffset		 :2; 
			volatile u32 reserve             :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 DataPtr;
} danube_rx_descriptor_t;

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN                 :1;
			volatile u32 C	                 :1;
			volatile u32 Sop                 :1;
			volatile u32 Eop	         :1;
			volatile u32 Byteoffset		 :5; 
			volatile u32 reserved            :7;
			volatile u32 DataLen             :16;
		}field;

		volatile u32 word;
	}status;
	
	volatile u32 DataPtr;
} danube_tx_descriptor_t;




static danube_rx_descriptor_t rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(8)));
static danube_tx_descriptor_t tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(8)));
static int tx_num, rx_num;

int danube_switch_init(struct eth_device *dev, bd_t * bis);
int danube_switch_send(struct eth_device *dev, volatile void *packet,int length);
int danube_switch_recv(struct eth_device *dev);
void danube_switch_halt(struct eth_device *dev);
static void danube_init_switch_chip(int mode);
static void danube_dma_init(void);



int danube_switch_initialize(bd_t * bis)
{
	struct eth_device *dev;
      
#if 0
	printf("Entered danube_switch_initialize()\n");
#endif

	if (!(dev = (struct eth_device *) malloc (sizeof *dev)))
	{
		printf("Failed to allocate memory\n");
		return 0;
	}
	memset(dev, 0, sizeof(*dev));

	danube_dma_init();
	danube_init_switch_chip(REV_MII_MODE);
        
#ifdef CLK_OUT2_25MHZ
       *DANUBE_GPIO_P0_DIR=0x0000ae78;
       *DANUBE_GPIO_P0_ALTSEL0=0x00008078; 
       //joelin for Mii-1       *DANUBE_GPIO_P0_ALTSEL1=0x80000080;
       *DANUBE_GPIO_P0_ALTSEL1=0x80000000; //joelin for Mii-1 
       *DANUBE_CGU_IFCCR=0x00400010;
       *DANUBE_GPIO_P0_OD=0x0000ae78;
#endif        
       
        /*patch for 6996*/
	
	*DANUBE_RCU_RST_REQ |=1;
        mdelay(200);
	*DANUBE_RCU_RST_REQ &=(unsigned long)~1; 
	mdelay(1);
	/*while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602;
	*/
	/*while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602;
	*/
	/***************/
	sprintf(dev->name, "danube Switch");
	dev->init = danube_switch_init;
	dev->halt = danube_switch_halt;
	dev->send = danube_switch_send;
	dev->recv = danube_switch_recv;

	eth_register(dev);

#if 0
	printf("Leaving danube_switch_initialize()\n");
#endif
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
	while((*DANUBE_PPE_ETOP_MDIO_ACC)&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8003840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8005840F;
	//while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	//*DANUBE_PPE_ETOP_MDIO_ACC =0x8006840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8007840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8008840F;
	while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x8001840F;
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
	*DANUBE_PPE_ETOP_MDIO_ACC =0x80123602; 
#ifdef CLK_OUT2_25MHZ         
        while(*DANUBE_PPE_ETOP_MDIO_ACC&0x80000000);
        *DANUBE_PPE_ETOP_MDIO_ACC =0x80334000;
#endif

	return 1;
}

int danube_switch_init(struct eth_device *dev, bd_t * bis)
{
	int i;

	tx_num=0;
	rx_num=0;
	
		/* Reset DMA 
		 */
//	serial_puts("i \n\0");

       *DANUBE_DMA_CS=RX_CHAN_NO;
       *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
       *DANUBE_DMA_CPOLL= 0x80000040;
       /*set descriptor base*/
       *DANUBE_DMA_CDBA=(u32)rx_des_ring;
       *DANUBE_DMA_CDLEN=NUM_RX_DESC;
       *DANUBE_DMA_CIE = 0;
       *DANUBE_DMA_CCTRL=0x30000;
	 	
       *DANUBE_DMA_CS=TX_CHAN_NO;
       *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
       *DANUBE_DMA_CPOLL= 0x80000040;
       *DANUBE_DMA_CDBA=(u32)tx_des_ring;
       *DANUBE_DMA_CDLEN=NUM_TX_DESC;  
       *DANUBE_DMA_CIE = 0;
       *DANUBE_DMA_CCTRL=0x30100;
	
	for(i=0;i < NUM_RX_DESC; i++)
	{
		danube_rx_descriptor_t * rx_desc = KSEG1ADDR(&rx_des_ring[i]);
		rx_desc->status.word=0;	
		rx_desc->status.field.OWN=1;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;   /* 1536  */	
		rx_desc->DataPtr=(u32)KSEG1ADDR(NetRxPackets[i]);
	}

	for(i=0;i < NUM_TX_DESC; i++)
	{
		danube_tx_descriptor_t * tx_desc = KSEG1ADDR(&tx_des_ring[i]);
		memset(tx_desc, 0, sizeof(tx_des_ring[0]));
	}
		/* turn on DMA rx & tx channel
		 */
	 *DANUBE_DMA_CS=RX_CHAN_NO;
	 *DANUBE_DMA_CCTRL|=1;/*reset and turn on the channel*/
	
	return 0;
}

void danube_switch_halt(struct eth_device *dev)
{
        int i; 
		for(i=0;i<8;i++)
	{
	   *DANUBE_DMA_CS=i;
	   *DANUBE_DMA_CCTRL&=~1;/*stop the dma channel*/
	}
//	udelay(1000000);
}

int danube_switch_send(struct eth_device *dev, volatile void *packet,int length)
{

	int                    	i;
	int 		 	res = -1;

	danube_tx_descriptor_t * tx_desc= KSEG1ADDR(&tx_des_ring[tx_num]);
	
	if (length <= 0)
	{
		printf ("%s: bad packet size: %d\n", dev->name, length);
		goto Done;
	}
	
	for(i=0; tx_desc->status.field.OWN==1; i++)
	{
		if(i>=TOUT_LOOP)
		{
			printf("NO Tx Descriptor...");
			goto Done;
		}
	}

	//serial_putc('s');

	tx_desc->status.field.Sop=1;
	tx_desc->status.field.Eop=1;
	tx_desc->status.field.C=0;
	tx_desc->DataPtr = (u32)KSEG1ADDR(packet);
	if(length<60)
		tx_desc->status.field.DataLen = 60;
	else
		tx_desc->status.field.DataLen = (u32)length;	
	
	asm("SYNC");
	tx_desc->status.field.OWN=1;
			 	
	res=length;
	tx_num++;
        if(tx_num==NUM_TX_DESC) tx_num=0;
	*DANUBE_DMA_CS=TX_CHAN_NO;
	  
	if(!(*DANUBE_DMA_CCTRL & 1))
	*DANUBE_DMA_CCTRL|=1;
        
Done:
	return res;
}

int danube_switch_recv(struct eth_device *dev)
{

	int                    length  = 0;

	danube_rx_descriptor_t * rx_desc;
        int anchor_num=0;
	int i;
	for (;;)
	{
	        rx_desc = KSEG1ADDR(&rx_des_ring[rx_num]);

	        if ((rx_desc->status.field.C == 0) || (rx_desc->status.field.OWN == 1))
		{
		   break;
		}
		 
                
		length = rx_desc->status.field.DataLen;
		if (length)
		{		
			NetReceive((void*)KSEG1ADDR(NetRxPackets[rx_num]), length - 4);
		//	serial_putc('*');
		}
		else
		{
			printf("Zero length!!!\n");
		}

		rx_desc->status.field.Sop=0;
		rx_desc->status.field.Eop=0;
		rx_desc->status.field.C=0;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;
		rx_desc->status.field.OWN=1;
		rx_num++;
		if(rx_num==NUM_RX_DESC) rx_num=0;

	}

	return length;
}


static void danube_init_switch_chip(int mode)
{
        int i;
        /*get and set mac address for MAC*/
        static unsigned char addr[6];
        char *tmp,*end; 
	tmp = getenv ("ethaddr");
	if (NULL == tmp) {
		printf("Can't get environment ethaddr!!!\n");
	//	return NULL;
	} else {
		printf("ethaddr=%s\n", tmp);
	}
        *DANUBE_PMU_PWDCR = *DANUBE_PMU_PWDCR & 0xFFFFEFDF;
        *DANUBE_PPE32_ETOP_MDIO_CFG &= ~0x6;
        *DANUBE_PPE32_ENET_MAC_CFG = 0x187;
  
  // turn on port0, set to rmii and turn off port1.
	if(mode==REV_MII_MODE)
		{
		 *DANUBE_PPE32_ETOP_CFG = (*DANUBE_PPE32_ETOP_CFG & 0xfffffffc) | 0x0000000a;
                } 
	else if (mode == MII_MODE)
	        {
		*DANUBE_PPE32_ETOP_CFG = (*DANUBE_PPE32_ETOP_CFG & 0xfffffffc) | 0x00000008;
                }	  

	*DANUBE_PPE32_ETOP_IG_PLEN_CTRL = 0x4005ee; // set packetlen.
        *ENET_MAC_CFG|=1<<11;/*enable the crc*/
	return;
}


static void danube_dma_init(void)
{
        int i;
//	serial_puts("d \n\0");

        *DANUBE_PMU_PWDCR &=~(1<<DANUBE_PMU_DMA_SHIFT);/*enable DMA from PMU*/
		/* Reset DMA 
		 */
	*DANUBE_DMA_CTRL|=1; 
        *DANUBE_DMA_IRNEN=0;/*disable all the interrupts first*/

	/* Clear Interrupt Status Register 		 
	*/
	*DANUBE_DMA_IRNCR=0xfffff;
	/*disable all the dma interrupts*/
	*DANUBE_DMA_IRNEN=0;
	/*disable channel 0 and channel 1 interrupts*/
	
	  *DANUBE_DMA_CS=RX_CHAN_NO;
	  *DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
          *DANUBE_DMA_CPOLL= 0x80000040;
          /*set descriptor base*/
          *DANUBE_DMA_CDBA=(u32)rx_des_ring;
          *DANUBE_DMA_CDLEN=NUM_RX_DESC;
          *DANUBE_DMA_CIE = 0;
          *DANUBE_DMA_CCTRL=0x30000;
	 	
	*DANUBE_DMA_CS=TX_CHAN_NO;
	*DANUBE_DMA_CCTRL=0x2;/*fix me, need to reset this channel first?*/
        *DANUBE_DMA_CPOLL= 0x80000040;
	*DANUBE_DMA_CDBA=(u32)tx_des_ring;
        *DANUBE_DMA_CDLEN=NUM_TX_DESC;  
	*DANUBE_DMA_CIE = 0;
	*DANUBE_DMA_CCTRL=0x30100;
	/*enable the poll function and set the poll counter*/
	//*DANUBE_DMA_CPOLL=DANUBE_DMA_POLL_EN | (DANUBE_DMA_POLL_COUNT<<4);
	/*set port properties, enable endian conversion for switch*/
	*DANUBE_DMA_PS=0;
	*DANUBE_DMA_PCTRL|=0xf<<8;/*enable 32 bit endian conversion*/

	return;
}





#endif
