$(KDIR)/root.tar.gz:
	tar -zcf $@ --owner=root --group=root -C $(KDIR)/root/ .

ifeq ($(BOARD),soekris)
install: tgz-install

tgz-install: $(KDIR)/root.tar.gz
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="tgz"
endif

prepare:
compile:
install: $(KDIR)/root.tar.gz

