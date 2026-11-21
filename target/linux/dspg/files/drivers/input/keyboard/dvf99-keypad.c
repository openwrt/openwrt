// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/input/matrix_keypad.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>

#define DVF99_KBD_CFG		0x00
#define DVF99_KBD_ROW		0x04
#define DVF99_KBD_COL		0x08

#define DVF99_KBD_CFG_ROW_SHIFT	8
#define DVF99_KBD_MAX_ROWS	10
#define DVF99_KBD_MAX_COLS	8
#define DVF99_KBD_SETTLE_US	20
#define DVF99_KBD_DEBOUNCE_MS	50

struct dvf99_keypad {
	struct input_dev *input;
	void __iomem *base;
	struct delayed_work scan_work;
	unsigned int rows;
	unsigned int cols;
	unsigned int row_shift;
	int irq;
	u16 state[DVF99_KBD_MAX_COLS];
};

static void dvf99_keypad_strobe(struct dvf99_keypad *kp, u32 cols)
{
	writel(cols, kp->base + DVF99_KBD_COL);
	usleep_range(DVF99_KBD_SETTLE_US, 2 * DVF99_KBD_SETTLE_US);
}

static void dvf99_keypad_scan(struct work_struct *work)
{
	struct dvf99_keypad *kp = container_of(to_delayed_work(work),
					       struct dvf99_keypad, scan_work);
	const unsigned short *keycodes = kp->input->keycode;
	unsigned int row, col, code;
	u16 sense, changed;

	for (col = 0; col < kp->cols; col++) {
		dvf99_keypad_strobe(kp, BIT(col));
		sense = readl(kp->base + DVF99_KBD_ROW) &
			GENMASK(kp->rows - 1, 0);
		changed = sense ^ kp->state[col];
		kp->state[col] = sense;

		for (row = 0; row < kp->rows; row++) {
			if (!(changed & BIT(row)))
				continue;

			code = MATRIX_SCAN_CODE(row, col, kp->row_shift);
			input_event(kp->input, EV_MSC, MSC_SCAN, code);
			input_report_key(kp->input, keycodes[code],
					 sense & BIT(row));
		}
	}

	input_sync(kp->input);

	dvf99_keypad_strobe(kp, GENMASK(kp->cols - 1, 0));
	enable_irq(kp->irq);
}

static irqreturn_t dvf99_keypad_irq(int irq, void *data)
{
	struct dvf99_keypad *kp = data;

	disable_irq_nosync(irq);
	schedule_delayed_work(&kp->scan_work,
			      msecs_to_jiffies(DVF99_KBD_DEBOUNCE_MS));

	return IRQ_HANDLED;
}

static void dvf99_keypad_cancel(void *data)
{
	struct dvf99_keypad *kp = data;

	disable_irq(kp->irq);
	cancel_delayed_work_sync(&kp->scan_work);
}

static int dvf99_keypad_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf99_keypad *kp;
	int ret;

	kp = devm_kzalloc(dev, sizeof(*kp), GFP_KERNEL);
	if (!kp)
		return -ENOMEM;

	kp->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(kp->base))
		return PTR_ERR(kp->base);

	kp->irq = platform_get_irq(pdev, 0);
	if (kp->irq < 0)
		return kp->irq;

	ret = matrix_keypad_parse_properties(dev, &kp->rows, &kp->cols);
	if (ret)
		return ret;

	if (kp->rows > DVF99_KBD_MAX_ROWS || kp->cols > DVF99_KBD_MAX_COLS)
		return dev_err_probe(dev, -EINVAL, "matrix %ux%u too large\n",
				     kp->rows, kp->cols);

	kp->row_shift = get_count_order(kp->cols);

	kp->input = devm_input_allocate_device(dev);
	if (!kp->input)
		return -ENOMEM;

	kp->input->name = pdev->name;
	kp->input->phys = "dvf99-keypad/input0";
	kp->input->id.bustype = BUS_HOST;

	ret = matrix_keypad_build_keymap(NULL, NULL, kp->rows, kp->cols, NULL,
					 kp->input);
	if (ret)
		return dev_err_probe(dev, ret, "failed to build keymap\n");

	input_set_capability(kp->input, EV_MSC, MSC_SCAN);
	input_set_drvdata(kp->input, kp);

	INIT_DELAYED_WORK(&kp->scan_work, dvf99_keypad_scan);

	writel(GENMASK(kp->cols - 1, 0) |
	       GENMASK(kp->rows - 1, 0) << DVF99_KBD_CFG_ROW_SHIFT,
	       kp->base + DVF99_KBD_CFG);
	dvf99_keypad_strobe(kp, GENMASK(kp->cols - 1, 0));

	ret = devm_request_irq(dev, kp->irq, dvf99_keypad_irq, 0,
			       dev_name(dev), kp);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev, dvf99_keypad_cancel, kp);
	if (ret)
		return ret;

	return input_register_device(kp->input);
}

static const struct of_device_id dvf99_keypad_of_match[] = {
	{ .compatible = "dspg,dvf99-keypad" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_keypad_of_match);

static struct platform_driver dvf99_keypad_driver = {
	.probe = dvf99_keypad_probe,
	.driver = {
		.name = "dvf99-keypad",
		.of_match_table = dvf99_keypad_of_match,
	},
};
module_platform_driver(dvf99_keypad_driver);

MODULE_DESCRIPTION("DSPG DVF99 keypad driver");
MODULE_LICENSE("GPL");
