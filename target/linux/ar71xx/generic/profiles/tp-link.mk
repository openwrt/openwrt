#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TLMR3020
	NAME:=TP-LINK TL-MR3020
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/TLMR3020/Description
	Package set optimized for the TP-LINK TL-MR3020.
endef
$(eval $(call Profile,TLMR3020))


define Profile/TLMR3220
	NAME:=TP-LINK TL-MR3220
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/TLMR3220/Description
	Package set optimized for the TP-LINK TL-MR3220.
endef
$(eval $(call Profile,TLMR3220))


define Profile/TLMR3420
	NAME:=TP-LINK TL-MR3420
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/TLMR3420/Description
	Package set optimized for the TP-LINK TL-MR3420.
endef
$(eval $(call Profile,TLMR3420))


define Profile/TLWR703
	NAME:=TP-LINK TL-WR703N
	PACKAGES:=kmod-usb-core kmod-usb2
endef


define Profile/TLWR703/Description
	Package set optimized for the TP-LINK TL-WR703N.
endef
$(eval $(call Profile,TLWR703))


define Profile/TLWA701
	NAME:=TP-LINK TL-WA701N/ND
	PACKAGES:=
endef

define Profile/TLWA701/Description
	Package set optimized for the TP-LINK TL-WA701N/ND.
endef
$(eval $(call Profile,TLWA701))


define Profile/TLWA901
	NAME:=TP-LINK TL-WA901N/ND
	PACKAGES:=
endef

define Profile/TLWA901/Description
	Package set optimized for the TP-LINK TL-WA901N/ND.
endef
$(eval $(call Profile,TLWA901))


define Profile/TLWR740
	NAME:=TP-LINK TL-WR740N/ND
	PACKAGES:=
endef

define Profile/TLWR740/Description
	Package set optimized for the TP-LINK TL-WR740N/ND.
endef
$(eval $(call Profile,TLWR740))


define Profile/TLWR741
	NAME:=TP-LINK TL-WR741N/ND
	PACKAGES:=
endef

define Profile/TLWR741/Description
	Package set optimized for the TP-LINK TL-WR741N/ND.
endef
$(eval $(call Profile,TLWR741))


define Profile/TLWR743
	NAME:=TP-LINK TL-WR743N/ND
	PACKAGES:=
endef

define Profile/TLWR743/Description
	Package set optimized for the TP-LINK TL-WR743N/ND.
endef
$(eval $(call Profile,TLWR743))


define Profile/TLWR841
	NAME:=TP-LINK TL-WR841N/ND
	PACKAGES:=
endef

define Profile/TLWR841/Description
	Package set optimized for the TP-LINK TL-WR841N/ND.
endef
$(eval $(call Profile,TLWR841))


define Profile/TLWR941
	NAME:=TP-LINK TL-WR941N/ND
	PACKAGES:=
endef

define Profile/TLWR941/Description
	Package set optimized for the TP-LINK TL-WR941N/ND.
endef
$(eval $(call Profile,TLWR941))


define Profile/TLWR1043
	NAME:=TP-LINK TL-WR1043N/ND
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/TLWR1043/Description
	Package set optimized for the TP-LINK TL-WR1043N/ND.
endef
$(eval $(call Profile,TLWR1043))


define Profile/TLWR2543
	NAME:=TP-LINK TL-WR2543N/ND
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/TLWR2543/Description
	Package set optimized for the TP-LINK TL-WR2543N/ND.
endef
$(eval $(call Profile,TLWR2543))
