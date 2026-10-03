/*
 * Annapurna Labs DMA Linux driver
 * Copyright(c) 2011 Annapurna Labs.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called COPYING.
 */
#ifndef AL_DMA_H
#define AL_DMA_H

#include <linux/cache.h>
#include <linux/circ_buf.h>
#include <linux/dmaengine.h>
#include <linux/dmapool.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>

#include "al_hal_ssm_raid.h"

#define AL_DMA_VERSION "0.01"

#define AL_DMA_IRQNAME_SIZE 40

#define AL_DMA_MAX_SIZE_SHIFT_MEMCPY 16  /* 64KB */
#define AL_DMA_MAX_SIZE_SHIFT_MEMSET 16  /* 64KB */
#define AL_DMA_MAX_SIZE_SHIFT_XOR 14     /* 16KB */
#define AL_DMA_MAX_SIZE_SHIFT_XOR_VAL 14 /* 16KB */
#define AL_DMA_MAX_SIZE_SHIFT_PQ 13      /* 8KB */
#define AL_DMA_MAX_SIZE_SHIFT_PQ_VAL 13  /* 8KB */

#define AL_DMA_ALIGN_SHIFT 0 /* No alignment requirements */

#ifndef CONFIG_ALPINE_VP_WA
#define AL_DMA_RAID_TX_CDESC_SIZE 8
#define AL_DMA_RAID_RX_CDESC_SIZE 8
#else
/* Currently in VP it is always 16 bytes */
#define AL_DMA_RAID_TX_CDESC_SIZE 16
#define AL_DMA_RAID_RX_CDESC_SIZE 16
#endif

#define AL_DMA_MAX_SIZE_MEMCPY (1 << AL_DMA_MAX_SIZE_SHIFT_MEMCPY)
#define AL_DMA_MAX_SIZE_MEMSET (1 << AL_DMA_MAX_SIZE_SHIFT_MEMSET)
#define AL_DMA_MAX_SIZE_XOR (1 << AL_DMA_MAX_SIZE_SHIFT_XOR)
#define AL_DMA_MAX_SIZE_XOR_VAL (1 << AL_DMA_MAX_SIZE_SHIFT_XOR_VAL)
#define AL_DMA_MAX_SIZE_PQ (1 << AL_DMA_MAX_SIZE_SHIFT_PQ)
#define AL_DMA_MAX_SIZE_PQ_VAL (1 << AL_DMA_MAX_SIZE_SHIFT_PQ_VAL)

#define AL_DMA_MAX_XOR AL_SSM_MAX_SRC_DESCS

#define AL_DMA_OP_MAX_BLOCKS (AL_DMA_MAX_XOR * 2)

#define AL_DMA_MAX_CHANNELS 4

#define AL_DMA_SW_RING_MIN_ORDER 4
#define AL_DMA_SW_RING_MAX_ORDER 16

/**
 * Issue pending transaction upon sumbit:
 * 0 - no, issue when issue_pending is called
 * 1 - yes, and do nothing when issue_pending is called
 */
#define AL_DMA_ISSUE_PNDNG_UPON_SUBMIT 1

/*#define AL_DMA_MEMCPY_VALIDATION*/
/*#define AL_DMA_XOR_VALIDATION*/

#ifdef CONFIG_AL_DMA_STATS
#define AL_DMA_STATS_INC(var, incval)                                          \
  {                                                                            \
    (var) += (incval);                                                         \
  }

#define AL_DMA_STATS_UPDATE(chan, num, cnt, size, size_inc)                    \
  {                                                                            \
    AL_DMA_STATS_INC((num), (cnt));                                            \
                                                                               \
    if (size_inc)                                                              \
      AL_DMA_STATS_INC((size), (size_inc));                                    \
                                                                               \
    AL_DMA_STATS_INC((chan)->stats_prep.matching_cpu,                          \
                     (cnt) * (((chan)->idx == smp_processor_id())));           \
                                                                               \
    AL_DMA_STATS_INC((chan)->stats_prep.mismatching_cpu,                       \
                     (cnt) * (!((chan)->idx == smp_processor_id())));          \
  }
#else
#define AL_DMA_STATS_INC(var, incval)
#define AL_DMA_STATS_UPDATE(chan, num, cnt, size, size_inc)
#endif

enum al_unmap_type {
  AL_UNMAP_SINGLE,
  AL_UNMAP_PAGE,
};

struct al_dma_unmap_info_ent {
  dma_addr_t handle;
  size_t size;
  int dir;
  enum al_unmap_type type;
};

/**
 * struct al_dma_sw_desc - software descriptor
 */
struct al_dma_sw_desc {
  struct al_raid_transaction hal_xaction;
  struct al_block blocks[AL_DMA_OP_MAX_BLOCKS];
  struct al_buf bufs[AL_DMA_OP_MAX_BLOCKS];

  size_t len;
  struct dma_async_tx_descriptor txd;
#ifdef DEBUG
  int id;
#endif

  int last_is_pq_val;
  enum sum_check_flags *pq_val_res;

  int last_is_xor_val;
  enum sum_check_flags *xor_val_res;

  int last_is_memcpy;

#ifdef AL_DMA_MEMCPY_VALIDATION
  void *memcpy_dest;
  void *memcpy_src;
  size_t memcpy_len;
#endif

  int last_is_xor;

#ifdef AL_DMA_XOR_VALIDATION
  void *xor_dest;
  int xor_src_cnt;
  void *xor_src[AL_DMA_OP_MAX_BLOCKS];
  size_t xor_len;
#endif

  struct al_dma_unmap_info_ent unmap_info[AL_DMA_OP_MAX_BLOCKS];
  int umap_ent_cnt;
};
#define to_al_dma_device(dev) container_of(dev, struct al_dma_device, common)
#define to_dev(al_dma_chan) (&(al_dma_chan)->device->pdev->dev)

#ifdef CONFIG_AL_DMA_STATS
/**
 * struct al_dma_chan_stats_prep - DMA channel statistics - preparation
 * @int_num - Total number of interrupt requests
 * @memcpy_num - Total number of memcpy operations
 * @memcpy_size - Total size of memcpy operations
 * @memset_num - Total number of memset operations
 * @memset_size - Total size of memset operations
 * @xor_num - Total number of xor operations
 * @xor_size - Total size of xor operations
 * @pq_num - Total number of pq operations
 * @pq_size - Total size of pq operations
 * @pq_val_num - Total number of pq validation operations
 * @pq_val_size - Total size of pq validation operations
 * @xor_val_num - Total number of xor validation operations
 * @xor_val_size - Total size of xor validation operations
 * @matching_cpu - Number of preparations with matching queue and cpu
 * @mismatching_cpu - Number of preparations with mismatching queue and cpu
 */
struct al_dma_chan_stats_prep {
  uint64_t int_num;
  uint64_t memcpy_num;
  uint64_t memcpy_size;
  uint64_t sg_memcpy_num;
  uint64_t sg_memcpy_size;
  uint64_t memset_num;
  uint64_t memset_size;
  uint64_t xor_num;
  uint64_t xor_size;
  uint64_t pq_num;
  uint64_t pq_size;
  uint64_t pq_val_num;
  uint64_t pq_val_size;
  uint64_t xor_val_num;
  uint64_t xor_val_size;
  uint64_t matching_cpu;
  uint64_t mismatching_cpu;
};

/**
 * struct al_dma_chan_stats_prep - DMA channel statistics - completion
 * @redundant_int_cnt - Total number of redundant interrupts (interrupts for
 *                      which there was no completions
 * @matching_cpu - Number of completions with matching queue and cpu
 * @mismatching_cpu - Number of completions with mismatching queue and cpu
 */
struct al_dma_chan_stats_comp {
  uint64_t redundant_int_cnt;
  uint64_t matching_cpu;
  uint64_t mismatching_cpu;
};
#endif

/* internal structure for AL Crypto IRQ
 */
struct al_dma_irq {
  char name[AL_DMA_IRQNAME_SIZE];
};

/**
 * struct al_dma_device - internal representation of a DMA device
 */
struct al_dma_device {
  struct pci_dev *pdev;
  u16 dev_id;
  u8 rev_id;

  struct al_ssm_dma_params ssm_dma_params;
  void __iomem *udma_regs_base;
  void __iomem *app_regs_base;
  struct al_ssm_dma hal_raid;

  struct dma_device common;

  struct msix_entry msix_entries[AL_DMA_MAX_CHANNELS];
  struct al_dma_irq irq_tbl[AL_DMA_MAX_CHANNELS];
  struct al_dma_chan *channels[AL_DMA_MAX_CHANNELS];
  int max_channels;

  struct kmem_cache *cache;
};

/**
 * struct al_dma_chan - internal representation of a DMA channel
 */
struct al_dma_chan {
  /* Misc */
  struct dma_chan common ____cacheline_aligned;
  struct al_ssm_dma *hal_raid;
  int idx;
  struct al_dma_device *device;
  cpumask_t affinity_mask;

  /* SW descriptors ring */
  struct al_dma_sw_desc **sw_ring;

  /* Tx UDMA hw ring */
  int tx_descs_num;       /* number of descriptors in Tx queue */
  void *tx_dma_desc_virt; /* Tx descriptors ring */
  dma_addr_t tx_dma_desc;

  /* Rx UDMA hw ring */
  int rx_descs_num;       /* number of descriptors in Rx queue */
  void *rx_dma_desc_virt; /* Rx descriptors ring */
  dma_addr_t rx_dma_desc;
  void *rx_dma_cdesc_virt; /* Rx completion descriptors ring */
  dma_addr_t rx_dma_cdesc;

  /* sysfs */
  struct kobject kobj;

  /* Channel allocation */
  u16 alloc_order;

  /* Preparation */
  spinlock_t prep_lock ____cacheline_aligned;
  u16 head;
  int sw_desc_num_locked;
  uint32_t tx_desc_produced;
#ifdef CONFIG_AL_DMA_STATS
  struct al_dma_chan_stats_prep stats_prep;
#endif

  /* Completion */
  spinlock_t cleanup_lock ____cacheline_aligned_in_smp;
  struct tasklet_struct cleanup_task;
  u16 tail;
#ifdef CONFIG_AL_DMA_STATS
  struct al_dma_chan_stats_comp stats_comp;
#endif
};

static inline u16 al_dma_ring_size(struct al_dma_chan *chan) {
  return 1 << chan->alloc_order;
}

/* count of transactions in flight with the engine */
static inline u16 al_dma_ring_active(struct al_dma_chan *chan) {
  return CIRC_CNT(chan->head, chan->tail, al_dma_ring_size(chan));
}

static inline u16 al_dma_ring_space(struct al_dma_chan *chan) {
  return CIRC_SPACE(chan->head, chan->tail, al_dma_ring_size(chan));
}

static inline struct al_dma_sw_desc *
al_dma_get_ring_ent(struct al_dma_chan *chan, u16 idx) {
  return chan->sw_ring[idx & (al_dma_ring_size(chan) - 1)];
}

struct al_dma_sysfs_entry {
  struct attribute attr;
  ssize_t (*show)(struct dma_chan *, char *);
};

static inline struct al_dma_chan *to_al_dma_chan(struct dma_chan *c) {
  return container_of(c, struct al_dma_chan, common);
}

/* wrapper around hardware descriptor format + additional software fields */

#ifdef DEBUG
#define set_desc_id(desc, i) ((desc)->id = (i))
#define desc_id(desc) ((desc)->id)
#else
#define set_desc_id(desc, i)
#define desc_id(desc) (0)
#endif

static inline struct al_dma_chan *
al_dma_chan_by_index(struct al_dma_device *device, int index) {
  return device->channels[index];
}

static inline u32 al_dma_chansts(struct al_dma_chan *chan) {
  u32 status = 0;

  return status;
}

static inline void al_dma_unmap_info_ent_set(struct al_dma_unmap_info_ent *ent,
                                             dma_addr_t handle, size_t size,
                                             int dir, enum al_unmap_type type) {
  ent->handle = handle;
  ent->size = size;
  ent->dir = dir;
  ent->type = type;
}

int al_dma_get_sw_desc_lock(struct al_dma_chan *chan, int num);

int al_dma_core_init(struct al_dma_device *device, void __iomem *iobase_udma,
                     void __iomem *iobase_app);

int al_dma_fast_init(struct al_dma_device *device, void __iomem *iobase_udma);

int al_dma_fast_terminate(struct al_dma_device *device);

int al_dma_core_terminate(struct al_dma_device *device);

int al_dma_cleanup_fn(struct al_dma_chan *chan, int from_tasklet);

int udma_fast_memcpy(int len, al_phys_addr_t src, al_phys_addr_t dst);

void al_dma_flr(struct pci_dev *pdev);

/**
 * Submit pending SW descriptors (enlarge the head) and unlock the prep-lock
 * in the case 'issue-pending' is responsible for submitting the HW descriptors
 */
void al_dma_tx_submit_sw_cond_unlock(struct al_dma_chan *chan,
                                     struct dma_async_tx_descriptor *tx);

void al_dma_kobject_add(struct al_dma_device *device, struct kobj_type *type);
void al_dma_kobject_del(struct al_dma_device *device);
extern const struct sysfs_ops al_dma_sysfs_ops;
extern struct al_dma_sysfs_entry al_dma_version_attr;
extern struct al_dma_sysfs_entry al_dma_cap_attr;

uint32_t al_dma_read_reg32(const volatile void __iomem *address);
uint16_t al_dma_read_reg16(const volatile void __iomem *address);
uint8_t al_dma_read_reg8(const volatile void __iomem *address);
void al_dma_write_reg32(volatile void __iomem *address, u32 val);

#endif /* AL_DMA_H */
