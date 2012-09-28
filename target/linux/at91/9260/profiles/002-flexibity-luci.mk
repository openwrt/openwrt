#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/flexibity-luci
  NAME:=Flexibity Connect (LuCI)
  PACKAGES:=bridge block-mount modprobe dnsmasq hotplug2 udev luci-flexibity  \
	syslog-ng sudo transmission-web miniupnpd nmap-ssl ip ntpclient ntpdate chat crda e169-stats genl hostapd-utils \
	iw madwimax netcat portmap ppp ppp-mod-pppoe pptp tcpdump usb-modem usb-modem-huawei-e175x usb-modem-huawei-eg162 \
	usb-modem-nokia-5800 wpa-cli wpa-supplicant motion badblocks blkid cifsmount disktype dosfsck dosfslabel e2fsprogs \
	fuse-utils mkdosfs nfs-utils ntfs-3g ntfs-3g-utils reiserfsprogs resize2fs sysfsutils tune2fs uuidgen certtool \
	gnutls-utils picocom setterm unrar unzip sqlite3-cli alsa-utils anyremote bluez-utils bzip2 comgt crypto-tools \
	file flock gdbserver gnupg gpioctl gsm-utils gzip huaweiaktbbo hwclock i2c-tools input-utils ldd lsof mdadm \
	module-init-tools mount-utils openssl-util procps psmisc px5g screen strace stress sysstat uboot-envtools \
	usb-modeswitch usb-modeswitch-data usbutils lua
endef

define Profile/flexibity-luci/Description
	Complete packages set for the Flexibity Connect device with LuCI.
endef

$(eval $(call Profile,flexibity-luci))

