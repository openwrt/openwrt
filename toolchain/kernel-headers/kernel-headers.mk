# linux kernel headers for toolchain

LINUX_HEADERS_VERSION=2.4.29
LINUX_HEADERS_SITE=http://www.kernel.org/pub/linux/kernel/v2.4
LINUX_HEADERS_SOURCE=linux-$(LINUX_HEADERS_VERSION).tar.bz2
LINUX_HEADERS_CONFIG=toolchain/kernel-headers/linux.config
LINUX_HEADERS_DIR=$(TOOL_BUILD_DIR)/linux-$(LINUX_HEADERS_VERSION)
LINUX_HEADERS_ARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	)

$(DL_DIR)/$(LINUX_HEADERS_SOURCE):
	-mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(LINUX_HEADERS_SITE)/$(LINUX_HEADERS_SOURCE)

$(LINUX_HEADERS_DIR)/.unpacked: $(DL_DIR)/$(LINUX_HEADERS_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	bzcat $(DL_DIR)/$(LINUX_HEADERS_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) - \
		linux-$(LINUX_HEADERS_VERSION)/include \
		linux-$(LINUX_HEADERS_VERSION)/Makefile \
		linux-$(LINUX_HEADERS_VERSION)/Rules.make \
		linux-$(LINUX_HEADERS_VERSION)/arch/$(LINUX_HEADERS_ARCH)/Makefile \
		linux-$(LINUX_HEADERS_VERSION)/scripts \
		linux-$(LINUX_HEADERS_VERSION)/arch/$(LINUX_HEADERS_ARCH)/config*.in \
		linux-$(LINUX_HEADERS_VERSION)/*/*/Config.in \
		linux-$(LINUX_HEADERS_VERSION)/*/Config.in
	touch $(LINUX_HEADERS_DIR)/.unpacked

$(LINUX_HEADERS_DIR)/.configured: $(LINUX_HEADERS_DIR)/.unpacked
	-cp -af $(LINUX_HEADERS_CONFIG) $(LINUX_HEADERS_DIR)/.config
	$(MAKE) -C $(LINUX_HEADERS_DIR) ARCH=$(LINUX_HEADERS_ARCH) oldconfig include/linux/version.h
	touch $(LINUX_HEADERS_DIR)/.configured

kernel-headers: $(LINUX_HEADERS_DIR)/.configured

kernel-headers-source: $(DL_DIR)/$(LINUX_HEADERS_SOURCE)

kernel-headers-clean: clean
	rm -rf $(LINUX_HEADERS_DIR)

kernel-headers-toolclean:
	rm -rf $(LINUX_HEADERS_DIR)
