# SPDX-License-Identifier: GPL-2.0-only

define KernelPackage/drm-rockchip
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Rockchip DRM support
  DEPENDS:=@TARGET_rockchip +kmod-drm-kms-helper +kmod-drm-dma-helper \
	+kmod-drm-display-helper +kmod-drm-client-lib +kmod-drm-analogix-dp \
	+kmod-drm-dw-dp +kmod-drm-dw-hdmi-qp +kmod-drm-dw-mipi-dsi2
  KCONFIG:= \
	CONFIG_DRM_ROCKCHIP \
	CONFIG_DRM_FBDEV_EMULATION=y \
	CONFIG_DRM_FBDEV_OVERALLOC=100 \
	CONFIG_DRM_CLIENT_DEFAULT_FBDEV=y \
	CONFIG_ROCKCHIP_VOP=y \
	CONFIG_ROCKCHIP_VOP2=y \
	CONFIG_ROCKCHIP_ANALOGIX_DP=y \
	CONFIG_ROCKCHIP_CDN_DP=n \
	CONFIG_ROCKCHIP_DW_DP=y \
	CONFIG_ROCKCHIP_DW_HDMI=n \
	CONFIG_ROCKCHIP_DW_HDMI_QP=y \
	CONFIG_ROCKCHIP_DW_MIPI_DSI=n \
	CONFIG_ROCKCHIP_DW_MIPI_DSI2=y \
	CONFIG_ROCKCHIP_INNO_HDMI=n \
	CONFIG_ROCKCHIP_LVDS=n \
	CONFIG_ROCKCHIP_RGB=n \
	CONFIG_ROCKCHIP_RK3066_HDMI=n
  FILES:=$(LINUX_DIR)/drivers/gpu/drm/rockchip/rockchipdrm.ko
  AUTOLOAD:=$(call AutoProbe,rockchipdrm)
endef

define KernelPackage/drm-rockchip/description
  Direct Rendering Manager (DRM) support for the Rockchip VOP and VOP2
  display controllers with their eDP, DP, HDMI QP and MIPI DSI2 outputs.
endef

$(eval $(call KernelPackage,drm-rockchip))


define KernelPackage/drm-rocket
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Rockchip NPU accelerator support
  DEPENDS:=@TARGET_rockchip +kmod-drm-sched +kmod-drm-shmem-helper
  KCONFIG:= \
	CONFIG_DRM_ACCEL=y \
	CONFIG_DRM_ACCEL_AMDXDNA=n \
	CONFIG_DRM_ACCEL_HABANALABS=n \
	CONFIG_DRM_ACCEL_IVPU=n \
	CONFIG_DRM_ACCEL_QAIC=n \
	CONFIG_DRM_ACCEL_ROCKET
  FILES:=$(LINUX_DIR)/drivers/accel/rocket/rocket.ko
  AUTOLOAD:=$(call AutoProbe,rocket)
endef

define KernelPackage/drm-rocket/description
  Compute acceleration driver for the Rockchip RKNN neural processing units.
endef

$(eval $(call KernelPackage,drm-rocket))


define KernelPackage/phy-rockchip-samsung-hdptx
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Rockchip Samsung HDMI/eDP combo PHY
  DEPENDS:=@TARGET_rockchip
  KCONFIG:=CONFIG_PHY_ROCKCHIP_SAMSUNG_HDPTX
  FILES:=$(LINUX_DIR)/drivers/phy/rockchip/phy-rockchip-samsung-hdptx.ko
  AUTOLOAD:=$(call AutoProbe,phy-rockchip-samsung-hdptx)
endef

$(eval $(call KernelPackage,phy-rockchip-samsung-hdptx))


define KernelPackage/phy-rockchip-samsung-dcphy
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Rockchip Samsung MIPI DCPHY
  DEPENDS:=@TARGET_rockchip
  KCONFIG:=CONFIG_PHY_ROCKCHIP_SAMSUNG_DCPHY
  FILES:=$(LINUX_DIR)/drivers/phy/rockchip/phy-rockchip-samsung-dcphy.ko
  AUTOLOAD:=$(call AutoProbe,phy-rockchip-samsung-dcphy)
endef

$(eval $(call KernelPackage,phy-rockchip-samsung-dcphy))


define KernelPackage/phy-rockchip-inno-csidphy
  SUBMENU:=$(VIDEO_MENU)
  TITLE:=Rockchip Innosilicon MIPI CSI DPHY
  DEPENDS:=@TARGET_rockchip
  KCONFIG:=CONFIG_PHY_ROCKCHIP_INNO_CSIDPHY
  FILES:=$(LINUX_DIR)/drivers/phy/rockchip/phy-rockchip-inno-csidphy.ko
  AUTOLOAD:=$(call AutoProbe,phy-rockchip-inno-csidphy)
endef

$(eval $(call KernelPackage,phy-rockchip-inno-csidphy))


define KernelPackage/video-rockchip-vdec
  TITLE:=Rockchip video decoder support
  DEPENDS:=@TARGET_rockchip +kmod-video-mem2mem +kmod-video-dma-contig \
	+kmod-video-vmalloc +kmod-video-v4l2-h264 +kmod-video-v4l2-vp9
  KCONFIG:=CONFIG_VIDEO_ROCKCHIP_VDEC
  FILES:=$(LINUX_DIR)/drivers/media/platform/rockchip/rkvdec/rockchip-vdec.ko
  AUTOLOAD:=$(call AutoProbe,rockchip-vdec)
  $(call AddDepends/video)
endef

define KernelPackage/video-rockchip-vdec/description
  Stateless V4L2 decoder driver for the Rockchip RKVDEC H.264, HEVC and
  VP9 video decoders.
endef

$(eval $(call KernelPackage,video-rockchip-vdec))


define KernelPackage/video-rockchip-rga
  TITLE:=Rockchip RGA 2D graphics acceleration
  DEPENDS:=@TARGET_rockchip +kmod-video-mem2mem +kmod-video-dma-sg
  KCONFIG:=CONFIG_VIDEO_ROCKCHIP_RGA
  FILES:=$(LINUX_DIR)/drivers/media/platform/rockchip/rga/rockchip-rga.ko
  AUTOLOAD:=$(call AutoProbe,rockchip-rga)
  $(call AddDepends/video)
endef

$(eval $(call KernelPackage,video-rockchip-rga))


define KernelPackage/video-synopsys-hdmirx
  TITLE:=Synopsys DesignWare HDMI receiver support
  DEPENDS:=@TARGET_rockchip +kmod-video-dma-contig +kmod-cec-core
  KCONFIG:= \
	CONFIG_VIDEO_SYNOPSYS_HDMIRX \
	CONFIG_VIDEO_SYNOPSYS_HDMIRX_LOAD_DEFAULT_EDID=y
  FILES:=$(LINUX_DIR)/drivers/media/platform/synopsys/hdmirx/synopsys-hdmirx.ko
  AUTOLOAD:=$(call AutoProbe,synopsys-hdmirx)
  $(call AddDepends/video)
endef

$(eval $(call KernelPackage,video-synopsys-hdmirx))


define KernelPackage/sound-soc-rockchip-i2s
  TITLE:=Rockchip I2S support
  DEPENDS:=@TARGET_rockchip +kmod-sound-soc-core
  KCONFIG:=CONFIG_SND_SOC_ROCKCHIP_I2S
  FILES:=$(LINUX_DIR)/sound/soc/rockchip/snd-soc-rockchip-i2s.ko
  AUTOLOAD:=$(call AutoProbe,snd-soc-rockchip-i2s)
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-rockchip-i2s))


define KernelPackage/sound-soc-rockchip-i2s-tdm
  TITLE:=Rockchip I2S/TDM support
  DEPENDS:=@TARGET_rockchip +kmod-sound-soc-core
  KCONFIG:=CONFIG_SND_SOC_ROCKCHIP_I2S_TDM
  FILES:=$(LINUX_DIR)/sound/soc/rockchip/snd-soc-rockchip-i2s-tdm.ko
  AUTOLOAD:=$(call AutoProbe,snd-soc-rockchip-i2s-tdm)
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-rockchip-i2s-tdm))


define KernelPackage/sound-soc-rockchip-spdif
  TITLE:=Rockchip S/PDIF support
  DEPENDS:=@TARGET_rockchip +kmod-sound-soc-core
  KCONFIG:=CONFIG_SND_SOC_ROCKCHIP_SPDIF
  FILES:=$(LINUX_DIR)/sound/soc/rockchip/snd-soc-rockchip-spdif.ko
  AUTOLOAD:=$(call AutoProbe,snd-soc-rockchip-spdif)
  $(call AddDepends/sound)
endef

$(eval $(call KernelPackage,sound-soc-rockchip-spdif))


define KernelPackage/rockchip-dfi
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Rockchip DDR monitor (DFI) devfreq event driver
  DEPENDS:=@TARGET_rockchip
  KCONFIG:= \
	CONFIG_PM_DEVFREQ_EVENT=y \
	CONFIG_DEVFREQ_EVENT_EXYNOS_NOCP=n \
	CONFIG_DEVFREQ_EVENT_EXYNOS_PPMU=n \
	CONFIG_DEVFREQ_EVENT_ROCKCHIP_DFI
  FILES:=$(LINUX_DIR)/drivers/devfreq/event/rockchip-dfi.ko
  AUTOLOAD:=$(call AutoProbe,rockchip-dfi)
endef

$(eval $(call KernelPackage,rockchip-dfi))
