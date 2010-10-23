ifeq ($(CONFIG_TARGET_s3c24xx_openmoko_gta02),y)
	SOUNDCORE_FILES = \
		$(LINUX_DIR)/sound/soundcore.ko \
		$(LINUX_DIR)/sound/core/snd.ko \
		$(LINUX_DIR)/sound/core/snd-page-alloc.ko \
		$(LINUX_DIR)/sound/core/snd-hwdep.ko \
		$(LINUX_DIR)/sound/core/seq/snd-seq-device.ko \
		$(LINUX_DIR)/sound/core/snd-timer.ko \
		$(LINUX_DIR)/sound/core/snd-pcm.ko \
		$(LINUX_DIR)/sound/soc/snd-soc-core.ko \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-s3c24xx-i2s.ko \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-s3c24xx.ko \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-neo1973-gta02-wm8753.ko \
		$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8753.ko
	SOUNDCORE_LOAD = \
		soundcore \
		snd \
		snd-page-alloc \
		snd-hwdep \
		snd-seq-device \
		snd-timer \
		snd-pcm \
		snd-soc-core \
		snd-soc-wm8753 \
		snd-soc-s3c24xx-i2s \
		snd-soc-s3c24xx \
		snd-soc-neo1973-gta02-wm8753
endif
