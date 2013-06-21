/*****************************************************************************
 **   FILE NAME       : ifxusb_ctl.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   SRC VERSION     : 3.2
 **   DATE            : 1/Jan/2011
 **   DESCRIPTION     : Implementing the procfs and sysfs for IFX USB driver
 **   FUNCTIONS       :
 **   COMPILER        : gcc
 **   REFERENCE       : Synopsys DWC-OTG Driver 2.7
 **   COPYRIGHT       :  Copyright (c) 2010
 **                      LANTIQ DEUTSCHLAND GMBH,
 **                      Am Campeon 3, 85579 Neubiberg, Germany
 **
 **    This program is free software; you can redistribute it and/or modify
 **    it under the terms of the GNU General Public License as published by
 **    the Free Software Foundation; either version 2 of the License, or
 **    (at your option) any later version.
 **
 **  Version Control Section  **
 **   $Author$
 **   $Date$
 **   $Revisions$
 **   $Log$       Revision history
 *****************************************************************************/

/*
 * This file contains code fragments from Synopsys HS OTG Linux Software Driver.
 * For this code the following notice is applicable:
 *
 * ==========================================================================
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

/*! \file ifxusb_ctl.c
  \ingroup IFXUSB_DRIVER_V3
    \brief Implementing the procfs and sysfs for IFX USB driver
*/

#include <linux/version.h>
#include "ifxusb_version.h"


#include <linux/proc_fs.h>
#include <asm/byteorder.h>
#include <asm/unaligned.h>
#include <asm/uaccess.h>

#include "ifxusb_plat.h"
#include "ifxusb_regs.h"
#include "ifxusb_cif.h"

#ifdef __IS_DEVICE__
	#include "ifxpcd.h"
	#ifdef __GADGET_COC__
		#include <asm/ifx/ifx_types.h>
		#include <asm/ifx/ifx_pmcu.h>
		IFX_PMCU_REGISTER_t pmcuRegisterUSBGadget;
	#endif
#endif

#ifdef __IS_HOST__
	#include "ifxhcd.h"
	#ifdef __HOST_COC__
		#include <asm/ifx/ifx_types.h>
		#include <asm/ifx/ifx_pmcu.h>
		#ifdef __IS_DUAL__
		   IFX_PMCU_REGISTER_t pmcuRegisterUSBHost_1;
		   IFX_PMCU_REGISTER_t pmcuRegisterUSBHost_2;
		#else
		   IFX_PMCU_REGISTER_t pmcuRegisterUSBHost;
		#endif
	#endif
#endif

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gfp.h>

#ifdef __IS_HOST__
	extern char ifxusb_hcd_driver_name[];

	#ifdef __IS_DUAL__
		extern ifxhcd_hcd_t ifxusb_hcd_1;
		extern ifxhcd_hcd_t ifxusb_hcd_2;
		extern char ifxusb_hcd_name_1[];
		extern char ifxusb_hcd_name_2[];
	#else
		extern ifxhcd_hcd_t ifxusb_hcd;
		extern char ifxusb_hcd_name[];
	#endif

#endif

#ifdef __IS_DEVICE__
	extern char ifxusb_pcd_driver_name[];

	extern ifxpcd_pcd_t ifxusb_pcd;
	extern char ifxusb_pcd_name[];
#endif


//Attributes for sysfs (for 2.6 only)

#ifdef __IS_HOST__
extern struct device_attribute dev_attr_version_h;
#else
extern struct device_attribute dev_attr_version_d;
#endif
#ifdef __IS_HOST__
extern struct device_attribute dev_attr_dbglevel_h;
#else
extern struct device_attribute dev_attr_dbglevel_d;
#endif
#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		extern struct device_attribute dev_attr_suspend_host_1;
		extern struct device_attribute dev_attr_suspend_host_2;
		extern struct device_attribute dev_attr_probe_host_1;
		extern struct device_attribute dev_attr_probe_host_2;
		extern struct device_attribute dev_attr_probe_timer1_val_h;
		extern struct device_attribute dev_attr_probe_timer2_val_h;
		extern struct device_attribute dev_attr_autoprobe_timer1_val_h;
		extern struct device_attribute dev_attr_autoprobe_timer2_val_h;
	#else
		extern struct device_attribute dev_attr_suspend_host;
		extern struct device_attribute dev_attr_probe_host;
		extern struct device_attribute dev_attr_probe_timer_val_h;
		extern struct device_attribute dev_attr_autoprobe_timer_val_h;
	#endif
#endif

#ifdef __IS_DEVICE__
	extern struct device_attribute dev_attr_suspend_device;
	extern struct device_attribute dev_attr_probe_device;
	extern struct device_attribute dev_attr_probe_timer_val_d;
	extern struct device_attribute dev_attr_autoprobe_timer_val_d;
#endif

#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		extern struct device_attribute dev_attr_dump_params_h_1;
		extern struct device_attribute dev_attr_dump_params_h_2;
		extern struct device_attribute dev_attr_mode_h_1;
		extern struct device_attribute dev_attr_mode_h_2;
	#else
		extern struct device_attribute dev_attr_dump_params_h;
		extern struct device_attribute dev_attr_mode_h;
	#endif
#else
	extern struct device_attribute dev_attr_dump_params_d;
	extern struct device_attribute dev_attr_mode_d;
#endif

#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		extern struct device_attribute dev_attr_pkt_count_limit_bi_1;
		extern struct device_attribute dev_attr_pkt_count_limit_bo_1;
		extern struct device_attribute dev_attr_pkt_count_limit_bi_2;
		extern struct device_attribute dev_attr_pkt_count_limit_bo_2;
		extern struct device_attribute dev_attr_bandwidth_fs_1;
		extern struct device_attribute dev_attr_bandwidth_ls_1;
		extern struct device_attribute dev_attr_bandwidth_hs_2;
		extern struct device_attribute dev_attr_bandwidth_fs_2;
		extern struct device_attribute dev_attr_bandwidth_ls_2;
		extern struct device_attribute dev_attr_buspower_1;
		extern struct device_attribute dev_attr_buspower_2;
		extern struct device_attribute dev_attr_bussuspend_1;
		extern struct device_attribute dev_attr_bussuspend_2;
		extern struct device_attribute dev_attr_busconnected_1;
		extern struct device_attribute dev_attr_busconnected_2;
		extern struct device_attribute dev_attr_connectspeed_1;
		extern struct device_attribute dev_attr_connectspeed_1;
	#else
		extern struct device_attribute dev_attr_pkt_count_limit_bi;
		extern struct device_attribute dev_attr_pkt_count_limit_bo;
		extern struct device_attribute dev_attr_bandwidth_hs;
		extern struct device_attribute dev_attr_bandwidth_fs;
		extern struct device_attribute dev_attr_bandwidth_ls;
		extern struct device_attribute dev_attr_buspower;
		extern struct device_attribute dev_attr_bussuspend;
		extern struct device_attribute dev_attr_busconnected;
		extern struct device_attribute dev_attr_connectspeed;
	#endif
#endif //__IS_HOST__

#ifdef __IS_DEVICE__
	extern struct device_attribute dev_attr_devspeed;
	extern struct device_attribute dev_attr_enumspeed;
#endif //__IS_DEVICE__

#ifdef __ENABLE_DUMP__
	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			extern struct device_attribute dev_attr_dump_reg_h_1;
			extern struct device_attribute dev_attr_dump_reg_h_2;
			extern struct device_attribute dev_attr_dump_spram_h_1;
			extern struct device_attribute dev_attr_dump_spram_h_2;
			extern struct device_attribute dev_attr_dump_host_state_1;
			extern struct device_attribute dev_attr_dump_host_state_2;
		#else
			extern struct device_attribute dev_attr_dump_reg_h;
			extern struct device_attribute dev_attr_dump_spram_h;
			extern struct device_attribute dev_attr_dump_host_state;
		#endif
	#else
		extern struct device_attribute dev_attr_dump_reg_d;
		extern struct device_attribute dev_attr_dump_spram_d;
	#endif
#endif //__ENABLE_DUMP__


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

static ssize_t procfs_version_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	return sprintf( buf, "%s\n",IFXUSB_VERSION );
}

#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	static ssize_t sysfs_version_show( struct device *_dev, struct device_attribute *attr,char *buf)
#else
	static ssize_t sysfs_version_show( struct device *_dev,                               char *buf)
#endif
{
	return sprintf( buf, "%s\n",IFXUSB_VERSION );
}

#ifdef __IS_HOST__
DEVICE_ATTR(version_h, S_IRUGO|S_IWUSR, sysfs_version_show, NULL);
#else
DEVICE_ATTR(version_d, S_IRUGO|S_IWUSR, sysfs_version_show, NULL);
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

static ssize_t procfs_dbglevel_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	#ifdef __IS_HOST__
		return sprintf( buf, "%08X\n",h_dbg_lvl );
	#else
		return sprintf( buf, "%08X\n",d_dbg_lvl );
	#endif
}

static ssize_t procfs_dbglevel_store(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buf[10];
	int i = 0;
	uint32_t value;
	if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
		return -EFAULT;
	value = simple_strtoul(buf, NULL, 16);
	#ifdef __IS_HOST__
		h_dbg_lvl =value;
	#else
		d_dbg_lvl =value;
	#endif
		//turn on and off power
	return count;
}

#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	static ssize_t sysfs_dbglevel_show( struct device *_dev, struct device_attribute *attr,char *buf)
#else
	static ssize_t sysfs_dbglevel_show( struct device *_dev,                               char *buf)
#endif
{
	#ifdef __IS_HOST__
		return sprintf( buf, "%08X\n",h_dbg_lvl );
	#else
		return sprintf( buf, "%08X\n",d_dbg_lvl );
	#endif
}

#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
	static ssize_t sysfs_dbglevel_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
#else
    static ssize_t sysfs_dbglevel_store( struct device *_dev,                               const char *buffer, size_t count )
#endif
{
	char buf[10];
	int i = 0;
	uint32_t value;
	if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
		return -EFAULT;
	value = simple_strtoul(buf, NULL, 16);
	#ifdef __IS_HOST__
		h_dbg_lvl =value;
	#else
		d_dbg_lvl =value;
	#endif
		//turn on and off power
	return count;
}

#ifdef __IS_HOST__
DEVICE_ATTR(dbglevel_h, S_IRUGO|S_IWUSR, sysfs_dbglevel_show, sysfs_dbglevel_store);
#else
DEVICE_ATTR(dbglevel_d, S_IRUGO|S_IWUSR, sysfs_dbglevel_show, sysfs_dbglevel_store);
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

static void ifxusb_dump_params(ifxusb_core_if_t *_core_if);

#ifdef __IS_DUAL__
	static void dump_params_1(void)
	{
		ifxusb_dump_params(&ifxusb_hcd_1.core_if);
	}
	static void dump_params_2(void)
	{
		ifxusb_dump_params(&ifxusb_hcd_2.core_if);
	}

	static ssize_t procfs_dump_params_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		dump_params_1();
		return 0;
	}
	static ssize_t procfs_dump_params_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		dump_params_2();
		return 0;
	}

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_dump_params_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_dump_params_show_1( struct device *_dev,char *buf)
	#endif
	{
		dump_params_1();
		return 0;
	}
	DEVICE_ATTR(dump_params_h_1, S_IRUGO|S_IWUSR, sysfs_dump_params_show_1, NULL);

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_dump_params_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_dump_params_show_2( struct device *_dev,char *buf)
	#endif
	{
		dump_params_2();
		return 0;
	}

	DEVICE_ATTR(dump_params_h_2, S_IRUGO|S_IWUSR, sysfs_dump_params_show_2, NULL);
#else
	static void dump_params(void)
	{
		#ifdef __IS_HOST__
			ifxusb_dump_params(&ifxusb_hcd.core_if);
		#else
			ifxusb_dump_params(&ifxusb_pcd.core_if);
		#endif
	}

	static ssize_t procfs_dump_params_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		dump_params();
		return 0;
	}

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_dump_params_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_dump_params_show( struct device *_dev,char *buf)
	#endif
	{
		dump_params();
		return 0;
	}

	#ifdef __IS_HOST__
	DEVICE_ATTR(dump_params_h, S_IRUGO|S_IWUSR, sysfs_dump_params_show, NULL);
	#else
	DEVICE_ATTR(dump_params_d, S_IRUGO|S_IWUSR, sysfs_dump_params_show, NULL);
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __IS_DUAL__
	static ssize_t mode_show_1(char *buf)
	{
		if((ifxusb_rreg(&ifxusb_hcd_1.core_if.core_global_regs->gintsts ) & 0x1) == 1)
			return sprintf( buf, "HOST\n" );
		else
			return sprintf( buf, "DEVICE(INCORRECT!)\n" );
	}

	static ssize_t mode_show_2(char *buf)
	{
		if((ifxusb_rreg(&ifxusb_hcd_2.core_if.core_global_regs->gintsts ) & 0x1) == 1)
			return sprintf( buf, "HOST\n" );
		else
			return sprintf( buf, "DEVICE(INCORRECT!)\n" );
	}

	static ssize_t procfs_mode_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return mode_show_1(buf);
	}
	static ssize_t procfs_mode_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return mode_show_2(buf);
	}

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_mode_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_mode_show_1( struct device *_dev,char *buf)
	#endif
	{
		return mode_show_1(buf);
	}

	DEVICE_ATTR(mode_h_1, S_IRUGO|S_IWUSR, sysfs_mode_show_1, 0);

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_mode_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_mode_show_2( struct device *_dev,char *buf)
	#endif
	{
		return mode_show_2(buf);
	}
	DEVICE_ATTR(mode_h_2, S_IRUGO|S_IWUSR, sysfs_mode_show_2, NULL);
#else
	static ssize_t mode_show(char *buf)
	{
		#ifdef __IS_HOST__
			if((ifxusb_rreg(&ifxusb_hcd.core_if.core_global_regs->gintsts ) & 0x1) == 1)
				return sprintf( buf, "HOST\n" );
			else
				return sprintf( buf, "DEVICE(INCORRECT!)\n" );
		#else
			if((ifxusb_rreg(&ifxusb_pcd.core_if.core_global_regs->gintsts ) & 0x1) != 1)
				return sprintf( buf, "DEVICE\n" );
			else
				return sprintf( buf, "HOST(INCORRECT!)\n" );
		#endif
	}
	static ssize_t procfs_mode_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return mode_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_mode_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_mode_show( struct device *_dev,                               char *buf)
	#endif
	{
		return mode_show(buf);
	}
	#ifdef __IS_HOST__
	DEVICE_ATTR(mode_h, S_IRUGO|S_IWUSR, sysfs_mode_show, NULL);
	#else
	DEVICE_ATTR(mode_d, S_IRUGO|S_IWUSR, sysfs_mode_show, NULL);
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __IS_HOST__
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
	#ifdef __IS_DUAL__
		static ssize_t bandwidth_hs_show_1(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_1.pkt_remaining_reload_hs );
		}
		static ssize_t bandwidth_fs_show_1(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_1.pkt_remaining_reload_fs );
		}
		static ssize_t bandwidth_ls_show_1(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_1.pkt_remaining_reload_ls );
		}
		static void bandwidth_hs_store_1(uint32_t value)
		{
			if(value>16 && value<120)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_1.pkt_remaining_reload_hs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd_1.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_fs_store_1(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_1.pkt_remaining_reload_fs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
				if(hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_LOW_SPEED && hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd_1.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_ls_store_1(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_1.pkt_remaining_reload_ls = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
					ifxusb_hcd_1.pkt_remaining_reload=value;
			}
		}
		static ssize_t bandwidth_hs_show_2(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_2.pkt_remaining_reload_hs );
		}
		static ssize_t bandwidth_fs_show_2(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_2.pkt_remaining_reload_fs );
		}
		static ssize_t bandwidth_ls_show_2(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_2.pkt_remaining_reload_ls );
		}
		static void bandwidth_hs_store_2(uint32_t value)
		{
			if(value>16 && value<120)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_2.pkt_remaining_reload_hs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd_2.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_fs_store_2(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_2.pkt_remaining_reload_fs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
				if(hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_LOW_SPEED && hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd_2.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_ls_store_2(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd_2.pkt_remaining_reload_ls = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
					ifxusb_hcd_2.pkt_remaining_reload=value;
			}
		}
		static ssize_t procfs_bandwidth_hs_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_hs_show_1(buf);
		}
		static ssize_t procfs_bandwidth_fs_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_fs_show_1(buf);
		}
		static ssize_t procfs_bandwidth_ls_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_ls_show_1(buf);
		}
		static ssize_t procfs_bandwidth_hs_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store_1(value);
			return count;
		}
		static ssize_t procfs_bandwidth_fs_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store_1(value);
			return count;
		}
		static ssize_t procfs_bandwidth_ls_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store_1(value);
			return count;
		}
		static ssize_t procfs_bandwidth_hs_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_hs_show_2(buf);
		}
		static ssize_t procfs_bandwidth_fs_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_fs_show_2(buf);
		}
		static ssize_t procfs_bandwidth_ls_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_ls_show_2(buf);
		}
		static ssize_t procfs_bandwidth_hs_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store_2(value);
			return count;
		}
		static ssize_t procfs_bandwidth_fs_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store_2(value);
			return count;
		}
		static ssize_t procfs_bandwidth_ls_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store_2(value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_hs_show_1( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_hs_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_hs_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store_1(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_hs_1, S_IRUGO|S_IWUSR, sysfs_bandwidth_hs_show_1, sysfs_bandwidth_hs_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_fs_show_1( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_fs_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_fs_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store_1(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_fs_1, S_IRUGO|S_IWUSR, sysfs_bandwidth_fs_show_1, sysfs_bandwidth_fs_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_ls_show_1( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_ls_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_ls_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store_1(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_ls_1, S_IRUGO|S_IWUSR, sysfs_bandwidth_ls_show_1, sysfs_bandwidth_ls_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_hs_show_2( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_hs_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_hs_store_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store_2(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_hs_2, S_IRUGO|S_IWUSR, sysfs_bandwidth_hs_show_2, sysfs_bandwidth_hs_store_2);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_fs_show_2( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_fs_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_fs_store_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store_2(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_fs_2, S_IRUGO|S_IWUSR, sysfs_bandwidth_fs_show_2, sysfs_bandwidth_fs_store_2);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_ls_show_2( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_ls_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_ls_store_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store_2(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_ls_2, S_IRUGO|S_IWUSR, sysfs_bandwidth_ls_show_2, sysfs_bandwidth_ls_store_2);
	#else
		static ssize_t bandwidth_hs_show(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd.pkt_remaining_reload_hs );
		}
		static ssize_t bandwidth_fs_show(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd.pkt_remaining_reload_fs );
		}
		static ssize_t bandwidth_ls_show(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd.pkt_remaining_reload_ls );
		}
		static void bandwidth_hs_store(uint32_t value)
		{
			if     (value>16 && value<120)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd.pkt_remaining_reload_hs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_fs_store(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd.pkt_remaining_reload_fs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
				if(hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_LOW_SPEED && hprt0.b.prtspd != IFXUSB_HPRT0_PRTSPD_HIGH_SPEED)
					ifxusb_hcd.pkt_remaining_reload=value;
			}
		}
		static void bandwidth_ls_store(uint32_t value)
		{
			if     (value>2 && value<30)
			{
				hprt0_data_t hprt0;
				ifxusb_hcd.pkt_remaining_reload_hs = value;
				hprt0.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
				if(hprt0.b.prtspd == IFXUSB_HPRT0_PRTSPD_LOW_SPEED)
					ifxusb_hcd.pkt_remaining_reload=value;
			}
		}
		static ssize_t procfs_bandwidth_hs_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_hs_show(buf);
		}
		static ssize_t procfs_bandwidth_fs_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_fs_show(buf);
		}
		static ssize_t procfs_bandwidth_ls_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bandwidth_ls_show(buf);
		}
		static ssize_t procfs_bandwidth_hs_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store(value);
			return count;
		}
		static ssize_t procfs_bandwidth_fs_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store(value);
			return count;
		}
		static ssize_t procfs_bandwidth_ls_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store(value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_hs_show( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_hs_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_hs_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_hs_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_hs_store(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_hs, S_IRUGO|S_IWUSR, sysfs_bandwidth_hs_show, sysfs_bandwidth_hs_store);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_fs_show( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_fs_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_fs_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_fs_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_fs_store(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_fs, S_IRUGO|S_IWUSR, sysfs_bandwidth_fs_show, sysfs_bandwidth_fs_store);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bandwidth_ls_show( struct device *_dev,char *buf)
		#endif
		{
			return bandwidth_ls_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bandwidth_ls_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_bandwidth_ls_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			bandwidth_ls_store(value);
			return count;
		}
		DEVICE_ATTR(bandwidth_ls, S_IRUGO|S_IWUSR, sysfs_bandwidth_ls_show, sysfs_bandwidth_ls_store);
	#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
	#ifdef __IS_DUAL__
		static ssize_t pkt_count_limit_bi_show_1(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_1.pkt_count_limit_bi );
		}
		static ssize_t pkt_count_limit_bo_show_1(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_1.pkt_count_limit_bo );
		}
		static void pkt_count_limit_bi_store_1(uint32_t value)
		{
			if(value<=13)
				ifxusb_hcd_1.pkt_count_limit_bi = value;
		}
		static void pkt_count_limit_bo_store_1(uint32_t value)
		{
			if     (value<=13)
				ifxusb_hcd_1.pkt_count_limit_bo = value;
		}
		static ssize_t pkt_count_limit_bi_show_2(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_2.pkt_count_limit_bi );
		}
		static ssize_t pkt_count_limit_bo_show_2(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd_2.pkt_count_limit_bo );
		}
		static void pkt_count_limit_bi_store_2(uint32_t value)
		{
			if(value<=13)
				ifxusb_hcd_2.pkt_count_limit_bi = value;
		}
		static void pkt_count_limit_bo_store_2(uint32_t value)
		{
			if(value<=13)
				ifxusb_hcd_2.pkt_count_limit_bo = value;
		}
		static ssize_t procfs_pkt_count_limit_bi_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bi_show_1(buf);
		}
		static ssize_t procfs_pkt_count_limit_bo_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bo_show_1(buf);
		}
		static ssize_t procfs_pkt_count_limit_bi_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store_1(value);
			return count;
		}
		static ssize_t procfs_pkt_count_limit_bo_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store_1(value);
			return count;
		}
		static ssize_t procfs_pkt_count_limit_bi_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bi_show_2(buf);
		}
		static ssize_t procfs_pkt_count_limit_bo_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bo_show_2(buf);
		}
		static ssize_t procfs_pkt_count_limit_bi_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store_2(value);
			return count;
		}
		static ssize_t procfs_pkt_count_limit_bo_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store_2(value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bi_show_1( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bi_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bi_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store_1(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bi_1, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bi_show_1, sysfs_pkt_count_limit_bi_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bo_show_1( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bo_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bo_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store_1(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bo_1, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bo_show_1, sysfs_pkt_count_limit_bo_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bi_show_2( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bi_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bi_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store_2(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bi_2, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bi_show_2, sysfs_pkt_count_limit_bi_store_2);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bo_show_2( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bo_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bo_store_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store_2(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bo_2, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bo_show_2, sysfs_pkt_count_limit_bo_store_2);
	#else
		static ssize_t pkt_count_limit_bi_show(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd.pkt_count_limit_bi );
		}
		static ssize_t pkt_count_limit_bo_show(char *buf)
		{
			return sprintf( buf, "%d\n",ifxusb_hcd.pkt_count_limit_bo );
		}
		static void pkt_count_limit_bi_store(uint32_t value)
		{
			if     (value<=13)
				ifxusb_hcd.pkt_count_limit_bi = value;
		}
		static void pkt_count_limit_bo_store(uint32_t value)
		{
			if     (value<=13)
				ifxusb_hcd.pkt_count_limit_bo = value;
		}
		static ssize_t procfs_pkt_count_limit_bi_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bi_show(buf);
		}
		static ssize_t procfs_pkt_count_limit_bo_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return pkt_count_limit_bo_show(buf);
		}
		static ssize_t procfs_pkt_count_limit_bi_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store(value);
			return count;
		}
		static ssize_t procfs_pkt_count_limit_bo_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store(value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bi_show( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bi_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bi_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bi_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bi_store(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bi, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bi_show, sysfs_pkt_count_limit_bi_store);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_pkt_count_limit_bo_show( struct device *_dev,char *buf)
		#endif
		{
			return pkt_count_limit_bo_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_pkt_count_limit_bo_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		        static ssize_t sysfs_pkt_count_limit_bo_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			pkt_count_limit_bo_store(value);
			return count;
		}
		DEVICE_ATTR(pkt_count_limit_bo, S_IRUGO|S_IWUSR, sysfs_pkt_count_limit_bo_show, sysfs_pkt_count_limit_bo_store);
	#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_DUAL__
		static ssize_t buspower_show_1(char *buf)
		{
			if(ifxusb_vbus (&ifxusb_hcd_1.core_if)==1) return sprintf( buf, "1\n" );
			if(ifxusb_vbus (&ifxusb_hcd_1.core_if)==0) return sprintf( buf, "0\n" );
			return sprintf( buf, "UNKNOWN\n" );
		}
		static void buspower_store_1(uint32_t value)
		{
			if     (value==1)  ifxusb_vbus_on (&ifxusb_hcd_1.core_if);
			else if(value==0)  ifxusb_vbus_off(&ifxusb_hcd_1.core_if);
		}
		static ssize_t buspower_show_2(char *buf)
		{
			if(ifxusb_vbus (&ifxusb_hcd_2.core_if)==1) return sprintf( buf, "1\n" );
			if(ifxusb_vbus (&ifxusb_hcd_2.core_if)==0) return sprintf( buf, "0\n" );
			return sprintf( buf, "UNKNOWN\n" );
		}
		static void buspower_store_2(uint32_t value)
		{
			if     (value==1)  ifxusb_vbus_on (&ifxusb_hcd_2.core_if);
			else if(value==0)  ifxusb_vbus_off(&ifxusb_hcd_2.core_if);
		}
		static ssize_t procfs_buspower_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return buspower_show_1(buf);
		}
		static ssize_t procfs_buspower_store_1(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store_1(value);
			return count;
		}
		static ssize_t procfs_buspower_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return buspower_show_2(buf);
		}
		static ssize_t procfs_buspower_store_2(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store_2(value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_buspower_show_1( struct device *_dev,char *buf)
		#endif
		{
			return buspower_show_1(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_store_1( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		    static ssize_t sysfs_buspower_store_1( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store_1(value);
			return count;
		}
		DEVICE_ATTR(buspower_1, S_IRUGO|S_IWUSR, sysfs_buspower_show_1, sysfs_buspower_store_1);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_buspower_show_2( struct device *_dev,char *buf)
		#endif
		{
			return buspower_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_store_2( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		    static ssize_t sysfs_buspower_store_2( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store_2(value);
			return count;
		}
		DEVICE_ATTR(buspower_2, S_IRUGO|S_IWUSR, sysfs_buspower_show_2, sysfs_buspower_store_2);
	#else
		static ssize_t buspower_show(char *buf)
		{
			if(ifxusb_vbus (&ifxusb_hcd.core_if)==1) return sprintf( buf, "1\n" );
			if(ifxusb_vbus (&ifxusb_hcd.core_if)==0) return sprintf( buf, "0\n" );
			return sprintf( buf, "UNKNOWN\n" );
		}
		static void buspower_store(uint32_t value)
		{
			if     (value==1)  ifxusb_vbus_on (&ifxusb_hcd.core_if);
			else if(value==0)  ifxusb_vbus_off(&ifxusb_hcd.core_if);
		}
		static ssize_t procfs_buspower_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return buspower_show(buf);
		}
		static ssize_t procfs_buspower_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store(value);
			return count;
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_buspower_show( struct device *_dev,                               char *buf)
		#endif
		{
			return buspower_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_buspower_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
		    static ssize_t sysfs_buspower_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			buspower_store(value);
			return count;
		}
		DEVICE_ATTR(buspower, S_IRUGO|S_IWUSR, sysfs_buspower_show, sysfs_buspower_store);
	#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


	#ifdef __IS_DUAL__
		static ssize_t bussuspend_show_1(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
			return sprintf (buf, "Bus Suspend = 0x%x\n", val.b.prtsusp);
		}
		static ssize_t bussuspend_show_2(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
			return sprintf (buf, "Bus Suspend = 0x%x\n", val.b.prtsusp);
		}

		static ssize_t procfs_bussuspend_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bussuspend_show_1(buf);
		}
		static ssize_t procfs_bussuspend_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bussuspend_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bussuspend_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bussuspend_show_1( struct device *_dev,char *buf)
		#endif
		{
			return bussuspend_show_1(buf);
		}
		DEVICE_ATTR(bussuspend_1, S_IRUGO|S_IWUSR, sysfs_bussuspend_show_1, 0);
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bussuspend_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bussuspend_show_2( struct device *_dev,char *buf)
		#endif
		{
			return bussuspend_show_2(buf);
		}
		DEVICE_ATTR(bussuspend_2, S_IRUGO|S_IWUSR, sysfs_bussuspend_show_2, 0);
	#else
		static ssize_t bussuspend_show(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
			return sprintf (buf, "Bus Suspend = 0x%x\n", val.b.prtsusp);
		}
		static ssize_t procfs_bussuspend_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return bussuspend_show(buf);
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_bussuspend_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_bussuspend_show( struct device *_dev,                               char *buf)
		#endif
		{
			return bussuspend_show(buf);
		}
		DEVICE_ATTR(bussuspend, S_IRUGO|S_IWUSR, sysfs_bussuspend_show, 0);
	#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_DUAL__
		static ssize_t busconnected_show_1(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
			return sprintf (buf, "Bus Connected = 0x%x\n", val.b.prtconnsts);
		}
		static ssize_t busconnected_show_2(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
			return sprintf (buf, "Bus Connected = 0x%x\n", val.b.prtconnsts);
		}

		static ssize_t procfs_busconnected_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return busconnected_show_1(buf);
		}
		static ssize_t procfs_busconnected_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return busconnected_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_busconnected_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_busconnected_show_1( struct device *_dev,char *buf)
		#endif
		{
			return busconnected_show_1(buf);
		}
		DEVICE_ATTR(busconnected_1, S_IRUGO|S_IWUSR, sysfs_busconnected_show_1, 0);
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_busconnected_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_busconnected_show_2( struct device *_dev,char *buf)
		#endif
		{
			return busconnected_show_2(buf);
		}
		DEVICE_ATTR(busconnected_2, S_IRUGO|S_IWUSR, sysfs_busconnected_show_2, 0);
	#else
		static ssize_t busconnected_show(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
			return sprintf (buf, "Bus Connected = 0x%x\n", val.b.prtconnsts);
		}
		static ssize_t procfs_busconnected_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return busconnected_show(buf);
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_busconnected_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_busconnected_show( struct device *_dev,                               char *buf)
		#endif
		{
			return busconnected_show(buf);
		}
		DEVICE_ATTR(busconnected, S_IRUGO|S_IWUSR, sysfs_busconnected_show, 0);
	#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_DUAL__
		static ssize_t connectspeed_show_1(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_1.core_if.hprt0);
			if( val.b.prtspd ==0) return sprintf (buf, "Bus Speed = High (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==1) return sprintf (buf, "Bus Speed = Full (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==2) return sprintf (buf, "Bus Speed = Low  (%d)\n", val.b.prtspd);
			                      return sprintf (buf, "Bus Speed = Unknown (%d)\n", val.b.prtspd);
		}
		static ssize_t connectspeed_show_2(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd_2.core_if.hprt0);
			if( val.b.prtspd ==0) return sprintf (buf, "Bus Speed = High (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==1) return sprintf (buf, "Bus Speed = Full (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==2) return sprintf (buf, "Bus Speed = Low  (%d)\n", val.b.prtspd);
			                      return sprintf (buf, "Bus Speed = Unknown (%d)\n", val.b.prtspd);
		}

		static ssize_t procfs_connectspeed_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return connectspeed_show_1(buf);
		}
		static ssize_t procfs_connectspeed_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return connectspeed_show_2(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_connectspeed_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_connectspeed_show_1( struct device *_dev,char *buf)
		#endif
		{
			return connectspeed_show_1(buf);
		}
		DEVICE_ATTR(connectspeed_1, S_IRUGO|S_IWUSR, sysfs_connectspeed_show_1, 0);
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_connectspeed_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_connectspeed_show_2( struct device *_dev,char *buf)
		#endif
		{
			return connectspeed_show_2(buf);
		}
		DEVICE_ATTR(connectspeed_2, S_IRUGO|S_IWUSR, sysfs_connectspeed_show_2, 0);
	#else
		static ssize_t connectspeed_show(char *buf)
		{
			hprt0_data_t val;
			val.d32 = ifxusb_rreg(ifxusb_hcd.core_if.hprt0);
			if( val.b.prtspd ==0) return sprintf (buf, "Bus Speed = High (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==1) return sprintf (buf, "Bus Speed = Full (%d)\n", val.b.prtspd);
			if( val.b.prtspd ==2) return sprintf (buf, "Bus Speed = Low  (%d)\n", val.b.prtspd);
			                      return sprintf (buf, "Bus Speed = Unknown (%d)\n", val.b.prtspd);
		}

		static ssize_t procfs_connectspeed_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return connectspeed_show(buf);
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_connectspeed_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_connectspeed_show( struct device *_dev,                               char *buf)
		#endif
		{
			return connectspeed_show(buf);
		}
		DEVICE_ATTR(connectspeed, S_IRUGO|S_IWUSR, sysfs_connectspeed_show, 0);
	#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


#ifdef __IS_DEVICE__
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
	static ssize_t devspeed_show(char *buf)
	{
		dcfg_data_t val;
		val.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dcfg);
		if( val.b.devspd ==0) return sprintf (buf, "Dev Speed = High (%d)\n", val.b.devspd);
		if( val.b.devspd ==1) return sprintf (buf, "Dev Speed = Full (%d)\n", val.b.devspd);
		if( val.b.devspd ==3) return sprintf (buf, "Dev Speed = Full (%d)\n", val.b.devspd);
		                      return sprintf (buf, "Dev Speed = Unknown (%d)\n", val.b.devspd);
	}

	static ssize_t procfs_devspeed_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return devspeed_show(buf);
	}

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_devspeed_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_devspeed_show( struct device *_dev,                               char *buf)
	#endif
	{
		return devspeed_show(buf);
	}
	DEVICE_ATTR(devspeed, S_IRUGO|S_IWUSR, sysfs_devspeed_show, 0);

	static ssize_t enumspeed_show(char *buf)
	{
		dsts_data_t val;
		val.d32 = ifxusb_rreg(&ifxusb_pcd.core_if.dev_global_regs->dsts);
		if( val.b.enumspd ==0) return sprintf (buf, "Enum Speed = High (%d)\n", val.b.enumspd);
		if( val.b.enumspd ==1) return sprintf (buf, "Enum Speed = Full (%d)\n", val.b.enumspd);
		if( val.b.enumspd ==2) return sprintf (buf, "Enum Speed = Low  (%d)\n", val.b.enumspd);
		return sprintf (buf, "Enum Speed = invalid(%d)\n", val.b.enumspd);
	}

	static ssize_t procfs_enumspeed_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return enumspeed_show(buf);
	}

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_enumspeed_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_enumspeed_show( struct device *_dev,                               char *buf)
	#endif
	{
		return enumspeed_show(buf);
	}
	DEVICE_ATTR(enumspeed, S_IRUGO|S_IWUSR, sysfs_enumspeed_show, 0);
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#endif


//////////////////////////////////////////////////////////////////////////////////
#ifdef __ENABLE_DUMP__

	#ifdef __IS_DUAL__
		static void dump_reg_1(void)
		{
			ifxusb_dump_registers_h(&ifxusb_hcd_1.core_if);
		}
		static void dump_reg_2(void)
		{
			ifxusb_dump_registers_h(&ifxusb_hcd_2.core_if);
		}

		static ssize_t procfs_dump_reg_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_reg_1();
			return 0;
		}
		static ssize_t procfs_dump_reg_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_reg_2();
			return 0;
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_reg_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_reg_show_1( struct device *_dev,char *buf)
		#endif
		{
			dump_reg_1();
			return 0;
		}
		DEVICE_ATTR(dump_reg_h_1, S_IRUGO|S_IWUSR, sysfs_dump_reg_show_1, 0);
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_reg_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_reg_show_2( struct device *_dev,char *buf)
		#endif
		{
			dump_reg_2();
			return 0;
		}
		DEVICE_ATTR(dump_reg_h_2, S_IRUGO|S_IWUSR, sysfs_dump_reg_show_2, 0);
	#else
		static void dump_reg(void)
		{
			#ifdef __IS_HOST__
				ifxusb_dump_registers_h(&ifxusb_hcd.core_if);
			#endif
			#ifdef __IS_DEVICE__
				ifxusb_dump_registers_d(&ifxusb_pcd.core_if);
			#endif
		}
		static ssize_t procfs_dump_reg_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_reg();
			return 0;
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_reg_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_reg_show( struct device *_dev,char *buf)
		#endif
		{
			dump_reg();
			return 0;
		}
		#ifdef __IS_HOST__
		DEVICE_ATTR(dump_reg_h, S_IRUGO|S_IWUSR, sysfs_dump_reg_show, 0);
		#else
		DEVICE_ATTR(dump_reg_d, S_IRUGO|S_IWUSR, sysfs_dump_reg_show, 0);
		#endif
	#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_DUAL__
		static void dump_spram_1(void)
		{
			ifxusb_dump_spram_h(&ifxusb_hcd_1.core_if);
		}
		static void dump_spram_2(void)
		{
			ifxusb_dump_spram_h(&ifxusb_hcd_2.core_if);
		}

		static ssize_t procfs_dump_spram_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_spram_1();
			return 0;
		}
		static ssize_t procfs_dump_spram_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_spram_2();
			return 0;
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_spram_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_spram_show_1( struct device *_dev,char *buf)
		#endif
		{
			dump_spram_1();
			return 0;
		}
		DEVICE_ATTR(dump_spram_h_1, S_IRUGO|S_IWUSR, sysfs_dump_spram_show_1, 0);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_spram_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_spram_show_2( struct device *_dev,char *buf)
		#endif
		{
			dump_spram_2();
			return 0;
		}
		DEVICE_ATTR(dump_spram_h_2, S_IRUGO|S_IWUSR, sysfs_dump_spram_show_2, 0);
	#else
		static void dump_spram(void)
		{
			#ifdef __IS_HOST__
				ifxusb_dump_spram_h(&ifxusb_hcd.core_if);
			#endif
			#ifdef __IS_DEVICE__
				ifxusb_dump_spram_d(&ifxusb_pcd.core_if);
			#endif
		}
		static ssize_t procfs_dump_spram_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			dump_spram();
			return 0;
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_spram_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_spram_show( struct device *_dev,char *buf)
		#endif
		{
			dump_spram();
			return 0;
		}
		#ifdef __IS_HOST__
		DEVICE_ATTR(dump_spram_h, S_IRUGO|S_IWUSR, sysfs_dump_spram_show, 0);
		#else
		DEVICE_ATTR(dump_spram_d, S_IRUGO|S_IWUSR, sysfs_dump_spram_show, 0);
		#endif

	#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			static ssize_t procfs_dump_host_state_show_1(char *buf, char **start, off_t offset, int count, int *eof, void *data)
			{
				ifxhcd_dump_state(&ifxusb_hcd_1);
				return 0;
			}
			static ssize_t procfs_dump_host_state_show_2(char *buf, char **start, off_t offset, int count, int *eof, void *data)
			{
				ifxhcd_dump_state(&ifxusb_hcd_2);
				return 0;
			}
			#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
				static ssize_t sysfs_dump_host_state_show_1( struct device *_dev, struct device_attribute *attr,char *buf)
			#else
				static ssize_t sysfs_dump_host_state_show_1( struct device *_dev,char *buf)
			#endif
			{
				ifxhcd_dump_state(&ifxusb_hcd_1);
				return 0;
			}
			DEVICE_ATTR(dump_host_state_1, S_IRUGO|S_IWUSR, sysfs_dump_host_state_show_1, 0);
			#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
				static ssize_t sysfs_dump_host_state_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
			#else
				static ssize_t sysfs_dump_host_state_show_2( struct device *_dev,char *buf)
			#endif
			{
				ifxhcd_dump_state(&ifxusb_hcd_2);
				return 0;
			}
			DEVICE_ATTR(dump_host_state_2, S_IRUGO|S_IWUSR, sysfs_dump_host_state_show_2, 0);
		#else
			static ssize_t procfs_dump_host_state_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
			{
				ifxhcd_dump_state(&ifxusb_hcd);
				return 0;
			}
			#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
				static ssize_t sysfs_dump_host_state_show( struct device *_dev, struct device_attribute *attr,char *buf)
			#else
				static ssize_t sysfs_dump_host_state_show( struct device *_dev,char *buf)
			#endif
			{
				ifxhcd_dump_state(&ifxusb_hcd);
				return 0;
			}
			DEVICE_ATTR(dump_host_state, S_IRUGO|S_IWUSR, sysfs_dump_host_state_show, 0);
		#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#endif //IS_HOST_

#endif //__ENABLE_DUMP__
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_HOST__
	static void host_probe(unsigned long _ptr)
	{
		ifxhcd_hcd_t *ifxhcd = (ifxhcd_hcd_t *)_ptr;

		if(ifxhcd->flags.b.port_connect_status)
		{
			del_timer(&ifxhcd->host_probe_timer);
			del_timer(&ifxhcd->autoprobe_timer);
			ifxhcd->power_status = 0;
		}
		else
		{
			del_timer(&ifxhcd->autoprobe_timer);
			ifxhcd->autoprobe_timer.expires = jiffies + (HZ*ifxhcd->autoprobe_sec);
			add_timer(&ifxhcd->autoprobe_timer);
			ifxhcd->power_status = 2;
			del_timer(&ifxhcd->host_probe_timer);
			do_suspend_h(&ifxhcd->core_if);
		}
	}

	static void host_autoprobe(unsigned long _ptr)
	{
		ifxhcd_hcd_t *ifxhcd = (ifxhcd_hcd_t *)_ptr;
		del_timer(&ifxhcd->host_probe_timer);
		ifxhcd->host_probe_timer.function = host_probe;
		ifxhcd->host_probe_timer.expires = jiffies + (HZ*ifxhcd->probe_sec);
		ifxhcd->host_probe_timer.data = (unsigned long)ifxhcd;
		add_timer(&ifxhcd->host_probe_timer);
		do_resume_h(&ifxhcd->core_if);
	}

	static void suspend_host_store(ifxhcd_hcd_t *ifxhcd , uint32_t value)
	{
		if(value==2)
		{
			del_timer(&ifxhcd->autoprobe_timer);
			ifxhcd->autoprobe_timer.function = host_autoprobe;
			ifxhcd->autoprobe_timer.expires = jiffies + (HZ*ifxhcd->autoprobe_sec);
			ifxhcd->autoprobe_timer.data = (unsigned long)ifxhcd;
			add_timer(&ifxhcd->autoprobe_timer);
			ifxhcd->power_status = 2;
		}
		else if(value==1)
		{
			do_suspend_h(&ifxhcd->core_if);
			ifxhcd->power_status = 1;
			del_timer(&ifxhcd->host_probe_timer);
			del_timer(&ifxhcd->autoprobe_timer);
		}
		else if(value==0)
		{
			do_resume_h(&ifxhcd->core_if);
			ifxhcd->power_status = 0;
			del_timer(&ifxhcd->host_probe_timer);
			del_timer(&ifxhcd->autoprobe_timer);
		}
	}
	#ifdef __IS_DUAL__
		static ssize_t procfs_suspend_host_2_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd_2,value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_suspend_host_2_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_suspend_host_2_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd_2,value);
			return count;
		}

		static ssize_t procfs_suspend_host_1_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd_1,value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_suspend_host_1_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_suspend_host_1_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd_1,value);
			return count;
		}
		DEVICE_ATTR(suspend_host_2, S_IWUSR,NULL, sysfs_suspend_host_2_store);
		DEVICE_ATTR(suspend_host_1, S_IWUSR,NULL, sysfs_suspend_host_1_store);
/////////////////////////////////////////////////////////////////////////////////////////////////////
	#else
		static ssize_t procfs_suspend_host_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd,value);
			return count;
		}

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_suspend_host_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_suspend_host_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			suspend_host_store(&ifxusb_hcd,value);
			return count;
		}
		DEVICE_ATTR(suspend_host, S_IWUSR,NULL, sysfs_suspend_host_store);
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_HOST__
	static void probe_host_store(ifxhcd_hcd_t *ifxhcd, uint32_t value)
	{
		if(ifxhcd->power_status == 1)
		{
			del_timer(&ifxhcd->host_probe_timer);
			ifxhcd->host_probe_timer.function = host_probe;
			ifxhcd->host_probe_timer.expires = jiffies + (HZ*ifxhcd->probe_sec);
			ifxhcd->host_probe_timer.data = (unsigned long) ifxhcd;
			add_timer(&ifxhcd->host_probe_timer);
			do_resume_h(&ifxhcd->core_if);
		}
	}
	#ifdef __IS_DUAL__
		static ssize_t probe_host_2_show(char *buf)
		{
			if(ifxusb_hcd_2.power_status == 0)
				return sprintf (buf,"Host 2 power status is ON\n");
			else if(ifxusb_hcd_2.power_status == 1)
				return sprintf (buf,"Host 2 power status is Suspend\n");
			else
				return sprintf (buf,"Host 2 power status is Auto-probing\n");
		}
		static ssize_t probe_host_1_show(char *buf)
		{
			if(ifxusb_hcd_1.power_status == 0)
				return sprintf (buf,"Host 1 power status is ON\n");
			else if(ifxusb_hcd_1.power_status == 1)
				return sprintf (buf,"Host 1 power status is Suspend\n");
			else
				return sprintf (buf,"Host 1 power status is Auto-probing\n");
		}
		static ssize_t procfs_probe_host_2_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd_2,value);
			return count;
		}
		static ssize_t procfs_probe_host_2_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_host_2_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_2_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_host_2_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_host_2_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_2_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_host_2_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd_2,value);
			return count;
		}

		static ssize_t procfs_probe_host_1_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd_1,value);
			return count;
		}
		static ssize_t procfs_probe_host_1_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_host_1_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_1_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_host_1_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_host_1_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_1_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_host_1_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd_1,value);
			return count;
		}
		DEVICE_ATTR(probe_host_2, S_IRUGO|S_IWUSR, sysfs_probe_host_2_show, sysfs_probe_host_2_store);
		DEVICE_ATTR(probe_host_1, S_IRUGO|S_IWUSR, sysfs_probe_host_1_show, sysfs_probe_host_1_store);
/////////////////////////////////////////////////////////////////////////////////////////////////////
	#else
		static ssize_t probe_host_show(char *buf)
		{
			if(ifxusb_hcd.power_status == 0)
				return sprintf (buf,"Host power status is ON\n");
			else if(ifxusb_hcd.power_status == 1)
				return sprintf (buf,"Host power status is Suspend\n");
			else
				return sprintf (buf,"Host power status is Auto-probing\n");
		}
		static ssize_t procfs_probe_host_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd,value);
			return count;
		}
		static ssize_t procfs_probe_host_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_host_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_host_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_host_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_host_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_host_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			probe_host_store(&ifxusb_hcd,value);
			return count;
		}
		DEVICE_ATTR(probe_host, S_IRUGO|S_IWUSR, sysfs_probe_host_show, sysfs_probe_host_store);
	#endif
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_DEVICE__
	static void device_probe(unsigned long _ptr)
	{
		if(ifxusb_pcd.power_status == 2)
		{
			del_timer(&ifxusb_pcd.device_autoprobe_timer);
			ifxusb_pcd.device_autoprobe_timer.expires = jiffies + (HZ*ifxusb_pcd.autoprobe_sec);
			add_timer(&ifxusb_pcd.device_autoprobe_timer);
			ifxusb_pcd.power_status = 2;
			do_suspend_d(&ifxusb_pcd.core_if);
		}
		else if(ifxusb_pcd.power_status == 1)
		{
			do_suspend_d(&ifxusb_pcd.core_if);
			ifxusb_pcd.power_status = 1;
		}
	}
	static void device_autoprobe(unsigned long _ptr)
	{
		init_timer(&ifxusb_pcd.device_probe_timer);
		ifxusb_pcd.device_probe_timer.function = device_probe;
		ifxusb_pcd.device_probe_timer.expires = jiffies + (HZ*ifxusb_pcd.probe_sec);
		add_timer(&ifxusb_pcd.device_probe_timer);
		do_resume_d(&ifxusb_pcd.core_if);
	}
	static void suspend_device_store(uint32_t value)
	{
		if(value==2)
		{
			del_timer(&ifxusb_pcd.device_autoprobe_timer);
			ifxusb_pcd.device_autoprobe_timer.function = device_autoprobe;
			ifxusb_pcd.device_autoprobe_timer.expires = jiffies + (HZ*ifxusb_pcd.autoprobe_sec);
			add_timer(&ifxusb_pcd.device_autoprobe_timer);
			ifxusb_pcd.power_status = 2;
		}
		else if(value==1)
		{
			do_suspend_d(&ifxusb_pcd.core_if);
			ifxusb_pcd.power_status = 1;
			del_timer(&ifxusb_pcd.device_autoprobe_timer);
			del_timer(&ifxusb_pcd.device_probe_timer);
		}
		else if(value==0)
		{
			do_resume_d(&ifxusb_pcd.core_if);
			ifxusb_pcd.power_status = 0;
			del_timer(&ifxusb_pcd.device_autoprobe_timer);
			del_timer(&ifxusb_pcd.device_probe_timer);
		}
	}
	static ssize_t procfs_suspend_device_store(struct file *file, const char *buffer, unsigned long count, void *data)
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		suspend_device_store(value);
		return count;
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_suspend_device_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
	#else
		static ssize_t sysfs_suspend_device_store( struct device *_dev,                               const char *buffer, size_t count )
	#endif
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		suspend_device_store(value);
		return count;
	}
	DEVICE_ATTR(suspend_device, S_IWUSR,NULL,sysfs_suspend_device_store);
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_DEVICE__
	static ssize_t probe_device_show(char *buf)
	{
		if(ifxusb_pcd.power_status == 0)
			return sprintf (buf,"Device power status is ON\n");
		else if(ifxusb_pcd.power_status == 1)
			return sprintf (buf,"Device power status is Suspend\n");
		else
			return printk(buf,"Device power status is Auto-probing\n");
	}
	static void probe_device_store(uint32_t value)
	{

		if(ifxusb_pcd.power_status == 1)
		{
			del_timer(&ifxusb_pcd.device_probe_timer);
			ifxusb_pcd.device_probe_timer.function = device_probe;
			ifxusb_pcd.device_probe_timer.expires = jiffies + (HZ*ifxusb_pcd.probe_sec);
			add_timer(&ifxusb_pcd.device_probe_timer);
			do_resume_d(&ifxusb_pcd.core_if);
		}
	}
	static ssize_t procfs_probe_device_store(struct file *file, const char *buffer, unsigned long count, void *data)
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		probe_device_store(value);
		return count;
	}
	static ssize_t procfs_probe_device_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return probe_device_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_probe_device_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_probe_device_show( struct device *_dev,                               char *buf)
	#endif
	{
		return probe_device_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_probe_device_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
	#else
		static ssize_t sysfs_probe_device_store( struct device *_dev,                               const char *buffer, size_t count )
	#endif
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		probe_device_store(value);
		return count;
	}
	DEVICE_ATTR(probe_device, S_IRUGO|S_IWUSR, sysfs_probe_device_show, sysfs_probe_device_store);
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		static ssize_t autoprobe_timer2_val_show(char *buf)
		{
			return sprintf (buf,"Host 2 auto-probe timer is %d second\n",ifxusb_hcd_2.autoprobe_sec);
		}
		static ssize_t procfs_autoprobe_timer2_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd_2.autoprobe_sec = value;
			return count;
		}
		static ssize_t procfs_autoprobe_timer2_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return autoprobe_timer2_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer2_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_autoprobe_timer2_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return autoprobe_timer2_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer2_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_autoprobe_timer2_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd_2.autoprobe_sec = value;
			return count;
		}

		static ssize_t autoprobe_timer1_val_show(char *buf)
		{
			return sprintf (buf,"Host 1 auto-probe timer is %d second\n",ifxusb_hcd_1.autoprobe_sec);
		}
		static ssize_t procfs_autoprobe_timer1_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd_1.autoprobe_sec = value;
			return count;
		}
		static ssize_t procfs_autoprobe_timer1_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return autoprobe_timer1_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer1_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_autoprobe_timer1_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return autoprobe_timer1_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer1_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_autoautoprobe_timer1_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd_1.autoprobe_sec = value;
			return count;
		}

		static ssize_t probe_timer2_val_show(char *buf)
		{
			return sprintf (buf,"Host 2 probe timer is %d second\n",ifxusb_hcd_2.probe_sec);
		}
		static ssize_t procfs_probe_timer2_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd_2.probe_sec = value;
			return count;
		}
		static ssize_t procfs_probe_timer2_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_timer2_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer2_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_timer2_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_timer2_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer2_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_timer2_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd_2.probe_sec = value;
			return count;
		}

		static ssize_t probe_timer1_val_show(char *buf)
		{
			return sprintf (buf,"Host 1 probe timer is %d second\n",ifxusb_hcd_1.probe_sec);
		}
		static ssize_t procfs_probe_timer1_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd_1.probe_sec = value;
			return count;
		}
		static ssize_t procfs_probe_timer1_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_timer1_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer1_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_timer1_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_timer1_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer1_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_timer1_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd_1.probe_sec = value;
			return count;
		}
		DEVICE_ATTR(probe_timer1_val_h, S_IRUGO|S_IWUSR, sysfs_probe_timer1_val_show, sysfs_probe_timer1_val_store);
		DEVICE_ATTR(probe_timer2_val_h, S_IRUGO|S_IWUSR, sysfs_probe_timer2_val_show, sysfs_probe_timer2_val_store);
		DEVICE_ATTR(autoprobe_timer1_val_h, S_IRUGO|S_IWUSR, sysfs_autoprobe_timer1_val_show, sysfs_autoprobe_timer1_val_store);
		DEVICE_ATTR(autoprobe_timer2_val_h, S_IRUGO|S_IWUSR, sysfs_autoprobe_timer2_val_show, sysfs_autoprobe_timer2_val_store);
	#else
		static ssize_t autoprobe_timer_val_show(char *buf)
		{
			return sprintf (buf,"Host auto-probe timer is %d second\n",ifxusb_hcd.autoprobe_sec);
		}
		static ssize_t procfs_autoprobe_timer_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd.autoprobe_sec = value;
			return count;
		}
		static ssize_t procfs_autoprobe_timer_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return autoprobe_timer_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_autoprobe_timer_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return autoprobe_timer_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_autoprobe_timer_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_autoautoprobe_timer_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 300))
				ifxusb_hcd.autoprobe_sec = value;
			return count;
		}
		static ssize_t probe_timer_val_show(char *buf)
		{
			return sprintf (buf,"Host probe timer is %d second\n",ifxusb_hcd.probe_sec);
		}
		static ssize_t procfs_probe_timer_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd.probe_sec = value;
			return count;
		}
		static ssize_t procfs_probe_timer_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
		{
			return probe_timer_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_probe_timer_val_show( struct device *_dev,                               char *buf)
		#endif
		{
			return probe_timer_val_show(buf);
		}
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_probe_timer_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
		#else
			static ssize_t sysfs_probe_timer_val_store( struct device *_dev,                               const char *buffer, size_t count )
		#endif
		{
			char buf[10];
			int i = 0;
			uint32_t value;
			if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
				return -EFAULT;
			value = simple_strtoul(buf, NULL, 10);
			if((value > 0)&&(value < 10))
				ifxusb_hcd.probe_sec = value;
			return count;
		}
		DEVICE_ATTR(probe_timer_val_h, S_IRUGO|S_IWUSR, sysfs_probe_timer_val_show, sysfs_probe_timer_val_store);
		DEVICE_ATTR(autoprobe_timer_val_h, S_IRUGO|S_IWUSR, sysfs_autoprobe_timer_val_show, sysfs_autoprobe_timer_val_store);
	#endif
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IS_DEVICE__
	static ssize_t autoprobe_timer_val_show(char *buf)
	{
		return sprintf (buf,"Device auto-probe timer is %d second\n",ifxusb_pcd.autoprobe_sec);
	}
	static ssize_t procfs_autoprobe_timer_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		if((value > 0)&&(value < 300))
			ifxusb_pcd.autoprobe_sec = value;
		return count;
	}
	static ssize_t procfs_autoprobe_timer_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return autoprobe_timer_val_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_autoprobe_timer_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_autoprobe_timer_val_show( struct device *_dev,                               char *buf)
	#endif
	{
		return autoprobe_timer_val_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_autoprobe_timer_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
	#else
		static ssize_t sysfs_autoautoprobe_timer_val_store( struct device *_dev,                               const char *buffer, size_t count )
	#endif
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		if((value > 0)&&(value < 300))
			ifxusb_pcd.autoprobe_sec = value;
		return count;
	}
	static ssize_t probe_timer_val_show(char *buf)
	{
		return sprintf (buf,"Device probe timer is %d second\n",ifxusb_pcd.probe_sec);
	}
	static ssize_t procfs_probe_timer_val_store(struct file *file, const char *buffer, unsigned long count, void *data)
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		if((value > 0)&&(value < 10))
			ifxusb_pcd.probe_sec = value;
		return count;
	}
	static ssize_t procfs_probe_timer_val_show(char *buf, char **start, off_t offset, int count, int *eof, void *data)
	{
		return probe_timer_val_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_probe_timer_val_show( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_probe_timer_val_show( struct device *_dev,                               char *buf)
	#endif
	{
		return probe_timer_val_show(buf);
	}
	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_probe_timer_val_store( struct device *_dev, struct device_attribute *attr,const char *buffer, size_t count )
	#else
		static ssize_t sysfs_probe_timer_val_store( struct device *_dev,                               const char *buffer, size_t count )
	#endif
	{
		char buf[10];
		int i = 0;
		uint32_t value;
		if (copy_from_user(buf, &buffer[i], sizeof("0xFFFFFFFF\n")+1))
			return -EFAULT;
		value = simple_strtoul(buf, NULL, 10);
		if((value > 0)&&(value < 10))
			ifxusb_pcd.probe_sec = value;
		return count;
	}
	DEVICE_ATTR(probe_timer_val_d, S_IRUGO|S_IWUSR, sysfs_probe_timer_val_show, sysfs_probe_timer_val_store);
	DEVICE_ATTR(autoprobe_timer_val_d, S_IRUGO|S_IWUSR, sysfs_autoprobe_timer_val_show, sysfs_autoprobe_timer_val_store);
#endif
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

static int  ifx_proc_addproc(char *funcname, read_proc_t *hookfuncr, write_proc_t *hookfuncw);
static void ifx_proc_delproc(char *funcname);

//////////////////////////////////////////////////////////////////////////////////

#if defined(__IS_HOST__) && defined(__HOST_COC__)
	#ifdef __IS_DUAL__
		static IFX_PMCU_MODULE_DEP_t depListUSBHost_1= 
		{ 
		     1,
		     {
		         {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
		     }
		};
		static IFX_PMCU_MODULE_DEP_t depListUSBHost_2= 
		{ 
		     1,
		     {
		         {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
		     }
		};
		// This functions returns the current power state of the module 
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateGet_1(IFX_PMCU_STATE_t *pmcuModState) {
		    printk(KERN_DEBUG "ifx_usbhost_stateGet_1 is called\n");
		    if(ifxusb_hcd_1.power_status == 0){
		        printk(KERN_DEBUG "current power state of USB Host #1 is D0\n");
		        *pmcuModState = IFX_PMCU_STATE_D0; // set here the right value
		    }
		    else if(ifxusb_hcd_1.power_status == 1){
		        printk(KERN_DEBUG "current power state of USB Host #1 is D3 (Suspend)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else if(ifxusb_hcd_1.power_status == 2){
		        printk(KERN_DEBUG "current power state of USB Host #1 is D3 (Auto-Probing)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else{
		        printk(KERN_DEBUG "current power state of USB Host #1 is unknown (%d)\n",ifxusb_hcd_1.power_status);
		        *pmcuModState = IFX_PMCU_STATE_INVALID; // must be set to INVALID
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateGet_2(IFX_PMCU_STATE_t *pmcuModState) {
		    printk(KERN_DEBUG "ifx_usbhost_stateGet_2 is called\n");
		    if(ifxusb_hcd_2.power_status == 0){
		        printk(KERN_DEBUG "current power state of USB Host #2 is D0\n");
		        *pmcuModState = IFX_PMCU_STATE_D0; // set here the right value
		    }
		    else if(ifxusb_hcd_2.power_status == 1){
		        printk(KERN_DEBUG "current power state of USB Host #2 is D3 (Suspend)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else if(ifxusb_hcd_2.power_status == 2){
		        printk(KERN_DEBUG "current power state of USB Host #2 is D3 (Auto-Probing)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else{
		        printk(KERN_DEBUG "current power state of USB Host #2 is unknown (%d)\n",ifxusb_hcd_2.power_status);
		        *pmcuModState = IFX_PMCU_STATE_INVALID; // must be set to INVALID
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		    
		
		// The function should be used to enable/disable the module specific power saving methods
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_pwrFeatureSwitch_1(IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna)
		{
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_ON) {
			    suspend_host_store(&ifxusb_hcd_1, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_OFF) {
			    suspend_host_store(&ifxusb_hcd_1, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_pwrFeatureSwitch_2(IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna)
		{
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_ON) {
			    suspend_host_store(&ifxusb_hcd_2, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_OFF) {
			    suspend_host_store(&ifxusb_hcd_2, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary clean-up's before a the real
		// power state change is initiated; e.g. flush all serial buffers inside the UART  before 
		// the frequency will be changed. 
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_preChange_1(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState) 
		{ 
		    printk(KERN_DEBUG "ifx_usbhost_preChange_1 is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_preChange_2(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState) 
		{ 
		    printk(KERN_DEBUG "ifx_usbhost_preChange_2 is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		
		// This function initiate the real power state change. The module should do all the necessary
		//   adpations to the new state.
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateChange_1(IFX_PMCU_STATE_t newState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_stateChange_1 is called\n");
		    if (newState == IFX_PMCU_STATE_D0) {
			    suspend_host_store(&ifxusb_hcd_1, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D1) {
			    suspend_host_store(&ifxusb_hcd_1, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D2) {
			    suspend_host_store(&ifxusb_hcd_1, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D3) {
			    suspend_host_store(&ifxusb_hcd_1, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateChange_2(IFX_PMCU_STATE_t newState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_stateChange_2 is called\n");
		    if (newState == IFX_PMCU_STATE_D0) {
			    suspend_host_store(&ifxusb_hcd_2, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D1) {
			    suspend_host_store(&ifxusb_hcd_2, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D2) {
			    suspend_host_store(&ifxusb_hcd_2, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D3) {
			    suspend_host_store(&ifxusb_hcd_2, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary post processing after a the real
		//   power state change was initiated.
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_postChange_1(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_postChange_1 is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_postChange_2(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_postChange_2 is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
	#else
		static IFX_PMCU_MODULE_DEP_t depListUSBHost= 
		{ 
		     1,
		     {
		         {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
		     }
		};
		// This functions returns the current power state of the module 
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateGet(IFX_PMCU_STATE_t *pmcuModState) {
		    printk(KERN_DEBUG "ifx_usbhost_stateGet is called\n");
		    if(ifxusb_hcd.power_status == 0){
		        printk(KERN_DEBUG "current power state of USB Host is D0\n");
		        *pmcuModState = IFX_PMCU_STATE_D0; // set here the right value
		    }
		    else if(ifxusb_hcd.power_status == 1){
		        printk(KERN_DEBUG "current power state of USB Host is D3 (Suspend)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else if(ifxusb_hcd.power_status == 2){
		        printk(KERN_DEBUG "current power state of USB Host is D3 (Auto-Probing)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else{
		        printk(KERN_DEBUG "current power state of USB Host is unknown (%d)\n",ifxusb_hcd.power_status);
		        *pmcuModState = IFX_PMCU_STATE_INVALID; // must be set to INVALID
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		// The function should be used to enable/disable the module specific power saving methods
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_pwrFeatureSwitch(IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna)
		{
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_ON) {
			    suspend_host_store(&ifxusb_hcd, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_OFF) {
			    suspend_host_store(&ifxusb_hcd, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary clean-up's before a the real
		// power state change is initiated; e.g. flush all serial buffers inside the UART  before 
		// the frequency will be changed. 
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_preChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState) 
		{ 
		    printk(KERN_DEBUG "ifx_usbhost_preChange is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		
		// This function initiate the real power state change. The module should do all the necessary
		//   adpations to the new state.
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_stateChange(IFX_PMCU_STATE_t newState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_stateChange is called\n");
		    if (newState == IFX_PMCU_STATE_D0) {
			    suspend_host_store(&ifxusb_hcd, 0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D1) {
			    suspend_host_store(&ifxusb_hcd, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D2) {
			    suspend_host_store(&ifxusb_hcd, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D3) {
			    suspend_host_store(&ifxusb_hcd, 1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary post processing after a the real
		//   power state change was initiated.
		static IFX_PMCU_RETURN_t 
		ifx_usbhost_postChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
		{
		    printk(KERN_DEBUG "ifx_usbhost_postChange is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
	#endif
#endif
#if defined(__IS_DEVICE__) && defined(__GADGET_COC__)
	static IFX_PMCU_MODULE_DEP_t depListUSBGadget= 
	{ 
	     1,
	     {
	         {IFX_PMCU_MODULE_CPU, IFX_PMCU_STATE_D0, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3, IFX_PMCU_STATE_D0D3}
	     }
	};
		// This functions returns the current power state of the module 
		static IFX_PMCU_RETURN_t 
		ifx_usbgadget_stateGet(IFX_PMCU_STATE_t *pmcuModState) {
		    printk(KERN_DEBUG "ifx_usbgadget_stateGet is called\n");
		    if(ifxusb_pcd.power_status == 0){
		        printk(KERN_DEBUG "current power state of USB Gadget is D0\n");
		        *pmcuModState = IFX_PMCU_STATE_D0; // set here the right value
		    }
		    else if(ifxusb_pcd.power_status == 1){
		        printk(KERN_DEBUG "current power state of USB Gadget is D3 (Suspend)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else if(ifxusb_pcd.power_status == 2){
		        printk(KERN_DEBUG "current power state of USB Gadget is D3 (Auto-Probing)\n");
		        *pmcuModState = IFX_PMCU_STATE_D3; // set here the right value
		    }
		    else{
		        printk(KERN_DEBUG "current power state of USB Gadget is unknown (%d)\n",ifxusb_pcd.power_status);
		        *pmcuModState = IFX_PMCU_STATE_INVALID; // must be set to INVALID
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		// The function should be used to enable/disable the module specific power saving methods
		static IFX_PMCU_RETURN_t 
		ifx_usbgadget_pwrFeatureSwitch(IFX_PMCU_PWR_STATE_ENA_t pmcuPwrStateEna)
		{
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_ON) {
			    suspend_device_store(0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (pmcuPwrStateEna == IFX_PMCU_PWR_STATE_OFF) {
			    suspend_device_store(1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary clean-up's before a the real
		// power state change is initiated; e.g. flush all serial buffers inside the UART  before 
		// the frequency will be changed. 
		static IFX_PMCU_RETURN_t 
		ifx_usbgadget_preChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState) 
		{ 
		    printk(KERN_DEBUG "ifx_usbgadget_preChange is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		
		// This function initiate the real power state change. The module should do all the necessary
		//   adpations to the new state.
		static IFX_PMCU_RETURN_t 
		ifx_usbgadget_stateChange(IFX_PMCU_STATE_t newState)
		{
		    printk(KERN_DEBUG "ifx_usbgadget_stateChange is called\n");
		    if (newState == IFX_PMCU_STATE_D0) {
			    suspend_device_store(0);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D1) {
			    suspend_device_store(1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D2) {
			    suspend_device_store(1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    if (newState == IFX_PMCU_STATE_D3) {
			    suspend_device_store(1);
		        return IFX_PMCU_RETURN_SUCCESS;
		    }
		    return IFX_PMCU_RETURN_SUCCESS;
		}
		
		// This function should be used to do all the necessary post processing after a the real
		//   power state change was initiated.
		static IFX_PMCU_RETURN_t 
		ifx_usbgadget_postChange(IFX_PMCU_MODULE_t pmcuModule, IFX_PMCU_STATE_t newState, IFX_PMCU_STATE_t oldState)
		{
		    printk(KERN_DEBUG "ifx_usbgadget_postChange is called\n");
		    return IFX_PMCU_RETURN_SUCCESS;
		}
#endif


/*!
  \brief This function create the sysfs and procfs entries
  \param[in] _dev Pointer of device structure, if applied
 */
#ifdef __IS_HOST__
void ifxusb_attr_create_h (void *_dev)
#else
void ifxusb_attr_create_d (void *_dev)
#endif
{
	int error;

	struct device *dev = (struct device *) _dev;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );

	error = ifx_proc_addproc("dbglevel", procfs_dbglevel_show, procfs_dbglevel_store);
	#ifdef __IS_HOST__
	error = device_create_file(dev, &dev_attr_dbglevel_h);
	#else
	error = device_create_file(dev, &dev_attr_dbglevel_d);
	#endif

	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("dump_params_1", procfs_dump_params_show_1, NULL);
			error = ifx_proc_addproc("dump_params_2", procfs_dump_params_show_2, NULL);
			error = device_create_file(dev, &dev_attr_dump_params_h_1);
			error = device_create_file(dev, &dev_attr_dump_params_h_2);

			error = ifx_proc_addproc("mode_1", procfs_mode_show_1, NULL);
			error = ifx_proc_addproc("mode_2", procfs_mode_show_2, NULL);
			error = device_create_file(dev, &dev_attr_mode_h_1);
			error = device_create_file(dev, &dev_attr_mode_h_2);
		#else
			error = ifx_proc_addproc("dump_params", procfs_dump_params_show, NULL);
			error = device_create_file(dev, &dev_attr_dump_params_h);
			error = ifx_proc_addproc("mode", procfs_mode_show, NULL);
			error = device_create_file(dev, &dev_attr_mode_h);
		#endif
	#else
		error = ifx_proc_addproc("dump_params", procfs_dump_params_show, NULL);
		error = device_create_file(dev, &dev_attr_dump_params_d);

		error = ifx_proc_addproc("mode", procfs_mode_show, NULL);
		error = device_create_file(dev, &dev_attr_mode_d);
	#endif

	#ifdef __IS_HOST__
		error = ifx_proc_addproc("version", procfs_version_show, NULL);
		error = device_create_file(dev, &dev_attr_version_h);
	#else
		error = ifx_proc_addproc("version", procfs_version_show, NULL);
		error = device_create_file(dev, &dev_attr_version_d);
	#endif


	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("pkt_count_limit_bi_1", procfs_pkt_count_limit_bi_show_1, procfs_pkt_count_limit_bi_store_1);
			error = ifx_proc_addproc("pkt_count_limit_bo_1", procfs_pkt_count_limit_bo_show_1, procfs_pkt_count_limit_bo_store_1);
			error = ifx_proc_addproc("pkt_count_limit_bi_2", procfs_pkt_count_limit_bi_show_2, procfs_pkt_count_limit_bi_store_2);
			error = ifx_proc_addproc("pkt_count_limit_bo_2", procfs_pkt_count_limit_bo_show_2, procfs_pkt_count_limit_bo_store_2);
			error = ifx_proc_addproc("bandwidth_hs_1", procfs_bandwidth_hs_show_1, procfs_bandwidth_hs_store_1);
			error = ifx_proc_addproc("bandwidth_fs_1", procfs_bandwidth_fs_show_1, procfs_bandwidth_fs_store_1);
			error = ifx_proc_addproc("bandwidth_ls_1", procfs_bandwidth_ls_show_1, procfs_bandwidth_ls_store_1);
			error = ifx_proc_addproc("bandwidth_hs_2", procfs_bandwidth_hs_show_2, procfs_bandwidth_hs_store_2);
			error = ifx_proc_addproc("bandwidth_fs_2", procfs_bandwidth_fs_show_2, procfs_bandwidth_fs_store_2);
			error = ifx_proc_addproc("bandwidth_ls_2", procfs_bandwidth_ls_show_2, procfs_bandwidth_ls_store_2);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bi_1);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bo_1);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bi_2);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bo_2);
			error = device_create_file(dev, &dev_attr_bandwidth_hs_1);
			error = device_create_file(dev, &dev_attr_bandwidth_fs_1);
			error = device_create_file(dev, &dev_attr_bandwidth_ls_1);
			error = device_create_file(dev, &dev_attr_bandwidth_hs_2);
			error = device_create_file(dev, &dev_attr_bandwidth_fs_2);
			error = device_create_file(dev, &dev_attr_bandwidth_ls_2);
		#else
			error = ifx_proc_addproc("pkt_count_limit_bi", procfs_pkt_count_limit_bi_show, procfs_pkt_count_limit_bi_store);
			error = ifx_proc_addproc("pkt_count_limit_bo", procfs_pkt_count_limit_bo_show, procfs_pkt_count_limit_bo_store);
			error = ifx_proc_addproc("bandwidth_hs", procfs_bandwidth_hs_show, procfs_bandwidth_hs_store);
			error = ifx_proc_addproc("bandwidth_fs", procfs_bandwidth_fs_show, procfs_bandwidth_fs_store);
			error = ifx_proc_addproc("bandwidth_ls", procfs_bandwidth_ls_show, procfs_bandwidth_ls_store);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bi);
			error = device_create_file(dev, &dev_attr_pkt_count_limit_bo);
			error = device_create_file(dev, &dev_attr_bandwidth_hs);
			error = device_create_file(dev, &dev_attr_bandwidth_fs);
			error = device_create_file(dev, &dev_attr_bandwidth_ls);
		#endif

		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("buspower_1", procfs_buspower_show_1, procfs_buspower_store_1);
			error = ifx_proc_addproc("buspower_2", procfs_buspower_show_2, procfs_buspower_store_2);
			error = device_create_file(dev, &dev_attr_buspower_1);
			error = device_create_file(dev, &dev_attr_buspower_2);
		#else
			error = ifx_proc_addproc("buspower", procfs_buspower_show, procfs_buspower_store);
			error = device_create_file(dev, &dev_attr_buspower);
		#endif

		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("bussuspend_1", procfs_bussuspend_show_1, NULL);
			error = ifx_proc_addproc("bussuspend_2", procfs_bussuspend_show_2, NULL);
			error = device_create_file(dev, &dev_attr_bussuspend_1);
			error = device_create_file(dev, &dev_attr_bussuspend_2);
		#else
			error = ifx_proc_addproc("bussuspend", procfs_bussuspend_show, NULL);
			error = device_create_file(dev, &dev_attr_bussuspend);
		#endif

		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("busconnected_1", procfs_busconnected_show_1, NULL);
			error = ifx_proc_addproc("busconnected_2", procfs_busconnected_show_2, NULL);
			error = device_create_file(dev, &dev_attr_busconnected_1);
			error = device_create_file(dev, &dev_attr_busconnected_2);
		#else
			error = ifx_proc_addproc("busconnected", procfs_busconnected_show, NULL);
			error = device_create_file(dev, &dev_attr_busconnected);
		#endif

		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("connectspeed_1", procfs_connectspeed_show_1, NULL);
			error = ifx_proc_addproc("connectspeed_2", procfs_connectspeed_show_2, NULL);
			error = device_create_file(dev, &dev_attr_connectspeed_1);
			error = device_create_file(dev, &dev_attr_connectspeed_2);
		#else
			error = ifx_proc_addproc("connectspeed", procfs_connectspeed_show, NULL);
			error = device_create_file(dev, &dev_attr_connectspeed);
		#endif
	#endif

	#ifdef __IS_DEVICE__
		error = ifx_proc_addproc("devspeed", procfs_devspeed_show, NULL);
		error = device_create_file(dev, &dev_attr_devspeed);
		error = ifx_proc_addproc("enumspeed", procfs_enumspeed_show, NULL);
		error = device_create_file(dev, &dev_attr_enumspeed);
	#endif

	//////////////////////////////////////////////////////
	#ifdef __ENABLE_DUMP__

		#ifdef __IS_HOST__
			#ifdef __IS_DUAL__
				error = ifx_proc_addproc("dump_reg_1", procfs_dump_reg_show_1, NULL);
				error = ifx_proc_addproc("dump_reg_2", procfs_dump_reg_show_2, NULL);
				error = device_create_file(dev, &dev_attr_dump_reg_h_1);
				error = device_create_file(dev, &dev_attr_dump_reg_h_2);
			#else
				error = ifx_proc_addproc("dump_reg", procfs_dump_reg_show, NULL);
				error = device_create_file(dev, &dev_attr_dump_reg_h);
			#endif

			#ifdef __IS_DUAL__
				error = ifx_proc_addproc("dump_spram_1", procfs_dump_spram_show_1, NULL);
				error = ifx_proc_addproc("dump_spram_2", procfs_dump_spram_show_2, NULL);
				error = device_create_file(dev, &dev_attr_dump_spram_h_1);
				error = device_create_file(dev, &dev_attr_dump_spram_h_2);
			#else
				error = ifx_proc_addproc("dump_spram", procfs_dump_spram_show, NULL);
				error = device_create_file(dev, &dev_attr_dump_spram_h);
			#endif

			#ifdef __IS_DUAL__
				error = ifx_proc_addproc("dump_host_state_1", procfs_dump_host_state_show_1, NULL);
				error = ifx_proc_addproc("dump_host_state_2", procfs_dump_host_state_show_2, NULL);
				error = device_create_file(dev, &dev_attr_dump_host_state_1);
				error = device_create_file(dev, &dev_attr_dump_host_state_2);
			#else
				error = ifx_proc_addproc("dump_host_state", procfs_dump_host_state_show, NULL);
				error = device_create_file(dev, &dev_attr_dump_host_state);
			#endif
		#else
			error = ifx_proc_addproc("dump_reg", procfs_dump_reg_show, NULL);
			error = device_create_file(dev, &dev_attr_dump_reg_d);
			error = ifx_proc_addproc("dump_spram", procfs_dump_spram_show, NULL);
			error = device_create_file(dev, &dev_attr_dump_spram_d);
		#endif
	#endif //__ENABLE_DUMP__
	//////////////////////////////////////////////////////
#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		error = ifx_proc_addproc("suspend_host_1",NULL, procfs_suspend_host_1_store);
		error = device_create_file(dev, &dev_attr_suspend_host_1);

		error = ifx_proc_addproc("probe_host_1", procfs_probe_host_1_show, procfs_probe_host_1_store);
		error = device_create_file(dev, &dev_attr_probe_host_1);

		error = ifx_proc_addproc("suspend_host_2",NULL, procfs_suspend_host_2_store);
		error = device_create_file(dev, &dev_attr_suspend_host_2);

		error = ifx_proc_addproc("probe_host_2", procfs_probe_host_2_show, procfs_probe_host_2_store);
		error = device_create_file(dev, &dev_attr_probe_host_2);

		error = ifx_proc_addproc("probe_timer1", procfs_probe_timer1_val_show, procfs_probe_timer1_val_store);
		error = device_create_file(dev, &dev_attr_probe_timer1_val_h);

		error = ifx_proc_addproc("probe_timer2", procfs_probe_timer2_val_show, procfs_probe_timer2_val_store);
		error = device_create_file(dev, &dev_attr_probe_timer2_val_h);

		error = ifx_proc_addproc("autoprobe_timer1", procfs_autoprobe_timer1_val_show, procfs_autoprobe_timer1_val_store);
		error = device_create_file(dev, &dev_attr_autoprobe_timer1_val_h);

		error = ifx_proc_addproc("autoprobe_timer2", procfs_autoprobe_timer2_val_show, procfs_autoprobe_timer2_val_store);
		error = device_create_file(dev, &dev_attr_autoprobe_timer2_val_h);
	#else
		error = ifx_proc_addproc("suspend_host",NULL, procfs_suspend_host_store);
		error = device_create_file(dev, &dev_attr_suspend_host);

		error = ifx_proc_addproc("probe_host", procfs_probe_host_show, procfs_probe_host_store);
		error = device_create_file(dev, &dev_attr_probe_host);

		error = ifx_proc_addproc("probe_timer", procfs_probe_timer_val_show, procfs_probe_timer_val_store);
		error = device_create_file(dev, &dev_attr_probe_timer_val_h);

		error = ifx_proc_addproc("autoprobe_timer", procfs_autoprobe_timer_val_show, procfs_autoprobe_timer_val_store);
		error = device_create_file(dev, &dev_attr_autoprobe_timer_val_h);
	#endif
#endif

#ifdef __IS_DEVICE__
	error = ifx_proc_addproc("suspend_device",NULL, procfs_suspend_device_store);
	error = device_create_file(dev, &dev_attr_suspend_device);

	error = ifx_proc_addproc("probe_device", procfs_probe_device_show, procfs_probe_device_store);
	error = device_create_file(dev, &dev_attr_probe_device);

	error = ifx_proc_addproc("probe_timer", procfs_probe_timer_val_show, procfs_probe_timer_val_store);
	error = device_create_file(dev, &dev_attr_probe_timer_val_d);

	error = ifx_proc_addproc("autoprobe_timer", procfs_autoprobe_timer_val_show, procfs_autoprobe_timer_val_store);
	error = device_create_file(dev, &dev_attr_autoprobe_timer_val_d);
#endif
#if defined(__IS_HOST__) && defined(__HOST_COC__)
	#ifdef __IS_DUAL__
	   memset (&pmcuRegisterUSBHost_1, 0, sizeof(pmcuRegisterUSBHost_1));
	   memset (&pmcuRegisterUSBHost_2, 0, sizeof(pmcuRegisterUSBHost_2));
	   pmcuRegisterUSBHost_1.pmcuModule=
	   pmcuRegisterUSBHost_2.pmcuModule=IFX_PMCU_MODULE_USB;
	   pmcuRegisterUSBHost_1.pmcuModuleNr=1;
	   pmcuRegisterUSBHost_2.pmcuModuleNr=2;
	   pmcuRegisterUSBHost_1.pmcuModuleDep = &depListUSBHost_1;
	   pmcuRegisterUSBHost_2.pmcuModuleDep = &depListUSBHost_2;
	   pmcuRegisterUSBHost_1.pre = ifx_usbhost_preChange_1;
	   pmcuRegisterUSBHost_2.pre = ifx_usbhost_preChange_2;
	   pmcuRegisterUSBHost_1.post = ifx_usbhost_postChange_1;
	   pmcuRegisterUSBHost_2.post = ifx_usbhost_postChange_2;
	   pmcuRegisterUSBHost_1.ifx_pmcu_state_change = ifx_usbhost_stateChange_1;
	   pmcuRegisterUSBHost_2.ifx_pmcu_state_change = ifx_usbhost_stateChange_2;
	   pmcuRegisterUSBHost_1.ifx_pmcu_state_get = ifx_usbhost_stateGet_1;
	   pmcuRegisterUSBHost_2.ifx_pmcu_state_get = ifx_usbhost_stateGet_2;
	   pmcuRegisterUSBHost_1.ifx_pmcu_pwr_feature_switch = ifx_usbhost_pwrFeatureSwitch_1;
	   pmcuRegisterUSBHost_2.ifx_pmcu_pwr_feature_switch = ifx_usbhost_pwrFeatureSwitch_2;
	   ifx_pmcu_register ( &pmcuRegisterUSBHost_1 );
	   ifx_pmcu_register ( &pmcuRegisterUSBHost_2 );
	#else
	   memset (&pmcuRegisterUSBHost, 0, sizeof(pmcuRegisterUSBHost));
	   pmcuRegisterUSBHost.pmcuModule=IFX_PMCU_MODULE_USB;
	   pmcuRegisterUSBHost.pmcuModuleNr=1;
	   pmcuRegisterUSBHost.pmcuModuleDep = &depListUSBHost;
	   pmcuRegisterUSBHost.pre = ifx_usbhost_preChange;
	   pmcuRegisterUSBHost.post = ifx_usbhost_postChange;
	   pmcuRegisterUSBHost.ifx_pmcu_state_change = ifx_usbhost_stateChange;
	   pmcuRegisterUSBHost.ifx_pmcu_state_get = ifx_usbhost_stateGet;
	   pmcuRegisterUSBHost.ifx_pmcu_pwr_feature_switch = ifx_usbhost_pwrFeatureSwitch;
	   ifx_pmcu_register ( &pmcuRegisterUSBHost );
	#endif
#endif
#if defined(__IS_DEVICE__) && defined(__GADGET_COC__)
	   memset (&pmcuRegisterUSBGadget, 0, sizeof(pmcuRegisterUSBGadget));
	   pmcuRegisterUSBGadget.pmcuModule=IFX_PMCU_MODULE_USB;
	   pmcuRegisterUSBGadget.pmcuModuleNr=0;
	   pmcuRegisterUSBGadget.pmcuModuleDep = &depListUSBGadget;
	   pmcuRegisterUSBGadget.pre = ifx_usbgadget_preChange;
	   pmcuRegisterUSBGadget.post = ifx_usbgadget_postChange;
	   pmcuRegisterUSBGadget.ifx_pmcu_state_change = ifx_usbgadget_stateChange;
	   pmcuRegisterUSBGadget.ifx_pmcu_state_get = ifx_usbgadget_stateGet;
	   pmcuRegisterUSBGadget.ifx_pmcu_pwr_feature_switch = ifx_usbgadget_pwrFeatureSwitch;
	   ifx_pmcu_register ( &pmcuRegisterUSBGadget );
#endif
}


/*!
  \brief This function remove the sysfs and procfs entries
  \param[in] _dev Pointer of device structure, if applied
 */
#ifdef __IS_HOST__
void ifxusb_attr_remove_h (void *_dev)
#else
void ifxusb_attr_remove_d (void *_dev)
#endif
{
	struct device *dev = (struct device *) _dev;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	ifx_proc_delproc("dbglevel");
	#ifdef __IS_HOST__
	device_remove_file(dev, &dev_attr_dbglevel_h);
	#else
	device_remove_file(dev, &dev_attr_dbglevel_d);
	#endif

	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			ifx_proc_delproc("dump_params_1");
			ifx_proc_delproc("dump_params_2");
			device_remove_file(dev, &dev_attr_dump_params_h_1);
			device_remove_file(dev, &dev_attr_dump_params_h_2);
		#else
			ifx_proc_delproc("dump_params");
			device_remove_file(dev, &dev_attr_dump_params_h);
		#endif

		#ifdef __IS_DUAL__
			ifx_proc_delproc("mode_1");
			ifx_proc_delproc("mode_2");
			device_remove_file(dev, &dev_attr_mode_h_1);
			device_remove_file(dev, &dev_attr_mode_h_2);
		#else
			ifx_proc_delproc("mode");
			device_remove_file(dev, &dev_attr_mode_h);
		#endif
	#else
		ifx_proc_delproc("dump_params");
		device_remove_file(dev, &dev_attr_dump_params_d);
		ifx_proc_delproc("mode");
		device_remove_file(dev, &dev_attr_mode_d);
	#endif

	#ifdef __IS_HOST__
		ifx_proc_delproc("version");
		device_remove_file(dev, &dev_attr_version_h);
	#else
		ifx_proc_delproc("version");
		device_remove_file(dev, &dev_attr_version_d);
	#endif


	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			ifx_proc_delproc("pkt_count_limit_bi_1");
			ifx_proc_delproc("pkt_count_limit_bo_1");
			ifx_proc_delproc("pkt_count_limit_bi_2");
			ifx_proc_delproc("pkt_count_limit_bo_2");
			ifx_proc_delproc("bandwidth_hs_1");
			ifx_proc_delproc("bandwidth_fs_1");
			ifx_proc_delproc("bandwidth_ls_1");
			ifx_proc_delproc("bandwidth_hs_2");
			ifx_proc_delproc("bandwidth_fs_2");
			ifx_proc_delproc("bandwidth_ls_2");
			device_remove_file(dev, &dev_attr_pkt_count_limit_bi_1);
			device_remove_file(dev, &dev_attr_pkt_count_limit_bo_1);
			device_remove_file(dev, &dev_attr_pkt_count_limit_bi_2);
			device_remove_file(dev, &dev_attr_pkt_count_limit_bo_2);
			device_remove_file(dev, &dev_attr_bandwidth_hs_1);
			device_remove_file(dev, &dev_attr_bandwidth_fs_1);
			device_remove_file(dev, &dev_attr_bandwidth_ls_1);
			device_remove_file(dev, &dev_attr_bandwidth_hs_2);
			device_remove_file(dev, &dev_attr_bandwidth_fs_2);
			device_remove_file(dev, &dev_attr_bandwidth_ls_2);
		#else
			ifx_proc_delproc("pkt_count_limit_bi");
			ifx_proc_delproc("pkt_count_limit_bo");
			ifx_proc_delproc("bandwidth_hs");
			ifx_proc_delproc("bandwidth_fs");
			ifx_proc_delproc("bandwidth_ls");
			device_remove_file(dev, &dev_attr_pkt_count_limit_bi);
			device_remove_file(dev, &dev_attr_pkt_count_limit_bo);
			device_remove_file(dev, &dev_attr_bandwidth_hs);
			device_remove_file(dev, &dev_attr_bandwidth_fs);
			device_remove_file(dev, &dev_attr_bandwidth_ls);
		#endif
	#endif

	#ifdef __IS_HOST__
		#ifdef __IS_DUAL__
			ifx_proc_delproc("buspower_1");
			ifx_proc_delproc("buspower_2");
			device_remove_file(dev, &dev_attr_buspower_1);
			device_remove_file(dev, &dev_attr_buspower_2);
		#else
			ifx_proc_delproc("buspower");
			device_remove_file(dev, &dev_attr_buspower);
		#endif

		#ifdef __IS_DUAL__
			ifx_proc_delproc("bussuspend_1");
			ifx_proc_delproc("bussuspend_2");
			device_remove_file(dev, &dev_attr_bussuspend_1);
			device_remove_file(dev, &dev_attr_bussuspend_2);
		#else
			ifx_proc_delproc("bussuspend");
			device_remove_file(dev, &dev_attr_bussuspend);
		#endif

		#ifdef __IS_DUAL__
			ifx_proc_delproc("busconnected_1");
			ifx_proc_delproc("busconnected_2");
			device_remove_file(dev, &dev_attr_busconnected_1);
			device_remove_file(dev, &dev_attr_busconnected_2);
		#else
			ifx_proc_delproc("busconnected");
			device_remove_file(dev, &dev_attr_busconnected);
		#endif

		#ifdef __IS_DUAL__
			ifx_proc_delproc("connectspeed_1");
			ifx_proc_delproc("connectspeed_2");
			device_remove_file(dev, &dev_attr_connectspeed_1);
			device_remove_file(dev, &dev_attr_connectspeed_2);
		#else
			ifx_proc_delproc("connectspeed");
			device_remove_file(dev, &dev_attr_connectspeed);
		#endif
	#endif

	#ifdef __IS_DEVICE__
		ifx_proc_delproc("devspeed");
		device_remove_file(dev, &dev_attr_devspeed);
		ifx_proc_delproc("enumspeed");
		device_remove_file(dev, &dev_attr_enumspeed);
	#endif

	#ifdef __ENABLE_DUMP__
		#ifdef __IS_HOST__
			#ifdef __IS_DUAL__
				ifx_proc_delproc("dump_reg_1");
				ifx_proc_delproc("dump_reg_2");
				device_remove_file(dev, &dev_attr_dump_reg_h_1);
				device_remove_file(dev, &dev_attr_dump_reg_h_2);
			#else
				ifx_proc_delproc("dump_reg");
				device_remove_file(dev, &dev_attr_dump_reg_h);
			#endif

			#ifdef __IS_DUAL__
				ifx_proc_delproc("dump_spram_1");
				ifx_proc_delproc("dump_spram_2");
				device_remove_file(dev, &dev_attr_dump_spram_h_1);
				device_remove_file(dev, &dev_attr_dump_spram_h_2);
			#else
				ifx_proc_delproc("dump_spram");
				device_remove_file(dev, &dev_attr_dump_spram_h);
			#endif

			#ifdef __IS_DUAL__
				ifx_proc_delproc("dump_host_state_1");
				ifx_proc_delproc("dump_host_state_2");
				device_remove_file(dev, &dev_attr_dump_host_state_1);
				device_remove_file(dev, &dev_attr_dump_host_state_2);
			#else
				ifx_proc_delproc("dump_host_state");
				device_remove_file(dev, &dev_attr_dump_host_state);
			#endif
		#else
			ifx_proc_delproc("dump_reg");
			device_remove_file(dev, &dev_attr_dump_reg_d);
			ifx_proc_delproc("dump_spram");
			device_remove_file(dev, &dev_attr_dump_spram_d);
		#endif

		#ifdef __IS_HOST__
		#endif
	#endif //__ENABLE_DUMP__
#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		ifx_proc_delproc("suspend_host_1");
		ifx_proc_delproc("probe_host_1");
		device_remove_file(dev, &dev_attr_suspend_host_1);
		device_remove_file(dev, &dev_attr_probe_host_1);
		ifx_proc_delproc("suspend_host_2");
		ifx_proc_delproc("probe_host_2");
		device_remove_file(dev, &dev_attr_suspend_host_2);
		device_remove_file(dev, &dev_attr_probe_host_2);
		ifx_proc_delproc("probe_timer1");
		ifx_proc_delproc("autoprobe_timer1");
		device_remove_file(dev, &dev_attr_probe_timer1_val_h);
		device_remove_file(dev, &dev_attr_autoprobe_timer1_val_h);
		ifx_proc_delproc("probe_timer2");
		ifx_proc_delproc("autoprobe_timer2");
		device_remove_file(dev, &dev_attr_probe_timer2_val_h);
		device_remove_file(dev, &dev_attr_autoprobe_timer2_val_h);
	#else
		ifx_proc_delproc("suspend_host");
		ifx_proc_delproc("probe_host");
		device_remove_file(dev, &dev_attr_suspend_host);
		device_remove_file(dev, &dev_attr_probe_host);
		ifx_proc_delproc("probe_timer");
		ifx_proc_delproc("autoprobe_timer");
		device_remove_file(dev, &dev_attr_probe_timer_val_h);
		device_remove_file(dev, &dev_attr_autoprobe_timer_val_h);
	#endif
	remove_proc_entry(ifxusb_hcd_driver_name, (void *)0);
#endif

#ifdef __IS_DEVICE__
	ifx_proc_delproc("suspend_device");
	ifx_proc_delproc("probe_device");
	device_remove_file(dev, &dev_attr_suspend_device);
	device_remove_file(dev, &dev_attr_probe_device);
	ifx_proc_delproc("probe_timer");
	ifx_proc_delproc("autoprobe_timer");
	device_remove_file(dev, &dev_attr_probe_timer_val_d);
	device_remove_file(dev, &dev_attr_autoprobe_timer_val_d);
	remove_proc_entry(ifxusb_pcd_driver_name, (void *)0);
#endif
#if defined(__IS_HOST__) && defined(__HOST_COC__)
	#ifdef __IS_DUAL__
	   ifx_pmcu_unregister ( &pmcuRegisterUSBHost_1 );
	   ifx_pmcu_unregister ( &pmcuRegisterUSBHost_2 );
	#else
	   ifx_pmcu_unregister ( &pmcuRegisterUSBHost );
	#endif
#endif
#if defined(__IS_DEVICE__) && defined(__GADGET_COC__)
	   ifx_pmcu_unregister ( &pmcuRegisterUSBGadget );
#endif

}

static struct proc_dir_entry * proc_ifx_root = NULL;

/* initialize the proc file system and make a dir named /proc/[name] */
static void ifx_proc_init(void)
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
#ifdef __IS_HOST__
	proc_ifx_root = proc_mkdir(ifxusb_hcd_driver_name, (void *)0);
	if (!proc_ifx_root){
		IFX_PRINT("%s proc initialization failed! \n", ifxusb_hcd_driver_name);
		return;
	}
#else
	proc_ifx_root = proc_mkdir(ifxusb_pcd_driver_name, (void *)0);
	if (!proc_ifx_root){
		IFX_PRINT("%s proc initialization failed! \n", ifxusb_pcd_driver_name);
		return;
	}
#endif
}

/* proc file system add function for debugging. */
static int ifx_proc_addproc(char *funcname, read_proc_t *hookfuncr, write_proc_t *hookfuncw)
{
	struct proc_dir_entry *pe;
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	if (!proc_ifx_root)
		ifx_proc_init();

	if (hookfuncw == NULL)
	{
		pe = create_proc_read_entry(funcname, S_IRUGO, proc_ifx_root, hookfuncr, NULL);
		if (!pe)
		{
			IFX_PRINT("ERROR in creating read proc entry (%s)! \n", funcname);
			return -1;
		}
	}
	else
	{
		pe = create_proc_entry(funcname, S_IRUGO | S_IWUGO, proc_ifx_root);
		if (pe)
		{
			pe->read_proc = hookfuncr;
			pe->write_proc = hookfuncw;
		}
		else
		{
			IFX_PRINT("ERROR in creating proc entry (%s)! \n", funcname);
			return -1;
		}
	}
	return 0;
}


/* proc file system del function for removing module. */
static void ifx_proc_delproc(char *funcname)
{
	char pname[30];
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	sprintf(pname, "%s", funcname);

	remove_proc_entry(pname, proc_ifx_root);

}

static void ifxusb_dump_params(ifxusb_core_if_t *_core_if)
{
	ifxusb_params_t *params=&_core_if->params;

	#ifdef __IS_HOST__
		IFX_PRINT("IFXUSB Dump Parameters ( Host Mode) \n");
	#endif //__IS_HOST__
	#ifdef __IS_DEVICE__
		IFX_PRINT("IFXUSB Dump Parameters ( Device Mode) \n");
	#endif //__IS_DEVICE__

	#ifdef __DESC_DMA__
		IFX_PRINT("DMA: Hermes DMA\n");
	#else
		IFX_PRINT("DMA: Non-Desc DMA\n");
	#endif
	IFX_PRINT("     Burst size: %d\n",params->dma_burst_size);

	if     (params->speed==1)
		IFX_PRINT("Full Speed only\n");
	else if(params->speed==0)
		IFX_PRINT("Full/Hign Speed\n");
	else
		IFX_PRINT("Unkonwn setting (%d) for Speed\n",params->speed);

	IFX_PRINT("Total Data FIFO size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",
		params->data_fifo_size,params->data_fifo_size,
		params->data_fifo_size*4, params->data_fifo_size*4
	);

	#ifdef __IS_DEVICE__
		IFX_PRINT("Rx FIFO size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",
			params->rx_fifo_size,params->rx_fifo_size,
			params->rx_fifo_size*4, params->rx_fifo_size*4
		);
		{
			int i;
			for(i=0;i<MAX_EPS_CHANNELS;i++)
			{
				IFX_PRINT("Tx FIFO #%d size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",i,
					params->tx_fifo_size[i],params->tx_fifo_size[i],
					params->tx_fifo_size[i]*4, params->tx_fifo_size[i]*4
				);
			}
		}
		#ifdef __DED_FIFO__
			IFX_PRINT("Treshold : %s Rx:%d Tx:%d \n",
				(params->thr_ctl)?"On":"Off",params->tx_thr_length,params->rx_thr_length);
		#endif
	#else //__IS_HOST__
		IFX_PRINT("Host Channels: %d\n",params->host_channels);

		IFX_PRINT("Rx FIFO size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",
			params->data_fifo_size,params->data_fifo_size,
			params->data_fifo_size*4, params->data_fifo_size*4
		);

		IFX_PRINT("NP Tx FIFO size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",
			params->nperio_tx_fifo_size,params->nperio_tx_fifo_size,
			params->nperio_tx_fifo_size*4, params->nperio_tx_fifo_size*4
		);

		IFX_PRINT(" P Tx FIFO size: %d(0x%06X) DWord, %d(0x%06X) Bytes\n",
			params->perio_tx_fifo_size,params->perio_tx_fifo_size,
			params->perio_tx_fifo_size*4, params->perio_tx_fifo_size*4
		);
	#endif //__IS_HOST__

	IFX_PRINT("Max Transfer size: %d(0x%06X) Bytes\n",
		params->max_transfer_size,params->max_transfer_size
	);
	IFX_PRINT("Max Packet Count: %d(0x%06X)\n",
		params->max_packet_count,params->max_packet_count
	);

	IFX_PRINT("PHY UTMI Width: %d\n",params->phy_utmi_width);

	IFX_PRINT("Turn Around Time: HS:%d FS:%d\n",params->turn_around_time_hs,params->turn_around_time_fs);
	IFX_PRINT("Timeout Calibration: HS:%d FS:%d\n",params->timeout_cal_hs,params->timeout_cal_fs);


	IFX_PRINT("==================================================\n");
	IFX_PRINT("End of Parameters Dump\n");
	IFX_PRINT("==================================================\n");
}

