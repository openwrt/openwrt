# 
# Copyright (C) 2007-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

TMP_DIR ?= $(TOPDIR)/tmp
ifeq ($(if $(TARGET_BUILD),,$(DUMP)),)
  -include $(TMP_DIR)/.host.mk
endif

ifneq ($(__host_inc),1)
__host_inc:=1

export PATH:=$(TOPDIR)/staging_dir/host/bin:$(PATH)

try-run = $(shell set -e; \
	TMP_F="$(TMP_DIR)/try-run.$$$$.tmp"; \
	if ($(1)) >/dev/null 2>&1; then echo "$(2)"; else echo "$(3)"; fi; \
	rm -f "$$TMP_F"; \
)

host-cc-option = $(call try-run, \
	$(HOSTCC) $(HOST_CFLAGS) $(1) -c -xc /dev/null -o "$$TMP_F",$(1),$(2) \
)

.PRECIOUS: $(TMP_DIR)/.host.mk
$(TMP_DIR)/.host.mk: $(TOPDIR)/include/host.mk
	@mkdir -p $(TMP_DIR)
	@( \
		HOST_OS=`uname`; \
		case "$$HOST_OS" in \
			Linux) HOST_ARCH=`uname -m`;; \
			Darwin) HOST_ARCH=`uname -m`;; \
			*) HOST_ARCH=`uname -p`;; \
		esac; \
		GNU_HOST_NAME=`gcc -dumpmachine`; \
		[ -z "$$GNU_HOST_NAME" -o "$$HOST_OS" = "Darwin" ] && \
			GNU_HOST_NAME=`$(TOPDIR)/scripts/config.guess`; \
		echo "HOST_OS:=$$HOST_OS" > $@; \
		echo "HOST_ARCH:=$$HOST_ARCH" >> $@; \
		echo "GNU_HOST_NAME:=$$GNU_HOST_NAME" >> $@; \
		if gfind -L /dev/null || find -L /dev/null; then \
			echo "FIND_L=find -L \$$(1)" >> $@; \
		else \
			echo "FIND_L=find \$$(1) -follow" >> $@; \
		fi \
	) >/dev/null 2>/dev/null

endif
