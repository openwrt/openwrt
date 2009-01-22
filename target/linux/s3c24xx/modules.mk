ifeq ($(CONFIG_TARGET_s3c24xx),y)
	SOUNDCORE_FILES = \
		$(LINUX_DIR)/sound/soundcore.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/snd.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/snd-page-alloc.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/snd-hwdep.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/seq/snd-seq-device.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/snd-timer.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/core/snd-pcm.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/soc/snd-soc-core.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-s3c24xx-i2s.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-s3c24xx.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-neo1973-gta02-wm8753.$(LINUX_KMOD_SUFFIX) \
		$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8753.$(LINUX_KMOD_SUFFIX)
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
