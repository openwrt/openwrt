GCC_VARIANT:=initial

include ../common.mk

GCC_CONFIGURE += \
	--with-newlib \
	--with-sysroot=$(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev \
	--enable-languages=c \
	--disable-shared \
	--disable-threads \

define Host/Compile
	$(CP) $(BUILD_DIR_TOOLCHAIN)/linux-dev/* $(BUILD_DIR_TOOLCHAIN)/$(LIBC)-dev/
	$(GCC_MAKE) -C $(GCC_BUILD_DIR) \
		all-build-libiberty \
		all-gcc \
		$(if $(GCC_BUILD_TARGET_LIBGCC),all-target-libgcc)
endef

define Host/Install
	$(GCC_MAKE) -C $(GCC_BUILD_DIR) \
		install-gcc \
		$(if $(GCC_BUILD_TARGET_LIBGCC),install-target-libgcc)

	# XXX: glibc insists on linking against libgcc_eh
	( cd $(TOOLCHAIN_DIR)/usr/lib/gcc/$(REAL_GNU_TARGET_NAME)/$(PKG_VERSION) ; \
		[ -e libgcc_eh.a ] || ln -sf libgcc.a libgcc_eh.a ; \
		cp libgcc.a libgcc_initial.a; \
	)
endef

$(eval $(call HostBuild))
