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


#ifndef _LTQ_ETHSW_API_LINUX_H_
#define _LTQ_ETHSW_API_LINUX_H_

#define PARAM_BUFFER_SIZE	2048
typedef enum {
	ETHSW_API_USERAPP = 0,
	ETHSW_API_KERNEL,
} ethsw_api_type_t;

/* general declaration fits for all low-level functions. */
typedef int (*gsw_ll_fkt) (void *, u32);
typedef struct gsw_lowlevel_fkts_t gsw_lowlevel_fkts_t;
/* Switch API low-level function tables to map all supported IOCTL commands */
struct gsw_lowlevel_fkts_t {
	/*Some device have multiple tables to split the generic
	switch API features and the device specific switch API features.
	Additional tables, if exist,	can be found under this next pointer.
	Every table comes along with a different 'nType'
	parameter to differentiate.*/
	gsw_lowlevel_fkts_t	*pNext;
	/* IOCTL type of all commands listed in the table. */
	u16 nType;
	/* Number of low-level functions listed in the table. */
	u32 nNumFkts;
	/* Pointer to the first entry of the ioctl number table.
	This table is used to check if the given ioctl command fits
	the the found low-level function pointer under 'pFkts'.*/
	/* u32	*pIoctlCmds;*/
	/* Pointer to the first entry of the function table.
	Table size is given by the parameter 'nNumFkts'. */
	gsw_ll_fkt	*pFkts;
};
/* function type declaration for the default IOCTL low-level function in
   case the command cannot be found in the low-level function table,
	or in case no low-level function table is provided.. */
typedef int (*ioctl_default_fkt) (void*, int, int);
/*typedef*/
typedef struct {
	gsw_lowlevel_fkts_t *pLlTable;
	void *pLlHandle;
	char paramBuffer[PARAM_BUFFER_SIZE];
} ioctl_cmd_handle_t;

/*typedef*/
typedef struct {
	ltq_bool_t	bInternalSwitch;
	/** Number of similar Low Level External Switch Devices */
	u8 nExternalSwitchNum;
	ioctl_cmd_handle_t *pIoctlHandle;
	/** Array of pEthSWDev pointers associated with this driver context */
	void *pEthSWDev[GSW_DEV_MAX];
} ioctl_wrapper_ctx_t;

/*typedef*/
typedef struct {
	gsw_lowlevel_fkts_t	*pLlTable;
} ioctl_wrapper_init_t;

/*typedef*/
typedef struct {
	u8 mn;
} dev_minor_num_t;

int gsw_api_drv_register(u32 major);
int gsw_api_drv_unregister(u32 major);
void	*ioctl_wrapper_init(ioctl_wrapper_init_t *pinit);
int ioctl_wrapper_dev_add(ioctl_wrapper_ctx_t *pioctldev,
                          void *pcoredev, u8 mnum);
int gsw_api_ioctl_wrapper_cleanup(void);

#endif    /* _LTQ_ETHSW_API_LINUX_H_ */
