$(KDIR)/root.tar.gz:
	tar -zcf $@ --owner=root --group=root -C $(BUILD_DIR)/root/ .

ifeq ($(BOARD),x86)
install: tgz-install

tgz-install: $(KDIR)/root.tar.gz
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="tgz"
endif

prepare-targets:
compile-targets:
install-targets: $(KDIR)/root.tar.gz

