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

ioctl_wrapper_ctx_t *iwctx;

#define ETHSW_API_DEV_NAME "switch_api"
#if defined(KERNEL_MODE) && KERNEL_MODE
/* Define and declare a semaphore, named swapi_sem, with a count of one */
static DEFINE_SEMAPHORE(swapi_sem);
static int gsw_api_open(struct inode *inode, struct file *filp);
static int gsw_api_release(struct inode *inode, struct file *filp);
static long gsw_api_ioctl(struct file *filp, u32 cmd, unsigned long arg);
#endif /* KERNEL_MODE */

ioctl_cmd_handle_t *alloc_cmd_handle(void)
{
	ioctl_cmd_handle_t *cmd_handle;
#if defined(KERNEL_MODE) && KERNEL_MODE
	gfp_t flags;
	if (in_atomic() || in_interrupt())
		flags = GFP_ATOMIC;
	else
		flags = GFP_KERNEL;
	cmd_handle = kmalloc(sizeof(ioctl_cmd_handle_t), flags);
#else
	cmd_handle = malloc(sizeof(ioctl_cmd_handle_t));
#endif /* KERNEL_MODE */
	if (cmd_handle)
		cmd_handle->pLlTable = iwctx->pIoctlHandle->pLlTable;
	return cmd_handle;
}

/** searching for Switch API IOCTL  command */
int gsw_command_search(void *phandle, u32 command,
                       u32 arg, ethsw_api_type_t apitype)
{
	int retvalue;
	ioctl_cmd_handle_t *pdrv = (ioctl_cmd_handle_t *)phandle;
	gsw_lowlevel_fkts_t *pLlTable = pdrv->pLlTable;
#if defined(KERNEL_MODE) && KERNEL_MODE
	/*  attempt to acquire the semaphore ...*/
	if (down_interruptible(&swapi_sem))
		return -1;
#endif /* KERNEL_MODE */
	/* This table contains the low-level function for the */
	/* IOCTL	commands with the same MAGIC-Type numer. */
	while (pLlTable != NULL) {
		if (GSW_IOC_TYPE(command) == pLlTable->nType) {
			gsw_ll_fkt fkt;
			u32 size;
			u32 cmdnr = GSW_IOC_NR(command);
			if (cmdnr >= pLlTable->nNumFkts)
				goto fail;
			fkt = pLlTable->pFkts[cmdnr];
			/* No low-level function given for this command. */
			if (fkt == NULL)
				goto fail;
			/* Copy parameter from userspace. */
			size = GSW_IOC_SIZE(command);
			/* Local temporary buffer to store the parameter is to small. */
			if (size > PARAM_BUFFER_SIZE)
				goto fail;
			if (apitype == ETHSW_API_USERAPP) {
#if defined(KERNEL_MODE) && KERNEL_MODE
				if (copy_from_user((void *)(pdrv->paramBuffer),
				                   (const void __user *)arg, (unsigned long)size))
					return -EFAULT;
				/* Now call the low-level function with the right low-level context */
				/* handle and the local copy of the parameter structure of 'arg'. */
				retvalue = fkt(pdrv->pLlHandle,
				               (u32)pdrv->paramBuffer);
				/* Copy parameter to userspace. */
				/* Only copy back to userspace if really required */
				if (_IOC_DIR(command) & _IOC_READ) {
					if(copy_to_user((void __user *)arg,
					                (const void *)(pdrv->paramBuffer),
					                (unsigned long)size))
						return -EFAULT;
				}
#endif /* KERNEL_MODE */
			} else {
				memcpy((void *)(pdrv->paramBuffer),
				       (const void *) arg, (unsigned long)size);

				retvalue = fkt(pdrv->pLlHandle,
				               (u32)pdrv->paramBuffer);
				memcpy((void *)arg,
				       (const void *)(pdrv->paramBuffer),
				       (unsigned long)size);
			}
#if defined(KERNEL_MODE) && KERNEL_MODE
			up(&swapi_sem);
#endif /* KERNEL_MODE */
			return retvalue;
		}
		/* If command was not found in the current table index, */
		/* look for the next linked table. Search till it is found */
		/* or we run out of tables.*/
		pLlTable = pLlTable->pNext;
	}
fail:
	/*  release the given semaphore */
#if defined(KERNEL_MODE) && KERNEL_MODE
	up(&swapi_sem);
#endif /* KERNEL_MODE */
	/* No supported command low-level function found.*/
	return -1;
}
#if defined(KERNEL_MODE) && KERNEL_MODE
/**The driver callbacks that will be registered with the kernel*/
/*static*/
const struct file_operations swapi_fops = {
owner:
	THIS_MODULE,
unlocked_ioctl :
	gsw_api_ioctl,
open :
	gsw_api_open,
release :
	gsw_api_release
};

static long gsw_api_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
	dev_minor_num_t *p;
	int ret;
	ioctl_wrapper_ctx_t *pdev;
	ioctl_cmd_handle_t *cmd_handle;

	p = filp->private_data;
	pdev = iwctx;
	cmd_handle = alloc_cmd_handle();
	if (!cmd_handle)
		return -1;
	if (!p->mn) {
		if (pdev->bInternalSwitch == 1)
			cmd_handle->pLlHandle = pdev->pEthSWDev[0];
		else {
			GSW_PRINT("%s[%d]: Not support internal switch\n\n",
			          __func__, __LINE__);
			kfree(cmd_handle);
			return -1;
		}
	} else {
		if (p->mn <= pdev->nExternalSwitchNum) {
			cmd_handle->pLlHandle = pdev->pEthSWDev[p->mn];
		} else {
			GSW_PRINT("(Not support external switch number: %d) %s:%s:%d\n",
			          p->mn, __FILE__, __func__, __LINE__);
			kfree(cmd_handle);
			return -1;
		}
	}
	ret = gsw_command_search(cmd_handle, cmd,
	                         arg, ETHSW_API_USERAPP);
	kfree(cmd_handle);
	return ret;
}

static int gsw_api_open(struct inode *inode, struct file *filp)
{
	u32 minornum, majornum;
	dev_minor_num_t *p;

	minornum = MINOR(inode->i_rdev);
	majornum = MAJOR(inode->i_rdev);
	p = kmalloc(sizeof(dev_minor_num_t), GFP_KERNEL);
	if (!p) {
		GSW_PRINT("%s[%d]: memory allocation failed !!\n",
		          __func__, __LINE__);
		return -ENOMEM;
	}
	p->mn = minornum;
	filp->private_data = p;
	return 0;
}

static int gsw_api_release(struct inode *inode,
                           struct file *filp)
{
	if (filp->private_data) {
		kfree(filp->private_data);
		filp->private_data = NULL;
	}
	return 0;
}

int gsw_api_drv_register(u32 major)
{
	int result;
	result = register_chrdev(major, ETHSW_API_DEV_NAME, &swapi_fops);
	if (result < 0) {
		GSW_PRINT("SWAPI: Register Char Dev failed with %d!!!\n", result);
		return result;
	}
	pr_info("SWAPI: Registered char device [%s] with major no [%d]\n",
	        ETHSW_API_DEV_NAME, major);
	return 0;
}

int gsw_api_drv_unregister(u32 major)
{
	unregister_chrdev(major, ETHSW_API_DEV_NAME);
	return 0;
}
#endif /* KERNEL_MODE */

void *ioctl_wrapper_init(ioctl_wrapper_init_t *pinit)
{
	u8 i;
	ioctl_wrapper_ctx_t *pdev;
#if defined(KERNEL_MODE) && KERNEL_MODE
	pdev = (ioctl_wrapper_ctx_t *)kmalloc(sizeof(ioctl_wrapper_ctx_t), GFP_KERNEL);
#else
	pdev = (ioctl_wrapper_ctx_t *)malloc(sizeof(ioctl_wrapper_ctx_t));
#endif /* KERNEL_MODE */
	if (!pdev) {
		GSW_PRINT("%s memory allocation failed !!\n", __func__);
		return pdev;
	}
	pdev->bInternalSwitch = 0;  /* internal switch, the value is 0 */
	pdev->nExternalSwitchNum = 0;
#if defined(KERNEL_MODE) && KERNEL_MODE
	pdev->pIoctlHandle = (ioctl_cmd_handle_t *)kmalloc(sizeof(ioctl_cmd_handle_t), GFP_KERNEL);
	if (!pdev->pIoctlHandle) {
		GSW_PRINT("%s memory allocation failed !!\n", __func__);
		if (pdev)
			kfree(pdev);
		return NULL;
	}
#else
	pdev->pIoctlHandle = (ioctl_cmd_handle_t *)malloc(sizeof(ioctl_cmd_handle_t));
	if (!pdev->pIoctlHandle) {
		GSW_PRINT("%s memory allocation failed !!\n", __func__);
		if (pdev)
			free(pdev);
		return pdev->pIoctlHandle;
	}
#endif /* KERNEL_MODE */

	pdev->pIoctlHandle->pLlTable = pinit->pLlTable;
	for (i = 0; i < GSW_DEV_MAX; i++)
		pdev->pEthSWDev[i] = NULL;
	iwctx = pdev;
	return pdev;
}

int ioctl_wrapper_dev_add(ioctl_wrapper_ctx_t *pioctldev,
                          void *pcoredev, u8 mnum)
{
	if (mnum >= GSW_DEV_MAX) {
		GSW_PRINT("(Device number: %d) %s:%s:%d\n", mnum,
		          __FILE__, __func__, __LINE__);
		return -1;
	}
	pioctldev->pEthSWDev[mnum] = pcoredev;
	if (!mnum) {
		pioctldev->bInternalSwitch = 1;
		/*
			GSW_PRINT(" %s:%s:%d\n", __FILE__, __func__, __LINE__);
		*/
	} else /* other than 0 means external switch */
		pioctldev->nExternalSwitchNum++;
	return 0;
}

int gsw_api_ioctl_wrapper_cleanup(void)
{
	ioctl_wrapper_ctx_t *pdev = iwctx;
	if (pdev != NULL) {
		if (pdev->pIoctlHandle != NULL) {
#if defined(KERNEL_MODE) && KERNEL_MODE
			kfree(pdev->pIoctlHandle);
			pdev->pIoctlHandle = NULL;
			kfree(pdev);
			pdev = NULL;
#else
			free(pdev->pIoctlHandle);
			free(pdev);
#endif
		}

	}
	return 0;
}

GSW_API_HANDLE gsw_api_kopen(char *name)
{
	ioctl_wrapper_ctx_t *pdev;
	void *pLlHandle;
	/* process /dev/switch/minor string */
	char *needle = "/";
	char *buf = strstr(name, needle);
	pdev = iwctx;
	name = buf+strlen(needle); /* pointer to dev */
	if (name != NULL)
		buf = strstr(name, needle);
	name = buf+strlen(needle); /* pointer to switch */
	if (name != NULL)
		buf = strstr(name, needle);
	name = buf+strlen(needle); /* pointer to minor */
	if (name == NULL)
		return 0;
	if (!strcmp(name, "0")) {
		if (pdev->bInternalSwitch == 1)
			pLlHandle = pdev->pEthSWDev[0];
		else {
			return 0;
		}
	} else if (!strcmp(name, "1")) {
		pLlHandle = pdev->pEthSWDev[1];
	} else {
		GSW_PRINT("\nNot support external switch number = %s\n\n", name);
		return 0;
	}
	return (GSW_API_HANDLE)pLlHandle;
}
#if defined(KERNEL_MODE) && KERNEL_MODE
EXPORT_SYMBOL(gsw_api_kopen);
#endif /* KERNEL_MODE */
GSW_return_t gsw_api_kioctl(GSW_API_HANDLE handle, u32 command, u32 arg)
{
	ioctl_wrapper_ctx_t *pdev = iwctx;
	ioctl_cmd_handle_t *cmd_handle;
	int ret;
	cmd_handle = alloc_cmd_handle();
	if (!cmd_handle)
		return -1;
	if (handle == (GSW_API_HANDLE)pdev->pEthSWDev[0]) {
		cmd_handle->pLlHandle = pdev->pEthSWDev[0];
	} else {
#if defined(KERNEL_MODE) && KERNEL_MODE
		kfree(cmd_handle);
#else
		free(cmd_handle);
#endif
		GSW_PRINT("ERROR:( Address:0x%08x) %s:%s:%d\n",
		          handle, __FILE__, __func__, __LINE__);
		return -1;
	}
	ret = gsw_command_search(cmd_handle, command,	arg, ETHSW_API_KERNEL);
#if defined(KERNEL_MODE) && KERNEL_MODE
	kfree(cmd_handle);
#else
	free(cmd_handle);
#endif
	return ret;
}
#if defined(KERNEL_MODE) && KERNEL_MODE
EXPORT_SYMBOL(gsw_api_kioctl);
#endif /* KERNEL_MODE */

GSW_return_t gsw_api_kclose(GSW_API_HANDLE handle)
{
	/* Nothing to do for kernel API's */
	return 0;
}

#if defined(KERNEL_MODE) && KERNEL_MODE
EXPORT_SYMBOL(gsw_api_kclose);
#endif /* KERNEL_MODE */
