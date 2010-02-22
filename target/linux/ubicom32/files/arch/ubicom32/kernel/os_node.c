/*
 * arch/ubicom32/kernel/os_node.c
 *   <TODO: Replace with short file description>
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "linux/types.h"
#include "linux/linkage.h"
#include "linux/uts.h"
#include "linux/utsrelease.h"
#include "linux/version.h"
#include <asm/ocm_size.h>
#include <asm/devtree.h>
#include <asm/ip5000.h>

extern asmlinkage void *_start;

/*
 * This file provides static information to the boot code allowing it to decide
 * if the os is compatible. Thus hopefully enabling the boot code to prevent
 * accidentally booting a kernel that has no hope of running.
 */
struct os_node {
	struct devtree_node node;
	unsigned long version; /* Always 1 */
	unsigned long entry_point;
	const char    os_name[32]; /* For diagnostic purposes only */
	const char    os_version_str[32];
	unsigned long os_version_num;
	unsigned long expected_ocm_code_start;/* OS Code */
	unsigned long expected_ocm_data_end;  /* OS Data */
	unsigned long expected_ram_start;
	unsigned long expected_ram_end;
	unsigned long arch_version;
	unsigned long expected_os_syscall_begin;
	unsigned long expected_os_syscall_end;
};


extern void __os_syscall_begin;
extern void __os_syscall_end;
/*
 * The os_node is only referenced by head.S and should never be modified at
 * run-time.
 */
asmlinkage const struct os_node _os_node = {
	.node = {
		.next = NULL,
		.name = { "OS" },
		.magic = 0x10203040,
	},
	.version = 0x10002,
	.entry_point = (unsigned long)&_start,
#if APP_OCM_CODE_SIZE || APP_OCM_DATA_SIZE
	.expected_ocm_code_start = OCMSTART + APP_OCM_CODE_SIZE,
	.expected_ocm_data_end   = OCMEND   - APP_OCM_DATA_SIZE,
#else
	.expected_ocm_code_start = OCMEND,
	.expected_ocm_data_end   = OCMEND,
#endif
	.os_name = { UTS_SYSNAME },
	.os_version_str = { UTS_RELEASE },
	.os_version_num = LINUX_VERSION_CODE,
	.expected_ram_start = KERNELSTART,
	.expected_ram_end = SDRAMSTART + CONFIG_MIN_RAMSIZE,
	.arch_version = UBICOM32_ARCH_VERSION,
	.expected_os_syscall_begin = (unsigned long)&__os_syscall_begin,
	.expected_os_syscall_end = (unsigned long)&__os_syscall_end,


};
