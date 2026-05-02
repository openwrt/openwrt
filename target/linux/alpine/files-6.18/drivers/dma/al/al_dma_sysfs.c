/*
 * Annapurna Labs DMA Linux driver - sysfs support
 * Copyright(c) 2011 Annapurna Labs.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

#include <linux/stat.h>

#include "al_dma.h"
#include "al_dma_sysfs.h"
#include <mach/al_hal_udma_debug.h>

#define to_ext_attr(x) container_of(x, struct dev_ext_attribute, attr)

#define DEVICE_STATS_PREP_ATTR(_name)                                          \
  {__ATTR(stats_prep_##_name, 0440, rd_stats_prep, NULL),                      \
   (void *)offsetof(struct al_dma_chan_stats_prep, _name)}

#define DEVICE_STATS_COMP_ATTR(_name)                                          \
  {__ATTR(stats_comp_##_name, 0440, rd_stats_comp, NULL),                      \
   (void *)offsetof(struct al_dma_chan_stats_comp, _name)}

#define UDMA_DUMP_PREP_ATTR(_name, _type)                                      \
  {__ATTR(udma_dump_##_name, 0660, rd_udma_dump, wr_udma_dump), (void *)_type}

enum udma_dump_type {
  UDMA_DUMP_M2S_REGS,
  UDMA_DUMP_M2S_Q_STRUCT,
  UDMA_DUMP_M2S_Q_POINTERS,
  UDMA_DUMP_S2M_REGS,
  UDMA_DUMP_S2M_Q_STRUCT,
  UDMA_DUMP_S2M_Q_POINTERS
};

#ifdef CONFIG_AL_DMA_STATS
static ssize_t rd_stats_prep(struct device *dev, struct device_attribute *attr,
                             char *buf);

static ssize_t rd_stats_comp(struct device *dev, struct device_attribute *attr,
                             char *buf);

static ssize_t rd_stats_rst(struct device *dev, struct device_attribute *attr,
                            char *buf);

static ssize_t wr_stats_rst(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count);

static ssize_t rd_udma_dump(struct device *dev, struct device_attribute *attr,
                            char *buf);

static ssize_t wr_udma_dump(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count);
#endif

#ifdef CONFIG_AL_DMA_STATS
struct dev_ext_attribute dev_attr_stats_prep[] = {
    DEVICE_STATS_PREP_ATTR(int_num),
    DEVICE_STATS_PREP_ATTR(memcpy_num),
    DEVICE_STATS_PREP_ATTR(memcpy_size),
    DEVICE_STATS_PREP_ATTR(sg_memcpy_num),
    DEVICE_STATS_PREP_ATTR(sg_memcpy_size),
    DEVICE_STATS_PREP_ATTR(memset_num),
    DEVICE_STATS_PREP_ATTR(memset_size),
    DEVICE_STATS_PREP_ATTR(xor_num),
    DEVICE_STATS_PREP_ATTR(xor_size),
    DEVICE_STATS_PREP_ATTR(pq_num),
    DEVICE_STATS_PREP_ATTR(pq_size),
    DEVICE_STATS_PREP_ATTR(pq_val_num),
    DEVICE_STATS_PREP_ATTR(pq_val_size),
    DEVICE_STATS_PREP_ATTR(xor_val_num),
    DEVICE_STATS_PREP_ATTR(xor_val_size),
    DEVICE_STATS_PREP_ATTR(matching_cpu),
    DEVICE_STATS_PREP_ATTR(mismatching_cpu),
};

struct dev_ext_attribute dev_attr_stats_comp[] = {
    DEVICE_STATS_COMP_ATTR(redundant_int_cnt),
    DEVICE_STATS_COMP_ATTR(matching_cpu),
    DEVICE_STATS_COMP_ATTR(mismatching_cpu),
};

/* Device attrs - udma debug */
static struct dev_ext_attribute dev_attr_udma_debug[] = {
    UDMA_DUMP_PREP_ATTR(m2s_regs, UDMA_DUMP_M2S_REGS),
    UDMA_DUMP_PREP_ATTR(m2s_q_struct, UDMA_DUMP_M2S_Q_STRUCT),
    UDMA_DUMP_PREP_ATTR(m2s_q_pointers, UDMA_DUMP_M2S_Q_POINTERS),
    UDMA_DUMP_PREP_ATTR(s2m_regs, UDMA_DUMP_S2M_REGS),
    UDMA_DUMP_PREP_ATTR(s2m_q_struct, UDMA_DUMP_S2M_Q_STRUCT),
    UDMA_DUMP_PREP_ATTR(s2m_q_pointers, UDMA_DUMP_S2M_Q_POINTERS)};

static DEVICE_ATTR(stats_rst, 0660, rd_stats_rst, wr_stats_rst);
#endif

/******************************************************************************
 *****************************************************************************/
int al_dma_sysfs_init(struct device *dev) {
  int status = 0;

#ifdef CONFIG_AL_DMA_STATS
  int i;

  for (i = 0; i < ARRAY_SIZE(dev_attr_stats_prep); i++) {
    status = sysfs_create_file(&dev->kobj, &dev_attr_stats_prep[i].attr.attr);
    if (status) {
      dev_err(dev, "%s: sysfs_create_file(stats_prep %d) failed\n", __func__,
              i);
      goto done;
    }
  }

  for (i = 0; i < ARRAY_SIZE(dev_attr_stats_comp); i++) {
    status = sysfs_create_file(&dev->kobj, &dev_attr_stats_comp[i].attr.attr);
    if (status) {
      dev_err(dev, "%s: sysfs_create_file(stats_comp %d) failed\n", __func__,
              i);
      goto done;
    }
  }

  status = sysfs_create_file(&dev->kobj, &dev_attr_stats_rst.attr);
  if (status) {
    dev_err(dev, "%s: sysfs_create_file(stats_rst) failed\n", __func__);
    goto done;
  }

  for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++) {
    status = sysfs_create_file(&dev->kobj, &dev_attr_udma_debug[i].attr.attr);
    if (status) {
      dev_err(dev, "%s: sysfs_create_file(stats_udma %d) failed\n", __func__,
              i);
      goto done;
    }
  }
done:
#endif

  return status;
}

/******************************************************************************
 *****************************************************************************/
void al_dma_sysfs_terminate(struct device *dev) {
#ifdef CONFIG_AL_DMA_STATS
  int i;

  for (i = 0; i < ARRAY_SIZE(dev_attr_stats_prep); i++)
    sysfs_remove_file(&dev->kobj, &dev_attr_stats_prep[i].attr.attr);

  for (i = 0; i < ARRAY_SIZE(dev_attr_stats_comp); i++)
    sysfs_remove_file(&dev->kobj, &dev_attr_stats_comp[i].attr.attr);

  sysfs_remove_file(&dev->kobj, &dev_attr_stats_rst.attr);

  for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++) {
    sysfs_remove_file(&dev->kobj, &dev_attr_udma_debug[i].attr.attr);
  }

#endif
}

#ifdef CONFIG_AL_DMA_STATS
/******************************************************************************
 *****************************************************************************/
static ssize_t rd_stats_prep(struct device *dev, struct device_attribute *attr,
                             char *buf) {
  struct al_dma_device *device = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  int i;
  ssize_t size = 0;

  size += sprintf(&buf[size], "statistics - %s:\n", attr->attr.name);

  for (i = 0; i < device->max_channels; i++) {
    uint64_t val;

    spin_lock_bh(&device->channels[i]->prep_lock);

    val = *(uint64_t *)(((uint8_t *)&device->channels[i]->stats_prep) + offset);

    spin_unlock_bh(&device->channels[i]->prep_lock);

    size += sprintf(&buf[size], "chan[%d] %llu\n", i, val);
  }

  return size;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t rd_stats_comp(struct device *dev, struct device_attribute *attr,
                             char *buf) {
  struct al_dma_device *device = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  int i;
  ssize_t size = 0;

  size += sprintf(&buf[size], "statistics - %s:\n", attr->attr.name);

  for (i = 0; i < device->max_channels; i++) {
    uint64_t val;

    spin_lock_bh(&device->channels[i]->cleanup_lock);

    val = *(uint64_t *)(((uint8_t *)&device->channels[i]->stats_comp) + offset);

    spin_unlock_bh(&device->channels[i]->cleanup_lock);

    size += sprintf(&buf[size], "chan[%d] %llu\n", i, val);
  }

  return size;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t rd_stats_rst(struct device *dev, struct device_attribute *attr,
                            char *buf) {
  return sprintf(buf, "Write anything to clear all statistics\n");
}

/******************************************************************************
 *****************************************************************************/
static ssize_t wr_stats_rst(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count) {
  struct al_dma_device *device = dev_get_drvdata(dev);

  int i;

  for (i = 0; i < device->max_channels; i++) {
    spin_lock_bh(&device->channels[i]->prep_lock);
    spin_lock_bh(&device->channels[i]->cleanup_lock);

    memset(&device->channels[i]->stats_prep, 0,
           sizeof(struct al_dma_chan_stats_prep));

    memset(&device->channels[i]->stats_comp, 0,
           sizeof(struct al_dma_chan_stats_comp));

    spin_unlock_bh(&device->channels[i]->cleanup_lock);
    spin_unlock_bh(&device->channels[i]->prep_lock);
  }

  return i;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t rd_udma_dump(struct device *dev, struct device_attribute *attr,
                            char *buf) {
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
  ssize_t rc = 0;

  switch (dump_type) {
  case UDMA_DUMP_M2S_REGS:
  case UDMA_DUMP_S2M_REGS:
    rc = sprintf(buf, "Write mask to dump corresponding udma regs\n");
    break;
  case UDMA_DUMP_M2S_Q_STRUCT:
  case UDMA_DUMP_S2M_Q_STRUCT:
    rc = sprintf(buf, "Write q num to dump correspoding q struct\n");
    break;
  case UDMA_DUMP_M2S_Q_POINTERS:
  case UDMA_DUMP_S2M_Q_POINTERS:
    rc = sprintf(buf, "Write q num (in hex) and add 1 for submission ring,"
                      " for ex:\n"
                      "0 for completion ring of q 0\n"
                      "10 for submission ring of q 0\n");
    break;
  default:
    break;
  }

  return rc;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t wr_udma_dump(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count) {
  int err;
  int q_id;
  unsigned long val;
  struct al_udma *dma;
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
  enum al_udma_ring_type ring_type = AL_RING_COMPLETION;
  struct al_dma_device *device = dev_get_drvdata(dev);

  err = kstrtoul(buf, 16, &val);
  if (err < 0)
    return err;

  switch (dump_type) {
  case UDMA_DUMP_M2S_REGS:
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_TX, &dma);
    al_udma_regs_print(dma, val);
    break;
  case UDMA_DUMP_S2M_REGS:
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_RX, &dma);
    al_udma_regs_print(dma, val);
    break;
  case UDMA_DUMP_M2S_Q_STRUCT:
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_TX, &dma);
    al_udma_q_struct_print(dma, val);
    break;
  case UDMA_DUMP_S2M_Q_STRUCT:
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_RX, &dma);
    al_udma_q_struct_print(dma, val);
    break;
  case UDMA_DUMP_M2S_Q_POINTERS:
    if (val & 0x10)
      ring_type = AL_RING_SUBMISSION;
    q_id = val & 0xf;
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_TX, &dma);
    al_udma_ring_print(dma, q_id, ring_type);
    break;
  case UDMA_DUMP_S2M_Q_POINTERS:
    if (val & 0x10)
      ring_type = AL_RING_SUBMISSION;
    q_id = val & 0xf;
    al_ssm_dma_handle_get(&device->hal_raid, UDMA_RX, &dma);
    al_udma_ring_print(dma, q_id, ring_type);
    break;
  default:
    break;
  }

  return count;
}
#endif
