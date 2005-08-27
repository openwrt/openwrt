include $(TOPDIR)/rules.mk

#############################################################
#
# uClibc (the C library)
#
#############################################################

ifeq ($(BR2_UCLIBC_VERSION_SNAPSHOT),y)
# Be aware that this changes daily....
UCLIBC_DIR:=$(TOOL_BUILD_DIR)/uClibc
UCLIBC_SOURCE:=uClibc-$(strip $(subst ",, $(BR2_USE_UCLIBC_SNAPSHOT))).tar.bz2
#"
UCLIBC_SITE:=http://www.uclibc.org/downloads/snapshots
UCLIBC_VER:=PKG_VERSION:=0.${shell date +"%G%m%d"}
else
UCLIBC_VER:=0.9.27
UCLIBC_DIR:=$(TOOL_BUILD_DIR)/uClibc-$(UCLIBC_VER)
UCLIBC_SOURCE:=uClibc-$(UCLIBC_VER).tar.bz2
UCLIBC_SITE:=http://www.uclibc.org/downloads
endif

UCLIBC_TARGET_ARCH:=$(shell echo $(ARCH) | sed -e s'/-.*//' \
		-e 's/i.86/i386/' \
		-e 's/sparc.*/sparc/' \
		-e 's/arm.*/arm/g' \
		-e 's/m68k.*/m68k/' \
		-e 's/ppc/powerpc/g' \
		-e 's/v850.*/v850/g' \
		-e 's/sh64/sh/' \
		-e 's/sh[234].*/sh/' \
		-e 's/mips.*/mips/' \
		-e 's/mipsel.*/mips/' \
		-e 's/cris.*/cris/' \
)


$(DL_DIR)/$(UCLIBC_SOURCE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(UCLIBC_SOURCE) x $(UCLIBC_SITE)

$(UCLIBC_DIR)/.unpacked: $(DL_DIR)/$(UCLIBC_SOURCE)
	mkdir -p $(TOOL_BUILD_DIR)
	bzcat $(DL_DIR)/$(UCLIBC_SOURCE) | tar -C $(TOOL_BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(UCLIBC_DIR) ./patches
	touch $(UCLIBC_DIR)/.unpacked

$(UCLIBC_DIR)/.configured: $(UCLIBC_DIR)/.unpacked
	$(SED) 's,^CROSS=.*,CROSS=$(TARGET_CROSS),g' $(UCLIBC_DIR)/Rules.mak
	cp ./files/config.$(UCLIBC_TARGET_ARCH) $(UCLIBC_DIR)/.config
	$(SED) 's,^KERNEL_SOURCE=.*,KERNEL_SOURCE=\"$(LINUX_HEADERS_DIR)\",g' \
		$(UCLIBC_DIR)/.config
ifeq ($(BR2_LARGEFILE),y)
	$(SED) 's,^.*UCLIBC_HAS_LFS.*,UCLIBC_HAS_LFS=y,g' $(UCLIBC_DIR)/.config
else
	$(SED) 's,^.*UCLIBC_HAS_LFS.*,UCLIBC_HAS_LFS=n,g' $(UCLIBC_DIR)/.config
endif
	$(SED) 's,.*UCLIBC_HAS_WCHAR.*,UCLIBC_HAS_WCHAR=y,g' $(UCLIBC_DIR)/.config
ifeq ($(BR2_SOFT_FLOAT),y)
	$(SED) 's,.*HAS_FPU.*,HAS_FPU=n\nUCLIBC_HAS_FLOATS=y\nUCLIBC_HAS_SOFT_FLOAT=y,g' $(UCLIBC_DIR)/.config
endif
	mkdir -p $(TOOL_BUILD_DIR)/uClibc_dev/usr/include
	mkdir -p $(TOOL_BUILD_DIR)/uClibc_dev/usr/lib
	mkdir -p $(TOOL_BUILD_DIR)/uClibc_dev/lib
	$(MAKE1) -C $(UCLIBC_DIR) \
		PREFIX=$(TOOL_BUILD_DIR)/uClibc_dev/ \
		DEVEL_PREFIX=/usr/ \
		RUNTIME_PREFIX=$(TOOL_BUILD_DIR)/uClibc_dev/ \
		HOSTCC="$(HOSTCC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)" \
		pregen install_dev;
	touch $(UCLIBC_DIR)/.configured

$(UCLIBC_DIR)/lib/libc.a: $(UCLIBC_DIR)/.configured $(LIBFLOAT_TARGET)
	$(MAKE1) -C $(UCLIBC_DIR) \
		PREFIX= \
		DEVEL_PREFIX=/ \
		RUNTIME_PREFIX=/ \
		HOSTCC="$(HOSTCC)" \
		CPU_CFLAGS="$(TARGET_CFLAGS)" \
		all
	touch -c $(UCLIBC_DIR)/lib/libc.a

$(STAGING_DIR)/lib/libc.a: $(UCLIBC_DIR)/lib/libc.a
	$(MAKE1) -C $(UCLIBC_DIR) \
		PREFIX=$(STAGING_DIR)/ \
		DEVEL_PREFIX=/ \
		RUNTIME_PREFIX=/ \
		install_runtime
	$(MAKE1) -C $(UCLIBC_DIR) \
		PREFIX=$(STAGING_DIR)/ \
		DEVEL_PREFIX=/ \
		RUNTIME_PREFIX=/ \
		install_dev
	echo $(UCLIBC_VER) > $(STAGING_DIR)/uclibc_version
	touch -c $(STAGING_DIR)/lib/libc.a

source: $(DL_DIR)/$(UCLIBC_SOURCE)
prepare: $(UCLIBC_DIR)/.configured
compile: $(UCLIBC_DIR)/lib/libc.a
install: $(STAGING_DIR)/lib/libc.a $(UCLIBC_TARGETS)
clean: 
	rm -rf $(UCLIBC_DIR) $(TOOL_BUILD_DIR)/uClibc_dev
