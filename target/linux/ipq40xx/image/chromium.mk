define Build/cros-gpt
	cp $@ $@.tmp 2>/dev/null || true
	ptgen -o $@.tmp -g \
		-T cros_kernel	-N kernel -p $(CONFIG_TARGET_KERNEL_PARTSIZE)m \
				-N rootfs -p $(CONFIG_TARGET_ROOTFS_PARTSIZE)m
	cat $@.tmp >> $@
	rm $@.tmp
endef

define Build/append-kernel-part
	dd if=$(IMAGE_KERNEL) bs=$(CONFIG_TARGET_KERNEL_PARTSIZE)M conv=sync >> $@
endef

# NB: Chrome OS bootloaders replace the '%U' in command lines with the UUID of
# the kernel partition it chooses to boot from. This gives a flexible way to
# consistently build and sign kernels that always use the subsequent
# (PARTNROFF=1) partition as their rootfs.
define Build/cros-vboot
	$(STAGING_DIR_HOST)/bin/cros-vbutil \
		-k $@ -c "root=PARTUUID=%U/PARTNROFF=1" -o $@.new
	@mv $@.new $@
endef
