/* ==========================================================================
 * $File: //dwh/usb_iip/dev/software/otg/linux/drivers/dwc_otg_driver.c $
 * $Revision: #63 $
 * $Date: 2008/09/24 $
 * $Change: 1101777 $
 *
 * Synopsys HS OTG Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */

/** @file
 * The dwc_otg_driver module provides the initialization and cleanup entry
 * points for the DWC_otg driver. This module will be dynamically installed
 * after Linux is booted using the insmod command. When the module is
 * installed, the dwc_otg_driver_init function is called. When the module is
 * removed (using rmmod), the dwc_otg_driver_cleanup function is called.
 *
 * This module also defines a data structure for the dwc_otg_driver, which is
 * used in conjunction with the standard ARM lm_device structure. These
 * structures allow the OTG driver to comply with the standard Linux driver
 * model in which devices and drivers are registered with a bus driver. This
 * has the benefit that Linux can expose attributes of the driver and device
 * in its special sysfs file system. Users can then read or write files in
 * this file system to perform diagnostics on the driver components or the
 * device.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/stat.h>	 /* permission constants */
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <asm/io.h>

#include <asm/sizes.h>
#include <mach/pm.h>

#include "otg_plat.h"
#include "otg_attr.h"
#include "otg_driver.h"
#include "otg_cil.h"
#include "otg_pcd.h"
#include "otg_hcd.h"

#define DWC_DRIVER_VERSION	"2.72a 24-JUN-2008"
#define DWC_DRIVER_DESC		"HS OTG USB Controller driver"

static const char dwc_driver_name[] = "dwc_otg";

/*-------------------------------------------------------------------------*/
/* Encapsulate the module parameter settings */

static dwc_otg_core_params_t dwc_otg_module_params = {
	.opt = -1,
	.otg_cap = -1,
	.dma_enable = -1,
	.dma_desc_enable = -1,
	.dma_burst_size = -1,
	.speed = -1,
	.host_support_fs_ls_low_power = -1,
	.host_ls_low_power_phy_clk = -1,
	.enable_dynamic_fifo = -1,
	.data_fifo_size = -1,
	.dev_rx_fifo_size = -1,
	.dev_nperio_tx_fifo_size = -1,
	.dev_perio_tx_fifo_size = {
		/* dev_perio_tx_fifo_size_1 */
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1
		/* 15 */
	},
	.host_rx_fifo_size = -1,
	.host_nperio_tx_fifo_size = -1,
	.host_perio_tx_fifo_size = -1,
	.max_transfer_size = -1,
	.max_packet_count = -1,
	.host_channels = -1,
	.dev_endpoints = -1,
	.phy_type = -1,
	.phy_utmi_width = -1,
	.phy_ulpi_ddr = -1,
	.phy_ulpi_ext_vbus = -1,
	.i2c_enable = -1,
	.ulpi_fs_ls = -1,
	.ts_dline = -1,
	.en_multiple_tx_fifo = -1,
	.dev_tx_fifo_size = {
		/* dev_tx_fifo_size */
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1,
		-1
		/* 15 */
	},
	.thr_ctl = -1,
	.tx_thr_length = -1,
	.rx_thr_length = -1,
	.pti_enable = -1,
	.mpi_enable = -1,
};

/**
 * Global Debug Level Mask.
 */
uint32_t g_dbg_lvl = 0; /* OFF */

/**
 * This function is called during module intialization to verify that
 * the module parameters are in a valid state.
 */
static int check_parameters(dwc_otg_core_if_t *core_if)
{
	int i;
	int retval = 0;

/* Checks if the parameter is outside of its valid range of values */
#define DWC_OTG_PARAM_TEST(_param_, _low_, _high_) \
		((dwc_otg_module_params._param_ < (_low_)) || \
		(dwc_otg_module_params._param_ > (_high_)))

/* If the parameter has been set by the user, check that the parameter value is
 * within the value range of values.  If not, report a module error. */
#define DWC_OTG_PARAM_ERR(_param_, _low_, _high_, _string_) \
		do { \
			if (dwc_otg_module_params._param_ != -1) { \
				if (DWC_OTG_PARAM_TEST(_param_, (_low_), (_high_))) { \
					DWC_ERROR("`%d' invalid for parameter `%s'\n", \
						  dwc_otg_module_params._param_, _string_); \
					dwc_otg_module_params._param_ = dwc_param_##_param_##_default; \
					retval++; \
				} \
			} \
		} while (0)

	DWC_OTG_PARAM_ERR(opt,0,1,"opt");
	DWC_OTG_PARAM_ERR(otg_cap,0,2,"otg_cap");
	DWC_OTG_PARAM_ERR(dma_enable,0,1,"dma_enable");
	DWC_OTG_PARAM_ERR(dma_desc_enable,0,1,"dma_desc_enable");
	DWC_OTG_PARAM_ERR(speed,0,1,"speed");
	DWC_OTG_PARAM_ERR(host_support_fs_ls_low_power,0,1,"host_support_fs_ls_low_power");
	DWC_OTG_PARAM_ERR(host_ls_low_power_phy_clk,0,1,"host_ls_low_power_phy_clk");
	DWC_OTG_PARAM_ERR(enable_dynamic_fifo,0,1,"enable_dynamic_fifo");
	DWC_OTG_PARAM_ERR(data_fifo_size,32,32768,"data_fifo_size");
	DWC_OTG_PARAM_ERR(dev_rx_fifo_size,16,32768,"dev_rx_fifo_size");
	DWC_OTG_PARAM_ERR(dev_nperio_tx_fifo_size,16,32768,"dev_nperio_tx_fifo_size");
	DWC_OTG_PARAM_ERR(host_rx_fifo_size,16,32768,"host_rx_fifo_size");
	DWC_OTG_PARAM_ERR(host_nperio_tx_fifo_size,16,32768,"host_nperio_tx_fifo_size");
	DWC_OTG_PARAM_ERR(host_perio_tx_fifo_size,16,32768,"host_perio_tx_fifo_size");
	DWC_OTG_PARAM_ERR(max_transfer_size,2047,524288,"max_transfer_size");
	DWC_OTG_PARAM_ERR(max_packet_count,15,511,"max_packet_count");
	DWC_OTG_PARAM_ERR(host_channels,1,16,"host_channels");
	DWC_OTG_PARAM_ERR(dev_endpoints,1,15,"dev_endpoints");
	DWC_OTG_PARAM_ERR(phy_type,0,2,"phy_type");
	DWC_OTG_PARAM_ERR(phy_ulpi_ddr,0,1,"phy_ulpi_ddr");
	DWC_OTG_PARAM_ERR(phy_ulpi_ext_vbus,0,1,"phy_ulpi_ext_vbus");
	DWC_OTG_PARAM_ERR(i2c_enable,0,1,"i2c_enable");
	DWC_OTG_PARAM_ERR(ulpi_fs_ls,0,1,"ulpi_fs_ls");
	DWC_OTG_PARAM_ERR(ts_dline,0,1,"ts_dline");

	if (dwc_otg_module_params.dma_burst_size != -1) {
		if (DWC_OTG_PARAM_TEST(dma_burst_size,1,1) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,4,4) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,8,8) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,16,16) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,32,32) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,64,64) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,128,128) &&
		    DWC_OTG_PARAM_TEST(dma_burst_size,256,256)) {
			DWC_ERROR("`%d' invalid for parameter `dma_burst_size'\n",
				  dwc_otg_module_params.dma_burst_size);
			dwc_otg_module_params.dma_burst_size = 32;
			retval++;
		}

		{
			uint8_t brst_sz = 0;
			while(dwc_otg_module_params.dma_burst_size > 1) {
				brst_sz ++;
				dwc_otg_module_params.dma_burst_size >>= 1;
			}
			dwc_otg_module_params.dma_burst_size = brst_sz;
		}
	}

	if (dwc_otg_module_params.phy_utmi_width != -1) {
		if (DWC_OTG_PARAM_TEST(phy_utmi_width, 8, 8) &&
		    DWC_OTG_PARAM_TEST(phy_utmi_width, 16, 16)) {
			DWC_ERROR("`%d' invalid for parameter `phy_utmi_width'\n",
				  dwc_otg_module_params.phy_utmi_width);
			dwc_otg_module_params.phy_utmi_width = 16;
			retval++;
		}
	}

	for (i = 0; i < 15; i++) {
		/** @todo should be like above */
		//DWC_OTG_PARAM_ERR(dev_perio_tx_fifo_size[i], 4, 768, "dev_perio_tx_fifo_size");
		if (dwc_otg_module_params.dev_perio_tx_fifo_size[i] != -1) {
			if (DWC_OTG_PARAM_TEST(dev_perio_tx_fifo_size[i], 4, 768)) {
				DWC_ERROR("`%d' invalid for parameter `%s_%d'\n",
					  dwc_otg_module_params.dev_perio_tx_fifo_size[i], "dev_perio_tx_fifo_size", i);
				dwc_otg_module_params.dev_perio_tx_fifo_size[i] = dwc_param_dev_perio_tx_fifo_size_default;
				retval++;
			}
		}
	}

	DWC_OTG_PARAM_ERR(en_multiple_tx_fifo, 0, 1, "en_multiple_tx_fifo");

	for (i = 0; i < 15; i++) {
		/** @todo should be like above */
		//DWC_OTG_PARAM_ERR(dev_tx_fifo_size[i], 4, 768, "dev_tx_fifo_size");
		if (dwc_otg_module_params.dev_tx_fifo_size[i] != -1) {
			if (DWC_OTG_PARAM_TEST(dev_tx_fifo_size[i], 4, 768)) {
				DWC_ERROR("`%d' invalid for parameter `%s_%d'\n",
					  dwc_otg_module_params.dev_tx_fifo_size[i], "dev_tx_fifo_size", i);
				dwc_otg_module_params.dev_tx_fifo_size[i] = dwc_param_dev_tx_fifo_size_default;
				retval++;
			}
		}
	}

	DWC_OTG_PARAM_ERR(thr_ctl, 0, 7, "thr_ctl");
	DWC_OTG_PARAM_ERR(tx_thr_length, 8, 128, "tx_thr_length");
	DWC_OTG_PARAM_ERR(rx_thr_length, 8, 128, "rx_thr_length");

	DWC_OTG_PARAM_ERR(pti_enable,0,1,"pti_enable");
	DWC_OTG_PARAM_ERR(mpi_enable,0,1,"mpi_enable");

	/* At this point, all module parameters that have been set by the user
	 * are valid, and those that have not are left unset.  Now set their
	 * default values and/or check the parameters against the hardware
	 * configurations of the OTG core. */

/* This sets the parameter to the default value if it has not been set by the
 * user */
#define DWC_OTG_PARAM_SET_DEFAULT(_param_) \
	({ \
		int changed = 1; \
		if (dwc_otg_module_params._param_ == -1) { \
			changed = 0; \
			dwc_otg_module_params._param_ = dwc_param_##_param_##_default; \
		} \
		changed; \
	})

/* This checks the macro agains the hardware configuration to see if it is
 * valid.  It is possible that the default value could be invalid. In this
 * case, it will report a module error if the user touched the parameter.
 * Otherwise it will adjust the value without any error. */
#define DWC_OTG_PARAM_CHECK_VALID(_param_, _str_, _is_valid_, _set_valid_) \
	({ \
		int changed = DWC_OTG_PARAM_SET_DEFAULT(_param_); \
		int error = 0; \
		if (!(_is_valid_)) { \
			if (changed) { \
				DWC_ERROR("`%d' invalid for parameter `%s'. Check HW configuration.\n", dwc_otg_module_params._param_, _str_); \
				error = 1; \
			} \
			dwc_otg_module_params._param_ = (_set_valid_); \
		} \
		error; \
	})

	/* OTG Cap */
	retval += DWC_OTG_PARAM_CHECK_VALID(otg_cap, "otg_cap",
				({
					int valid;
					valid = 1;
					switch (dwc_otg_module_params.otg_cap) {
					case DWC_OTG_CAP_PARAM_HNP_SRP_CAPABLE:
						if (core_if->hwcfg2.b.op_mode != DWC_HWCFG2_OP_MODE_HNP_SRP_CAPABLE_OTG)
							valid = 0;
						break;
					case DWC_OTG_CAP_PARAM_SRP_ONLY_CAPABLE:
						if ((core_if->hwcfg2.b.op_mode != DWC_HWCFG2_OP_MODE_HNP_SRP_CAPABLE_OTG) &&
						    (core_if->hwcfg2.b.op_mode != DWC_HWCFG2_OP_MODE_SRP_ONLY_CAPABLE_OTG) &&
						    (core_if->hwcfg2.b.op_mode != DWC_HWCFG2_OP_MODE_SRP_CAPABLE_DEVICE) &&
						    (core_if->hwcfg2.b.op_mode != DWC_HWCFG2_OP_MODE_SRP_CAPABLE_HOST)) {
							valid = 0;
						}
						break;
					case DWC_OTG_CAP_PARAM_NO_HNP_SRP_CAPABLE:
						/* always valid */
						break;
					}
					valid;
				}),
				(((core_if->hwcfg2.b.op_mode == DWC_HWCFG2_OP_MODE_HNP_SRP_CAPABLE_OTG) ||
				  (core_if->hwcfg2.b.op_mode == DWC_HWCFG2_OP_MODE_SRP_ONLY_CAPABLE_OTG) ||
				  (core_if->hwcfg2.b.op_mode == DWC_HWCFG2_OP_MODE_SRP_CAPABLE_DEVICE) ||
				  (core_if->hwcfg2.b.op_mode == DWC_HWCFG2_OP_MODE_SRP_CAPABLE_HOST)) ?
				 DWC_OTG_CAP_PARAM_SRP_ONLY_CAPABLE :
				 DWC_OTG_CAP_PARAM_NO_HNP_SRP_CAPABLE));

	retval += DWC_OTG_PARAM_CHECK_VALID(dma_enable, "dma_enable",
				((dwc_otg_module_params.dma_enable == 1) && (core_if->hwcfg2.b.architecture == 0)) ? 0 : 1,
				0);

	retval += DWC_OTG_PARAM_CHECK_VALID(dma_desc_enable, "dma_desc_enable",
				((dwc_otg_module_params.dma_desc_enable == 1) &&
				 ((dwc_otg_module_params.dma_enable == 0) || (core_if->hwcfg4.b.desc_dma == 0))) ? 0 : 1,
				0);

	retval += DWC_OTG_PARAM_CHECK_VALID(opt, "opt", 1, 0);

	DWC_OTG_PARAM_SET_DEFAULT(dma_burst_size);

	retval += DWC_OTG_PARAM_CHECK_VALID(host_support_fs_ls_low_power,
				"host_support_fs_ls_low_power",
				1, 0);

	retval += DWC_OTG_PARAM_CHECK_VALID(enable_dynamic_fifo,
					"enable_dynamic_fifo",
					((dwc_otg_module_params.enable_dynamic_fifo == 0) ||
					(core_if->hwcfg2.b.dynamic_fifo == 1)), 0);

	retval += DWC_OTG_PARAM_CHECK_VALID(data_fifo_size,
					"data_fifo_size",
					(dwc_otg_module_params.data_fifo_size <= core_if->hwcfg3.b.dfifo_depth),
					core_if->hwcfg3.b.dfifo_depth);

	retval += DWC_OTG_PARAM_CHECK_VALID(dev_rx_fifo_size,
					"dev_rx_fifo_size",
					(dwc_otg_module_params.dev_rx_fifo_size <= dwc_read_reg32(&core_if->core_global_regs->grxfsiz)),
					dwc_read_reg32(&core_if->core_global_regs->grxfsiz));

	retval += DWC_OTG_PARAM_CHECK_VALID(dev_nperio_tx_fifo_size,
					"dev_nperio_tx_fifo_size",
					(dwc_otg_module_params.dev_nperio_tx_fifo_size <= (dwc_read_reg32(&core_if->core_global_regs->gnptxfsiz) >> 16)),
					(dwc_read_reg32(&core_if->core_global_regs->gnptxfsiz) >> 16));

	retval += DWC_OTG_PARAM_CHECK_VALID(host_rx_fifo_size,
					"host_rx_fifo_size",
					(dwc_otg_module_params.host_rx_fifo_size <= dwc_read_reg32(&core_if->core_global_regs->grxfsiz)),
					dwc_read_reg32(&core_if->core_global_regs->grxfsiz));

	retval += DWC_OTG_PARAM_CHECK_VALID(host_nperio_tx_fifo_size,
					"host_nperio_tx_fifo_size",
					(dwc_otg_module_params.host_nperio_tx_fifo_size <= (dwc_read_reg32(&core_if->core_global_regs->gnptxfsiz) >> 16)),
					(dwc_read_reg32(&core_if->core_global_regs->gnptxfsiz) >> 16));

	retval += DWC_OTG_PARAM_CHECK_VALID(host_perio_tx_fifo_size,
					"host_perio_tx_fifo_size",
					(dwc_otg_module_params.host_perio_tx_fifo_size <= ((dwc_read_reg32(&core_if->core_global_regs->hptxfsiz) >> 16))),
					((dwc_read_reg32(&core_if->core_global_regs->hptxfsiz) >> 16)));

	retval += DWC_OTG_PARAM_CHECK_VALID(max_transfer_size,
					"max_transfer_size",
					(dwc_otg_module_params.max_transfer_size < (1 << (core_if->hwcfg3.b.xfer_size_cntr_width + 11))),
					((1 << (core_if->hwcfg3.b.xfer_size_cntr_width + 11)) - 1));

	retval += DWC_OTG_PARAM_CHECK_VALID(max_packet_count,
					"max_packet_count",
					(dwc_otg_module_params.max_packet_count < (1 << (core_if->hwcfg3.b.packet_size_cntr_width + 4))),
					((1 << (core_if->hwcfg3.b.packet_size_cntr_width + 4)) - 1));

	retval += DWC_OTG_PARAM_CHECK_VALID(host_channels,
					"host_channels",
					(dwc_otg_module_params.host_channels <= (core_if->hwcfg2.b.num_host_chan + 1)),
					(core_if->hwcfg2.b.num_host_chan + 1));

	retval += DWC_OTG_PARAM_CHECK_VALID(dev_endpoints,
					"dev_endpoints",
					(dwc_otg_module_params.dev_endpoints <= (core_if->hwcfg2.b.num_dev_ep)),
					core_if->hwcfg2.b.num_dev_ep);

/*
 * Define the following to disable the FS PHY Hardware checking.  This is for
 * internal testing only.
 *
 * #define NO_FS_PHY_HW_CHECKS
 */

#ifdef NO_FS_PHY_HW_CHECKS
	retval += DWC_OTG_PARAM_CHECK_VALID(phy_type,
				"phy_type", 1, 0);
#else
	retval += DWC_OTG_PARAM_CHECK_VALID(phy_type,
				"phy_type",
				({
					int valid = 0;
					if ((dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_UTMI) &&
					((core_if->hwcfg2.b.hs_phy_type == 1) ||
					 (core_if->hwcfg2.b.hs_phy_type == 3))) {
						valid = 1;
					}
					else if ((dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_ULPI) &&
						 ((core_if->hwcfg2.b.hs_phy_type == 2) ||
						  (core_if->hwcfg2.b.hs_phy_type == 3))) {
						valid = 1;
					}
					else if ((dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_FS) &&
						 (core_if->hwcfg2.b.fs_phy_type == 1)) {
						valid = 1;
					}
					valid;
				}),
				({
					int set = DWC_PHY_TYPE_PARAM_FS;
					if (core_if->hwcfg2.b.hs_phy_type) {
						if ((core_if->hwcfg2.b.hs_phy_type == 3) ||
						(core_if->hwcfg2.b.hs_phy_type == 1)) {
							set = DWC_PHY_TYPE_PARAM_UTMI;
						}
						else {
							set = DWC_PHY_TYPE_PARAM_ULPI;
						}
					}
					set;
				}));
#endif

	retval += DWC_OTG_PARAM_CHECK_VALID(speed, "speed",
				(dwc_otg_module_params.speed == 0) && (dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_FS) ? 0 : 1,
				dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_FS ? 1 : 0);

	retval += DWC_OTG_PARAM_CHECK_VALID(host_ls_low_power_phy_clk,
				"host_ls_low_power_phy_clk",
				((dwc_otg_module_params.host_ls_low_power_phy_clk == DWC_HOST_LS_LOW_POWER_PHY_CLK_PARAM_48MHZ) && (dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_FS) ? 0 : 1),
				((dwc_otg_module_params.phy_type == DWC_PHY_TYPE_PARAM_FS) ? DWC_HOST_LS_LOW_POWER_PHY_CLK_PARAM_6MHZ : DWC_HOST_LS_LOW_POWER_PHY_CLK_PARAM_48MHZ));

	DWC_OTG_PARAM_SET_DEFAULT(phy_ulpi_ddr);
	DWC_OTG_PARAM_SET_DEFAULT(phy_ulpi_ext_vbus);
	DWC_OTG_PARAM_SET_DEFAULT(phy_utmi_width);
	DWC_OTG_PARAM_SET_DEFAULT(ulpi_fs_ls);
	DWC_OTG_PARAM_SET_DEFAULT(ts_dline);

#ifdef NO_FS_PHY_HW_CHECKS
	retval += DWC_OTG_PARAM_CHECK_VALID(i2c_enable, "i2c_enable", 1, 0);
#else
	retval += DWC_OTG_PARAM_CHECK_VALID(i2c_enable,
				"i2c_enable",
				(dwc_otg_module_params.i2c_enable == 1) && (core_if->hwcfg3.b.i2c == 0) ? 0 : 1,
				0);
#endif

	for (i = 0; i < 15; i++) {
		int changed = 1;
		int error = 0;

		if (dwc_otg_module_params.dev_perio_tx_fifo_size[i] == -1) {
			changed = 0;
			dwc_otg_module_params.dev_perio_tx_fifo_size[i] = dwc_param_dev_perio_tx_fifo_size_default;
		}
		if (!(dwc_otg_module_params.dev_perio_tx_fifo_size[i] <= (dwc_read_reg32(&core_if->core_global_regs->dptxfsiz_dieptxf[i])))) {
			if (changed) {
				DWC_ERROR("`%d' invalid for parameter `dev_perio_fifo_size_%d'. Check HW configuration.\n", dwc_otg_module_params.dev_perio_tx_fifo_size[i], i);
				error = 1;
			}
			dwc_otg_module_params.dev_perio_tx_fifo_size[i] = dwc_read_reg32(&core_if->core_global_regs->dptxfsiz_dieptxf[i]);
		}
		retval += error;
	}

	retval += DWC_OTG_PARAM_CHECK_VALID(en_multiple_tx_fifo, "en_multiple_tx_fifo",
						((dwc_otg_module_params.en_multiple_tx_fifo == 1) && (core_if->hwcfg4.b.ded_fifo_en == 0)) ? 0 : 1,
						0);

	for (i = 0; i < 15; i++) {
		int changed = 1;
		int error = 0;

		if (dwc_otg_module_params.dev_tx_fifo_size[i] == -1) {
			changed = 0;
			dwc_otg_module_params.dev_tx_fifo_size[i] = dwc_param_dev_tx_fifo_size_default;
		}
		if (!(dwc_otg_module_params.dev_tx_fifo_size[i] <= (dwc_read_reg32(&core_if->core_global_regs->dptxfsiz_dieptxf[i])))) {
			if (changed) {
				DWC_ERROR("%d' invalid for parameter `dev_perio_fifo_size_%d'. Check HW configuration.\n", dwc_otg_module_params.dev_tx_fifo_size[i], i);
				error = 1;
			}
			dwc_otg_module_params.dev_tx_fifo_size[i] = dwc_read_reg32(&core_if->core_global_regs->dptxfsiz_dieptxf[i]);
		}
		retval += error;
	}

	retval += DWC_OTG_PARAM_CHECK_VALID(thr_ctl, "thr_ctl",
				((dwc_otg_module_params.thr_ctl != 0) && ((dwc_otg_module_params.dma_enable == 0) || (core_if->hwcfg4.b.ded_fifo_en == 0))) ? 0 : 1,
				0);

	DWC_OTG_PARAM_SET_DEFAULT(tx_thr_length);
	DWC_OTG_PARAM_SET_DEFAULT(rx_thr_length);

	retval += DWC_OTG_PARAM_CHECK_VALID(pti_enable, "pti_enable",
		((dwc_otg_module_params.pti_enable == 0) || ((dwc_otg_module_params.pti_enable == 1) && (core_if->snpsid >= 0x4F54272A))) ? 1 : 0,
			0);

	retval += DWC_OTG_PARAM_CHECK_VALID(mpi_enable, "mpi_enable",
			((dwc_otg_module_params.mpi_enable == 0) || ((dwc_otg_module_params.mpi_enable == 1) && (core_if->hwcfg2.b.multi_proc_int == 1))) ? 1 : 0,
			0);
	return retval;
}

/**
 * This function is the top level interrupt handler for the Common
 * (Device and host modes) interrupts.
 */
static irqreturn_t dwc_otg_common_irq(int irq, void *dev)
{
	dwc_otg_device_t *otg_dev = dev;
	int32_t retval = IRQ_NONE;

	retval = dwc_otg_handle_common_intr(otg_dev->core_if);
	return IRQ_RETVAL(retval);
}

/**
 * This function is called when a lm_device is unregistered with the
 * dwc_otg_driver. This happens, for example, when the rmmod command is
 * executed. The device may or may not be electrically present. If it is
 * present, the driver stops device processing. Any resources used on behalf
 * of this device are freed.
 *
 * @param[in] lmdev
 */
static int dwc_otg_driver_cleanup(struct platform_device *pdev)
{
	dwc_otg_device_t *otg_dev = platform_get_drvdata(pdev);
	DWC_DEBUGPL(DBG_ANY, "%s(%p)\n", __func__, pdev);

	if (!otg_dev) {
		/* Memory allocation for the dwc_otg_device failed. */
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev NULL!\n", __func__);
		return 0;
	}

	/*
	 * Free the IRQ
	 */
	if (otg_dev->common_irq_installed) {
		free_irq(otg_dev->irq, otg_dev);
	}

#ifndef DWC_DEVICE_ONLY
	if (otg_dev->hcd) {
		dwc_otg_hcd_remove(pdev);
	} else {
		DWC_DEBUGPL(DBG_ANY, "%s: otg_dev->hcd NULL!\n", __func__);
		return 0;
	}
#endif

#ifndef DWC_HOST_ONLY
	if (otg_dev->pcd) {
		dwc_otg_pcd_remove(pdev);
	}
#endif
	if (otg_dev->core_if) {
		dwc_otg_cil_remove(otg_dev->core_if);
	}

	/*
	 * Remove the device attributes
	 */
	dwc_otg_attr_remove(pdev);

	/*
	 * Return the memory.
	 */
	if (otg_dev->base) {
		iounmap(otg_dev->base);
	}
	kfree(otg_dev);

	/*
	 * Clear the drvdata pointer.
	 */
	platform_set_drvdata(pdev, 0);

	return 0;
}

/**
 * This function is called when an lm_device is bound to a
 * dwc_otg_driver. It creates the driver components required to
 * control the device (CIL, HCD, and PCD) and it initializes the
 * device. The driver components are stored in a dwc_otg_device
 * structure. A reference to the dwc_otg_device is saved in the
 * lm_device. This allows the driver to access the dwc_otg_device
 * structure on subsequent calls to driver methods for this device.
 *
 * @param[in] lmdev  lm_device definition
 */
static int dwc_otg_driver_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int retval = 0;
	uint32_t snpsid;
	dwc_otg_device_t *dwc_otg_device;
	struct resource *res;

	dev_dbg(dev, "dwc_otg_driver_probe(%p)\n", pdev);

	dwc_otg_device = kmalloc(sizeof(dwc_otg_device_t), GFP_KERNEL);

	if (!dwc_otg_device) {
		dev_err(dev, "kmalloc of dwc_otg_device failed\n");
		retval = -ENOMEM;
		goto fail;
	}

	memset(dwc_otg_device, 0, sizeof(*dwc_otg_device));
	dwc_otg_device->reg_offset = 0xFFFFFFFF;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "Found OTG with no register addr.\n");
		retval = -ENODEV;
		goto fail;
	}
	dwc_otg_device->rsrc_start = res->start;
	dwc_otg_device->rsrc_len = res->end - res->start + 1;

	dwc_otg_device->base = ioremap(dwc_otg_device->rsrc_start, dwc_otg_device->rsrc_len);

	if (!dwc_otg_device->base) {
		dev_err(dev, "ioremap() failed\n");
		retval = -ENOMEM;
		goto fail;
	}
	dev_dbg(dev, "base=0x%08x\n", (unsigned)dwc_otg_device->base);

	/*
	 * Attempt to ensure this device is really a DWC_otg Controller.
	 * Read and verify the SNPSID register contents. The value should be
	 * 0x45F42XXX, which corresponds to "OT2", as in "OTG version 2.XX".
	 */
	snpsid = dwc_read_reg32((uint32_t *)((uint8_t *)dwc_otg_device->base + 0x40));

	if ((snpsid & 0xFFFFF000) != OTG_CORE_REV_2_00) {
		dev_err(dev, "Bad value for SNPSID: 0x%08x\n", snpsid);
		retval = -EINVAL;
		goto fail;
	}

	DWC_PRINT("Core Release: %x.%x%x%x\n",
			(snpsid >> 12 & 0xF),
			(snpsid >> 8 & 0xF),
			(snpsid >> 4 & 0xF),
			(snpsid & 0xF));

	/*
	 * Initialize driver data to point to the global DWC_otg
	 * Device structure.
	 */
	platform_set_drvdata(pdev, dwc_otg_device);

	dev_dbg(dev, "dwc_otg_device=0x%p\n", dwc_otg_device);

	dwc_otg_device->core_if = dwc_otg_cil_init(dwc_otg_device->base,
						   &dwc_otg_module_params);

	dwc_otg_device->core_if->snpsid = snpsid;

	if (!dwc_otg_device->core_if) {
		dev_err(dev, "CIL initialization failed!\n");
		retval = -ENOMEM;
		goto fail;
	}

	/*
	 * Validate parameter values.
	 */
	if (check_parameters(dwc_otg_device->core_if)) {
		retval = -EINVAL;
		goto fail;
	}

	/*
	 * Create Device Attributes in sysfs
	 */
	dwc_otg_attr_create(pdev);

	/*
	 * Disable the global interrupt until all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_disable_global_interrupts(dwc_otg_device->core_if);

	/*
	 * Install the interrupt handler for the common interrupts before
	 * enabling common interrupts in core_init below.
	 */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(dev, "Fount OTG with to IRQ.\n");
		retval = -ENODEV;
		goto fail;
	}
	dwc_otg_device->irq = res->start;

	retval = request_irq(res->start, dwc_otg_common_irq,
			     IRQF_SHARED, "dwc_otg", dwc_otg_device);
	if (retval) {
		DWC_ERROR("request of irq%d failed\n", res->start);
		retval = -EBUSY;
		goto fail;
	} else {
		dwc_otg_device->common_irq_installed = 1;
	}

	/*
	 * Initialize the DWC_otg core.
	 */
	dwc_otg_core_init(dwc_otg_device->core_if);

#ifndef DWC_HOST_ONLY
	/*
	 * Initialize the PCD
	 */
	retval = dwc_otg_pcd_init(pdev);
	if (retval != 0) {
		DWC_ERROR("dwc_otg_pcd_init failed\n");
		dwc_otg_device->pcd = NULL;
		goto fail;
	}
#endif
#ifndef DWC_DEVICE_ONLY
	/*
	 * Initialize the HCD
	 */
	retval = dwc_otg_hcd_init(pdev);
	if (retval != 0) {
		DWC_ERROR("dwc_otg_hcd_init failed\n");
		dwc_otg_device->hcd = NULL;
		goto fail;
	}
#endif

	/*
	 * Enable the global interrupt after all the interrupt
	 * handlers are installed.
	 */
	dwc_otg_enable_global_interrupts(dwc_otg_device->core_if);

	return 0;

 fail:
	dwc_otg_driver_cleanup(pdev);
	return retval;
}

static int dwc_otg_driver_remove(struct platform_device *pdev)
{
	return dwc_otg_driver_cleanup(pdev);
}

static struct platform_driver dwc_otg_platform_driver = {
	.driver.name = "dwc_otg",
	.probe = dwc_otg_driver_probe,
	.remove = dwc_otg_driver_remove,
};

static int __init dwc_otg_init_module(void)
{
	return platform_driver_register(&dwc_otg_platform_driver);
}

static void __exit dwc_otg_cleanup_module(void)
{
	platform_driver_unregister(&dwc_otg_platform_driver);
}

module_init(dwc_otg_init_module);
module_exit(dwc_otg_cleanup_module);

/**
 * This function is called when the driver is removed from the kernel
 * with the rmmod command. The driver unregisters itself with its bus
 * driver.
 *
 */

MODULE_DESCRIPTION(DWC_DRIVER_DESC);
MODULE_AUTHOR("Synopsys Inc.");
MODULE_LICENSE("GPL");

module_param_named(otg_cap, dwc_otg_module_params.otg_cap, int, 0444);
MODULE_PARM_DESC(otg_cap, "OTG Capabilities 0=HNP&SRP 1=SRP Only 2=None");
module_param_named(opt, dwc_otg_module_params.opt, int, 0444);
MODULE_PARM_DESC(opt, "OPT Mode");
module_param_named(dma_enable, dwc_otg_module_params.dma_enable, int, 0444);
MODULE_PARM_DESC(dma_enable, "DMA Mode 0=Slave 1=DMA enabled");

module_param_named(dma_desc_enable, dwc_otg_module_params.dma_desc_enable, int, 0444);
MODULE_PARM_DESC(dma_desc_enable, "DMA Desc Mode 0=Address DMA 1=DMA Descriptor enabled");

module_param_named(dma_burst_size, dwc_otg_module_params.dma_burst_size, int, 0444);
MODULE_PARM_DESC(dma_burst_size, "DMA Burst Size 1, 4, 8, 16, 32, 64, 128, 256");
module_param_named(speed, dwc_otg_module_params.speed, int, 0444);
MODULE_PARM_DESC(speed, "Speed 0=High Speed 1=Full Speed");
module_param_named(host_support_fs_ls_low_power, dwc_otg_module_params.host_support_fs_ls_low_power, int, 0444);
MODULE_PARM_DESC(host_support_fs_ls_low_power, "Support Low Power w/FS or LS 0=Support 1=Don't Support");
module_param_named(host_ls_low_power_phy_clk, dwc_otg_module_params.host_ls_low_power_phy_clk, int, 0444);
MODULE_PARM_DESC(host_ls_low_power_phy_clk, "Low Speed Low Power Clock 0=48Mhz 1=6Mhz");
module_param_named(enable_dynamic_fifo, dwc_otg_module_params.enable_dynamic_fifo, int, 0444);
MODULE_PARM_DESC(enable_dynamic_fifo, "0=cC Setting 1=Allow Dynamic Sizing");
module_param_named(data_fifo_size, dwc_otg_module_params.data_fifo_size, int, 0444);
MODULE_PARM_DESC(data_fifo_size, "Total number of words in the data FIFO memory 32-32768");
module_param_named(dev_rx_fifo_size, dwc_otg_module_params.dev_rx_fifo_size, int, 0444);
MODULE_PARM_DESC(dev_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(dev_nperio_tx_fifo_size, dwc_otg_module_params.dev_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(dev_nperio_tx_fifo_size, "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(dev_perio_tx_fifo_size_1, dwc_otg_module_params.dev_perio_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_1, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_2, dwc_otg_module_params.dev_perio_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_2, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_3, dwc_otg_module_params.dev_perio_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_3, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_4, dwc_otg_module_params.dev_perio_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_4, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_5, dwc_otg_module_params.dev_perio_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_5, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_6, dwc_otg_module_params.dev_perio_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_6, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_7, dwc_otg_module_params.dev_perio_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_7, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_8, dwc_otg_module_params.dev_perio_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_8, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_9, dwc_otg_module_params.dev_perio_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_9, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_10, dwc_otg_module_params.dev_perio_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_10, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_11, dwc_otg_module_params.dev_perio_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_11, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_12, dwc_otg_module_params.dev_perio_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_12, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_13, dwc_otg_module_params.dev_perio_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_13, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_14, dwc_otg_module_params.dev_perio_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_14, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(dev_perio_tx_fifo_size_15, dwc_otg_module_params.dev_perio_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_perio_tx_fifo_size_15, "Number of words in the periodic Tx FIFO 4-768");
module_param_named(host_rx_fifo_size, dwc_otg_module_params.host_rx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_rx_fifo_size, "Number of words in the Rx FIFO 16-32768");
module_param_named(host_nperio_tx_fifo_size, dwc_otg_module_params.host_nperio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_nperio_tx_fifo_size, "Number of words in the non-periodic Tx FIFO 16-32768");
module_param_named(host_perio_tx_fifo_size, dwc_otg_module_params.host_perio_tx_fifo_size, int, 0444);
MODULE_PARM_DESC(host_perio_tx_fifo_size, "Number of words in the host periodic Tx FIFO 16-32768");
module_param_named(max_transfer_size, dwc_otg_module_params.max_transfer_size, int, 0444);
/** @todo Set the max to 512K, modify checks */
MODULE_PARM_DESC(max_transfer_size, "The maximum transfer size supported in bytes 2047-65535");
module_param_named(max_packet_count, dwc_otg_module_params.max_packet_count, int, 0444);
MODULE_PARM_DESC(max_packet_count, "The maximum number of packets in a transfer 15-511");
module_param_named(host_channels, dwc_otg_module_params.host_channels, int, 0444);
MODULE_PARM_DESC(host_channels, "The number of host channel registers to use 1-16");
module_param_named(dev_endpoints, dwc_otg_module_params.dev_endpoints, int, 0444);
MODULE_PARM_DESC(dev_endpoints, "The number of endpoints in addition to EP0 available for device mode 1-15");
module_param_named(phy_type, dwc_otg_module_params.phy_type, int, 0444);
MODULE_PARM_DESC(phy_type, "0=Reserved 1=UTMI+ 2=ULPI");
module_param_named(phy_utmi_width, dwc_otg_module_params.phy_utmi_width, int, 0444);
MODULE_PARM_DESC(phy_utmi_width, "Specifies the UTMI+ Data Width 8 or 16 bits");
module_param_named(phy_ulpi_ddr, dwc_otg_module_params.phy_ulpi_ddr, int, 0444);
MODULE_PARM_DESC(phy_ulpi_ddr, "ULPI at double or single data rate 0=Single 1=Double");
module_param_named(phy_ulpi_ext_vbus, dwc_otg_module_params.phy_ulpi_ext_vbus, int, 0444);
MODULE_PARM_DESC(phy_ulpi_ext_vbus, "ULPI PHY using internal or external vbus 0=Internal");
module_param_named(i2c_enable, dwc_otg_module_params.i2c_enable, int, 0444);
MODULE_PARM_DESC(i2c_enable, "FS PHY Interface");
module_param_named(ulpi_fs_ls, dwc_otg_module_params.ulpi_fs_ls, int, 0444);
MODULE_PARM_DESC(ulpi_fs_ls, "ULPI PHY FS/LS mode only");
module_param_named(ts_dline, dwc_otg_module_params.ts_dline, int, 0444);
MODULE_PARM_DESC(ts_dline, "Term select Dline pulsing for all PHYs");
module_param_named(debug, g_dbg_lvl, int, 0444);
MODULE_PARM_DESC(debug, "");

module_param_named(en_multiple_tx_fifo, dwc_otg_module_params.en_multiple_tx_fifo, int, 0444);
MODULE_PARM_DESC(en_multiple_tx_fifo, "Dedicated Non Periodic Tx FIFOs 0=disabled 1=enabled");
module_param_named(dev_tx_fifo_size_1, dwc_otg_module_params.dev_tx_fifo_size[0], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_1, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_2, dwc_otg_module_params.dev_tx_fifo_size[1], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_2, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_3, dwc_otg_module_params.dev_tx_fifo_size[2], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_3, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_4, dwc_otg_module_params.dev_tx_fifo_size[3], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_4, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_5, dwc_otg_module_params.dev_tx_fifo_size[4], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_5, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_6, dwc_otg_module_params.dev_tx_fifo_size[5], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_6, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_7, dwc_otg_module_params.dev_tx_fifo_size[6], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_7, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_8, dwc_otg_module_params.dev_tx_fifo_size[7], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_8, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_9, dwc_otg_module_params.dev_tx_fifo_size[8], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_9, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_10, dwc_otg_module_params.dev_tx_fifo_size[9], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_10, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_11, dwc_otg_module_params.dev_tx_fifo_size[10], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_11, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_12, dwc_otg_module_params.dev_tx_fifo_size[11], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_12, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_13, dwc_otg_module_params.dev_tx_fifo_size[12], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_13, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_14, dwc_otg_module_params.dev_tx_fifo_size[13], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_14, "Number of words in the Tx FIFO 4-768");
module_param_named(dev_tx_fifo_size_15, dwc_otg_module_params.dev_tx_fifo_size[14], int, 0444);
MODULE_PARM_DESC(dev_tx_fifo_size_15, "Number of words in the Tx FIFO 4-768");

module_param_named(thr_ctl, dwc_otg_module_params.thr_ctl, int, 0444);
MODULE_PARM_DESC(thr_ctl, "Thresholding enable flag bit 0 - non ISO Tx thr., 1 - ISO Tx thr., 2 - Rx thr.- bit 0=disabled 1=enabled");
module_param_named(tx_thr_length, dwc_otg_module_params.tx_thr_length, int, 0444);
MODULE_PARM_DESC(tx_thr_length, "Tx Threshold length in 32 bit DWORDs");
module_param_named(rx_thr_length, dwc_otg_module_params.rx_thr_length, int, 0444);
MODULE_PARM_DESC(rx_thr_length, "Rx Threshold length in 32 bit DWORDs");

module_param_named(pti_enable, dwc_otg_module_params.pti_enable, int, 0444);
MODULE_PARM_DESC(pti_enable, "Per Transfer Interrupt mode 0=disabled 1=enabled");

module_param_named(mpi_enable, dwc_otg_module_params.mpi_enable, int, 0444);
MODULE_PARM_DESC(mpi_enable, "Multiprocessor Interrupt mode 0=disabled 1=enabled");
