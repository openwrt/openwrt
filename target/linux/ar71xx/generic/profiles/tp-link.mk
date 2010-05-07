#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TLWR741NDV1
	NAME:=TP-LINK TL-WR741ND v1
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR741NDV1/Description
	Package set optimized for the TP-LINK TL-WR741ND v1.
endef

$(eval $(call Profile,TLWR741NDV1))

define Profile/TLWR841NV15
	NAME:=TP-LINK TL-WR841N v1.5
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR841NV15/Description
	Package set optimized for the TP-LINK TL-WR841N v1.5.
endef

$(eval $(call Profile,TLWR841NV15))

define Profile/TLWR841NDV3
	NAME:=TP-LINK TL-WR841ND v3
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR841NDV3/Description
	Package set optimized for the TP-LINK TL-WR841ND v3.
endef

$(eval $(call Profile,TLWR841NDV3))

define Profile/TLWR841NDV5
	NAME:=TP-LINK TL-WR841ND v5
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR841NDV5/Description
	Package set optimized for the TP-LINK TL-WR841ND v5.
endef

$(eval $(call Profile,TLWR841NDV5))

define Profile/TLWR941NDV2
	NAME:=TP-LINK TL-WR941ND v2
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR941NDV2/Description
	Package set optimized for the TP-LINK TL-WR941ND v2.
endef

$(eval $(call Profile,TLWR941NDV2))

define Profile/TLWR941NDV3
	NAME:=TP-LINK TL-WR941ND v3
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR941NDV3/Description
	Package set optimized for the TP-LINK TL-WR941ND v3.
endef

$(eval $(call Profile,TLWR941NDV3))

define Profile/TLWR941NDV4
	NAME:=TP-LINK TL-WR941ND v4
	PACKAGES:=kmod-ath9k wpad-mini
endef

define Profile/TLWR941NDV4/Description
	Package set optimized for the TP-LINK TL-WR941ND v4.
endef

$(eval $(call Profile,TLWR941NDV4))

define Profile/TLWR1043NDV1
	NAME:=TP-LINK TL-WR1043ND v1
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-core kmod-usb2
endef

define Profile/TLWR1043NDV1/Description
	Package set optimized for the TP-LINK TL-WR1043ND v1.
endef

$(eval $(call Profile,TLWR1043NDV1))
