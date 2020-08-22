ramips: mt7621: add support for Xiaomi Mi Router 4 …

mir4 is the same as mir3g, except for the RAM(256Mib→128Mib) LED and gpio(MiNet button).

Specification
=============
Power: 12 VDC, 1.5 A
Connector type: barrel
CPU1: MediaTek MT7621A (880 MHz, 4 cores)
FLA1: 128 MiB (ESMT F59L1G81MA)
RAM1: 128 MiB (ESMT M15T1G1664A)
WI1 chip1: MediaTek MT7603EN
WI1 802dot11 protocols: bgn
WI1 MIMO config: 2x2:2
WI1 antenna connector: U.FL
WI2 chip1: MediaTek MT7612EN
WI2 802dot11 protocols: an+ac
WI2 MIMO config: 2x2:2
WI2 antenna connector: U.FL
ETH chip1: MediaTek MT7621A
Switch: MediaTek MT7621A

TTL Serial
[o] TX
[o] GND
[o] RX
[ ] VCC - Do not connect it

Flash instruction
=================
1.Create a simple http server using caddy V1.0
2.set ttl enable
To enable writing to the console, you must reset to factory settings
Then you see uboot boot, press the keyboard 4 button (enter uboot command line)
If it is not successful, repeat the above operation of restoring the factory settings.
After entering the uboot command line, type:

setenv uart_en 1
saveenv
boot

3.use shell in ttl
cd /tmp
wget http://"your computerip:80"/openwrt-ramips-mt7621-xiaomi_mir4-squashfs-kernel1.bin
wget http://"your computer ip:80"/openwrt-ramips-mt7621-xiaomi_mir4-squashfs-rootfs0.bin
mtd write openwrt-ramips-mt7621-xiaomi_mir4-squashfs-kernel1.bin kernel1
mtd write openwrt-ramips-mt7621-xiaomi_mir4-squashfs-rootfs0.bin rootfs0
nvram set flag_try_sys1_failed=1
nvram commit
reboot
4.login to the router http://192.168.1.1/

Fork Signed-off-by: Zhangshiru <2548379078@qq.com>