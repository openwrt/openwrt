/******************************************************************************
 *
 * Copyright(c) 2022 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include <drv_types.h>
#include <linux/rfkill-wlan.h>
extern int get_wifi_chip_type(void);
extern int rockchip_wifi_power(int on);
extern int rockchip_wifi_set_carddetect(int val);

void platform_wifi_get_oob_irq(int *oob_irq)
{

}

void platform_wifi_mac_addr(u8 *mac_addr)
{
	if (!rockchip_wifi_mac_addr(mac_addr))
		RTW_ERR("Get mac address from flash:"MAC_FMT"\n", MAC_ARG(mac_addr));
}

int platform_wifi_power_on(void)
{
	int ret = 0;

	RTW_PRINT("\n");
	RTW_PRINT("=======================================================\n");
	RTW_PRINT("==== Launching Wi-Fi driver! (Powered by Rockchip) ====\n");
	RTW_PRINT("=======================================================\n");
	RTW_PRINT("Realtek %s WiFi driver (Powered by Rockchip,Ver %s) init.\n", DRV_NAME, DRIVERVERSION);
	rockchip_wifi_power(1);
	return ret;
}

void platform_wifi_power_off(void)
{
	RTW_PRINT("\n");
	RTW_PRINT("=======================================================\n");
	RTW_PRINT("==== Dislaunching Wi-Fi driver! (Powered by Rockchip) ====\n");
	RTW_PRINT("=======================================================\n");
	RTW_PRINT("Realtek %s WiFi driver (Powered by Rockchip,Ver %s) init.\n", DRV_NAME, DRIVERVERSION);
	rockchip_wifi_power(0);
}
