#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/hwmon-sysfs.h>
#include <linux/hwmon.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define TS0_OFF			(0)
#define TS1_OFF			(0xC)
#define TS_PD			BIT(0)
#define TS_RUN			BIT(1)
#define TS_CLOAD		BIT(2)
#define TS_AN_EN		BIT(3)
#define TS_AN_SEL		GENMASK(11, 8)
#define TS_CFG			GENMASK(23, 16)
#define TS_RDY			BIT(16)
#define TS_DOUT			GENMASK(11, 0)

#define VM0_OFF			(0x4)
#define VM1_OFF			(0x10)
#define VM_PD			BIT(0)
#define VM_RUN			BIT(1)
#define VM_CLOAD		BIT(2)
#define VM_AN_SEL		GENMASK(11, 8)
#define VM_CFG1			GENMASK(23, 16)
#define VM_CFG2			GENMASK(31, 24)
#define VM_RDY			BIT(16)
#define VM_DOUT			GENMASK(13, 0)

#define IRQ_CTRL		(0x18)
#define TS_REF			(0x1C)
#define VM_REF			(0x28)
#define TS_W_IRQ		(0)
#define TS_F_IRQ		(1)
#define VM_W_IRQ		(6)
#define VM_F_IRQ		(7)
#define CLEAR_IRQ_OFF	(4)
int dur_off[4] = { 0x20, 0x24, 0x2C, 0x30 };

#define readl_pvt(p, off)			\
		readl((p)->base + (off))
#define writel_pvt(p, off, val)			\
		writel((val), (p)->base + (off))

static DEFINE_SPINLOCK(pvt_volt_lock);

struct sf_pvt_info {
	struct device  *dev;
	struct device  *hd_temp;
	struct device  *hd_volt;
	struct reset_control  *rstc;
	void __iomem   *base;
	int     	   type;
	int  		   irq[4];
	unsigned int   ref[4];
};

int count[4] = { 0, 0, 0, 0 };

static irqreturn_t sf_pvt_int_handler(int irq, struct sf_pvt_info *pvt, int off)
{
	int int_sta, type;
	int_sta = readl_pvt(pvt, IRQ_CTRL) & 0xFFFF;
	type = (off > 1) ? (off - 4) : (off);

	writel_pvt(pvt, IRQ_CTRL, int_sta | BIT(off + CLEAR_IRQ_OFF));
	writel_pvt(pvt, IRQ_CTRL, int_sta & (~BIT(off + CLEAR_IRQ_OFF)));
	count[type]++;
	if (count[type] == 5) {
		writel_pvt(pvt, IRQ_CTRL, int_sta & (~BIT(off)));
		writel_pvt(pvt, IRQ_CTRL, int_sta | BIT(off + 2));
		printk("========================irq %d blocked !!! =====================\n",
		       type);
	}
	return IRQ_HANDLED;
}

static irqreturn_t sf_pvt_ts_w_handler(int irq, void *arg)
{
	struct sf_pvt_info *pvt = arg;
	pr_warn("Temperature is higher than warning setting %u(0.001 Celsius)\n",
		pvt->ref[0]);
	return sf_pvt_int_handler(irq, pvt, TS_W_IRQ);
}

static irqreturn_t sf_pvt_ts_f_handler(int irq, void *arg)
{
	struct sf_pvt_info *pvt = arg;
	pr_err("Temperature is higher than fatal setting %u(0.001 Celsius)\n",
	       pvt->ref[1]);
	return sf_pvt_int_handler(irq, pvt, TS_F_IRQ);
}

static irqreturn_t sf_pvt_vm_w_handler(int irq, void *arg)
{
	struct sf_pvt_info *pvt = arg;
	pr_warn("VM OUT is lower than warning setting %u(mV)\n", pvt->ref[2]);
	return sf_pvt_int_handler(irq, pvt, VM_W_IRQ);
}

static irqreturn_t sf_pvt_vm_f_handler(int irq, void *arg)
{
	struct sf_pvt_info *pvt = arg;
	pr_err("VM OUT is lower than fatal setting %u(mV)\n", pvt->ref[3]);
	return sf_pvt_int_handler(irq, pvt, VM_F_IRQ);
}

static void set_temp_cfg(struct sf_pvt_info *pvt, int mode)
{
	unsigned long cfg;
	cfg = readl_pvt(pvt, TS0_OFF);
	cfg &= ~TS_RUN;
	cfg |= TS_CLOAD;
	writel_pvt(pvt, TS0_OFF, cfg);

	cfg = readl_pvt(pvt, TS0_OFF);
	cfg &= ~(TS_AN_EN | TS_AN_SEL | TS_CFG);
	if (mode)
		cfg |= FIELD_PREP(TS_CFG, 1);
	else
		cfg &= ~FIELD_PREP(TS_CFG, 0);
	writel_pvt(pvt, TS0_OFF, cfg);

	cfg = readl_pvt(pvt, TS0_OFF);
	// cfg |= TS_RUN;
	cfg &= ~TS_CLOAD;
	writel_pvt(pvt, TS0_OFF, cfg);
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) | TS_RUN);
	return;
}

static int read_temp(struct sf_pvt_info *pvt)
{
	int ready, ts_dout;
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) & ~TS_RUN);
	do {
		ready = readl_pvt(pvt, TS1_OFF) & TS_RDY;
	} while (!ready);
	ts_dout = FIELD_PREP(TS_DOUT, readl_pvt(pvt, TS1_OFF));
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) | TS_RUN);
	return ts_dout;
}

// Temperature in 0.001 degrees Celsius
static long temp_cal(long ts, int mode)
{
	long a, b, c, cal5, Fclkm, Nbs, temp;
	if (mode) {
		a = 5910000;
		b = 2028;
		c = -16;
	} else {
		a = 4274000;
		b = 2005;
		c = -16;
	}
	cal5 = 4094;
	Fclkm = 6000;
	Nbs = ts * 10000;
	temp = (a + b * (Nbs / cal5 - 5000) + c * Fclkm) / 100;
	//temperature calibration by evb test.
	temp = (temp + 25000) * 915 / 1000 - 15000;
	return temp;
}

static int sf_temp_read(struct device *dev, enum hwmon_sensor_types type,
						u32 attr, int channel, long *val)
{
	struct sf_pvt_info *pvt = dev_get_drvdata(dev);
	int mode = (readl_pvt(pvt, TS0_OFF) & TS_CFG) ? 1 : 0;
	long ts = 0;

	if (type != hwmon_temp || attr != hwmon_temp_input) {
		return -EINVAL;
	}
	ts = (long)read_temp(pvt);
	*val = temp_cal(ts, mode);
	return 0;
}

static umode_t sf_temp_is_visible(const void *data,
								  enum hwmon_sensor_types type,
								  u32 attr, int channel)
{
	if (type != hwmon_temp || attr != hwmon_temp_input)
		return -EINVAL;

	return 0444;
}

static const struct hwmon_channel_info *pvt_temp_info[] = {
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
	NULL
};

static const struct hwmon_ops pvt_temp_ops = {
	.is_visible = sf_temp_is_visible,
	.read       = sf_temp_read,
};

static const struct hwmon_chip_info pvt_temp_chip_info = {
	.ops  = &pvt_temp_ops,
	.info = pvt_temp_info,
};

static void set_volt_cfg(struct sf_pvt_info *pvt, int port)
{
	unsigned long cfg;

	spin_lock(&pvt_volt_lock);
	cfg = readl_pvt(pvt, VM0_OFF);
	cfg &= ~VM_RUN;
	cfg |= VM_CLOAD;
	writel_pvt(pvt, VM0_OFF, cfg);

	cfg = readl_pvt(pvt, VM0_OFF);
	cfg &= ~VM_AN_SEL;
	cfg &= ~VM_CFG1;
	cfg &= ~VM_CFG2;
	cfg |= FIELD_PREP(VM_CFG2, port);
	writel_pvt(pvt, VM0_OFF, cfg);

	cfg = readl_pvt(pvt, VM0_OFF);
	cfg &= ~VM_CLOAD;
	writel_pvt(pvt, VM0_OFF, cfg);

	cfg = readl_pvt(pvt, VM1_OFF);
	cfg &= ~VM_RDY;
	writel_pvt(pvt, VM1_OFF, cfg);
	spin_unlock(&pvt_volt_lock);

	return;
}

static int read_volt(struct sf_pvt_info *pvt)
{
	int ready, vm_dout;

	spin_lock(&pvt_volt_lock);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) | VM_RUN);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_RUN);
	do {
		ready = readl_pvt(pvt, VM1_OFF) & VM_RDY;
	} while (!ready);
	vm_dout = FIELD_PREP(VM_DOUT, readl_pvt(pvt, VM1_OFF));
	spin_unlock(&pvt_volt_lock);
	return vm_dout;
}

// Voltage in millivolts
static long volt_cal(long vm)
{
	long volt, vref, n, r2;
	vref = 121160;
	r2 = 16384;
	n = vm * 100000;
	volt = (vref / 5) * (6 * n / 16384 - 3 * 100000 / r2 - 1 * 100000);
	volt = volt * 2 / 10000000;
	return volt;
}

static ssize_t
in_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int index = to_sensor_dev_attr(attr)->index;
	struct sf_pvt_info *pvt = dev_get_drvdata(dev);
	long vm = 0;
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_RUN);
	msleep(20);
	set_volt_cfg(pvt, index);
	msleep(100);
	vm = (long)read_volt(pvt);
	msleep(20);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) | VM_RUN);
	return sprintf(buf, "\n now voltage = %ld mV\n", (long)volt_cal(vm));
}

static SENSOR_DEVICE_ATTR_RO(in0_input, in, 0);
static SENSOR_DEVICE_ATTR_RO(in1_input, in, 1);
static SENSOR_DEVICE_ATTR_RO(in2_input, in, 2);
static SENSOR_DEVICE_ATTR_RO(in3_input, in, 3);
static SENSOR_DEVICE_ATTR_RO(in4_input, in, 4);
static SENSOR_DEVICE_ATTR_RO(in5_input, in, 5);
static SENSOR_DEVICE_ATTR_RO(in6_input, in, 6);
static SENSOR_DEVICE_ATTR_RO(in7_input, in, 7);
static SENSOR_DEVICE_ATTR_RO(in8_input, in, 8);
static SENSOR_DEVICE_ATTR_RO(in9_input, in, 9);
static SENSOR_DEVICE_ATTR_RO(in10_input, in, 10);
static SENSOR_DEVICE_ATTR_RO(in11_input, in, 11);
static SENSOR_DEVICE_ATTR_RO(in12_input, in, 12);
static SENSOR_DEVICE_ATTR_RO(in13_input, in, 13);
static SENSOR_DEVICE_ATTR_RO(in14_input, in, 14);
static SENSOR_DEVICE_ATTR_RO(in15_input, in, 15);

static struct attribute *pvt_volt_attr_in[] = {
	&sensor_dev_attr_in0_input.dev_attr.attr,
	&sensor_dev_attr_in1_input.dev_attr.attr,
	&sensor_dev_attr_in2_input.dev_attr.attr,
	&sensor_dev_attr_in3_input.dev_attr.attr,
	&sensor_dev_attr_in4_input.dev_attr.attr,
	&sensor_dev_attr_in5_input.dev_attr.attr,
	&sensor_dev_attr_in6_input.dev_attr.attr,
	&sensor_dev_attr_in7_input.dev_attr.attr,
	&sensor_dev_attr_in8_input.dev_attr.attr,
	&sensor_dev_attr_in9_input.dev_attr.attr,
	&sensor_dev_attr_in10_input.dev_attr.attr,
	&sensor_dev_attr_in11_input.dev_attr.attr,
	&sensor_dev_attr_in12_input.dev_attr.attr,
	&sensor_dev_attr_in13_input.dev_attr.attr,
	&sensor_dev_attr_in14_input.dev_attr.attr,
	&sensor_dev_attr_in15_input.dev_attr.attr,
	NULL
};


static long temp_to_ref(unsigned int temp, int mode)
{
	long a, b, c, cal5, Fclkm, ts;
	if (mode) {
		a = 5910000;
		b = 2028;
		c = -16;
	} else {
		a = 4274000;
		b = 2005;
		c = -16;
	}
	cal5 = 4094;
	Fclkm = 6000;
	//temperature calibration by evb test.
	temp = (temp + 15000) * 1000 / 915 - 25000;
	ts = ((long)temp * 100 - a - c * Fclkm) / b + 5000;
	ts = ts * cal5 / 10000;
	return ts;
}

static long volt_to_ref(unsigned int volt)
{
	long vm, vref, r2;
	vref = 121160;
	r2 = 16384;
	vm = (long)volt * 50000000 / (2 * vref) + 100000 + 3 * 100000 / r2;
	vm = vm * 16384 / 600000;
	return vm;
}

static void set_ref_dur(struct sf_pvt_info *pvt, u32 val, u32 dur, int type)
{
	int temp, mask = 0xFFF, off = 16, addr = TS_REF, mode = 0;
	long ref;

	/* Set interrupt trigger time */
	dur &= 0xFFFFFF;
	writel(dur, pvt->base + dur_off[type]);

	if (type == 0 || type == 2)
		off = 0;
	if (type > 1) {
		mask = 0x3FFF;
		addr = VM_REF;
	}
	temp = readl_pvt(pvt, addr);
	mode = (readl_pvt(pvt, TS0_OFF) & TS_CFG) ? 1 : 0;
	if (type < 2) {
		ref = temp_to_ref(val, mode);
	} else {
		ref = volt_to_ref(val);
	}
	ref &= mask;
	temp = (temp & (~(mask << off))) | (ref << off);
	writel_pvt(pvt, addr, temp);
	pvt->ref[type] = val;
	//printk("enable irq type:%d setting ref = %ld\n", type, set);
}


static const struct attribute_group pvt_volt_group = {
	.attrs = pvt_volt_attr_in,
};

static const struct attribute_group *pvt_volt_groups[] = {
	&pvt_volt_group,
	NULL
};

static int sf_pvt_probe(struct platform_device *pdev)
{
	struct sf_pvt_info *pvt;
	struct device_node *np;
	int rc;
	unsigned int ts_w[2], ts_f[2], vm_w[2], vm_f[2], ret;

	pvt = devm_kzalloc(&pdev->dev, sizeof(*pvt), GFP_KERNEL);
	if (!pvt)
		return -ENOMEM;
	pvt->dev = &pdev->dev;
	np = pdev->dev.of_node;

	pvt->rstc = devm_reset_control_get(&pdev->dev, NULL);
	if (IS_ERR(pvt->rstc))
		return dev_err_probe(&pdev->dev, PTR_ERR(pvt->rstc), "reset lookup error!\n");

	pvt->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(pvt->base)) {
		dev_err(&pdev->dev, "reg ioremap error!\n");
		return PTR_ERR(pvt->base);
	}

	platform_set_drvdata(pdev, pvt);

	/*init temp module*/
	reset_control_assert(pvt->rstc);
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) & ~TS_PD);
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) & ~TS_CLOAD);
	writel_pvt(pvt, TS0_OFF, readl_pvt(pvt, TS0_OFF) & ~TS_RUN);
	reset_control_deassert(pvt->rstc);

	if (of_property_read_u32(np, "mode", &rc))
		return -EINVAL;
	set_temp_cfg(pvt, rc);

	pvt->hd_temp = devm_hwmon_device_register_with_info(pvt->dev, "pvt_temp", pvt,
							    &pvt_temp_chip_info, NULL);
	if (IS_ERR(pvt->hd_temp)) {
		dev_err(&pdev->dev, "pvt temperature hwdev register error!\n");
		return PTR_ERR(pvt->hd_temp);
	}

	reset_control_assert(pvt->rstc);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_PD);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_CLOAD);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_RUN);
	reset_control_deassert(pvt->rstc);

	pvt->hd_volt = devm_hwmon_device_register_with_groups(pvt->dev, "pvt_volt",
							      pvt, pvt_volt_groups);

	if (IS_ERR(pvt->hd_volt)) {
		dev_err(&pdev->dev, "pvt voltage hwdev register error!\n");
		return PTR_ERR(pvt->hd_volt);
	}

	/*setup warning & fatal range*/
	if (of_property_read_u32_array(np, "ts-warning", ts_w, 2) ||
	    of_property_read_u32_array(np, "ts-fatal", ts_f, 2) ||
	    of_property_read_u32_array(np, "vm-warning", vm_w, 2) ||
	    of_property_read_u32_array(np, "vm-fatal", vm_f, 2))
		return -EINVAL;

	set_ref_dur(pvt, ts_w[0], ts_w[1], 0);
	set_ref_dur(pvt, ts_f[0], ts_f[1], 1);
	set_ref_dur(pvt, vm_w[0], vm_w[1], 2);
	set_ref_dur(pvt, vm_f[0], vm_f[1], 3);

	pvt->irq[0] = platform_get_irq_byname(pdev, "ts-w");
	if (pvt->irq[0] < 0)
		return pvt->irq[0];
	ret = devm_request_irq(&pdev->dev, pvt->irq[0], sf_pvt_ts_w_handler,
			       IRQF_SHARED, "sf_ts_w_intr", pvt);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	pvt->irq[1] = platform_get_irq_byname(pdev, "ts-f");
	if (pvt->irq[1] < 0)
		return pvt->irq[1];
	ret = devm_request_irq(&pdev->dev, pvt->irq[1], sf_pvt_ts_f_handler,
			       IRQF_SHARED, "sf_ts_f_intr", pvt);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	pvt->irq[2] = platform_get_irq_byname(pdev, "vm-w");
	if (pvt->irq[2] < 0)
		return pvt->irq[2];
	ret = devm_request_irq(&pdev->dev, pvt->irq[2], sf_pvt_vm_w_handler,
			       IRQF_SHARED, "sf_vm_w_intr", pvt);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	pvt->irq[3] = platform_get_irq_byname(pdev, "vm-f");
	if (pvt->irq[3] < 0)
		return pvt->irq[3];
	ret = devm_request_irq(&pdev->dev, pvt->irq[3], sf_pvt_vm_f_handler,
			       IRQF_SHARED, "sf_vm_f_intr", pvt);
	if (ret) {
		dev_err(&pdev->dev, "failed to request irq\n");
		return ret;
	}

	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) & ~VM_RUN);
	msleep(20);
	set_volt_cfg(pvt, 2);
	msleep(100);

	if ((read_volt(pvt) > volt_to_ref(vm_w[0])) &&
	    (read_temp(pvt) < temp_to_ref(ts_w[0], 0))) {
		writel_pvt(pvt, IRQ_CTRL, 0xC3);
		printk("PVT: All irq enabled.\n");
		printk("PVT: Volt Warning %u(mV), Temperature Warning %u(0.001 Celsius)\n",
		       vm_w[0], ts_w[0]);
	} else {
		writel_pvt(pvt, IRQ_CTRL, 0x0);
		printk("Already trigger warning, block all pvt irq.\n");
	}
	msleep(20);
	writel_pvt(pvt, VM0_OFF, readl_pvt(pvt, VM0_OFF) | VM_RUN);
	return 0;
}

static void sf_pvt_remove(struct platform_device *pdev)
{
	struct sf_pvt_info *pvt = platform_get_drvdata(pdev);
	reset_control_assert(pvt->rstc);
}

#ifdef CONFIG_OF
static const struct of_device_id sf_pvt_dt_match[] = {
	{
		.compatible = "siflower,sf-pvt",
	},
	{},
};
MODULE_DEVICE_TABLE(of, sf_pvt_dt_match);
#endif

static struct platform_driver sf_pvt_driver = {
	.probe  = sf_pvt_probe,
	.remove = sf_pvt_remove,
	.driver = {
		.name           = "sf_pvt",
		.of_match_table = of_match_ptr(sf_pvt_dt_match),
	},
};
module_platform_driver(sf_pvt_driver);

MODULE_DESCRIPTION("Siflower SF21 PVT (temperature/voltage) monitor");
MODULE_LICENSE("GPL");
