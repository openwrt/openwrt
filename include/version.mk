#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

PKG_CONFIG_DEPENDS += \
	CONFIG_VERSION_NUMBER \
	CONFIG_VERSION_NICK \
	CONFIG_VERSION_REPO \
	CONFIG_VERSION_DIST

VERSION_NUMBER:=$(call qstrip,$(CONFIG_VERSION_NUMBER))
VERSION_NUMBER:=$(if $(VERSION_NUMBER),$(VERSION_NUMBER),$(REVISION))

VERSION_CODE:=$(call qstrip,$(CONFIG_VERSION_NUMBER))
VERSION_CODE:=$(if $(VERSION_CODE),$(VERSION_CODE),Bleeding Edge)

VERSION_NICK:=$(call qstrip,$(CONFIG_VERSION_NICK))
VERSION_NICK:=$(if $(VERSION_NICK),$(VERSION_NICK),$(RELEASE))

VERSION_REPO:=$(call qstrip,$(CONFIG_VERSION_REPO))
VERSION_REPO:=$(if $(VERSION_REPO),$(VERSION_REPO),http://downloads.openwrt.org/snapshots/trunk/%T/packages)

VERSION_DIST:=$(call qstrip,$(CONFIG_VERSION_DIST))
VERSION_DIST:=$(if $(VERSION_DIST),$(VERSION_DIST),OpenWrt)

VERSION_SED:=$(SED) 's,%U,$(VERSION_REPO),g' \
	-e 's,%V,$(VERSION_NUMBER),g' \
	-e 's,%v,\L$(subst $(space),_,$(VERSION_NUMBER)),g' \
	-e 's,%C,$(VERSION_CODE),g' \
	-e 's,%c,\L$(subst $(space),_,$(VERSION_CODE)),g' \
	-e 's,%N,$(VERSION_NICK),g' \
	-e 's,%n,\L$(subst $(space),_,$(VERSION_NICK)),g' \
	-e 's,%D,$(VERSION_DIST),g' \
	-e 's,%d,\L$(subst $(space),_,$(VERSION_DIST)),g' \
	-e 's,%R,$(REVISION),g' \
	-e 's,%T,$(BOARD),g' \
	-e 's,%S,$(BOARD)$(if $(SUBTARGET),/$(SUBTARGET)),g' \
