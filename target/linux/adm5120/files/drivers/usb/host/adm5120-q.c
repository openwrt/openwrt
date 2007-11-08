/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 *
 * This file is licenced under the GPL.
 */

#include <linux/irq.h>

/*-------------------------------------------------------------------------*/

/*
 * URB goes back to driver, and isn't reissued.
 * It's completely gone from HC data structures.
 * PRECONDITION:  ahcd lock held, irqs blocked.
 */
static void
finish_urb(struct admhcd *ahcd, struct urb *urb)
__releases(ahcd->lock)
__acquires(ahcd->lock)
{
	urb_priv_free(ahcd, urb->hcpriv);
	urb->hcpriv = NULL;

	spin_lock(&urb->lock);
	if (likely(urb->status == -EINPROGRESS))
		urb->status = 0;

	/* report short control reads right even though the data TD always
	 * has TD_R set.  (much simpler, but creates the 1-td limit.)
	 */
	if (unlikely(urb->transfer_flags & URB_SHORT_NOT_OK)
			&& unlikely(usb_pipecontrol(urb->pipe))
			&& urb->actual_length < urb->transfer_buffer_length
			&& usb_pipein(urb->pipe)
			&& urb->status == 0) {
		urb->status = -EREMOTEIO;
#ifdef ADMHC_VERBOSE_DEBUG
		urb_print(ahcd, urb, "SHORT", usb_pipeout (urb->pipe));
#endif
	}
	spin_unlock(&urb->lock);

	switch (usb_pipetype(urb->pipe)) {
	case PIPE_ISOCHRONOUS:
		admhcd_to_hcd(ahcd)->self.bandwidth_isoc_reqs--;
		break;
	case PIPE_INTERRUPT:
		admhcd_to_hcd(ahcd)->self.bandwidth_int_reqs--;
		break;
	}

#ifdef ADMHC_VERBOSE_DEBUG
	urb_print(ahcd, urb, "FINISH", 0);
#endif

	/* urb->complete() can reenter this HCD */
	spin_unlock(&ahcd->lock);
	usb_hcd_giveback_urb(admhcd_to_hcd(ahcd), urb);
	spin_lock(&ahcd->lock);
}


/*-------------------------------------------------------------------------*
 * ED handling functions
 *-------------------------------------------------------------------------*/

static struct ed *ed_create(struct admhcd *ahcd, unsigned int type, u32 info)
{
	struct ed *ed;
	struct td *td;

	ed = ed_alloc(ahcd, GFP_ATOMIC);
	if (!ed)
		goto err;

	/* dummy td; end of td list for this ed */
	td = td_alloc(ahcd, GFP_ATOMIC);
	if (!td)
		goto err_free_ed;

	switch (type) {
	case PIPE_INTERRUPT:
		info |= ED_INT;
		break;
	case PIPE_ISOCHRONOUS:
		info |= ED_ISO;
		break;
	}

	ed->dummy = td;
	ed->state = ED_NEW;
	ed->type = type;

	ed->hwINFO = cpu_to_hc32(ahcd, info);
	ed->hwTailP = cpu_to_hc32(ahcd, td->td_dma);
	ed->hwHeadP = cpu_to_hc32(ahcd, td->td_dma);

	return ed;

err_free_ed:
	ed_free(ahcd, ed);
err:
	return NULL;
}

/* get and maybe (re)init an endpoint. init _should_ be done only as part
 * of enumeration, usb_set_configuration() or usb_set_interface().
 */
static struct ed *ed_get(struct admhcd *ahcd,	struct usb_host_endpoint *ep,
	struct usb_device *udev, unsigned int pipe, int interval)
{
	struct ed		*ed;
	unsigned long		flags;

	spin_lock_irqsave(&ahcd->lock, flags);
	ed = ep->hcpriv;
	if (!ed) {
		u32		info;

		/* FIXME: usbcore changes dev->devnum before SET_ADDRESS
		 * suceeds ... otherwise we wouldn't need "pipe".
		 */
		info = usb_pipedevice(pipe);
		info |= (ep->desc.bEndpointAddress & ~USB_DIR_IN) << ED_EN_SHIFT;
		info |= le16_to_cpu(ep->desc.wMaxPacketSize) << ED_MPS_SHIFT;
		if (udev->speed == USB_SPEED_FULL)
			info |= ED_SPEED_FULL;

		ed = ed_create(ahcd, usb_pipetype(pipe), info);
		if (ed)
			ep->hcpriv = ed;
	}
	spin_unlock_irqrestore(&ahcd->lock, flags);

	return ed;
}

/* link an ed into the HC chain */
static int ed_schedule(struct admhcd *ahcd, struct ed *ed)
{
	struct ed *old_tail;

	if (admhcd_to_hcd(ahcd)->state == HC_STATE_QUIESCING)
		return -EAGAIN;

	if (ed->state != ED_NEW)
		return 0;

	admhc_dump_ed(ahcd, "ED-SCHED", ed, 0);

	ed->state = ED_IDLE;

	ed->hwINFO &= ~cpu_to_hc32(ahcd, ED_SKIP);

	old_tail = ahcd->ed_tails[ed->type];

	ed->ed_next = old_tail->ed_next;
	if (ed->ed_next) {
		ed->ed_next->ed_prev = ed;
		ed->hwNextED = cpu_to_hc32(ahcd, ed->ed_next->dma);
	}
	ed->ed_prev = old_tail;

	old_tail->ed_next = ed;
	old_tail->hwNextED = cpu_to_hc32(ahcd, ed->dma);

	ahcd->ed_tails[ed->type] = ed;

	admhc_intr_enable(ahcd, ADMHC_INTR_SOFI);

	return 0;
}

static void ed_deschedule(struct admhcd *ahcd, struct ed *ed)
{
	admhc_dump_ed(ahcd, "ED-DESCHED", ed, 0);

	/* remove this ED from the HC list */
	ed->ed_prev->hwNextED = ed->hwNextED;

	/* and remove it from our list */
	ed->ed_prev->ed_next = ed->ed_next;

	if (ed->ed_next) {
		ed->ed_next->ed_prev = ed->ed_prev;
		ed->ed_next = NULL;
	}

	if (ahcd->ed_tails[ed->type] == ed)
		ahcd->ed_tails[ed->type] = ed->ed_prev;

	ed->state = ED_NEW;
}

static void ed_start_deschedule(struct admhcd *ahcd, struct ed *ed)
{
	admhc_dump_ed(ahcd, "ED-UNLINK", ed, 0);

	ed->hwINFO |= cpu_to_hc32(ahcd, ED_SKIP);

	ed->state = ED_UNLINK;

	/* add this ED into the remove list */
	ed->ed_rm_next = ahcd->ed_rm_list;
	ahcd->ed_rm_list = ed;

	/* SOF interrupt might get delayed; record the frame counter value that
	 * indicates when the HC isn't looking at it, so concurrent unlinks
	 * behave.  frame_no wraps every 2^16 msec, and changes right before
	 * SOF is triggered.
	 */
	ed->tick = admhc_frame_no(ahcd) + 1;

	/* enable SOF interrupt */
	admhc_intr_enable(ahcd, ADMHC_INTR_SOFI);
}

/*-------------------------------------------------------------------------*
 * TD handling functions
 *-------------------------------------------------------------------------*/

static void td_fill(struct admhcd *ahcd, u32 info, dma_addr_t data, int len,
		struct urb_priv *up)
{
	struct td *td;
	u32 cbl = 0;

	if (up->td_idx >= up->td_cnt) {
		admhc_dbg(ahcd, "td_fill error, idx=%d, cnt=%d\n", up->td_idx,
				up->td_cnt);
		return;
	}

	td = up->td[up->td_idx];
	td->data_dma = data;
	if (!len)
		data = 0;

#if 1
	if (up->td_idx == up->td_cnt-1)
#endif
		cbl |= TD_IE;

	if (data)
		cbl |= (len & TD_BL_MASK);

	info |= TD_OWN;

	/* setup hardware specific fields */
	td->hwINFO = cpu_to_hc32(ahcd, info);
	td->hwDBP = cpu_to_hc32(ahcd, data);
	td->hwCBL = cpu_to_hc32(ahcd, cbl);

	if (up->td_idx > 0)
		up->td[up->td_idx-1]->hwNextTD = cpu_to_hc32(ahcd, td->td_dma);

	up->td_idx++;
}

/*-------------------------------------------------------------------------*/

/* Prepare all TDs of a transfer, and queue them onto the ED.
 * Caller guarantees HC is active.
 * Usually the ED is already on the schedule, so TDs might be
 * processed as soon as they're queued.
 */
static void td_submit_urb(struct admhcd *ahcd, struct urb *urb)
{
	struct urb_priv	*urb_priv = urb->hcpriv;
	dma_addr_t	data;
	int		data_len = urb->transfer_buffer_length;
	int		cnt = 0;
	u32		info = 0;
	int		is_out = usb_pipeout(urb->pipe);
	u32		toggle = 0;

	/* OHCI handles the bulk/interrupt data toggles itself.  We just
	 * use the device toggle bits for resetting, and rely on the fact
	 * that resetting toggle is meaningless if the endpoint is active.
	 */

	if (usb_gettoggle(urb->dev, usb_pipeendpoint(urb->pipe), is_out)) {
		toggle = TD_T_CARRY;
	} else {
		toggle = TD_T_DATA0;
		usb_settoggle(urb->dev, usb_pipeendpoint (urb->pipe),
			is_out, 1);
	}

	urb_priv->td_idx = 0;

	if (data_len)
		data = urb->transfer_dma;
	else
		data = 0;

	/* NOTE:  TD_CC is set so we can tell which TDs the HC processed by
	 * using TD_CC_GET, as well as by seeing them on the done list.
	 * (CC = NotAccessed ... 0x0F, or 0x0E in PSWs for ISO.)
	 */
	switch (urb_priv->ed->type) {
	case PIPE_INTERRUPT:
		info = is_out
			? TD_T_CARRY | TD_SCC_NOTACCESSED | TD_DP_OUT
			: TD_T_CARRY | TD_SCC_NOTACCESSED | TD_DP_IN;

		/* setup service interval and starting frame number */
		info |= (urb->start_frame & TD_FN_MASK);
		info |= (urb->interval & TD_ISI_MASK) << TD_ISI_SHIFT;

		td_fill(ahcd, info, data, data_len, urb_priv);
		cnt++;

		admhcd_to_hcd(ahcd)->self.bandwidth_int_reqs++;
		break;

	case PIPE_BULK:
		info = is_out
			? TD_SCC_NOTACCESSED | TD_DP_OUT
			: TD_SCC_NOTACCESSED | TD_DP_IN;

		/* TDs _could_ transfer up to 8K each */
		while (data_len > TD_DATALEN_MAX) {
			td_fill(ahcd, info | ((cnt) ? TD_T_CARRY : toggle),
				data, TD_DATALEN_MAX, urb_priv);
			data += TD_DATALEN_MAX;
			data_len -= TD_DATALEN_MAX;
			cnt++;
		}

		td_fill(ahcd, info | ((cnt) ? TD_T_CARRY : toggle), data,
			data_len, urb_priv);
		cnt++;

		if ((urb->transfer_flags & URB_ZERO_PACKET)
				&& (cnt < urb_priv->td_cnt)) {
			td_fill(ahcd, info | ((cnt) ? TD_T_CARRY : toggle),
				0, 0, urb_priv);
			cnt++;
		}
		break;

	/* control manages DATA0/DATA1 toggle per-request; SETUP resets it,
	 * any DATA phase works normally, and the STATUS ack is special.
	 */
	case PIPE_CONTROL:
		/* fill a TD for the setup */
		info = TD_SCC_NOTACCESSED | TD_DP_SETUP | TD_T_DATA0;
		td_fill(ahcd, info, urb->setup_dma, 8, urb_priv);
		cnt++;

		if (data_len > 0) {
			/* fill a TD for the data */
			info = TD_SCC_NOTACCESSED | TD_T_DATA1;
			info |= is_out ? TD_DP_OUT : TD_DP_IN;
			/* NOTE:  mishandles transfers >8K, some >4K */
			td_fill(ahcd, info, data, data_len, urb_priv);
			cnt++;
		}

		/* fill a TD for the ACK */
		info = (is_out || data_len == 0)
			? TD_SCC_NOTACCESSED | TD_DP_IN | TD_T_DATA1
			: TD_SCC_NOTACCESSED | TD_DP_OUT | TD_T_DATA1;
		td_fill(ahcd, info, data, 0, urb_priv);
		cnt++;

		break;

	/* ISO has no retransmit, so no toggle;
	 * Each TD could handle multiple consecutive frames (interval 1);
	 * we could often reduce the number of TDs here.
	 */
	case PIPE_ISOCHRONOUS:
		info = TD_SCC_NOTACCESSED;
		for (cnt = 0; cnt < urb->number_of_packets; cnt++) {
			int frame = urb->start_frame;

			frame += cnt * urb->interval;
			frame &= TD_FN_MASK;
			td_fill(ahcd, info | frame,
				data + urb->iso_frame_desc[cnt].offset,
				urb->iso_frame_desc[cnt].length,
				urb_priv);
		}
		admhcd_to_hcd(ahcd)->self.bandwidth_isoc_reqs++;
		break;
	}

	if (urb_priv->td_cnt != cnt)
		admhc_err(ahcd, "bad number of tds created for urb %p\n", urb);

	urb_priv->td_idx = 0;
}

/* calculate transfer length/status and update the urb
 * PRECONDITION:  irqsafe (only for urb->status locking)
 */
static int td_done(struct admhcd *ahcd, struct urb *urb, struct td *td)
{
	u32	info = hc32_to_cpup(ahcd, &td->hwINFO);
	u32	dbp = hc32_to_cpup(ahcd, &td->hwDBP);
	u32	cbl = TD_BL_GET(hc32_to_cpup(ahcd, &td->hwCBL));
	int	type = usb_pipetype(urb->pipe);
	int	cc;

	cc = TD_CC_GET(info);

	/* ISO ... drivers see per-TD length/status */
	if (type == PIPE_ISOCHRONOUS) {
#if 0
		/* TODO */
		int	dlen = 0;

		/* NOTE:  assumes FC in tdINFO == 0, and that
		 * only the first of 0..MAXPSW psws is used.
		 */

		cc = TD_CC_GET(td);
		if (tdINFO & TD_CC)	/* hc didn't touch? */
			return;

		if (usb_pipeout (urb->pipe))
			dlen = urb->iso_frame_desc[td->index].length;
		else {
			/* short reads are always OK for ISO */
			if (cc == TD_DATAUNDERRUN)
				cc = TD_CC_NOERROR;
			dlen = tdPSW & 0x3ff;
		}

		urb->actual_length += dlen;
		urb->iso_frame_desc[td->index].actual_length = dlen;
		urb->iso_frame_desc[td->index].status = cc_to_error[cc];

		if (cc != TD_CC_NOERROR)
			admhc_vdbg (ahcd,
				"urb %p iso td %p (%d) len %d cc %d\n",
				urb, td, 1 + td->index, dlen, cc);
#endif
	/* BULK, INT, CONTROL ... drivers see aggregate length/status,
	 * except that "setup" bytes aren't counted and "short" transfers
	 * might not be reported as errors.
	 */
	} else {
		admhc_dump_td(ahcd, "td_done", td);

		/* count all non-empty packets except control SETUP packet */
		if ((type != PIPE_CONTROL || td->index != 0) && dbp != 0) {
			urb->actual_length += dbp - td->data_dma + cbl;
		}
	}

	return cc;
}

/*-------------------------------------------------------------------------*/

static inline struct td *
ed_halted(struct admhcd *ahcd, struct td *td, int cc, struct td *rev)
{
#if 0
	struct urb		*urb = td->urb;
	struct ed		*ed = td->ed;
	struct list_head	*tmp = td->td_list.next;
	__hc32			toggle = ed->hwHeadP & cpu_to_hc32 (ahcd, ED_C);

	admhc_dump_ed(ahcd, "ed halted", td->ed, 1);
	/* clear ed halt; this is the td that caused it, but keep it inactive
	 * until its urb->complete() has a chance to clean up.
	 */
	ed->hwINFO |= cpu_to_hc32 (ahcd, ED_SKIP);
	wmb ();
	ed->hwHeadP &= ~cpu_to_hc32 (ahcd, ED_H);

	/* put any later tds from this urb onto the donelist, after 'td',
	 * order won't matter here: no errors, and nothing was transferred.
	 * also patch the ed so it looks as if those tds completed normally.
	 */
	while (tmp != &ed->td_list) {
		struct td	*next;
		__hc32		info;

		next = list_entry(tmp, struct td, td_list);
		tmp = next->td_list.next;

		if (next->urb != urb)
			break;

		/* NOTE: if multi-td control DATA segments get supported,
		 * this urb had one of them, this td wasn't the last td
		 * in that segment (TD_R clear), this ed halted because
		 * of a short read, _and_ URB_SHORT_NOT_OK is clear ...
		 * then we need to leave the control STATUS packet queued
		 * and clear ED_SKIP.
		 */
		info = next->hwINFO;
#if 0		/* FIXME */
		info |= cpu_to_hc32 (ahcd, TD_DONE);
		info &= ~cpu_to_hc32 (ahcd, TD_CC);
#endif
		next->hwINFO = info;

		next->next_dl_td = rev;
		rev = next;

		ed->hwHeadP = next->hwNextTD | toggle;
	}

	/* help for troubleshooting:  report anything that
	 * looks odd ... that doesn't include protocol stalls
	 * (or maybe some other things)
	 */
	switch (cc) {
	case TD_CC_DATAUNDERRUN:
		if ((urb->transfer_flags & URB_SHORT_NOT_OK) == 0)
			break;
		/* fallthrough */
	case TD_CC_STALL:
		if (usb_pipecontrol (urb->pipe))
			break;
		/* fallthrough */
	default:
		admhc_dbg (ahcd,
			"urb %p path %s ep%d%s %08x cc %d --> status %d\n",
			urb, urb->dev->devpath,
			usb_pipeendpoint (urb->pipe),
			usb_pipein (urb->pipe) ? "in" : "out",
			hc32_to_cpu(ahcd, td->hwINFO),
			cc, cc_to_error [cc]);
	}

	return rev;
#else
	return NULL;
#endif
}

/*-------------------------------------------------------------------------*/

static int ed_next_urb(struct admhcd *ahcd, struct ed *ed)
{
	struct urb_priv *up;
	u32 carry;

	if (ed->state != ED_IDLE)
		return 1;

	if (ed->urb_active)
		return 1;

	if (list_empty(&ed->urb_pending))
		return 0;

	up = list_entry(ed->urb_pending.next, struct urb_priv, pending);
	list_del(&up->pending);
	ed->urb_active = up;
	ed->state = ED_OPER;

#ifdef ADMHC_VERBOSE_DEBUG
	urb_print(ahcd, up->urb, "NEXT", 0);
	admhc_dump_ed(ahcd, " ", ed, 0);
#endif

	up->td[up->td_cnt-1]->hwNextTD = cpu_to_hc32(ahcd, ed->dummy->td_dma);

	carry = hc32_to_cpup(ahcd, &ed->hwHeadP) & ED_C;
	ed->hwHeadP = cpu_to_hc32(ahcd, up->td[0]->td_dma | carry);
	ed->hwINFO &= ~cpu_to_hc32(ahcd, ED_SKIP);

	return 1;
}

static void ed_update(struct admhcd *ahcd, struct ed *ed, int partial)
{
	struct urb_priv *up;
	struct urb *urb;
	int cc;

	up = ed->urb_active;
	if (!up)
		return;

	urb = up->urb;

#ifdef ADMHC_VERBOSE_DEBUG
	urb_print(ahcd, urb, "UPDATE", 0);
#endif
	admhc_dump_ed(ahcd, "ED-UPDATE", ed, 1);

	cc = TD_CC_NOERROR;
	for (; up->td_idx < up->td_cnt; up->td_idx++) {
		struct td *td = up->td[up->td_idx];

		if (hc32_to_cpup(ahcd, &td->hwINFO) & TD_OWN)
			break;

		cc = td_done(ahcd, urb, td);
		if (cc != TD_CC_NOERROR) {
			admhc_vdbg(ahcd,
				"urb %p td %p (%d) cc %d, len=%d/%d\n",
				urb, td, td->index, cc,
				urb->actual_length,
				urb->transfer_buffer_length);

			up->td_idx = up->td_cnt;
			break;
		}
	}

	if ((up->td_idx != up->td_cnt) && (!partial))
		/* the URB is not completed yet */
		return;

	/* update packet status if needed (short is normally ok) */
	if (cc == TD_CC_DATAUNDERRUN
			&& !(urb->transfer_flags & URB_SHORT_NOT_OK))
		cc = TD_CC_NOERROR;

	if (cc != TD_CC_NOERROR && cc < TD_CC_HCD0) {
		spin_lock(&urb->lock);
		if (urb->status == -EINPROGRESS)
			urb->status = cc_to_error[cc];
		spin_unlock(&urb->lock);
	}

	finish_urb(ahcd, urb);

	ed->urb_active = NULL;
	ed->state = ED_IDLE;
}

/* there are some tds completed; called in_irq(), with HCD locked */
static void admhc_td_complete(struct admhcd *ahcd)
{
	struct ed *ed;

	for (ed = ahcd->ed_head; ed; ed = ed->ed_next) {
		if (ed->state != ED_OPER)
			continue;

		if (hc32_to_cpup(ahcd, &ed->hwINFO) & ED_SKIP)
			continue;

		if (hc32_to_cpup(ahcd, &ed->hwHeadP) & ED_H) {
			/* TODO */
			continue;
		}

		ed_update(ahcd, ed, 0);
	}
}

/* there are some urbs/eds to unlink; called in_irq(), with HCD locked */
static void admhc_finish_unlinks(struct admhcd *ahcd, u16 tick)
{
	struct ed *ed;

	for (ed = ahcd->ed_head; ed; ed = ed->ed_next) {
		if (ed->state != ED_UNLINK)
			continue;

		if (likely(HC_IS_RUNNING(admhcd_to_hcd(ahcd)->state)))
			if (tick_before(tick, ed->tick))
				continue;

		/* process partial status */
		ed_update(ahcd, ed, 1);
	}
}

static void admhc_sof_refill(struct admhcd *ahcd)
{
	struct ed *ed;
	int disable_dma = 1;

	for (ed = ahcd->ed_head; ed; ed = ed->ed_next) {

		if (hc32_to_cpup(ahcd, &ed->hwHeadP) & ED_H) {
			ed_update(ahcd, ed, 1);
			ed->hwHeadP &= ~cpu_to_hc32 (ahcd, ED_H);
		}

		if (ed_next_urb(ahcd, ed)) {
			disable_dma = 0;
		} else {
			struct ed *tmp;
			tmp = ed->ed_prev;
			ed_deschedule(ahcd, ed);
			ed = tmp;
		}
	}

	if (disable_dma) {
		admhc_intr_disable(ahcd, ADMHC_INTR_SOFI);
		admhc_dma_disable(ahcd);
	} else {
		admhc_intr_enable(ahcd, ADMHC_INTR_SOFI);
		admhc_dma_enable(ahcd);
	}
}
