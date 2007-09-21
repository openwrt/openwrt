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
static void admhc_rhsc_enable(struct usb_hcd *hcd)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);

	spin_lock_irq(&ahcd->lock);
	if (!ahcd->autostop)
		del_timer(&hcd->rh_timer);	/* Prevent next poll */
	admhc_intr_enable(ahcd, ADMHC_INTR_INSM);
	spin_unlock_irq(&ahcd->lock);
}

#define OHCI_SCHED_ENABLES \
	(OHCI_CTRL_CLE|OHCI_CTRL_BLE|OHCI_CTRL_PLE|OHCI_CTRL_IE)

#ifdef	CONFIG_PM
static int admhc_restart(struct admhcd *ahcd);

static int admhc_rh_suspend(struct admhcd *ahcd, int autostop)
__releases(ahcd->lock)
__acquires(ahcd->lock)
{
	int			status = 0;

	ahcd->hc_control = admhc_readl(ahcd, &ahcd->regs->control);
	switch (ahcd->hc_control & OHCI_CTRL_HCFS) {
	case OHCI_USB_RESUME:
		admhc_dbg(ahcd, "resume/suspend?\n");
		ahcd->hc_control &= ~OHCI_CTRL_HCFS;
		ahcd->hc_control |= OHCI_USB_RESET;
		admhc_writel(ahcd, ahcd->hc_control, &ahcd->ahcd->regs->control);
		(void) admhc_readl(ahcd, &ahcd->regs->control);
		/* FALL THROUGH */
	case OHCI_USB_RESET:
		status = -EBUSY;
		admhc_dbg(ahcd, "needs reinit!\n");
		goto done;
	case OHCI_USB_SUSPEND:
		if (!ahcd->autostop) {
			admhc_dbg(ahcd, "already suspended\n");
			goto done;
		}
	}
	admhc_dbg(ahcd, "%s root hub\n",
			autostop ? "auto-stop" : "suspend");

	/* First stop any processing */
	if (!autostop && (ahcd->hc_control & OHCI_SCHED_ENABLES)) {
		ahcd->hc_control &= ~OHCI_SCHED_ENABLES;
		admhc_writel(ahcd, ahcd->hc_control, &ahcd->ahcd->regs->control);
		ahcd->hc_control = admhc_readl(ahcd, &ahcd->regs->control);
		admhc_writel(ahcd, OHCI_INTR_SF, &ahcd->regs->intrstatus);

		/* sched disables take effect on the next frame,
		 * then the last WDH could take 6+ msec
		 */
		admhc_dbg(ahcd, "stopping schedules ...\n");
		ahcd->autostop = 0;
		spin_unlock_irq (&ahcd->lock);
		msleep (8);
		spin_lock_irq (&ahcd->lock);
	}
	dl_done_list (ahcd);
	finish_unlinks (ahcd, admhc_frame_no(ahcd));

	/* maybe resume can wake root hub */
	if (device_may_wakeup(&admhcd_to_hcd(ahcd)->self.root_hub->dev) ||
			autostop)
		ahcd->hc_control |= OHCI_CTRL_RWE;
	else {
		admhc_writel(ahcd, OHCI_INTR_RHSC, &ahcd->regs->intrdisable);
		ahcd->hc_control &= ~OHCI_CTRL_RWE;
	}

	/* Suspend hub ... this is the "global (to this bus) suspend" mode,
	 * which doesn't imply ports will first be individually suspended.
	 */
	ahcd->hc_control &= ~OHCI_CTRL_HCFS;
	ahcd->hc_control |= OHCI_USB_SUSPEND;
	admhc_writel(ahcd, ahcd->hc_control, &ahcd->ahcd->regs->control);
	(void) admhc_readl(ahcd, &ahcd->regs->control);

	/* no resumes until devices finish suspending */
	if (!autostop) {
		ahcd->next_statechange = jiffies + msecs_to_jiffies (5);
		ahcd->autostop = 0;
	}

done:
	return status;
}

static inline struct ed *find_head(struct ed *ed)
{
	/* for bulk and control lists */
	while (ed->ed_prev)
		ed = ed->ed_prev;
	return ed;
}

/* caller has locked the root hub */
static int admhc_rh_resume(struct admhcd *ahcd)
__releases(ahcd->lock)
__acquires(ahcd->lock)
{
	struct usb_hcd		*hcd = admhcd_to_hcd (ahcd);
	u32			temp, enables;
	int			status = -EINPROGRESS;
	int			autostopped = ahcd->autostop;

	ahcd->autostop = 0;
	ahcd->hc_control = admhc_readl(ahcd, &ahcd->regs->control);

	if (ahcd->hc_control & (OHCI_CTRL_IR | OHCI_SCHED_ENABLES)) {
		/* this can happen after resuming a swsusp snapshot */
		if (hcd->state == HC_STATE_RESUMING) {
			admhc_dbg(ahcd, "BIOS/SMM active, control %03x\n",
					ahcd->hc_control);
			status = -EBUSY;
		/* this happens when pmcore resumes HC then root */
		} else {
			admhc_dbg(ahcd, "duplicate resume\n");
			status = 0;
		}
	} else switch (ahcd->hc_control & OHCI_CTRL_HCFS) {
	case OHCI_USB_SUSPEND:
		ahcd->hc_control &= ~(OHCI_CTRL_HCFS|OHCI_SCHED_ENABLES);
		ahcd->hc_control |= OHCI_USB_RESUME;
		admhc_writel(ahcd, ahcd->hc_control, &ahcd->ahcd->regs->control);
		(void) admhc_readl(ahcd, &ahcd->regs->control);
		admhc_dbg(ahcd, "%s root hub\n",
				autostopped ? "auto-start" : "resume");
		break;
	case OHCI_USB_RESUME:
		/* HCFS changes sometime after INTR_RD */
		admhc_dbg(ahcd, "%swakeup root hub\n",
				autostopped ? "auto-" : "");
		break;
	case OHCI_USB_OPER:
		/* this can happen after resuming a swsusp snapshot */
		admhc_dbg(ahcd, "snapshot resume? reinit\n");
		status = -EBUSY;
		break;
	default:		/* RESET, we lost power */
		admhc_dbg(ahcd, "lost power\n");
		status = -EBUSY;
	}
	if (status == -EBUSY) {
		if (!autostopped) {
			spin_unlock_irq (&ahcd->lock);
			(void) ahcd_init (ahcd);
			status = admhc_restart (ahcd);
			spin_lock_irq (&ahcd->lock);
		}
		return status;
	}
	if (status != -EINPROGRESS)
		return status;
	if (autostopped)
		goto skip_resume;
	spin_unlock_irq (&ahcd->lock);

	/* Some controllers (lucent erratum) need extra-long delays */
	msleep (20 /* usb 11.5.1.10 */ + 12 /* 32 msec counter */ + 1);

	temp = admhc_readl(ahcd, &ahcd->regs->control);
	temp &= OHCI_CTRL_HCFS;
	if (temp != OHCI_USB_RESUME) {
		admhc_err (ahcd, "controller won't resume\n");
		spin_lock_irq(&ahcd->lock);
		return -EBUSY;
	}

	/* disable old schedule state, reinit from scratch */
	admhc_writel(ahcd, 0, &ahcd->regs->ed_controlhead);
	admhc_writel(ahcd, 0, &ahcd->regs->ed_controlcurrent);
	admhc_writel(ahcd, 0, &ahcd->regs->ed_bulkhead);
	admhc_writel(ahcd, 0, &ahcd->regs->ed_bulkcurrent);
	admhc_writel(ahcd, 0, &ahcd->regs->ed_periodcurrent);
	admhc_writel(ahcd, (u32) ahcd->hcca_dma, &ahcd->ahcd->regs->hcca);

	/* Sometimes PCI D3 suspend trashes frame timings ... */
	periodic_reinit(ahcd);

	/* the following code is executed with ahcd->lock held and
	 * irqs disabled if and only if autostopped is true
	 */

skip_resume:
	/* interrupts might have been disabled */
	admhc_writel(ahcd, OHCI_INTR_INIT, &ahcd->regs->int_enable);
	if (ahcd->ed_rm_list)
		admhc_writel(ahcd, OHCI_INTR_SF, &ahcd->regs->int_enable);

	/* Then re-enable operations */
	admhc_writel(ahcd, OHCI_USB_OPER, &ahcd->regs->control);
	(void) admhc_readl(ahcd, &ahcd->regs->control);
	if (!autostopped)
		msleep (3);

	temp = ahcd->hc_control;
	temp &= OHCI_CTRL_RWC;
	temp |= OHCI_CONTROL_INIT | OHCI_USB_OPER;
	ahcd->hc_control = temp;
	admhc_writel(ahcd, temp, &ahcd->regs->control);
	(void) admhc_readl(ahcd, &ahcd->regs->control);

	/* TRSMRCY */
	if (!autostopped) {
		msleep (10);
		spin_lock_irq (&ahcd->lock);
	}
	/* now ahcd->lock is always held and irqs are always disabled */

	/* keep it alive for more than ~5x suspend + resume costs */
	ahcd->next_statechange = jiffies + STATECHANGE_DELAY;

	/* maybe turn schedules back on */
	enables = 0;
	temp = 0;
	if (!ahcd->ed_rm_list) {
		if (ahcd->ed_controltail) {
			admhc_writel(ahcd,
					find_head (ahcd->ed_controltail)->dma,
					&ahcd->regs->ed_controlhead);
			enables |= OHCI_CTRL_CLE;
			temp |= OHCI_CLF;
		}
		if (ahcd->ed_bulktail) {
			admhc_writel(ahcd, find_head (ahcd->ed_bulktail)->dma,
				&ahcd->regs->ed_bulkhead);
			enables |= OHCI_CTRL_BLE;
			temp |= OHCI_BLF;
		}
	}
	if (hcd->self.bandwidth_isoc_reqs || hcd->self.bandwidth_int_reqs)
		enables |= OHCI_CTRL_PLE|OHCI_CTRL_IE;
	if (enables) {
		admhc_dbg(ahcd, "restarting schedules ... %08x\n", enables);
		ahcd->hc_control |= enables;
		admhc_writel(ahcd, ahcd->hc_control, &ahcd->ahcd->regs->control);
		if (temp)
			admhc_writel(ahcd, temp, &ahcd->regs->cmdstatus);
		(void) admhc_readl(ahcd, &ahcd->regs->control);
	}

	return 0;
}

static int admhc_bus_suspend(struct usb_hcd *hcd)
{
	struct admhcd	*ahcd = hcd_to_admhcd(hcd);
	int		rc;

	spin_lock_irq(&ahcd->lock);

	if (unlikely(!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags)))
		rc = -ESHUTDOWN;
	else
		rc = admhc_rh_suspend (ahcd, 0);
	spin_unlock_irq(&ahcd->lock);
	return rc;
}

static int admhc_bus_resume(struct usb_hcd *hcd)
{
	struct admhcd		*ahcd = hcd_to_admhcd(hcd);
	int			rc;

	if (time_before(jiffies, ahcd->next_statechange))
		msleep(5);

	spin_lock_irq (&ahcd->lock);

	if (unlikely(!test_bit(HCD_FLAG_HW_ACCESSIBLE, &hcd->flags)))
		rc = -ESHUTDOWN;
	else
		rc = admhc_rh_resume (ahcd);
	spin_unlock_irq(&ahcd->lock);

	/* poll until we know a device is connected or we autostop */
	if (rc == 0)
		usb_hcd_poll_rh_status(hcd);
	return rc;
}

/* Carry out polling-, autostop-, and autoresume-related state changes */
static int admhc_root_hub_state_changes(struct admhcd *ahcd, int changed,
		int any_connected)
{
	int	poll_rh = 1;

	switch (ahcd->hc_control & OHCI_CTRL_HCFS) {

	case OHCI_USB_OPER:
		/* keep on polling until we know a device is connected
		 * and RHSC is enabled */
		if (!ahcd->autostop) {
			if (any_connected ||
					!device_may_wakeup(&admhcd_to_hcd(ahcd)
						->self.root_hub->dev)) {
				if (admhc_readl(ahcd, &ahcd->regs->int_enable) &
						OHCI_INTR_RHSC)
					poll_rh = 0;
			} else {
				ahcd->autostop = 1;
				ahcd->next_statechange = jiffies + HZ;
			}

		/* if no devices have been attached for one second, autostop */
		} else {
			if (changed || any_connected) {
				ahcd->autostop = 0;
				ahcd->next_statechange = jiffies +
						STATECHANGE_DELAY;
			} else if (time_after_eq(jiffies,
						ahcd->next_statechange)
					&& !ahcd->ed_rm_list
					&& !(ahcd->hc_control &
						OHCI_SCHED_ENABLES)) {
				ahcd_rh_suspend(ahcd, 1);
			}
		}
		break;

	/* if there is a port change, autostart or ask to be resumed */
	case OHCI_USB_SUSPEND:
	case OHCI_USB_RESUME:
		if (changed) {
			if (ahcd->autostop)
				admhc_rh_resume(ahcd);
			else
				usb_hcd_resume_root_hub(admhcd_to_hcd(ahcd));
		} else {
			/* everything is idle, no need for polling */
			poll_rh = 0;
		}
		break;
	}
	return poll_rh;
}

#else	/* CONFIG_PM */

static inline int admhc_rh_resume(struct admhcd *ahcd)
{
	return 0;
}

/* Carry out polling-related state changes.
 * autostop isn't used when CONFIG_PM is turned off.
 */
static int admhc_root_hub_state_changes(struct admhcd *ahcd, int changed,
		int any_connected)
{
	int	poll_rh = 1;

	/* keep on polling until RHSC is enabled */
	if (admhc_readl(ahcd, &ahcd->regs->int_enable) & ADMHC_INTR_INSM)
		poll_rh = 0;

	return poll_rh;
}

#endif	/* CONFIG_PM */

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
	int		retval = 0;

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
			retval = root_port_reset(ahcd, wIndex);
			break;
		default:
			goto error;
		}
		break;

	default:
error:
		/* "protocol stall" on error */
		retval = -EPIPE;
	}
	return retval;
}

