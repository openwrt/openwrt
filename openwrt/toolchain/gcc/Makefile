include $(TOPDIR)/rules.mk

include ./gcc-uclibc-3.x.mk

source: gcc-source
prepare: gcc_initial
compile: $(GCC_BUILD_DIR2)/.compiled
install: gcc-install
clean: gcc_initial-clean gcc-clean
