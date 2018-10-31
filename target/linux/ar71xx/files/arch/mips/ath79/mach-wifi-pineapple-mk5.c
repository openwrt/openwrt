#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "dev-usb.h"

#define WIFI_PINEAPPLE_MK5_DIP_01                      1
#define WIFI_PINEAPPLE_MK5_DIP_02                      13
#define WIFI_PINEAPPLE_MK5_DIP_03                      15
#define WIFI_PINEAPPLE_MK5_DIP_04                      16
#define WIFI_PINEAPPLE_MK5_DIP_05                      11

#define WIFI_PINEAPPLE_MK5_SD_01                       23

#define WIFI_PINEAPPLE_MK5_GPIO_LED_01                 27
#define WIFI_PINEAPPLE_MK5_GPIO_LED_02                 14
#define WIFI_PINEAPPLE_MK5_GPIO_LED_03                 17
#define WIFI_PINEAPPLE_MK5_GPIO_LED_POWER              0

#define WIFI_PINEAPPLE_MK5_KEYS_POLL_INTERVAL  20      /* msecs */
#define WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL (3 * WIFI_PINEAPPLE_MK5_KEYS_POLL_INTERVAL)

static const char *wifi_pineapple_mk5_part_probes[] = {
        "tp-link",
        NULL,
};

static struct flash_platform_data wifi_pineapple_mk5_flash_data = {
        .part_probes    = wifi_pineapple_mk5_part_probes,
};

static struct gpio_led wifi_pineapple_mk5_leds_gpio[] __initdata = {
        {
                .name           = "mk5:red:system",
                .gpio           = WIFI_PINEAPPLE_MK5_GPIO_LED_01,
                .active_low     = 1,
        },{
                .name           = "mk5:blue:wifi",
                .gpio           = WIFI_PINEAPPLE_MK5_GPIO_LED_02,
                .active_low     = 1,
        },{
                .name           = "mk5:yellow:lan",
                .gpio           = WIFI_PINEAPPLE_MK5_GPIO_LED_03,
                .active_low     = 1,
        },
};

static struct gpio_keys_button wifi_pineapple_mk5_gpio_keys[] __initdata = {
/*      {
                .desc           = "BTN_1",
                .type           = EV_KEY,
                .code           = BTN_1,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_DIP_01,
                .active_low     = 1,
        },{
                .desc           = "BTN_2",
                .type           = EV_KEY,
                .code           = BTN_2,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_DIP_02,
                .active_low     = 1,
        },{
                .desc           = "BTN_3",
                .type           = EV_KEY,
                .code           = BTN_3,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_DIP_03,
                .active_low     = 1,
        },{
                .desc           = "BTN_4",
                .type           = EV_KEY,
                .code           = BTN_4,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_DIP_04,
                .active_low     = 1,
        },{
                .desc           = "BTN_5",
                .type           = EV_KEY,
                .code           = BTN_5,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_DIP_05,
                .active_low     = 1,
        },
*/
        {
                .desc           = "SD",
                .type           = EV_KEY,
                .code           = BTN_6,
                .debounce_interval = WIFI_PINEAPPLE_MK5_KEYS_DEBOUNCE_INTERVAL,
                .gpio           = WIFI_PINEAPPLE_MK5_SD_01,
                .active_low     = 1,
        },

};

static void __init wifi_pineapple_mk5_setup(void)
{
        u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
        u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

                /* Disable hardware control LAN1 and LAN2 LEDs, enabling GPIO14 and GPIO15 */
        ath79_gpio_function_disable(AR933X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
                AR933X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
                AR933X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
                AR933X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

        ath79_register_leds_gpio(-1, ARRAY_SIZE(wifi_pineapple_mk5_leds_gpio),
                wifi_pineapple_mk5_leds_gpio);

        ath79_register_gpio_keys_polled(-1, WIFI_PINEAPPLE_MK5_KEYS_POLL_INTERVAL,
                ARRAY_SIZE(wifi_pineapple_mk5_gpio_keys),
                wifi_pineapple_mk5_gpio_keys);


        ath79_register_usb();

        ath79_register_m25p80(&wifi_pineapple_mk5_flash_data);

        ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);


        ath79_register_mdio(0, 0x0);
        ath79_register_eth(0);

        ath79_register_wmac(ee, mac);
}

MIPS_MACHINE(ATH79_MACH_WIFI_PINEAPPLE_MK5, "WIFI-PINEAPPLE-MK5",
        "Hak5 WiFi Pineapple MK5", wifi_pineapple_mk5_setup);

