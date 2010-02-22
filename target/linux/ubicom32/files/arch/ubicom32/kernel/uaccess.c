/*
 * arch/ubicom32/include/asm/uaccess.c
 *   User space memory access functions for Ubicom32 architecture.
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
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/module.h>

#include <asm/segment.h>
#include <asm/uaccess.h>

extern int _stext, _etext, _sdata, _edata, _sbss, _ebss, _end;

/*
 * __access_ok()
 *	Check that the address is in the current processes.
 *
 * NOTE: The kernel uses "pretend" user addresses that wind
 * up calling access_ok() so this approach has only marginal
 * value because you wind up with lots of false positives.
 */
int __access_ok(unsigned long addr, unsigned long size)
{
	// struct vm_area_struct *vma;

	/*
	 * Don't do anything if we are not a running system yet.
	 */
	if (system_state != SYSTEM_RUNNING) {
		return 1;
	}

	/*
	 * It appears that Linux will call this function even when we are not
	 * in the context of a user space application that has a VM address
	 * space.  So we must check that current and mm are valid before
	 * performing the check.
	 */
	if ((!current) || (!current->mm)) {
		return 1;
	}

	/*
	 * We perform some basic checks on the address to ensure that it
	 * is at least within the range of DRAM.
	 */
	if ((addr < (int)&_etext) || (addr > memory_end)) {
		printk(KERN_WARNING "pid=%d[%s]: range [%lx - %lx] not in memory area: [%lx - %lx]\n",
			current->pid, current->comm,
			addr, addr + size,
			memory_start, memory_end);
		return 0;
	}

	/*
	 * For nommu Linux we can check this by looking at the allowed
	 * memory map for the process.
	 *
	 * TODO: Since the kernel passes addresses in it's own space as though
	 * they were user address, we can not validate the addresses this way.
	 */
#if 0
	if (!down_read_trylock(&current->mm->mmap_sem)) {
		return 1;
	}
	vma = find_vma(current->mm, addr);
	if (!vma) {
		up_read(&current->mm->mmap_sem);
		printk(KERN_WARNING "pid=%d[%s]: possible invalid acesss on range: [%lx - %lx]\n",
				current->pid, current->comm, addr, addr + size);
		return 1;
	}
	if ((addr + size) > vma->vm_end) {
		up_read(&current->mm->mmap_sem);
		printk(KERN_WARNING "pid=%d[%s]: possible invalid length on range: [%lx - %lx]\n",
				current->pid, current->comm, addr, addr + size);
		return 1;
	}
	up_read(&current->mm->mmap_sem);
#endif
	return 1;
}

EXPORT_SYMBOL(__access_ok);
