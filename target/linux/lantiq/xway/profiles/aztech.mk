define Profile/GR7000
  NAME:=Aztech GR7000
  PACKAGES:= \
	kmod-ltq-deu-ar9 kmod-ltq-hcd-ar9 \
	swconfig
endef
$(eval $(call Profile,GR7000))
