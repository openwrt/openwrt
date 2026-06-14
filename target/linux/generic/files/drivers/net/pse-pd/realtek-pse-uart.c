// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pse-pd/pse.h>
#include <linux/serdev.h>
#include <linux/string.h>

#include "realtek-pse.h"

#define RTPSE_UART_BAUD_DEFAULT	19200
#define RTPSE_UART_TX_TIMEOUT		msecs_to_jiffies(100)
#define RTPSE_UART_RX_TIMEOUT		msecs_to_jiffies(RTPSE_MCU_RESPONSE_MAX_MS)

struct rtpse_uart {
	struct rtpse_ctrl pse;
	struct serdev_device *serdev;
	struct completion rx_done;
	size_t rx_len;
	u8 rx_buf[RTPSE_MCU_MSG_SIZE];
};

#define to_rtpse_uart(p)  container_of(p, struct rtpse_uart, pse)

/*
 * No frame alignment is performed here: a stray byte arriving during
 * transmission, or a truncated/extra-byte frame, will misalign the next
 * response. The misaligned frame then fails opcode/checksum validation
 * in the core (-EBADMSG); the following _send resets rx_len and
 * resyncs. Net cost is one lost transaction per glitch.
 */
static size_t rtpse_uart_receive(struct serdev_device *serdev,
				   const u8 *buf, size_t count)
{
	struct rtpse_uart *ctx = serdev_device_get_drvdata(serdev);
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

static const struct serdev_device_ops rtpse_uart_serdev_ops = {
	.receive_buf = rtpse_uart_receive,
	.write_wakeup = serdev_device_write_wakeup,
};

static int rtpse_uart_send(struct rtpse_ctrl *pse, const struct rtpse_mcu_msg *req)
{
	struct rtpse_uart *ctx = to_rtpse_uart(pse);
	int written;

	/* clear any leftover rx state before transmitting */
	reinit_completion(&ctx->rx_done);
	ctx->rx_len = 0;

	written = serdev_device_write(ctx->serdev, (const u8 *)req, sizeof(*req),
				      RTPSE_UART_TX_TIMEOUT);
	if (written < 0)
		return written;
	if (written != sizeof(*req))
		return -EIO;

	return 0;
}

static int rtpse_uart_recv(struct rtpse_ctrl *pse,
			     const struct rtpse_mcu_msg *req,
			     struct rtpse_mcu_msg *resp)
{
	struct rtpse_uart *ctx = to_rtpse_uart(pse);

	if (!wait_for_completion_timeout(&ctx->rx_done, RTPSE_UART_RX_TIMEOUT))
		return -ETIMEDOUT;

	if (ctx->rx_len != sizeof(*resp))
		return -EIO;

	memcpy(resp, ctx->rx_buf, sizeof(*resp));
	return 0;
}

static const struct rtpse_transport_ops rtpse_uart_transport_ops = {
	.send = rtpse_uart_send,
	.recv = rtpse_uart_recv,
};

static int rtpse_uart_probe(struct serdev_device *serdev)
{
	u32 speed = RTPSE_UART_BAUD_DEFAULT;
	struct device *dev = &serdev->dev;
	struct rtpse_uart *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->serdev = serdev;
	ctx->pse.dev = dev;
	ctx->pse.transport = &rtpse_uart_transport_ops;
	init_completion(&ctx->rx_done);

	serdev_device_set_drvdata(serdev, ctx);
	serdev_device_set_client_ops(serdev, &rtpse_uart_serdev_ops);

	ret = devm_serdev_device_open(dev, serdev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to open serdev\n");

	fwnode_property_read_u32(dev_fwnode(dev), "current-speed", &speed);
	serdev_device_set_baudrate(serdev, speed);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	return rtpse_register(&ctx->pse);
}

static const struct of_device_id rtpse_uart_of_match[] = {
	RTPSE_OF_MATCH_ENTRIES
};
MODULE_DEVICE_TABLE(of, rtpse_uart_of_match);

static struct serdev_device_driver rtpse_uart_driver = {
	.driver = {
		.name = "realtek-pse-uart",
		.of_match_table = rtpse_uart_of_match,
	},
	.probe  = rtpse_uart_probe,
};
module_serdev_device_driver(rtpse_uart_driver);

MODULE_AUTHOR("Jonas Jelonek <jelonek.jonas@gmail.com>");
MODULE_DESCRIPTION("Driver for MCU fronting Realtek/Broadcom PoE PSE chips (UART transport)");
MODULE_LICENSE("GPL");
