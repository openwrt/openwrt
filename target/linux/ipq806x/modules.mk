define KernelPackage/phy-qcom-ipq806x-usb
  TITLE:=Qualcomm IPQ806x DWC3 USB PHY driver
  DEPENDS:=@TARGET_ipq806x
  KCONFIG:= CONFIG_PHY_QCOM_IPQ806X_USB
  FILES:= \
    $(LINUX_DIR)/drivers/phy/qualcomm/phy-qcom-ipq806x-usb.ko
  AUTOLOAD:=$(call AutoLoad,45,phy-qcom-ipq806x-usb,1)
  $(call AddDepends/usb)
endef

define KernelPackage/phy-qcom-ipq806x-usb/description
 This driver provides support for the integrated DesignWare
 USB3 IP Core within the QCOM SoCs.
endef

$(eval $(call KernelPackage,phy-qcom-ipq806x-usb))


define KernelPackage/sound-soc-ipq8064-storm
  TITLE:=Qualcomm IPQ8064 SoC support for Google Storm
  DEPENDS:=@TARGET_ipq806x +kmod-sound-soc-core
  KCONFIG:=\
	CONFIG_IPQ_LCC_806X \
	CONFIG_SND_SOC_QCOM \
	CONFIG_SND_SOC_STORM \
	CONFIG_SND_SOC_APQ8016_SBC=n \
	CONFIG_SND_SOC_SC7180=n
  FILES:=\
	$(LINUX_DIR)/drivers/clk/qcom/lcc-ipq806x.ko \
	$(LINUX_DIR)/sound/soc/codecs/snd-soc-max98357a.ko \
	$(LINUX_DIR)/sound/soc/qcom/snd-soc-lpass-cpu.ko \
	$(LINUX_DIR)/sound/soc/qcom/snd-soc-lpass-ipq806x.ko \
	$(LINUX_DIR)/sound/soc/qcom/snd-soc-lpass-platform.ko \
	$(LINUX_DIR)/sound/soc/qcom/snd-soc-storm.ko
  AUTOLOAD:=$(call AutoProbe,lcc-ipq806x \
	snd-soc-max98357a snd-soc-lpass-ipq806x snd-soc-storm)
  $(call AddDepends/sound)
endef

define KernelPackage/sound-soc-ipq8064-storm/description
 Provides sound support for the Google Storm platform, with a Qualcomm IPQ8064
 SoC.
endef

$(eval $(call KernelPackage,sound-soc-ipq8064-storm))
