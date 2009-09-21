#ifndef __GLAMO_MFD_H
#define __GLAMO_MFD_H

struct glamo_core;
struct glamo_spigpio_platform_data;
struct glamo_fb_platform_data;

struct glamo_mmc_platform_data {
	int	(*glamo_mmc_use_slow)(void);

    struct glamo_core *core;
};

struct glamo_platform_data {
    struct glamo_fb_platform_data      *fb_data;
    struct glamo_spigpio_platform_data *spigpio_data;
    struct glamo_mmc_platform_data     *mmc_data;
    int gpio_base;

    unsigned int osci_clock_rate;

    int (*glamo_irq_is_wired)(void);
    void (*glamo_external_reset)(int);
    void (*registered)(struct device *dev);
};

enum glamo_engine {
	GLAMO_ENGINE_CAPTURE = 0,
	GLAMO_ENGINE_ISP = 1,
	GLAMO_ENGINE_JPEG = 2,
	GLAMO_ENGINE_MPEG_ENC = 3,
	GLAMO_ENGINE_MPEG_DEC = 4,
	GLAMO_ENGINE_LCD = 5,
	GLAMO_ENGINE_CMDQ = 6,
	GLAMO_ENGINE_2D = 7,
	GLAMO_ENGINE_3D = 8,
	GLAMO_ENGINE_MMC = 9,
	GLAMO_ENGINE_MICROP0 = 10,
	GLAMO_ENGINE_RISC = 11,
	GLAMO_ENGINE_MICROP1_MPEG_ENC = 12,
	GLAMO_ENGINE_MICROP1_MPEG_DEC = 13,
#if 0
	GLAMO_ENGINE_H264_DEC = 14,
	GLAMO_ENGINE_RISC1 = 15,
	GLAMO_ENGINE_SPI = 16,
#endif
	__NUM_GLAMO_ENGINES
};


#endif
