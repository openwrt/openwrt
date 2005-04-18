ifneq ($(strip $(PKG_SOURCE)),)
$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)
endif

ifneq ($(strip $(PKG_CAT)),)
$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	rm -rf $(PKG_BUILD_DIR)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	if [ -d ./patches ]; then \
		$(PATCH) $(PKG_BUILD_DIR) ./patches ; \
	fi
	touch $(PKG_BUILD_DIR)/.prepared
endif

all: compile

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(COMPILE_TARGETS)
install: $(INSTALL_TARGETS)

clean: $(CLEAN_TARGETS)
	rm -rf $(PKG_FILES) $(PKG_BUILD_DIR)

.PHONY: all source prepare compile install clean