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

	find $(TOOLS_BUILD_DIR)/staging_dir '!' -path '*stamp*' -name '.*' | \
		$(XARGS) rm -rf
	find $(TOOLS_BUILD_DIR)/staging_dir -name '*.pc' | \
		$(XARGS) rm -rf

	$(CP) \
		./files/* \
		$(TOPDIR)/LICENSES \
		$(TOPDIR)/COPYING \
		$(TOOLS_BUILD_DIR)/

	@-( \
		find \
			$(TOOLS_BUILD_DIR)/*/bin \
			$(TOOLS_BUILD_DIR)/*/*/bin \
			-type f; \
	) | $(XARGS) strip 2>/dev/null >/dev/null

	mkdir -p $(BIN_DIR)
	(cd $(BUILD_DIR)/$(TOOLS_NAME); \
		$(TAR) -I '$(STAGING_DIR_HOST)/bin/xz -7e -T$(if $(filter 1,$(NPROC)),2,0)' -cf $@ . \
		--mtime="$(shell date --date=@$(SOURCE_DATE_EPOCH))"; \
	)

download:
prepare:
compile: $(BIN_DIR)/$(TOOLS_NAME).tar.xz
install: compile

clean:
	rm -rf $(TOOLS_BUILD_DIR) $(BIN_DIR)/$(TOOLS_NAME).tar.xz
