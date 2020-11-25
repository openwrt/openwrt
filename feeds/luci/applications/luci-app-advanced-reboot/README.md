# Advanced Reboot Web UI (luci-app-advanced-reboot)

## Description

This package allows you to reboot to an alternative partition on the supported (dual-partition) routers and to power off (power down) your OpenWrt device.

## Supported Devices

Currently supported dual-partition devices include:

- Linksys EA3500
- Linksys E4200v2
- Linksys EA4500
- Linksys EA6350v3
- Linksys EA8300
- Linksys EA8500
- Linksys WRT1200AC
- Linksys WRT1900AC
- Linksys WRT1900ACv2
- Linksys WRT1900ACS
- Linksys WRT3200ACM
- Linksys WRT32X
- ZyXEL NBG6817

If your device is not in the list above, however it is a [dual-firmware device](https://openwrt.org/tag/dual_firmware?do=showtag&tag=dual_firmware) and you're interested in having your device supported, please post in [OpenWrt Forum Support Thread](https://forum.openwrt.org/t/web-ui-to-reboot-to-another-partition-dual-partition-routers/3423).

## Screenshot (luci-app-advanced-reboot)

![screenshot](https://raw.githubusercontent.com/stangri/openwrt_packages/master/screenshots/luci-app-advanced-reboot/screenshot02.png "screenshot")

## How to install

Install ```luci-app-advanced-reboot``` from Web UI or connect to your router via ssh and run the following commands:

```sh
opkg update
opkg install luci-app-advanced-reboot
```

If the ```luci-app-advanced-reboot``` package is not found in the official feed/repo for your version of OpenWrt/LEDE Project, you will need to [add a custom repo to your router](https://github.com/stangri/openwrt_packages/blob/master/README.md#on-your-router) first.

## Notes/Known Issues

- When you reboot to a different partition, your current settings (WiFi SSID/password, etc.) will not apply to a different partition. Different partitions might have completely different settings and even firmware.
- If you reboot to a partition which doesn't allow you to switch boot partitions (like stock vendor firmware), you might not be able to boot back to OpenWrt unless you reflash it, losing all the settings.
- Some devices allow you to trigger reboot to an alternative partition by interrupting boot 3 times in a row (by resetting/switching off the device or pulling power). As these methods might be different for different devices, do your own homework.
- Newer versions of this package try to mount alternative partition on compatible NAND routers in order to retrieve detailed firmware information. When that happens, it is normal to have messages similar to the below in the system log:

  ```sh
  Tue Nov 19 15:45:03 2019 user.notice luci-app-advanced-reboot: attempting to mount alternative   partition (mtd6)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.673826] ubi2: attaching mtd6
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.876698] ubi2: scanning is finished
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.885267] ubi2: attached mtd6 (name "rootfs1", size   74 MiB)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.891063] ubi2: PEB size: 131072 bytes (128 KiB),   LEB size: 126976 bytes
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.898011] ubi2: min./max. I/O unit sizes: 2048/2048,   sub-page size 2048
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.904878] ubi2: VID header offset: 2048 (aligned   2048), data offset: 4096
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.911928] ubi2: good PEBs: 592, bad PEBs: 0,   corrupted PEBs: 0
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.917962] ubi2: user volume: 2, internal volumes: 1,   max. volumes count: 128
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.925252] ubi2: max/mean erase counter: 48/32, WL   threshold: 4096, image sequence number: 1659081076
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.934623] ubi2: available PEBs: 0, total reserved   PEBs: 592, PEBs reserved for bad PEB handling: 40
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.944346] ubi2: background thread "ubi_bgt2d"   started, PID 26780
  Tue Nov 19 15:45:03 2019 kern.info kernel: [30392.952596] block ubiblock2_0: created from ubi2:0  (rootfs)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30392.964083] UBIFS (ubi2:1): background thread   "ubifs_bgt2_1" started, PID 26787
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.009298] UBIFS (ubi2:1): UBIFS: mounted UBI device   2, volume 1, name "rootfs_data"
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.017185] UBIFS (ubi2:1): LEB size: 126976 bytes   (124 KiB), min./max. I/O unit sizes: 2048 bytes/2048 bytes
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.027213] UBIFS (ubi2:1): FS size: 61075456 bytes   (58 MiB, 481 LEBs), journal size 3047424 bytes (2 MiB, 24 LEBs)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.037733] UBIFS (ubi2:1): reserved for root: 2884744   bytes (2817 KiB)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.044389] UBIFS (ubi2:1): media format: w4/r0   (latest is w5/r0), UUID 76F0C52C-6197-4E00-B306-747262B06545, small LPT model
  Tue Nov 19 15:45:03 2019 user.notice luci-app-advanced-reboot: attempting to unmount alternative   partition (mtd6)
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.132743] UBIFS (ubi2:1): un-mount UBI device 2
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.137481] UBIFS (ubi2:1): background thread   "ubifs_bgt2_1" stops
  Tue Nov 19 15:45:03 2019 kern.info kernel: [30393.390961] block ubiblock2_0: released
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.396576] ubi2: detaching mtd6
  Tue Nov 19 15:45:03 2019 kern.notice kernel: [30393.400117] ubi2: mtd6 is detached
  ```

## Thanks

I'd like to thank everyone who helped create, test and troubleshoot this package. Without help from [@hnyman](https://github.com/hnyman), [@jpstyves](https://github.com/jpstyves), [@imi2003](https://github.com/imi2003), [@jeffsf](https://github.com/jeffsf) and many contributions from [@slh](https://github.com/pkgadd) it wouldn't have been possible.
