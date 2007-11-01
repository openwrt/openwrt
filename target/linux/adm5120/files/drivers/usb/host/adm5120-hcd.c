/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2004 David Brownell <dbrownell@users.sourceforge.net>
 *
 * [ Initialisation is based on Linus'  ]
 * [ uhci code and gregs ahcd fragments ]
 * [ (C) Copyright 1999 Linus Torvalds  ]
 * [ (C) Copyright 1999 Gregory P. Smith]
 *
 *
 * OHCI is the main "non-Intel/VIA" standard for USB 1.1 host controller
 * interfaces (though some non-x86 Intel chips use it).  It supports
 * smarter hardware than UHCI.  A download link for the spec available
 * through the http://www.usb.org website.
 *
 * This file is licenced under the GPL.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/usb.h>
#include <linux/usb/otg.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/reboot.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <asm/byteorder.h>

#include "../core/hcd.h"
#include "../core/hub.h"

#define DRIVER_VERSION	"v0.02"
#define DRIVER_AUTHOR	"Gabor Juhos <juhosg at openwrt.org>"
#define DRIVER_DESC	"ADMtek USB 1.1 Host Controller Driver"

/*-------------------------------------------------------------------------*/

#define ADMHC_VERBOSE_DEBUG	/* not always helpful */
#undef LATE_ED_SCHEDULE

/* For initializing controller (mask in an HCFS mode too) */
#define	OHCI_CONTROL_INIT	OHCI_CTRL_CBSR

#define	ADMHC_INTR_INIT \
		( ADMHC_INTR_MIE | ADMHC_INTR_INSM | ADMHC_INTR_FATI \
		| ADMHC_INTR_RESI | ADMHC_INTR_TDC | ADMHC_INTR_BABI )

/*-------------------------------------------------------------------------*/

static const char hcd_name [] = "admhc-hcd";

#define	STATECHANGE_DELAY	msecs_to_jiffies(300)

#include "adm5120.h"

static void admhc_dump(struct admhcd *ahcd, int verbose);
static int admhc_init(struct admhcd *ahcd);
static void admhc_stop(struct usb_hcd *hcd);

#include "adm5120-hub.c"
#include "adm5120-dbg.c"
#include "adm5120-mem.c"
#include "adm5120-q.c"

/*-------------------------------------------------------------------------*/

/*
 * queue up an urb for anything except the root hub
 */
static int admhc_urb_enqueue(struct usb_hcd *hcd, struct usb_host_endpoint *ep,
	struct urb *urb, gfp_t mem_flags)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);
	struct ed	*ed;
	struct urb_priv	*urb_priv;
	unsigned int	pipe = urb->pipe;
	int		td_cnt = 0;
	unsigned long	flags;
	int		retval = 0;

#ifdef ADMHC_VERBOSE_DEBUG
	spin_lock_irqsave(&ahcd->lock, flags);
	urb_print(ahcd, urb, "ENQEUE", usb_pipein(pipe));
	spin_unlock_irqrestore(&ahcd->lock, flags);
#endif

	/* every endpoint has an ed, locate and maybe (re)initialize it */
	ed = ed_get(ahcd, ep, urb->dev, pipe, urb->interval);
	if (!ed)
		return -ENOMEM;

	/* for the private part of the URB we need the number of TDs */
	switch (ed->type) {
	case PIPE_CONTROL:
		if (urb->transfer_buffer_length > TD_DATALEN_MAX)
			/* td_submit_urb() doesn't yet handle these */
			return -EMSGSIZE;

		/* 1 TD for setup, 1 for ACK, plus ... */
		td_cnt = 2;
		if (urb->transfer_buffer_length)
			td_cnt++;
		break;
	case PIPE_BULK:
		/* one TD for every 4096 Bytes (can be upto 8K) */
		td_cnt = urb->transfer_buffer_length / TD_DATALEN_MAX;
		/* ... and for any remaining bytes ... */
		if ((urb->transfer_buffer_length % TD_DATALEN_MAX) != 0)
			td_cnt++;
		/* ... and maybe a zero length packet to wrap it up */
		if (td_cnt == 0)
			td_cnt++;
		else if ((urb->transfer_flags & URB_ZERO_PACKET) != 0
			&& (urb->transfer_buffer_length
				% usb_maxpacket(urb->dev, pipe,
					usb_pipeout (pipe))) == 0)
			td_cnt++;
		break;
	case PIPE_INTERRUPT:
		/*
		 * for Interrupt IN/OUT transactions, each ED contains
		 * only 1 TD.
		 * TODO: check transfer_buffer_length?
		 */
		td_cnt = 1;
		break;
	case PIPE_ISOCHRONOUS:
		/* number of packets from URB */
		td_cnt = urb->number_of_packets;
		break;
	default:
		/* paranoia */
		admhc_err(ahcd, "bad EP type %d", ed->type);
		return -EINVAL;
	}

	urb_priv = urb_priv_alloc(ahcd, td_cnt, mem_flags);
	if (!urb_priv)
		return -ENOMEM;

	urb_priv->ed = ed;
	urb_priv->urb = urb;

	spin_lock_irqsave(&ahcd->lock, flags);
	/* don't submit to a dead HC */
	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags)) {
		retval = -ENODEV;
		goto fail;
	}
	if (!HC_IS_RUNNING(hcd->state)) {
		retval = -ENODEV;
		goto fail;
	}

	/* in case of unlink-during-submit */
	spin_lock(&urb->lock);
	if (urb->status != -EINPROGRESS) {
		spin_unlock(&urb->lock);
		urb->hcpriv = urb_priv;
		finish_urb(ahcd, urb);
		retval = 0;
		goto fail;
	}

	if (ed->type == PIPE_ISOCHRONOUS) {
		if (ed->state == ED_NEW) {
			u16	frame = admhc_frame_no(ahcd);

			/* delay a few frames before the first TD */
			frame += max_t (u16, 8, ed->interval);
			frame &= ~(ed->interval - 1);
			frame |= ed->branch;
			urb->start_frame = frame;

			/* yes, only URB_ISO_ASAP is supported, and
			 * urb->start_frame is never used as input.
			 */
		} else
			urb->start_frame = ed->last_iso + ed->interval;
	}

	urb->hcpriv = urb_priv;
	td_submit_urb(ahcd, urb_priv->urb);

	/* append it to the ED's queue */
	list_add_tail(&urb_priv->pending, &ed->urb_pending);

	/* schedule the ED */
	retval = ed_schedule(ahcd, ed);

fail0:
	spin_unlock(&urb->lock);
fail:
	if (retval) {
		urb_priv = urb->hcpriv;
		urb_priv_free(ahcd, urb_priv);
	}

	spin_unlock_irqrestore(&ahcd->lock, flags);
	return retval;
}

/*
 * decouple the URB from the HC queues (TDs, urb_priv); it's
 * already marked using urb->status.  reporting is always done
 * asynchronously, and we might be dealing with an urb that's
 * partially transferred, or an ED with other urbs being unlinked.
 */
static int admhc_urb_dequeue(struct usb_hcd *hcd, struct urb *urb)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	struct urb_priv *up;
	unsigned long flags;

	up = urb->hcpriv;
	if (!up)
		return 0;

	spin_lock_irqsave(&ahcd->lock, flags);

#ifdef ADMHC_VERBOSE_DEBUG
	urb_print(ahcd, urb, "DEQEUE", 1);
#endif

	if (HC_IS_RUNNING(hcd->state)) {
		/* Unless an IRQ completed the unlink while it was being
		 * handed to us, flag it for unlink and giveback, and force
		 * some upcoming INTR_SF to call finish_unlinks()
		 */
		if (up->ed->urb_active != up) {
			list_del(&up->pending);
			finish_urb(ahcd, urb);
		} else {
			ed_start_deschedule(ahcd, up->ed);
		}
	} else {
		/*
		 * with HC dead, we won't respect hc queue pointers
		 * any more ... just clean up every urb's memory.
		 */
		if (up->ed->urb_active != up) {
			list_del(&up->pending);
			finish_urb(ahcd, urb);
		} else {
			finish_urb(ahcd, urb);
			up->ed->urb_active = NULL;
			up->ed->state = ED_IDLE;
		}
	}
	spin_unlock_irqrestore(&ahcd->lock, flags);

	return 0;
}

/*-------------------------------------------------------------------------*/

/* frees config/altsetting state for endpoints,
 * including ED memory, dummy TD, and bulk/intr data toggle
 */
static void admhc_endpoint_disable(struct usb_hcd *hcd,
		struct usb_host_endpoint *ep)
{
	struct admhcd		*ahcd = hcd_to_admhcd(hcd);
	unsigned long		flags;
	struct ed		*ed = ep->hcpriv;
	unsigned		limit = 1000;

	/* ASSERT:  any requests/urbs are being unlinked */
	/* ASSERT:  nobody can be submitting urbs for this any more */

	if (!ed)
		return;

#ifdef ADMHC_VERBOSE_DEBUG
	spin_lock_irqsave(&ahcd->lock, flags);
	admhc_dump_ed(ahcd, "EP-DISABLE", ed, 1);
	spin_unlock_irqrestore(&ahcd->lock, flags);
#endif

rescan:
	spin_lock_irqsave(&ahcd->lock, flags);

	if (!HC_IS_RUNNING(hcd->state)) {
sanitize:
		ed->state = ED_UNLINK;
		admhc_finish_unlinks(ahcd, 0);
	}

	switch (ed->state) {
	case ED_UNLINK:		/* wait for hw to finish? */
		/* major IRQ delivery trouble loses INTR_SOFI too... */
		if (limit-- == 0) {
			admhc_warn(ahcd, "IRQ INTR_SOFI lossage\n");
			goto sanitize;
		}
		spin_unlock_irqrestore(&ahcd->lock, flags);
		schedule_timeout_uninterruptible(1);
		goto rescan;
	case ED_IDLE:
	case ED_NEW:		/* fully unlinked */
		if (list_empty(&ed->urb_pending)) {
			td_free(ahcd, ed->dummy);
			ed_free(ahcd, ed);
			break;
		}
		/* else FALL THROUGH */
	default:
		/* caller was supposed to have unlinked any requests;
		 * that's not our job.  can't recover; must leak ed.
		 */
		admhc_err(ahcd, "leak ed %p (#%02x) %s act %p%s\n",
			ed, ep->desc.bEndpointAddress,
			ed_statestring(ed->state),
			ed->urb_active,
			list_empty(&ed->urb_pending) ? "" : " (has urbs)");
		break;
	}

	ep->hcpriv = NULL;

	spin_unlock_irqrestore(&ahcd->lock, flags);
	return;
}

static int admhc_get_frame(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);

	return admhc_frame_no(ahcd);
}

static void admhc_usb_reset(struct admhcd *ahcd)
{
	ahcd->host_control = ADMHC_BUSS_RESET;
	admhc_writel(ahcd, ahcd->host_control ,&ahcd->regs->host_control);
}

/* admhc_shutdown forcibly disables IRQs and DMA, helping kexec and
 * other cases where the next software may expect clean state from the
 * "firmware".  this is bus-neutral, unlike shutdown() methods.
 */
static void
admhc_shutdown(struct usb_hcd *hcd)
{
	struct admhcd *ahcd;

	ahcd = hcd_to_admhcd(hcd);
	admhc_intr_disable(ahcd, ADMHC_INTR_MIE);
	admhc_dma_disable(ahcd);
	admhc_usb_reset(ahcd);
	/* flush the writes */
	admhc_writel_flush(ahcd);
}

/*-------------------------------------------------------------------------*
 * HC functions
 *-------------------------------------------------------------------------*/

static void admhc_eds_cleanup(struct admhcd *ahcd)
{
	if (ahcd->ed_tails[PIPE_INTERRUPT]) {
		ed_free(ahcd, ahcd->ed_tails[PIPE_INTERRUPT]);
		ahcd->ed_tails[PIPE_INTERRUPT] = NULL;
	}

	if (ahcd->ed_tails[PIPE_ISOCHRONOUS]) {
		ed_free(ahcd, ahcd->ed_tails[PIPE_ISOCHRONOUS]);
		ahcd->ed_tails[PIPE_ISOCHRONOUS] = NULL;
	}

	if (ahcd->ed_tails[PIPE_CONTROL]) {
		ed_free(ahcd, ahcd->ed_tails[PIPE_CONTROL]);
		ahcd->ed_tails[PIPE_CONTROL] = NULL;
	}

	if (ahcd->ed_tails[PIPE_BULK]) {
		ed_free(ahcd, ahcd->ed_tails[PIPE_BULK]);
		ahcd->ed_tails[PIPE_BULK] = NULL;
	}

	ahcd->ed_head = NULL;
}

#define ED_DUMMY_INFO	(ED_SPEED_FULL | ED_SKIP)

static int admhc_eds_init(struct admhcd *ahcd)
{
	struct ed *ed;

	ed = ed_create(ahcd, PIPE_INTERRUPT, ED_DUMMY_INFO);
	if (!ed)
		goto err;

	ahcd->ed_tails[PIPE_INTERRUPT] = ed;

	ed = ed_create(ahcd, PIPE_ISOCHRONOUS, ED_DUMMY_INFO);
	if (!ed)
		goto err;

	ahcd->ed_tails[PIPE_ISOCHRONOUS] = ed;
	ed->ed_prev = ahcd->ed_tails[PIPE_INTERRUPT];
	ahcd->ed_tails[PIPE_INTERRUPT]->ed_next = ed;
	ahcd->ed_tails[PIPE_INTERRUPT]->hwNextED = cpu_to_hc32(ahcd, ed->dma);

	ed = ed_create(ahcd, PIPE_CONTROL, ED_DUMMY_INFO);
	if (!ed)
		goto err;

	ahcd->ed_tails[PIPE_CONTROL] = ed;
	ed->ed_prev = ahcd->ed_tails[PIPE_ISOCHRONOUS];
	ahcd->ed_tails[PIPE_ISOCHRONOUS]->ed_next = ed;
	ahcd->ed_tails[PIPE_ISOCHRONOUS]->hwNextED = cpu_to_hc32(ahcd, ed->dma);

	ed = ed_create(ahcd, PIPE_BULK, ED_DUMMY_INFO);
	if (!ed)
		goto err;

	ahcd->ed_tails[PIPE_BULK] = ed;
	ed->ed_prev = ahcd->ed_tails[PIPE_CONTROL];
	ahcd->ed_tails[PIPE_CONTROL]->ed_next = ed;
	ahcd->ed_tails[PIPE_CONTROL]->hwNextED = cpu_to_hc32(ahcd, ed->dma);

	ahcd->ed_head = ahcd->ed_tails[PIPE_INTERRUPT];

#ifdef ADMHC_VERBOSE_DEBUG
	admhc_dump_ed(ahcd, "ed intr", ahcd->ed_tails[PIPE_INTERRUPT], 1);
	admhc_dump_ed(ahcd, "ed isoc", ahcd->ed_tails[PIPE_ISOCHRONOUS], 1);
	admhc_dump_ed(ahcd, "ed ctrl", ahcd->ed_tails[PIPE_CONTROL], 1);
	admhc_dump_ed(ahcd, "ed bulk", ahcd->ed_tails[PIPE_BULK], 1);
#endif

	return 0;

err:
	admhc_eds_cleanup(ahcd);
	return -ENOMEM;
}

/* init memory, and kick BIOS/SMM off */

static int admhc_init(struct admhcd *ahcd)
{
	struct usb_hcd *hcd = admhcd_to_hcd(ahcd);
	int ret;

	admhc_disable(ahcd);
	ahcd->regs = hcd->regs;

	/* Disable HC interrupts */
	admhc_intr_disable(ahcd, ADMHC_INTR_MIE);

	/* Read the number of ports unless overridden */
	if (ahcd->num_ports == 0)
		ahcd->num_ports = admhc_get_rhdesc(ahcd) & ADMHC_RH_NUMP;

	ret = admhc_mem_init(ahcd);
	if (ret)
		goto err;

	/* init dummy endpoints */
	ret = admhc_eds_init(ahcd);
	if (ret)
		goto err;

	create_debug_files(ahcd);

	return 0;

err:
	admhc_stop(hcd);
	return ret;
}

/*-------------------------------------------------------------------------*/

/* Start an OHCI controller, set the BUS operational
 * resets USB and controller
 * enable interrupts
 */
static int admhc_run(struct admhcd *ahcd)
{
	u32			temp;
	int			first = ahcd->fminterval == 0;
	struct usb_hcd		*hcd = admhcd_to_hcd(ahcd);

	admhc_disable(ahcd);

	/* boot firmware should have set this up (5.1.1.3.1) */
	if (first) {
		temp = admhc_readl(ahcd, &ahcd->regs->fminterval);
		ahcd->fminterval = temp & ADMHC_SFI_FI_MASK;
		if (ahcd->fminterval != FI)
			admhc_dbg(ahcd, "fminterval delta %d\n",
				ahcd->fminterval - FI);
		ahcd->fminterval |=
			(FSLDP (ahcd->fminterval) << ADMHC_SFI_FSLDP_SHIFT);
		/* also: power/overcurrent flags in rhdesc */
	}

	switch (ahcd->host_control & ADMHC_HC_BUSS) {
	case ADMHC_BUSS_OPER:
		temp = 0;
		break;
	case ADMHC_BUSS_SUSPEND:
		/* FALLTHROUGH ? */
	case ADMHC_BUSS_RESUME:
		ahcd->host_control = ADMHC_BUSS_RESUME;
		temp = 10 /* msec wait */;
		break;
	/* case ADMHC_BUSS_RESET: */
	default:
		ahcd->host_control = ADMHC_BUSS_RESET;
		temp = 50 /* msec wait */;
		break;
	}
	admhc_writel(ahcd, ahcd->host_control, &ahcd->regs->host_control);

	/* flush the writes */
	admhc_writel_flush(ahcd);

	msleep(temp);
	temp = admhc_get_rhdesc(ahcd);
	if (!(temp & ADMHC_RH_NPS)) {
		/* power down each port */
		for (temp = 0; temp < ahcd->num_ports; temp++)
			admhc_writel(ahcd, ADMHC_PS_CPP,
				&ahcd->regs->portstatus[temp]);
	}
	/* flush those writes */
	admhc_writel_flush(ahcd);

	/* 2msec timelimit here means no irqs/preempt */
	spin_lock_irq(&ahcd->lock);

retry:
	admhc_writel(ahcd, ADMHC_CTRL_SR,  &ahcd->regs->gencontrol);
	temp = 30;	/* ... allow extra time */
	while ((admhc_readl(ahcd, &ahcd->regs->gencontrol) & ADMHC_CTRL_SR) != 0) {
		if (--temp == 0) {
			spin_unlock_irq(&ahcd->lock);
			admhc_err(ahcd, "USB HC reset timed out!\n");
			return -1;
		}
		udelay (1);
	}

	/* enable HOST mode, before access any host specific register */
	admhc_writel(ahcd, ADMHC_CTRL_UHFE,  &ahcd->regs->gencontrol);

	/* Tell the controller where the descriptor list is */
	admhc_writel(ahcd, (u32)ahcd->ed_head->dma, &ahcd->regs->hosthead);

	periodic_reinit(ahcd);

	/* use rhsc irqs after khubd is fully initialized */
	hcd->poll_rh = 1;
	hcd->uses_new_polling = 1;

#if 0
	/* wake on ConnectStatusChange, matching external hubs */
	admhc_writel(ahcd, RH_HS_DRWE, &ahcd->regs->roothub.status);
#else
	/* FIXME roothub_write_status (ahcd, ADMHC_RH_DRWE); */
#endif

	/* Choose the interrupts we care about now, others later on demand */
	admhc_intr_ack(ahcd, ~0);
	admhc_intr_enable(ahcd, ADMHC_INTR_INIT);

	admhc_writel(ahcd, ADMHC_RH_NPS | ADMHC_RH_LPSC, &ahcd->regs->rhdesc);

	/* flush those writes */
	admhc_writel_flush(ahcd);

	/* start controller operations */
	ahcd->host_control = ADMHC_BUSS_OPER;
	admhc_writel(ahcd, ahcd->host_control, &ahcd->regs->host_control);

	temp = 20;
	while ((admhc_readl(ahcd, &ahcd->regs->host_control)
			& ADMHC_HC_BUSS) != ADMHC_BUSS_OPER) {
		if (--temp == 0) {
			spin_unlock_irq(&ahcd->lock);
			admhc_err(ahcd, "unable to setup operational mode!\n");
			return -1;
		}
		mdelay(1);
	}

	hcd->state = HC_STATE_RUNNING;

	ahcd->next_statechange = jiffies + STATECHANGE_DELAY;

#if 0
	/* FIXME: enabling DMA is always failed here for an unknown reason */
	admhc_dma_enable(ahcd);

	temp = 200;
	while ((admhc_readl(ahcd, &ahcd->regs->host_control)
			& ADMHC_HC_DMAE) != ADMHC_HC_DMAE) {
		if (--temp == 0) {
			spin_unlock_irq(&ahcd->lock);
			admhc_err(ahcd, "unable to enable DMA!\n");
			admhc_dump(ahcd, 1);
			return -1;
		}
		mdelay(1);
	}

#endif

	spin_unlock_irq(&ahcd->lock);

	mdelay(ADMHC_POTPGT);

	return 0;
}

/*-------------------------------------------------------------------------*/

/* an interrupt happens */

static irqreturn_t admhc_irq(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);
	struct admhcd_regs __iomem *regs = ahcd->regs;
 	u32 ints;

	ints = admhc_readl(ahcd, &regs->int_status);
	if ((ints & ADMHC_INTR_INTA) == 0) {
		/* no unmasked interrupt status is set */
		return IRQ_NONE;
	}

	ints &= admhc_readl(ahcd, &regs->int_enable);

	spin_lock(&ahcd->lock);
	if (ints & ADMHC_INTR_FATI) {
		/* e.g. due to PCI Master/Target Abort */
		admhc_disable(ahcd);
		admhc_err(ahcd, "Fatal Error, controller disabled\n");
		admhc_dump(ahcd, 1);
		admhc_usb_reset(ahcd);
	}

	if (ints & ADMHC_INTR_BABI) {
		admhc_intr_disable(ahcd, ADMHC_INTR_MIE);
		admhc_err(ahcd, "Babble Detected\n");
		admhc_disable(ahcd);
		admhc_usb_reset(ahcd);
	}

	if (ints & ADMHC_INTR_INSM) {
		admhc_vdbg(ahcd, "Root Hub Status Change\n");
		ahcd->next_statechange = jiffies + STATECHANGE_DELAY;
		admhc_intr_ack(ahcd, ADMHC_INTR_RESI | ADMHC_INTR_INSM);

		/* NOTE: Vendors didn't always make the same implementation
		 * choices for RHSC.  Many followed the spec; RHSC triggers
		 * on an edge, like setting and maybe clearing a port status
		 * change bit.  With others it's level-triggered, active
		 * until khubd clears all the port status change bits.  We'll
		 * always disable it here and rely on polling until khubd
		 * re-enables it.
		 */
		admhc_intr_disable(ahcd, ADMHC_INTR_INSM);
		usb_hcd_poll_rh_status(hcd);
	} else if (ints & ADMHC_INTR_RESI) {
		/* For connect and disconnect events, we expect the controller
		 * to turn on RHSC along with RD.  But for remote wakeup events
		 * this might not happen.
		 */
		admhc_vdbg(ahcd, "Resume Detect\n");
		admhc_intr_ack(ahcd, ADMHC_INTR_RESI);
		hcd->poll_rh = 1;
		if (ahcd->autostop) {
			admhc_rh_resume(ahcd);
		} else
			usb_hcd_resume_root_hub(hcd);
	}

	if (ints & ADMHC_INTR_TDC) {
		admhc_intr_ack(ahcd, ADMHC_INTR_TDC);
		if (HC_IS_RUNNING(hcd->state))
			admhc_intr_disable(ahcd, ADMHC_INTR_TDC);
		admhc_vdbg(ahcd, "Transfer Descriptor Complete\n");
		admhc_td_complete(ahcd);
		if (HC_IS_RUNNING(hcd->state))
			admhc_intr_enable(ahcd, ADMHC_INTR_TDC);
	}

	if (ints & ADMHC_INTR_SO) {
		/* could track INTR_SO to reduce available PCI/... bandwidth */
		admhc_vdbg(ahcd, "Schedule Overrun\n");
	}

	if (ints & ADMHC_INTR_SOFI) {
		admhc_intr_ack(ahcd, ADMHC_INTR_SOFI);
		/* handle any pending ED removes */
		admhc_finish_unlinks(ahcd, admhc_frame_no(ahcd));
		admhc_sof_refill(ahcd);
	}

	if (HC_IS_RUNNING(hcd->state)) {
		admhc_intr_ack(ahcd, ints);
		admhc_intr_enable(ahcd, ADMHC_INTR_MIE);
		admhc_writel_flush(ahcd);
	}
	spin_unlock(&ahcd->lock);

	return IRQ_HANDLED;
}

/*-------------------------------------------------------------------------*/

static void admhc_stop(struct usb_hcd *hcd)
{
	struct admhcd *ahcd = hcd_to_admhcd(hcd);

	admhc_dump(ahcd, 1);

	flush_scheduled_work();

	admhc_usb_reset(ahcd);
	admhc_intr_disable(ahcd, ADMHC_INTR_MIE);

	free_irq(hcd->irq, hcd);
	hcd->irq = -1;

	remove_debug_files(ahcd);
	admhc_eds_cleanup(ahcd);
	admhc_mem_cleanup(ahcd);
}

/*-------------------------------------------------------------------------*/

/* must not be called from interrupt context */

#ifdef	CONFIG_PM

static int admhc_restart(struct admhcd *ahcd)
{
	int temp;
	int i;
	struct urb_priv *priv;

	/* mark any devices gone, so they do nothing till khubd disconnects.
	 * recycle any "live" eds/tds (and urbs) right away.
	 * later, khubd disconnect processing will recycle the other state,
	 * (either as disconnect/reconnect, or maybe someday as a reset).
	 */
	spin_lock_irq(&ahcd->lock);
	admhc_disable(ahcd);
	usb_root_hub_lost_power(admhcd_to_hcd(ahcd)->self.root_hub);
	if (!list_empty(&ahcd->pending))
		admhc_dbg(ahcd, "abort schedule...\n");
		list_for_each_entry(priv, &ahcd->pending, pending) {
		struct urb	*urb = priv->td[0]->urb;
		struct ed	*ed = priv->ed;

		switch (ed->state) {
		case ED_OPER:
			ed->state = ED_UNLINK;
			ed->hwINFO |= cpu_to_hc32(ahcd, ED_DEQUEUE);
			ed_deschedule (ahcd, ed);

			ed->ed_next = ahcd->ed_rm_list;
			ed->ed_prev = NULL;
			ahcd->ed_rm_list = ed;
			/* FALLTHROUGH */
		case ED_UNLINK:
			break;
		default:
			admhc_dbg(ahcd, "bogus ed %p state %d\n",
					ed, ed->state);
		}

		spin_lock(&urb->lock);
		urb->status = -ESHUTDOWN;
		spin_unlock(&urb->lock);
	}
	finish_unlinks(ahcd, 0);
	spin_unlock_irq(&ahcd->lock);

	/* paranoia, in case that didn't work: */

	/* empty the interrupt branches */
	for (i = 0; i < NUM_INTS; i++) ahcd->load[i] = 0;
	for (i = 0; i < NUM_INTS; i++) ahcd->hcca->int_table[i] = 0;

	/* no EDs to remove */
	ahcd->ed_rm_list = NULL;

	/* empty control and bulk lists */
	ahcd->ed_controltail = NULL;
	ahcd->ed_bulktail    = NULL;

	if ((temp = admhc_run(ahcd)) < 0) {
		admhc_err(ahcd, "can't restart, %d\n", temp);
		return temp;
	} else {
		/* here we "know" root ports should always stay powered,
		 * and that if we try to turn them back on the root hub
		 * will respond to CSC processing.
		 */
		i = ahcd->num_ports;
		while (i--)
			admhc_writel(ahcd, RH_PS_PSS,
				&ahcd->regs->portstatus[i]);
		admhc_dbg(ahcd, "restart complete\n");
	}
	return 0;
}
#endif

/*-------------------------------------------------------------------------*/

#ifdef CONFIG_MIPS_ADM5120
#include "adm5120-drv.c"
#define PLATFORM_DRIVER		usb_hcd_adm5120_driver
#endif

#if	!defined(PLATFORM_DRIVER)
#error "missing bus glue for admhc-hcd"
#endif

#define DRIVER_INFO DRIVER_DESC " " DRIVER_VERSION

static int __init admhc_hcd_mod_init(void)
{
	int retval = 0;

	if (usb_disabled())
		return -ENODEV;

	pr_info("%s: " DRIVER_INFO "\n", hcd_name);
	pr_info("%s: block sizes: ed %Zd td %Zd\n", hcd_name,
		sizeof (struct ed), sizeof (struct td));

#ifdef PLATFORM_DRIVER
	retval = platform_driver_register(&PLATFORM_DRIVER);
	if (retval < 0)
		goto error_platform;
#endif

	return retval;

#ifdef PLATFORM_DRIVER
	platform_driver_unregister(&PLATFORM_DRIVER);
error_platform:
#endif
	return retval;
}
module_init(admhc_hcd_mod_init);

static void __exit admhc_hcd_mod_exit(void)
{
	platform_driver_unregister(&PLATFORM_DRIVER);
}
module_exit(admhc_hcd_mod_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_INFO);
MODULE_LICENSE("GPL");
