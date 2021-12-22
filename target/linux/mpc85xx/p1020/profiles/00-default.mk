# SPDX-License-Identifier: GPL-2.0-only

define Profile/Default
  NAME:=Default Profile
  PRIORITY:=1
  PACKAGES := uboot-envtools kmod-mtd-rw
endef

define Profile/Default/Description
	Default package set compatible with most P1020 boards.
endef

$(eval $(call Profile,Default))
