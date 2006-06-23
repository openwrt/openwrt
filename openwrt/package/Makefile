# Main makefile for the packages
include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config
include $(TOPDIR)/.pkgdeps

SOURCE_PACKAGES:=$(patsubst %,%-source,$(package-y) $(package-m))
COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

$(STAMP_DIR) $(TARGET_DIR):
	mkdir -p $@

%-source: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-source,%,$@) source MAKEFLAGS="$(BUILD_MAKEFLAGS)"

%-prepare: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare MAKEFLAGS="$(BUILD_MAKEFLAGS)"

%-compile: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-compile,%,$@) compile MAKEFLAGS="$(BUILD_MAKEFLAGS)"

%-install: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-install,%,$@) install MAKEFLAGS="$(BUILD_MAKEFLAGS)"

%-clean: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-clean,%,$@) clean MAKEFLAGS="$(BUILD_MAKEFLAGS)"

ifeq ($(SDK),1)
GENDEP_OPTS := -s
endif

$(TOPDIR)/.pkgdeps: $(TOPDIR)/.pkginfo
	@$(TOPDIR)/scripts/gen_deps.pl $(GENDEP_OPTS) < $< > $@ || rm -f $@

all: compile
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
download: $(SOURCE_PACKAGES)
compile-targets: $(COMPILE_PACKAGES)
compile:
	$(MAKE) -j$(CONFIG_JLEVEL) compile-targets
install-targets: base-files-install $(INSTALL_PACKAGES)
install:
	rm -rf $(BUILD_DIR)/root
	$(MAKE) install-targets

