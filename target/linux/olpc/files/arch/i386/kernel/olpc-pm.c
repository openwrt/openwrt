/* olpc-pm.c
 * © 2006 Red Hat, Inc.
 * Portions also copyright 2006 Advanced Micro Devices, Inc.
 * GPLv2
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/suspend.h>
#include <linux/bootmem.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/mc146818rtc.h>
#include <asm/io.h>

#include <asm/olpc.h>

/* A few words about accessing the ACPI and PM registers.  Long story short,
   byte and word accesses of the ACPI and PM registers is broken.  The only
   way to do it really correctly is to use dword accesses, which we do
   throughout this code.  For more details, please consult Eratta 17 and 18
   here:

   http://www.amd.com/files/connectivitysolutions/geode/geode_gx/34472D_CS5536_B1_specupdate.pdf
*/

#define PM_IRQ 3

#define CS5536_PM_PWRBTN (1 << 8)
#define CS5536_PM_RTC    (1 << 10)

#define GPIO_WAKEUP_EC (1 << 31)
#define GPIO_WAKEUP_LID (1 << 30)

#define PM_MODE_NORMAL 0
#define PM_MODE_TEST   1
#define PM_MODE_MAX    2

/* These, and the battery EC commands, should be in an olpc.h. */
#define EC_WRITE_SCI_MASK 0x1b
#define EC_READ_SCI_MASK  0x1c

extern void do_olpc_suspend_lowlevel(void);

static struct {
	unsigned long address;
	unsigned short segment;
} ofw_bios_entry = { 0, __KERNEL_CS };

static int olpc_pm_mode = PM_MODE_NORMAL;
static unsigned long acpi_base;
static unsigned long pms_base;
static int sci_irq;
static int olpc_lid_flag;

static struct input_dev *pm_inputdev;
static struct input_dev *lid_inputdev;
static struct input_dev *ebook_inputdev;
static struct pm_ops olpc_pm_ops;

static int gpio_wake_events = 0;
static int ebook_state = -1;
static u16 olpc_wakeup_mask = 0;

struct platform_device olpc_powerbutton_dev = {
	.name = "powerbutton",
	.id = -1,
};

struct platform_device olpc_lid_dev = {
	.name = "lid",
	.id = -1,
};

static void __init init_ebook_state(void)
{
	if (olpc_ec_cmd(0x2a, NULL, 0, (unsigned char *) &ebook_state, 1)) {
		printk(KERN_WARNING "olpc-pm:  failed to get EBOOK state!\n");
		ebook_state = 0;
	}
	ebook_state &= 1;

	/* the input layer needs to know what value to default to as well */
	input_report_switch(ebook_inputdev, SW_TABLET_MODE, ebook_state);
	input_sync(ebook_inputdev);
}

static void (*battery_callback)(unsigned long);
static DEFINE_SPINLOCK(battery_callback_lock);

/* propagate_events is non-NULL if run from workqueue,
   NULL when called at init time to flush SCI queue */
static void process_sci_queue(struct work_struct *propagate_events)
{
	unsigned char data = 0;
	unsigned char battery_events = 0;
	int ret;

	do {
		ret = olpc_ec_cmd(0x84, NULL, 0, &data, 1);
		if (!ret) {
			printk(KERN_DEBUG "olpc-pm:  SCI 0x%x received\n",
					data);

			switch (data) {
			case EC_SCI_SRC_EMPTY:
			case EC_SCI_SRC_GAME:
			case EC_SCI_SRC_WLAN:
				/* we ignore these for now */
				break;
			case EC_SCI_SRC_BATERR:
				printk(KERN_ERR "olpc-pm:  Battery Management System detected an error!  Remove turnip from battery slot.\n");
			case EC_SCI_SRC_BATSOC:
			case EC_SCI_SRC_BATTERY:
			case EC_SCI_SRC_ACPWR:
				battery_events |= data;
				break;
			case EC_SCI_SRC_EBOOK:
				ebook_state = !ebook_state;
				if (propagate_events) {
					input_report_switch(ebook_inputdev,
						SW_TABLET_MODE, ebook_state);
					input_sync(ebook_inputdev);
				}
				break;
			default:
				printk(KERN_ERR "olpc-pm:  Unknown SCI event 0x%x occurred!\n", data);
			}
		}
	} while (data && !ret);

	if (battery_events && battery_callback && propagate_events) {
		void (*cbk)(unsigned long);
		
		/* Older EC versions didn't distinguish between AC and battery
		   events */
		if (olpc_platform_info.ecver < 0x45)
			battery_events = EC_SCI_SRC_BATTERY | EC_SCI_SRC_ACPWR;

		spin_lock(&battery_callback_lock);
		cbk = battery_callback;
		spin_unlock(&battery_callback_lock);

		cbk(battery_events);
	}

	if (ret)
		printk(KERN_WARNING "Failed to clear SCI queue!\n");
}

static DECLARE_WORK(sci_work, process_sci_queue);

void olpc_register_battery_callback(void (*f)(unsigned long))
{
	spin_lock(&battery_callback_lock);
	battery_callback = f;
	spin_unlock(&battery_callback_lock);
}
EXPORT_SYMBOL_GPL(olpc_register_battery_callback);

void olpc_deregister_battery_callback(void)
{
	spin_lock(&battery_callback_lock);
	battery_callback = NULL;
	spin_unlock(&battery_callback_lock);
	cancel_work_sync(&sci_work);
}
EXPORT_SYMBOL_GPL(olpc_deregister_battery_callback);


static int olpc_pm_interrupt(int irq, void *id)
{
	uint32_t sts, gpe = 0;

	sts = inl(acpi_base + PM1_STS);
	outl(sts | 0xFFFF, acpi_base + PM1_STS);

	if (olpc_get_rev() >= OLPC_REV_B2) {
		gpe = inl(acpi_base + PM_GPE0_STS);
		outl(0xFFFFFFFF, acpi_base + PM_GPE0_STS);
	}

	if (sts & CS5536_PM_PWRBTN) {
		input_report_key(pm_inputdev, KEY_POWER, 1);
		input_sync(pm_inputdev);
		printk(KERN_DEBUG "olpm-pm:  PM_PWRBTN event received\n");
		/* Do we need to delay this (and hence schedule_work)? */
		input_report_key(pm_inputdev, KEY_POWER, 0);
		input_sync(pm_inputdev);
	}

	if (gpe & GPIO_WAKEUP_EC) {
		geode_gpio_clear(OLPC_GPIO_ECSCI, GPIO_NEGATIVE_EDGE_STS);
		schedule_work(&sci_work);
	}

	if (gpe & GPIO_WAKEUP_LID) {
		/* Disable events */
		geode_gpio_clear(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
			
		/* Clear the edge */
			
		if (olpc_lid_flag)
			geode_gpio_clear(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_EN);
		else
			geode_gpio_clear(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_EN);

		/* Clear the status too */
		geode_gpio_set(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_STS);
		geode_gpio_set(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_STS);

		/* The line is high when the LID is open, but SW_LID
		 * should be high when the LID is closed, so we pass the old
		 * value of olpc_lid_flag
		 */

		input_report_switch(lid_inputdev, SW_LID, olpc_lid_flag);
		input_sync(lid_inputdev);

		/* Swap the status */
		olpc_lid_flag = !olpc_lid_flag;

		if (olpc_lid_flag)
			geode_gpio_set(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_EN);
		else
			geode_gpio_set(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_EN);

		/* re-enable the event */
		geode_gpio_set(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
	}

	return IRQ_HANDLED;
}

/*
 * For now, only support STR.  We also don't support suspending on
 * B1s, due to difficulties with the cafe FPGA.
 */
static int olpc_pm_state_valid(suspend_state_t pm_state)
{
        if (pm_state == PM_SUSPEND_MEM && olpc_rev_after(OLPC_REV_B1))
                return 1;

        return 0;
}

/* This is a catchall function for operations that just don't belong
 * anywhere else.  Later we will evaluate if these belong in the
 * individual device drivers or the firmware.
 * If you add something to this function, please explain yourself with
 * a comment.
 */

extern void gxfb_flatpanel_control(int state);

static u32 gpio_wakeup[2];
static u64 irq_sources[4];
static u64 mfgpt_irq_msr, mfgpt_nr_msr;

void olpc_fixup_wakeup(void)
{
	u32 base = geode_gpio_base();
	int i;

	/* This clears any pending events from the status register -
	 * the firmware also does this, but its possible that it tries
	 * it too early before the key has a chance to debounce
	 */

	outl((CS5536_PM_PWRBTN << 16) | 0xFFFF, acpi_base + PM1_STS);

	/* Enable the flatpanel sequencing as early as possible, because
	   it takes ~64ms to resume.  This probably belongs in the firmware */

	//gxfb_flatpanel_control(1);

	/* Restore the interrupt sources */
	wrmsrl(MSR_PIC_YSEL_LOW, irq_sources[0]);
	wrmsrl(MSR_PIC_ZSEL_LOW, irq_sources[1]);
	wrmsrl(MSR_PIC_YSEL_HIGH, irq_sources[2]);
	wrmsrl(MSR_PIC_ZSEL_HIGH, irq_sources[3]);

	/* Restore the X and Y sources for GPIO */
	outl(gpio_wakeup[0], base + GPIO_MAP_X);
	outl(gpio_wakeup[1], base + GPIO_MAP_Y);

	/* Resture the MFGPT MSRs */
	wrmsrl(MFGPT_IRQ_MSR, mfgpt_irq_msr);
	wrmsrl(MFGPT_NR_MSR, mfgpt_nr_msr);
	
	for (i=0;i<2;i++) {
		/* tell the wireless module to restart USB communication */
		olpc_ec_cmd(0x24, NULL, 0, NULL, 0);
	}
}

void olpc_fixup_sleep(void)
{
	u32 base = geode_gpio_base();
	int i;

	/* Save the X and Y sources for GPIO */
	gpio_wakeup[0] = inl(base + GPIO_MAP_X);
	gpio_wakeup[1] = inl(base + GPIO_MAP_Y);

	/* Save the Y and Z  unrestricted sources */

	rdmsrl(MSR_PIC_YSEL_LOW, irq_sources[0]);
	rdmsrl(MSR_PIC_ZSEL_LOW, irq_sources[1]);
	rdmsrl(MSR_PIC_YSEL_HIGH, irq_sources[2]);
	rdmsrl(MSR_PIC_ZSEL_HIGH, irq_sources[3]);

	/* Turn off the MFGPT timers on the way down */

	for(i = 0; i < 8; i++) {
		u32 val = geode_mfgpt_read(i, MFGPT_REG_SETUP);

		if (val & MFGPT_SETUP_SETUP) {
			val &= ~MFGPT_SETUP_CNTEN;
			geode_mfgpt_write(i, MFGPT_REG_SETUP, val);
		}
	}

	/* Save the MFGPT MSRs */
	rdmsrl(MFGPT_IRQ_MSR, mfgpt_irq_msr);
	rdmsrl(MFGPT_NR_MSR, mfgpt_nr_msr);

	if (device_may_wakeup(&olpc_powerbutton_dev.dev))
		olpc_wakeup_mask |= CS5536_PM_PWRBTN;
	else
		olpc_wakeup_mask &= ~(CS5536_PM_PWRBTN);

	if (device_may_wakeup(&olpc_lid_dev.dev)) {
		geode_gpio_set(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
		gpio_wake_events |= GPIO_WAKEUP_LID;
	} else {
		geode_gpio_clear(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
		gpio_wake_events &= ~(GPIO_WAKEUP_LID);
	}
}

static int olpc_pm_enter(suspend_state_t pm_state)
{
        /* Only STR is supported */
        if (pm_state != PM_SUSPEND_MEM)
                return -EINVAL;

	olpc_fixup_sleep();

	/* Set the GPIO wakeup bits */
	outl(gpio_wake_events, acpi_base + PM_GPE0_EN);
	outl(0xFFFFFFFF, acpi_base + PM_GPE0_STS);

        /* Save CPU state */
        do_olpc_suspend_lowlevel();

	olpc_fixup_wakeup();

	/* Restore the SCI wakeup events */
	outl(gpio_wake_events, acpi_base + PM_GPE0_EN);

	return 0;
}

int asmlinkage olpc_do_sleep(u8 sleep_state)
{
	void *pgd_addr = __va(read_cr3());
	printk(KERN_ERR "olpc_do_sleep!\n"); /* this needs to remain here so
					      * that gcc doesn't optimize
					      * away our __va! */
	/* FIXME: Set the SCI bits we want to wake up on here */

	/* FIXME:  Set any other SCI events that we might want here */

	outl((olpc_wakeup_mask << 16) | 0xFFFF, acpi_base + PM1_STS);

	/* If we are in test mode, then just return (simulate a successful
	   suspend/resume).  Otherwise, if we are doing the real thing,
	   then go for the gusto */

 	if (olpc_pm_mode != PM_MODE_TEST) {
		__asm__ __volatile__("movl %0,%%eax" : : "r" (pgd_addr));
		__asm__("call *(%%edi); cld"
			: : "D" (&ofw_bios_entry));
	}

	return 0;
}

/* This code will slowly disappear as we fixup the issues in the BIOS */

static void __init olpc_fixup_bios(void)
{
	unsigned long hi, lo;

	if (olpc_has_vsa()) {
		/* The VSA aggressively sets up the ACPI and PM register for
		 * trapping - its not enough to force these values in the BIOS -
		 * they seem to be changed during PCI init as well.
		 */

		/* Change the PM registers to decode to the DD */

		rdmsr(0x510100e2, lo, hi);
		hi |= 0x80000000;
		wrmsr(0x510100e2, lo, hi);

		/* Change the ACPI registers to decode to the DD */

		rdmsr(0x510100e3, lo, hi);
		hi |= 0x80000000;
		wrmsr(0x510100e3, lo, hi);
	}

	/* GPIO24 controls WORK_AUX */

	geode_gpio_set(OLPC_GPIO_WORKAUX, GPIO_OUTPUT_ENABLE);
	geode_gpio_set(OLPC_GPIO_WORKAUX, GPIO_OUTPUT_AUX1);

	if (olpc_get_rev() >= OLPC_REV_B2) {
		/* GPIO10 is connected to the thermal alarm */
		geode_gpio_set(OLPC_GPIO_THRM_ALRM, GPIO_INPUT_ENABLE);
		geode_gpio_set(OLPC_GPIO_THRM_ALRM, GPIO_INPUT_AUX1);

		/* Set up to get LID events */
		geode_gpio_set(OLPC_GPIO_LID, GPIO_INPUT_ENABLE);

		/* Clear edge detection and event enable for now */
		geode_gpio_clear(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
		geode_gpio_clear(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_EN);
		geode_gpio_clear(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_EN);

		geode_gpio_set(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_STS);
		geode_gpio_set(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_STS);

		/* Set the LID to cause an PME event on group 6 */
		geode_gpio_event_pme(OLPC_GPIO_LID, 6);

		/* Set PME group 6 to fire the SCI interrupt */
		geode_gpio_set_irq(6, sci_irq);
	}

	geode_gpio_set(OLPC_GPIO_ECSCI, GPIO_INPUT_ENABLE);

	/* Clear pending events */

	geode_gpio_set(OLPC_GPIO_ECSCI, GPIO_NEGATIVE_EDGE_STS);
	geode_gpio_set(OLPC_GPIO_ECSCI, GPIO_POSITIVE_EDGE_STS);

	//geode_gpio_set(OLPC_GPIO_ECSCI, GPIO_NEGATIVE_EDGE_EN);
	geode_gpio_set(OLPC_GPIO_ECSCI, GPIO_EVENTS_ENABLE);

	/* Set the SCI to cause a PME event on group 7 */
	geode_gpio_event_pme(OLPC_GPIO_ECSCI, 7);

	/* And have group 6 also fire the SCI interrupt */
	geode_gpio_set_irq(7, sci_irq);
}

/* This provides a control file for setting up testing of the
   power management system.  For now, there is just one setting:
   "test" which means that we don't actually enter the power
   off routine.
*/

static const char * const pm_states[] = {
	[PM_MODE_NORMAL]  = "normal",
	[PM_MODE_TEST]    = "test",
};

extern struct mutex pm_mutex;
extern struct kset power_subsys;

static ssize_t control_show(struct kset *s, char *buf)
{
	return sprintf(buf, "%s\n", pm_states[olpc_pm_mode]);
}

static ssize_t control_store(struct kset *s, const char *buf, size_t n)
{
	int i, len;
	char *p;

	p = memchr(buf, '\n', n);
	len = p ? p - buf : n;

	/* Grab the mutex */
	mutex_lock(&pm_mutex);
	
	for(i = 0; i < PM_MODE_MAX; i++) {
		if (!strncmp(buf, pm_states[i], len)) {
			olpc_pm_mode = i;
			break;
		}
	}

	mutex_unlock(&pm_mutex);

	return (i == PM_MODE_MAX) ? -EINVAL : n;
}
       
static struct subsys_attribute control_attr = {
	.attr = {
		.name = "olpc-pm",
		.mode = 0644,
	},
	.show = control_show,
	.store = control_store,
};

static struct attribute * olpc_attributes[] = {
	&control_attr.attr,
	NULL
};

static struct attribute_group olpc_attrs = {
	.attrs = olpc_attributes,
};

static int __init alloc_inputdevs(void)
{
	int ret = -ENOMEM;

	pm_inputdev = input_allocate_device();
	if (!pm_inputdev)
		goto err;

	pm_inputdev->name = "OLPC PM";
	pm_inputdev->phys = "olpc_pm/input0";
	set_bit(EV_KEY, pm_inputdev->evbit);
	set_bit(KEY_POWER, pm_inputdev->keybit);

	ret = input_register_device(pm_inputdev);
	if (ret) {
		printk(KERN_ERR "olpc-pm:  failed to register PM input device: %d\n", ret);
		goto err;
	}

	lid_inputdev = input_allocate_device();
	if (!lid_inputdev)
		goto err;

	lid_inputdev->name = "OLPC lid switch";
	lid_inputdev->phys = "olpc_pm/input1";
	set_bit(EV_SW, lid_inputdev->evbit);
	set_bit(SW_LID, lid_inputdev->swbit);

	ret = input_register_device(lid_inputdev);
	if (ret) {
		printk(KERN_ERR "olpc-pm:  failed to register lid input device: %d\n", ret);
		goto err;
	}

	ebook_inputdev = input_allocate_device();
	if (!ebook_inputdev)
		goto err;

	ebook_inputdev->name = "OLPC ebook switch";
	ebook_inputdev->phys = "olpc_pm/input2";
	set_bit(EV_SW, ebook_inputdev->evbit);
	set_bit(SW_TABLET_MODE, ebook_inputdev->swbit);

	ret = input_register_device(ebook_inputdev);
	if (ret) {
		printk(KERN_ERR "olpc-pm:  failed to register ebook input device: %d\n", ret);
		goto err;
	}

	return ret;
err:
	if (ebook_inputdev) {
		input_unregister_device(ebook_inputdev);
		ebook_inputdev = NULL;
	}
	if (lid_inputdev) {
		input_unregister_device(lid_inputdev);
		lid_inputdev = NULL;
	}
	if (pm_inputdev) {
		input_unregister_device(pm_inputdev);
		pm_inputdev = NULL;
	}

	return ret;
}

static int __init olpc_pm_init(void)
{
	uint32_t lo, hi;
	int ret;
	uint8_t ec_byte;

	if (!machine_is_olpc())
		return -ENODEV;

	acpi_base = geode_acpi_base();
	pms_base = geode_pms_base();

	if (!acpi_base || !pms_base)
	  return -ENODEV;

	ret = alloc_inputdevs();
	if (ret)
		return ret;

	rdmsr(0x51400020, lo, hi);
	sci_irq = (lo >> 20) & 15;

	if (sci_irq) {
		printk(KERN_INFO "SCI is mapped to IRQ %d\n", sci_irq);
	} else {
		/* Zero doesn't mean zero -- it means masked */
		printk(KERN_INFO "SCI unmapped. Mapping to IRQ 3\n");
		sci_irq = 3;
		lo |= 0x00300000;
		wrmsrl(0x51400020, lo);
	}

	olpc_fixup_bios();

	lo = inl(pms_base + PM_FSD);

	/* Lock, enable failsafe, 4 seconds */
	outl(0xc001f400, pms_base + PM_FSD);

	/* Here we set up the SCI events we're interested in during
	 * real-time.  We have no sleep button, and the RTC doesn't make
	 * sense, so set up the power button
	 */

	outl(inl(acpi_base) | ((CS5536_PM_PWRBTN) << 16), acpi_base);

	if (olpc_get_rev() >= OLPC_REV_B2) {
		gpio_wake_events |= GPIO_WAKEUP_LID;

		/* Get the current value of the GPIO, and set up the edges */
		olpc_lid_flag = geode_gpio_isset(OLPC_GPIO_LID, GPIO_READ_BACK);

		/* Watch for the opposite edge */

		if (olpc_lid_flag)
			geode_gpio_set(OLPC_GPIO_LID, GPIO_NEGATIVE_EDGE_EN);
		else
			geode_gpio_set(OLPC_GPIO_LID, GPIO_POSITIVE_EDGE_EN);

		/* Enable the event */
		geode_gpio_set(OLPC_GPIO_LID, GPIO_EVENTS_ENABLE);
	}

	/* Set up the mask for wakeups the EC will generate SCIs on */

	ret = olpc_ec_cmd(EC_READ_SCI_MASK, NULL, 0, &ec_byte, 1);
	if (ret) 
		printk(KERN_ERR "Error getting the EC SCI mask: %d\n", ret);
	
	/* Disable battery 1% charge wakeups */
	ec_byte &= ~EC_SCI_SRC_BATSOC;

	ret = olpc_ec_cmd(EC_WRITE_SCI_MASK, &ec_byte, 1, NULL, 0);
	if (ret)
		printk(KERN_ERR "Error setting the EC SCI mask: %d\n", ret);

	/* Set up the EC SCI */

	gpio_wake_events |= GPIO_WAKEUP_EC;

	outl(gpio_wake_events, acpi_base + PM_GPE0_EN);
	outl(0xFFFFFFFF, acpi_base + PM_GPE0_STS);

	/* Select level triggered in PIC */

	if (sci_irq < 8) {
		lo = inb(0x4d0);
		lo |= 1 << sci_irq;
		outb(lo, 0x4d0);
	} else {
		lo = inb(0x4d1);
		lo |= 1 << (sci_irq - 8);
		outb(lo, 0x4d1);
	}
	/* Clear pending interrupt */
	outl(inl(acpi_base) | 0xFFFF, acpi_base);
	process_sci_queue(0);	/* we just want to flush the queue here */
	init_ebook_state();

	/* Enable the interrupt */

	ret = request_irq(sci_irq, &olpc_pm_interrupt, 0, "SCI", &acpi_base);

	if (ret) {
		printk(KERN_ERR "Error registering SCI: %d\n", ret);
		return ret;
	}

	ofw_bios_entry.address = 0xF0000 + PAGE_OFFSET;
	pm_set_ops(&olpc_pm_ops);

	sysfs_create_group(&power_subsys.kobj, &olpc_attrs);

	return 0;
}


#if defined (CONFIG_RTC_DRV_CMOS) || defined (CONFIG_RTC_DRV_CMOS_MODULE)
struct resource rtc_platform_resource[2] = {
	{
		.flags		= IORESOURCE_IO,
		.start		= RTC_PORT(0),
		.end		= RTC_PORT(0) + RTC_IO_EXTENT
	},
	{
		.flags = IORESOURCE_IRQ,
		.start = 8,
		.end = 8,
	},
};


static void rtc_wake_on(struct device *dev)
{
	olpc_wakeup_mask |= CS5536_PM_RTC;
}

static void rtc_wake_off(struct device *dev)
{
	olpc_wakeup_mask &= ~(CS5536_PM_RTC);
}

static struct cmos_rtc_board_info rtc_info = {
	.rtc_day_alarm = 0,
	.rtc_mon_alarm = 0,
	.rtc_century = 0,
	.wake_on = rtc_wake_on,
	.wake_off = rtc_wake_off,
};

struct platform_device olpc_rtc_device = {
	.name = "rtc_cmos",
	.id = -1,
	.num_resources = ARRAY_SIZE(rtc_platform_resource),
	.dev.platform_data = &rtc_info,
	.resource = rtc_platform_resource,
};

static int __init olpc_platform_init(void)
{
	(void)platform_device_register(&olpc_rtc_device);
	device_init_wakeup(&olpc_rtc_device.dev, 1);

	(void)platform_device_register(&olpc_powerbutton_dev);
	device_init_wakeup(&olpc_powerbutton_dev.dev, 1);

	(void)platform_device_register(&olpc_lid_dev);
	device_init_wakeup(&olpc_lid_dev.dev, 1);

	return 0;
}
arch_initcall(olpc_platform_init);
#endif /* CONFIG_RTC_DRV_CMOS */

static void olpc_pm_exit(void)
{
	/* Clear any pending events, and disable them */
	outl(0xFFFF, acpi_base+2);

	free_irq(sci_irq, &acpi_base);
	input_unregister_device(pm_inputdev);
	input_unregister_device(lid_inputdev);
	input_unregister_device(ebook_inputdev);
}

static struct pm_ops olpc_pm_ops = {
        .valid = olpc_pm_state_valid,
        .enter = olpc_pm_enter,
};

module_init(olpc_pm_init);
module_exit(olpc_pm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_DESCRIPTION("AMD Geode power management for OLPC CL1");
