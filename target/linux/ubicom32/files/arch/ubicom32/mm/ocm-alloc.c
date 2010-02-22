/*
 * arch/ubicom32/mm/ocm-alloc.c
 *	OCM allocator for Uibcom32 On-Chip memory
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *  Copyright 2004-2008 Analog Devices Inc.
 *
 *  Based on:
 *
 *  arch/blackfin/mm/sram-alloc.c
 *
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/rtc.h>
#include <asm/ocm-alloc.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt, a...)
#endif
/*
 * the data structure for OCM heap pieces
 */
struct ocm_piece {
	void *paddr;
	int size;
	pid_t pid;
	struct ocm_piece *next;
};

/*
 * struct ocm_heap
 */
struct ocm_heap {
	struct ocm_piece free_head;
	struct ocm_piece used_head;
	struct mutex lock;
};

static struct ocm_heap ocm_inst_heap;
int ubi32_ocm_skbuf_max = 21, ubi32_ocm_skbuf, ubi32_ddr_skbuf;

/*
 * OCM area for storing code
 */
extern asmlinkage void *__ocm_free_begin;
extern asmlinkage void *__ocm_free_end;
extern asmlinkage void *__ocm_inst_heap_begin;
extern asmlinkage void *__ocm_inst_heap_end;
#define OCM_INST_HEAP_BEGIN ((unsigned int)&__ocm_inst_heap_begin)
#define OCM_INST_HEAP_END ((unsigned int)&__ocm_inst_heap_end)
#define OCM_INST_HEAP_LENGTH (OCM_INST_HEAP_END - OCM_INST_HEAP_BEGIN)

static struct kmem_cache *ocm_piece_cache;

/*
 * _ocm_heap_init()
 */
static int __init _ocm_heap_init(struct ocm_heap *ocmh,
				  unsigned int start,
				  unsigned int size)
{
	ocmh->free_head.next = kmem_cache_alloc(ocm_piece_cache, GFP_KERNEL);

	if (!ocmh->free_head.next)
		return -1;

	ocmh->free_head.next->paddr = (void *)start;
	ocmh->free_head.next->size = size;
	ocmh->free_head.next->pid = 0;
	ocmh->free_head.next->next = 0;

	ocmh->used_head.next = NULL;

	/* mutex initialize */
	mutex_init(&ocmh->lock);

	return 0;
}

/*
 * _ocm_alloc_init()
 *
 * starts the ocm heap(s)
 */
static int __init _ocm_alloc_init(void)
{
	if (OCM_INST_HEAP_LENGTH) {
		ocm_piece_cache = kmem_cache_create("ocm_piece_cache",
						    sizeof(struct ocm_piece),
						    0, SLAB_PANIC, NULL);

		if (_ocm_heap_init(&ocm_inst_heap,
				   OCM_INST_HEAP_BEGIN,
				   OCM_INST_HEAP_LENGTH) == 0)
			printk(KERN_INFO "OCM Instruction Heap %d KB\n",
			       OCM_INST_HEAP_LENGTH >> 10);
		else
			printk(KERN_INFO "Failed to initialize OCM "
			       "Instruction Heap\n");

	} else
		printk(KERN_INFO "No space available for OCM "
		       "Instruction Heap\n");

	return 0;
}
pure_initcall(_ocm_alloc_init);

/*
 * _ocm_alloc()
 *	generic alloc a block in the ocm heap, if successful
 *	returns the pointer.
 */
static void *_ocm_alloc(size_t size, pid_t pid, struct ocm_heap *ocmheap)
{
	struct ocm_piece *pslot, *plast, *pavail;
	struct ocm_piece *pfree_head = &ocmheap->free_head;
	struct ocm_piece *pused_head = &ocmheap->used_head;

	if (size <= 0 || !pfree_head || !pused_head)
		return NULL;

	/* Align the size */
	size = (size + 3) & ~3;

	pslot = pfree_head->next;
	plast = pfree_head;

	/*
	 * search an available piece slot
	 */
	while (pslot != NULL && size > pslot->size) {
		plast = pslot;
		pslot = pslot->next;
	}

	if (!pslot)
		return NULL;

	if (pslot->size == size) {
		/*
		 * Unlink this block from the list
		 */
		plast->next = pslot->next;
		pavail = pslot;
	} else {
		/*
		 * Split this block in two.
		 */
		pavail = kmem_cache_alloc(ocm_piece_cache, GFP_KERNEL);

		if (!pavail)
			return NULL;

		pavail->paddr = pslot->paddr;
		pavail->size = size;
		pslot->paddr += size;
		pslot->size -= size;
	}

	pavail->pid = pid;

	pslot = pused_head->next;
	plast = pused_head;

	/*
	 * insert new piece into used piece list !!!
	 */
	while (pslot != NULL && pavail->paddr < pslot->paddr) {
		plast = pslot;
		pslot = pslot->next;
	}

	pavail->next = pslot;
	plast->next = pavail;

	DEBUGP("_ocm_alloc %d bytes at %p from in %p",
	       size, pavail->paddr, ocmheap);

	return pavail->paddr;
}

#if 0
/* Allocate the largest available block.  */
static void *_ocm_alloc_max(struct ocm_heap *ocmheap,
			     unsigned long *psize)
{
	struct ocm_piece *pfree_head = &ocmheap->free_head;
	struct ocm_piece *pslot, *pmax;

	pmax = pslot = pfree_head->next;

	/* search an available piece slot */
	while (pslot != NULL) {
		if (pslot->size > pmax->size)
			pmax = pslot;
		pslot = pslot->next;
	}

	if (!pmax)
		return NULL;

	*psize = pmax->size;

	return _ocm_alloc(*psize, ocmheap);
}
#endif

/*
 * _ocm_free()
 *	generic free a block in the ocm heap, if successful
 */
static int _ocm_free(const void *addr,
		     struct ocm_heap *ocmheap)
{
	struct ocm_piece *pslot, *plast, *pavail;
	struct ocm_piece *pfree_head = &ocmheap->free_head;
	struct ocm_piece *pused_head = &ocmheap->used_head;

	/* search the relevant memory slot */
	pslot = pused_head->next;
	plast = pused_head;

	/* search an available piece slot */
	while (pslot != NULL && pslot->paddr != addr) {
		plast = pslot;
		pslot = pslot->next;
	}

	if (!pslot) {
		DEBUGP("_ocm_free %p  not found in %p", addr, ocmheap);
		return -1;
	}
	DEBUGP("_ocm_free %p from in %p", addr, ocmheap);

	plast->next = pslot->next;
	pavail = pslot;
	pavail->pid = 0;

	/* insert free pieces back to the free list */
	pslot = pfree_head->next;
	plast = pfree_head;

	while (pslot != NULL && addr > pslot->paddr) {
		plast = pslot;
		pslot = pslot->next;
	}

	if (plast != pfree_head &&
	    plast->paddr + plast->size == pavail->paddr) {
		plast->size += pavail->size;
		kmem_cache_free(ocm_piece_cache, pavail);
	} else {
		pavail->next = plast->next;
		plast->next = pavail;
		plast = pavail;
	}

	if (pslot && plast->paddr + plast->size == pslot->paddr) {
		plast->size += pslot->size;
		plast->next = pslot->next;
		kmem_cache_free(ocm_piece_cache, pslot);
	}

	return 0;
}

/*
 * ocm_inst_alloc()
 *
 *	allocates a block of size in the ocm instrction heap, if
 *	successful returns address allocated.
 */
void *ocm_inst_alloc(size_t size, pid_t pid)
{
	void *addr;

	if (!OCM_INST_HEAP_LENGTH)
		return NULL;


	mutex_lock(&ocm_inst_heap.lock);

	addr = _ocm_alloc(size, pid, &ocm_inst_heap);

	mutex_unlock(&ocm_inst_heap.lock);

	return addr;
}
EXPORT_SYMBOL(ocm_inst_alloc);

/*
 * ocm_inst_free()
 *	free a block in the ocm instrction heap, returns 0 if successful.
 */
int ocm_inst_free(const void *addr)
{
	int ret;

	if (!OCM_INST_HEAP_LENGTH)
		return -1;

	mutex_lock(&ocm_inst_heap.lock);

	ret = _ocm_free(addr, &ocm_inst_heap);

	mutex_unlock(&ocm_inst_heap.lock);

	return ret;
}
EXPORT_SYMBOL(ocm_inst_free);

/*
 * ocm_free()
 *	free a block in one of the ocm heaps, returns 0 if successful.
 */
int ocm_free(const void *addr)
{
	if (addr >= (void *)OCM_INST_HEAP_BEGIN
		 && addr < (void *)(OCM_INST_HEAP_END))
		return ocm_inst_free(addr);
	else
		return -1;
}
EXPORT_SYMBOL(ocm_free);


#ifdef CONFIG_PROC_FS
/* Need to keep line of output the same.  Currently, that is 46 bytes
 * (including newline).
 */
static int _ocm_proc_read(char *buf, int *len, int count, const char *desc,
			   struct ocm_heap *ocmheap)
{
	struct ocm_piece *pslot;
	struct ocm_piece *pfree_head = &ocmheap->free_head;
	struct ocm_piece *pused_head = &ocmheap->used_head;

	/* The format is the following
	 * --- OCM 123456789012345 Size   PID State     \n
	 * 12345678-12345678 1234567890 12345 1234567890\n
	 */
	int l;
	l = sprintf(&buf[*len], "--- OCM %-15s Size   PID State     \n",
		    desc);

	*len += l;
	count -= l;

	mutex_lock(&ocm_inst_heap.lock);

	/*
	 * search the relevant memory slot
	 */
	pslot = pused_head->next;

	while (pslot != NULL && count > 46) {
		l = sprintf(&buf[*len], "%p-%p %10i %5i %-10s\n",
			     pslot->paddr, pslot->paddr + pslot->size,
			     pslot->size, pslot->pid, "ALLOCATED");

		*len += l;
		count -= l;
		pslot = pslot->next;
	}

	pslot = pfree_head->next;

	while (pslot != NULL && count > 46) {
		l = sprintf(&buf[*len], "%p-%p %10i %5i %-10s\n",
			    pslot->paddr, pslot->paddr + pslot->size,
			    pslot->size, pslot->pid, "FREE");

		*len += l;
		count -= l;
		pslot = pslot->next;
	}

	mutex_unlock(&ocm_inst_heap.lock);

	return 0;
}

static int ocm_proc_read(char *buf, char **start, off_t offset, int count,
		int *eof, void *data)
{
	int len = 0;

	len = sprintf(&buf[len], "--- OCM SKB usage (max RX buf %d)\n"
			"(SKB in OCM) %d - (SKB in DDR) %d\n",
			ubi32_ocm_skbuf_max,
			ubi32_ocm_skbuf,
			ubi32_ddr_skbuf);

	len += sprintf(&buf[len], "--- OCM Data Heap       Size\n"
			"%p-%p %10i\n",
			((void *)&__ocm_free_begin),
			((void *)&__ocm_free_end),
			((unsigned int)&__ocm_free_end) -
			((unsigned int)&__ocm_free_begin));

	if (_ocm_proc_read(buf, &len, count - len, "Inst Heap",
			    &ocm_inst_heap))
		goto not_done;
	*eof = 1;
 not_done:
	return len;
}

static int ocm_proc_write(struct file *file, const char __user *buffer,
                           unsigned long count, void *data)
{
	int n, v;
	char in[8];

	if (count > sizeof(in))
		return -EINVAL;

	if (copy_from_user(in, buffer, count))
		return -EFAULT;
	in[count-1] = 0;

	printk(KERN_INFO "OCM skb alloc max = %s\n", in);

	n = 0;
	v = 0;
	while ((in[n] >= '0') && (in[n] <= '9')) {
		v = v * 10 + (int)(in[n] - '0');
		n++;
	}

	if (v == 0)
		return -EINVAL;

	ubi32_ocm_skbuf_max = v;
	ubi32_ocm_skbuf = ubi32_ddr_skbuf = 0;

	return count;
}

static int __init sram_proc_init(void)
{
	struct proc_dir_entry *ptr;
	ptr = create_proc_entry("ocm", S_IFREG | S_IRUGO, NULL);
	if (!ptr) {
		printk(KERN_WARNING "unable to create /proc/ocm\n");
		return -1;
	}
	ptr->read_proc = ocm_proc_read;
	ptr->write_proc = ocm_proc_write;
	return 0;
}
late_initcall(sram_proc_init);
#endif
