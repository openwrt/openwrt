define Profile/Default
	NAME:=Default Profile
	PRIORITY:=1
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef

DEFAULT_PACKAGES+= \
	rcw-layerscape-ls1043ardb uboot-layerscape-$(SUBTARGET)-ls1043ardb \
	fman-layerscape-ls1043ardb

$(eval $(call Profile,Default))
