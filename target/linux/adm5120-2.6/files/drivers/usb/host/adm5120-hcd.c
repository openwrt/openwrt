/*
 *	HCD driver for ADM5120 SoC
 *
 *	Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *
 *	Based on the ADMtek 2.4 driver
 *	(C) Copyright 2003 Junius Chen <juniusc@admtek.com.tw>
 *	Which again was based on the ohci and uhci drivers.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/usb.h>
#include <linux/platform_device.h>

#include <asm/bootinfo.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/byteorder.h>
#include <asm/mach-adm5120/adm5120_info.h>

#include "../core/hcd.h"

MODULE_DESCRIPTION("ADM5120 USB Host Controller Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jeroen Vreeken (pe1rxq@amsat.org)");

#define PFX	"adm5120-hcd: "

#define ADMHCD_REG_CONTROL		0x00
#define  ADMHCD_SW_RESET		0x00000008	/* Reset */
#define  ADMHCD_DMAA			0x00000004	/* DMA arbitration control */
#define  ADMHCD_SW_INTREQ		0x00000002	/* request software int */
#define  ADMHCD_HOST_EN			0x00000001	/* Host enable */
#define ADMHCD_REG_INTSTATUS		0x04
#define  ADMHCD_INT_ACT			0x80000000	/* Interrupt active */
#define  ADMHCD_INT_FATAL		0x40000000	/* Fatal interrupt */
#define  ADMHCD_INT_SW			0x20000000	/* software interrupt */
#define  ADMHCD_INT_TD			0x00100000	/* TD completed */
#define  ADMHCD_FNO			0x00000800	/* Frame number overaflow */
#define  ADMHCD_SO			0x00000400	/* Scheduling overrun */
#define  ADMHCD_INSMI			0x00000200	/* Root hub status change */
#define  ADMHCD_BABI			0x00000100	/* Babble detected, host mode */
#define  ADMHCD_RESI			0x00000020	/* Resume detected */
#define  ADMHCD_SOFI			0x00000010	/* SOF transmitted/received, host mode */
#define ADMHCD_REG_INTENABLE		0x08
#define  ADMHCD_INT_EN			0x80000000	/* Interrupt enable */
#define  ADMHCD_INTMASK			0x00000001	/* Interrupt mask */	
#define ADMHCD_REG_HOSTCONTROL		0x10
#define  ADMHCD_DMA_EN			0x00000004	/* USB host DMA enable */
#define  ADMHCD_STATE_RST		0x00000000	/* bus state reset */
#define  ADMHCD_STATE_RES		0x00000001	/* bus state resume */
#define  ADMHCD_STATE_OP		0x00000002	/* bus state operational */
#define  ADMHCD_STATE_SUS		0x00000003	/* bus state suspended */
#define ADMHCD_REG_FMINTERVAL		0x18
#define ADMHCD_REG_FMNUMBER		0x1c
#define ADMHCD_REG_LSTHRESH		0x70
#define ADMHCD_REG_RHDESCR		0x74
#define  ADMHCD_CRWE			0x20000000	/* Clear wakeup enable */
#define  ADMHCD_DRWE			0x10000000	/* Device remote wakeup enable */
#define  ADMHCD_HW_OCIC			0x08000000	/* Over current indication change */
#define  ADMHCD_LPSC			0x04000000	/* Local power switch change */
#define  ADMHCD_OCI			0x02000000	/* Over current indication */
#define  ADMHCD_LPS			0x01000000	/* Local power switch/global power switch */
#define  ADMHCD_NOCP			0x00000800	/* No over current protect mode */
#define  ADMHCD_OPCM			0x00000400	/* Over current protect mode */
#define  ADMHCD_NPS			0x00000200	/* No Power Switch */
#define  ADMHCD_PSM			0x00000100	/* Power switch mode */
#define ADMHCD_REG_PORTSTATUS0		0x78
#define  ADMHCD_CCS			0x00000001	/* current connect status */
#define  ADMHCD_PES			0x00000002	/* port enable status */
#define  ADMHCD_PSS			0x00000004	/* port suspend status */
#define  ADMHCD_POCI			0x00000008	/* port overcurrent indicator */
#define  ADMHCD_PRS			0x00000010	/* port reset status */
#define  ADMHCD_PPS			0x00000100	/* port power status */
#define  ADMHCD_LSDA			0x00000200	/* low speed device attached */
#define  ADMHCD_CSC			0x00010000	/* connect status change */
#define  ADMHCD_PESC			0x00020000	/* enable status change */
#define  ADMHCD_PSSC			0x00040000	/* suspend status change */
#define  ADMHCD_OCIC			0x00080000	/* overcurrent change*/
#define  ADMHCD_PRSC			0x00100000	/* reset status change */
#define ADMHCD_REG_PORTSTATUS1		0x7c
#define ADMHCD_REG_HOSTHEAD		0x80


#define ADMHCD_NUMPORTS		2

struct admhcd_ed {
	/* Don't change first four, they used for DMA */
	u32				control;
	struct admhcd_td		*tail;
	struct admhcd_td		*head;
	struct admhcd_ed		*next;
	/* the rest is for the driver only: */
	struct admhcd_td		*cur;
	struct usb_host_endpoint 	*ep;
	struct urb			*urb;
	struct admhcd_ed		*real;
} __attribute__ ((packed));

#define ADMHCD_ED_EPSHIFT	7		/* Shift for endpoint number */
#define ADMHCD_ED_INT		0x00000800	/* Is this an int endpoint */
#define ADMHCD_ED_SPEED		0x00002000	/* Is it a high speed dev? */
#define ADMHCD_ED_SKIP		0x00004000	/* Skip this ED */
#define ADMHCD_ED_FORMAT	0x00008000	/* Is this an isoc endpoint */
#define ADMHCD_ED_MAXSHIFT	16		/* Shift for max packet size */

struct admhcd_td {
	/* Don't change first four, they are used for DMA */
	u32			control;
	u32			buffer;
	u32			buflen;
	struct admhcd_td	*next;
	/* the rest is for the driver only: */
	struct urb		*urb;
	struct admhcd_td	*real;
} __attribute__ ((packed));

#define ADMHCD_TD_OWN		0x80000000
#define ADMHCD_TD_TOGGLE	0x00000000
#define ADMHCD_TD_DATA0		0x01000000
#define ADMHCD_TD_DATA1		0x01800000
#define ADMHCD_TD_OUT		0x00200000
#define ADMHCD_TD_IN		0x00400000
#define ADMHCD_TD_SETUP		0x00000000
#define ADMHCD_TD_ISO		0x00010000
#define ADMHCD_TD_R		0x00040000
#define ADMHCD_TD_INTEN		0x00010000

static int admhcd_td_err[16] = {
	0,		/* No */
	-EREMOTEIO,		/* CRC */
	-EREMOTEIO,	/* bit stuff */
	-EREMOTEIO,		/* data toggle */
	-EPIPE,		/* stall */
	-ETIMEDOUT,	/* timeout */
	-EPROTO,	/* pid err */
	-EPROTO,	/* unexpected pid */
	-EREMOTEIO,	/* data overrun */
	-EREMOTEIO,	/* data underrun */
	-ETIMEDOUT,	/* 1010 */
	-ETIMEDOUT,	/* 1011 */
	-EREMOTEIO,	/* buffer overrun */
	-EREMOTEIO,	/* buffer underrun */
	-ETIMEDOUT,	/* 1110 */
	-ETIMEDOUT,	/* 1111 */
};

#define ADMHCD_TD_ERRMASK	0x38000000
#define ADMHCD_TD_ERRSHIFT	27

#define TD(td)	((struct admhcd_td *)(((u32)(td)) & ~0xf))
#define ED(ed)	((struct admhcd_ed *)(((u32)(ed)) & ~0xf))

struct admhcd {
	spinlock_t	lock;

	void __iomem *data_reg;
	/* Root hub registers */
	u32 rhdesca;
	u32 rhdescb;
	u32 rhstatus;
	u32 rhport[2];

	/* async schedule: control, bulk */
	struct list_head async;
	u32		base;
	u32		dma_en;
	unsigned long	flags;

};

static inline struct admhcd *hcd_to_admhcd(struct usb_hcd *hcd)
{
	return (struct admhcd *)(hcd->hcd_priv);
}

static inline struct usb_hcd *admhcd_to_hcd(struct admhcd *admhcd)
{
	return container_of((void *)admhcd, struct usb_hcd, hcd_priv);
}

static char hcd_name[] = "adm5120-hcd";

static u32 admhcd_reg_get(struct admhcd *ahcd, int reg)
{
	return *(volatile u32 *)KSEG1ADDR(ahcd->base+reg);
}

static void admhcd_reg_set(struct admhcd *ahcd, int reg, u32 val)
{
	*(volatile u32 *)KSEG1ADDR(ahcd->base+reg) = val;
}

static void admhcd_lock(struct admhcd *ahcd)
{
	spin_lock_irqsave(&ahcd->lock, ahcd->flags);
	ahcd->dma_en = admhcd_reg_get(ahcd, ADMHCD_REG_HOSTCONTROL) &
	    ADMHCD_DMA_EN;
	admhcd_reg_set(ahcd, ADMHCD_REG_HOSTCONTROL, ADMHCD_STATE_OP);
}

static void admhcd_unlock(struct admhcd *ahcd)
{
	admhcd_reg_set(ahcd, ADMHCD_REG_HOSTCONTROL,
	    ADMHCD_STATE_OP | ahcd->dma_en);
	spin_unlock_irqrestore(&ahcd->lock, ahcd->flags);
}

static struct admhcd_td *admhcd_td_alloc(struct admhcd_ed *ed, struct urb *urb)
{
	struct admhcd_td *tdn, *td;

	tdn = kmalloc(sizeof(struct admhcd_td), GFP_ATOMIC);
	if (!tdn)
		return NULL;
	tdn->real = tdn;
	tdn = (struct admhcd_td *)KSEG1ADDR(tdn);
	memset(tdn, 0, sizeof(struct admhcd_td));
	if (ed->cur == NULL) {
		ed->cur = tdn;
		ed->head = tdn;
		ed->tail = tdn;
		td = tdn;
	} else {
		/* Supply back the old tail and link in new td as tail */
		td = TD(ed->tail);
		TD(ed->tail)->next = tdn;
		ed->tail = tdn;
	}
	td->urb = urb;

	return td;
}

static void admhcd_td_free(struct admhcd_ed *ed, struct urb *urb)
{
	struct admhcd_td *td, **tdp;

	if (urb == NULL)
		ed->control |= ADMHCD_ED_SKIP;
	tdp = &ed->cur;
	td = ed->cur;
	do {
		if (td->urb == urb)
			break;
		tdp = &td->next;
		td = TD(td->next);
	} while (td);
	while (td && td->urb == urb) {
		*tdp = TD(td->next);
		kfree(td->real);
		td = *tdp;
	}
}

/* Find an endpoint's descriptor, if needed allocate a new one and link it
   in the DMA chain
 */
static struct admhcd_ed *admhcd_get_ed(struct admhcd *ahcd,
    struct usb_host_endpoint *ep, struct urb *urb)
{
	struct admhcd_ed *hosthead;
	struct admhcd_ed *found = NULL, *ed = NULL;
	unsigned int pipe = urb->pipe;

	admhcd_lock(ahcd);
	hosthead = (struct admhcd_ed *)admhcd_reg_get(ahcd, ADMHCD_REG_HOSTHEAD);
	if (hosthead) {
		for (ed = hosthead;; ed = ED(ed->next)) {
			if (ed->ep == ep) {
				found = ed;
				break;
			}
			if (ED(ed->next) == hosthead)
				break;
		}
	}
	if (!found) {
		found = kmalloc(sizeof(struct admhcd_ed), GFP_ATOMIC);
		if (!found)
			goto out;
		memset(found, 0, sizeof(struct admhcd_ed));
		found->real = found;
		found->ep = ep;
		found = (struct admhcd_ed *)KSEG1ADDR(found);
		found->control = usb_pipedevice(pipe) |
		    (usb_pipeendpoint(pipe) << ADMHCD_ED_EPSHIFT) |
		    (usb_pipeint(pipe) ? ADMHCD_ED_INT : 0) |
		    (urb->dev->speed == USB_SPEED_FULL ? ADMHCD_ED_SPEED : 0) |
		    (usb_pipeisoc(pipe) ? ADMHCD_ED_FORMAT : 0) |
		    (usb_maxpacket(urb->dev, pipe, usb_pipeout(pipe)) << ADMHCD_ED_MAXSHIFT);
		/* Alloc first dummy td */
		admhcd_td_alloc(found, NULL);
		if (hosthead) {
			found->next = hosthead;
			ed->next = found;
		} else {
			found->next = found;
			admhcd_reg_set(ahcd, ADMHCD_REG_HOSTHEAD, (u32)found);
		}
	}
out:
	admhcd_unlock(ahcd);
	return found;
}

static struct admhcd_td *admhcd_td_fill(u32 control, struct admhcd_td *td,
    dma_addr_t data, int len)
{
	td->buffer = data;
	td->buflen = len;
	td->control = control;
	return TD(td->next);
}

static void admhcd_ed_start(struct admhcd *ahcd, struct admhcd_ed *ed)
{
	struct admhcd_td *td = ed->cur;

	if (ed->urb)
		return;
	if (td->urb) {
		ed->urb = td->urb;
		while (1) {
			td->control |= ADMHCD_TD_OWN;
			if (TD(td->next)->urb != td->urb) {
				td->buflen |= ADMHCD_TD_INTEN;
				break;
			}
			td = TD(td->next);
		}
	}
	ed->head = TD(ed->head);
	ahcd->dma_en |= ADMHCD_DMA_EN;
}

static irqreturn_t adm5120hcd_irq(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	u32 intstatus;

	intstatus = admhcd_reg_get(ahcd, ADMHCD_REG_INTSTATUS);
	if (intstatus & ADMHCD_INT_FATAL) {
		admhcd_reg_set(ahcd, ADMHCD_REG_INTSTATUS, ADMHCD_INT_FATAL);
		//
	}
	if (intstatus & ADMHCD_INT_SW) {
		admhcd_reg_set(ahcd, ADMHCD_REG_INTSTATUS, ADMHCD_INT_SW);
		//
	}
	if (intstatus & ADMHCD_INT_TD) {
		struct admhcd_ed *ed, *head;

		admhcd_reg_set(ahcd, ADMHCD_REG_INTSTATUS, ADMHCD_INT_TD);

		head = (struct admhcd_ed *)admhcd_reg_get(ahcd, ADMHCD_REG_HOSTHEAD);
		ed = head;
		if (ed) do {
			/* Is it a finished TD? */
			if (ed->urb && !(ed->cur->control & ADMHCD_TD_OWN)) {
				struct admhcd_td *td;
				int error;

				td = ed->cur;
				error = (td->control & ADMHCD_TD_ERRMASK) >>
				    ADMHCD_TD_ERRSHIFT;
				ed->urb->status = admhcd_td_err[error];
				admhcd_td_free(ed, ed->urb);
				// Calculate real length!!!
				ed->urb->actual_length = ed->urb->transfer_buffer_length;
				ed->urb->hcpriv = NULL;
				usb_hcd_giveback_urb(hcd, ed->urb);
				ed->urb = NULL;
			}
			admhcd_ed_start(ahcd, ed);
			ed = ED(ed->next);
		} while (ed != head);
	}

	return IRQ_HANDLED;
}

static int admhcd_urb_enqueue(struct usb_hcd *hcd, struct usb_host_endpoint *ep,
    struct urb *urb, gfp_t mem_flags)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	struct admhcd_ed *ed;
	struct admhcd_td *td;
	int size = 0, i, zero = 0, ret = 0;
	unsigned int pipe = urb->pipe, toggle = 0;
	dma_addr_t data = (dma_addr_t)urb->transfer_buffer;
	int data_len = urb->transfer_buffer_length;

	ed = admhcd_get_ed(ahcd, ep, urb);
	if (!ed)
		return -ENOMEM;

	switch(usb_pipetype(pipe)) {
		case PIPE_CONTROL:
			size = 2;
		case PIPE_INTERRUPT:
		case PIPE_BULK:
		default:
			size += urb->transfer_buffer_length / 4096;
			if (urb->transfer_buffer_length % 4096)
				size++;
			if (size == 0)
				size++;
			else if (urb->transfer_flags & URB_ZERO_PACKET &&
			    !(urb->transfer_buffer_length %
			      usb_maxpacket(urb->dev, pipe, usb_pipeout(pipe)))) {
				size++;
				zero = 1;
			}
			break;
		case PIPE_ISOCHRONOUS:
			size = urb->number_of_packets;
			break;
	}

	admhcd_lock(ahcd);
	/* Remember the first td */
	td = admhcd_td_alloc(ed, urb);
	if (!td) {
		ret = -ENOMEM;
		goto out;
	}
	/* Allocate additionall tds first */
	for (i = 1; i < size; i++) {
		if (admhcd_td_alloc(ed, urb) == NULL) {
			admhcd_td_free(ed, urb);
			ret = -ENOMEM;
			goto out;
		}
	}

	if (usb_gettoggle(urb->dev, usb_pipeendpoint(pipe), usb_pipeout(pipe)))
		toggle = ADMHCD_TD_TOGGLE;
	else {
		toggle = ADMHCD_TD_DATA0;
		usb_settoggle(urb->dev, usb_pipeendpoint(pipe),
		    usb_pipeout(pipe), 1);
	}

	switch(usb_pipetype(pipe)) {
		case PIPE_CONTROL:
			td = admhcd_td_fill(ADMHCD_TD_SETUP | ADMHCD_TD_DATA0,
			    td, (dma_addr_t)urb->setup_packet, 8);
			while (data_len > 0) {
				td = admhcd_td_fill(ADMHCD_TD_DATA1
				    | ADMHCD_TD_R |
				    (usb_pipeout(pipe) ?
				    ADMHCD_TD_OUT : ADMHCD_TD_IN), td,
				    data, data_len % 4097);
				data_len -= 4096;
			}
			admhcd_td_fill(ADMHCD_TD_DATA1 | (usb_pipeout(pipe) ?
			    ADMHCD_TD_IN : ADMHCD_TD_OUT), td,
			    data, 0);
			break;
		case PIPE_INTERRUPT:
		case PIPE_BULK:
			//info ok for interrupt?
			i = 0;
			while(data_len > 4096) {
				td = admhcd_td_fill((usb_pipeout(pipe) ?
				    ADMHCD_TD_OUT :
				    ADMHCD_TD_IN | ADMHCD_TD_R) |
				    (i ? ADMHCD_TD_TOGGLE : toggle), td,
				    data, 4096);
				data += 4096;
				data_len -= 4096;
				i++;
			}
			td = admhcd_td_fill((usb_pipeout(pipe) ?
			    ADMHCD_TD_OUT : ADMHCD_TD_IN) |
			    (i ? ADMHCD_TD_TOGGLE : toggle), td, data, data_len);
			i++;
			if (zero)
				admhcd_td_fill((usb_pipeout(pipe) ?
				    ADMHCD_TD_OUT : ADMHCD_TD_IN) |
				    (i ? ADMHCD_TD_TOGGLE : toggle), td, 0, 0);
			break;
		case PIPE_ISOCHRONOUS:
			for (i = 0; i < urb->number_of_packets; i++) {
				td = admhcd_td_fill(ADMHCD_TD_ISO |
				    ((urb->start_frame + i) & 0xffff), td,
				    data + urb->iso_frame_desc[i].offset,
				    urb->iso_frame_desc[i].length);
			}
			break;
	}
	urb->hcpriv = ed;
	admhcd_ed_start(ahcd, ed);
out:
	admhcd_unlock(ahcd);
	return ret;
}

static int admhcd_urb_dequeue(struct usb_hcd *hcd, struct urb *urb)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	struct admhcd_ed *ed;

	admhcd_lock(ahcd);

	ed = urb->hcpriv;
	if (ed && ed->urb != urb)
		admhcd_td_free(ed, urb);

	admhcd_unlock(ahcd);
	return 0;
}

static void admhcd_endpoint_disable(struct usb_hcd *hcd, struct usb_host_endpoint *ep)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	struct admhcd_ed *ed, *edt, *head;

	admhcd_lock(ahcd);

	head = (struct admhcd_ed *)admhcd_reg_get(ahcd, ADMHCD_REG_HOSTHEAD);
	if (!head)
		goto out;
	for (ed = head; ED(ed->next) != head; ed = ED(ed->next))
		if (ed->ep == ep)
			break;
	if (ed->ep != ep)
		goto out;
	while (ed->cur)
		admhcd_td_free(ed, ed->cur->urb);
	if (head == ed) {
		if (ED(ed->next) == ed) {
			admhcd_reg_set(ahcd, ADMHCD_REG_HOSTHEAD, 0);
			ahcd->dma_en = 0;
			goto out_free;
		}
		head = ED(ed->next);
		for (edt = head; ED(edt->next) != head; edt = ED(edt->next));
		edt->next = ED(ed->next);
		admhcd_reg_set(ahcd, ADMHCD_REG_HOSTHEAD, (u32)ed->next);
		goto out_free;
	}
	for (edt = head; edt->next != ed; edt = edt->next);
	edt->next = ed->next;
out_free:
	kfree(ed->real);
out:
	admhcd_unlock(ahcd);
}

static int admhcd_get_frame_number(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);

	return admhcd_reg_get(ahcd, ADMHCD_REG_FMNUMBER) & 0x0000ffff;
}

static int admhcd_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	int port;

	*buf = 0;
	for (port = 0; port < ADMHCD_NUMPORTS; port++) {
		if (admhcd_reg_get(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4) &
		    (ADMHCD_CSC | ADMHCD_PESC | ADMHCD_PSSC | ADMHCD_OCIC |
		     ADMHCD_PRSC))
			*buf |= (1 << (port + 1));
	}
	return !!*buf;
}

static __u8 root_hub_hub_des[] = {
	0x09,		/* __u8  bLength; */
	0x29,		/* __u8  bDescriptorType; Hub-descriptor */
	0x02,		/* __u8  bNbrPorts; */
	0x0a, 0x00,	/* __u16 wHubCharacteristics; */
	0x01,		/* __u8  bPwrOn2pwrGood; 2ms */
	0x00,		/* __u8  bHubContrCurrent; 0mA */
	0x00,		/* __u8  DeviceRemovable; */
	0xff,		/* __u8  PortPwrCtrlMask; */
};

static int admhcd_hub_control(struct usb_hcd *hcd, u16 typeReq, u16 wValue,
    u16 wIndex, char *buf, u16 wLength)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	int retval = 0, len;
	unsigned int port = wIndex -1;

	switch (typeReq) {

	case GetHubStatus:
		*(__le32 *)buf = cpu_to_le32(0);
		break;
	case GetPortStatus:
		if (port >= ADMHCD_NUMPORTS)
			goto err;
		*(__le32 *)buf = cpu_to_le32(
		    admhcd_reg_get(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4));
		break;
	case SetHubFeature:		/* We don't implement these */
	case ClearHubFeature:
		switch (wValue) {
		case C_HUB_OVER_CURRENT:
		case C_HUB_LOCAL_POWER:
			break;
		default:
			goto err;
		}
	case SetPortFeature:
		if (port >= ADMHCD_NUMPORTS)
			goto err;

		switch (wValue) {
		case USB_PORT_FEAT_SUSPEND:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_PSS);
			break;
		case USB_PORT_FEAT_RESET:
			if (admhcd_reg_get(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4)
			    & ADMHCD_CCS) {
				admhcd_reg_set(ahcd,
				    ADMHCD_REG_PORTSTATUS0 + port*4,
				    ADMHCD_PRS | ADMHCD_CSC);
				mdelay(50);
				admhcd_reg_set(ahcd,
				    ADMHCD_REG_PORTSTATUS0 + port*4,
				    ADMHCD_PES | ADMHCD_CSC);
			}
			break;
		case USB_PORT_FEAT_POWER:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_PPS);
			break;
		default:
			goto err;
		}
		break;
	case ClearPortFeature:
		if (port >= ADMHCD_NUMPORTS)
			goto err;

		switch (wValue) {
		case USB_PORT_FEAT_ENABLE:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_CCS);
			break;
		case USB_PORT_FEAT_C_ENABLE:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_PESC);
			break;
		case USB_PORT_FEAT_SUSPEND:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_POCI);
			break;
		case USB_PORT_FEAT_C_SUSPEND:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_PSSC);
		case USB_PORT_FEAT_POWER:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_LSDA);
			break;
		case USB_PORT_FEAT_C_CONNECTION:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_CSC);
			break;
		case USB_PORT_FEAT_C_OVER_CURRENT:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_OCIC);
			break;
		case USB_PORT_FEAT_C_RESET:
			admhcd_reg_set(ahcd, ADMHCD_REG_PORTSTATUS0 + port*4,
			    ADMHCD_PRSC);
			break;
		default:
			goto err;
		}
		break;
	case GetHubDescriptor:
		len = min_t(unsigned int, sizeof(root_hub_hub_des), wLength);
		memcpy(buf, root_hub_hub_des, len);
		break;
	default:
err:
		retval = -EPIPE;
	}

	return retval;
}

static int admhcd_start(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	unsigned long flags;

	printk(KERN_DEBUG PFX "calling admhcd_start\n");

	spin_lock_irqsave(&ahcd->lock, flags);

 	/* Initialise the HCD registers */
        admhcd_reg_set(ahcd, ADMHCD_REG_INTENABLE, 0);
        mdelay(10);

        admhcd_reg_set(ahcd, ADMHCD_REG_CONTROL, ADMHCD_SW_RESET);

        while (admhcd_reg_get(ahcd, ADMHCD_REG_CONTROL) & ADMHCD_SW_RESET) {
		printk(KERN_WARNING PFX "waiting for reset to complete\n");
                mdelay(1);
	}
	
	hcd->uses_new_polling = 1;

	/* Enable USB host mode */
        admhcd_reg_set(ahcd, ADMHCD_REG_CONTROL, ADMHCD_HOST_EN);

	/* Set host specific settings */
        admhcd_reg_set(ahcd, ADMHCD_REG_HOSTHEAD, 0x00000000);
        admhcd_reg_set(ahcd, ADMHCD_REG_FMINTERVAL, 0x20002edf);
        admhcd_reg_set(ahcd, ADMHCD_REG_LSTHRESH, 0x628);

	/* Set interrupts */
        admhcd_reg_set(ahcd, ADMHCD_REG_INTENABLE,
            ADMHCD_INT_ACT | ADMHCD_INT_FATAL | ADMHCD_INT_SW | ADMHCD_INT_TD);
        admhcd_reg_set(ahcd, ADMHCD_REG_INTSTATUS,
            ADMHCD_INT_ACT | ADMHCD_INT_FATAL | ADMHCD_INT_SW | ADMHCD_INT_TD);

	/* Power on all ports */
        admhcd_reg_set(ahcd, ADMHCD_REG_RHDESCR, ADMHCD_NPS | ADMHCD_LPSC);

	/* HCD is now operationnal */
        admhcd_reg_set(ahcd, ADMHCD_REG_HOSTCONTROL, ADMHCD_STATE_OP);

       	hcd->state = HC_STATE_RUNNING;

	spin_unlock_irqrestore(&ahcd->lock, flags);

	printk(KERN_DEBUG PFX "returning 0 from admhcd_start\n");
       	return 0;
}

static int admhcd_sw_reset(struct admhcd *ahcd)
{
	int retries = 15;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&ahcd->lock, flags);

	admhcd_reg_set(ahcd, ADMHCD_REG_INTENABLE, 0);
        mdelay(10);

        admhcd_reg_set(ahcd, ADMHCD_REG_CONTROL, ADMHCD_SW_RESET);

        while (--retries) {
		mdelay(1);
		if (!(admhcd_reg_get(ahcd, ADMHCD_REG_CONTROL) & ADMHCD_SW_RESET))
			break;
	}
	if (!retries) {
		printk(KERN_WARNING "%s Software reset timeout\n", hcd_name);
		ret = -ETIME;
	}
	spin_unlock_irqrestore(&ahcd->lock, flags);
	return ret;
}

static int admhcd_reset(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
        u32 val = 0;
	int ret, timeout = 15; /* ms */
	unsigned long t;

	ret = admhcd_sw_reset(ahcd);
	if (ret)
		return ret;

	t = jiffies + msecs_to_jiffies(timeout);
        while (time_before_eq(jiffies, t)) {
                msleep(4);
                spin_lock_irq(&ahcd->lock);
                val = admhcd_reg_get(ahcd, ADMHCD_REG_HOSTCONTROL) & ADMHCD_STATE_RST;
                spin_unlock_irq(&ahcd->lock);
                if (val)
                        break;
        }
        if (!val) {
                printk(KERN_WARNING "Device not ready after %dms\n", timeout);
                ret = -ENODEV;
        }
        return ret;
}

static void admhcd_stop(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
        unsigned long flags;
	u32 val;

        spin_lock_irqsave(&ahcd->lock, flags);
        admhcd_reg_set(ahcd, ADMHCD_REG_INTENABLE, 0);
	
	/* Set global control of power for ports */
	val = admhcd_reg_get(ahcd, ADMHCD_REG_RHDESCR);
	val &= (~ADMHCD_PSM | ADMHCD_LPS);
	admhcd_reg_set(ahcd, ADMHCD_REG_RHDESCR, val);

	spin_unlock_irqrestore(&ahcd->lock, flags);

	/* Ask for software reset */
	admhcd_sw_reset(ahcd);
}


static struct hc_driver adm5120_hc_driver = {
	.description =		hcd_name,
	.product_desc =		"ADM5120 HCD",
	.hcd_priv_size =	sizeof(struct admhcd),
	.irq =			adm5120hcd_irq,
	.flags =		HCD_USB11,
	.urb_enqueue =		admhcd_urb_enqueue,
	.urb_dequeue =		admhcd_urb_dequeue,
	.endpoint_disable =	admhcd_endpoint_disable,
	.get_frame_number =	admhcd_get_frame_number,
	.hub_status_data =	admhcd_hub_status_data,
	.hub_control =		admhcd_hub_control,
	.start	=		admhcd_start,
	.stop	=		admhcd_stop,
	.reset =		admhcd_reset,
};

#define resource_len(r) (((r)->end - (r)->start) + 1)

static int __init adm5120hcd_probe(struct platform_device *pdev)
{
        struct usb_hcd *hcd;
        struct admhcd *ahcd;
	struct resource *data;
	void __iomem *data_reg;

        int err = 0, irq;

	if (pdev->num_resources < 2) {
		printk(KERN_WARNING PFX "not enough resources\n");
		err = -ENODEV;
		goto out;
        }

	irq = platform_get_irq(pdev, 0);
	data = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (pdev->dev.dma_mask) {
                printk(KERN_DEBUG PFX "no we won't dma\n");
                return -EINVAL;
        }

	if (!data || irq < 0) {
		printk(KERN_DEBUG PFX "either IRQ or data resource is invalid\n");
                err = -ENODEV;
                goto out;
        }

        if (!request_mem_region(data->start, 2, hcd_name)) {
		printk(KERN_DEBUG PFX "cannot request memory regions for the data resource\n");
                err = -EBUSY;
                goto out;
        }

        data_reg = ioremap(data->start, resource_len(data));

        if (data_reg == NULL) {
		printk(KERN_DEBUG PFX "unable to ioremap\n");
                err = -ENOMEM;
                goto out_mem;
        }

	hcd = usb_create_hcd(&adm5120_hc_driver, &pdev->dev, pdev->dev.bus_id);
        if (!hcd) {
		printk(KERN_DEBUG PFX "unable to create the hcd\n");
		err = -ENOMEM;
                goto out_mem;
	}

	hcd->rsrc_start = data->start;
	hcd->rsrc_len = resource_len(data);
	hcd->regs = data_reg;
	
	ahcd = hcd_to_admhcd(hcd);
	ahcd->data_reg = data_reg;
	ahcd->base = (u32)data_reg;

	spin_lock_init(&ahcd->lock);
	INIT_LIST_HEAD(&ahcd->async);

	hcd->product_desc = "ADM5120 HCD";

	err = usb_add_hcd(hcd, irq, IRQF_DISABLED);
	if (err) {
		printk(KERN_DEBUG PFX "unable to add hcd\n");
		goto out_dev;
	}

	return 0;

out_dev:
	usb_put_hcd(hcd);
out_unmap:
	iounmap(data_reg);
out_mem:
	release_mem_region(pdev->resource[0].start, pdev->resource[0].end - pdev->resource[0].start +1);
out:
	return err;
}

#ifdef CONFIG_PM
static int adm5120hcd_suspend(struct platform_device *pdev,  pm_message_t state)
{
	pdev-dev.power.power_state = state;
	mdelay(1);
	return 0;
}

static int adm5120hcd_resume(struct platform_device *pdev, pm_message_t state)
{
	pdev->dev.power.power_state = PMSG_ON;
	mdelay(1);
	return 0;
}
#else
#define adm5120hcd_suspend 	NULL
#define adm5120hcd_resume 	NULL
#endif

static int __init_or_module adm5120hcd_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct admhcd *ahcd;

	if (!hcd)
		return 0;
	ahcd = hcd_to_admhcd(hcd);
	usb_remove_hcd(hcd);

	usb_put_hcd(hcd);
	return 0;
}

static struct platform_driver adm5120hcd_driver = {
	.probe =	adm5120hcd_probe,
	.remove =	adm5120hcd_remove,
	.suspend = 	adm5120hcd_suspend,
	.remove = 	adm5120hcd_resume,
	.driver	=	{
		.name 	= (char *)hcd_name,
		.owner 	= THIS_MODULE,
	},
};

static int __init adm5120hcd_init(void)
{
	int ret;

	if (usb_disabled()) {
		printk(KERN_DEBUG PFX "USB support is disabled\n");
		return -ENODEV;
	}

	if (mips_machgroup != MACH_GROUP_ADM5120) {
		printk(KERN_DEBUG PFX "unsupported machine group\n");
		return -ENODEV;
	}

	ret = platform_driver_register(&adm5120hcd_driver);
	if (ret == 0)
		printk(KERN_INFO PFX "registered\n");

	return ret;
}

static void __exit adm5120hcd_exit(void)
{	
	platform_driver_unregister(&adm5120hcd_driver);
	printk(KERN_INFO PFX "driver unregistered\n");
}

module_init(adm5120hcd_init);
module_exit(adm5120hcd_exit);
