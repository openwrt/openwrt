GCC_VARIANT:=final

include ../common.mk

GCC_CONFIGURE += \
	--enable-languages=$(TARGET_LANGUAGES) \
	--enable-shared \
	--enable-threads \
	--with-slibdir=$(TOOLCHAIN_DIR)/lib

ifneq ($(CONFIG_GCC_VERSION_4_5),)
  GCC_CONFIGURE += \
	--enable-lto \
	--with-libelf=$(TOPDIR)/staging_dir/host
endif

ifneq ($(CONFIG_TLS_SUPPORT),)
  GCC_CONFIGURE += \
	--enable-tls
else
  GCC_CONFIGURE += \
	--disable-tls
endif

ifneq ($(CONFIG_SJLJ_EXCEPTIONS),)
  GCC_CONFIGURE += \
	--enable-sjlj-exceptions
endif

define Host/Configure
	mkdir -p $(GCC_BUILD_DIR) $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)
	# Important!  Required for limits.h to be fixed.
	rm -rf $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	ln -sf ../include $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/sys-include
	rm -rf $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	ln -sf ../lib $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/lib
	$(if $(CONFIG_mips64)$(CONFIG_mips64el)$(CONFIG_x86_64),ln -sf ../lib64 $(TOOLCHAIN_DIR)/$(REAL_GNU_TARGET_NAME)/lib64)
	(cd $(GCC_BUILD_DIR) && rm -f config.cache; \
		$(GCC_CONFIGURE) \
	);
endef

define Host/Compile
	$(GCC_MAKE) -C $(GCC_BUILD_DIR) all
endef

define SetupExtraArch
	for app in $(TOOLCHAIN_DIR)/bin/$(OPTIMIZE_FOR_CPU)*-{gcc,gcc-*,g++}; do \
		[ -e $$$$app ] || continue; \
		old_base=$$$$(basename $$$$app); \
		new_base=$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_NAME))-$$$${old_base##$(OPTIMIZE_FOR_CPU)-}; \
		sed -e "s/@CC_BASE@/$$$$old_base/" \
			-e 's/@EXTRA_ARCH_OPTS@/$(call qstrip,$(CONFIG_EXTRA_TARGET_ARCH_OPTS))/' \
			 ../files/alternate-arch-cc.in > \
			 $(TOOLCHAIN_DIR)/bin/$$$$new_base; \
		chmod a+x $(TOOLCHAIN_DIR)/bin/$$$$new_base; \
	done
endef

define Host/Install
	$(_SINGLE)$(GCC_MAKE) -C $(GCC_BUILD_DIR) install
	# Set up the symlinks to enable lying about target name.
	set -e; \
	(cd $(TOOLCHAIN_DIR); \
		ln -sf $(REAL_GNU_TARGET_NAME) $(GNU_TARGET_NAME); \
		cd bin; \
		for app in $(REAL_GNU_TARGET_NAME)-* ; do \
			ln -sf $$$${app} \
		   	$(GNU_TARGET_NAME)$$$${app##$(REAL_GNU_TARGET_NAME)}; \
		done; \
	);
	$(if $(CONFIG_EXTRA_TARGET_ARCH),$(call SetupExtraArch))
endef

$(eval $(call HostBuild))

