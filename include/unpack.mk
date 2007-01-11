ifeq ($(strip $(PKG_UNPACK)),)
  ifneq ($(HOST_OS),Linux)
	HOST_TAR:=trapret 2 $(TAR)
  else
    HOST_TAR:=$(TAR)
  endif
  ifneq ($(strip $(PKG_CAT)),)
    # use existing PKG_CAT
    PKG_UNPACK:=$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | $(HOST_TAR) -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS)
    ifeq ($(PKG_CAT),unzip)
      PKG_UNPACK:=unzip -d $(PKG_BUILD_DIR)/.. $(DL_DIR)/$(PKG_SOURCE)
    endif
    # replace zcat with $(ZCAT), because some system have it as gzcat
    ifeq ($(PKG_CAT),zcat)
      PKG_UNPACK:=$(ZCAT) $(DL_DIR)/$(PKG_SOURCE) | $(HOST_TAR) -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS)
    endif
  else
    # try to autodetect file type
  endif
endif
