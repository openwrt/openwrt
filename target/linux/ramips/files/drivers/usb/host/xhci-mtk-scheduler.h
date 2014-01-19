#ifndef _XHCI_MTK_SCHEDULER_H
#define _XHCI_MTK_SCHEDULER_H

#define MTK_SCH_NEW		1

#define SCH_SUCCESS		1
#define SCH_FAIL		0

#define MAX_EP_NUM		64
#define SS_BW_BOUND		51000
#define HS_BW_BOUND		6144

#define USB_EP_CONTROL		0
#define USB_EP_ISOC		1
#define USB_EP_BULK		2
#define USB_EP_INT		3

#define USB_SPEED_LOW		1
#define USB_SPEED_FULL		2
#define USB_SPEED_HIGH		3
#define USB_SPEED_SUPER		5

/* mtk scheduler bitmasks */
#define BPKTS(p)		((p) & 0x3f)
#define BCSCOUNT(p)		(((p) & 0x7) << 8)
#define BBM(p)			((p) << 11)
#define BOFFSET(p)		((p) & 0x3fff)
#define BREPEAT(p)		(((p) & 0x7fff) << 16)


#if 1
typedef unsigned int mtk_u32;
typedef unsigned long long mtk_u64;
#endif

#define NULL ((void *)0)

struct mtk_xhci_ep_ctx {
	mtk_u32	ep_info;
	mtk_u32	ep_info2;
	mtk_u64	deq;
	mtk_u32	tx_info;
	/* offset 0x14 - 0x1f reserved for HC internal use */
	mtk_u32	reserved[3];
};


struct sch_ep
{
	//device info
	int dev_speed;
	int isTT;
	//ep info
	int is_in;
	int ep_type;
	int maxp;
	int interval;
	int burst;
	int mult;
	//scheduling info
	int offset;
	int repeat;
	int pkts;
	int cs_count;
	int burst_mode;
	//other
	int bw_cost;	//bandwidth cost in each repeat; including overhead
	mtk_u32 *ep;		//address of usb_endpoint pointer
};

int mtk_xhci_scheduler_init(void);
int mtk_xhci_scheduler_add_ep(int dev_speed, int is_in, int isTT, int ep_type, int maxp, int interval, int burst
	, int mult, mtk_u32 *ep, mtk_u32 *ep_ctx, struct sch_ep *sch_ep);
struct sch_ep * mtk_xhci_scheduler_remove_ep(int dev_speed, int is_in, int isTT, int ep_type, mtk_u32 *ep);


#endif
