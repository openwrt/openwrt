// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pse-pd/pse.h>
#include <linux/serdev.h>
#include <linux/string.h>

#include "rtl-pse.h"

#define RTL_PSE_UART_BAUD_DEFAULT	19200
#define RTL_PSE_UART_TX_TIMEOUT		msecs_to_jiffies(100)
#define RTL_PSE_UART_RX_TIMEOUT		msecs_to_jiffies(RTL_PSE_MCU_RESPONSE_MAX_MS)

struct rtl_pse_uart {
	struct rtl_pse pse;
	struct serdev_device *serdev;
	struct completion rx_done;
	size_t rx_len;
	u8 rx_buf[RTL_PSE_MSG_SIZE];
};

#define to_rtl_pse_uart(p)  container_of(p, struct rtl_pse_uart, pse)

/*
 * No frame alignment is performed here: a stray byte arriving during
 * transmission, or a truncated/extra-byte frame, will misalign the next
 * response. The misaligned frame then fails opcode/checksum validation
 * in the core (-EBADMSG); the following _send resets rx_len and
 * resyncs. Net cost is one lost transaction per glitch.
 */
static size_t rtl_pse_uart_receive(struct serdev_device *serdev,
				   const u8 *buf, size_t count)
{
	struct rtl_pse_uart *ctx = serdev_device_get_drvdata(serdev);
	size_t take;

	take = min(count, sizeof(ctx->rx_buf) - ctx->rx_len);
	if (take == 0)
		return count;  /* drop overflow bytes */

	memcpy(ctx->rx_buf + ctx->rx_len, buf, take);
	ctx->rx_len += take;

	if (ctx->rx_len == sizeof(ctx->rx_buf))
		complete(&ctx->rx_done);

	return take;
}

static const struct serdev_device_ops rtl_pse_uart_serdev_ops = {
	.receive_buf = rtl_pse_uart_receive,
	.write_wakeup = serdev_device_write_wakeup,
};

static int rtl_pse_uart_send(struct rtl_pse *pse, const struct rtl_pse_msg *req)
{
	struct rtl_pse_uart *ctx = to_rtl_pse_uart(pse);
	int ret;

	/* clear any leftover rx state before transmitting */
	reinit_completion(&ctx->rx_done);
	ctx->rx_len = 0;

	ret = serdev_device_write(ctx->serdev, (const u8 *)req, sizeof(*req),
				  RTL_PSE_UART_TX_TIMEOUT);
	if (ret < 0)
		return ret;
	if (ret != sizeof(*req))
		return -EIO;

	return 0;
}

static int rtl_pse_uart_recv(struct rtl_pse *pse,
			     const struct rtl_pse_msg *req,
			     struct rtl_pse_msg *resp)
{
	struct rtl_pse_uart *ctx = to_rtl_pse_uart(pse);

	if (!wait_for_completion_timeout(&ctx->rx_done, RTL_PSE_UART_RX_TIMEOUT))
		return -ETIMEDOUT;

	if (ctx->rx_len != sizeof(*resp))
		return -EIO;

	memcpy(resp, ctx->rx_buf, sizeof(*resp));
	return 0;
}

static const struct rtl_pse_transport_ops rtl_pse_uart_transport_ops = {
	.send = rtl_pse_uart_send,
	.recv = rtl_pse_uart_recv,
};

static int rtl_pse_uart_probe(struct serdev_device *serdev)
{
	u32 speed = RTL_PSE_UART_BAUD_DEFAULT;
	struct device *dev = &serdev->dev;
	struct rtl_pse_uart *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->serdev = serdev;
	ctx->pse.dev = dev;
	ctx->pse.transport = &rtl_pse_uart_transport_ops;
	init_completion(&ctx->rx_done);

	serdev_device_set_drvdata(serdev, ctx);
	serdev_device_set_client_ops(serdev, &rtl_pse_uart_serdev_ops);

	ret = devm_serdev_device_open(dev, serdev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to open serdev\n");

	fwnode_property_read_u32(dev_fwnode(dev), "current-speed", &speed);
	serdev_device_set_baudrate(serdev, speed);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	return rtl_pse_register(&ctx->pse);
}

static const struct of_device_id rtl_pse_uart_of_match[] = {
	RTL_PSE_OF_MATCH_ENTRIES
};
MODULE_DEVICE_TABLE(of, rtl_pse_uart_of_match);

static struct serdev_device_driver rtl_pse_uart_driver = {
	.driver = {
		.name = "rtl-pse-uart",
		.of_match_table = rtl_pse_uart_of_match,
	},
	.probe  = rtl_pse_uart_probe,
};
module_serdev_device_driver(rtl_pse_uart_driver);

MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_DESCRIPTION("Realtek PoE MCU (UART) PSE driver");
MODULE_LICENSE("GPL");
