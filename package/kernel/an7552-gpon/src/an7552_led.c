// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha AN7552 xPON MAC - PON and LOS LED triggers.
 *
 * The SDK drives two board LEDs through ledctrl (xpon_led.c):
 * change_pon_led_status() -> LED_XPON_STATUS and change_alarm_led_status()
 * -> LED_XPON_LOS_ON_STATUS, each off / on / flicker, flicker being
 * ledBlinkOn(led, LED_SPEED_MEDIUM) = 500 ms on, 500 ms off
 * (ledctrl_kernel_api.c:71-80).  Here they are the LED triggers "xpon-pon"
 * and "xpon-los", to be bound to the board LEDs in the DTS
 * (linux,default-trigger) or in /etc/config/system.
 *
 * GPON (stock xPON driver):
 *   xpondrv_init()             LOS flicker
 *   gpon_act_led_config(), on every state change but O6:
 *     O1                       PON off, LOS flicker
 *     O7                       PON off, LOS on
 *     O2                       PON flicker, LOS off
 *     O3, O4                   LOS off
 *     O5                       LOS off
 *   gwan_create_new_gemport()  PON on for a GEM port other than the OMCC
 *   Type-B recovery to O5      PON on (gpon_ploam.c:573)
 *   xpon_phy_stop_rouge_handle()  LOS off
 */
#include <linux/leds.h>

#include "an7552_mac.h"

/* LED_SPEED_MEDIUM_BNO */

/* delay_off 0 = on, delay_on 0 = off: led_blink_set() also stops a blink */
static void an7552_led_delays(enum an7552_led_mode mode, unsigned long *on,
			    unsigned long *off)
{
	switch (mode) {
	case XPON_LED_ON:
		*on = 1;
		*off = 0;
		break;
	case XPON_LED_BLINK:
		*on = XPON_LED_BLINK_MS;
		*off = XPON_LED_BLINK_MS;
		break;
	default:
		*on = 0;
		*off = 1;
		break;
	}
}

/* a LED bound later takes the current mode */
static int an7552_led_activate(struct led_classdev *cdev)
{
	struct an7552_led *l = container_of(cdev->trigger,
						struct an7552_led, trig);
	unsigned long on, off;

	an7552_led_delays(READ_ONCE(l->mode), &on, &off);
	led_blink_set_nosleep(cdev, on, off);

	return 0;
}

/* called with x->lock held */
void an7552_led_set(struct an7552_led *l, enum an7552_led_mode mode)
{
	unsigned long on, off;

	if (!l->registered || READ_ONCE(l->mode) == mode)
		return;
	WRITE_ONCE(l->mode, mode);
	an7552_led_delays(mode, &on, &off);
	led_trigger_blink(&l->trig, on, off);
}

/* gpon_act_led_config(); called with x->lock held */
void an7552_led_gpon_state(struct an7552_mac *x)
{
	switch (x->state) {
	case GPON_O6:
		break;
	case GPON_O1:
		an7552_led_set(&x->led_los, XPON_LED_BLINK);
		an7552_led_set(&x->led_pon, XPON_LED_OFF);
		break;
	case GPON_O7:
		an7552_led_set(&x->led_los, XPON_LED_ON);
		an7552_led_set(&x->led_pon, XPON_LED_OFF);
		break;
	case GPON_O2:
		an7552_led_set(&x->led_pon, XPON_LED_BLINK);
		fallthrough;
	default:
		an7552_led_set(&x->led_los, XPON_LED_OFF);
		break;
	}
}

static int an7552_led_register(struct an7552_led *l, const char *name,
			     enum an7552_led_mode mode)
{
	int ret;

	l->trig.name = name;
	l->trig.activate = an7552_led_activate;
	l->mode = mode;
	ret = led_trigger_register(&l->trig);
	if (!ret)
		l->registered = true;

	return ret;
}

int an7552_led_init(struct an7552_mac *x)
{
	int ret;

	ret = an7552_led_register(&x->led_pon, "xpon-pon", XPON_LED_OFF);
	if (ret)
		return ret;
	/* xpondrv_init(): change_alarm_led_status(ALARM_LED_FLICKER) */
	ret = an7552_led_register(&x->led_los, "xpon-los", XPON_LED_BLINK);
	if (ret) {
		led_trigger_unregister(&x->led_pon.trig);
		x->led_pon.registered = false;
	}

	return ret;
}

/* the setters run with x->lock held; stop them before unregistering */
void an7552_led_exit(struct an7552_mac *x)
{
	bool pon, los;

	mutex_lock(&x->lock);
	pon = x->led_pon.registered;
	los = x->led_los.registered;
	x->led_pon.registered = false;
	x->led_los.registered = false;
	mutex_unlock(&x->lock);

	if (los)
		led_trigger_unregister(&x->led_los.trig);
	if (pon)
		led_trigger_unregister(&x->led_pon.trig);
}
