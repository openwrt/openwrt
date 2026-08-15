# RTL8192ES driver composition

The package uses the maintained RTL8192E core and Linux/cfg80211 compatibility
code from `Mange/rtl8192eu-linux-driver` at commit
`da21d44a40690fe0df21f5603a972abd3cbb311d` (v5.6.4 family).

That tree is USB-only. The files under `src/` provide its missing SDIO glue and
the RTL8192ES HAL. They originate from `user0-07161/android_kernel_tolino_page2`
at commit `3c616e84d235bc2ee0f37d5746d0024464cf0257`, directory
`drivers/net/wireless/rtl8192es` (Realtek v5.3.5_27304.20180808), with the small
6.18 compatibility changes visible directly in the vendored files.

Both source families declare GPL version 2 licensing in their source headers.
The resulting module matches SDIO ID `024c:818b` and is named `8192es.ko`.

The OEM SDIO HAL conditionally compiles its MAC, baseband and RF
initialization behind `HAL_MAC_ENABLE`, `HAL_BB_ENABLE` and `HAL_RF_ENABLE`.
The original Android kernel build supplied those defines outside the driver
directory.  This OpenWrt package therefore defines all three explicitly;
without them the module can bind and create `wlan0`, but omits the hardware
initialization needed by `ndo_open`.

## ZTE MF283V factory data

The module is deliberately not registered in `/etc/modules.d`.  The
`mf283v-wlan-data` init service loads it only after validating device-specific
factory data:

* the WLAN MAC stored as NV item 4678 must occur exactly once in both `EFS2`
  and `EFSBAK`, and both copies must match;
* the `ztefile` UBI volume is mounted read-only and all eight files below
  `wlan0/` are checked for their exact length, hexadecimal encoding and valid
  value ranges;
* a conventional 512-byte textual EFUSE map is generated for the vendor
  driver's existing MAC, crystal-cap and thermal-meter paths;
* the exact 14-channel, two-RF-path power tables are supplied separately so
  they are not reduced to the driver's coarser channel groups.

The validated runtime files live below `/tmp/mf283v-wlan`.  If any partition,
redundant MAC copy or calibration file fails validation, the service removes
all generated files before loading the driver, which then falls back to its
built-in conservative defaults and a random local MAC.

`CONFIG_RTW_IOCTL_SET_COUNTRY` is enabled so cfg80211 user regulatory requests
also update the vendor driver's internal channel plan.  On this OpenWrt build,
cfg80211 and the installed regulatory database remain authoritative for the
per-channel transmit restrictions.  The vendor channel plan is intersected
with those restrictions and can only disable additional channels; it cannot
clear `NO_IR`, radar or power restrictions imposed by cfg80211.  This avoids
the driver's legacy custom regulatory domain (`99`) and permits AP operation
on channels allowed by the configured country, including channels 12 and 13
in ETSI countries.

The firmware exposes one persistent primary full-MAC netdev.  OpenWrt reuses
that netdev when switching between station and AP modes instead of creating
and deleting virtual interfaces.  Both the hostap ucode helper and the driver
protect the primary netdev from `NL80211_CMD_DEL_INTERFACE`; deleting it from
inside the cfg80211 callback would recursively enter the cfg80211 netdev
notifier while the wiphy mutex is held.  Reuse also omits the MAC attribute
from `NL80211_CMD_SET_INTERFACE`: the primary netdev already owns the factory
address.  The driver now publishes that factory address before registering the
wiphy, and the hostap address allocator excludes the persistent netdev being
reused instead of treating its address as occupied by a second interface.
