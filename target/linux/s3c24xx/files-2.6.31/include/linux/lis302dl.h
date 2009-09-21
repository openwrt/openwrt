#ifndef _LINUX_LIS302DL_H
#define _LINUX_LIS302DL_H

#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/input.h>
#include <linux/workqueue.h>

struct lis302dl_info;

struct lis302dl_platform_data {
	char *name;
	unsigned long pin_chip_select;
	unsigned long pin_clk;
	unsigned long pin_mosi;
	unsigned long pin_miso;
	int open_drain;
	int interrupt;
	void (*lis302dl_suspend_io)(struct lis302dl_info *, int resuming);
};

struct lis302dl_info {
	struct lis302dl_platform_data *pdata;
	struct device *dev;
	struct input_dev *input_dev;
	unsigned int flags;
	unsigned int threshold;
	unsigned int duration;
	uint32_t overruns;
	struct {
		unsigned int threshold; /* mg */
		unsigned int duration;  /* ms */
	} wakeup;

	struct spi_device *spi;
	uint8_t regs[0x40];
};

enum lis302dl_reg {
	LIS302DL_REG_WHO_AM_I		= 0x0f,
	LIS302DL_REG_CTRL1		= 0x20,
	LIS302DL_REG_CTRL2		= 0x21,
	LIS302DL_REG_CTRL3		= 0x22,
	LIS302DL_REG_HP_FILTER_RESET	= 0x23,
	LIS302DL_REG_STATUS		= 0x27,
	LIS302DL_REG_OUT_X		= 0x29,
	LIS302DL_REG_OUT_Y		= 0x2b,
	LIS302DL_REG_OUT_Z		= 0x2d,
	LIS302DL_REG_FF_WU_CFG_1	= 0x30,
	LIS302DL_REG_FF_WU_SRC_1	= 0x31,
	LIS302DL_REG_FF_WU_THS_1	= 0x32,
	LIS302DL_REG_FF_WU_DURATION_1	= 0x33,
	LIS302DL_REG_FF_WU_CFG_2	= 0x34,
	LIS302DL_REG_FF_WU_SRC_2	= 0x35,
	LIS302DL_REG_FF_WU_THS_2	= 0x36,
	LIS302DL_REG_FF_WU_DURATION_2	= 0x37,
	LIS302DL_REG_CLICK_CFG		= 0x38,
	LIS302DL_REG_CLICK_SRC		= 0x39,
	LIS302DL_REG_CLICK_THSY_X	= 0x3b,
	LIS302DL_REG_CLICK_THSZ		= 0x3c,
	LIS302DL_REG_CLICK_TIME_LIMIT	= 0x3d,
	LIS302DL_REG_CLICK_LATENCY	= 0x3e,
	LIS302DL_REG_CLICK_WINDOW	= 0x3f,
};

enum lis302dl_reg_ctrl1 {
	LIS302DL_CTRL1_Xen		= 0x01,
	LIS302DL_CTRL1_Yen		= 0x02,
	LIS302DL_CTRL1_Zen		= 0x04,
	LIS302DL_CTRL1_STM		= 0x08,
	LIS302DL_CTRL1_STP		= 0x10,
	LIS302DL_CTRL1_FS		= 0x20,
	LIS302DL_CTRL1_PD		= 0x40,
	LIS302DL_CTRL1_DR		= 0x80,
};

enum lis302dl_reg_ctrl2 {
	LIS302DL_CTRL2_HPC1		= 0x01,
	LIS302DL_CTRL2_HPC2		= 0x02,
	LIS302DL_CTRL2_HPFF1		= 0x04,
	LIS302DL_CTRL2_HPFF2		= 0x08,
	LIS302DL_CTRL2_FDS		= 0x10,
	LIS302DL_CTRL2_BOOT		= 0x40,
	LIS302DL_CTRL2_SIM		= 0x80,
};
enum lis302dl_reg_ctrl3 {
	LIS302DL_CTRL3_PP_OD		= 0x40,
	LIS302DL_CTRL3_IHL		= 0x80,
};

enum lis302dl_reg_status {
	LIS302DL_STATUS_XDA		= 0x01,
	LIS302DL_STATUS_YDA		= 0x02,
	LIS302DL_STATUS_ZDA		= 0x04,
	LIS302DL_STATUS_XYZDA		= 0x08,
	LIS302DL_STATUS_XOR		= 0x10,
	LIS302DL_STATUS_YOR		= 0x20,
	LIS302DL_STATUS_ZOR		= 0x40,
	LIS302DL_STATUS_XYZOR		= 0x80,
};

/* Wakeup/freefall interrupt defs */
enum lis302dl_reg_ffwucfg {
	LIS302DL_FFWUCFG_XLIE		= 0x01,
	LIS302DL_FFWUCFG_XHIE		= 0x02,
	LIS302DL_FFWUCFG_YLIE		= 0x04,
	LIS302DL_FFWUCFG_YHIE		= 0x08,
	LIS302DL_FFWUCFG_ZLIE		= 0x10,
	LIS302DL_FFWUCFG_ZHIE		= 0x20,
	LIS302DL_FFWUCFG_LIR		= 0x40,
	LIS302DL_FFWUCFG_AOI		= 0x80,
};

enum lis302dl_reg_ffwuths {
	LIS302DL_FFWUTHS_DCRM		= 0x80,
};

enum lis302dl_reg_ffwusrc {
	LIS302DL_FFWUSRC_XL		= 0x01,
	LIS302DL_FFWUSRC_XH		= 0x02,
	LIS302DL_FFWUSRC_YL		= 0x04,
	LIS302DL_FFWUSRC_YH		= 0x08,
	LIS302DL_FFWUSRC_ZL		= 0x10,
	LIS302DL_FFWUSRC_ZH		= 0x20,
	LIS302DL_FFWUSRC_IA		= 0x40,
};

enum lis302dl_reg_cloik_src {
	LIS302DL_CLICKSRC_SINGLE_X	= 0x01,
	LIS302DL_CLICKSRC_DOUBLE_X	= 0x02,
	LIS302DL_CLICKSRC_SINGLE_Y	= 0x04,
	LIS302DL_CLICKSRC_DOUBLE_Y	= 0x08,
	LIS302DL_CLICKSRC_SINGLE_Z	= 0x10,
	LIS302DL_CLICKSRC_DOUBLE_Z	= 0x20,
	LIS302DL_CLICKSRC_IA		= 0x40,
};

#define LIS302DL_WHO_AM_I_MAGIC		0x3b

#define LIS302DL_F_WUP_FF_1		0x0001	/* wake up from free fall */
#define LIS302DL_F_WUP_FF_2		0x0002
#define LIS302DL_F_WUP_FF		0x0003
#define LIS302DL_F_WUP_CLICK	0x0004
#define LIS302DL_F_POWER		0x0010
#define LIS302DL_F_FS			0x0020 	/* ADC full scale */
#define LIS302DL_F_INPUT_OPEN 	0x0040  /* Set if input device is opened */
#define LIS302DL_F_IRQ_WAKE 	0x0080  /* IRQ is setup in wake mode */
#define LIS302DL_F_DR			0x0100 	/* Data rate, 400Hz/100Hz */

#endif /* _LINUX_LIS302DL_H */

