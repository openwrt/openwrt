BOARDNAME := Devices which boot from SATA (NAS)
FEATURES += ext4 usb
DEFAULT_PACKAGES += badblocks block-mount e2fsprogs \
		    kmod-dm kmod-md-mod partx-utils

define Target/Description
	Build firmware images for APM82181 boards that boot from SATA.
	For NAS like the MyBook Live Series.
endef

$(eval $(call $(if $(CONFIG_TARGET_ROOTFS_EXT4FS),RequireCommand,Ignore),genext2fs, \
        Please install genext2fs. \
))
