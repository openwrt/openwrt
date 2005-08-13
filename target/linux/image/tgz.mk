$(KDIR)/root.tar.gz:
	tar -zcf $@ --owner=root --group=root -C $(KDIR)/root/ .

tgz-install: $(KDIR)/root.tar.gz
	$(MAKE) -C $(BOARD) install KERNEL="$(KERNEL)" FS="tgz"

prepare:
compile:
install: tgz-install
