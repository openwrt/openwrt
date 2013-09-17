define Profile/VG3503J
  NAME:=BT Openreach - ECI VDSL Modem
  PACKAGES:=kmod-ltq-vdsl-vr9-mei kmod-ltq-vdsl-vr9 \
       kmod-ltq-ptm-vr9 ltq-vdsl-app
endef

$(eval $(call Profile,VG3503J))
