# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2016 LEDE project

define Profile/Default
  NAME:=Default Profile
  PACKAGES:=kmod-b43 wpad-basic-mbedtls
  PRIORITY:=1
endef

define Profile/Default/Description
  Package set compatible with most boards.
endef

$(eval $(call Profile,Default))
