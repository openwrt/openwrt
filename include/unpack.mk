# 
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# unpacking files with +s may break on some platforms. this typically emits error code 2
ifneq ($(HOST_OS),Linux)
  HOST_TAR:=trapret 2 $(TAR)
else
  HOST_TAR:=$(TAR)
endif
TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS)
UNZIP_CMD:=unzip -d $(PKG_BUILD_DIR)/.. $(DL_DIR)/$(PKG_SOURCE)

ifeq ($(PKG_SOURCE),)
  PKG_UNPACK ?= true
endif
ifeq ($(strip $(PKG_UNPACK)),)
  ifeq ($(strip $(PKG_CAT)),)
    # try to autodetect file type
    EXT:=$(call ext,$(PKG_SOURCE))
    EXT1:=$(EXT)

    ifeq ($(filter gz tgz,$(EXT)),$(EXT))
      EXT:=$(call ext,$(PKG_SOURCE:.$(EXT)=))
      UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) |
    endif	
    ifeq ($(filter bzip2 bz2 bz tbz2 tbz,$(EXT)),$(EXT))
      EXT:=$(call ext,$(PKG_SOURCE:.$(EXT)=))
      UNPACK:=bzcat $(DL_DIR)/$(PKG_SOURCE) |
    endif
    ifeq ($(filter tgz tbz tbz2,$(EXT1)),$(EXT1))
      EXT:=tar
    endif
    UNPACK ?= cat $(DL_DIR)/$(PKG_SOURCE) |
    ifeq ($(EXT),tar)
      PKG_UNPACK:=$(UNPACK) $(TAR_CMD)
    endif
    ifeq ($(EXT),cpio)
      PKG_UNPACK:=$(UNPACK) (cd $(PKG_BUILD_DIR)/..; cpio -i -d)
    endif
    ifeq ($(EXT),zip)
      PKG_UNPACK:=$(UNZIP_CMD)
    endif
  endif
 
  # compatibility code for packages that set PKG_CAT
  ifeq ($(strip $(PKG_UNPACK)),)
    # use existing PKG_CAT
    PKG_UNPACK:=$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | $(TAR_CMD)
    ifeq ($(PKG_CAT),unzip)
      PKG_UNPACK:=$(UNZIP_CMD)
    endif
    # replace zcat with $(ZCAT), because some system don't support it properly
    ifeq ($(PKG_CAT),zcat)
      PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR_CMD)
    endif
  endif
  ifneq ($(strip $(CRLF_WORKAROUND)),)
    PKG_UNPACK += && find $(PKG_BUILD_DIR) -type f -print0 | xargs -0 perl -pi -e 's!\r$$$$!!g'
  endif
endif
