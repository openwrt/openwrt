#############################################################
#
# System Linux kernel target
#
# This uses an existing linux kernel source tree on
# your build system, and makes no effort at compiling
# anything....
#
# You will probably want to change LINUX_SOURCE to
# point to wherever you installed you kernel.
#
#  -Erik
#
#############################################################
ifneq ($(filter $(TARGETS),system-linux),)

LINUX_SOURCE=/usr/src/linux
LINUX_DIR=$(BUILD_DIR)/linux
LINUX_KERNEL=$(BUILD_DIR)/buildroot-kernel
# Used by pcmcia-cs and others
LINUX_SOURCE_DIR=$(LINUX_SOURCE)

$(LINUX_DIR)/.configured:
	mkdir -p $(LINUX_DIR)/include
	(cd $(LINUX_DIR)/include; \
	for i in $(LINUX_SOURCE)/include/*; do ln -sf $$i ; done; \
	rm -f asm; \
	if [ "$(ARCH)" = "powerpc" ];then \
	    ln -fs asm-ppc asm; \
	elif [ "$(ARCH)" = "mips" ];then \
	    ln -fs asm-mips asm; \
	elif [ "$(ARCH)" = "mipsel" ];then \
	    ln -fs asm-mips asm; \
	elif [ "$(ARCH)" = "arm" ];then \
	    ln -fs asm-arm asm; \
	    (cd asm-arm; \
	    if [ ! -L proc ] ; then \
	    ln -fs proc-armv proc; \
	    ln -fs arch-ebsa285 arch; fi); \
	elif [ "$(ARCH)" = "cris" ];then \
	    ln -fs asm-cris asm; \
	else ln -fs asm-$(ARCH) asm; \
	fi)
	cp $(LINUX_SOURCE)/Makefile $(LINUX_DIR)/
	cp $(LINUX_SOURCE)/Rules.make $(LINUX_DIR)/
	touch $(LINUX_DIR)/.configured

$(LINUX_KERNEL): $(LINUX_DIR)/.configured

system-linux: $(LINUX_DIR)/.configured

system-linux-clean: clean
	rm -f $(LINUX_KERNEL)
	rm -rf $(LINUX_DIR)

system-linux-dirclean:
	rm -rf $(LINUX_DIR)

endif
