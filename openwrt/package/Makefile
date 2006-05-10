# Main makefile for the packages
include $(TOPDIR)/rules.mk
COMPILE_PACKAGES:=$(patsubst %,%-compile,$(package-y) $(package-m))
INSTALL_PACKAGES:=$(patsubst %,%-install,$(package-y))

all: compile
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m))
compile: $(COMPILE_PACKAGES)
install: base-files-install $(INSTALL_PACKAGES)

$(STAMP_DIR) $(TARGET_DIR):
	mkdir -p $@

%-prepare: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-prepare,%,$@) prepare

%-compile: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-compile,%,$@) compile

%-clean: $(STAMP_DIR) $(TARGET_DIR)
	$(MAKE) -C $(patsubst %-clean,%,$@) clean

