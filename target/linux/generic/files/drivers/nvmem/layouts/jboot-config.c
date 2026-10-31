// SPDX-License-Identifier: GPL-2.0-only
/*
 * NVMEM layout driver for the factory data block of AMIT JBOOT devices
 *
 * Devices booting the AMIT "JBOOT" loader (D-Link DWR/DIR series, Zyxel LTE
 * series, ...) keep their factory data (base MAC address, wireless
 * calibration data, serial number, credentials, ...) in a config partition.
 * The factory block is a STAG header followed by a CSXF header and a body of
 * tightly packed, variable length records:
 *
 *   STAG (16 bytes, little endian)
 *     u8  cmark          0x02 for the factory block
 *     u8  id             0x02 for the factory block
 *     u16 magic          0x2b24
 *     u32 time_stamp
 *     u32 image_length   CSXF header + body
 *     u16 image_checksum
 *     u16 tag_checksum   ~jboot_checksum() over the first 14 bytes
 *
 *   CSXF (16 bytes, little endian)
 *     u16 magic          0x5343 ("CS")
 *     u16 checksum       ~jboot_checksum() over header (checksum zeroed) + body
 *     u32 body_length
 *     u8  body_encoding  0 = plain (the factory block is never compressed)
 *     u8  reserved[3]
 *     u32 raw_length
 *
 *   record (6 byte header, little endian, no alignment)
 *     u8  id
 *     u8  type           0x42 = config, 0xf5 = factory/calibration
 *     u16 flags          0x8000
 *     u16 length
 *     u8  data[length]
 *
 * The record list is terminated by an all-zero record header. Since some of
 * the records hold per-unit strings of varying length (keys, PINs, ...), the
 * absolute offset of e.g. the MAC address differs between units of the same
 * model, so the cells cannot be described by a fixed layout.
 *
 * The layout must be attached to a (sub)partition that starts with the
 * factory block STAG header.
 *
 * Copyright (C) 2026 Ulv Michel <ulv@on2.de>
 * Format documentation based on jboot_config_read by
 * Paweł Dembicki <paweldembicki@gmail.com>
 */

#include <linux/etherdevice.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/nvmem-provider.h>
#include <linux/of.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
#include <linux/unaligned.h>
#else
#include <asm/unaligned.h>
#endif

#define JBOOT_STAG_SIZE			16
#define JBOOT_STAG_MAGIC		0x2b24
#define JBOOT_STAG_ID_FACTORY		0x02

#define JBOOT_CSXF_SIZE			16
#define JBOOT_CSXF_MAGIC		0x5343
#define JBOOT_CSXF_ENC_PLAIN		0x00

#define JBOOT_REC_HDR_SIZE		6
#define JBOOT_REC_FLAGS			0x8000

#define JBOOT_REC_TYPE_CONFIG		0x42
#define JBOOT_REC_TYPE_FACTORY		0xf5

#define JBOOT_REC_ID_MAC		0x30
#define JBOOT_REC_ID_EEPROM		0x00

/* sanity limit, the factory block is a few hundred bytes in practice */
#define JBOOT_MAX_BODY_SIZE		SZ_64K

struct jboot_cell {
	u8 type;
	u8 id;
	const char *name;
	nvmem_cell_post_process_t read_post_process;
	unsigned int bytes;	/* 0: use record length */
};

static int jboot_mac_read_cb(void *priv, const char *id, int index,
			     unsigned int offset, void *buf, size_t bytes)
{
	if (bytes != ETH_ALEN)
		return -EINVAL;

	if (index < 0)
		return -EINVAL;

	if (!is_valid_ether_addr(buf))
		return -EINVAL;

	eth_addr_add(buf, index);

	return 0;
}

static const struct jboot_cell jboot_cells[] = {
	{
		.type = JBOOT_REC_TYPE_CONFIG,
		.id = JBOOT_REC_ID_MAC,
		.name = "mac-address",
		.read_post_process = jboot_mac_read_cb,
		.bytes = ETH_ALEN,
	}, {
		.type = JBOOT_REC_TYPE_FACTORY,
		.id = JBOOT_REC_ID_EEPROM,
		.name = "eeprom",
	},
};

/*
 * One's complement style 16 bit sum as used by JBOOT. Words are little
 * endian and may be unaligned. For an odd length, the trailing byte is added
 * and 0xff subtracted, matching the vendor implementation.
 */
static u16 jboot_checksum(const u8 *data, size_t size)
{
	u32 sum = 0;

	while (size > 1) {
		sum += get_unaligned_le16(data);
		data += 2;
		size -= 2;
		sum = (sum & 0xffff) + (sum >> 16);
	}

	if (size) {
		sum += *data;
		sum -= 0xff;
	}

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return sum;
}

static bool jboot_stag_is_valid(struct device *dev, const u8 *stag)
{
	u16 csum;

	if (stag[0] != JBOOT_STAG_ID_FACTORY ||
	    stag[1] != JBOOT_STAG_ID_FACTORY ||
	    get_unaligned_le16(stag + 2) != JBOOT_STAG_MAGIC) {
		dev_err(dev, "no JBOOT factory STAG header found\n");
		return false;
	}

	csum = ~jboot_checksum(stag, JBOOT_STAG_SIZE - 2);
	if (csum != get_unaligned_le16(stag + 14)) {
		dev_err(dev, "invalid STAG checksum (got 0x%04x, expected 0x%04x)\n",
			get_unaligned_le16(stag + 14), csum);
		return false;
	}

	return true;
}

static int jboot_csxf_parse(struct device *dev, const u8 *csxf,
			    size_t avail, u32 *body_len)
{
	u32 len, raw_len;
	u8 enc;

	if (get_unaligned_le16(csxf) != JBOOT_CSXF_MAGIC) {
		dev_err(dev, "invalid CSXF magic 0x%04x\n",
			get_unaligned_le16(csxf));
		return -EINVAL;
	}

	len = get_unaligned_le32(csxf + 4);
	enc = csxf[8];
	raw_len = get_unaligned_le32(csxf + 12);

	if (enc != JBOOT_CSXF_ENC_PLAIN) {
		dev_err(dev, "unsupported CSXF body encoding 0x%02x\n", enc);
		return -EOPNOTSUPP;
	}

	if (len != raw_len || len > avail || len > JBOOT_MAX_BODY_SIZE) {
		dev_err(dev, "invalid CSXF body length 0x%x/0x%x\n",
			len, raw_len);
		return -EINVAL;
	}

	*body_len = len;

	return 0;
}

static bool jboot_csxf_csum_is_valid(struct device *dev, u8 *csxf, u32 body_len)
{
	u16 stored, csum;

	/* the checksum is calculated with the checksum field zeroed */
	stored = get_unaligned_le16(csxf + 2);
	put_unaligned_le16(0, csxf + 2);
	csum = ~jboot_checksum(csxf, JBOOT_CSXF_SIZE + body_len);
	put_unaligned_le16(stored, csxf + 2);

	if (csum != stored) {
		dev_err(dev, "invalid CSXF checksum (got 0x%04x, expected 0x%04x)\n",
			stored, csum);
		return false;
	}

	return true;
}

static const struct jboot_cell *jboot_find_cell(u8 type, u8 id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(jboot_cells); i++)
		if (jboot_cells[i].type == type && jboot_cells[i].id == id)
			return &jboot_cells[i];

	return NULL;
}

struct jboot_cell_pos {
	unsigned int offset;
	unsigned int bytes;
	bool found;
};

/*
 * Walk the complete record list first and only register cells if the whole
 * block is consistent, so a corrupted block never yields partial results.
 */
static int jboot_walk_records(struct device *dev, const u8 *body, u32 body_len,
			      unsigned int body_offset,
			      struct jboot_cell_pos *cpos)
{
	u32 pos = 0;

	while (pos + JBOOT_REC_HDR_SIZE <= body_len) {
		const struct jboot_cell *jc;
		const u8 *rec = body + pos;
		struct jboot_cell_pos *cp;
		u16 flags, len;
		u8 id, type;

		id = rec[0];
		type = rec[1];
		flags = get_unaligned_le16(rec + 2);
		len = get_unaligned_le16(rec + 4);

		/* all-zero record header terminates the list */
		if (!id && !type && !flags)
			return 0;

		if (flags != JBOOT_REC_FLAGS) {
			dev_err(dev, "invalid record header at 0x%x\n",
				body_offset + pos);
			return -EINVAL;
		}

		pos += JBOOT_REC_HDR_SIZE;
		if (len > body_len - pos) {
			dev_err(dev, "record 0x%02x/0x%02x at 0x%x exceeds body\n",
				type, id, body_offset + pos - JBOOT_REC_HDR_SIZE);
			return -EINVAL;
		}

		jc = jboot_find_cell(type, id);
		if (!jc)
			goto next;

		cp = &cpos[jc - jboot_cells];
		if (cp->found) {
			dev_warn(dev, "ignoring duplicate record 0x%02x/0x%02x\n",
				 type, id);
			goto next;
		}

		if (jc->bytes && len < jc->bytes) {
			dev_err(dev, "record 0x%02x/0x%02x too short (%u bytes)\n",
				type, id, len);
			return -EINVAL;
		}

		cp->found = true;
		cp->offset = body_offset + pos;
		cp->bytes = jc->bytes ?: len;
next:
		pos += len;
	}

	/* no terminator, but every record was within bounds */
	return 0;
}

static int jboot_add_cells(struct device *dev, struct nvmem_device *nvmem,
			   const u8 *body, u32 body_len, unsigned int body_offset)
{
	struct jboot_cell_pos cpos[ARRAY_SIZE(jboot_cells)] = {};
	struct device_node *layout_np;
	int i, ret;

	ret = jboot_walk_records(dev, body, body_len, body_offset, cpos);
	if (ret)
		return ret;

	layout_np = of_nvmem_layout_get_container(nvmem);
	if (!layout_np)
		return -ENOENT;

	for (i = 0; i < ARRAY_SIZE(jboot_cells); i++) {
		struct nvmem_cell_info info = {};

		info.np = of_get_child_by_name(layout_np, jboot_cells[i].name);

		if (!cpos[i].found) {
			if (info.np)
				dev_warn(dev, "no record found for cell %s\n",
					 jboot_cells[i].name);
			of_node_put(info.np);
			continue;
		}

		info.name = jboot_cells[i].name;
		info.offset = cpos[i].offset;
		info.bytes = cpos[i].bytes;
		info.read_post_process = jboot_cells[i].read_post_process;

		ret = nvmem_add_one_cell(nvmem, &info);
		if (ret) {
			of_node_put(info.np);
			break;
		}

		dev_dbg(dev, "%s: 0x%x bytes at 0x%x\n", info.name,
			info.bytes, info.offset);
	}

	of_node_put(layout_np);

	return ret;
}

static int jboot_parse(struct nvmem_layout *layout)
{
	struct nvmem_device *nvmem = layout->nvmem;
	struct device *dev = &layout->dev;
	u8 hdr[JBOOT_STAG_SIZE + JBOOT_CSXF_SIZE];
	size_t size, avail;
	u32 body_len;
	u8 *buf;
	int ret;

	size = nvmem_dev_size(nvmem);
	if (size < sizeof(hdr))
		return -EINVAL;

	ret = nvmem_device_read(nvmem, 0, sizeof(hdr), hdr);
	if (ret < 0)
		return ret;
	if (ret != sizeof(hdr))
		return -EIO;

	if (!jboot_stag_is_valid(dev, hdr))
		return -EINVAL;

	avail = size - sizeof(hdr);
	ret = jboot_csxf_parse(dev, hdr + JBOOT_STAG_SIZE, avail, &body_len);
	if (ret)
		return ret;

	buf = kmalloc(JBOOT_CSXF_SIZE + body_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	ret = nvmem_device_read(nvmem, JBOOT_STAG_SIZE,
				JBOOT_CSXF_SIZE + body_len, buf);
	if (ret < 0)
		goto out;
	if (ret != JBOOT_CSXF_SIZE + body_len) {
		ret = -EIO;
		goto out;
	}

	if (!jboot_csxf_csum_is_valid(dev, buf, body_len)) {
		ret = -EINVAL;
		goto out;
	}

	ret = jboot_add_cells(dev, nvmem, buf + JBOOT_CSXF_SIZE, body_len,
			      sizeof(hdr));

out:
	kfree(buf);
	return ret;
}

static int jboot_probe(struct nvmem_layout *layout)
{
	layout->add_cells = jboot_parse;

	return nvmem_layout_register(layout);
}

static void jboot_remove(struct nvmem_layout *layout)
{
	nvmem_layout_unregister(layout);
}

static const struct of_device_id jboot_of_match_table[] = {
	{ .compatible = "amit,jboot-config" },
	{},
};
MODULE_DEVICE_TABLE(of, jboot_of_match_table);

static struct nvmem_layout_driver jboot_layout = {
	.driver = {
		.name = "jboot-config-layout",
		.of_match_table = jboot_of_match_table,
	},
	.probe = jboot_probe,
	.remove = jboot_remove,
};

/*
 * Register early (built-in case): the layout device is created as soon as
 * the flash partition is registered, and consumers such as the ralink
 * ethernet driver read their MAC address only once during probe without
 * handling -EPROBE_DEFER. Binding the layout while the partition registers
 * makes the cells available just like fixed-layout cells.
 * The nvmem layout bus itself is registered by nvmem_init() at
 * subsys_initcall level, which is linked before this driver.
 */
static int __init jboot_layout_init(void)
{
	return nvmem_layout_driver_register(&jboot_layout);
}
subsys_initcall(jboot_layout_init);

static void __exit jboot_layout_exit(void)
{
	nvmem_layout_driver_unregister(&jboot_layout);
}
module_exit(jboot_layout_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ulv Michel <ulv@on2.de>");
MODULE_DESCRIPTION("NVMEM layout driver for AMIT JBOOT factory config blocks");
