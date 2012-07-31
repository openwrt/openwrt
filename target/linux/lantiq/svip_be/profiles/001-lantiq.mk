define Profile/EASY336
  NAME:=EASY336
  PACKAGES:=
endef

define Profile/EASY336/Description
	Lantiq EASY336 evalkit
endef

$(eval $(call Profile,EASY336))

define Profile/EASY33016
  NAME:=EASY33016
  PACKAGES:= \
	kmod-ifxos ifx-vos ifx-evtlog svip-switch \
	voice-lib-tapi voice-lib-cli voice-tapi-cli \
	voice-tapi voice-tapidemo-xt voice-vatests voice-vxt voice-xtbox
endef

define Profile/EASY33016/Description
	Lantiq EASY33016 evalkit
endef

$(eval $(call Profile,EASY33016))

