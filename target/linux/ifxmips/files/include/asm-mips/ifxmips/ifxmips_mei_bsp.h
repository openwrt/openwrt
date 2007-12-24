/******************************************************************************
**
** FILE NAME    : ifxmips_mei_bsp.h
** PROJECT      : Danube
** MODULES      : MEI
**
** DATE         : 1 Jan 2006
** AUTHOR       : TC Chen
** DESCRIPTION  : MEI Driver
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Version $Date      $Author     $Comment
*******************************************************************************/
#ifndef _IFXMIPS_MEI_BSP_H_
#define _IFXMIPS_MEI_BSP_H_

/***	Register address offsets, relative to MEI_SPACE_ADDRESS ***/
#define MEI_DATA_XFR_OFFSET				(0x0000)
#define	MEI_VERSION_OFFSET				(0x0004)
#define	MEI_ARC_GP_STAT_OFFSET				(0x0008)
#define MEI_DATA_XFR_STAT_OFFSET			(0x000C)
#define	MEI_XFR_ADDR_OFFSET				(0x0010)
#define MEI_MAX_WAIT_OFFSET				(0x0014)
#define	MEI_TO_ARC_INT_OFFSET				(0x0018)
#define	ARC_TO_MEI_INT_OFFSET				(0x001C)
#define	ARC_TO_MEI_INT_MASK_OFFSET			(0x0020)
#define	MEI_DEBUG_WAD_OFFSET				(0x0024)
#define MEI_DEBUG_RAD_OFFSET				(0x0028)
#define	MEI_DEBUG_DATA_OFFSET				(0x002C)
#define	MEI_DEBUG_DEC_OFFSET				(0x0030)
#define MEI_CONFIG_OFFSET				(0x0034)
#define	MEI_RST_CONTROL_OFFSET				(0x0038)
#define	MEI_DBG_MASTER_OFFSET				(0x003C)
#define	MEI_CLK_CONTROL_OFFSET				(0x0040)
#define	MEI_BIST_CONTROL_OFFSET				(0x0044)
#define	MEI_BIST_STAT_OFFSET				(0x0048)
#define MEI_XDATA_BASE_SH_OFFSET			(0x004c)
#define MEI_XDATA_BASE_OFFSET				(0x0050)
#define MEI_XMEM_BAR_BASE_OFFSET			(0x0054)
#define MEI_XMEM_BAR0_OFFSET				(0x0054)
#define MEI_XMEM_BAR1_OFFSET				(0x0058)
#define MEI_XMEM_BAR2_OFFSET				(0x005C)
#define MEI_XMEM_BAR3_OFFSET				(0x0060)
#define MEI_XMEM_BAR4_OFFSET				(0x0064)
#define MEI_XMEM_BAR5_OFFSET				(0x0068)
#define MEI_XMEM_BAR6_OFFSET				(0x006C))
#define MEI_XMEM_BAR7_OFFSET				(0x0070)
#define MEI_XMEM_BAR8_OFFSET				(0x0074)
#define MEI_XMEM_BAR9_OFFSET				(0x0078)
#define MEI_XMEM_BAR10_OFFSET				(0x007C)
#define MEI_XMEM_BAR11_OFFSET				(0x0080)
#define MEI_XMEM_BAR12_OFFSET				(0x0084)
#define MEI_XMEM_BAR13_OFFSET				(0x0088)
#define MEI_XMEM_BAR14_OFFSET				(0x008C)
#define MEI_XMEM_BAR15_OFFSET				(0x0090)
#define MEI_XMEM_BAR16_OFFSET				(0x0094)

#define WHILE_DELAY 20000
/*
**	Define where in ME Processor's memory map the Stratify chip lives
*/

#define MAXSWAPSIZE      	8 * 1024	//8k *(32bits)

//      Mailboxes
#define MSG_LENGTH		16	// x16 bits
#define YES_REPLY      	 	1
#define NO_REPLY         	0

#define CMV_TIMEOUT		1000	//jiffies

//  Block size per BAR
#define SDRAM_SEGMENT_SIZE	(64*1024)
// Number of Bar registers
#define MAX_BAR_REGISTERS	(17)

#define XDATA_REGISTER		(15)

#define IFXMIPS_MEI_IOCTL_CMV_WINHOST		IFX_ADSL_IOC_CMV_WINHOST

#define IFXMIPS_MEI_IOCTL_CMV_READ		IFX_ADSL_IOC_CMV_READ
#define IFXMIPS_MEI_IOCTL_CMV_WRITE		IFX_ADSL_IOC_CMV_WRITE

#define IFXMIPS_MEI_IOCTL_GET_BASE_ADDRESS	IFX_ADSL_IOC_GET_BASE_ADDRESS

// ARC register addresss
#define ARC_STATUS				0x0
#define ARC_LP_START				0x2
#define ARC_LP_END				0x3
#define ARC_DEBUG				0x5
#define ARC_INT_MASK				0x10A

#define IRAM0_BASE 				(0x00000)
#define IRAM1_BASE 				(0x04000)
#define BRAM_BASE  				(0x0A000)

#define ADSL_BASE				(0x20000)
#define CRI_BASE				(ADSL_BASE + 0x11F00)
#define CRI_CCR0				(CRI_BASE + 0x00)
#define CRI_RST					(CRI_BASE + 0x04*4)
#define ADSL_DILV_BASE 				(ADSL_BASE+0x20000)

//
#define IRAM0_ADDR_BIT_MASK   0xFFF
#define IRAM1_ADDR_BIT_MASK   0xFFF
#define BRAM_ADDR_BIT_MASK    0xFFF
#define RX_DILV_ADDR_BIT_MASK 0x1FFF

/***  Bit definitions ***/

#define FALSE	0
#define TRUE	1
#define BIT0	1<<0
#define BIT1	1<<1
#define BIT2	1<<2
#define BIT3	1<<3
#define BIT4	1<<4
#define BIT5	1<<5
#define BIT6	1<<6
#define BIT7	1<<7
#define BIT8	1<<8
#define BIT9	1<<9
#define BIT10 	1<<10
#define BIT11	1<<11
#define BIT12	1<<12
#define BIT13	1<<13
#define BIT14	1<<14
#define BIT15	1<<15
#define BIT16 	1<<16
#define BIT17	1<<17
#define BIT18	1<<18
#define BIT19	1<<19
#define BIT20	1<<20
#define BIT21	1<<21
#define BIT22	1<<22
#define BIT23	1<<23
#define BIT24	1<<24
#define BIT25	1<<25
#define BIT26	1<<26
#define BIT27	1<<27
#define BIT28	1<<28
#define BIT29	1<<29
#define BIT30	1<<30
#define BIT31	1<<31

// CRI_CCR0 Register definitions
#define CLK_2M_MODE_ENABLE			BIT6
#define	ACL_CLK_MODE_ENABLE			BIT4
#define FDF_CLK_MODE_ENABLE			BIT2
#define STM_CLK_MODE_ENABLE			BIT0

// CRI_RST Register definitions
#define FDF_SRST				BIT3
#define MTE_SRST				BIT2
#define FCI_SRST				BIT1
#define AAI_SRST				BIT0

//      MEI_TO_ARC_INTERRUPT Register definitions
#define	MEI_TO_ARC_INT1			BIT3
#define	MEI_TO_ARC_INT0			BIT2
#define MEI_TO_ARC_CS_DONE		BIT1	//need to check
#define	MEI_TO_ARC_MSGAV		BIT0

//      ARC_TO_MEI_INTERRUPT Register definitions
#define	ARC_TO_MEI_INT1			BIT8
#define	ARC_TO_MEI_INT0			BIT7
#define	ARC_TO_MEI_CS_REQ		BIT6
#define	ARC_TO_MEI_DBG_DONE		BIT5
#define	ARC_TO_MEI_MSGACK		BIT4
#define	ARC_TO_MEI_NO_ACCESS		BIT3
#define	ARC_TO_MEI_CHECK_AAITX		BIT2
#define	ARC_TO_MEI_CHECK_AAIRX		BIT1
#define	ARC_TO_MEI_MSGAV		BIT0

//      ARC_TO_MEI_INTERRUPT_MASK Register definitions
#define	GP_INT1_EN			BIT8
#define	GP_INT0_EN			BIT7
#define	CS_REQ_EN			BIT6
#define	DBG_DONE_EN			BIT5
#define	MSGACK_EN			BIT4
#define	NO_ACC_EN			BIT3
#define	AAITX_EN			BIT2
#define	AAIRX_EN			BIT1
#define	MSGAV_EN			BIT0

#define	MEI_SOFT_RESET			BIT0

#define	HOST_MSTR			BIT0

#define JTAG_MASTER_MODE		0x0
#define MEI_MASTER_MODE			HOST_MSTR

//      MEI_DEBUG_DECODE Register definitions
#define MEI_DEBUG_DEC_MASK		(0x3)
#define MEI_DEBUG_DEC_AUX_MASK		(0x0)
#define MEI_DEBUG_DEC_DMP1_MASK		(0x1)
#define MEI_DEBUG_DEC_DMP2_MASK		(0x2)
#define MEI_DEBUG_DEC_CORE_MASK         (0x3)

#define AUX_STATUS			(0x0)
//      ARC_TO_MEI_MAILBOX[11] is a special location used to indicate
//      page swap requests.
#define MEI_TO_ARC_MAILBOX		(0xDFD0)
#define MEI_TO_ARC_MAILBOXR		(MEI_TO_ARC_MAILBOX + 0x2C)

#define	ARC_TO_MEI_MAILBOX		(0xDFA0)
#define ARC_MEI_MAILBOXR		(ARC_TO_MEI_MAILBOX + 0x2C)

// Codeswap request messages are indicated by setting BIT31
#define OMB_CODESWAP_MESSAGE_MSG_TYPE_MASK	(0x80000000)

// Clear Eoc messages received are indicated by setting BIT17 
#define OMB_CLEAREOC_INTERRUPT_CODE	(0x00020000)

/*
**	Swap page header
*/
//      Page must be loaded at boot time if size field has BIT31 set
#define BOOT_FLAG	(BIT31)
#define BOOT_FLAG_MASK	~BOOT_FLAG

#define FREE_RELOAD		1
#define FREE_SHOWTIME		2
#define FREE_ALL		3

// marcos
#define	IFXMIPS_WRITE_REGISTER_L(data,addr)	do{ *((volatile u32*)(addr)) = (u32)(data);} while (0)
#define IFXMIPS_READ_REGISTER_L(addr) 	(*((volatile u32*)(addr)))
#define SET_BIT(reg, mask)                  reg |= (mask)
#define CLEAR_BIT(reg, mask)                reg &= (~mask)
#define CLEAR_BITS(reg, mask)               CLEAR_BIT(reg, mask)
#define SET_BITS(reg, mask)                 SET_BIT(reg, mask)
#define SET_BITFIELD(reg, mask, off, val)   {reg &= (~mask); reg |= (val << off);}

#define ALIGN_SIZE			    ( 1L<<10 )	//1K size align
#define MEM_ALIGN(addr)		    (((addr) + ALIGN_SIZE - 1) & ~ (ALIGN_SIZE -1) )

// swap marco
#define MEI_HALF_WORD_SWAP(data) {data = ((data & 0xffff)<<16) + ((data & 0xffff0000)>>16);}
#define MEI_BYTE_SWAP(data) {data = ((data & 0xff)<<24) + ((data & 0xff00)<<8)+ ((data & 0xff0000)>>8)+ ((data & 0xff000000)>>24);}

//      Swap page header describes size in 32-bit words, load location, and image offset
//      for program and/or data segments
typedef struct _arc_swp_page_hdr {
	u32 p_offset;		//Offset bytes of progseg from beginning of image
	u32 p_dest;		//Destination addr of progseg on processor
	u32 p_size;		//Size in 32-bitwords of program segment
	u32 d_offset;		//Offset bytes of dataseg from beginning of image
	u32 d_dest;		//Destination addr of dataseg on processor
	u32 d_size;		//Size in 32-bitwords of data segment
} ARC_SWP_PAGE_HDR;

/*
**	Swap image header
*/
#define GET_PROG	0	//      Flag used for program mem segment
#define GET_DATA	1	//      Flag used for data mem segment

//      Image header contains size of image, checksum for image, and count of
//      page headers. Following that are 'count' page headers followed by
//      the code and/or data segments to be loaded
typedef struct _arc_img_hdr {
	u32 size;		//      Size of binary image in bytes
	u32 checksum;		//      Checksum for image
	u32 count;		//      Count of swp pages in image
	ARC_SWP_PAGE_HDR page[1];	//      Should be "count" pages - '1' to make compiler happy
} ARC_IMG_HDR;

typedef struct smmu_mem_info {
	int type;
	unsigned long nCopy;
	unsigned long size;
	unsigned char *address;
	unsigned char *org_address;
} smmu_mem_info_t;

typedef struct ifxmips_mei_device_private {
	int modem_ready;
	int arcmsgav;
	int cmv_reply;
	int cmv_waiting;
	// Mei to ARC CMV count, reply count, ARC Indicator count       
	int indicator_count;
	int cmv_count;
	int reply_count;
	unsigned long image_size;
	int nBar;
	u16 Recent_indicator[MSG_LENGTH];

	u16 CMV_RxMsg[MSG_LENGTH] __attribute__ ((aligned (4)));

	smmu_mem_info_t adsl_mem_info[MAX_BAR_REGISTERS];
	ARC_IMG_HDR *img_hdr;
	//  to wait for arc cmv reply, sleep on wait_queue_arcmsgav;
	wait_queue_head_t wait_queue_arcmsgav;
	wait_queue_head_t wait_queue_modemready;
	MEI_mutex_t mei_cmv_sema;
} ifxmips_mei_device_private_t;

#endif //_IFXMIPS_MEI_BSP_H_
