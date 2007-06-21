obj-$(CONFIG_BCM43XX_MAC80211) += bcm43xx-mac80211.o

bcm43xx-mac80211-obj-$(CONFIG_BCM43XX_MAC80211_PCMCIA) += bcm43xx_pcmcia.o

bcm43xx-mac80211-obj-$(CONFIG_BCM43XX_MAC80211_DEBUG) += bcm43xx_debugfs.o

bcm43xx-mac80211-obj-$(CONFIG_BCM43XX_MAC80211_DMA) += bcm43xx_dma.o
bcm43xx-mac80211-obj-$(CONFIG_BCM43XX_MAC80211_PIO) += bcm43xx_pio.o

bcm43xx-mac80211-objs := bcm43xx_main.o \
			 bcm43xx_tables.o \
		         bcm43xx_phy.o \
		         bcm43xx_power.o \
		         bcm43xx_sysfs.o \
		         bcm43xx_leds.o \
		         bcm43xx_xmit.o \
		         bcm43xx_lo.o \
		         $(bcm43xx-mac80211-obj-y)
