/*
 *  USB LED driver for the TPLINK MR3420 v3
 *
 *  Copyright (C) 2009 Jacek Trzcinski <jt@mgt.pl>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#define DRIVER_NAME	"leds-tl-mr3420-v3"


#define TL_MR3420_V3_GPIO11_LATCH_OE	11
#define TL_MR3420_V3_GPIO16_SHFT_IN	16
#define TL_MR3420_V3_GPIO15_SHFT_CLK	15
#define TL_MR3420_V3_GPIO14_SHFT_RST	14
#define TL_MR3420_V3_GPIO13_LATCH_CLK	13


/*
NOTE: the order of the LAN LEDs has been inversed to see 'on' state
      when respective RJ45 "behind LED" is connected
      and now is as following:
	pwr/status	wlan	lan4	lan3	lan2	lan1	3g	qss


  74hc595 chip
                  _________        _________   hi bit              LED   (all active low - except wan)
        input ---|         |------|         |------QA                lan4
    (gpio 16)    |         |------|         |------QB                lan3
                 | shift   |------| latch   |------QC  parallel      lan2
                 |register |------|         |------QD     data       lan1
                 |         |------|         |------QE     output     wan    dual color 2pin LED  (color
                 |         |------|         |------QF                wan       controlled with  QE,QF state)
                 |         |------|         |------QG                wlan
        shift    |         |------|         |------QH                3g
         clock --|>        |    --|>        |  lo bit
    (gpio 15)    |_________|--  | |_________|
                      o      |  |       o
(gpio 14) reset ______|      |  |       |
(gpio 13) latch clk _________+__|       |
(gpio 11) output enable _____+__________|
                             |
                             |___________________ ser data out


*/

static uint8_t tl_mr3420_v3_latch_state;

#define TL_MR3420_V3_LED_LAN4		0x7F
#define TL_MR3420_V3_LED_LAN3		0xBF
#define TL_MR3420_V3_LED_LAN2		0xDF
#define TL_MR3420_V3_LED_LAN1		0xEF
#define TL_MR3420_V3_LED_WAN_GREEN	0xF7	//--->F3
#define TL_MR3420_V3_LED_WAN_RED	0xFB	//--->F3
#define TL_MR3420_V3_LED_WLAN		0xFD
#define TL_MR3420_V3_LED_3G		0xFE


static void tl_mr3420_v3_set_leds(uint8_t led_bits) {   // bit val 0  <----> repective LED 'on'
 uint8_t ii;

   tl_mr3420_v3_latch_state = led_bits;

   // clear shift register
   gpio_set_value(TL_MR3420_V3_GPIO14_SHFT_RST, 0);
   gpio_set_value(TL_MR3420_V3_GPIO14_SHFT_RST, 1);

   for(ii = 1; ii != 0 ; ii = ii << 1) {
      // set input pin state
      if(tl_mr3420_v3_latch_state & ii) {
         gpio_set_value(TL_MR3420_V3_GPIO16_SHFT_IN, 1);
      }
      else {
         gpio_set_value(TL_MR3420_V3_GPIO16_SHFT_IN, 0);
      }

      // enter into shift reg
      gpio_set_value(TL_MR3420_V3_GPIO15_SHFT_CLK, 1);
      gpio_set_value(TL_MR3420_V3_GPIO15_SHFT_CLK, 0);
   }

   // set latch
   gpio_set_value(TL_MR3420_V3_GPIO13_LATCH_CLK, 1);
   gpio_set_value(TL_MR3420_V3_GPIO13_LATCH_CLK, 0);

   gpio_set_value(TL_MR3420_V3_GPIO16_SHFT_IN, 0);        // 'normalize' input pin state
}

static void tl_mr3420_v3_clr_leds(void) {
   gpio_set_value(TL_MR3420_V3_GPIO11_LATCH_OE, 1);       // disable output
   gpio_set_value(TL_MR3420_V3_GPIO14_SHFT_RST, 0);       // set 'reset' of shift register

   // normalize input, shft clk & latch clk
   gpio_set_value(TL_MR3420_V3_GPIO16_SHFT_IN, 0);
   gpio_set_value(TL_MR3420_V3_GPIO13_LATCH_CLK, 0);
   gpio_set_value(TL_MR3420_V3_GPIO15_SHFT_CLK, 0);

   gpio_set_value(TL_MR3420_V3_GPIO14_SHFT_RST, 1);       // remove 'reset' state of shift register

   // clear all LEDs
   tl_mr3420_v3_latch_state = 0xFF;
   tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state);

   gpio_set_value(TL_MR3420_V3_GPIO11_LATCH_OE, 0);       // enable output
}



/*     3G    */
static void tl_mr3420_v3_led_3g_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_3G);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_3G));
}

static enum led_brightness tl_mr3420_v3_led_3g_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_3G)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_3g = {
	.name = "tp-link:green:3g",
	.brightness_set = tl_mr3420_v3_led_3g_set,
	.brightness_get = tl_mr3420_v3_led_3g_get,
};


/*     WAN green   */
static void tl_mr3420_v3_led_wan_green_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds((tl_mr3420_v3_latch_state &  TL_MR3420_V3_LED_WAN_GREEN ) | (~TL_MR3420_V3_LED_WAN_RED));
	else           tl_mr3420_v3_set_leds((tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_WAN_GREEN) ));

}

static enum led_brightness tl_mr3420_v3_led_wan_green_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_WAN_GREEN)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_wan_green = {
	.name = "tp-link:green:wan",
	.brightness_set = tl_mr3420_v3_led_wan_green_set,
	.brightness_get = tl_mr3420_v3_led_wan_green_get,
};


/*     WAN red   */
static void tl_mr3420_v3_led_wan_red_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds((tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_WAN_GREEN )) & TL_MR3420_V3_LED_WAN_RED );
	else           tl_mr3420_v3_set_leds((tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_WAN_RED   )) );
}

static enum led_brightness tl_mr3420_v3_led_wan_red_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_WAN_RED)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_wan_red = {
	.name = "tp-link:red:wan",
	.brightness_set = tl_mr3420_v3_led_wan_red_set,
	.brightness_get = tl_mr3420_v3_led_wan_red_get,
};

/*     WLAN    */
static void tl_mr3420_v3_led_wlan_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_WLAN);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_WLAN));
}

static enum led_brightness tl_mr3420_v3_led_wlan_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_WLAN)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_wlan = {
	.name = "tp-link:green:wlan",
	.brightness_set = tl_mr3420_v3_led_wlan_set,
	.brightness_get = tl_mr3420_v3_led_wlan_get,
};


/*     LAN1    */
static void tl_mr3420_v3_led_lan1_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_LAN1);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_LAN1));
}

static enum led_brightness tl_mr3420_v3_led_lan1_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_LAN1)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_lan1 = {
	.name = "tp-link:green:lan1",
	.brightness_set = tl_mr3420_v3_led_lan1_set,
	.brightness_get = tl_mr3420_v3_led_lan1_get,
};


/*     LAN2    */
static void tl_mr3420_v3_led_lan2_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_LAN2);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_LAN2));
}

static enum led_brightness tl_mr3420_v3_led_lan2_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_LAN2)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_lan2 = {
	.name = "tp-link:green:lan2",
	.brightness_set = tl_mr3420_v3_led_lan2_set,
	.brightness_get = tl_mr3420_v3_led_lan2_get,
};


/*     LAN3    */
static void tl_mr3420_v3_led_lan3_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_LAN3);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_LAN3));
}

static enum led_brightness tl_mr3420_v3_led_lan3_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_LAN3)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_lan3 = {
	.name = "tp-link:green:lan3",
	.brightness_set = tl_mr3420_v3_led_lan3_set,
	.brightness_get = tl_mr3420_v3_led_lan3_get,
};


/*     LAN4    */
static void tl_mr3420_v3_led_lan4_set(struct led_classdev *cdev, enum led_brightness brightness) {
	if(brightness) tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state & TL_MR3420_V3_LED_LAN4);
	else tl_mr3420_v3_set_leds(tl_mr3420_v3_latch_state | (~TL_MR3420_V3_LED_LAN4));
}

static enum led_brightness tl_mr3420_v3_led_lan4_get(struct led_classdev *cdev) {
	return( (~(tl_mr3420_v3_latch_state | TL_MR3420_V3_LED_LAN4)) != 0 ?  LED_FULL : LED_OFF);
}

static struct led_classdev tl_mr3420_v3_led_lan4 = {
	.name = "tp-link:green:lan4",
	.brightness_set = tl_mr3420_v3_led_lan4_set,
	.brightness_get = tl_mr3420_v3_led_lan4_get,
};


static int tl_mr3420_v3_leds_probe(struct platform_device *pdev) {
    int ret;
	if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_3g)) == 0) {
	    if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_wan_green)) == 0) {
		if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_wan_red)) == 0) {
		    if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_lan4)) == 0) {
			if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_lan3)) == 0) {
			    if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_lan2)) == 0) {
				if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_lan1)) == 0) {
				    if( (ret = led_classdev_register(&pdev->dev, &tl_mr3420_v3_led_wlan)) == 0) {
					tl_mr3420_v3_clr_leds();
					return(0);
				    }
				    led_classdev_unregister(&tl_mr3420_v3_led_lan1);
				}
				led_classdev_unregister(&tl_mr3420_v3_led_lan2);
			    }
			    led_classdev_unregister(&tl_mr3420_v3_led_lan3);
			}
			led_classdev_unregister(&tl_mr3420_v3_led_lan4);
		    }
		    led_classdev_unregister(&tl_mr3420_v3_led_wan_red);
		}
		led_classdev_unregister(&tl_mr3420_v3_led_wan_green);
	    }
	    led_classdev_unregister(&tl_mr3420_v3_led_3g);
	}
	return(ret);
}

static int tl_mr3420_v3_leds_remove(struct platform_device *pdev) {
	led_classdev_unregister(&tl_mr3420_v3_led_wlan);
	led_classdev_unregister(&tl_mr3420_v3_led_lan1);
	led_classdev_unregister(&tl_mr3420_v3_led_lan2);
	led_classdev_unregister(&tl_mr3420_v3_led_lan3);
	led_classdev_unregister(&tl_mr3420_v3_led_lan4);
	led_classdev_unregister(&tl_mr3420_v3_led_wan_red);
	led_classdev_unregister(&tl_mr3420_v3_led_wan_green);
	led_classdev_unregister(&tl_mr3420_v3_led_3g);
	return 0;
}


static struct platform_driver tl_mr3420_v3_leds_driver = {
	.probe = tl_mr3420_v3_leds_probe,
	.remove = tl_mr3420_v3_leds_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
};



//---------------------------------------------------------------
static int __init tl_mr3420_v3_leds_init(void) {
    return platform_driver_register(&tl_mr3420_v3_leds_driver);
}

static void __exit tl_mr3420_v3_leds_exit(void) {
	platform_driver_unregister(&tl_mr3420_v3_leds_driver);
}

module_init(tl_mr3420_v3_leds_init);
module_exit(tl_mr3420_v3_leds_exit);

MODULE_DESCRIPTION("Some LEDs driver for TPLINK MR3420 v3");
MODULE_AUTHOR("Jacek Trzcinski <jt@mgt.pl>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
