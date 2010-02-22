/*
 * arch/ubicom32/mach-common/profile.c
 *   Implementation for Ubicom32 Profiler
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
 */

#include <linux/platform_device.h>
#include "profile.h"
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/fs.h>
#include <linux/page-flags.h>
#include <asm/uaccess.h>
#include <asm/devtree.h>
#include <asm/profilesample.h>
#include <asm/memory_map.h>
#include <asm/page.h>
#include <asm/ip5000.h>

/*
 * spacs for all memory blocks so we can hold locks for short time when walking tables
 */
#define PROFILE_NUM_MAPS 5000
static struct profile_map profile_pm[PROFILE_NUM_MAPS];

static struct profilenode *node = NULL;
static int profile_first_packet = 1;

static int profile_open(struct inode *inode, struct file *filp)
{
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (!node->enabled) {
		node->enabled = 1;
		node->busy = 0;
		profile_first_packet = 1;
		return 0;
	}
	node->busy = 0;
	return -EBUSY;
}

static int profile_sequence_num;

/*
 * make a packet full of sample data
 */
static int profile_make_data_packet(char *buf, int count)
{
	int samples;		/* number of samples requested */
	int i;
	struct profile_header ph;
	char *ptr;

	if (count < sizeof(struct profile_header) + sizeof(struct profile_sample)) {
		return -EINVAL;
	}

	/*
	 * fill in the packet header
	 */
	memset(&ph, 0, sizeof(struct profile_header));
	ph.magic = PROF_MAGIC + PROFILE_VERSION;
	ph.header_size = sizeof(struct profile_header);
	ph.clocks = node->clocks;
	for (i = 0; i < PROFILE_MAX_THREADS; ++i) {
		ph.instruction_count[i] = node->inst_count[i];
	}
	ph.profile_instructions = 0;
	ph.enabled = node->enabled_threads;
	ph.hrt = node->hrt;
	ph.high = 0;
	ph.profiler_thread = node->profiler_thread;
	ph.clock_freq = node->clock_freq;
	ph.seq_num = profile_sequence_num++;
	ph.cpu_id = node->cpu_id;
	ph.perf_counters[0] = node->stats[0];
	ph.perf_counters[1] = node->stats[1];
	ph.perf_counters[2] = node->stats[2];
	ph.perf_counters[3] = node->stats[3];
	ph.ddr_freq = node->ddr_freq;

	ptr = buf + sizeof(struct profile_header);

	samples = (count - sizeof(struct profile_header)) / sizeof(struct profile_sample);
	for (i = 0; i < samples && node->count; ++i) {
		if (copy_to_user(ptr, &node->samples[node->tail], sizeof(struct profile_sample)) != 0) {
			return -EFAULT;
		}
		node->count--;
		node->tail++;
		if (node->tail >= node->max_samples) {
			node->tail = 0;
		}
		ptr += sizeof(struct profile_sample);
	}
	ph.sample_count = i;
	if (copy_to_user(buf, &ph, sizeof(struct profile_header)) != 0) {
		return -EFAULT;
	}
	if (ph.sample_count == 0)
		return 0;
	else
		return sizeof(struct profile_header) + ph.sample_count * sizeof(struct profile_sample);
}

static void profile_get_memory_stats(unsigned int *total_free, unsigned int *max_free)
{
	struct list_head *p;
	struct zone *zone;
	unsigned int size;

	*total_free = 0;
	*max_free = 0;

	/*
	 * get all the free regions.  In each zone, the array of free_area lists contains the first page of each frame of size 1 << order
	 */
	for_each_zone(zone) {
		unsigned long order, flags, i;

		if (!populated_zone(zone))
			continue;

		if (!is_normal(zone))
			continue;

		spin_lock_irqsave(&zone->lock, flags);
		for_each_migratetype_order(order, i) {
			size = ((1 << order) << PAGE_SHIFT) >> 10;
			list_for_each(p, &(zone->free_area[order].free_list[i])) {
				if (size > *max_free) {
					*max_free = size;
				}
				*total_free += size;
			}
		}
		spin_unlock_irqrestore(&zone->lock, flags);
	}
}

struct profile_counter_pkt profile_builtin_stats[] =
{
	{
	"Free memory(KB)", 0
	},
	{
	"Max free Block(KB)", 0
	}
};

/*
 * make a packet full of performance counters
 */
static char prof_pkt[PROFILE_MAX_PACKET_SIZE];
static int profile_make_stats_packet(char *buf, int count)
{
	char *ptr = prof_pkt;
	struct profile_header_counters hdr;
	int stat_count = 0;
	int i;
	unsigned int total_free, max_free;
	int builtin_count = sizeof(profile_builtin_stats) / sizeof(struct profile_counter_pkt);

	if (count > PROFILE_MAX_PACKET_SIZE) {
		count = PROFILE_MAX_PACKET_SIZE;
	}
	stat_count = (count - sizeof(struct profile_header_counters)) / sizeof (struct profile_counter_pkt);
	stat_count -= builtin_count;

	if (stat_count <= 0) {
		return 0;
	}

	if (stat_count > node->num_counters) {
		stat_count = node->num_counters;
	}

	hdr.magic = PROF_MAGIC_COUNTERS;
	hdr.ultra_sample_time = node->clocks;
	hdr.ultra_count = stat_count;
	hdr.linux_sample_time = UBICOM32_IO_TIMER->sysval;
	hdr.linux_count = builtin_count;
	memcpy(ptr, (void *)&hdr, sizeof(struct profile_header_counters));
	ptr += sizeof(struct profile_header_counters);


	for (i = 0; i < stat_count; ++i) {
		memcpy(ptr, (void *)(&(node->counters[i])), sizeof(struct profile_counter));
		ptr += sizeof(struct profile_counter);
	}

	/*
	 * built in statistics
	 */
	profile_get_memory_stats(&total_free, &max_free);
	profile_builtin_stats[0].value = total_free;
	profile_builtin_stats[1].value = max_free;
	memcpy(ptr, (void *)profile_builtin_stats, sizeof(profile_builtin_stats));
	ptr += sizeof(profile_builtin_stats);

	if (copy_to_user(buf, prof_pkt, ptr - prof_pkt) != 0) {
		return -EFAULT;
	}
	return ptr - prof_pkt;
}

/*
 * return a udp packet ready to send to the profiler tool
 * when there are no packets left to make, return 0
 */
static int profile_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	int result = 0;
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (!node->enabled) {
		node->busy = 0;
		return -EPERM;
	}
	if (!node->samples) {
		node->busy = 0;
		return -ENOMEM;
	}

	if (profile_first_packet) {
		result = profile_make_stats_packet(buf, count);
		profile_first_packet = 0;
	}
	if (result == 0) {
		result = profile_make_data_packet(buf, count);
		if (result == 0) {
			profile_first_packet = 1;
		}
	}
	node->busy = 0;
	return result;

}

static int profile_release(struct inode *inode, struct file *filp)
{
	if (!node) {
		return -ENOENT;
	}
	node->busy = 1;
	if (node->enabled) {
		node->enabled = 0;
		node->count = 0;
		node->tail = node->head;
		node->busy = 0;
		return 0;
	}
	node->busy = 0;
	profile_first_packet = 1;
	return -EBADF;
}

static const struct file_operations profile_fops = {
	.open		= profile_open,
	.read		= profile_read,
	.release	= profile_release,
};

static int page_aligned(void *x)
{
	return !((unsigned int)x & ((1 << PAGE_SHIFT) - 1));
}

static int profile_maps_open(struct inode *inode, struct file *filp)
{
	struct rb_node *rb;
	int num = 0;
	int slab_start;
	struct vm_area_struct *vma;
	int type = PROFILE_MAP_TYPE_UNKNOWN;
	int flags, i;
	struct list_head *p;
	struct zone *zone;

	/*
	 * get the slab data (first so dups will show up as vmas)
	 */
	slab_start = num;
	num += kmem_cache_block_info("size-512", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("size-1024", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("size-2048", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("size-4096", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("size-8192", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);

	for (i = slab_start; i < num; ++i) {
		profile_pm[i].type_size |= PROFILE_MAP_TYPE_SMALL << PROFILE_MAP_TYPE_SHIFT;
	}

	slab_start = num;
	num += kmem_cache_block_info("dentry", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("inode_cache", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("sysfs_dir_cache", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);
	num += kmem_cache_block_info("proc_inode_cache", (struct kmem_cache_size_info *)&profile_pm[num], PROFILE_NUM_MAPS - num);

	for (i = slab_start; i < num; ++i) {
		profile_pm[i].type_size |= PROFILE_MAP_TYPE_FS << PROFILE_MAP_TYPE_SHIFT;
	}

	/*
	 * get all the vma regions (allocated by mmap, most likely
	 */
#if 0
	down_read(&nommu_vma_sem);
	for (rb = rb_first(&nommu_vma_tree); rb && num < PROFILE_NUM_MAPS; rb = rb_next(rb)) {
		vma = rb_entry(rb, struct vm_area_struct, vm_rb);
		profile_pm[num].start = (vma->vm_start - SDRAMSTART) >> PAGE_SHIFT;
		profile_pm[num].type_size = (vma->vm_end - vma->vm_start + (1 << PAGE_SHIFT) - 1) >> PAGE_SHIFT;
		flags = vma->vm_flags & 0xf;
		if (flags == (VM_READ | VM_EXEC)) {
			type = PROFILE_MAP_TYPE_TEXT;
		} else if (flags == (VM_READ | VM_WRITE | VM_EXEC)) {
			type = PROFILE_MAP_TYPE_STACK;
		} else if (flags == (VM_READ | VM_WRITE)) {
			type = PROFILE_MAP_TYPE_APP_DATA;
		}
		profile_pm[num].type_size |= type << PROFILE_MAP_TYPE_SHIFT;
		num++;
	}
	up_read(&nommu_vma_sem);
	if (rb) {
		return -ENOMEM;
	}
#endif

	/*
	 * get all the free regions.  In each zone, the array of free_area lists contains the first page of each frame of size 1 << order
	 */
	for_each_zone(zone) {
		unsigned long order, flags, i;
		struct page *page;

		if (!populated_zone(zone))
			continue;

		if (!is_normal(zone))
			continue;

		spin_lock_irqsave(&zone->lock, flags);
		for_each_migratetype_order(order, i) {
			list_for_each(p, &(zone->free_area[order].free_list[i])) {
				page = list_entry(p, struct page, lru);
				profile_pm[num].start = ((page_to_phys(page) - SDRAMSTART) >> PAGE_SHIFT) - 0x40;
				profile_pm[num].type_size = (PROFILE_MAP_TYPE_FREE << PROFILE_MAP_TYPE_SHIFT) | order;
				num++;
				if (num >= PROFILE_NUM_MAPS) {
					spin_unlock_irqrestore(&zone->lock, flags);
					return -ENOMEM;
				}
			}
		}
		spin_unlock_irqrestore(&zone->lock, flags);
	}

	/*
	 * get the filesystem inodes
	 */
	list_for_each(p, &(super_blocks)) {
		struct super_block *sb;
		struct list_head *q;
		if (num >= PROFILE_NUM_MAPS)
			break;
		sb = list_entry(p, struct super_block, s_list);
		if (page_aligned(sb)) {
			profile_pm[num].start = ((unsigned int)sb - SDRAMSTART) >> PAGE_SHIFT;
			profile_pm[num].type_size = (PROFILE_MAP_TYPE_FS << PROFILE_MAP_TYPE_SHIFT);
			num++;
		}
		list_for_each(q, &(sb->s_inodes)) {
			struct inode *in;
			if (num >= PROFILE_NUM_MAPS)
				break;
			in = list_entry(q, struct inode, i_sb_list);
			if (page_aligned(in)) {
				profile_pm[num].start = ((unsigned int)in - SDRAMSTART) >> PAGE_SHIFT;
				profile_pm[num].type_size = (PROFILE_MAP_TYPE_FS << PROFILE_MAP_TYPE_SHIFT);
				num++;
			}
		}
	}

	/*
	 * get the buffer cache pages
	 */
	for (i = 0; i < num_physpages && num < PROFILE_NUM_MAPS; ++i) {
		if ((mem_map + i)->flags & (1 << PG_lru)) {
			int start = i;
			while ((mem_map + i)->flags & (1 << PG_lru) && i < num_physpages)
				i++;
			profile_pm[num].start = start;
			profile_pm[num].type_size = (i - start) | (PROFILE_MAP_TYPE_CACHE << PROFILE_MAP_TYPE_SHIFT);
			num++;
		}
	}

	filp->private_data = (void *)num;
	return 0;
}

/*
 * return one packet of map data, or 0 if all maps have been returned already
 */
static int profile_maps_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	struct profile_header_maps header;
	char *p = buf + sizeof(header);
	int total = (int)filp->private_data;

	header.count = (count - sizeof(header)) / sizeof(struct profile_map);
	if (header.count > PROFILE_MAX_MAPS) {
		header.count = PROFILE_MAX_MAPS;;
	}
	if (header.count > total - *f_pos) {
		header.count = total - *f_pos;
	}

	if (header.count == 0) {
		return 0;
	}

	header.magic = PROF_MAGIC_MAPS;
	header.page_shift = PAGE_SHIFT;

	if (copy_to_user(buf, &header, sizeof(header)) != 0) {
		return -EFAULT;
	}
	if (copy_to_user(p, (void *)&profile_pm[*f_pos], sizeof(struct profile_map) * header.count) != 0) {
		return -EFAULT;
	}
	*f_pos += header.count;

	return sizeof(header) + sizeof(struct profile_map) * header.count;
}

static int profile_maps_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static const struct file_operations profile_maps_fops = {
	.open		= profile_maps_open,
	.read		= profile_maps_read,
	.release	= profile_maps_release,
};

static int profile_rate_show(struct seq_file *m, void *v)
{
	if (node) {
		seq_printf(m, "%d samples per second.  %d virtual counters.\n", node->rate, node->num_counters);
	} else {
		seq_printf(m, "Profiler is not initialized.\n");
	}
	return 0;
}

static int profile_rate_open(struct inode *inode, struct file *filp)
{
	return single_open(filp, profile_rate_show, NULL);
}

static int profile_rate_write(struct file *filp, const char *buf, size_t len, loff_t *off)
{
	*off = 0;
	return 0;
}

static const struct file_operations profile_rate_fops = {
	.open		= profile_rate_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= profile_rate_write,
};

int ubi32_profile_init_module(void)
{
	struct proc_dir_entry *pdir;

	/*
	 * find the device
	 */
	node = (struct profilenode *)devtree_find_node("profiler");
	if (!node) {
		printk(KERN_INFO "Profiler does not exist.\n");
		return -ENODEV;
	}

	/*
	 * allocate the sample buffer
	 */
	node->max_samples = PROFILE_MAX_SAMPLES;
	node->samples = kmalloc(node->max_samples * sizeof(struct profile_sample), GFP_KERNEL);
	if (!node->samples) {
		printk(KERN_INFO "Profiler sample buffer kmalloc failed.\n");
		return -ENOMEM;
	}

	/*
	 * connect to the file system
	 */
	pdir = proc_mkdir("profile", NULL);
	if (!pdir) {
		return -ENOMEM;
	}
	if (!proc_create("data", 0, pdir, &profile_fops)) {
		return -ENOMEM;
	}
	if (!proc_create("rate", 0, pdir, &profile_rate_fops)) {
		return -ENOMEM;
	}
	if (!proc_create("maps", 0, pdir, &profile_maps_fops)) {
		return -ENOMEM;
	}
	return 0;
}


module_init(ubi32_profile_init_module);

MODULE_AUTHOR("David Fotland");
MODULE_LICENSE("GPL");
