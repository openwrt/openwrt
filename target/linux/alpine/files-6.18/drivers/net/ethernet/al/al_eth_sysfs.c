/* al_eth_sysfs.c: AnnapurnaLabs Unified 1GbE and 10GbE ethernet driver.
 *
 * Copyright (c) 2013 AnnapurnaLabs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/device.h>
#include <linux/stat.h>
#include <linux/sysfs.h>

#include "al_eth.h"

int al_eth_sysfs_init(struct device *dev);
void al_eth_sysfs_terminate(struct device *dev);
#include <mach/al_hal_udma.h>
#include <mach/al_hal_udma_debug.h>

#define to_ext_attr(x) container_of(x, struct dev_ext_attribute, attr)

static int al_eth_validate_small_copy_len(struct al_eth_adapter *adapter,
                                          unsigned long len) {
  if (len > adapter->netdev->mtu)
    return -EINVAL;

  return 0;
}

static ssize_t al_eth_store_small_copy_len(struct device *dev,
                                           struct device_attribute *attr,
                                           const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long small_copy_len;
  int err;

  err = kstrtoul(buf, 10, &small_copy_len);
  if (err < 0)
    return err;

  err = al_eth_validate_small_copy_len(adapter, small_copy_len);
  if (err)
    return err;

  rtnl_lock();
  adapter->small_copy_len = small_copy_len;
  rtnl_unlock();

  return len;
}

static ssize_t al_eth_show_small_copy_len(struct device *dev,
                                          struct device_attribute *attr,
                                          char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  return sprintf(buf, "%d\n", adapter->small_copy_len);
}

static struct device_attribute dev_attr_small_copy_len = {
    .attr = {.name = "small_copy_len", .mode = (S_IRUGO | S_IWUSR)},
    .show = al_eth_show_small_copy_len,
    .store = al_eth_store_small_copy_len,
};

static ssize_t al_eth_store_link_poll_interval(struct device *dev,
                                               struct device_attribute *attr,
                                               const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long link_poll_interval;
  int err;

  err = kstrtoul(buf, 10, &link_poll_interval);
  if (err < 0)
    return err;

  adapter->link_poll_interval = link_poll_interval;

  return len;
}

static ssize_t al_eth_show_link_poll_interval(struct device *dev,
                                              struct device_attribute *attr,
                                              char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  return sprintf(buf, "%d\n", adapter->link_poll_interval);
}

static struct device_attribute dev_attr_link_poll_interval = {
    .attr = {.name = "link_poll_interval", .mode = (S_IRUGO | S_IWUSR)},
    .show = al_eth_show_link_poll_interval,
    .store = al_eth_store_link_poll_interval,
};

static ssize_t al_eth_store_link_management_debug(struct device *dev,
                                                  struct device_attribute *attr,
                                                  const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long link_management_debug;
  int err;

  err = kstrtoul(buf, 10, &link_management_debug);
  if (err < 0)
    return err;

  al_eth_lm_debug_mode_set(&adapter->lm_context,
                           (link_management_debug) ? true : false);

  return len;
}

static struct device_attribute dev_attr_link_management_debug = {
    .attr = {.name = "link_management_debug", .mode = S_IWUSR},
    .show = NULL,
    .store = al_eth_store_link_management_debug,
};

static ssize_t al_eth_store_link_training_enable(struct device *dev,
                                                 struct device_attribute *attr,
                                                 const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long link_training_enable;
  int err;

  err = kstrtoul(buf, 10, &link_training_enable);
  if (err < 0)
    return err;

  adapter->lt_en = (link_training_enable == 0) ? AL_FALSE : AL_TRUE;

  if (adapter->up)
    dev_warn(dev,
             "%s this action will take place in the next activation (up)\n",
             __func__);

  return len;
}

static ssize_t al_eth_show_link_training_enable(struct device *dev,
                                                struct device_attribute *attr,
                                                char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  return sprintf(buf, "%d\n", adapter->lt_en);
}

static struct device_attribute dev_attr_link_training_enable = {
    .attr = {.name = "link_training_enable", .mode = (S_IRUGO | S_IWUSR)},
    .show = al_eth_show_link_training_enable,
    .store = al_eth_store_link_training_enable,
};

static ssize_t al_eth_store_force_1000_base_x(struct device *dev,
                                              struct device_attribute *attr,
                                              const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long force_1000_base_x;
  int err;

  err = kstrtoul(buf, 10, &force_1000_base_x);
  if (err < 0)
    return err;

  adapter->link_config.force_1000_base_x =
      (force_1000_base_x == 0) ? AL_FALSE : AL_TRUE;

  if (adapter->up)
    dev_warn(dev,
             "%s this action will take place in the next activation (up)\n",
             __func__);

  return len;
}

static ssize_t al_eth_show_force_1000_base_x(struct device *dev,
                                             struct device_attribute *attr,
                                             char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  return sprintf(buf, "%d\n", adapter->link_config.force_1000_base_x);
}

static struct device_attribute dev_attr_force_1000_base_x = {
    .attr = {.name = "force_1000_base_x", .mode = (S_IRUGO | S_IWUSR)},
    .show = al_eth_show_force_1000_base_x,
    .store = al_eth_store_force_1000_base_x,
};

static ssize_t al_eth_store_serdes_tx_param(struct device *dev,
                                            struct device_attribute *attr,
                                            const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  struct al_serdes_adv_tx_params tx_params;
  unsigned long param;
  int err;

  err = kstrtoul(buf, 16, &param);
  if (err < 0)
    return err;

  spin_lock(&adapter->serdes_config_lock);

  al_eth_lm_static_parameters_get(&adapter->lm_context, &tx_params, NULL);

  *(((uint8_t *)&tx_params) + offset) = param;

  spin_unlock(&adapter->serdes_config_lock);

  return len;
}

static ssize_t al_eth_show_serdes_tx_param(struct device *dev,
                                           struct device_attribute *attr,
                                           char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  struct al_serdes_adv_tx_params tx_params;
  uint8_t val;

  al_eth_lm_static_parameters_get(&adapter->lm_context, &tx_params, NULL);

  val = *(((uint8_t *)&tx_params) + offset);

  return sprintf(buf, "%d\n", val);
}

static ssize_t al_eth_store_serdes_rx_param(struct device *dev,
                                            struct device_attribute *attr,
                                            const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  struct al_serdes_adv_rx_params rx_params;
  unsigned long param;
  int err;

  err = kstrtoul(buf, 16, &param);
  if (err < 0)
    return err;

  spin_lock(&adapter->serdes_config_lock);

  al_eth_lm_static_parameters_get(&adapter->lm_context, NULL, &rx_params);

  *(((uint8_t *)&rx_params) + offset) = param;

  spin_unlock(&adapter->serdes_config_lock);

  return len;
}

static ssize_t al_eth_show_serdes_rx_param(struct device *dev,
                                           struct device_attribute *attr,
                                           char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  uintptr_t offset = (uintptr_t)ea->var;
  struct al_serdes_adv_rx_params rx_params;
  uint8_t val;

  al_eth_lm_static_parameters_get(&adapter->lm_context, NULL, &rx_params);

  val = *(((uint8_t *)&rx_params) + offset);

  return sprintf(buf, "%d\n", val);
}

#define AL_ETH_SERDES_TX_PARAMS_ATTR(_name)                                    \
  {__ATTR(serdes_tx_##_name, 0660, al_eth_show_serdes_tx_param,                \
          al_eth_store_serdes_tx_param),                                       \
   (void *)offsetof(struct al_serdes_adv_tx_params, _name)}

#define AL_ETH_SERDES_RX_PARAMS_ATTR(_name)                                    \
  {__ATTR(serdes_rx_##_name, 0660, al_eth_show_serdes_rx_param,                \
          al_eth_store_serdes_rx_param),                                       \
   (void *)offsetof(struct al_serdes_adv_rx_params, _name)}

struct dev_ext_attribute dev_attr_serdes_params[] = {
    AL_ETH_SERDES_TX_PARAMS_ATTR(amp),
    AL_ETH_SERDES_TX_PARAMS_ATTR(total_driver_units),
    AL_ETH_SERDES_TX_PARAMS_ATTR(c_plus_1),
    AL_ETH_SERDES_TX_PARAMS_ATTR(c_plus_2),
    AL_ETH_SERDES_TX_PARAMS_ATTR(c_minus_1),
    AL_ETH_SERDES_TX_PARAMS_ATTR(slew_rate),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dcgain),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_3db_freq),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_gain),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_first_tap_ctrl),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_secound_tap_ctrl),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_third_tap_ctrl),
    AL_ETH_SERDES_RX_PARAMS_ATTR(dfe_fourth_tap_ctrl),
    AL_ETH_SERDES_RX_PARAMS_ATTR(low_freq_agc_gain),
    AL_ETH_SERDES_RX_PARAMS_ATTR(precal_code_sel),
    AL_ETH_SERDES_RX_PARAMS_ATTR(high_freq_agc_boost)};

static ssize_t
al_eth_store_max_rx_buff_alloc_size(struct device *dev,
                                    struct device_attribute *attr,
                                    const char *buf, size_t len) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);
  unsigned long max_rx_buff_alloc_size;
  int err;

  err = kstrtoul(buf, 10, &max_rx_buff_alloc_size);
  if (err < 0)
    return err;

  adapter->max_rx_buff_alloc_size = max_rx_buff_alloc_size;

  return len;
}

static ssize_t al_eth_show_max_rx_buff_alloc_size(struct device *dev,
                                                  struct device_attribute *attr,
                                                  char *buf) {
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  return sprintf(buf, "%d\n", adapter->max_rx_buff_alloc_size);
}

static struct device_attribute dev_attr_max_rx_buff_alloc_size = {
    .attr = {.name = "max_rx_buff_alloc_size", .mode = (S_IRUGO | S_IWUSR)},
    .show = al_eth_show_max_rx_buff_alloc_size,
    .store = al_eth_store_max_rx_buff_alloc_size,
};

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

static ssize_t rd_udma_dump(struct device *dev, struct device_attribute *attr,
                            char *buf);

static ssize_t wr_udma_dump(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count);

/* Device attrs - udma debug */
static struct dev_ext_attribute dev_attr_udma_debug[] = {
    UDMA_DUMP_PREP_ATTR(m2s_regs, UDMA_DUMP_M2S_REGS),
    UDMA_DUMP_PREP_ATTR(m2s_q_struct, UDMA_DUMP_M2S_Q_STRUCT),
    UDMA_DUMP_PREP_ATTR(m2s_q_pointers, UDMA_DUMP_M2S_Q_POINTERS),
    UDMA_DUMP_PREP_ATTR(s2m_regs, UDMA_DUMP_S2M_REGS),
    UDMA_DUMP_PREP_ATTR(s2m_q_struct, UDMA_DUMP_S2M_Q_STRUCT),
    UDMA_DUMP_PREP_ATTR(s2m_q_pointers, UDMA_DUMP_S2M_Q_POINTERS)};

/******************************************************************************
 *****************************************************************************/
int al_eth_sysfs_init(struct device *dev) {
  int status = 0;

  int i;

  if (device_create_file(dev, &dev_attr_small_copy_len))
    dev_info(dev, "failed to create small_copy_len sysfs entry");

  if (device_create_file(dev, &dev_attr_link_poll_interval))
    dev_info(dev, "failed to create link_poll_interval sysfs entry");

  if (device_create_file(dev, &dev_attr_link_management_debug))
    dev_info(dev, "failed to create link_management_debug sysfs entry");

  if (device_create_file(dev, &dev_attr_max_rx_buff_alloc_size))
    dev_info(dev, "failed to create max_rx_buff_alloc_size sysfs entry");

  if (device_create_file(dev, &dev_attr_link_training_enable))
    dev_info(dev, "failed to create link_training_enable sysfs entry");

  if (device_create_file(dev, &dev_attr_force_1000_base_x))
    dev_info(dev, "failed to create force_1000_base_x sysfs entry");

  for (i = 0; i < ARRAY_SIZE(dev_attr_serdes_params); i++) {
    status =
        sysfs_create_file(&dev->kobj, &dev_attr_serdes_params[i].attr.attr);
    if (status) {
      dev_err(dev, "%s: sysfs_create_file(serdes_params %d) failed\n", __func__,
              i);
      goto done;
    }
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

  return status;
}

/******************************************************************************
 *****************************************************************************/
void al_eth_sysfs_terminate(struct device *dev) {
  int i;

  device_remove_file(dev, &dev_attr_small_copy_len);
  device_remove_file(dev, &dev_attr_link_poll_interval);
  device_remove_file(dev, &dev_attr_link_management_debug);
  device_remove_file(dev, &dev_attr_max_rx_buff_alloc_size);
  device_remove_file(dev, &dev_attr_link_training_enable);
  device_remove_file(dev, &dev_attr_force_1000_base_x);

  for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++)
    sysfs_remove_file(&dev->kobj, &dev_attr_udma_debug[i].attr.attr);

  for (i = 0; i < ARRAY_SIZE(dev_attr_serdes_params); i++) {
    sysfs_remove_file(&dev->kobj, &dev_attr_serdes_params[i].attr.attr);
  }
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

struct al_eth_adapter;

/******************************************************************************
 *****************************************************************************/
static ssize_t wr_udma_dump(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count) {
  int err;
  int q_id;
  unsigned long val;
  struct dev_ext_attribute *ea = to_ext_attr(attr);
  enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
  enum al_udma_ring_type ring_type = AL_RING_COMPLETION;
  struct al_eth_adapter *adapter = dev_get_drvdata(dev);

  err = kstrtoul(buf, 16, &val);
  if (err < 0)
    return err;

  switch (dump_type) {
  case UDMA_DUMP_M2S_REGS:
    al_udma_regs_print(al_eth_udma_get(adapter, 1), val);
    break;
  case UDMA_DUMP_S2M_REGS:
    al_udma_regs_print(al_eth_udma_get(adapter, 0), val);
    break;
  case UDMA_DUMP_M2S_Q_STRUCT:
    al_udma_q_struct_print(al_eth_udma_get(adapter, 1), val);
    break;
  case UDMA_DUMP_S2M_Q_STRUCT:
    al_udma_q_struct_print(al_eth_udma_get(adapter, 0), val);
    break;
  case UDMA_DUMP_M2S_Q_POINTERS:
    if (val & 0x10)
      ring_type = AL_RING_SUBMISSION;
    q_id = val & 0xf;
    al_udma_ring_print(al_eth_udma_get(adapter, 1), q_id, ring_type);
    break;
  case UDMA_DUMP_S2M_Q_POINTERS:
    if (val & 0x10)
      ring_type = AL_RING_SUBMISSION;
    q_id = val & 0xf;
    al_udma_ring_print(al_eth_udma_get(adapter, 0), q_id, ring_type);
    break;
  default:
    break;
  }

  return count;
}
