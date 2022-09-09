include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/version.mk

override MAKEFLAGS=

TOOLS_NAME:=$(VERSION_DIST_SANITIZED)-tools.$(HOST_OS)-$(HOST_ARCH)
TOOLS_BUILD_DIR:=$(BUILD_DIR)/$(TOOLS_NAME)

all: compile

$(BIN_DIR)/$(TOOLS_NAME).tar.xz: clean
	mkdir -p $(TOOLS_BUILD_DIR)/build_dir/host
	mkdir -p $(TOOLS_BUILD_DIR)/staging_dir/host

	$(TAR) -cf - -C $(TOPDIR)/build_dir/host/ \
		. | \
		$(TAR) -xf - -C $(TOOLS_BUILD_DIR)/build_dir/host
	$(TAR) -cf - -C $(TOPDIR)/staging_dir/host/ \
		. | \
		$(TAR) -xf - -C $(TOOLS_BUILD_DIR)/staging_dir/host

	$(CP) \
		$(TOPDIR)/LICENSES \
		$(TOPDIR)/COPYING \
		./files/README.TOOLS \
		$(TOOLS_BUILD_DIR)/

	$(Q)-( \
		find \
			$(TOOLS_BUILD_DIR)/*/bin \
			$(TOOLS_BUILD_DIR)/*/*/bin \
			$(TOOLS_BUILD_DIR)/*/libexec \
			-type f; \
	) | xargs strip 2>/dev/null >/dev/null

	find $(TOOLS_BUILD_DIR) -name .git | $(XARGS) $(RM) -r
	find $(TOOLS_BUILD_DIR) -name .svn | $(XARGS) $(RM) -r
	find $(TOOLS_BUILD_DIR) -name CVS | $(XARGS) $(RM) -r
	mkdir -p $(BIN_DIR)
	(cd $(BUILD_DIR)/$(TOOLS_NAME); \
		tar -I '$(STAGING_DIR_HOST)/bin/xz -7e -T$(if $(filter 1,$(NPROC)),2,0)' -cf $@ . \
		--mtime="$(shell date --date=@$(SOURCE_DATE_EPOCH))"; \
	)

download:
prepare:
compile: $(BIN_DIR)/$(TOOLS_NAME).tar.xz
install: compile

clean:
	$(RM) -r $(TOOLS_BUILD_DIR) $(BIN_DIR)/$(TOOLS_NAME).tar.xz
