define Build/yuncore-tftp-header-qca953x
	( \
		echo -n -e "YUNCOREsetenv bootcmd \"bootm 0x9f050000 || bootm 0x9fe80000\" && " \
			"setenv resetcmd \"erase 0x9f050000 +0xfa0000 && cp.b 0x800600c0 0x9f050000 0xfa0000\" && " \
			"setenv bootargs \"console=ttyS0,115200 rootfstype=squashfs,jffs2 noinitrd\" && " \
			"saveenv && erase 0x9f050000 +0xfa0000 && cp.b 0x800600c0 0x9f050000 0xfa0000" | \
		dd bs=192 count=1 conv=sync; \
		dd if=$@; \
	) > $@.new
	mv $@.new $@
endef
