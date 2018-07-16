# Copyright (c) 2013 The Linux Foundation. All rights reserved.
# allow for local directory containing source to be used

LOCAL_SRC ?= $(TOPDIR)/package/qca/$(PKG_NAME)/src

ifeq (exists, $(shell [ -d $(LOCAL_SRC) ] && echo exists))
PKG_REV=$(shell cd $(LOCAL_SRC)/; git describe --long --always | sed 's/.*-g//g')
PKG_VERSION:=g$(PKG_REV)
PKG_SOURCE_URL:=
PKG_UNPACK=mkdir -p $(PKG_BUILD_DIR); $(CP) $(LOCAL_SRC)/* $(PKG_BUILD_DIR)/
endif
