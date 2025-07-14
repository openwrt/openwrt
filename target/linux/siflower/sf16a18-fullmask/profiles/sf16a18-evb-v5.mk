#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SF16A18-EVB-V5
 NAME:= SF16A18 EVB V5
 PACKAGES:=\
	iperf \
	samba36-server luci-app-samba vsftpd vsftpd-tls \
	libwebsockets ssst subcloud libcurl ndscan tc curl netdetect netdiscover\
	iwinfo luasql-sqlite3 luci-ssl luci-lib-json \
	block-mount fstools badblocks ntfs-3g \
	kmod-fs-vfat kmod-fs-ntfs kmod-fs-ext4 kmod-nls-base kmod-nls-utf8 kmod-nls-cp936  kmod-scsi-core \
	kmod-nls-cp437 kmod-nls-cp850 kmod-nls-iso8859-1 kmod-nls-iso8859-15 kmod-nls-cp950 \
	openssl-util p2p tcpdump rwnxtools wandetect luci-app-upnp miniupnpd kmod-sf-ts kmod-ipt-ipset ipset
endef

define Profile/SF16A18-EVB-V5/Description
 Support for siflower evb boards v1.0
endef

define Profile/SF16A18-EVB-V5/Config
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_CPU
select BUSYBOX_DEFAULT_FEATURE_TOP_DECIMALS
select BUSYBOX_DEFAULT_FEATURE_TOP_SMP_PROCESS
select BUSYBOX_DEFAULT_FEATURE_TOPMEM
select BUSYBOX_DEFAULT_FEATURE_USE_TERMIOS
select BUSYBOX_DEFAULT_CKSUM
select TARGET_ROOTFS_SQUASHFS
select LUCI_LANG_zh-cn
endef

$(eval $(call Profile,SF16A18-EVB-V5))
