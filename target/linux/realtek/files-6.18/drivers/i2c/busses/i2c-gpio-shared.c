// SPDX-License-Identifier: GPL-2.0
/* Bitbanging driver for up to 32 I2C buses that share a single SCL pin */

#include <linux/gpio/consumer.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>

#define MAX_BUSES 32

struct gpio_shared_ctx;

struct gpio_shared_bus {
	struct gpio_desc *sda;
	struct i2c_adapter adap;
	struct i2c_algo_bit_data bit_data;
	struct i2c_bus_recovery_info recovery_info;
	struct gpio_shared_ctx *ctx;
};

struct gpio_shared_ctx {
	struct gpio_desc *scl;
	struct mutex lock;
	struct gpio_shared_bus bus[];
};

static struct gpio_shared_bus *adap_to_bus(struct i2c_adapter *adap)
{
	return container_of(adap, struct gpio_shared_bus, adap);
}

static struct gpio_shared_ctx *adap_to_ctx(struct i2c_adapter *adap)
{
	return adap_to_bus(adap)->ctx;
}

static void gpio_shared_setsda(void *data, int state)
{
	struct gpio_shared_bus *bus = data;

	gpiod_set_value_cansleep(bus->sda, state);
}

static void gpio_shared_setscl(void *data, int state)
{
	struct gpio_shared_bus *bus = data;

	gpiod_set_value_cansleep(bus->ctx->scl, state);
}

static int gpio_shared_getsda(void *data)
{
	struct gpio_shared_bus *bus = data;

	return gpiod_get_value_cansleep(bus->sda);
}

static int gpio_shared_getscl(void *data)
{
	struct gpio_shared_bus *bus = data;

	return gpiod_get_value_cansleep(bus->ctx->scl);
}

static int gpio_shared_pre_xfer(struct i2c_adapter *adap)
{
	return mutex_lock_interruptible(&adap_to_ctx(adap)->lock);
}

static void gpio_shared_recovery_set_scl(struct i2c_adapter *adap, int val)
{
	gpiod_set_value_cansleep(adap_to_ctx(adap)->scl, val);
}

static int gpio_shared_recovery_get_scl(struct i2c_adapter *adap)
{
	return gpiod_get_value_cansleep(adap_to_ctx(adap)->scl);
}

static void gpio_shared_recovery_set_sda(struct i2c_adapter *adap, int val)
{
	gpiod_set_value_cansleep(adap_to_bus(adap)->sda, val);
}

static int gpio_shared_recovery_get_sda(struct i2c_adapter *adap)
{
	return gpiod_get_value_cansleep(adap_to_bus(adap)->sda);
}

static void gpio_shared_prepare_recovery(struct i2c_adapter *adap)
{
	mutex_lock(&adap_to_ctx(adap)->lock);
}

static void gpio_shared_unlock(struct i2c_adapter *adap)
{
	mutex_unlock(&adap_to_ctx(adap)->lock);
}

static void gpio_shared_del_adapter(void *data)
{
	i2c_del_adapter(data);
}

static int gpio_shared_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int bus_count, sda_count, ret;
	struct gpio_shared_ctx *ctx;
	u32 used_buses = 0;

	bus_count = device_get_child_node_count(dev);
	if (!bus_count)
		return dev_err_probe(dev, -EINVAL, "no buses defined\n");

	if (bus_count > MAX_BUSES)
		return dev_err_probe(dev, -EINVAL, "maximum of %d buses allowed\n", MAX_BUSES);

	sda_count = gpiod_count(dev, "sda");
	if (sda_count < 0)
		return dev_err_probe(dev, sda_count, "sda-gpios missing\n");

	if (sda_count != bus_count)
		return dev_err_probe(dev, -EINVAL,
				     "sda-gpios count (%d) must match child node count (%d)\n",
				     sda_count, bus_count);

	ctx = devm_kzalloc(dev, struct_size(ctx, bus, bus_count), GFP_KERNEL);
	if (!ctx)
		return dev_err_probe(dev, -ENOMEM, "memory allocation failed\n");

	ret = devm_mutex_init(dev, &ctx->lock);
	if (ret)
		return dev_err_probe(dev, ret, "mutex initialization failed\n");

	platform_set_drvdata(pdev, ctx);

	ctx->scl = devm_gpiod_get(dev, "scl", GPIOD_OUT_HIGH_OPEN_DRAIN);
	if (IS_ERR(ctx->scl))
		return dev_err_probe(dev, PTR_ERR(ctx->scl), "shared SCL GPIO not found\n");

	device_for_each_child_node_scoped(dev, child) {
		struct i2c_algo_bit_data *bit_data;
		struct i2c_bus_recovery_info *ri;
		struct gpio_shared_bus *bus;
		struct i2c_adapter *adap;
		u32 bus_num, clock_freq;

		/* Use the child's reg value as the index into the sda-gpios array. */
		ret = fwnode_property_read_u32(child, "reg", &bus_num);
		if (ret)
			return dev_err_probe(dev, ret,
					     "missing reg property in child node\n");

		if (bus_num >= sda_count)
			return dev_err_probe(dev, -EINVAL,
					     "reg value %u out of range (max %d)\n",
					     bus_num, sda_count - 1);

		if (used_buses & BIT(bus_num))
			return dev_err_probe(dev, -EINVAL,
					     "duplicate definition of bus %d\n", bus_num);

		bus = &ctx->bus[bus_num];
		bit_data = &bus->bit_data;
		ri = &bus->recovery_info;
		adap = &bus->adap;

		bus->sda = devm_gpiod_get_index(dev, "sda", bus_num,
						GPIOD_OUT_HIGH_OPEN_DRAIN);
		if (IS_ERR(bus->sda))
			return dev_err_probe(dev, PTR_ERR(bus->sda),
					     "SDA GPIO for bus %u not found\n", bus_num);
		/*
		 * Data transfer synchronization between multiple busses is realized by mutex
		 * locking/unlocking within pre_xfer and post_xfer helpers.
		 */
		bus->ctx = ctx;
		bit_data->data = bus;
		bit_data->setsda = gpio_shared_setsda;
		bit_data->setscl = gpio_shared_setscl;
		bit_data->getsda = gpio_shared_getsda;
		bit_data->getscl = gpio_shared_getscl;
		bit_data->pre_xfer = gpio_shared_pre_xfer;
		bit_data->post_xfer = gpio_shared_unlock;
		bit_data->timeout = msecs_to_jiffies(100);
		/*
		 * clock-frequency specifies the I2C bus frequency. Convert to the half-period
		 * delay in microseconds that i2c-algo-bit expects. Default to 5 us (~100 kHz)
		 * if not specified. This is usually lower than the configured frequency,
		 * especially near the 400 kHz limit.
		 */
		if (!fwnode_property_read_u32(child, "clock-frequency", &clock_freq) &&
		    clock_freq > 0)
			bit_data->udelay = DIV_ROUND_UP(1000000, 2 * min(clock_freq, 400000));
		else
			bit_data->udelay = 5;
		/*
		 * i2c_recover_bus() is called by the I2C core without going through pre_xfer
		 * and post_xfer. So the shared SCL mutex is NOT held at that point. Provide
		 * prepare_recovery/unprepare_recovery to explicitly prevent concurrent SCL
		 * access from another bus.
		 */
		ri->recover_bus = i2c_generic_scl_recovery;
		ri->set_scl = gpio_shared_recovery_set_scl;
		ri->get_scl = gpio_shared_recovery_get_scl;
		ri->set_sda = gpio_shared_recovery_set_sda;
		ri->get_sda = gpio_shared_recovery_get_sda;
		ri->prepare_recovery = gpio_shared_prepare_recovery;
		ri->unprepare_recovery = gpio_shared_unlock;

		adap->dev.parent = dev;
		adap->owner = THIS_MODULE;
		adap->algo_data = bit_data;
		adap->bus_recovery_info = ri;
		device_set_node(&adap->dev, fwnode_handle_get(child));
		snprintf(adap->name, sizeof(adap->name), "i2c-gpio-shared:%u", bus_num);

		ret = devm_add_action_or_reset(dev, gpio_shared_del_adapter, adap);
		if (ret)
			return dev_err_probe(dev, ret,
					     "bus %u cleanup registration failed\n", bus_num);

		ret = i2c_bit_add_bus(adap);
		if (ret)
			return dev_err_probe(dev, ret, "failed to register bus %u\n", bus_num);

		dev_info(dev, "registered I2C bus %u with shared SCL (udelay %d us)\n",
			 bus_num, bit_data->udelay);

		used_buses |= BIT(bus_num);
	}

	return 0;
}

static const struct of_device_id gpio_shared_of_match[] = {
	{ .compatible = "i2c-gpio-shared" },
	{}
};
MODULE_DEVICE_TABLE(of, gpio_shared_of_match);

static struct platform_driver gpio_shared_driver = {
	.probe = gpio_shared_probe,
	.driver = {
		.name = "i2c-gpio-shared",
		.of_match_table = gpio_shared_of_match,
	},
};

module_platform_driver(gpio_shared_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Markus Stockhausen <markus.stockhausen@gmx.de>");
MODULE_DESCRIPTION("bitbanging multi I2C driver for shared SCL");
