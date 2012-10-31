PATH_PREFIX := .
VARIANT:=final
HOST_BUILD_PARALLEL:=1

include ./common.mk

define Host/Compile
	$(MAKE) -C $(CUR_BUILD_DIR) all PARALLELMFLAGS="$(HOST_JOBS)"
endef

define Host/Install
	$(call Host/SetToolchainInfo)
	$(MAKE) -C $(CUR_BUILD_DIR) \
		install_root="$(TOOLCHAIN_DIR)" \
		install
	( cd $(TOOLCHAIN_DIR) ; \
		for d in lib usr/lib ; do \
		  for f in libc.so libpthread.so libgcc_s.so ; do \
		    if [ -f $$$$d/$$$$f -a ! -L $$$$d/$$$$f ] ; then \
		      $(SED) 's,/usr/lib/,,g;s,/lib/,,g' $$$$d/$$$$f ; \
		    fi \
		  done \
		done \
	)
endef

$(eval $(call HostBuild))
