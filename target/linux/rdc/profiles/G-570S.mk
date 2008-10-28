#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/g570s
  NAME:=ZyXEL G-570S
  PACKAGES:=kmod-madwifi kmod-r6040
endef
$(eval $(call Profile,g570s))

