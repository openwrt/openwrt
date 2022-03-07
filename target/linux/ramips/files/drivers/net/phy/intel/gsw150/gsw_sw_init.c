/*
 This file is provided under a dual BSD/GPLv2 license.  When using or
 redistributing this file, you may do so under either license.

 GPL LICENSE SUMMARY

 Copyright(c) 2016 - 2017 Intel Corporation.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 Contact Information:
  Intel Corporation
  2200 Mission College Blvd.
  Santa Clara, CA  97052

 BSD LICENSE

 Copyright(c) 2016 - 2017 Intel Corporation.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
  * Neither the name of Intel Corporation nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include "gsw_sw_init.h"

/**************************************************************************/
/*      EXTERNS Declaration:                                              */
/**************************************************************************/
/* These incldue from Master MDIO bus routies */
/* pseudo functions for MDIO access */
/* extern int mdio_read_ext(int regaddr, int *phydata);
	extern int mdio_write_ext(int regaddr, int phydata);
*/
GSW_return_t gsw_reg_rd(void *pdev, u16 offset, u16 shift, u16 sz, ur *v);
GSW_return_t gsw_reg_wr(void *pdev, u16 offset, u16 shift, u16 sz, u32 v);
extern GSW_return_t config_sw_def_vals(void *pdev);
#if defined(UART_INTERFACE) && UART_INTERFACE
extern int pc_uart_dataread(u16 Offset, u16 *value);
extern int pc_uart_datawrite(u16 Offset, u16 value);
#endif /* UART_INTERFACE */
/**************************************************************************/
/*      DEFINES:                                                          */
/**************************************************************************/
#define SWAPI_MAJOR_NUMBER	81

#define SMDIO_DEFAULT_BUSADDR   (0x1)
#define SMDIO_DEFAULT_PHYADDR	(0x1F)
#define DEFAULT_SWITCH_NUM      (1)
#define SMDIO_WRADDR	(0x1F)
#define SMDIO_RDADDR	(0x0)
#define SUPPORT_AS_LOADABLE_MODULE 1
#define GSW_SGMII_PORT	0x4
#define RGMII_PORT0	0x5
#define RGMII_PORT1	0x6

/**************************************************************************/
/*      LOCAL DECLARATIONS:                                               */
/**************************************************************************/
#if defined(UART_INTERFACE) && UART_INTERFACE
static GSW_return_t uart_reg_rd(u16 regaddr, u16 *data);
static GSW_return_t uart_reg_wr(u16 regaddr, u16 data);
#endif /* UART_INTERFACE */
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
static GSW_return_t ReadMdio(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 *data);
static GSW_return_t WriteMdio(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 data);
static GSW_return_t smdio_reg_rd(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 *data);
static GSW_return_t smdio_reg_wr(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 data);
#endif /* SMDIO_INTERFACE */
ioctl_wrapper_init_t ioctlinit;
ioctl_wrapper_ctx_t *pioctlctl;
ethsw_api_dev_t *pedev0[GSW_DEV_MAX];
extern gsw_lowlevel_fkts_t flow_fkt_tbl;

#if defined(SUPPORT_AS_LOADABLE_MODULE) && SUPPORT_AS_LOADABLE_MODULE
#if defined(KERNEL_MODE) && KERNEL_MODE
static int gsw_num = DEFAULT_SWITCH_NUM, hw_init = 0, id_argc = 0, addr_argc = 0;
static int gsw_mdio_id[5] = { SMDIO_DEFAULT_BUSADDR, 0, 0, 0, 0 };
static int gsw_mdio_addr[5] = { SMDIO_DEFAULT_PHYADDR, 0x1F, 0, 0, 0 };
module_param(gsw_num, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(gsw_num, "No.of Switches supports");
module_param(hw_init, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(hw_init, "Switch HW Init during module load");
module_param_array(gsw_mdio_id, int, &id_argc, 0000);
MODULE_PARM_DESC(gsw_mdio_id, "An array of mdio bus id's");
module_param_array(gsw_mdio_addr, int, &addr_argc, 0000);
MODULE_PARM_DESC(gsw_mdio_addr, "An array of mdio address");
#endif /* KERNEL_MODE */
#endif /* SUPPORT_AS_LOADABLE_MODULE */
/**************************************************************************/
/*	This is the switch core layer init function.*/
void *ethsw_api_core_init(ethsw_core_init_t *einit)
{
	ethsw_api_dev_t *pd;
	if (einit->sdev >= GSW_DEV_MAX) {
		GSW_PRINT("%s:%s:%d (Error)\n",
		          __FILE__, __func__, __LINE__);
		return 0;
	}
#if defined(KERNEL_MODE) && KERNEL_MODE
	pd = (ethsw_api_dev_t *)kmalloc(sizeof(ethsw_api_dev_t), GFP_KERNEL);
#else
	pd = (ethsw_api_dev_t *)malloc(sizeof(ethsw_api_dev_t));
#endif /* KERNEL_MODE */
	if (!pd) {
		GSW_PRINT("%s:%s:%d (malloc failed)\n",
		          __FILE__, __func__, __LINE__);
		return pd;
	}
	memset(pd, 0, sizeof(ethsw_api_dev_t));
	pd->ecint = einit;
	pd->edev = einit->sdev;
	return pd;
}

/**	This is the switch core layer cleanup function.*/
void ethsw_api_core_exit(void *pdev)
{
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	if (pd) {
#if defined(KERNEL_MODE) && KERNEL_MODE
		kfree(pd);
#else
		free(pd);
#endif
	}
}

#if defined(UART_INTERFACE) && UART_INTERFACE
/* UART inetrface suppot function */
static GSW_return_t uart_reg_rd(u16 regaddr, u16 *data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_dataread(regaddr, data);
	return GSW_statusOk;
}
static GSW_return_t uart_reg_wr(u16 regaddr, u16 data)
{
	/* Add customer UART routines*/
	/* pseudo function */
	pc_uart_datawrite(regaddr, data);
	return GSW_statusOk;
}
#endif /* UART_INTERFACE */

#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE

/* Customer can modify MDIO routines depends on SOC supports
*/
static GSW_return_t WriteMdio(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 data)
{
#if defined(KERNEL_MODE) && KERNEL_MODE
	extern int mdio_write_ext(int mdionum, int phyaddr, int phyreg, int phydata);
	int result;

	/* Access MDIO */
	result = mdio_write_ext(mdio_id, phyaddr, regaddr, data);
	return (result != 0) ? GSW_statusErr : GSW_statusOk;
#else
	return GSW_statusErr;
#endif
}

/* Customer can modify MDIO routines depends on SOC supports
*/
static GSW_return_t ReadMdio(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 *data)
{
#if defined(KERNEL_MODE) && KERNEL_MODE
	extern int mdio_read_ext(int mdionum, int phyaddr, int phyreg, int *phydata);
	int result;
	int value;

	/* Access MDIO */
	result = mdio_read_ext(mdio_id, phyaddr, regaddr, &value);
	*data = value;
	return (result != 0) ? GSW_statusErr : GSW_statusOk;
#else
	return GSW_statusErr;
#endif
}

/* SMDIO supported functions */
static GSW_return_t smdio_reg_rd(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 *rdata)
{
	int rs;
	u16 d = 0;
	rs = ReadMdio(mdio_id, phyaddr, regaddr, &d);
	if (rs != GSW_statusOk) {
		GSW_PRINT("%s:%d pa=%d, ra=0x%x, rs:%d\n", __func__, __LINE__, \
		          phyaddr, regaddr, rs);
		return rs;
	}
	*rdata = d;
	return GSW_statusOk;
}

static GSW_return_t smdio_reg_wr(u8 mdio_id, u16 phyaddr, u16 regaddr, u16 data)
{
	int rs;
	rs = WriteMdio(mdio_id, phyaddr, regaddr, data);
	if (rs != GSW_statusOk) {
		GSW_PRINT("%s:%d pa=%d, ra=0x%x, data=%d, rs:%d\n", __func__, __LINE__, \
		          phyaddr, regaddr, data, rs);
		return rs;
	}
	return GSW_statusOk;
}
#endif

GSW_return_t gsw_reg_rd(void *pdev, u16 Offset, u16 Shift, u16 Size, ur *value)
{
	GSW_return_t s;
	u16 ro, rv = 0, mask;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL)
	if ((Offset & 0xD000) == 0xD000)
		ro = Offset;
	else
		ro = (Offset | 0xE000);
#if defined(UART_INTERFACE) && UART_INTERFACE
	uart_reg_rd(ro, &rv);
#endif /* UART_INTERFACE */
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
	s = smdio_reg_wr(pd->mdio_id, pd->mdio_addr, SMDIO_WRADDR, ro);
	if (s != GSW_statusOk)
		return s;
	s = smdio_reg_rd(pd->mdio_id, pd->mdio_addr, SMDIO_RDADDR, &rv);
	if (s != GSW_statusOk)
		return s;
#endif /* SMDIO_INTERFACE */
	mask = (1 << Size) - 1 ;
	rv = (rv >> Shift);
	*value = (rv & mask);
	return GSW_statusOk;
}

GSW_return_t gsw_reg_wr(void *pdev, u16 Offset, u16 Shift, u16 Size, u32 value)
{
	GSW_return_t s;
	u16 ro, rv = 0, mask;
	ethsw_api_dev_t *pd = (ethsw_api_dev_t *)pdev;
	SWAPI_ASSERT(pd == NULL);
	if ((Offset & 0xD000) == 0xD000)
		ro = Offset;
	else
		ro = (Offset | 0xE000);
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
	s = smdio_reg_wr(pd->mdio_id, pd->mdio_addr, SMDIO_WRADDR, ro);
	if (s != GSW_statusOk)
		return s;
#endif /* SMDIO_INTERFACE */
	if (Size != 16) {
#if defined(UART_INTERFACE) && UART_INTERFACE
		uart_reg_rd(ro, &rv);
#endif /* UART_INTERFACE */
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
		s = smdio_reg_rd(pd->mdio_id, pd->mdio_addr, SMDIO_RDADDR, &rv);
		if (s != GSW_statusOk)
			return s;
#endif /* SMDIO_INTERFACE */
		mask = (1 << Size) - 1;
		mask = (mask << Shift);
		value = ((value << Shift) & mask);
		value = ((rv & ~mask) | value);
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
		s = smdio_reg_wr(pd->mdio_id, pd->mdio_addr, SMDIO_WRADDR, ro);
		if (s != GSW_statusOk)
			return s;
#endif /* SMDIO_INTERFACE */
	}
#if defined(UART_INTERFACE) && UART_INTERFACE
	uart_reg_wr(ro, value);
#endif /* UART_INTERFACE */
#if defined(SMDIO_INTERFACE) && SMDIO_INTERFACE
	s = smdio_reg_wr(pd->mdio_id, pd->mdio_addr, SMDIO_RDADDR, value);
	if (s != GSW_statusOk)
		return s;
#endif /* SMDIO_INTERFACE */
	return GSW_statusOk;
}

int ethsw_swapi_register(void)
{
	int ret, devid;
	ethsw_core_init_t core_init;
#if defined(KERNEL_MODE) && KERNEL_MODE
	ret = gsw_api_drv_register(SWAPI_MAJOR_NUMBER);
	if (ret != 0) {
		GSW_PRINT("%s:%s:%d (Error)\n", __FILE__, __func__, __LINE__);
		return ret;
	}
#endif /* KERNEL_MODE */
	for ( devid = 0; devid < gsw_num; devid++) {
		if (devid >= GSW_DEV_MAX)
			return -1;
		core_init.sdev = devid;
		pedev0[devid] = ethsw_api_core_init(&core_init);
		if (pedev0[devid] == NULL) {
			GSW_PRINT("%s:%s:%d (Error)\n", __FILE__, __func__, __LINE__);
			return -1;
		}
		/* Change depends on switch supported port number*/
		pedev0[devid]->cport = GSW_SGMII_PORT;
		pedev0[devid]->gsw_dev = devid;
		pedev0[devid]->mdio_id = gsw_mdio_id[devid];
		pedev0[devid]->mdio_addr = gsw_mdio_addr[devid];
		if ( hw_init == 1) {
			if (config_sw_def_vals(pedev0[devid]) != GSW_statusOk) {
				GSW_PRINT("%s:%s:%d (config_sw_def_vals Error)\n", __FILE__, __func__, __LINE__);
				return 0;
			}
		}
#if defined(GSW_IOCTL_SUPPORT) && GSW_IOCTL_SUPPORT
		if (devid == 0) {
			ioctlinit.pLlTable = &flow_fkt_tbl;
			pioctlctl = ioctl_wrapper_init(&ioctlinit);
			if (pioctlctl == NULL) {
				GSW_PRINT("%s:%s:%d (Error)\n", __FILE__, __func__, __LINE__);
				return -1;
			}
		}
		if (pioctlctl && pedev0[devid])
			ioctl_wrapper_dev_add(pioctlctl, pedev0[devid], devid);
#endif /* GSW_IOCTL_SUPPORT */
	}
	return 0;
}

int ethsw_swapi_unregister(void)
{
	int devid;
	/* Free the device data block */
#if defined(KERNEL_MODE) && KERNEL_MODE
	gsw_api_drv_unregister(SWAPI_MAJOR_NUMBER);
#endif
	for ( devid = 0; devid < gsw_num; devid++) {
		ethsw_api_core_exit(pedev0[devid]);
	}
#if defined(GSW_IOCTL_SUPPORT) && GSW_IOCTL_SUPPORT
	gsw_api_ioctl_wrapper_cleanup();
#endif /* GSW_IOCTL_SUPPORT */
	return 0;
}

#if defined(SUPPORT_AS_LOADABLE_MODULE) && SUPPORT_AS_LOADABLE_MODULE
#if defined(KERNEL_MODE) && KERNEL_MODE
static int __init gsw_swapi_init(void)
{
	int res;
	GSW_PRINT("SWITCH API, Version %s.\n", SWAPI_DRV_VERSION);
	if ((gsw_num > GSW_DEV_MAX) || (gsw_num == 0))
		return -1;
	res = ethsw_swapi_register();
	GSW_PRINT("SWITCH API, Init Done. result=%d\n", res);
	return res;
}

static void __exit gsw_swapi_exit(void)
{
	ethsw_swapi_unregister();
}
module_init(gsw_swapi_init);
module_exit(gsw_swapi_exit);
#endif /* KERNEL_MODE */
#else
static int gsw_swapi_init()
{
	int res;
	GSW_PRINT("SWITCH API, Version %s.\n", SWAPI_DRV_VERSION);
	res = ethsw_swapi_register();
	GSW_PRINT("SWITCH API, Init Done. result=%d\n", res);
	return res;
}
static void gsw_swapi_exit(void)
{
	ethsw_swapi_unregister();
}
#endif /* SUPPORT_AS_LOADABLE_MODULE */

#if defined(KERNEL_MODE) && KERNEL_MODE
MODULE_AUTHOR("INTEL");
MODULE_DESCRIPTION("Switch API");
MODULE_LICENSE("GPL");
MODULE_VERSION(SWAPI_DRV_VERSION);
#endif
