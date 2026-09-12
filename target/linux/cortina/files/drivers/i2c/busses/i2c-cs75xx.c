// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/io.h>

#define CS75XX_I2C_CTLR_NAME	"cs75xx-i2c"

#define CS75XX_BIW_CFG		0x00
#define CS75XX_BIW_CTRL		0x04
#define CS75XX_BIW_TXR		0x08
#define CS75XX_BIW_RXR		0x0C
#define CS75XX_BIW_ACK		0x10
#define CS75XX_BIW_IE0		0x14
#define CS75XX_BIW_INT0		0x18
#define CS75XX_BIW_IE1		0x1C
#define CS75XX_BIW_INT1		0x20
#define CS75XX_BIW_STAT		0x24

#define BIW_CFG_CORE_EN		BIT(0)
#define BIW_CFG_PRER_OFF	16
#define BIW_CFG_PRER_MASK	0xFFFF0000

#define BIW_CTRL_DONE		BIT(0)
#define BIW_CTRL_ACK_IN		BIT(3)
#define BIW_CTRL_WRITE		BIT(4)
#define BIW_CTRL_READ		BIT(5)
#define BIW_CTRL_STOP		BIT(6)
#define BIW_CTRL_START		BIT(7)

#define BIW_ACK_AL		BIT(0)
#define BIW_ACK_BUSY		BIT(1)
#define BIW_ACK_ACK_OUT		BIT(2)

#define BIW_INT_BIT		BIT(0)

#define DEFAULT_TIMEOUT_MS	100
#define DEFAULT_RETRIES		3


enum {
	CS75XX_I2C_STATE_INVALID,
	CS75XX_I2C_STATE_IDLE,
	CS75XX_I2C_STATE_START,
	CS75XX_I2C_STATE_WAITING_FOR_ADDR_1_ACK,
	CS75XX_I2C_STATE_WAITING_FOR_ADDR_2_ACK,
	CS75XX_I2C_STATE_WAITING_FOR_WRITE_ACK,
	CS75XX_I2C_STATE_WAITING_FOR_READ_ACK,
};

struct cs75xx_i2c_data {
	int			irq;
	u32			state;
	u32			nostop;
	u32			nostart;
	void __iomem		*reg_base;
	u32			addr1;
	u32			addr2;
	u32			bytes_left;
	u32			byte_posn;
	u32			block;
	int			rc;
	u32			freq_rcl;
	u32			freq_scl;
	u32			ack;
	wait_queue_head_t	waitq;
	spinlock_t		lock;
	struct i2c_msg		*msg;
	struct i2c_adapter	adapter;
};

static void cs75xx_i2c_hw_fini(struct cs75xx_i2c_data *drv_data)
{
	writel(0, drv_data->reg_base + CS75XX_BIW_CFG);
	drv_data->state = CS75XX_I2C_STATE_INVALID;
}

static void cs75xx_i2c_hw_init(struct cs75xx_i2c_data *drv_data)
{
	u32 cfg = readl(drv_data->reg_base + CS75XX_BIW_CFG);

	if (drv_data->freq_scl == 0) {
		cfg &= ~BIW_CFG_CORE_EN;
	} else {
		u32 prer;

		if (cfg & BIW_CFG_CORE_EN) {
			cs75xx_i2c_hw_fini(drv_data);
			usleep_range(5000, 10000);
		}
		if (drv_data->freq_scl != 100000 && drv_data->freq_scl != 400000)
			dev_warn(&drv_data->adapter.dev, "I2C SCL set to %u Hz\n",
				 drv_data->freq_scl);

		prer = DIV_ROUND_UP(drv_data->freq_rcl, 5 * drv_data->freq_scl) - 1;
		cfg = (prer << BIW_CFG_PRER_OFF) | BIW_CFG_CORE_EN;
	}
	writel(cfg, drv_data->reg_base + CS75XX_BIW_CFG);
	usleep_range(5000, 10000);

	drv_data->state = CS75XX_I2C_STATE_IDLE;
}

static void cs75xx_i2c_fsm(struct cs75xx_i2c_data *drv_data)
{
	if ((drv_data->ack & (BIW_ACK_ACK_OUT | BIW_ACK_AL)) &&
	    (drv_data->bytes_left != 0 ||
	     drv_data->state == CS75XX_I2C_STATE_WAITING_FOR_ADDR_1_ACK ||
	     drv_data->state == CS75XX_I2C_STATE_WAITING_FOR_ADDR_2_ACK)) {
		writel(BIW_CTRL_STOP, drv_data->reg_base + CS75XX_BIW_CTRL);
		drv_data->rc = (drv_data->ack & BIW_ACK_AL) ? -EAGAIN : -ENXIO;
		drv_data->state = CS75XX_I2C_STATE_IDLE;
		drv_data->block = 0;
		wake_up(&drv_data->waitq);
		return;
	}

	switch (drv_data->state) {
	case CS75XX_I2C_STATE_START:
		drv_data->state = CS75XX_I2C_STATE_WAITING_FOR_ADDR_1_ACK;
		writel(drv_data->addr1, drv_data->reg_base + CS75XX_BIW_TXR);
		writel(BIW_CTRL_START | BIW_CTRL_WRITE, drv_data->reg_base + CS75XX_BIW_CTRL);
		break;

	case CS75XX_I2C_STATE_WAITING_FOR_ADDR_1_ACK:
		if ((drv_data->msg->flags & I2C_M_TEN) && !drv_data->nostart) {
			drv_data->state = CS75XX_I2C_STATE_WAITING_FOR_ADDR_2_ACK;
			writel(drv_data->addr2, drv_data->reg_base + CS75XX_BIW_TXR);
			writel(BIW_CTRL_WRITE, drv_data->reg_base + CS75XX_BIW_CTRL);
			break;
		}
		fallthrough;
	case CS75XX_I2C_STATE_WAITING_FOR_ADDR_2_ACK:
		if (drv_data->msg->flags & I2C_M_RD) {
			drv_data->state = CS75XX_I2C_STATE_WAITING_FOR_READ_ACK;
			drv_data->bytes_left--;
			if (drv_data->bytes_left || drv_data->nostop)
				writel(BIW_CTRL_READ, drv_data->reg_base + CS75XX_BIW_CTRL);
			else
				writel(BIW_CTRL_READ | BIW_CTRL_ACK_IN | BIW_CTRL_STOP,
				       drv_data->reg_base + CS75XX_BIW_CTRL);
		} else {
			drv_data->state = CS75XX_I2C_STATE_WAITING_FOR_WRITE_ACK;
			writel(drv_data->msg->buf[drv_data->byte_posn++],
			       drv_data->reg_base + CS75XX_BIW_TXR);
			drv_data->bytes_left--;
			if (drv_data->bytes_left || drv_data->nostop)
				writel(BIW_CTRL_WRITE, drv_data->reg_base + CS75XX_BIW_CTRL);
			else
				writel(BIW_CTRL_WRITE | BIW_CTRL_STOP,
				       drv_data->reg_base + CS75XX_BIW_CTRL);
		}
		break;

	case CS75XX_I2C_STATE_WAITING_FOR_WRITE_ACK:
		if (drv_data->bytes_left) {
			writel(drv_data->msg->buf[drv_data->byte_posn++],
			       drv_data->reg_base + CS75XX_BIW_TXR);
			drv_data->bytes_left--;
			if (drv_data->bytes_left || drv_data->nostop)
				writel(BIW_CTRL_WRITE, drv_data->reg_base + CS75XX_BIW_CTRL);
			else
				writel(BIW_CTRL_WRITE | BIW_CTRL_STOP,
				       drv_data->reg_base + CS75XX_BIW_CTRL);
		} else {
			drv_data->block = 0;
			wake_up(&drv_data->waitq);
		}
		break;

	case CS75XX_I2C_STATE_WAITING_FOR_READ_ACK:
		drv_data->msg->buf[drv_data->byte_posn++] =
			readl(drv_data->reg_base + CS75XX_BIW_RXR);
		if (drv_data->bytes_left) {
			drv_data->bytes_left--;
			if (drv_data->bytes_left)
				writel(BIW_CTRL_READ, drv_data->reg_base + CS75XX_BIW_CTRL);
			else
				writel(BIW_CTRL_READ | BIW_CTRL_ACK_IN | BIW_CTRL_STOP,
				       drv_data->reg_base + CS75XX_BIW_CTRL);
		} else {
			drv_data->block = 0;
			wake_up(&drv_data->waitq);
		}
		break;

	case CS75XX_I2C_STATE_INVALID:
	default:
		dev_dbg(&drv_data->adapter.dev,
			"fsm error: state 0x%x ack 0x%x addr 0x%x flags 0x%x left %u\n",
			drv_data->state, drv_data->ack, drv_data->msg->addr,
			drv_data->msg->flags, drv_data->bytes_left);
		writel(BIW_CTRL_STOP, drv_data->reg_base + CS75XX_BIW_CTRL);
		drv_data->rc = -EIO;
		drv_data->state = CS75XX_I2C_STATE_IDLE;
		drv_data->block = 0;
		wake_up(&drv_data->waitq);
	}
}

static irqreturn_t cs75xx_i2c_intr(int irq, void *param)
{
	struct cs75xx_i2c_data *drv_data = param;
	u32 ctrl;

	writel(0, drv_data->reg_base + CS75XX_BIW_IE0);

	writel(readl(drv_data->reg_base + CS75XX_BIW_INT0) | BIW_INT_BIT,
	       drv_data->reg_base + CS75XX_BIW_INT0);

	ctrl = readl(drv_data->reg_base + CS75XX_BIW_CTRL);
	writel(ctrl, drv_data->reg_base + CS75XX_BIW_CTRL);

	drv_data->ack = readl(drv_data->reg_base + CS75XX_BIW_ACK);

	spin_lock(&drv_data->lock);
	cs75xx_i2c_fsm(drv_data);
	spin_unlock(&drv_data->lock);

	writel(1, drv_data->reg_base + CS75XX_BIW_IE0);

	return IRQ_HANDLED;
}

static void cs75xx_i2c_prepare_for_io(struct cs75xx_i2c_data *drv_data,
				      struct i2c_msg *msg)
{
	u32 dir = (msg->flags & I2C_M_RD) ? 1 : 0;

	drv_data->msg = msg;
	drv_data->byte_posn = 0;
	drv_data->bytes_left = msg->len;
	drv_data->rc = 0;
	drv_data->ack = 0;

	if (msg->flags & I2C_M_TEN) {
		drv_data->addr1 = 0xf0 | (((u32)msg->addr & 0x300) >> 7) | dir;
		drv_data->addr2 = (u32)msg->addr & 0xff;
	} else {
		drv_data->addr1 = ((u32)msg->addr & 0x7f) << 1 | dir;
		drv_data->addr2 = 0;
	}
}

static void cs75xx_i2c_wait_for_completion(struct cs75xx_i2c_data *drv_data)
{
	long time_left;

	time_left = wait_event_timeout(drv_data->waitq, !drv_data->block,
				       drv_data->adapter.timeout);
	if (time_left)
		return;

	spin_lock_irq(&drv_data->lock);
	drv_data->rc = -ETIMEDOUT;
	drv_data->state = CS75XX_I2C_STATE_IDLE;
	drv_data->block = 0;
	spin_unlock_irq(&drv_data->lock);
	dev_err(&drv_data->adapter.dev, "transfer timed out, resetting the controller\n");
	cs75xx_i2c_hw_init(drv_data);
}

static int cs75xx_i2c_execute_msg(struct cs75xx_i2c_data *drv_data,
				  struct i2c_msg *msg, bool nostart, bool nostop)
{
	spin_lock_irq(&drv_data->lock);

	cs75xx_i2c_prepare_for_io(drv_data, msg);
	drv_data->nostart = nostart;
	drv_data->nostop = nostop;

	if (unlikely(nostart))
		drv_data->state = CS75XX_I2C_STATE_WAITING_FOR_ADDR_2_ACK;
	else
		drv_data->state = CS75XX_I2C_STATE_START;

	drv_data->block = 1;
	cs75xx_i2c_fsm(drv_data);

	spin_unlock_irq(&drv_data->lock);

	cs75xx_i2c_wait_for_completion(drv_data);

	return drv_data->rc;
}

static u32 cs75xx_i2c_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR | I2C_FUNC_SMBUS_EMUL;
}

static int cs75xx_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
	struct cs75xx_i2c_data *drv_data = i2c_get_adapdata(adap);
	int i, rc = 0, retry;
	bool nostart, nostop;

	for (i = 0; i < num; i++) {
		nostart = (msgs[i].flags & I2C_M_NOSTART) && i > 0 &&
			  (msgs[i - 1].flags & I2C_M_RD) == (msgs[i].flags & I2C_M_RD);
		nostop = i < num - 1 && (msgs[i + 1].flags & I2C_M_NOSTART) &&
			 (msgs[i].flags & I2C_M_RD) == (msgs[i + 1].flags & I2C_M_RD);

		rc = cs75xx_i2c_execute_msg(drv_data, &msgs[i], nostart, nostop);
		for (retry = 0; rc < 0 && rc != -ENXIO && retry < adap->retries; retry++)
			rc = cs75xx_i2c_execute_msg(drv_data, &msgs[i], nostart, nostop);
		if (rc < 0)
			return rc;
	}

	return num;
}

static const struct i2c_algorithm cs75xx_i2c_algo = {
	.xfer		= cs75xx_i2c_xfer,
	.functionality	= cs75xx_i2c_functionality,
};

static int cs75xx_i2c_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct cs75xx_i2c_data *drv_data;
	struct clk *clk;
	u32 val;
	int rc;

	drv_data = devm_kzalloc(dev, sizeof(*drv_data), GFP_KERNEL);
	if (!drv_data)
		return -ENOMEM;

	drv_data->reg_base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(drv_data->reg_base))
		return PTR_ERR(drv_data->reg_base);

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no clock\n");
	drv_data->freq_rcl = clk_get_rate(clk);

	if (of_property_read_u32(np, "clock-frequency", &val))
		val = I2C_MAX_STANDARD_MODE_FREQ;
	drv_data->freq_scl = val;

	drv_data->irq = platform_get_irq(pdev, 0);
	if (drv_data->irq < 0)
		return drv_data->irq;

	strscpy(drv_data->adapter.name, CS75XX_I2C_CTLR_NAME " adapter",
		sizeof(drv_data->adapter.name));
	init_waitqueue_head(&drv_data->waitq);
	spin_lock_init(&drv_data->lock);
	drv_data->state = CS75XX_I2C_STATE_INVALID;

	drv_data->adapter.dev.parent = dev;
	drv_data->adapter.dev.of_node = np;
	drv_data->adapter.algo = &cs75xx_i2c_algo;
	drv_data->adapter.owner = THIS_MODULE;
	drv_data->adapter.class = I2C_CLASS_HWMON;
	if (of_property_read_u32(np, "timeout", &val))
		val = DEFAULT_TIMEOUT_MS;
	drv_data->adapter.timeout = msecs_to_jiffies(val);
	if (of_property_read_u32(np, "retries", &val))
		val = DEFAULT_RETRIES;
	drv_data->adapter.retries = val;
	drv_data->adapter.nr = pdev->id < 0 ? 0 : pdev->id;

	platform_set_drvdata(pdev, drv_data);
	i2c_set_adapdata(&drv_data->adapter, drv_data);

	cs75xx_i2c_hw_init(drv_data);

	rc = devm_request_irq(dev, drv_data->irq, cs75xx_i2c_intr, 0,
			      CS75XX_I2C_CTLR_NAME, drv_data);
	if (rc)
		return dev_err_probe(dev, rc, "cannot request irq %d\n", drv_data->irq);

	writel(1, drv_data->reg_base + CS75XX_BIW_IE0);

	rc = i2c_add_numbered_adapter(&drv_data->adapter);
	if (rc)
		return dev_err_probe(dev, rc, "cannot add adapter\n");

	dev_info(dev, "I2C SCL %u Hz (ref %u Hz)\n", drv_data->freq_scl, drv_data->freq_rcl);

	return 0;
}

static void cs75xx_i2c_remove(struct platform_device *pdev)
{
	struct cs75xx_i2c_data *drv_data = platform_get_drvdata(pdev);

	i2c_del_adapter(&drv_data->adapter);
	cs75xx_i2c_hw_fini(drv_data);
}

static const struct of_device_id cs75xx_i2c_of_match_table[] = {
	{ .compatible = "cortina,cs75xx-i2c" },
	{},
};
MODULE_DEVICE_TABLE(of, cs75xx_i2c_of_match_table);

static struct platform_driver cs75xx_i2c_driver = {
	.probe	= cs75xx_i2c_probe,
	.remove	= cs75xx_i2c_remove,
	.driver	= {
		.name	= CS75XX_I2C_CTLR_NAME,
		.of_match_table = cs75xx_i2c_of_match_table,
	},
};
module_platform_driver(cs75xx_i2c_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cortina CS75xx I2C controller driver");
