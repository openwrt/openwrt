/*****************************************************************************
 **   FILE NAME       : ifxusb_ctl.c
 **   PROJECT         : IFX USB sub-system V3
 **   MODULES         : IFX USB sub-system Host and Device driver
 **   SRC VERSION     : 1.0
 **   DATE            : 1/Jan/2009
 **   AUTHOR          : Chen, Howard
 **   DESCRIPTION     : Implementing the procfs and sysfs for IFX USB driver
 *****************************************************************************/

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
#endif

#ifdef __IS_HOST__
	#include "ifxhcd.h"
#endif

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gfp.h>


#ifdef __IS_HOST__
	extern char ifxusb_driver_name[];

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
	extern char ifxusb_driver_name[];

	extern ifxpcd_pcd_t ifxusb_pcd;
	extern char ifxusb_pcd_name[];
#endif


//Attributes for sysfs (for 2.6 only)

extern struct device_attribute dev_attr_dbglevel;

#ifdef __IS_DUAL__
	extern struct device_attribute dev_attr_dump_params_1;
	extern struct device_attribute dev_attr_dump_params_2;
#else
	extern struct device_attribute dev_attr_dump_params;
#endif

#ifdef __IS_DUAL__
	extern struct device_attribute dev_attr_mode_1;
	extern struct device_attribute dev_attr_mode_2;
#else
	extern struct device_attribute dev_attr_mode;
#endif

#ifdef __IS_HOST__
	#ifdef __IS_DUAL__
		extern struct device_attribute dev_attr_buspower_1;
		extern struct device_attribute dev_attr_buspower_2;
		extern struct device_attribute dev_attr_bussuspend_1;
		extern struct device_attribute dev_attr_bussuspend_2;
		extern struct device_attribute dev_attr_busconnected_1;
		extern struct device_attribute dev_attr_busconnected_2;
		extern struct device_attribute dev_attr_connectspeed_1;
		extern struct device_attribute dev_attr_connectspeed_1;
	#else
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
	#ifdef __IS_DUAL__
		extern struct device_attribute dev_attr_dump_reg_1;
		extern struct device_attribute dev_attr_dump_reg_2;
		extern struct device_attribute dev_attr_dump_spram_1;
		extern struct device_attribute dev_attr_dump_spram_2;
		#ifdef __IS_HOST__
			extern struct device_attribute dev_attr_dump_host_state_1;
			extern struct device_attribute dev_attr_dump_host_state_2;
		#else
		#endif
	#else
		extern struct device_attribute dev_attr_dump_reg;
		extern struct device_attribute dev_attr_dump_spram;
		#ifdef __IS_HOST__
			extern struct device_attribute dev_attr_dump_host_state;
		#else
		#endif
	#endif
#endif //__ENABLE_DUMP__


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

DEVICE_ATTR(dbglevel, S_IRUGO|S_IWUSR, sysfs_dbglevel_show, sysfs_dbglevel_store);


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
	DEVICE_ATTR(dump_params_1, S_IRUGO|S_IWUSR, sysfs_dump_params_show_1, NULL);

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_dump_params_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_dump_params_show_2( struct device *_dev,char *buf)
	#endif
	{
		dump_params_2();
		return 0;
	}

	DEVICE_ATTR(dump_params_2, S_IRUGO|S_IWUSR, sysfs_dump_params_show_2, NULL);
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
	DEVICE_ATTR(dump_params, S_IRUGO|S_IWUSR, sysfs_dump_params_show, NULL);
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

	DEVICE_ATTR(mode_1, S_IRUGO|S_IWUSR, sysfs_mode_show_1, 0);

	#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
		static ssize_t sysfs_mode_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
	#else
		static ssize_t sysfs_mode_show_2( struct device *_dev,char *buf)
	#endif
	{
		return mode_show_2(buf);
	}
	DEVICE_ATTR(mode_2, S_IRUGO|S_IWUSR, sysfs_mode_show_2, NULL);
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
	DEVICE_ATTR(mode, S_IRUGO|S_IWUSR, sysfs_mode_show, NULL);
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __IS_HOST__
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
			value = simple_strtoul(buf, NULL, 16);
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
			value = simple_strtoul(buf, NULL, 16);
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
			value = simple_strtoul(buf, NULL, 16);
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
			value = simple_strtoul(buf, NULL, 16);
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
			value = simple_strtoul(buf, NULL, 16);
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
			value = simple_strtoul(buf, NULL, 16);
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
			ifxusb_dump_registers(&ifxusb_hcd_1.core_if);
		}
		static void dump_reg_2(void)
		{
			ifxusb_dump_registers(&ifxusb_hcd_2.core_if);
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
		DEVICE_ATTR(dump_reg_1, S_IRUGO|S_IWUSR, sysfs_dump_reg_show_1, 0);
		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_reg_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_reg_show_2( struct device *_dev,char *buf)
		#endif
		{
			dump_reg_2();
			return 0;
		}
		DEVICE_ATTR(dump_reg_2, S_IRUGO|S_IWUSR, sysfs_dump_reg_show_2, 0);
	#else
		static void dump_reg(void)
		{
			#ifdef __IS_HOST__
				ifxusb_dump_registers(&ifxusb_hcd.core_if);
			#endif
			#ifdef __IS_DEVICE__
				ifxusb_dump_registers(&ifxusb_pcd.core_if);
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
		DEVICE_ATTR(dump_reg, S_IRUGO|S_IWUSR, sysfs_dump_reg_show, 0);
	#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

	#ifdef __IS_DUAL__
		static void dump_spram_1(void)
		{
			ifxusb_dump_spram(&ifxusb_hcd_1.core_if);
		}
		static void dump_spram_2(void)
		{
			ifxusb_dump_spram(&ifxusb_hcd_2.core_if);
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
		DEVICE_ATTR(dump_spram_1, S_IRUGO|S_IWUSR, sysfs_dump_spram_show_1, 0);

		#if   LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
			static ssize_t sysfs_dump_spram_show_2( struct device *_dev, struct device_attribute *attr,char *buf)
		#else
			static ssize_t sysfs_dump_spram_show_2( struct device *_dev,char *buf)
		#endif
		{
			dump_spram_2();
			return 0;
		}
		DEVICE_ATTR(dump_spram_2, S_IRUGO|S_IWUSR, sysfs_dump_spram_show_2, 0);
	#else
		static void dump_spram(void)
		{
			#ifdef __IS_HOST__
				ifxusb_dump_spram(&ifxusb_hcd.core_if);
			#endif
			#ifdef __IS_DEVICE__
				ifxusb_dump_spram(&ifxusb_pcd.core_if);
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
		DEVICE_ATTR(dump_spram, S_IRUGO|S_IWUSR, sysfs_dump_spram_show, 0);
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

//////////////////////////////////////////////////////////////////////////////////

static int  ifx_proc_addproc(char *funcname, read_proc_t *hookfuncr, write_proc_t *hookfuncw);
static void ifx_proc_delproc(char *funcname);

//////////////////////////////////////////////////////////////////////////////////

/*!
  \brief This function create the sysfs and procfs entries
  \param[in] _dev Pointer of device structure, if applied
 */
void ifxusb_attr_create (void *_dev)
{
	int error;

	struct device *dev = (struct device *) _dev;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	error = ifx_proc_addproc("dbglevel", procfs_dbglevel_show, procfs_dbglevel_store);
	error = device_create_file(dev, &dev_attr_dbglevel);

	#ifdef __IS_DUAL__
		error = ifx_proc_addproc("dump_params_1", procfs_dump_params_show_1, NULL);
		error = ifx_proc_addproc("dump_params_2", procfs_dump_params_show_2, NULL);
		error = device_create_file(dev, &dev_attr_dump_params_1);
		error = device_create_file(dev, &dev_attr_dump_params_2);
	#else
		error = ifx_proc_addproc("dump_params", procfs_dump_params_show, NULL);
		error = device_create_file(dev, &dev_attr_dump_params);
	#endif

	#ifdef __IS_DUAL__
		error = ifx_proc_addproc("mode_1", procfs_mode_show_1, NULL);
		error = ifx_proc_addproc("mode_2", procfs_mode_show_2, NULL);
		error = device_create_file(dev, &dev_attr_mode_1);
		error = device_create_file(dev, &dev_attr_mode_2);
	#else
		error = ifx_proc_addproc("mode", procfs_mode_show, NULL);
		error = device_create_file(dev, &dev_attr_mode);
	#endif

	#ifdef __IS_HOST__
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
		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("dump_reg_1", procfs_dump_reg_show_1, NULL);
			error = ifx_proc_addproc("dump_reg_2", procfs_dump_reg_show_2, NULL);
			error = device_create_file(dev, &dev_attr_dump_reg_1);
			error = device_create_file(dev, &dev_attr_dump_reg_2);
		#else
			error = ifx_proc_addproc("dump_reg", procfs_dump_reg_show, NULL);
			error = device_create_file(dev, &dev_attr_dump_reg);
		#endif

		#ifdef __IS_DUAL__
			error = ifx_proc_addproc("dump_spram_1", procfs_dump_spram_show_1, NULL);
			error = ifx_proc_addproc("dump_spram_2", procfs_dump_spram_show_2, NULL);
			error = device_create_file(dev, &dev_attr_dump_spram_1);
			error = device_create_file(dev, &dev_attr_dump_spram_2);
		#else
			error = ifx_proc_addproc("dump_spram", procfs_dump_spram_show, NULL);
			error = device_create_file(dev, &dev_attr_dump_spram);
		#endif

		#ifdef __IS_HOST__
			#ifdef __IS_DUAL__
				error = ifx_proc_addproc("dump_host_state_1", procfs_dump_host_state_show_1, NULL);
				error = ifx_proc_addproc("dump_host_state_2", procfs_dump_host_state_show_2, NULL);
				error = device_create_file(dev, &dev_attr_dump_host_state_1);
				error = device_create_file(dev, &dev_attr_dump_host_state_2);
			#else
				error = ifx_proc_addproc("dump_host_state", procfs_dump_host_state_show, NULL);
				error = device_create_file(dev, &dev_attr_dump_host_state);
			#endif
		#endif
	#endif //__ENABLE_DUMP__
	//////////////////////////////////////////////////////
}


/*!
  \brief This function remove the sysfs and procfs entries
  \param[in] _dev Pointer of device structure, if applied
 */
void ifxusb_attr_remove (void *_dev)
{
	struct device *dev = (struct device *) _dev;

	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	ifx_proc_delproc("dbglevel");
	device_remove_file(dev, &dev_attr_dbglevel);

	#ifdef __IS_DUAL__
		ifx_proc_delproc("dump_params_1");
		ifx_proc_delproc("dump_params_2");
		device_remove_file(dev, &dev_attr_dump_params_1);
		device_remove_file(dev, &dev_attr_dump_params_2);
	#else
		ifx_proc_delproc("dump_params");
		device_remove_file(dev, &dev_attr_dump_params);
	#endif

	#ifdef __IS_DUAL__
		ifx_proc_delproc("mode_1");
		ifx_proc_delproc("mode_2");
		device_remove_file(dev, &dev_attr_mode_1);
		device_remove_file(dev, &dev_attr_mode_2);
	#else
		ifx_proc_delproc("mode");
		device_remove_file(dev, &dev_attr_mode);
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
		#ifdef __IS_DUAL__
			ifx_proc_delproc("dump_reg_1");
			ifx_proc_delproc("dump_reg_2");
			device_remove_file(dev, &dev_attr_dump_reg_1);
			device_remove_file(dev, &dev_attr_dump_reg_2);
		#else
			ifx_proc_delproc("dump_reg");
			device_remove_file(dev, &dev_attr_dump_reg);
		#endif

		#ifdef __IS_DUAL__
			ifx_proc_delproc("dump_spram_1");
			ifx_proc_delproc("dump_spram_2");
			device_remove_file(dev, &dev_attr_dump_spram_1);
			device_remove_file(dev, &dev_attr_dump_spram_2);
		#else
			ifx_proc_delproc("dump_spram");
			device_remove_file(dev, &dev_attr_dump_spram);
		#endif

		#ifdef __IS_HOST__
			#ifdef __IS_DUAL__
				ifx_proc_delproc("dump_host_state_1");
				ifx_proc_delproc("dump_host_state_2");
				device_remove_file(dev, &dev_attr_dump_host_state_1);
				device_remove_file(dev, &dev_attr_dump_host_state_2);
			#else
				ifx_proc_delproc("dump_host_state");
				device_remove_file(dev, &dev_attr_dump_host_state);
			#endif
		#endif
	#endif //__ENABLE_DUMP__
	/* AVM/WK fix: del IFXUSB root dir*/
	ifx_proc_delproc(NULL);
}

static struct proc_dir_entry * proc_ifx_root = NULL;

/* initialize the proc file system and make a dir named /proc/[name] */
static void ifx_proc_init(void)
{
	IFX_DEBUGPL(DBG_ENTRY, "%s() %d\n", __func__, __LINE__ );
	proc_ifx_root = proc_mkdir(ifxusb_driver_name, (void *)0);
	if (!proc_ifx_root){
		IFX_PRINT("%s proc initialization failed! \n", ifxusb_driver_name);
		return;
	}
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
/* AVM/WK Fix*/
	if (funcname != NULL) {
		remove_proc_entry(funcname, proc_ifx_root);
	} else {
		remove_proc_entry(ifxusb_driver_name, NULL);
		proc_ifx_root = NULL;
	}
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


