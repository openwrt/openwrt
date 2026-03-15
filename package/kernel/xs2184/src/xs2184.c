// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/gpio/consumer.h>
#include <linux/err.h>

static int xs2184_init(struct i2c_client *client)
{
	if (i2c_smbus_read_byte_data(client, 0x1b) != 0xd0) {
		dev_err(&client->dev, "not xs2184, skipping...\n");
		return -1;
	}

	i2c_smbus_write_byte_data(client, 0x1a, 0x00);
	i2c_smbus_write_byte_data(client, 0x01, 0x92);
	i2c_smbus_write_byte_data(client, 0x19, 0xf0);
	i2c_smbus_write_byte_data(client, 0x12, 0xff);
	i2c_smbus_write_byte_data(client, 0x1c, 0x00);
	i2c_smbus_write_byte_data(client, 0x44, 0x0f);
	i2c_smbus_write_byte_data(client, 0x46, 0x01);
	i2c_smbus_write_byte_data(client, 0x4b, 0x01);
	i2c_smbus_write_byte_data(client, 0x50, 0x01);
	i2c_smbus_write_byte_data(client, 0x55, 0x01);
	i2c_smbus_write_byte_data(client, 0x1e, 0x77);
	i2c_smbus_write_byte_data(client, 0x1f, 0x77);
	i2c_smbus_write_byte_data(client, 0x13, 0x0f);
	i2c_smbus_write_byte_data(client, 0x14, 0xff);
	i2c_smbus_write_byte_data(client, 0x48, 0x80);
	i2c_smbus_write_byte_data(client, 0x47, 0x14);
	i2c_smbus_write_byte_data(client, 0x4d, 0x80);
	i2c_smbus_write_byte_data(client, 0x4c, 0x14);
	i2c_smbus_write_byte_data(client, 0x52, 0x80);
	i2c_smbus_write_byte_data(client, 0x51, 0x14);
	i2c_smbus_write_byte_data(client, 0x57, 0x80);
	i2c_smbus_write_byte_data(client, 0x56, 0x14);
	i2c_smbus_write_byte_data(client, 0x1e, 0x77);
	i2c_smbus_write_byte_data(client, 0x1f, 0x77);
	i2c_smbus_write_byte_data(client, 0x19, 0x0f);

	return 0;
}

static int xs2184_probe(struct i2c_client *client)
{
	struct gpio_desc *gpio_en;

	if (!i2c_check_functionality(client->adapter, 0x00180000))
		return -ENODEV;

	gpio_en = devm_gpiod_get_optional(&client->dev, "enable", GPIOD_OUT_HIGH);
	if (IS_ERR(gpio_en))
		return PTR_ERR(gpio_en);

	if (xs2184_init(client) < 0) {
		dev_err(&client->dev, "xs2184 init failed\n");
		return -1;
	}

	pr_info("xs2184 init success\n");
	return 0;
}

static const struct i2c_device_id xs2184_id[] = {
	{ "xs2184", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, xs2184_id);

static const struct of_device_id xs2184_of_match[] = {
	{ .compatible = "chipup,xs2184" },
	{ }
};
MODULE_DEVICE_TABLE(of, xs2184_of_match);

static struct i2c_driver xs2184_driver = {
	.driver = {
		.name = "xs2184",
		.of_match_table = xs2184_of_match,
	},
	.probe = xs2184_probe,
	.id_table = xs2184_id,
};

static int __init xs2184_driver_init(void)
{
	return i2c_register_driver(THIS_MODULE, &xs2184_driver);
}

static void __exit xs2184_driver_exit(void)
{
	i2c_del_driver(&xs2184_driver);
}

module_init(xs2184_driver_init);
module_exit(xs2184_driver_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Chipup XS2184 PSE I2C driver");
MODULE_AUTHOR("Tianchen Tang <ttc0419@outlook.com>");
