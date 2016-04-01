#
# MT7688 Profiles
#

Image/Build/Profile/LinkIt7688=$(call BuildFirmware/Default32M/$(1),$(1),LinkIt7688,LINKIT7688)
Image/Build/Profile/WRTNODE2R=$(call BuildFirmware/Default16M/$(1),$(1),wrtnode2r,WRTNODE2R)

define Image/Build/Profile/Default
	$(call Image/Build/Profile/LinkIt7688,$(1))
	$(call Image/Build/Profile/WRTNODE2R,$(1))
endef
