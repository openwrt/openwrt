// SPDX-License-Identifier: GPL-2.0-only

#include <asm/mach-rtl-otto/mach-rtl-otto.h>
#include <linux/leds.h>
#include <linux/mutex.h>
#include <linux/of.h>

#include "leds.h"
#include "rtl-otto.h"

#define RTL930X_LED_SW_CTRL			(0xCC48)
#define RTL930X_LED_SW_CTRL_START		BIT(0)
/* 4 bits per port, set bits hand the LED from the scan engine to software */
#define RTL930X_LED_SW_P_EN_CTRL(p)		(0xCC4C + (((p) >> 3) << 2))
#define RTL930X_LED_SW_P_EN_SHIFT(p)		(((p) & 0x7) << 2)
/* 3 bits per LED, copper in 11:0 and fibre in 23:12 */
#define RTL930X_LED_SW_P_CTRL(p)		(0xCC5C + ((p) << 2))
#define RTL930X_LED_SW_MODE_SHIFT(l, fib)	(((fib) ? 12 : 0) + (l) * 3)
#define RTL930X_LED_SW_MODE_MASK(l, fib)	(0x7 << RTL930X_LED_SW_MODE_SHIFT(l, fib))
#define RTL930X_LED_SW_MODE_OFF			0
/* 1-6 blink, at rates doubling from 32 ms to 1024 ms */
#define RTL930X_LED_SW_MODE_ON			7

struct rtldsa_930x_sw_led {
	struct led_classdev cdev;
	struct rtl838x_switch_priv *priv;
	u8 port;
	u8 index;
};

static void rtldsa_930x_sw_led_mode_set(struct rtldsa_930x_sw_led *led, u32 mode)
{
	/* Which half the scan engine reads depends on the port type masks */
	u32 clear = RTL930X_LED_SW_MODE_MASK(led->index, false) |
		    RTL930X_LED_SW_MODE_MASK(led->index, true);
	u32 set = mode << RTL930X_LED_SW_MODE_SHIFT(led->index, false) |
		  mode << RTL930X_LED_SW_MODE_SHIFT(led->index, true);

	mutex_lock(&led->priv->reg_mutex);
	sw_w32_mask(clear, set, RTL930X_LED_SW_P_CTRL(led->port));
	sw_w32(RTL930X_LED_SW_CTRL_START, RTL930X_LED_SW_CTRL);
	mutex_unlock(&led->priv->reg_mutex);
}

static int rtldsa_930x_sw_led_brightness_set(struct led_classdev *cdev,
					     enum led_brightness brightness)
{
	struct rtldsa_930x_sw_led *led = container_of(cdev, struct rtldsa_930x_sw_led, cdev);

	rtldsa_930x_sw_led_mode_set(led, brightness ? RTL930X_LED_SW_MODE_ON :
						      RTL930X_LED_SW_MODE_OFF);

	return 0;
}

static enum led_brightness rtldsa_930x_sw_led_brightness_get(struct led_classdev *cdev)
{
	struct rtldsa_930x_sw_led *led = container_of(cdev, struct rtldsa_930x_sw_led, cdev);
	u32 v = sw_r32(RTL930X_LED_SW_P_CTRL(led->port));

	return (v & RTL930X_LED_SW_MODE_MASK(led->index, false)) ? LED_ON : LED_OFF;
}

static void rtldsa_930x_sw_led_own(struct rtldsa_930x_sw_led *led, bool own)
{
	u32 bit = BIT(led->index) << RTL930X_LED_SW_P_EN_SHIFT(led->port);

	mutex_lock(&led->priv->reg_mutex);
	sw_w32_mask(own ? 0 : bit, own ? bit : 0, RTL930X_LED_SW_P_EN_CTRL(led->port));
	mutex_unlock(&led->priv->reg_mutex);
}

/* Hand the LED back to the scan engine */
static void rtldsa_930x_sw_led_release(void *led)
{
	rtldsa_930x_sw_led_own(led, false);
}

static int rtldsa_930x_sw_led_add(struct rtl838x_switch_priv *priv,
				  struct device_node *np)
{
	struct led_init_data init_data = {};
	struct rtldsa_930x_sw_led *led;
	u32 reg[2];
	int err;

	err = of_property_read_u32_array(np, "reg", reg, ARRAY_SIZE(reg));
	if (err)
		return err;

	/* Past the CPU port the per-port control array runs into other registers */
	if (reg[0] >= priv->r->cpu_port || reg[1] > 3)
		return -EINVAL;

	led = devm_kzalloc(priv->dev, sizeof(*led), GFP_KERNEL);
	if (!led)
		return -ENOMEM;

	led->priv = priv;
	led->port = reg[0];
	led->index = reg[1];
	led->cdev.max_brightness = LED_ON;
	led->cdev.brightness_set_blocking = rtldsa_930x_sw_led_brightness_set;
	led->cdev.brightness_get = rtldsa_930x_sw_led_brightness_get;

	switch (led_init_default_state_get(of_fwnode_handle(np))) {
	case LEDS_DEFSTATE_ON:
		led->cdev.brightness = LED_ON;
		break;
	case LEDS_DEFSTATE_KEEP:
		led->cdev.brightness = rtldsa_930x_sw_led_brightness_get(&led->cdev);
		break;
	default:
		led->cdev.brightness = LED_OFF;
		break;
	}

	/*
	 * Registration reads the brightness back from the hardware, so the
	 * LED is taken over and set to its default state first.
	 */
	rtldsa_930x_sw_led_own(led, true);
	rtldsa_930x_sw_led_brightness_set(&led->cdev, led->cdev.brightness);

	err = devm_add_action_or_reset(priv->dev, rtldsa_930x_sw_led_release, led);
	if (err)
		return err;

	init_data.fwnode = of_fwnode_handle(np);

	err = devm_led_classdev_register_ext(priv->dev, &led->cdev, &init_data);
	if (err)
		devm_release_action(priv->dev, rtldsa_930x_sw_led_release, led);

	return err;
}

void rtldsa_930x_sw_leds_init(struct rtl838x_switch_priv *priv,
			      struct device_node *node)
{
	struct device_node *leds;
	int err;

	leds = of_get_child_by_name(node, "software-leds");
	if (!leds)
		return;

	for_each_available_child_of_node_scoped(leds, np) {
		err = rtldsa_930x_sw_led_add(priv, np);
		if (err)
			dev_err(priv->dev, "failed to add LED %pOF: %d\n", np, err);
	}

	of_node_put(leds);
}
