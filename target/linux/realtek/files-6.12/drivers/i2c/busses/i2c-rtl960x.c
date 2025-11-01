// SPDX-License-Identifier: GPL-2.0-only

#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/i2c-mux.h>
#include <linux/mod_devicetable.h>
#include <linux/mfd/syscon.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/unaligned.h>

#define RTL960X_IO_MODE_EN				0x23014

#define RTL960X_I2C_CONFIG				0x0

#define RTL960X_I2C_IND_WD				0xB4
#define RTL960X_I2C_IND_ADR				0xBC
#define RTL960X_I2C_IND_RD				0xCC
#define RTL960X_I2C_IND_CMD				0xC4

#define RTL960X_I2C_PORT_OFS			0x4

enum rtl960x_i2c_reg_scope {
	REG_SCOPE_GLOBAL,
	REG_SCOPE_MASTER,
	REG_SCOPE_PORT,
};

struct rtl960x_i2c_reg_field {
	struct reg_field field;
	enum rtl960x_i2c_reg_scope scope;
};

enum rtl960x_i2c_reg_fields {
	F_CLK_DIV = 0,
	F_DATA_WIDTH,
	F_REG_ADDR_WIDTH,
	F_SLV_ADDR,
	F_EXT_SCK_5MS,
	F_I2C_EN,
	F_CMD_EN,
	F_RW_EN,
	F_BUSY,
	F_SLV_NACK,

	/* keep last */
	F_NUM_FIELDS
};

struct rtl960x_i2c_drv_data {
	struct rtl960x_i2c_reg_field field_desc[F_NUM_FIELDS];
	u32 rd_reg;
	u32 wd_reg;
	u32 adr_reg;
};

struct rtl960x_i2c {
	struct regmap *regmap;
	struct device *dev;
	struct i2c_adapter adap;
	struct regmap_field *fields[F_NUM_FIELDS];
	u32 reg_base;
	u32 clk_div;
	u32 rd_data;
	u32 wd_reg;
	u32 adr_reg;
	u32 bus_freq;
	u8 port;
	struct clk *clk;
	spinlock_t lock;
};

enum rtl960x_i2c_xfer_type {
	RTL960X_I2C_XFER_BYTE,
	RTL960X_I2C_XFER_WORD,
};

struct rtl960x_i2c_xfer {
	enum rtl960x_i2c_xfer_type type;
	u16 dev_addr;
	u8 reg_addr;
	u8 reg_addr_len;
	u8 *data;
	u8 data_len;
	bool write;
};

static int rtl960x_i2c_reg_addr_set(struct rtl960x_i2c *i2c, u32 reg, u16 len)
{
	int ret;

	ret = regmap_field_write(i2c->fields[F_REG_ADDR_WIDTH], len);
	if (ret)
		return ret;

	return regmap_write(i2c->regmap, i2c->adr_reg, reg);
}

static int rtl960x_i2c_writel(struct rtl960x_i2c *i2c, u32 data)
{
	return regmap_write(i2c->regmap, i2c->wd_reg, data);
}

static int rtl960x_i2c_prepare_xfer(struct rtl960x_i2c *i2c, struct rtl960x_i2c_xfer *xfer)
{
	int ret;

	ret = regmap_field_write(i2c->fields[F_SLV_ADDR], xfer->dev_addr);
	if (ret)
		return ret;

	ret = rtl960x_i2c_reg_addr_set(i2c, xfer->reg_addr, xfer->reg_addr_len);
	if (ret)
		return ret;

	ret = regmap_field_write(i2c->fields[F_RW_EN], xfer->write);
	if (ret)
		return ret;

	ret = regmap_field_write(i2c->fields[F_DATA_WIDTH], (xfer->data_len - 1) & 0xf);
	if (ret)
		return ret;

	if (xfer->write) {
		switch (xfer->type) {
		case RTL960X_I2C_XFER_BYTE:
			ret = rtl960x_i2c_writel(i2c, *xfer->data);
			break;
		case RTL960X_I2C_XFER_WORD:
			ret = rtl960x_i2c_writel(i2c, get_unaligned((const u16 *)xfer->data));
			break;
		}
	}

	return ret;
}

static int rtl9300_i2c_do_xfer(struct rtl960x_i2c *i2c, struct rtl960x_i2c_xfer *xfer)
{
	u32 val;
	int ret;

	ret = regmap_field_write(i2c->fields[F_CMD_EN], 1);
	if (ret)
		return ret;

	ret = regmap_field_read_poll_timeout(i2c->fields[F_BUSY], val, !val, 100, 100000);
	if (ret)
		return ret;

	ret = regmap_field_read(i2c->fields[F_SLV_NACK], &val);
	if (ret)
		return ret;
	if (val)
		return -ENXIO;

	if (!xfer->write) {
		switch (xfer->type) {
		case RTL960X_I2C_XFER_BYTE:
			ret = regmap_read(i2c->regmap, i2c->rd_data, &val);
			if (ret)
				return ret;

			*xfer->data = val & 0xff;
			break;
		case RTL960X_I2C_XFER_WORD:
			ret = regmap_read(i2c->regmap, i2c->rd_data, &val);
			if (ret)
				return ret;

			put_unaligned(val & 0xffff, (u16 *)xfer->data);
			break;
		}
	}

	return 0;
}

static int rtl960x_i2c_smbus_xfer(struct i2c_adapter *adap, u16 addr, unsigned short flags,
				  char read_write, u8 command, int size,
				  union i2c_smbus_data *data)
{
	struct rtl960x_i2c *i2c = i2c_get_adapdata(adap);
	struct rtl960x_i2c_xfer xfer = {0};
	unsigned long spin_flags;
	int ret;

	if (addr > 0x7f)
		return -EINVAL;

	xfer.dev_addr = addr & 0x7f;
	xfer.write = (read_write == I2C_SMBUS_WRITE);
	xfer.reg_addr = command;
	xfer.reg_addr_len = 0;

	switch (size) {
	case I2C_SMBUS_BYTE:
		xfer.data = (read_write == I2C_SMBUS_READ) ? &data->byte : &command;
		xfer.data_len = 1;
		xfer.reg_addr = 0;
		xfer.type = RTL960X_I2C_XFER_BYTE;
		break;
	case I2C_SMBUS_BYTE_DATA:
		xfer.data = &data->byte;
		xfer.data_len = 1;
		xfer.type = RTL960X_I2C_XFER_BYTE;
		break;
	case I2C_SMBUS_WORD_DATA:
		xfer.data = (u8 *)&data->word;
		xfer.data_len = 2;
		xfer.reg_addr_len = 1;
		xfer.type = RTL960X_I2C_XFER_WORD;
		break;
	default:
		dev_err(&adap->dev, "Unsupported transaction %d\n", size);
		return -EOPNOTSUPP;
	}

	spin_lock_irqsave(&i2c->lock, spin_flags);

	ret = rtl960x_i2c_prepare_xfer(i2c, &xfer);
	if (ret)
		goto spin_unlock;

	ret = rtl9300_i2c_do_xfer(i2c, &xfer);
	if (ret)
		goto spin_unlock;

spin_unlock:
	spin_unlock_irqrestore(&i2c->lock, spin_flags);
	return ret;
}

static u32 rtl960x_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA |
	       I2C_FUNC_SMBUS_WORD_DATA;
}

static const struct i2c_algorithm rtl960x_i2c_algo = {
	.smbus_xfer = rtl960x_i2c_smbus_xfer,
	.functionality = rtl960x_i2c_func,
};

static int rtl960x_i2c_init(struct rtl960x_i2c *i2c)
{
	u32 val;
	int ret;

	/* Set reg_addr and data widdth to 8-bit (0) */
	ret = regmap_field_write(i2c->fields[F_REG_ADDR_WIDTH], 0);
	if (ret)
		return ret;

	ret = regmap_field_write(i2c->fields[F_DATA_WIDTH], 0);
	if (ret)
		return ret;

	/* Set 5ms EEPROM disable delay */
	ret = regmap_field_write(i2c->fields[F_EXT_SCK_5MS], 1);
	if (ret)
		return ret;

	/* Setup the clock */
	ret = regmap_field_write(i2c->fields[F_CLK_DIV], i2c->clk_div);
	if (ret)
		return ret;

	/* Enable the I2C port (0 - 1st port, 1 - 2nd port)*/
	return regmap_field_set_bits(i2c->fields[F_I2C_EN], BIT(i2c->port));
}

static int rtl960x_i2c_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtl960x_i2c *i2c;
	struct i2c_adapter *adap;
	struct rtl960x_i2c_drv_data *drv_data;
	struct reg_field fields[F_NUM_FIELDS];
	u32 port, port_offset;
	int ret, i = 0;

	i2c = devm_kzalloc(dev, sizeof(*i2c), GFP_KERNEL);
	if (!i2c)
		return -ENOMEM;

	i2c->regmap = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR(i2c->regmap))
		return PTR_ERR(i2c->regmap);

	i2c->dev = dev;

	spin_lock_init(&i2c->lock);

	ret = device_property_read_u32(dev, "reg", &i2c->reg_base);
	if (ret)
		return ret;

	ret = device_property_read_u32(dev, "realtek,scl", &port);
	if (ret || port != 1)
		port = 0;
	i2c->port = (u8)port;
	port_offset = port * RTL960X_I2C_PORT_OFS;

	drv_data = (struct rtl960x_i2c_drv_data *)device_get_match_data(i2c->dev);

	i2c->rd_data = drv_data->rd_reg + port_offset;
	i2c->wd_reg = drv_data->wd_reg + port_offset;
	i2c->adr_reg = drv_data->adr_reg + port_offset;

	for (i = 0; i < F_NUM_FIELDS; i++) {
		fields[i] = drv_data->field_desc[i].field;
		if (drv_data->field_desc[i].scope == REG_SCOPE_PORT)
			fields[i].reg += port_offset;
		if (drv_data->field_desc[i].scope == REG_SCOPE_MASTER)
			fields[i].reg += i2c->reg_base;
	}

	ret = devm_regmap_field_bulk_alloc(dev, i2c->regmap, i2c->fields,
					   fields, F_NUM_FIELDS);
	if (ret)
		return ret;

	i2c->clk = devm_clk_get_enabled(&pdev->dev, NULL);
	if (IS_ERR(i2c->clk)) {
		dev_err(&pdev->dev, "Failed to enable clock\n");
		return PTR_ERR(i2c->clk);
	}

	if (of_property_read_u32(pdev->dev.of_node, "clock-frequency",
					 &i2c->bus_freq))
		i2c->bus_freq = I2C_MAX_STANDARD_MODE_FREQ;

	if (i2c->bus_freq == 0) {
		dev_warn(i2c->dev, "clock-frequency 0 not supported\n");
		return -EINVAL;
	}

	i2c->clk_div = clk_get_rate(i2c->clk) / i2c->bus_freq - 1;

	adap = &i2c->adap;
	adap->owner = THIS_MODULE;
	adap->algo = &rtl960x_i2c_algo;
	adap->retries = 3;
	adap->dev.parent = dev;
	i2c_set_adapdata(adap, i2c);
	adap->dev.of_node = pdev->dev.of_node;
	snprintf(adap->name, sizeof(adap->name), "%s port%d\n", dev_name(dev), port);

	platform_set_drvdata(pdev, i2c);

	ret = rtl960x_i2c_init(i2c);
	if (ret)
		return ret;

	ret = devm_i2c_add_adapter(dev, adap);
	if (ret)
		return ret;

	dev_info(&pdev->dev, "clock %u kHz\n", i2c->bus_freq / 1000);

	return 0;
}

#define GLB_REG_FIELD(reg, lsb, msb)    \
	{ .field = REG_FIELD(reg, lsb, msb), .scope = REG_SCOPE_GLOBAL }
#define MST_REG_FIELD(reg, lsb, msb)    \
	{ .field = REG_FIELD(reg, lsb, msb), .scope = REG_SCOPE_MASTER }
#define PORT_REG_FIELD(reg, lsb, msb)    \
	{ .field = REG_FIELD(reg, lsb, msb), .scope = REG_SCOPE_PORT }


static const struct rtl960x_i2c_drv_data rtl960x_i2c_drv_data = {
	.field_desc = {
		[F_I2C_EN]		= GLB_REG_FIELD(RTL960X_IO_MODE_EN, 13, 14),
		[F_EXT_SCK_5MS]		= MST_REG_FIELD(RTL960X_I2C_CONFIG, 26, 26),
		[F_SLV_ADDR]		= MST_REG_FIELD(RTL960X_I2C_CONFIG, 14, 20),
		[F_REG_ADDR_WIDTH]	= MST_REG_FIELD(RTL960X_I2C_CONFIG, 12, 13),
		[F_DATA_WIDTH]		= MST_REG_FIELD(RTL960X_I2C_CONFIG, 10, 11),
		[F_CLK_DIV]		= MST_REG_FIELD(RTL960X_I2C_CONFIG, 0, 9),
		[F_SLV_NACK]		= PORT_REG_FIELD(RTL960X_I2C_IND_CMD, 3, 3),
		[F_BUSY]		= PORT_REG_FIELD(RTL960X_I2C_IND_CMD, 2, 2),
		[F_RW_EN]		= PORT_REG_FIELD(RTL960X_I2C_IND_CMD, 1, 1),
		[F_CMD_EN]		= PORT_REG_FIELD(RTL960X_I2C_IND_CMD, 0, 0)
	},
	.rd_reg = RTL960X_I2C_IND_RD,
	.wd_reg = RTL960X_I2C_IND_WD,
	.adr_reg = RTL960X_I2C_IND_ADR,
};

static const struct of_device_id i2c_rtl960x_dt_ids[] = {
	{ .compatible = "realtek,rtl960x-i2c", .data = (void *) &rtl960x_i2c_drv_data },
	{}
};
MODULE_DEVICE_TABLE(of, i2c_rtl960x_dt_ids);

static struct platform_driver rtl960x_i2c_driver = {
	.probe = rtl960x_i2c_probe,
	.driver = {
		.name = "i2c-rtl960x",
		.of_match_table = i2c_rtl960x_dt_ids,
	},
};

module_platform_driver(rtl960x_i2c_driver);

MODULE_DESCRIPTION("RTL960X I2C controller driver");
MODULE_LICENSE("GPL");
