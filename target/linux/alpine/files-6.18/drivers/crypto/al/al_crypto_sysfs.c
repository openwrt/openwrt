/*
 * Annapurna Labs Crypto Linux driver - sysfs support
 * Copyright(c) 2013 Annapurna Labs.
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
#include <linux/device.h>

#include <mach/al_hal_udma_debug.h>

#include "al_crypto.h"

#define to_ext_attr(x) container_of(x, struct dev_ext_attribute, attr)

#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
static void al_crypto_release_channel(struct kobject *kobj)
{
	struct al_crypto_chan *chan =
			container_of(kobj, struct al_crypto_chan, kobj);

	kfree(chan);
}

/******************************************************************************
 *****************************************************************************/
struct al_crypto_chan_attr {
	struct attribute attr;
	size_t offset;
	ssize_t (*show) (struct al_crypto_chan *chan, size_t offset, char *buf);
	ssize_t (*store) (struct al_crypto_chan *chan, size_t offset,
			const char *buf, size_t size);
};

/******************************************************************************
 *****************************************************************************/
static ssize_t al_crypto_chan_attr_show(struct kobject *kobj,
		struct attribute *attr, char *buf)
{
	struct al_crypto_chan *chan =
			container_of(kobj, struct al_crypto_chan, kobj);
	struct al_crypto_chan_attr *chan_attr =
			container_of(attr, struct al_crypto_chan_attr, attr);
	ssize_t ret = 0;

	if (chan_attr->show)
		ret = chan_attr->show(chan, chan_attr->offset, buf);

	return ret;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t al_crypto_chan_attr_store(struct kobject *kobj,
		struct attribute *attr, const char *buf, size_t size)
{
	struct al_crypto_chan *chan =
			container_of(kobj, struct al_crypto_chan, kobj);
	struct al_crypto_chan_attr *chan_attr =
			container_of(attr, struct al_crypto_chan_attr, attr);
	ssize_t ret = 0;

	if (chan_attr->store)
		ret = chan_attr->store(chan, chan_attr->offset, buf, size);

	return ret;
}

/******************************************************************************
 *****************************************************************************/
static const struct sysfs_ops al_crypto_chan_sysfs_ops = {
	.show = al_crypto_chan_attr_show,
	.store = al_crypto_chan_attr_store,
};

/******************************************************************************
 *****************************************************************************/
static ssize_t al_crypto_chan_rd_stats_prep(
	struct al_crypto_chan *chan,
	size_t offset,
	char *buf)
{
	uint64_t val;
	ssize_t size;

	spin_lock_bh(&chan->prep_lock);

	val = *(uint64_t *)(((uint8_t *)&chan->stats_prep) + offset);

	spin_unlock_bh(&chan->prep_lock);

	size = sprintf(buf, "%llu\n", val);

	return size;
}

static ssize_t al_crypto_chan_rd_stats_comp(
	struct al_crypto_chan *chan,
	size_t offset,
	char *buf)
{
	uint64_t val;
	ssize_t size;

	spin_lock_bh(&chan->cleanup_lock);

	val = *(uint64_t *)(((uint8_t *)&chan->stats_comp) + offset);

	spin_unlock_bh(&chan->cleanup_lock);

	size = sprintf(buf, "%llu\n", val);

	return size;
}

static ssize_t al_crypto_chan_rd_stats_gen(
	struct al_crypto_chan *chan,
	size_t offset,
	char *buf)
{
	uint64_t val;
	ssize_t size;

	val = *(uint64_t *)(((uint8_t *)&chan->stats_gen) + offset);

	size = sprintf(buf, "%llu\n", val);

	return size;
}


#define al_crypto_chan_init_attr(_name, _group)				\
static struct al_crypto_chan_attr al_crypto_chan_##_name = {		\
	.attr	= { .name = __stringify(_name), .mode = S_IRUGO },	\
	.offset	= offsetof(struct al_crypto_chan_stats_##_group, _name), \
	.show = al_crypto_chan_rd_stats_##_group,			\
	.store = NULL,							\
}

/* Channel attrs */
al_crypto_chan_init_attr(skcipher_encrypt_reqs, prep);
al_crypto_chan_init_attr(skcipher_encrypt_bytes, prep);
al_crypto_chan_init_attr(skcipher_decrypt_reqs, prep);
al_crypto_chan_init_attr(skcipher_decrypt_bytes, prep);
al_crypto_chan_init_attr(aead_encrypt_hash_reqs, prep);
al_crypto_chan_init_attr(aead_encrypt_bytes, prep);
al_crypto_chan_init_attr(aead_hash_bytes, prep);
al_crypto_chan_init_attr(aead_decrypt_validate_reqs, prep);
al_crypto_chan_init_attr(aead_decrypt_bytes, prep);
al_crypto_chan_init_attr(aead_validate_bytes, prep);
al_crypto_chan_init_attr(ahash_reqs, prep);
al_crypto_chan_init_attr(ahash_bytes, prep);
al_crypto_chan_init_attr(crc_reqs, prep);
al_crypto_chan_init_attr(crc_bytes, prep);
al_crypto_chan_init_attr(cache_misses, prep);
al_crypto_chan_init_attr(skcipher_reqs_le512, prep);
al_crypto_chan_init_attr(skcipher_reqs_512_2048, prep);
al_crypto_chan_init_attr(skcipher_reqs_2048_4096, prep);
al_crypto_chan_init_attr(skcipher_reqs_gt4096, prep);
al_crypto_chan_init_attr(aead_reqs_le512, prep);
al_crypto_chan_init_attr(aead_reqs_512_2048, prep);
al_crypto_chan_init_attr(aead_reqs_2048_4096, prep);
al_crypto_chan_init_attr(aead_reqs_gt4096, prep);
al_crypto_chan_init_attr(ahash_reqs_le512, prep);
al_crypto_chan_init_attr(ahash_reqs_512_2048, prep);
al_crypto_chan_init_attr(ahash_reqs_2048_4096, prep);
al_crypto_chan_init_attr(ahash_reqs_gt4096, prep);
al_crypto_chan_init_attr(crc_reqs_le512, prep);
al_crypto_chan_init_attr(crc_reqs_512_2048, prep);
al_crypto_chan_init_attr(crc_reqs_2048_4096, prep);
al_crypto_chan_init_attr(crc_reqs_gt4096, prep);
al_crypto_chan_init_attr(redundant_int_cnt, comp);
al_crypto_chan_init_attr(max_active_descs, comp);
al_crypto_chan_init_attr(skcipher_tfms, gen);
al_crypto_chan_init_attr(aead_tfms, gen);
al_crypto_chan_init_attr(ahash_tfms, gen);
al_crypto_chan_init_attr(crc_tfms, gen);

static struct attribute *al_crypto_chan_default_attrs[] = {
	&al_crypto_chan_skcipher_encrypt_reqs.attr,
	&al_crypto_chan_skcipher_encrypt_bytes.attr,
	&al_crypto_chan_skcipher_decrypt_reqs.attr,
	&al_crypto_chan_skcipher_decrypt_bytes.attr,
	&al_crypto_chan_aead_encrypt_hash_reqs.attr,
	&al_crypto_chan_aead_encrypt_bytes.attr,
	&al_crypto_chan_aead_hash_bytes.attr,
	&al_crypto_chan_aead_decrypt_validate_reqs.attr,
	&al_crypto_chan_aead_decrypt_bytes.attr,
	&al_crypto_chan_aead_validate_bytes.attr,
	&al_crypto_chan_ahash_reqs.attr,
	&al_crypto_chan_ahash_bytes.attr,
	&al_crypto_chan_cache_misses.attr,
	&al_crypto_chan_skcipher_reqs_le512.attr,
	&al_crypto_chan_skcipher_reqs_512_2048.attr,
	&al_crypto_chan_skcipher_reqs_2048_4096.attr,
	&al_crypto_chan_skcipher_reqs_gt4096.attr,
	&al_crypto_chan_aead_reqs_le512.attr,
	&al_crypto_chan_aead_reqs_512_2048.attr,
	&al_crypto_chan_aead_reqs_2048_4096.attr,
	&al_crypto_chan_aead_reqs_gt4096.attr,
	&al_crypto_chan_ahash_reqs_le512.attr,
	&al_crypto_chan_ahash_reqs_512_2048.attr,
	&al_crypto_chan_ahash_reqs_2048_4096.attr,
	&al_crypto_chan_ahash_reqs_gt4096.attr,

	&al_crypto_chan_redundant_int_cnt.attr,
	&al_crypto_chan_max_active_descs.attr,

	&al_crypto_chan_skcipher_tfms.attr,
	&al_crypto_chan_aead_tfms.attr,
	&al_crypto_chan_ahash_tfms.attr,
	NULL
};

ATTRIBUTE_GROUPS(al_crypto_chan_default);

static struct attribute *al_crypto_crc_chan_default_attrs[] = {
	&al_crypto_chan_crc_reqs.attr,
	&al_crypto_chan_crc_bytes.attr,
	&al_crypto_chan_cache_misses.attr,
	&al_crypto_chan_crc_reqs_le512.attr,
	&al_crypto_chan_crc_reqs_512_2048.attr,
	&al_crypto_chan_crc_reqs_2048_4096.attr,
	&al_crypto_chan_crc_reqs_gt4096.attr,

	&al_crypto_chan_redundant_int_cnt.attr,
	&al_crypto_chan_max_active_descs.attr,

	&al_crypto_chan_crc_tfms.attr,
	NULL
};

ATTRIBUTE_GROUPS(al_crypto_crc_chan_default);

static struct kobj_type chan_ktype = {
	.sysfs_ops = &al_crypto_chan_sysfs_ops,
	.release = al_crypto_release_channel,
	.default_groups = al_crypto_chan_default_groups,
};

static struct kobj_type crc_chan_ktype = {
	.sysfs_ops = &al_crypto_chan_sysfs_ops,
	.release = al_crypto_release_channel,
	.default_groups = al_crypto_crc_chan_default_groups,
};

enum udma_dump_type {
	UDMA_DUMP_M2S_REGS,
	UDMA_DUMP_M2S_Q_STRUCT,
	UDMA_DUMP_M2S_Q_POINTERS,
	UDMA_DUMP_S2M_REGS,
	UDMA_DUMP_S2M_Q_STRUCT,
	UDMA_DUMP_S2M_Q_POINTERS
};

/******************************************************************************
 *****************************************************************************/
static ssize_t rd_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
	ssize_t rc = 0;

	switch (dump_type) {
	case UDMA_DUMP_M2S_REGS:
	case UDMA_DUMP_S2M_REGS:
		rc = sprintf(
			buf,
			"Write mask to dump corresponding udma regs\n");
		break;
	case UDMA_DUMP_M2S_Q_STRUCT:
	case UDMA_DUMP_S2M_Q_STRUCT:
		rc = sprintf(
			buf,
			"Write q num to dump correspoding q struct\n");
		break;
	case UDMA_DUMP_M2S_Q_POINTERS:
	case UDMA_DUMP_S2M_Q_POINTERS:
		rc = sprintf(
			buf,
			"Write q num (in hex) and add 1 for submission ring,"
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
static ssize_t wr_udma_dump(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	int err;
	int q_id;
	unsigned long val;
	struct al_udma* dma;
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum udma_dump_type dump_type = (enum udma_dump_type)ea->var;
	enum al_udma_ring_type ring_type = AL_RING_COMPLETION;
	struct al_crypto_device *device = dev_get_drvdata(dev);

	err = kstrtoul(buf, 16, &val);
	if (err < 0)
		return err;

	switch (dump_type) {
	case UDMA_DUMP_M2S_REGS:
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_TX, &dma);
		al_udma_regs_print(dma, val);
		break;
	case UDMA_DUMP_S2M_REGS:
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_RX, &dma);
		al_udma_regs_print(dma, val);
		break;
	case UDMA_DUMP_M2S_Q_STRUCT:
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_TX, &dma);
		al_udma_q_struct_print(dma, val);
		break;
	case UDMA_DUMP_S2M_Q_STRUCT:
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_RX, &dma);
		al_udma_q_struct_print(dma, val);
		break;
	case UDMA_DUMP_M2S_Q_POINTERS:
		if (val & 0x10)
			ring_type = AL_RING_SUBMISSION;
		q_id = val & 0xf;
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_TX, &dma);
		al_udma_ring_print(dma, q_id, ring_type);
		break;
	case UDMA_DUMP_S2M_Q_POINTERS:
		if (val & 0x10)
			ring_type = AL_RING_SUBMISSION;
		q_id = val & 0xf;
		al_ssm_dma_handle_get(&device->hal_crypto, UDMA_RX, &dma);
		al_udma_ring_print(dma, q_id, ring_type);
		break;
	default:
		break;
	}

	return count;
}

#define UDMA_DUMP_PREP_ATTR(_name, _type) {\
	__ATTR(udma_dump_##_name, 0660, rd_udma_dump, wr_udma_dump),\
	(void*)_type }

/* Device attrs - udma debug */
static struct dev_ext_attribute dev_attr_udma_debug[] = {
	UDMA_DUMP_PREP_ATTR(m2s_regs, UDMA_DUMP_M2S_REGS),
	UDMA_DUMP_PREP_ATTR(m2s_q_struct, UDMA_DUMP_M2S_Q_STRUCT),
	UDMA_DUMP_PREP_ATTR(m2s_q_pointers, UDMA_DUMP_M2S_Q_POINTERS),
	UDMA_DUMP_PREP_ATTR(s2m_regs, UDMA_DUMP_S2M_REGS),
	UDMA_DUMP_PREP_ATTR(s2m_q_struct, UDMA_DUMP_S2M_Q_STRUCT),
	UDMA_DUMP_PREP_ATTR(s2m_q_pointers, UDMA_DUMP_S2M_Q_POINTERS)
};
#endif /* CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS */

enum al_crypto_config_type {
	CONFIG_INT_MODERATION
};

/******************************************************************************
 *****************************************************************************/
static ssize_t al_crypto_rd_config(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum al_crypto_config_type config_type =
			(enum al_crypto_config_type)ea->var;
	struct al_crypto_device *device = dev_get_drvdata(dev);
	ssize_t rc = 0;

	switch (config_type) {
	case CONFIG_INT_MODERATION:
		rc = sprintf(buf, "%d\n", al_crypto_get_int_moderation(device));
		break;
	default:
		break;
	}

	return rc;
}

/******************************************************************************
 *****************************************************************************/
static ssize_t al_crypto_wr_config(
	struct device *dev,
	struct device_attribute *attr,
	const char *buf,
	size_t count)
{
	int err;
	unsigned long val;
	struct dev_ext_attribute *ea = to_ext_attr(attr);
	enum al_crypto_config_type config_type =
			(enum al_crypto_config_type)ea->var;
	struct al_crypto_device *device = dev_get_drvdata(dev);

	err = kstrtoul(buf, 10, &val);
	if (err < 0)
		return err;

	switch (config_type) {
	case CONFIG_INT_MODERATION:
		al_crypto_set_int_moderation(device, val);
		break;
	default:
		break;
	}

	return count;
}

#define CONFIG_PREP_ATTR(_name, _type) {\
	__ATTR(_name, 0660,\
			al_crypto_rd_config, al_crypto_wr_config),\
	(void*)_type }

/* Device attrs - config */
static struct dev_ext_attribute dev_attr_config[] = {
	CONFIG_PREP_ATTR(int_moderation, CONFIG_INT_MODERATION),
};

/******************************************************************************
 *****************************************************************************/
void al_crypto_free_channel(struct al_crypto_chan *chan)
{
#ifndef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
	kfree(chan);
#endif
}

/******************************************************************************
 *****************************************************************************/
int al_crypto_sysfs_init(
	struct al_crypto_device *device)
{
	int rc = 0;
	struct device* dev = &device->pdev->dev;
	int i;

#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS

	device->channels_kset =
			kset_create_and_add("channels", NULL,
					&device->pdev->dev.kobj);
	if (!device->channels_kset)
		return -ENOMEM;

	for (i = 0; i < device->num_channels; i++) {
		struct al_crypto_chan *chan = device->channels[i];
		chan->kobj.kset = device->channels_kset;
		if (chan->type == AL_CRYPT_AUTH_Q)
			rc = kobject_init_and_add(&chan->kobj, &chan_ktype,
					NULL, "chan%d", i);
		else
			rc = kobject_init_and_add(&chan->kobj, &crc_chan_ktype,
					NULL, "chan%d", i);
		if (rc) {
			int j;
			for (j = 0; j <= i; j++)
				kobject_put(&device->channels[j]->kobj);
			kset_unregister(device->channels_kset);
			for (j = i+1; j < device->num_channels; j++)
				kfree(device->channels[j]);
			return -ENOMEM;
		}

		kobject_uevent(&chan->kobj, KOBJ_ADD);
	}

	for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++)
		rc = sysfs_create_file(
				&dev->kobj,
				&dev_attr_udma_debug[i].attr.attr);
#endif
	for (i = 0; i < ARRAY_SIZE(dev_attr_config); i++)
		rc = sysfs_create_file(
				&dev->kobj,
				&dev_attr_config[i].attr.attr);

	return rc;
}

/******************************************************************************
 *****************************************************************************/
void al_crypto_sysfs_terminate(
	struct al_crypto_device *device)
{
	int i;
	struct device* dev = &device->pdev->dev;

	for (i = 0; i < ARRAY_SIZE(dev_attr_config); i++)
		sysfs_remove_file(
				&dev->kobj,
				&dev_attr_config[i].attr.attr);

#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS

	for (i = 0; i < ARRAY_SIZE(dev_attr_udma_debug); i++)
		sysfs_remove_file(
			&dev->kobj,
			&dev_attr_udma_debug[i].attr.attr);

	for (i = 0; i < device->num_channels; i++)
		kobject_put(&device->channels[i]->kobj);
	kset_unregister(device->channels_kset);
#endif
}
