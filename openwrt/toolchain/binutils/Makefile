include $(TOPDIR)/rules.mk

include ./binutils.mk

source: binutils-source
prepare: $(BINUTILS_DIR)/.patched
compile: $(BINUTILS_DIR1)/binutils/objdump
install: binutils
clean: binutils-distclean binutils-toolclean
