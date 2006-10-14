ifeq ($(strip $(PKG_UNPACK)),)
  ifneq ($(strip $(PKG_CAT)),)
    # use existing PKG_CAT
    UNPACK:=$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS) -
    ifeq ($(PKG_CAT),unzip)
      UNPACK=unzip -d $(PKG_BUILD_DIR) $(DL_DIR)/$(PKG_SOURCE)
    endif
    # replace zcat with $(ZCAT), because some system have it as gzcat
    ifeq ($(PKG_CAT),zcat)
      UNPACK:=$(ZCAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(PKG_BUILD_DIR)/.. $(TAR_OPTIONS) -
    endif
  else
    # try to autodetect file type
  endif
endif
