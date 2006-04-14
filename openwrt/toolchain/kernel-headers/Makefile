include $(TOPDIR)/rules.mk

# linux kernel headers for toolchain

LINUX_HEADERS_VERSION=2.4.32
LINUX_KERNEL_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c
LINUX_HEADERS_SITE= \
		http://www.de.kernel.org/pub/linux/kernel/v2.4 \
		http://www.fi.kernel.org/pub/linux/kernel/v2.4 \
		http://www.fr.kernel.org/pub/linux/kernel/v2.4 \
		http://www.kernel.org/pub/linux/kernel/v2.4 \

LINUX_HEADERS_SOURCE=linux-$(LINUX_HEADERS_VERSION).tar.bz2
LINUX_HEADERS_ARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipseb/mips/' \
	-e 's/mipsel/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
	)

$(DL_DIR)/$(LINUX_HEADERS_SOURCE):
	-mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_HEADERS_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_HEADERS_SITE)

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
	(cd $(LINUX_HEADERS_DIR)-$(LINUX_HEADERS_VERSION) ; patch -p1 < $(TOPDIR)/toolchain/kernel-headers/patches/include.patch)
	ln -sf $(LINUX_HEADERS_DIR)-$(LINUX_HEADERS_VERSION) $(LINUX_HEADERS_DIR)
	$(SED) 's/@expr length/@-expr length/' $(LINUX_HEADERS_DIR)/Makefile 
	touch $(LINUX_HEADERS_DIR)/.unpacked

$(LINUX_HEADERS_DIR)/.configured: $(LINUX_HEADERS_DIR)/.unpacked
	-$(CP) ./files/config.$(LINUX_HEADERS_ARCH) $(LINUX_HEADERS_DIR)/.config
	$(MAKE) -C $(LINUX_HEADERS_DIR) ARCH=$(LINUX_HEADERS_ARCH) oldconfig include/linux/version.h
	touch $(LINUX_HEADERS_DIR)/.configured

source: $(DL_DIR)/$(LINUX_HEADERS_SOURCE)
prepare: $(LINUX_HEADERS_DIR)/.configured
compile:
install:
clean:
	rm -rf $(LINUX_HEADERS_DIR)

