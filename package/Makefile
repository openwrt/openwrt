# Main makefile for the packages
include $(TOPDIR)/rules.mk
include $(TOPDIR)/.config
include $(TOPDIR)/.pkgdeps

COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

$(STAMP_DIR) $(TARGET_DIR):
	mkdir -p $@

%-prepare: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare

%-compile: $(STAMP_DIR) $(TARGET_DIR)
	@echo "-> make $@"
	$(MAKE) -C $(patsubst %-compile,%,$@) compile

%-install: $(STAMP_DIR) $(TARGET_DIR)
	@echo "-> make $@"
	$(MAKE) -C $(patsubst %-install,%,$@) install

%-clean: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-clean,%,$@) clean


$(TOPDIR)/.pkgdeps: $(TOPDIR)/.pkginfo
	@$(TOPDIR)/scripts/gen_deps.pl < $< > $@ || rm -f $@

all: compile
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
compile: $(COMPILE_PACKAGES)
install: base-files-install $(INSTALL_PACKAGES)


