/*
 * RT2X00 Compatability fixes for specific kernels.
 */
#ifndef RT2X00_COMPAT_H
#define RT2X00_COMPAT_H

/*
 * First include the 2 config headers.
 * The rt2x00_config.h should overrule
 * the kernel configuration.
 */
#include <linux/autoconf.h>
#include "rt2x00_config.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>

/*
 * Check minimal requirements.
 */
#if (!defined(CONFIG_MAC80211) && !defined(CONFIG_MAC80211_MODULE))
#error mac80211 support not enabled in kernel!
#endif

#if !defined(CONFIG_WLAN_80211)
#error 802.11 wlan card support not enabled in kernel!
#endif

#if (defined(CONFIG_RT2400PCI) || defined(CONFIG_RT2500PCI) || defined(CONFIG_RT61PCI))
#if (!defined(CONFIG_PCI) && !defined(CONFIG_PCI_MODULE))
#error PCI has been disabled in your kernel!
#endif
#if (!defined(CONFIG_EEPROM_93CX6) && !defined(CONFIG_EEPROM_93CX6_MODULE))
#error EEPROM_93CX6 has been disabled in your kernel!
#endif
#endif

#if (defined(CONFIG_RT2500USB) || defined(CONFIG_RT73USB))
#if (!defined(CONFIG_USB) && !defined(CONFIG_USB_MODULE))
#warning USB has been disabled in your kernel!
#endif
#endif

#if (defined(CONFIG_RT61PCI) || defined(CONFIG_RT73USB))
#if (!defined(CONFIG_FW_LOADER) && !defined(CONFIG_FW_LOADER_MODULE))
#error Firmware loading has been disabled in your kernel!
#endif
#if (!defined(CONFIG_CRC_ITU_T) && !defined(CONFIG_CRC_ITU_T_MODULE))
#error CRC_ITU_T loading has been disabled in your kernel!
#endif
#endif

#if (defined(CONFIG_RT2X00_DEBUGFS))
#if (!defined(CONFIG_MAC80211_DEBUGFS) && !defined(CONFIG_MAC80211_DEBUGFS_MODULE))
#error mac80211 debugfs support has been disabled in your kernel!
#endif
#endif

#if (defined(CONFIG_RT2400PCI_BUTTON) || defined(CONFIG_RT2500PCI_BUTTON) || defined(CONFIG_RT61PCI_BUTTON))
#if (!defined(CONFIG_RFKILL) && !defined (CONFIG_RFKILL_MODULE))
#error RFKILL has been disabled in your kernel!
#endif
#endif

#endif /* RT2X00_COMPAT_H */
