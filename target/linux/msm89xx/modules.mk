# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/qcom-rproc
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Qualcomm remoteproc support
  DEPENDS:=@TARGET_msm89xx
  KCONFIG:=\
    CONFIG_QCOM_MDT_LOADER \
    CONFIG_QCOM_RPROC_COMMON \
    CONFIG_QCOM_PIL_INFO
  FILES:=\
    $(LINUX_DIR)/drivers/soc/qcom/mdt_loader.ko \
    $(LINUX_DIR)/drivers/remoteproc/qcom_common.ko \
    $(LINUX_DIR)/drivers/remoteproc/qcom_pil_info.ko
  AUTOLOAD:=$(call AutoProbe,mdt_loader qcom_common qcom_pil_info)
endef

define KernelPackage/qcom-rproc/description
Support for loading remoteprocs in some Qualcomm chipsets
endef

$(eval $(call KernelPackage,qcom-rproc))

define KernelPackage/qcom-rproc-wcnss
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Qualcomm WCNSS remoteproc support
  DEPENDS:=@TARGET_msm89xx +kmod-qcom-rproc
  KCONFIG:=\
    CONFIG_QCOM_WCNSS_PIL \
    CONFIG_QCOM_WCNSS_CTRL
  FILES:=\
    $(LINUX_DIR)/drivers/remoteproc/qcom_wcnss_pil.ko \
    $(LINUX_DIR)/drivers/soc/qcom/wcnss_ctrl.ko
  AUTOLOAD:=$(call AutoProbe,qcom_wcnss_pil wcnss_ctrl)
endef

define KernelPackage/qcom-rproc-wcnss/description
Firmware loading and control for the WCNSS remoteproc
endef

$(eval $(call KernelPackage,qcom-rproc-wcnss))

define KernelPackage/qcom-rproc-modem
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Qualcomm modem remoteproc support
  DEPENDS:=@TARGET_msm89xx +kmod-qcom-rproc
  KCONFIG:=\
    CONFIG_QCOM_Q6V5_COMMON \
    CONFIG_QCOM_Q6V5_MSS
  FILES:=\
    $(LINUX_DIR)/drivers/remoteproc/qcom_q6v5.ko \
    $(LINUX_DIR)/drivers/remoteproc/qcom_q6v5_mss.ko
  AUTOLOAD:=$(call AutoProbe,qcom_q6v5 qcom_q6v5_mss)
endef

define KernelPackage/qcom-rproc-modem/description
Firmware loading and control for the modem remoteproc.
endef

$(eval $(call KernelPackage,qcom-rproc-modem))
