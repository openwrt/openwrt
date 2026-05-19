// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pse-pd/pse.h>

#include "rtl-pse.h"

static int rtl_pse_i2c_send(struct rtl_pse *pse, const struct rtl_pse_msg *req)
{
	struct i2c_client *client = to_i2c_client(pse->dev);

	/* Send opcode as SMBus command byte; remaining 11 bytes as block data */
	return i2c_smbus_write_i2c_block_data(client, req->opcode, RTL_PSE_MSG_SIZE - 1,
					      (u8 *)req + 1);
}

/*
 * The core has already waited RTL_PSE_MCU_RESPONSE_MS before calling us, so
 * the response is normally ready on the very first read. For commands the
 * MCU produces more slowly, keep polling at the typical response cadence
 * up to the worst-case ceiling.
 */
#define RTL_PSE_I2C_RETRY_MS	RTL_PSE_MCU_RESPONSE_MS
#define RTL_PSE_I2C_MAX_TRIES	(RTL_PSE_MCU_RESPONSE_MAX_MS / RTL_PSE_I2C_RETRY_MS)

static int rtl_pse_i2c_recv(struct rtl_pse *pse,
			    const struct rtl_pse_msg *req,
			    struct rtl_pse_msg *resp)
{
	struct i2c_client *client = to_i2c_client(pse->dev);
	int tries, ret;

	for (tries = 0; tries < RTL_PSE_I2C_MAX_TRIES; tries++) {
		if (tries > 0)
			msleep(RTL_PSE_I2C_RETRY_MS);

		/* MCU needs 0x00 as command byte for read */
		ret = i2c_smbus_read_i2c_block_data(client, 0x00,
						    RTL_PSE_MSG_SIZE,
						    (u8 *)resp);
		if (ret < 0)
			return ret;
		if (ret == RTL_PSE_MSG_SIZE && resp->opcode == req->opcode)
			return 0;
	}

	return -ETIMEDOUT;
}

static const struct rtl_pse_transport_ops rtl_pse_i2c_transport_ops = {
	.send = rtl_pse_i2c_send,
	.recv = rtl_pse_i2c_recv,
};

static int rtl_pse_i2c_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct rtl_pse *pse;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_WRITE_I2C_BLOCK |
						      I2C_FUNC_SMBUS_READ_I2C_BLOCK))
		return dev_err_probe(dev, -EOPNOTSUPP,
				     "adapter lacks SMBus I2C-block support\n");

	pse = devm_kzalloc(dev, sizeof(*pse), GFP_KERNEL);
	if (!pse)
		return -ENOMEM;

	pse->dev = dev;
	pse->transport = &rtl_pse_i2c_transport_ops;

	i2c_set_clientdata(client, pse);

	return rtl_pse_register(pse);
}

static const struct of_device_id rtl_pse_i2c_of_match[] = {
	RTL_PSE_OF_MATCH_ENTRIES
};
MODULE_DEVICE_TABLE(of, rtl_pse_i2c_of_match);

static struct i2c_driver rtl_pse_i2c_driver = {
	.driver = {
		.name		= "rtl-pse-i2c",
		.of_match_table	= rtl_pse_i2c_of_match,
	},
	.probe	= rtl_pse_i2c_probe,
};
module_i2c_driver(rtl_pse_i2c_driver);

MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_DESCRIPTION("Realtek PoE MCU (I2C) PSE driver");
MODULE_LICENSE("GPL");
