#ifndef		_AMAZON_MEI_H
#define        	_AMAZON_MEI_H
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "amazon_mei_app.h"

#define AMAZON_MEI_DEBUG_ON
#define AMAZON_MEI_CMV_EXTRA

#define AMAZON_MEI_MAJOR	106

/*
**	Define where in ME Processor's memory map the Stratify chip lives
*/
#define MEI_SPACE_ACCESS	0xB0100C00

#define MAXSWAPSIZE      	8 * 1024	//8k *(32bits)
//#define AMAZON_ADSL_IMAGESIZE  16*1024    	// 16k * (32bits)


//	Mailboxes
#define MSG_LENGTH		16     	// x16 bits
#define YES_REPLY      	 	1
#define NO_REPLY         	0

#define CMV_TIMEOUT		100	//jiffies
#define MIB_INTERVAL		10000 	//msec

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


/***	Register address offsets, relative to MEI_SPACE_ADDRESS ***/
#define MEI_DATA_XFR				(0x0000 + MEI_SPACE_ACCESS)
#define	MEI_VERSION				(0x0200 + MEI_SPACE_ACCESS)	
#define	ARC_GP_STAT				(0x0204 + MEI_SPACE_ACCESS)	
#define	MEI_XFR_ADDR				(0x020C + MEI_SPACE_ACCESS)	
#define	MEI_TO_ARC_INT				(0x021C + MEI_SPACE_ACCESS)
#define	ARC_TO_MEI_INT				(0x0220 + MEI_SPACE_ACCESS)	
#define	ARC_TO_MEI_INT_MASK			(0x0224 + MEI_SPACE_ACCESS)	
#define	MEI_DEBUG_WAD				(0x0228 + MEI_SPACE_ACCESS)	
#define MEI_DEBUG_RAD				(0x022C + MEI_SPACE_ACCESS)	
#define	MEI_DEBUG_DATA				(0x0230 + MEI_SPACE_ACCESS)	
#define	MEI_DEBUG_DEC				(0x0234 + MEI_SPACE_ACCESS)	
#define	MEI_CONTROL				(0x0238 + MEI_SPACE_ACCESS)	
#define	AT_CELLRDY_BC0				(0x023C + MEI_SPACE_ACCESS)
#define	AT_CELLRDY_BC1				(0x0240 + MEI_SPACE_ACCESS)	
#define	AR_CELLRDY_BC0				(0x0244 + MEI_SPACE_ACCESS)	
#define	AR_CELLRDY_BC1				(0x0248 + MEI_SPACE_ACCESS)	
#define	AAI_ACCESS				(0x024C + MEI_SPACE_ACCESS)	
#define	AAITXCB0				(0x0300 + MEI_SPACE_ACCESS)	
#define	AAITXCB1				(0x0304 + MEI_SPACE_ACCESS)	
#define	AAIRXCB0				(0x0308 + MEI_SPACE_ACCESS)	
#define	AAIRXCB1				(0x030C + MEI_SPACE_ACCESS)	


//	MEI_TO_ARC_INTERRUPT Register definitions
#define	MEI_TO_ARC_INT1			BIT3
#define	MEI_TO_ARC_INT0			BIT2
#define MEI_TO_ARC_CS_DONE		BIT1
#define	MEI_TO_ARC_MSGAV		BIT0

//	ARC_TO_MEI_INTERRUPT Register definitions
#define	ARC_TO_MEI_INT1			BIT8
#define	ARC_TO_MEI_INT0			BIT7
#define	ARC_TO_MEI_CS_REQ		BIT6
#define	ARC_TO_MEI_DBG_DONE		BIT5
#define	ARC_TO_MEI_MSGACK		BIT4
#define	ARC_TO_MEI_NO_ACCESS		BIT3
#define	ARC_TO_MEI_CHECK_AAITX		BIT2
#define	ARC_TO_MEI_CHECK_AAIRX		BIT1
#define	ARC_TO_MEI_MSGAV		BIT0

//	ARC_TO_MEI_INTERRUPT_MASK Register definitions
#define	GP_INT1_EN			BIT8
#define	GP_INT0_EN			BIT7
#define	CS_REQ_EN			BIT6
#define	DBG_DONE_EN			BIT5
#define	MSGACK_EN			BIT4
#define	NO_ACC_EN			BIT3
#define	AAITX_EN			BIT2
#define	AAIRX_EN			BIT1
#define	MSGAV_EN			BIT0

//	MEI_CONTROL Register definitions
#define	INT_LEVEL			BIT2
#define	SOFT_RESET			BIT1
#define	HOST_MSTR			BIT0

//	MEI_DEBUG_DECODE Register definitions
#define MEI_DEBUG_DEC_MASK		(0x3)
#define MEI_DEBUG_DEC_AUX_MASK		(0x0)
#define MEI_DEBUG_DEC_DMP1_MASK		(0x1)
#define MEI_DEBUG_DEC_DMP2_MASK		(0x2)
#define MEI_DEBUG_DEC_CORE_MASK         (0x3)


//	ARC_TO_MEI_MAILBOX[11] is a special location used to indicate
//	page swap requests.
#define	MEI_TO_ARC_MAILBOX		(0x15FC0)
#define MEI_TO_ARC_MAILBOXR		(0x15FEC)			
#define	ARC_TO_MEI_MAILBOX		(0x15F90)	
#define ARC_MEI_MAILBOXR		(0x15FBC)

// Codeswap request messages are indicated by setting BIT31 
#define OMB_CODESWAP_MESSAGE_MSG_TYPE_MASK	(0x80000000)

/*
**	Swap page header
*/
//	Page must be loaded at boot time if size field has BIT31 set
#define BOOT_FLAG	(BIT31)
#define BOOT_FLAG_MASK	~BOOT_FLAG

//	Swap page header describes size in 32-bit words, load location, and image offset
//	for program and/or data segments
typedef struct _arc_swp_page_hdr
{
	u32		p_offset;	//	Offset bytes of progseg from beginning of image
	u32		p_dest;		//	Destination addr of progseg on processor
	u32		p_size;		//	Size in 32-bitwords of program segment
	u32		d_offset;	//	Offset bytes of dataseg from beginning of image
	u32		d_dest;		//	Destination addr of dataseg on processor
	u32		d_size;		//	Size in 32-bitwords of data segment
}ARC_SWP_PAGE_HDR;


/*
**	Swap image header
*/
#define GET_PROG	0	//	Flag used for program mem segment
#define GET_DATA	1	//	Flag used for data mem segment

//	Image header contains size of image, checksum for image, and count of
//	page headers. Following that are 'count' page headers followed by
//	the code and/or data segments to be loaded
typedef struct _arc_img_hdr
{
	u32			size;		// 	Size of binary image in bytes
	u32			checksum;	//	Checksum for image
	u32			count;		//	Count of swp pages in image
	ARC_SWP_PAGE_HDR 	page[1];	//	Should be "count" pages - '1' to make compiler happy
}ARC_IMG_HDR;



/*
**	Native size for the Stratiphy interface is 32-bits. All reads and writes
**	MUST be aligned on 32-bit boundaries. Trickery must be invoked to read word and/or
**	byte data. Read routines are provided. Write routines are probably a bad idea, as the
**	Arc has unrestrained, unseen access to the same memory, so a read-modify-write cycle
**	could very well have unintended results.
*/
MEI_ERROR meiCMV(u16 *, int);                         // first arg is CMV to ARC, second to indicate whether need reply

void meiLongwordWrite(u32 ul_address, u32 ul_data);
void meiLongwordRead(u32 ul_address, u32 *pul_data);


MEI_ERROR meiDMAWrite(u32 destaddr, u32 *databuff, u32 databuffsize);
MEI_ERROR meiDebugWrite(u32 destaddr, u32 *databuff, u32 databuffsize);

MEI_ERROR meiDMARead(u32 srcaddr, u32 *databuff, u32 databuffsize);
MEI_ERROR meiDebugRead(u32 srcaddr, u32 *databuff, u32 databuffsize);

void meiPollForDbgDone(void);

void meiMailboxInterruptsDisable(void);
void meiMailboxInterruptsEnable(void);

MEI_ERROR meiMailboxWrite(u16 *msgsrcbuffer, u16 msgsize);
MEI_ERROR meiMailboxRead(u16 *msgdestbuffer, u16 msgsize);

int meiGetPage( u32 Page, u32 data, u32 MaxSize, u32 *Buffer, u32 *Dest);

MEI_ERROR meiHaltArc(void);
MEI_ERROR meiRunArc(void);

MEI_ERROR meiDownloadBootCode(void);

MEI_ERROR meiForceRebootAdslModem(void);

void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data);

#endif

