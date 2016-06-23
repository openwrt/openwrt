#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AP113
	NAME:=Atheros AP113 reference board
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/AP113/description
	Package set optimized for the Atheros AP113 reference board.
endef

$(eval $(call Profile,AP113))

define Profile/AP121
	NAME:=Atheros AP121 reference board
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/AP121/Description
	Package set optimized for the Atheros AP121 reference board.
endef

$(eval $(call Profile,AP121))

define Profile/AP121MINI
	NAME:=Atheros AP121-MINI reference board
	PACKAGES:=
endef

define Profile/AP121MINI/Description
	Package set optimized for the Atheros AP121-MINI reference board.
endef

$(eval $(call Profile,AP121MINI))

define Profile/AP132
	NAME:=Atheros AP132 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP132/Description
	Package set optimized for the Atheros AP132 reference board.
endef

$(eval $(call Profile,AP132))

define Profile/AP135
	NAME:=Atheros AP135 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP135/Description
	Package set optimized for the Atheros AP135 reference board.
endef

$(eval $(call Profile,AP135))

define Profile/AP136
	NAME:=Atheros AP136 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP136/Description
	Package set optimized for the Atheros AP136 reference board.
endef

$(eval $(call Profile,AP136))

define Profile/AP143
	NAME:=Qualcomm Atheros AP143 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP143/Description
	Package set optimized for the Qualcomm Atheros AP143 reference board.
endef

$(eval $(call Profile,AP143))

define Profile/AP147
	NAME:=Qualcomm Atheros AP147 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP147/Description
	Package set optimized for the Atheros AP147 reference board.
endef

$(eval $(call Profile,AP147))

define Profile/AP152
	NAME:=Qualcomm Atheros AP152 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/AP152/Description
	Package set optimized for the Qualcomm Atheros AP152 reference board.
endef

$(eval $(call Profile,AP152))

define Profile/AP81
	NAME:=Atheros AP81 reference board
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/AP81/Description
	Package set optimized for the Atheros AP81 reference board.
endef

$(eval $(call Profile,AP81))

define Profile/AP83
	NAME:=Atheros AP83 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 \
		  vsc7385-ucode-ap83 vsc7395-ucode-ap83
endef

define Profile/AP83/Description
	Package set optimized for the Atheros AP83 reference board.
endef

$(eval $(call Profile,AP83))

define Profile/AP96
	NAME:=Atheros AP96 reference board
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/AP96/Description
	Package set optimized for the Atheros AP96 reference board.
endef

$(eval $(call Profile,AP96))

define Profile/DB120
	NAME:=Atheros DB120 reference board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/DB120/Description
	Package set optimized for the Atheros DB120 reference board.
endef

$(eval $(call Profile,DB120))

define Profile/PB42
	NAME:=Atheros PB42 reference board
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/PB42/Description
	Package set optimized for the Atheros PB42 reference board.
endef

$(eval $(call Profile,PB42))

define Profile/PB44
	NAME:=Atheros PB44 reference board
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 \
		  vsc7385-ucode-pb44 vsc7395-ucode-pb44
endef

define Profile/PB44/Description
	Package set optimized for the Atheros PB44 reference board.
endef

$(eval $(call Profile,PB44))

define Profile/PB92
	NAME:=Atheros PB92 reference board
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/PB92/Description
	Package set optimized for the Atheros PB92 reference board.
endef

$(eval $(call Profile,PB92))
