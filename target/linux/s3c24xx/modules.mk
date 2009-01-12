ifeq ($(CONFIG_TARGET_s3c24xx),y)
	SOUNDCORE_FILES = \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-s3c24xx-i2s.ko \
		$(LINUX_DIR)/sound/soc/s3c24xx/snd-soc-neo1973-gta02-wm8753.ko \
		$(LINUX_DIR)/sound/soc/codecs/snd-soc-wm8753.ko
	SOUNDCORE_LOAD = \
		snd-soc-wm8753.ko \
		snd-soc-s3c24xx-i2s.ko \
		snd-soc-neo1973-gta02-wm8753
endif
