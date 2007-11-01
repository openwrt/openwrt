/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 * (C) Copyright 1999 Roman Weissgaerber <weissg@vienna.at>
 * (C) Copyright 2000-2002 David Brownell <dbrownell@users.sourceforge.net>
 *
 * This file is licenced under the GPL.
 */

/*-------------------------------------------------------------------------*/

#ifdef DEBUG

static inline char *ed_typestring(int ed_type)
{
	switch (ed_type) {
	case PIPE_CONTROL:
		return "ctrl";
	case PIPE_BULK:
		return "bulk";
	case PIPE_INTERRUPT:
		return "intr";
	case PIPE_ISOCHRONOUS:
		return "isoc";
	}
	return "(bad ed_type)";
}

static inline char *ed_statestring(int state)
{
	switch (state) {
	case ED_IDLE:
		return "IDLE";
	case ED_UNLINK:
		return "UNLINK";
	case ED_OPER:
		return "OPER";
	case ED_NEW:
		return "NEW";
	}
	return "?STATE";
}

static inline char *pipestring(int pipe)
{
	return ed_typestring(usb_pipetype(pipe));
}

static inline char *td_pidstring(u32 info)
{
	switch (info & TD_DP) {
	case TD_DP_SETUP:
		return "SETUP";
	case TD_DP_IN:
		return "IN";
	case TD_DP_OUT:
		return "OUT";
	}
	return "?PID";
}

static inline char *td_togglestring(u32 info)
{
	switch (info & TD_T) {
	case TD_T_DATA0:
		return "DATA0";
	case TD_T_DATA1:
		return "DATA1";
	case TD_T_CARRY:
		return "CARRY";
	}
	return "?TOGGLE";
}

/* debug| print the main components of an URB
 * small: 0) header + data packets 1) just header
 */
static void __attribute__((unused))
urb_print(struct admhcd *ahcd, struct urb * urb, char * str, int small)
{
	unsigned int pipe = urb->pipe;

	if (!urb->dev || !urb->dev->bus) {
		admhc_dbg(ahcd, "%s URB: no dev", str);
		return;
	}

#ifndef	ADMHC_VERBOSE_DEBUG
	if (urb->status != 0)
#endif
	admhc_dbg(ahcd, "URB-%s %p dev=%d ep=%d%s-%s flags=%x len=%d/%d "
			"stat=%d\n",
			str,
			urb,
			usb_pipedevice(pipe),
			usb_pipeendpoint(pipe),
			usb_pipeout(pipe)? "out" : "in",
			pipestring(pipe),
			urb->transfer_flags,
			urb->actual_length,
			urb->transfer_buffer_length,
			urb->status);

#ifdef	ADMHC_VERBOSE_DEBUG
	if (!small) {
		int i, len;

		if (usb_pipecontrol(pipe)) {
			admhc_dbg(admhc, "setup(8): ");
			for (i = 0; i < 8 ; i++)
				printk (" %02x", ((__u8 *) urb->setup_packet) [i]);
			printk ("\n");
		}
		if (urb->transfer_buffer_length > 0 && urb->transfer_buffer) {
			admhc_dbg(admhc, "data(%d/%d): ",
				urb->actual_length,
				urb->transfer_buffer_length);
			len = usb_pipeout(pipe)?
						urb->transfer_buffer_length: urb->actual_length;
			for (i = 0; i < 16 && i < len; i++)
				printk (" %02x", ((__u8 *) urb->transfer_buffer) [i]);
			printk ("%s stat:%d\n", i < len? "...": "", urb->status);
		}
	}
#endif /* ADMHC_VERBOSE_DEBUG */
}

#define admhc_dbg_sw(ahcd, next, size, format, arg...) \
	do { \
	if (next) { \
		unsigned s_len; \
		s_len = scnprintf(*next, *size, format, ## arg ); \
		*size -= s_len; *next += s_len; \
	} else \
		admhc_dbg(ahcd,format, ## arg ); \
	} while (0);


static void admhc_dump_intr_mask(struct admhcd *ahcd, char *label, u32 mask,
		char **next, unsigned *size)
{
	admhc_dbg_sw(ahcd, next, size, "%s 0x%08x%s%s%s%s%s%s%s%s%s%s\n",
		label,
		mask,
		(mask & ADMHC_INTR_INTA) ? " INTA" : "",
		(mask & ADMHC_INTR_FATI) ? " FATI" : "",
		(mask & ADMHC_INTR_SWI) ? " SWI" : "",
		(mask & ADMHC_INTR_TDC) ? " TDC" : "",
		(mask & ADMHC_INTR_FNO) ? " FNO" : "",
		(mask & ADMHC_INTR_SO) ? " SO" : "",
		(mask & ADMHC_INTR_INSM) ? " INSM" : "",
		(mask & ADMHC_INTR_BABI) ? " BABI" : "",
		(mask & ADMHC_INTR_RESI) ? " RESI" : "",
		(mask & ADMHC_INTR_SOFI) ? " SOFI" : ""
		);
}

static void maybe_print_eds(struct admhcd *ahcd, char *label, u32 value,
		char **next, unsigned *size)
{
	if (value)
		admhc_dbg_sw(ahcd, next, size, "%s %08x\n", label, value);
}

static char *buss2string (int state)
{
	switch (state) {
	case ADMHC_BUSS_RESET:
		return "reset";
	case ADMHC_BUSS_RESUME:
		return "resume";
	case ADMHC_BUSS_OPER:
		return "operational";
	case ADMHC_BUSS_SUSPEND:
		return "suspend";
	}
	return "?state";
}

static void
admhc_dump_status(struct admhcd *ahcd, char **next, unsigned *size)
{
	struct admhcd_regs __iomem *regs = ahcd->regs;
	u32			temp;

	temp = admhc_readl(ahcd, &regs->gencontrol);
	admhc_dbg_sw(ahcd, next, size,
		"gencontrol 0x%08x%s%s%s%s\n",
		temp,
		(temp & ADMHC_CTRL_UHFE) ? " UHFE" : "",
		(temp & ADMHC_CTRL_SIR) ? " SIR" : "",
		(temp & ADMHC_CTRL_DMAA) ? " DMAA" : "",
		(temp & ADMHC_CTRL_SR) ? " SR" : ""
		);

	temp = admhc_readl(ahcd, &regs->host_control);
	admhc_dbg_sw(ahcd, next, size,
		"host_control 0x%08x BUSS=%s%s\n",
		temp,
		buss2string(temp & ADMHC_HC_BUSS),
		(temp & ADMHC_HC_DMAE) ? " DMAE" : ""
		);

	admhc_dump_intr_mask(ahcd, "int_status",
			admhc_readl(ahcd, &regs->int_status),
			next, size);
	admhc_dump_intr_mask(ahcd, "int_enable",
			admhc_readl(ahcd, &regs->int_enable),
			next, size);

	maybe_print_eds(ahcd, "hosthead",
			admhc_readl(ahcd, &regs->hosthead), next, size);
}

#define dbg_port_sw(hc,num,value,next,size) \
	admhc_dbg_sw(hc, next, size, \
		"portstatus [%d] " \
		"0x%08x%s%s%s%s%s%s%s%s%s%s%s%s\n", \
		num, temp, \
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


static void
admhc_dump_roothub(
	struct admhcd *ahcd,
	int verbose,
	char **next,
	unsigned *size)
{
	u32			temp, i;

	temp = admhc_get_rhdesc(ahcd);
	if (temp == ~(u32)0)
		return;

	if (verbose) {
		admhc_dbg_sw(ahcd, next, size,
			"rhdesc %08x%s%s%s%s%s%s PPCM=%02x%s%s%s%s NUMP=%d(%d)\n",
			temp,
			(temp & ADMHC_RH_CRWE) ? " CRWE" : "",
			(temp & ADMHC_RH_OCIC) ? " OCIC" : "",
			(temp & ADMHC_RH_LPSC) ? " LPSC" : "",
			(temp & ADMHC_RH_LPSC) ? " DRWE" : "",
			(temp & ADMHC_RH_LPSC) ? " OCI" : "",
			(temp & ADMHC_RH_LPSC) ? " LPS" : "",
			((temp & ADMHC_RH_PPCM) >> 16),
			(temp & ADMHC_RH_NOCP) ? " NOCP" : "",
			(temp & ADMHC_RH_OCPM) ? " OCPM" : "",
			(temp & ADMHC_RH_NPS) ? " NPS" : "",
			(temp & ADMHC_RH_PSM) ? " PSM" : "",
			(temp & ADMHC_RH_NUMP), ahcd->num_ports
			);
	}

	for (i = 0; i < ahcd->num_ports; i++) {
		temp = admhc_get_portstatus(ahcd, i);
		dbg_port_sw(ahcd, i, temp, next, size);
	}
}

static void admhc_dump(struct admhcd *ahcd, int verbose)
{
	admhc_dbg(ahcd, "ADMHC ahcd state\n");

	/* dumps some of the state we know about */
	admhc_dump_status(ahcd, NULL, NULL);
	admhc_dbg(ahcd,"current frame #%04x\n",
		admhc_frame_no(ahcd));

	admhc_dump_roothub(ahcd, verbose, NULL, NULL);
}

static const char data0[] = "DATA0";
static const char data1[] = "DATA1";

static void admhc_dump_td(const struct admhcd *ahcd, const char *label,
		const struct td *td)
{
	u32	tmp;

	admhc_dbg(ahcd, "%s td %p; urb %p index %d; hwNextTD %08x\n",
		label, td,
		td->urb, td->index,
		hc32_to_cpup(ahcd, &td->hwNextTD));

	tmp = hc32_to_cpup(ahcd, &td->hwINFO);
	admhc_dbg(ahcd, "     status %08x%s CC=%x EC=%d %s %s ISI=%x FN=%x\n",
		tmp,
		(tmp & TD_OWN) ? " OWN" : "",
		TD_CC_GET(tmp),
		TD_EC_GET(tmp),
		td_togglestring(tmp),
		td_pidstring(tmp),
		TD_ISI_GET(tmp),
		TD_FN_GET(tmp));

	tmp = hc32_to_cpup(ahcd, &td->hwCBL);
	admhc_dbg(ahcd, "     dbp %08x; cbl %08x; LEN=%d%s\n",
		hc32_to_cpup(ahcd, &td->hwDBP),
		tmp,
		TD_BL_GET(tmp),
		(tmp & TD_IE) ? " IE" : "");
}

static void admhc_dump_up(const struct admhcd *ahcd, const char *label,
		const struct urb_priv *up)
{
	int i;

	admhc_dbg(ahcd, "%s urb/%p:\n", label, up->urb);
	for (i = 0; i < up->td_cnt; i++) {
		struct td *td = up->td[i];
		admhc_dump_td(ahcd, "    ->", td);
	}
}

/* caller MUST own hcd spinlock if verbose is set! */
static void __attribute__((unused))
admhc_dump_ed(const struct admhcd *ahcd, const char *label,
		const struct ed *ed, int verbose)
{
	u32 tmp = hc32_to_cpu(ahcd, ed->hwINFO);

	admhc_dbg(ahcd, "%s ed %p %s type %s; next ed %08x\n",
		label,
		ed, ed_statestring(ed->state), ed_typestring(ed->type),
		hc32_to_cpup(ahcd, &ed->hwNextED));

	admhc_dbg(ahcd, "  info %08x MAX=%d%s%s%s%s EP=%d DEV=%d\n", tmp,
		ED_MPS_GET(tmp),
		(tmp & ED_ISO) ? " ISO" : "",
		(tmp & ED_SKIP) ? " SKIP" : "",
		(tmp & ED_SPEED_FULL) ? " FULL" : " LOW",
		(tmp & ED_INT) ? " INT" : "",
		ED_EN_GET(tmp),
		ED_FA_GET(tmp));

	tmp = hc32_to_cpup(ahcd, &ed->hwHeadP);
	admhc_dbg(ahcd, "  tds: head %08x tail %08x %s%s\n",
		tmp & TD_MASK,
		hc32_to_cpup(ahcd, &ed->hwTailP),
		(tmp & ED_C) ? data1 : data0,
		(tmp & ED_H) ? " HALT" : "");

	if (ed->urb_active)
		admhc_dump_up(ahcd, "  active ", ed->urb_active);

	if ((verbose) && (!list_empty(&ed->urb_pending))) {
		struct list_head *entry;
		/* dump pending URBs */
		list_for_each(entry, &ed->urb_pending) {
			struct urb_priv *up;
			up = list_entry(entry, struct urb_priv, pending);
			admhc_dump_up(ahcd, "  pending ", up);
		}
	}
}

#else /* ifdef DEBUG */

static inline void urb_print(struct urb * urb, char * str, int small) {}
static inline void admhc_dump_up(const struct admhcd *ahcd, const char *label,
	const struct urb_priv *up) {}
static inline void admhc_dump_ed(const struct admhcd *ahcd, const char *label,
	const struct ed *ed, int verbose) {}
static inline void admhc_dump(struct admhcd *ahcd, int verbose) {}

#undef ADMHC_VERBOSE_DEBUG

#endif /* DEBUG */

/*-------------------------------------------------------------------------*/

#ifdef STUB_DEBUG_FILES

static inline void create_debug_files(struct admhcd *bus) { }
static inline void remove_debug_files(struct admhcd *bus) { }

#else

static ssize_t
show_urb_priv(struct admhcd *ahcd, char *buf, size_t count,
		struct urb_priv *up)
{
	unsigned temp, size = count;
	int i;

	if (!up)
		return 0;

	temp = scnprintf(buf, size,"\n\turb %p ", up->urb);
	size -= temp;
	buf += temp;

	for (i = 0; i< up->td_cnt; i++) {
		struct td *td;
		u32 dbp, cbl, info;

		td = up->td[i];
		info = hc32_to_cpup(ahcd, &td->hwINFO);
		dbp = hc32_to_cpup(ahcd, &td->hwDBP);
		cbl = hc32_to_cpup(ahcd, &td->hwCBL);

		temp = scnprintf(buf, size,
			"\n\t\ttd %p %s %d %s%scc=%x (%08x,%08x)",
			td,
			td_pidstring(info),
			TD_BL_GET(cbl),
			(info & TD_OWN) ? "WORK " : "DONE ",
			(cbl & TD_IE) ? "IE " : "",
			TD_CC_GET(info), info, cbl);
		size -= temp;
		buf += temp;
	}

	return count - size;
}

static ssize_t
show_list(struct admhcd *ahcd, char *buf, size_t count, struct ed *ed)
{
	unsigned		temp, size = count;

	if (!ed)
		return 0;

	/* dump a snapshot of the bulk or control schedule */
	while (ed) {
		u32 info = hc32_to_cpu(ahcd, ed->hwINFO);
		u32 headp = hc32_to_cpu(ahcd, ed->hwHeadP);

		temp = scnprintf(buf, size,
			"ed/%p %s %cs dev%d ep%d %s%smax %d %08x%s%s %s",
			ed,
			ed_typestring (ed->type),
			(info & ED_SPEED_FULL) ? 'f' : 'l',
			info & ED_FA_MASK,
			(info >> ED_EN_SHIFT) & ED_EN_MASK,
			(info & ED_INT) ? "INT " : "",
			(info & ED_ISO) ? "ISO " : "",
			(info >> ED_MPS_SHIFT) & ED_MPS_MASK ,
			info,
			(info & ED_SKIP) ? " S" : "",
			(headp & ED_H) ? " H" : "",
			(headp & ED_C) ? "DATA1" : "DATA0");
		size -= temp;
		buf += temp;

		if (ed->urb_active) {
			temp = scnprintf(buf, size, "\nactive urb:");
			size -= temp;
			buf += temp;

			temp = show_urb_priv(ahcd, buf, size, ed->urb_active);
			size -= temp;
			buf += temp;
		}

		if (!list_empty(&ed->urb_pending)) {
			struct list_head *entry;

			temp = scnprintf(buf, size, "\npending urbs:");
			size -= temp;
			buf += temp;

			list_for_each(entry, &ed->urb_pending) {
				struct urb_priv *up;
				up = list_entry(entry, struct urb_priv,
					pending);

				temp = show_urb_priv(ahcd, buf, size, up);
				size -= temp;
				buf += temp;
			}
		}

		temp = scnprintf(buf, size, "\n");
		size -= temp;
		buf += temp;

		ed = ed->ed_next;
	}

	return count - size;
}

static ssize_t
show_async(struct class_device *class_dev, char *buf)
{
	struct usb_bus		*bus;
	struct usb_hcd		*hcd;
	struct admhcd		*ahcd;
	size_t			temp;
	unsigned long		flags;

	bus = class_get_devdata(class_dev);
	hcd = bus_to_hcd(bus);
	ahcd = hcd_to_admhcd(hcd);

	/* display control and bulk lists together, for simplicity */
	spin_lock_irqsave(&ahcd->lock, flags);
	temp = show_list(ahcd, buf, PAGE_SIZE, ahcd->ed_head);
	spin_unlock_irqrestore(&ahcd->lock, flags);

	return temp;
}
static CLASS_DEVICE_ATTR(async, S_IRUGO, show_async, NULL);


#define DBG_SCHED_LIMIT 64

static ssize_t
show_periodic(struct class_device *class_dev, char *buf)
{
#if 0
	struct usb_bus		*bus;
	struct usb_hcd		*hcd;
	struct admhcd		*ahcd;
	struct ed		**seen, *ed;
	unsigned long		flags;
	unsigned		temp, size, seen_count;
	char			*next;
	unsigned		i;

	if (!(seen = kmalloc(DBG_SCHED_LIMIT * sizeof *seen, GFP_ATOMIC)))
		return 0;
	seen_count = 0;

	bus = class_get_devdata(class_dev);
	hcd = bus_to_hcd(bus);
	ahcd = hcd_to_admhcd(hcd);
	next = buf;
	size = PAGE_SIZE;

	temp = scnprintf(next, size, "size = %d\n", NUM_INTS);
	size -= temp;
	next += temp;

	/* dump a snapshot of the periodic schedule (and load) */
	spin_lock_irqsave(&ahcd->lock, flags);
	for (i = 0; i < NUM_INTS; i++) {
		if (!(ed = ahcd->periodic [i]))
			continue;

		temp = scnprintf(next, size, "%2d [%3d]:", i, ahcd->load [i]);
		size -= temp;
		next += temp;

		do {
			temp = scnprintf(next, size, " ed%d/%p",
				ed->interval, ed);
			size -= temp;
			next += temp;
			for (temp = 0; temp < seen_count; temp++) {
				if (seen [temp] == ed)
					break;
			}

			/* show more info the first time around */
			if (temp == seen_count) {
				u32	info = hc32_to_cpu (ahcd, ed->hwINFO);
				struct list_head	*entry;
				unsigned		qlen = 0;

				/* qlen measured here in TDs, not urbs */
				list_for_each (entry, &ed->td_list)
					qlen++;
				temp = scnprintf(next, size,
					" (%cs dev%d ep%d%s qlen %u"
					" max %d %08x%s%s)",
					(info & ED_SPEED_FULL) ? 'f' : 'l',
					ED_FA_GET(info),
					ED_EN_GET(info),
					(info & ED_ISO) ? "iso" : "int",
					qlen,
					ED_MPS_GET(info),
					info,
					(info & ED_SKIP) ? " K" : "",
					(ed->hwHeadP &
						cpu_to_hc32(ahcd, ED_H)) ?
							" H" : "");
				size -= temp;
				next += temp;

				if (seen_count < DBG_SCHED_LIMIT)
					seen [seen_count++] = ed;

				ed = ed->ed_next;

			} else {
				/* we've seen it and what's after */
				temp = 0;
				ed = NULL;
			}

		} while (ed);

		temp = scnprintf(next, size, "\n");
		size -= temp;
		next += temp;
	}
	spin_unlock_irqrestore(&ahcd->lock, flags);
	kfree (seen);

	return PAGE_SIZE - size;
#else
	return 0;
#endif
}
static CLASS_DEVICE_ATTR(periodic, S_IRUGO, show_periodic, NULL);


#undef DBG_SCHED_LIMIT

static ssize_t
show_registers(struct class_device *class_dev, char *buf)
{
	struct usb_bus		*bus;
	struct usb_hcd		*hcd;
	struct admhcd		*ahcd;
	struct admhcd_regs __iomem *regs;
	unsigned long		flags;
	unsigned		temp, size;
	char			*next;
	u32			rdata;

	bus = class_get_devdata(class_dev);
	hcd = bus_to_hcd(bus);
	ahcd = hcd_to_admhcd(hcd);
	regs = ahcd->regs;
	next = buf;
	size = PAGE_SIZE;

	spin_lock_irqsave(&ahcd->lock, flags);

	/* dump driver info, then registers in spec order */

	admhc_dbg_sw(ahcd, &next, &size,
		"bus %s, device %s\n"
		"%s\n"
		"%s version " DRIVER_VERSION "\n",
		hcd->self.controller->bus->name,
		hcd->self.controller->bus_id,
		hcd->product_desc,
		hcd_name);

	if (bus->controller->power.power_state.event) {
		size -= scnprintf(next, size,
			"SUSPENDED (no register access)\n");
		goto done;
	}

	admhc_dump_status(ahcd, &next, &size);

	/* other registers mostly affect frame timings */
	rdata = admhc_readl(ahcd, &regs->fminterval);
	temp = scnprintf(next, size,
			"fmintvl 0x%08x %sFSLDP=0x%04x FI=0x%04x\n",
			rdata, (rdata & ADMHC_SFI_FIT) ? "FIT " : "",
			(rdata >> ADMHC_SFI_FSLDP_SHIFT) & ADMHC_SFI_FSLDP_MASK,
			rdata  & ADMHC_SFI_FI_MASK);
	size -= temp;
	next += temp;

	rdata = admhc_readl(ahcd, &regs->fmnumber);
	temp = scnprintf(next, size, "fmnumber 0x%08x %sFR=0x%04x FN=%04x\n",
			rdata, (rdata & ADMHC_SFN_FRT) ? "FRT " : "",
			(rdata >> ADMHC_SFN_FR_SHIFT) & ADMHC_SFN_FR_MASK,
			rdata  & ADMHC_SFN_FN_MASK);
	size -= temp;
	next += temp;

	/* TODO: use predefined bitmask */
	rdata = admhc_readl(ahcd, &regs->lsthresh);
	temp = scnprintf(next, size, "lsthresh 0x%04x\n",
			rdata & 0x3fff);
	size -= temp;
	next += temp;

	temp = scnprintf(next, size, "hub poll timer: %s\n",
			admhcd_to_hcd(ahcd)->poll_rh ? "ON" : "OFF");
	size -= temp;
	next += temp;

	/* roothub */
	admhc_dump_roothub(ahcd, 1, &next, &size);

done:
	spin_unlock_irqrestore(&ahcd->lock, flags);
	return PAGE_SIZE - size;
}
static CLASS_DEVICE_ATTR(registers, S_IRUGO, show_registers, NULL);


static inline void create_debug_files (struct admhcd *ahcd)
{
	struct class_device *cldev = admhcd_to_hcd(ahcd)->self.class_dev;
	int retval;

	retval = class_device_create_file(cldev, &class_device_attr_async);
	retval = class_device_create_file(cldev, &class_device_attr_periodic);
	retval = class_device_create_file(cldev, &class_device_attr_registers);
	admhc_dbg(ahcd, "created debug files\n");
}

static inline void remove_debug_files (struct admhcd *ahcd)
{
	struct class_device *cldev = admhcd_to_hcd(ahcd)->self.class_dev;

	class_device_remove_file(cldev, &class_device_attr_async);
	class_device_remove_file(cldev, &class_device_attr_periodic);
	class_device_remove_file(cldev, &class_device_attr_registers);
}

#endif

/*-------------------------------------------------------------------------*/

