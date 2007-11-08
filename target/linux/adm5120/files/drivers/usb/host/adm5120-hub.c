/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2004 David Brownell <dbrownell@users.sourceforge.net>
 *
 * This file is licenced under GPL
 */

/*-------------------------------------------------------------------------*/

/*
 * OHCI Root Hub ... the nonsharable stuff
 */

#define dbg_port(hc,label,num,value) \
	admhc_dbg(hc, \
		"%s port%d " \
		"= 0x%08x%s%s%s%s%s%s%s%s%s%s%s%s\n", \
		label, num, temp, \
		(temp & ADMHC_PS_PRSC) ? " PRSC" : "", \
		(temp & ADMHC_PS_OCIC) ? " OCIC" : "", \
		(temp & ADMHC_PS_PSSC) ? " PSSC" : "", \
		(temp & ADMHC_PS_PESC) ? " PESC" : "", \
		(temp & ADMHC_PS_CSC) ? " CSC" : "", \
		\
		(temp & ADMHC_PS_LSDA) ? " LSDA" : "", \
		(temp & ADMHC_PS_PPS) ? " PPS" : "", \
		(temp & ADMHC_PS_PRS) ? " PRS" : "", \
		(temp & ADMHC_PS_POCI) ? " POCI" : "", \
		(temp & ADMHC_PS_PSS) ? " PSS" : "", \
		\
		(temp & ADMHC_PS_PES) ? " PES" : "", \
		(temp & ADMHC_PS_CCS) ? " CCS" : "" \
		);

#define dbg_port_write(hc,label,num,value) \
	admhc_dbg(hc, \
		"%s port%d " \
		"= 0x%08x%s%s%s%s%s%s%s%s%s%s%s%s\n", \
		label, num, temp, \
		(temp & ADMHC_PS_PRSC) ? " PRSC" : "", \
		(temp & ADMHC_PS_OCIC) ? " OCIC" : "", \
		(temp & ADMHC_PS_PSSC) ? " PSSC" : "", \
		(temp & ADMHC_PS_PESC) ? " PESC" : "", \
		(temp & ADMHC_PS_CSC) ? " CSC" : "", \
		\
		(temp & ADMHC_PS_CPP) ? " CPP" : "", \
		(temp & ADMHC_PS_SPP) ? " SPP" : "", \
		(temp & ADMHC_PS_SPR) ? " SPR" : "", \
		(temp & ADMHC_PS_CPS) ? " CPS" : "", \
		(temp & ADMHC_PS_SPS) ? " SPS" : "", \
		\
		(temp & ADMHC_PS_SPE) ? " SPE" : "", \
		(temp & ADMHC_PS_CPE) ? " CPE" : "" \
		);

/*-------------------------------------------------------------------------*/

/* hcd->hub_irq_enable() */
static void admhc_hub_irq_enable(struct usb_hcd *hcd)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);

	spin_lock_irq(&ahcd->lock);
	if (!ahcd->autostop)
		del_timer(&hcd->rh_timer);	/* Prevent next poll */
	admhc_intr_enable(ahcd, ADMHC_INTR_INSM);
	spin_unlock_irq(&ahcd->lock);
}

/*-------------------------------------------------------------------------*/

/* build "status change" packet (one or two bytes) from HC registers */

static int
admhc_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);
	int		i, changed = 0, length = 1;
	int		any_connected = 0;
	unsigned long	flags;
	u32		status;

	spin_lock_irqsave(&ahcd->lock, flags);
	if (!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags))
		goto done;

	/* init status */
	status = admhc_get_rhdesc(ahcd);
	if (status & (ADMHC_RH_LPSC | ADMHC_RH_OCIC))
		buf [0] = changed = 1;
	else
		buf [0] = 0;
	if (ahcd->num_ports > 7) {
		buf [1] = 0;
		length++;
	}

	/* look at each port */
	for (i = 0; i < ahcd->num_ports; i++) {
		status = admhc_get_portstatus(ahcd, i);

		/* can't autostop if ports are connected */
		any_connected |= (status & ADMHC_PS_CCS);

		if (status & (ADMHC_PS_CSC | ADMHC_PS_PESC | ADMHC_PS_PSSC
				| ADMHC_PS_OCIC | ADMHC_PS_PRSC)) {
			changed = 1;
			if (i < 7)
			    buf [0] |= 1 << (i + 1);
			else
			    buf [1] |= 1 << (i - 7);
		}
	}

	hcd->poll_rh = admhc_root_hub_state_changes(ahcd, changed,
			any_connected);

done:
	spin_unlock_irqrestore(&ahcd->lock, flags);

	return changed ? length : 0;
}

/*-------------------------------------------------------------------------*/

static void admhc_hub_descriptor(struct admhcd *ahcd,
		struct usb_hub_descriptor *desc)
{
	u32		rh = admhc_get_rhdesc(ahcd);
	u16		temp;

	desc->bDescriptorType = USB_DT_HUB;	/* Hub-descriptor */
	desc->bPwrOn2PwrGood = ADMHC_POTPGT/2;	/* use default value */
	desc->bHubContrCurrent = 0x00;	/* 0mA */

	desc->bNbrPorts = ahcd->num_ports;
	temp = 1 + (ahcd->num_ports / 8);
	desc->bDescLength = USB_DT_HUB_NONVAR_SIZE + 2 * temp;

	/* FIXME */
	temp = 0;
	if (rh & ADMHC_RH_NPS)		/* no power switching? */
	    temp |= 0x0002;
	if (rh & ADMHC_RH_PSM)		/* per-port power switching? */
	    temp |= 0x0001;
	if (rh & ADMHC_RH_NOCP)		/* no overcurrent reporting? */
	    temp |= 0x0010;
	else if (rh & ADMHC_RH_OCPM)	/* per-port overcurrent reporting? */
	    temp |= 0x0008;
	desc->wHubCharacteristics = (__force __u16)cpu_to_hc16(ahcd, temp);

	/* two bitmaps:  ports removable, and usb 1.0 legacy PortPwrCtrlMask */
	desc->bitmap [0] = 0;
	desc->bitmap [0] = ~0;
}

/*-------------------------------------------------------------------------*/

#ifdef	CONFIG_USB_OTG

static int admhc_start_port_reset(struct usb_hcd *hcd, unsigned port)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);
	u32			status;

	if (!port)
		return -EINVAL;
	port--;

	/* start port reset before HNP protocol times out */
	status = admhc_readl(ahcd, &ahcd->regs->portstatus[port]);
	if (!(status & ADMHC_PS_CCS))
		return -ENODEV;

	/* khubd will finish the reset later */
	admhc_writel(ahcd, ADMHC_PS_PRS, &ahcd->regs->portstatus[port]);
	return 0;
}

static void start_hnp(struct admhcd *ahcd);

#else

#define	admhc_start_port_reset		NULL

#endif

/*-------------------------------------------------------------------------*/


/* See usb 7.1.7.5:  root hubs must issue at least 50 msec reset signaling,
 * not necessarily continuous ... to guard against resume signaling.
 * The short timeout is safe for non-root hubs, and is backward-compatible
 * with earlier Linux hosts.
 */
#ifdef	CONFIG_USB_SUSPEND
#define	PORT_RESET_MSEC		50
#else
#define	PORT_RESET_MSEC		10
#endif

/* this timer value might be vendor-specific ... */
#define	PORT_RESET_HW_MSEC	10

/* wrap-aware logic morphed from <linux/jiffies.h> */
#define tick_before(t1,t2) ((s16)(((s16)(t1))-((s16)(t2))) < 0)

/* called from some task, normally khubd */
static inline int root_port_reset(struct admhcd *ahcd, unsigned port)
{
#if 0
	/* FIXME: revert to this when frame numbers are updated */
	__hc32 __iomem *portstat = &ahcd->regs->portstatus[port];
	u32	temp;
	u16	now = admhc_readl(ahcd, &ahcd->regs->fmnumber);
	u16	reset_done = now + PORT_RESET_MSEC;

	/* build a "continuous enough" reset signal, with up to
	 * 3msec gap between pulses.  scheduler HZ==100 must work;
	 * this might need to be deadline-scheduled.
	 */
	do {
		/* spin until any current reset finishes */
		for (;;) {
			temp = admhc_readl(ahcd, portstat);
			/* handle e.g. CardBus eject */
			if (temp == ~(u32)0)
				return -ESHUTDOWN;
			if (!(temp & ADMHC_PS_PRS))
				break;
			udelay (500);
		}

		if (!(temp & ADMHC_PS_CCS))
			break;
		if (temp & ADMHC_PS_PRSC)
			admhc_writel(ahcd, ADMHC_PS_PRSC, portstat);

		/* start the next reset, sleep till it's probably done */
		admhc_writel(ahcd, ADMHC_PS_PRS, portstat);
		msleep(PORT_RESET_HW_MSEC);
		now = admhc_readl(ahcd, &ahcd->regs->fmnumber);
	} while (tick_before(now, reset_done));
	/* caller synchronizes using PRSC */
#else
	__hc32 __iomem *portstat = &ahcd->regs->portstatus[port];
	u32	temp;
	unsigned long	reset_done = jiffies + msecs_to_jiffies(PORT_RESET_MSEC);

	/* build a "continuous enough" reset signal, with up to
	 * 3msec gap between pulses.  scheduler HZ==100 must work;
	 * this might need to be deadline-scheduled.
	 */
	do {
		/* spin until any current reset finishes */
		for (;;) {
			temp = admhc_readl(ahcd, portstat);
			/* handle e.g. CardBus eject */
			if (temp == ~(u32)0)
				return -ESHUTDOWN;
			if (!(temp & ADMHC_PS_PRS))
				break;
			udelay (500);
		}

		if (!(temp & ADMHC_PS_CCS))
			break;

		if (temp & ADMHC_PS_PRSC)
			admhc_writel(ahcd, ADMHC_PS_PRSC, portstat);

		/* start the next reset, sleep till it's probably done */
		admhc_writel(ahcd, ADMHC_PS_PRS, portstat);
		msleep(PORT_RESET_HW_MSEC);
	} while (time_before(jiffies, reset_done));

	admhc_writel(ahcd, ADMHC_PS_SPE | ADMHC_PS_CSC, portstat);
	msleep(100);
#endif
	return 0;
}

static int admhc_hub_control (
	struct usb_hcd	*hcd,
	u16		typeReq,
	u16		wValue,
	u16		wIndex,
	char		*buf,
	u16		wLength
) {
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);
	int		ports = hcd_to_bus (hcd)->root_hub->maxchild;
	u32		temp;
	int		ret = 0;

	if (unlikely(!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags)))
		return -ESHUTDOWN;

	switch (typeReq) {
	case ClearHubFeature:
		switch (wValue) {
		case C_HUB_OVER_CURRENT:
#if 0			/* FIXME */
			admhc_writel(ahcd, ADMHC_RH_OCIC,
					&ahcd->regs->roothub.status);
#endif
		case C_HUB_LOCAL_POWER:
			break;
		default:
			goto error;
		}
		break;
	case ClearPortFeature:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;

		switch (wValue) {
		case USB_PORT_FEAT_ENABLE:
			temp = ADMHC_PS_CPE;
			break;
		case USB_PORT_FEAT_SUSPEND:
			temp = ADMHC_PS_CPS;
			break;
		case USB_PORT_FEAT_POWER:
			temp = ADMHC_PS_CPP;
			break;
		case USB_PORT_FEAT_C_CONNECTION:
			temp = ADMHC_PS_CSC;
			break;
		case USB_PORT_FEAT_C_ENABLE:
			temp = ADMHC_PS_PESC;
			break;
		case USB_PORT_FEAT_C_SUSPEND:
			temp = ADMHC_PS_PSSC;
			break;
		case USB_PORT_FEAT_C_OVER_CURRENT:
			temp = ADMHC_PS_OCIC;
			break;
		case USB_PORT_FEAT_C_RESET:
			temp = ADMHC_PS_PRSC;
			break;
		default:
			goto error;
		}
		admhc_writel(ahcd, temp, &ahcd->regs->portstatus[wIndex]);
		break;
	case GetHubDescriptor:
		admhc_hub_descriptor(ahcd, (struct usb_hub_descriptor *) buf);
		break;
	case GetHubStatus:
		temp = admhc_get_rhdesc(ahcd);
		temp &= ~(ADMHC_RH_CRWE | ADMHC_RH_DRWE);
		put_unaligned(cpu_to_le32 (temp), (__le32 *) buf);
		break;
	case GetPortStatus:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;
		temp = admhc_get_portstatus(ahcd, wIndex);
		put_unaligned(cpu_to_le32 (temp), (__le32 *) buf);

		dbg_port(ahcd, "GetPortStatus", wIndex, temp);
		break;
	case SetHubFeature:
		switch (wValue) {
		case C_HUB_OVER_CURRENT:
			// FIXME:  this can be cleared, yes?
		case C_HUB_LOCAL_POWER:
			break;
		default:
			goto error;
		}
		break;
	case SetPortFeature:
		if (!wIndex || wIndex > ports)
			goto error;
		wIndex--;

		switch (wValue) {
		case USB_PORT_FEAT_ENABLE:
			admhc_writel(ahcd, ADMHC_PS_SPE,
				&ahcd->regs->portstatus[wIndex]);
			break;
		case USB_PORT_FEAT_SUSPEND:
#ifdef	CONFIG_USB_OTG
			if (hcd->self.otg_port == (wIndex + 1)
					&& hcd->self.b_hnp_enable)
				start_hnp(ahcd);
			else
#endif
			admhc_writel(ahcd, ADMHC_PS_SPS,
				&ahcd->regs->portstatus[wIndex]);
			break;
		case USB_PORT_FEAT_POWER:
			admhc_writel(ahcd, ADMHC_PS_SPP,
				&ahcd->regs->portstatus[wIndex]);
			break;
		case USB_PORT_FEAT_RESET:
			ret = root_port_reset(ahcd, wIndex);
			break;
		default:
			goto error;
		}
		break;

	default:
error:
		/* "protocol stall" on error */
		ret = -EPIPE;
	}
	return ret;
}

