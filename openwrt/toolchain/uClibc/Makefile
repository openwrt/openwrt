include $(TOPDIR)/rules.mk

include ./uclibc.mk

source: uclibc-source
prepare: uclibc-configured
compile: $(UCLIBC_DIR)/lib/libc.a
install: uclibc
	$(STRIP) $(TARGET_DIR)/lib/*.so
clean: uclibc-clean
