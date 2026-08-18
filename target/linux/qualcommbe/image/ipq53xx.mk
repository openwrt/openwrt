define Build/fit-inline-rootfs
	rm -f $@.dtb $@.kernel
	cp $@ $@.kernel
	cp $(word 2,$(1)) $@.dtb
	cp $@.kernel $@
	$(call Build/fit-its,$(word 1,$(1)) $@.dtb with-rootfs)
	$(call Build/fit-image,$(word 1,$(1)) $@.dtb with-rootfs)
	rootfs_offset="$$(grep -oba hsqs $@ | head -n1 | cut -d: -f1)"; \
	[ -n "$$rootfs_offset" ] || { echo "Failed to locate SquashFS in $@"; exit 1; }; \
	pad="$$(( (4096 - ($$rootfs_offset % 4096)) % 4096 ))"; \
	cp $(word 2,$(1)) $@.dtb; \
	dd if=/dev/zero bs=1 count="$$pad" >> $@.dtb 2>/dev/null; \
	cp $@.kernel $@; \
	$(call Build/fit-its,$(word 1,$(1)) $@.dtb with-rootfs)
	$(call Build/fit-image,$(word 1,$(1)) $@.dtb with-rootfs)
	rootfs_offset="$$(grep -oba hsqs $@ | head -n1 | cut -d: -f1)"; \
	[ "$$(( $$rootfs_offset % 4096 ))" -eq 0 ] || { echo "SquashFS is misaligned in $@"; exit 1; }; \
	rm -f $@.dtb $@.kernel
endef
