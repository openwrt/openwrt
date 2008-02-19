/*
 * OLPC touchpad PS/2 mouse driver
 *
 * Copyright (c) 2006 One Laptop Per Child, inc.
 * Authors Zephaniah E. Hull and Andres Salomon <dilinger@laptop.org>
 *
 * This driver is partly based on the ALPS driver, which is:
 *
 * Copyright (c) 2003 Neil Brown <neilb@cse.unsw.edu.au>
 * Copyright (c) 2003-2005 Peter Osterlund <petero2@telia.com>
 * Copyright (c) 2004 Dmitry Torokhov <dtor@mail.ru>
 * Copyright (c) 2005 Vojtech Pavlik <vojtech@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * The touchpad on the OLPC is fairly wide, with the entire area usable
 * as a tablet ("PT mode"), and the center 1/3rd also usable as a touchpad
 * ("GS mode").
 *
 * Earlier version of the device had simultaneous reporting; however, that
 * was removed.  Instead, the device now reports packets in one mode, and
 * tells the driver when a mode switch needs to happen.
 */

#include <linux/input.h>
#include <linux/serio.h>
#include <linux/libps2.h>
#include <linux/delay.h>
#include <asm/olpc.h>

#include "psmouse.h"
#include "olpc.h"

static int tpdebug;
module_param(tpdebug, int, 0644);

#define OLPC_GS		1	/* The GS sensor. */
#define OLPC_PT		2	/* The PT sensor. */

static struct olpc_model_info olpc_model_data[] = {
	{ { 0x67, 0x00, 0x00 }, OLPC_GS | OLPC_PT }, /* unknown ID */
	{ { 0x67, 0x00, 0x0a }, OLPC_GS | OLPC_PT }, /* pre-B1 */
	{ { 0x67, 0x00, 0x14 }, OLPC_GS }, /* B1.*/
	{ { 0x67, 0x00, 0x28 }, OLPC_GS | OLPC_PT }, /* B2 */
	{ { 0x67, 0x00, 0x3c }, OLPC_GS | OLPC_PT }, /* B2-2 */
	{ { 0x67, 0x00, 0x50 }, OLPC_GS | OLPC_PT }, /* C1 */
};

#define OLPC_PKT_PT	0xcf
#define OLPC_PKT_GS	0xff

static int olpc_absolute_mode(struct psmouse *psmouse, int mode);

/*
 * OLPC absolute Mode - single-mode format
 *
 * byte 0:      1    1    0    0    1    1     1     1
 * byte 1:      0   x6   x5   x4   x3   x2    x1    x0
 * byte 2(PT):  0    0   x9   x8   x7    ? pt-dsw gs-dsw
 * byte 2(GS):  0  x10   x9   x8   x7    ? gs-dsw pt-dsw
 * byte 3:      0   y9   y8   y7    1    0   swr   swl
 * byte 4:      0   y6   y5   y4   y3   y2    y1    y0
 * byte 5:      0   z6   z5   z4   z3   z2    z1    z0
 *
 * ?'s are not defined in the protocol spec, may vary between models.
 *
 * swr/swl are the left/right buttons.
 *
 * pt-dsw/gs-dsw indicate that the pt/gs sensor is detecting a
 * pen/finger
 */

static void olpc_process_packet_gspt(struct psmouse *psmouse)
{
	struct olpc_data *priv = psmouse->private;
	unsigned char *packet = psmouse->packet;
	struct input_dev *dev = psmouse->dev;
	struct input_dev *dev2 = priv->dev2;
	int x, y, z, gs_down = 0, pt_down = 0, left, right;
	struct timeval now_tv;
	s64 now_ns;

	left = packet[3] & 1;
	right = packet[3] & 2;
	x = packet[1] | ((packet[2] & 0x78) << 4);
	y = packet[4] | ((packet[3] & 0x70) << 3);
	z = packet[5];

	if (psmouse->packet[0] == OLPC_PKT_GS) {
		pt_down = !!(packet[2] & 1);
		gs_down = !!(packet[2] & 2);
	} else if (psmouse->packet[0] == OLPC_PKT_PT) {
		gs_down = !!(packet[2] & 1);
		pt_down = !!(packet[2] & 2);
	}

	/*
	 * XXX: Kludge.
	 * If it's been more than 30ms since the last packet,
	 * assume that there was a lift we were never told about.
	 */
	do_gettimeofday(&now_tv);
	now_ns = timeval_to_ns (&now_tv);
	if (now_ns >= priv->late) {
		input_report_key(dev, BTN_TOUCH, 0);
		input_report_key(dev, BTN_TOOL_PEN, 0);
		input_report_key(dev2, BTN_TOUCH, 0);
		input_report_key(dev2, BTN_TOOL_FINGER, 0);

		input_sync(dev);
		input_sync(dev2);
	}

	priv->late = now_ns + (30 * NSEC_PER_MSEC);


	if (tpdebug) {
		printk(KERN_DEBUG "%s %02x %02x %02x %02x %02x %02x\n",
		       __FUNCTION__, psmouse->packet[0], psmouse->packet[1],
		       psmouse->packet[2], psmouse->packet[3], psmouse->packet[4],
		       psmouse->packet[5]);
		printk(KERN_DEBUG "l=%d r=%d p=%d g=%d x=%d y=%d z=%d\n",
		       left, right, pt_down, gs_down, x, y, z);
	}

	if (psmouse->packet[0] == OLPC_PKT_PT) {
		input_report_key(dev, BTN_LEFT, left);
		input_report_key(dev, BTN_RIGHT, right);
	} else if (psmouse->packet[0] == OLPC_PKT_GS) {
		input_report_key(dev, BTN_LEFT, left);
		input_report_key(dev, BTN_RIGHT, right);
		input_report_key(dev2, BTN_LEFT, left);
		input_report_key(dev2, BTN_RIGHT, right);
	}

	input_report_key(dev, BTN_TOUCH, pt_down);
	input_report_key(dev, BTN_TOOL_PEN, pt_down);
	input_report_key(dev2, BTN_TOUCH, gs_down);
	input_report_key(dev2, BTN_TOOL_FINGER, gs_down);

	input_report_abs(dev2, ABS_PRESSURE, z);

	if (psmouse->packet[0] == OLPC_PKT_PT && pt_down) {
		input_report_abs(dev, ABS_X, x);
		input_report_abs(dev, ABS_Y, y);
	} else if (psmouse->packet[0] == OLPC_PKT_GS && gs_down) {
		input_report_abs(dev2, ABS_X, x);
		input_report_abs(dev2, ABS_Y, y);
	}

	input_sync(dev);
	input_sync(dev2);

	if (priv->pending_mode == OLPC_GS &&
			psmouse->packet[0] == OLPC_PKT_PT && pt_down) {
		priv->pending_mode = 0;
		cancel_delayed_work(&priv->mode_switch);
	}

	if (priv->i->flags & (OLPC_PT|OLPC_GS)) {
		int pending = 0;
		if (psmouse->packet[0] == OLPC_PKT_PT && !pt_down)
			pending = OLPC_GS;
		else if (psmouse->packet[0] == OLPC_PKT_GS && pt_down)
			pending = OLPC_PT;

		if (priv->current_mode == pending) {
			priv->pending_mode = 0;
			pending = priv->current_mode;
		}
		else if (priv->pending_mode != pending) {
			priv->pending_mode = pending;
			if (tpdebug)
				printk(KERN_WARNING "Scheduling mode switch to %s.\n",
						pending == OLPC_GS ? "GS" : "PT");

			/*
			 * Apply a de-bounce when switching from PT to GS, to allow for
			 * spurious PT-up packets.
			 */
			if (priv->pending_mode == OLPC_GS)
				queue_delayed_work(kpsmoused_wq, &priv->mode_switch, msecs_to_jiffies(50));
			else
				queue_delayed_work(kpsmoused_wq, &priv->mode_switch, 0);
		}
	}
}

static psmouse_ret_t olpc_process_byte(struct psmouse *psmouse)
{
	psmouse_ret_t ret = PSMOUSE_BAD_DATA;

	if (psmouse->packet[0] != OLPC_PKT_PT &&
	    psmouse->packet[0] != OLPC_PKT_GS)
		goto out;

	/* Bytes 2 - 6 should have 0 in the highest bit */
	if (psmouse->pktcnt >= 2 && psmouse->pktcnt <= 6 &&
			(psmouse->packet[psmouse->pktcnt - 1] & 0x80))
		goto out;

	if (psmouse->pktcnt == 6) {
		olpc_process_packet_gspt(psmouse);
		ret = PSMOUSE_FULL_PACKET;
		goto out;
	}

	ret = PSMOUSE_GOOD_DATA;
out:
	if (ret != PSMOUSE_GOOD_DATA && ret != PSMOUSE_FULL_PACKET)
		printk(KERN_DEBUG "%s: (%d) %02x %02x %02x %02x %02x %02x\n",
		       __FUNCTION__, psmouse->pktcnt, psmouse->packet[0],
			   psmouse->packet[1], psmouse->packet[2],
			   psmouse->packet[3], psmouse->packet[4],
		       psmouse->packet[5]);
	return ret;
}

static struct olpc_model_info *olpc_get_model(struct psmouse *psmouse)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char param[4];
	int i;

	/*
	 * Now try "E7 report". Allowed responses are in
	 * olpc_model_data[].signature
	 */
	if (ps2_command(ps2dev,  NULL, PSMOUSE_CMD_SETSCALE21) ||
	    ps2_command(ps2dev,  NULL, PSMOUSE_CMD_SETSCALE21) ||
	    ps2_command(ps2dev,  NULL, PSMOUSE_CMD_SETSCALE21))
		return NULL;

	param[0] = param[1] = param[2] = 0xff;
	if (ps2_command(ps2dev, param, PSMOUSE_CMD_GETINFO))
		return NULL;

	pr_debug("olpc.c(%d): E7 report: %2.2x %2.2x %2.2x",
		__LINE__, param[0], param[1], param[2]);

	for (i = 0; i < ARRAY_SIZE(olpc_model_data); i++) {
		if (!memcmp(param, olpc_model_data[i].signature,
				sizeof(olpc_model_data[i].signature))) {
			printk(KERN_INFO __FILE__ ": OLPC touchpad revision 0x%x.\n", param[2]);
			return olpc_model_data + i;
		}
	}

	/* 
	 * ALPS creates new IDs pretty frequently; rather than listing them
	 * all, just assume they support the defaults.  We've set aside the
	 * first entry of olpc_model_data as the catch-all.
	 */
	if (!memcmp(param, olpc_model_data[0].signature, 2)) {
		printk(KERN_INFO __FILE__ ": unknown ALPS revision %x, assuming default flags.\n", param[2]);
		return &olpc_model_data[0];
	}

	return NULL;
}

static int olpc_find_mode(struct psmouse *psmouse)
{
	struct olpc_data *priv = psmouse->private;
	int mode = priv->i->flags;

	if (mode & OLPC_GS)
		mode = OLPC_GS;
	else if (mode & OLPC_PT)
		mode = OLPC_PT;
	else
		mode = -1;

	return mode;
}

/*
 * Touchpad should be disabled before calling this!
 */
static int olpc_new_mode(struct psmouse *psmouse, int mode)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	struct olpc_data *priv = psmouse->private;
	unsigned char param;
	int ret;

	if (tpdebug)
		printk(KERN_WARNING __FILE__ ": Switching to %d. [%lu]\n", mode, jiffies);

	if ((ret = ps2_command(ps2dev, &param, 0x01F2)))
		goto failed;
	if ((ret = ps2_command(ps2dev, &param, 0x01F2)))
		goto failed;
	if ((ret = ps2_command(ps2dev, &param, 0x01F2)))
		goto failed;

	switch (mode) {
	default:
		printk(KERN_WARNING __FILE__ ": Invalid mode %d. Defaulting to OLPC_GS.\n", mode);
	case OLPC_GS:
		ret = ps2_command(ps2dev, NULL, 0xE6);
		break;
	case OLPC_PT:
		ret = ps2_command(ps2dev, NULL, 0xE7);
		break;
	}
	if (ret)
		goto failed;

	/* XXX: This is a bit hacky, make sure this isn't screwing stuff up. */
	psmouse->pktcnt = psmouse->out_of_sync = 0;
	psmouse->last = jiffies;
	psmouse->state = PSMOUSE_ACTIVATED;

	if ((ret = ps2_command(ps2dev, NULL, PSMOUSE_CMD_ENABLE)))
		goto failed;

	priv->current_mode = mode;
	priv->pending_mode = 0;
	if (tpdebug)
		printk(KERN_WARNING __FILE__ ": Switched to mode %d successful.\n", mode);

failed:
	if (ret)
		printk(KERN_WARNING __FILE__ ": Mode switch to %d failed! (%d) [%lu]\n", mode, ret, jiffies);
	return ret;
}

static int olpc_absolute_mode(struct psmouse *psmouse, int mode)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;

	/* Switch to 'Advanced mode.', four disables in a row. */
	if (ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE) ||
			ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE) ||
			ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE) ||
			ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE))
		return -1;
	
	return olpc_new_mode(psmouse, mode);
}

/*
 * olpc_poll() - poll the touchpad for current motion packet.
 * Used in resync.
 * Note: We can't poll, so always return failure.
 */
static int olpc_poll(struct psmouse *psmouse)
{
	return -1;
}

static int olpc_reconnect(struct psmouse *psmouse)
{
	struct olpc_data *priv = psmouse->private;
	int mode;

	if (olpc_rev_after(OLPC_REV_B2))
		if (psmouse->ps2dev.serio->dev.power.power_state.event != PM_EVENT_ON)
			return 0;

	psmouse_reset(psmouse);

	if (!(priv->i = olpc_get_model(psmouse)))
		return -1;

	mode = olpc_find_mode(psmouse);
	if (mode < 0)
		return -1;

	if (olpc_absolute_mode(psmouse, mode)) {
		printk(KERN_ERR __FILE__ ": Failed to reenable absolute mode.\n");
		return -1;
	}

	return 0;
}

static void olpc_disconnect(struct psmouse *psmouse)
{
	struct olpc_data *priv = psmouse->private;

	psmouse_reset(psmouse);
	input_unregister_device(priv->dev2);
	kfree(priv);
}

static void olpc_mode_switch(struct work_struct *w)
{
	struct delayed_work *work = container_of(w, struct delayed_work, work);
	struct olpc_data *priv = container_of(work, struct olpc_data, mode_switch);
	struct psmouse *psmouse = priv->psmouse;
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	int pending_mode, ret;

	if (priv->pending_mode == priv->current_mode) {
		priv->pending_mode = 0;
		printk (KERN_DEBUG __FILE__ ": In switch_mode, no target mode.\n");
		return;
	}

	if (tpdebug)
		printk(KERN_WARNING __FILE__ ": Disable for switch to %d. [%lu]\n", priv->pending_mode, jiffies);

	/* XXX: This is a bit hacky, make sure this isn't screwing stuff up. */
	psmouse->state = PSMOUSE_INITIALIZING;

	ret = ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE);
	if (ret) {
		/* XXX: if this ever fails, we need to do a full reset! */
		printk(KERN_WARNING __FILE__ ": Disable failed for switch to %d. (%d) [%lu]\n", priv->pending_mode, ret, jiffies);
		return;
	}

	/*
	 * ALPS tells us that it may take up to 20msec for the disable to
	 * take effect; however, ps2_command() will wait up to 200msec for
	 * the ACK to come back (and I'm assuming that by the time the
	 * hardware sends back its ACK, it has stopped sending bytes).
	 */
	pending_mode = priv->pending_mode;

	if (olpc_new_mode(psmouse, priv->pending_mode))
		goto bad;

	/*
	 * Deal with a potential race condition.
	 *
	 * If there is a brief tap of a stylus or a fingernail that
	 * triggers a mode switch to PT mode, and the stylus/fingernail is
	 * lifted after the DISABLE above, but before we reenable in the new mode,
	 * then we can get stuck in PT mode.
	 */
	if (pending_mode == OLPC_PT) {
		priv->pending_mode = OLPC_GS;
		queue_delayed_work(kpsmoused_wq, &priv->mode_switch, msecs_to_jiffies(50));
	}

	return;

bad:
	printk(KERN_WARNING __FILE__ ": Failure to switch modes, resetting device...\n");
	olpc_reconnect(psmouse);
}

int olpc_init(struct psmouse *psmouse)
{
	struct olpc_data *priv;
	struct input_dev *dev = psmouse->dev;
	struct input_dev *dev2;
	int mode;

	priv = kzalloc(sizeof(struct olpc_data), GFP_KERNEL);
	dev2 = input_allocate_device();
	if (!priv || !dev2)
		goto init_fail;

	psmouse->private = priv;
	priv->dev2 = dev2;
	priv->psmouse = psmouse;

	psmouse_reset(psmouse);
	if (!(priv->i = olpc_get_model(psmouse)))
		goto init_fail;

	mode = olpc_find_mode(psmouse);
	if (mode < 0) {
		printk(KERN_ERR __FILE__ ": Failed to identify proper mode\n");
		goto init_fail;
	}

	if (olpc_absolute_mode(psmouse, mode)) {
		printk(KERN_ERR __FILE__ ": Failed to enable absolute mode\n");
		goto init_fail;
	}

	/*
	 * Unset some of the default bits for things we don't have.
	 */
	dev->evbit[LONG(EV_REL)] &= ~BIT(EV_REL);
	dev->relbit[LONG(REL_X)] &= ~(BIT(REL_X) | BIT(REL_Y));
	dev->keybit[LONG(BTN_MIDDLE)] &= ~BIT(BTN_MIDDLE);

	dev->evbit[LONG(EV_KEY)] |= BIT(EV_KEY);
	dev->keybit[LONG(BTN_TOUCH)] |= BIT(BTN_TOUCH);
	dev->keybit[LONG(BTN_TOOL_PEN)] |= BIT(BTN_TOOL_PEN);
	dev->keybit[LONG(BTN_LEFT)] |= BIT(BTN_LEFT) | BIT(BTN_RIGHT);

	dev->evbit[LONG(EV_ABS)] |= BIT(EV_ABS);
	input_set_abs_params(dev, ABS_X, 2, 1000, 0, 0);
	input_set_abs_params(dev, ABS_Y, 0, 717, 0, 0);

	snprintf(priv->phys, sizeof(priv->phys),
		"%s/input1", psmouse->ps2dev.serio->phys);
	dev2->phys = priv->phys;
	dev2->name = "OLPC ALPS GlideSensor";
	dev2->id.bustype = BUS_I8042;
	dev2->id.vendor  = 0x0002;
	dev2->id.product = PSMOUSE_OLPC;
	dev2->id.version = 0x0000;

	dev2->evbit[LONG(EV_KEY)] |= BIT(EV_KEY);
	dev2->keybit[LONG(BTN_TOUCH)] |= BIT(BTN_TOUCH);
	dev2->keybit[LONG(BTN_TOOL_FINGER)] |= BIT(BTN_TOOL_FINGER);
	dev2->keybit[LONG(BTN_LEFT)] |= BIT(BTN_LEFT) | BIT(BTN_RIGHT);

	dev2->evbit[LONG(EV_ABS)] |= BIT(EV_ABS);
	input_set_abs_params(dev2, ABS_X, 350, 512, 0, 0);
	input_set_abs_params(dev2, ABS_Y, 70, 325, 0, 0);
	input_set_abs_params(dev2, ABS_PRESSURE, 0, 63, 0, 0);

	if (input_register_device(dev2)) {
		printk(KERN_ERR __FILE__ ": Failed to register GlideSensor\n");
		goto init_fail;
	}

	psmouse->protocol_handler = olpc_process_byte;
	psmouse->poll = olpc_poll;
	psmouse->disconnect = olpc_disconnect;
	psmouse->reconnect = olpc_reconnect;
	psmouse->pktsize = 6;

	/* Disable the idle resync. */
	psmouse->resync_time = 0;
	/* Reset after a lot of bad bytes. */
	psmouse->resetafter = 1024;

	INIT_DELAYED_WORK(&priv->mode_switch, olpc_mode_switch);

	return 0;

init_fail:
	input_free_device(dev2);
	kfree(priv);
	return -1;
}

int olpc_detect(struct psmouse *psmouse, int set_properties)
{
	if (!olpc_get_model(psmouse))
		return -1;

	if (set_properties) {
		psmouse->vendor = "ALPS";
		psmouse->name = "PenTablet";
		psmouse->model = 0;
	}
	return 0;
}

