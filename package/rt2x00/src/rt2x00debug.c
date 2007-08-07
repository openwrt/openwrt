/*
	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00lib
	Abstract: rt2x00 debugfs specific routines.
	Supported chipsets: RT2460, RT2560, RT2570,
	rt2561, rt2561s, rt2661, rt2571W & rt2671.
 */

#include <linux/debugfs.h>

#include <asm/uaccess.h>

#include "rt2x00.h"
#include "rt2x00debug.h"

#define PRINT_REG8_STR		( "0x%.2x\n" )
#define PRINT_REG16_STR		( "0x%.4x\n" )
#define PRINT_REG32_STR		( "0x%.8x\n" )
#define PRINT_REG_LEN_MAX	( 16 )
#define PRINT_LINE_LEN_MAX	( 32 )

struct rt2x00debug_intf {
	/*
	 * Pointer to driver structure where
	 * this debugfs entry belongs to.
	 */
	struct rt2x00_dev *rt2x00dev;

	/*
	 * Reference to the rt2x00debug structure
	 * which can be used to communicate with
	 * the registers.
	 */
	const struct rt2x00debug *debug;

	/*
	 * Debugfs entries for:
	 * - driver folder
	 * - driver file
	 * - chipset file
	 * - register offset/value files
	 * - eeprom offset/value files
	 * - bbp offset/value files
	 */
	struct dentry *driver_folder;
	struct dentry *driver_entry;
	struct dentry *chipset_entry;
	struct dentry *csr_off_entry;
	struct dentry *csr_val_entry;
	struct dentry *eeprom_off_entry;
	struct dentry *eeprom_val_entry;
	struct dentry *bbp_off_entry;
	struct dentry *bbp_val_entry;

	/*
	 * Driver and chipset files will use a data buffer
	 * that has been created in advance. This will simplify
	 * the code since we can use the debugfs functions.
	 */
	struct debugfs_blob_wrapper driver_blob;
	struct debugfs_blob_wrapper chipset_blob;

	/*
	 * Requested offset for each register type.
	 */
	unsigned int offset_csr;
	unsigned int offset_eeprom;
	unsigned int offset_bbp;
};

static int rt2x00debug_file_open(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = inode->i_private;

	file->private_data = inode->i_private;

	if (!try_module_get(intf->debug->owner))
		return -EBUSY;

	return 0;
}

static int rt2x00debug_file_release(struct inode *inode, struct file *file)
{
	struct rt2x00debug_intf *intf = file->private_data;

	module_put(intf->debug->owner);

	return 0;
}

static ssize_t rt2x00debug_file_read(void *device, char __user *buf,
	loff_t *offset, unsigned int word, const struct rt2x00debug_reg *reg)
{
	unsigned long value;
	unsigned int size;
	char *line;

	if (*offset)
		return 0;

	line = kzalloc(PRINT_REG_LEN_MAX, GFP_KERNEL);
	if (!line)
		return -ENOMEM;

	reg->read(device, word, &value);

	if (reg->word_size == sizeof(u8))
		size = sprintf(line, PRINT_REG8_STR, (u8)value);
	else if (reg->word_size == sizeof(u16))
		size = sprintf(line, PRINT_REG16_STR, (u16)value);
	else
		size = sprintf(line, PRINT_REG32_STR, (u32)value);

	if (copy_to_user(buf, line, size))
		goto exit;

	kfree(line);

	*offset += size;
	return size;

exit:
	kfree(line);

	return -EFAULT;
}

static ssize_t rt2x00debug_file_write(void *device, const char __user *buf,
	loff_t *offset, unsigned int word, unsigned int length,
	const struct rt2x00debug_reg *reg)
{
	unsigned long value;
	int size;
	char *line;

	line = kzalloc(length, GFP_KERNEL);
	if (!line)
		return -ENOMEM;

	if (copy_from_user(line, buf, length))
		goto exit;

	size = strlen(line);
	value = simple_strtoul(line, NULL, 0);

	reg->write(device, word, &value);

	kfree(line);

	*offset += size;
	return size;

exit:
	kfree(line);

	return -EFAULT;
}

#define RT2X00DEBUGFS_OPS_READ(__name)					\
	static ssize_t rt2x00debug_read_##__name(struct file *file,	\
		char __user *buf, size_t length, loff_t *offset)	\
	{								\
		struct rt2x00debug_intf *intf =	file->private_data;	\
		const struct rt2x00debug *debug = intf->debug;		\
		const struct rt2x00debug_reg *reg = &debug->reg_##__name;\
									\
		if (intf->offset_##__name > reg->word_count)		\
			return -EINVAL;					\
									\
		return rt2x00debug_file_read(intf->rt2x00dev, buf,	\
			offset, intf->offset_##__name, reg);		\
	}

RT2X00DEBUGFS_OPS_READ(csr);
RT2X00DEBUGFS_OPS_READ(eeprom);
RT2X00DEBUGFS_OPS_READ(bbp);

#define RT2X00DEBUGFS_OPS_WRITE(__name)					\
	static ssize_t rt2x00debug_write_##__name(struct file *file,	\
		const char __user *buf, size_t length, loff_t *offset)	\
	{								\
		struct rt2x00debug_intf *intf =	file->private_data;	\
		const struct rt2x00debug *debug = intf->debug;		\
		const struct rt2x00debug_reg *reg = &debug->reg_##__name;\
									\
		if (intf->offset_##__name > reg->word_count)		\
			return -EINVAL;					\
									\
		return rt2x00debug_file_write(intf->rt2x00dev, buf,	\
			offset, intf->offset_##__name, length, reg);	\
	}

RT2X00DEBUGFS_OPS_WRITE(csr);
RT2X00DEBUGFS_OPS_WRITE(eeprom);
RT2X00DEBUGFS_OPS_WRITE(bbp);

#define RT2X00DEBUGFS_OPS(__name)					\
	static const struct file_operations rt2x00debug_fop_##__name = {\
		.owner		= THIS_MODULE,				\
		.read		= rt2x00debug_read_##__name,		\
		.write		= rt2x00debug_write_##__name,		\
		.open		= rt2x00debug_file_open,		\
		.release	= rt2x00debug_file_release,		\
	};

RT2X00DEBUGFS_OPS(csr);
RT2X00DEBUGFS_OPS(eeprom);
RT2X00DEBUGFS_OPS(bbp);

static struct dentry *rt2x00debug_create_file_driver(const char *name,
	struct rt2x00debug_intf *intf, struct debugfs_blob_wrapper *blob)
{
	char *data;

	data = kzalloc(3 * PRINT_LINE_LEN_MAX, GFP_KERNEL);
	if (!data)
		return NULL;

	blob->data = data;
	data += sprintf(data, "driver: %s\n", intf->rt2x00dev->ops->name);
	data += sprintf(data, "version: %s\n", DRV_VERSION);
	data += sprintf(data, "compiled: %s %s\n", __DATE__, __TIME__);
	blob->size = strlen(blob->data);

	return debugfs_create_blob(name, S_IRUGO, intf->driver_folder, blob);
}

static struct dentry *rt2x00debug_create_file_chipset(const char *name,
	struct rt2x00debug_intf *intf, struct debugfs_blob_wrapper *blob)
{
	const struct rt2x00debug *debug = intf->debug;
	char *data;

	data = kzalloc(3 * PRINT_LINE_LEN_MAX, GFP_KERNEL);
	if (!data)
		return NULL;

	blob->data = data;
	data += sprintf(data, "csr length: %d\n", debug->reg_csr.word_count);
	data += sprintf(data, "eeprom length: %d\n",
		debug->reg_eeprom.word_count);
	data += sprintf(data, "bbp length: %d\n", debug->reg_bbp.word_count);
	blob->size = strlen(blob->data);

	return debugfs_create_blob(name, S_IRUGO, intf->driver_folder, blob);
}

void rt2x00debug_register(struct rt2x00_dev *rt2x00dev)
{
	const struct rt2x00debug *debug = rt2x00dev->ops->debugfs;
	struct rt2x00debug_intf *intf;

	intf = kzalloc(sizeof(struct rt2x00debug_intf), GFP_KERNEL);
	if (!intf) {
		ERROR(rt2x00dev, "Failed to allocate debug handler.\n");
		return;
	}

	intf->debug = debug;
	intf->rt2x00dev = rt2x00dev;
	rt2x00dev->debugfs_intf = intf;

	intf->driver_folder = debugfs_create_dir(intf->rt2x00dev->ops->name,
		rt2x00dev->hw->wiphy->debugfsdir);
	if (IS_ERR(intf->driver_folder))
		goto exit;

	intf->driver_entry = rt2x00debug_create_file_driver("driver",
		intf, &intf->driver_blob);
	if (IS_ERR(intf->driver_entry))
		goto exit;

	intf->chipset_entry = rt2x00debug_create_file_chipset("chipset",
		intf, &intf->chipset_blob);
	if (IS_ERR(intf->chipset_entry))
		goto exit;

	intf->csr_off_entry = debugfs_create_u32("csr_offset",
		S_IRUGO | S_IWUSR, intf->driver_folder, &intf->offset_csr);
	if (IS_ERR(intf->csr_off_entry))
		goto exit;

	intf->csr_val_entry = debugfs_create_file("csr_value",
		S_IRUGO | S_IWUSR, intf->driver_folder, intf,
		&rt2x00debug_fop_csr);
	if (IS_ERR(intf->csr_val_entry))
		goto exit;

	intf->eeprom_off_entry = debugfs_create_u32("eeprom_offset",
		S_IRUGO | S_IWUSR, intf->driver_folder, &intf->offset_eeprom);
	if (IS_ERR(intf->eeprom_off_entry))
		goto exit;

	intf->eeprom_val_entry = debugfs_create_file("eeprom_value",
		S_IRUGO | S_IWUSR, intf->driver_folder, intf,
		&rt2x00debug_fop_eeprom);
	if (IS_ERR(intf->eeprom_val_entry))
		goto exit;

	intf->bbp_off_entry = debugfs_create_u32("bbp_offset",
		S_IRUGO | S_IWUSR, intf->driver_folder, &intf->offset_bbp);
	if (IS_ERR(intf->bbp_off_entry))
		goto exit;

	intf->bbp_val_entry = debugfs_create_file("bbp_value",
		S_IRUGO | S_IWUSR, intf->driver_folder, intf,
		&rt2x00debug_fop_bbp);
	if (IS_ERR(intf->bbp_val_entry))
		goto exit;

	return;

exit:
	rt2x00debug_deregister(rt2x00dev);
	ERROR(rt2x00dev, "Failed to register debug handler.\n");

	return;
}

void rt2x00debug_deregister(struct rt2x00_dev *rt2x00dev)
{
	const struct rt2x00debug_intf *intf = rt2x00dev->debugfs_intf;

	if (unlikely(!intf))
		return;

	debugfs_remove(intf->bbp_val_entry);
	debugfs_remove(intf->bbp_off_entry);
	debugfs_remove(intf->eeprom_val_entry);
	debugfs_remove(intf->eeprom_off_entry);
	debugfs_remove(intf->csr_val_entry);
	debugfs_remove(intf->csr_off_entry);
	debugfs_remove(intf->chipset_entry);
	debugfs_remove(intf->driver_entry);
	debugfs_remove(intf->driver_folder);
	kfree(intf->chipset_blob.data);
	kfree(intf->driver_blob.data);
	kfree(intf);

	rt2x00dev->debugfs_intf = NULL;
}
