/*
 * Micron SPI-ER NAND Flash Memory
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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/err.h>

#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#define NAND_SPI_ER_BLOCK_FROM_ROW(row)		(row >> 6)

#define NAND_SPI_ER_STATUS_P_FAIL		(1 << 3)
#define NAND_SPI_ER_STATUS_E_FAIL		(1 << 2)
#define NAND_SPI_ER_STATUS_OIP			(1 << 0)

#define NAND_SPI_ER_LAST_ROW_INVALID		0xFFFFFFFF
#define	NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET	0x08

struct nand_spi_er_device {
	const char		*name;

	uint8_t			id0;
	uint8_t			id1;

	unsigned int		blocks;
	unsigned int		pages_per_block;
	unsigned int		page_size;
	unsigned int		write_size;
	unsigned int		erase_size;
};

struct nand_spi_er {
	char				name[24];

	const struct nand_spi_er_device	*device;

	struct mutex			lock;
	struct spi_device		*spi;

	struct mtd_info			mtd;

	unsigned int			last_row;	/* the last row we fetched */

	/*
	 * Bad block table (MUST be last in strcuture)
	 */
	unsigned long			nbb;
	unsigned long			bbt[0];
};

const struct nand_spi_er_device nand_spi_er_devices[] = {
	{
		name:			"MT29F1G01ZDC",
		id0:			0x2C,
		id1:			0x12,
		blocks:			1024,
		pages_per_block:	64,
		page_size:		2048,
		write_size:		512,
		erase_size:		64 * 2048,
	},
	{
		name:			"MT29F1G01ZDC",
		id0:			0x2C,
		id1:			0x13,
		blocks:			1024,
		pages_per_block:	64,
		page_size:		2048,
		write_size:		512,
		erase_size:		64 * 2048,
	},
};

static int read_only = 0;
module_param(read_only, int, 0);
MODULE_PARM_DESC(read_only, "Leave device locked");

/*
 * nand_spi_er_get_feature
 *	Get Feature register
 */
static int nand_spi_er_get_feature(struct nand_spi_er *chip, int reg, uint8_t *data)
{
	uint8_t txbuf[2];
	uint8_t rxbuf[1];
	int res;

	txbuf[0] = 0x0F;
	txbuf[1] = reg;
	res = spi_write_then_read(chip->spi, txbuf, 2, rxbuf, 1);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed get feature res=%d\n", chip->name, res);
		return res;
	}
	*data = rxbuf[0];
	return 0;
}

/*
 * nand_spi_er_busywait
 *	Wait until the chip is not busy
 */
static int nand_spi_er_busywait(struct nand_spi_er *chip, uint8_t *data)
{
	int i;

	for (i = 0; i < 100; i++) {
		int res = nand_spi_er_get_feature(chip, 0xC0, data);
		if (res) {
			return res;
		}
		if (!(*data & NAND_SPI_ER_STATUS_OIP)) {
			break;
		}
	}

	return 0;
}

/*
 * nand_spi_er_erase
 *	Erase a block, parameters must be block aligned
 */
static int nand_spi_er_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct nand_spi_er *chip = mtd->priv;
	struct spi_device *spi = chip->spi;
	uint8_t txbuf[4];
	int res;

	DEBUG(MTD_DEBUG_LEVEL3, "%s: erase addr:%x len:%x\n", chip->name, instr->addr, instr->len);

	if ((instr->addr + instr->len) > mtd->size) {
		return -EINVAL;
	}

	if (instr->addr & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: erase address is not aligned %x\n", chip->name, instr->addr);
		return -EINVAL;
	}

	if (instr->len & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: erase len is not aligned %x\n", chip->name, instr->len);
		return -EINVAL;
	}

	mutex_lock(&chip->lock);
	chip->last_row = NAND_SPI_ER_LAST_ROW_INVALID;

	while (instr->len) {
		uint32_t block = instr->addr >> 17;
		uint32_t row = block << 6;
		uint8_t stat;
		DEBUG(MTD_DEBUG_LEVEL3, "%s: block erase row:%x block:%x addr:%x rem:%x\n", chip->name, row, block, instr->addr, instr->len);

		/*
		 * Write enable
		 */
		txbuf[0] = 0x06;
		res = spi_write(spi, txbuf, 1);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write enable res=%d\n", chip->name, res);
			mutex_unlock(&chip->lock);
			return res;
		}

		/*
		 * Test for bad block
		 */
		if (test_bit(block, chip->bbt)) {
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			res = -EBADMSG;
			goto done;
		}

		/*
		 * Block erase
		 */
		txbuf[0] = 0xD8;
		txbuf[1] = 0x00;
		txbuf[2] = row >> 8;
		txbuf[3] = row & 0xFF;
		res = spi_write(spi, txbuf, 4);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed block erase res=%d\n", chip->name, res);
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			goto done;
		}

		/*
		 * Wait
		 */
		res = nand_spi_er_busywait(chip, &stat);
		if (res || (stat & NAND_SPI_ER_STATUS_OIP)) {
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive res=%d stat=%02x\n", chip->name, res, stat);
			if (res) {
				goto done;
			}

			/*
			 * Chip is stuck?
			 */
			res = -EIO;
			goto done;
		}

		/*
		 * Check the status register
		 */
		if (stat & NAND_SPI_ER_STATUS_E_FAIL) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: E_FAIL signalled (%02x)\n", chip->name, stat);
			instr->fail_addr = block << 17;
			instr->state = MTD_ERASE_FAILED;
			goto done;
		}

		/*
		 * Next
		 */
		block++;
		instr->len -= chip->device->erase_size;
		instr->addr += chip->device->erase_size;
	}

	instr->state = MTD_ERASE_DONE;

	mutex_unlock(&chip->lock);
	return 0;

done:
	/*
	 * Write disable
	 */
	txbuf[0] = 0x04;
	res = spi_write(spi, txbuf, 1);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write disable res=%d\n", chip->name, res);
	}

	mutex_unlock(&chip->lock);

	mtd_erase_callback(instr);
	return 0;
}

/*
 * nand_spi_er_read
 *
 * return -EUCLEAN: ecc error recovered
 * return -EBADMSG: ecc error not recovered
*/
static int nand_spi_er_read(struct mtd_info *mtd, loff_t from, size_t len,
			       size_t *retlen, u_char *buf)
{
	struct nand_spi_er *chip = mtd->priv;
	struct spi_device *spi = chip->spi;

	uint32_t row;
	uint32_t column;
	int retval = 0;

	*retlen = 0;
	DEBUG(MTD_DEBUG_LEVEL2, "%s: read block from %llx len %d into %p\n", chip->name, from, len, buf);

	/*
	 * Zero length reads, nothing to do
	 */
	if (len == 0) {
		return 0;
	}

	/*
	 * Reject reads which go over the end of the flash
	 */
	if ((from + len) > mtd->size) {
		return -EINVAL;
	}

	/*
	 * Get the row and column address to start at
	 */
	row = from >> 11;
	column = from & 0x7FF;
	DEBUG(MTD_DEBUG_LEVEL3, "%s: row=%x %d column=%x %d last_row=%x %d\n", chip->name, row, row, column, column, chip->last_row, chip->last_row);

	/*
	 * Read the data from the chip
	 */
	mutex_lock(&chip->lock);
	while (len) {
		uint8_t stat;
		uint8_t txbuf[4];
		struct spi_message message;
		struct spi_transfer x[2];
		int res;
		size_t toread;

		/*
		 * Figure out how much to read
		 *
		 * If we are reading from the middle of a page then the most we
		 * can read is to the end of the page
		 */
		toread = len;
		if (toread > (chip->device->page_size - column)) {
			toread = chip->device->page_size - column;
		}

		DEBUG(MTD_DEBUG_LEVEL3, "%s: buf=%p toread=%x row=%x column=%x last_row=%x\n", chip->name, buf, toread, row, column, chip->last_row);

		if (chip->last_row != row) {
			/*
			 * Check if the block is bad
			 */
			if (test_bit(NAND_SPI_ER_BLOCK_FROM_ROW(row), chip->bbt)) {
				mutex_unlock(&chip->lock);
				return -EBADMSG;
			}

			/*
			 * Load the appropriate page
			 */
			txbuf[0] = 0x13;
			txbuf[1] = 0x00;
			txbuf[2] = row >> 8;
			txbuf[3] = row & 0xFF;
			res = spi_write(spi, txbuf, 4);
			if (res) {
				DEBUG(MTD_DEBUG_LEVEL1, "%s: failed page load res=%d\n", chip->name, res);
				mutex_unlock(&chip->lock);
				return res;
			}

			/*
			 * Wait
			 */
			res = nand_spi_er_busywait(chip, &stat);
			if (res || (stat & NAND_SPI_ER_STATUS_OIP)) {
				DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive res=%d stat=%02x\n", chip->name, res, stat);
				if (res) {
					mutex_unlock(&chip->lock);
					return res;
				}

				/*
				 * Chip is stuck?
				 */
				mutex_unlock(&chip->lock);
				return -EIO;
			}

			/*
			 * Check the ECC bits
			 */
			stat >>= 4;
			if (stat == 1) {
				DEBUG(MTD_DEBUG_LEVEL1, "%s: ECC recovered, row=%x\n", chip->name, row);
				retval = -EUCLEAN;
			}
			if (stat == 2) {
				DEBUG(MTD_DEBUG_LEVEL0, "%s: failed ECC, row=%x\n", chip->name, row);
				chip->last_row = NAND_SPI_ER_LAST_ROW_INVALID;
				mutex_unlock(&chip->lock);
				return -EBADMSG;
			}

		}

		chip->last_row = row;

		/*
		 * Read out the data
		 */
		spi_message_init(&message);
		memset(x, 0, sizeof(x));

		txbuf[0] = 0x03;
		txbuf[1] = column >> 8;
		txbuf[2] = column & 0xFF;
		txbuf[3] = 0;
		x[0].tx_buf = txbuf;
		x[0].len = 4;
		spi_message_add_tail(&x[0], &message);

		x[1].rx_buf = buf;
		x[1].len = toread;
		spi_message_add_tail(&x[1], &message);

		res = spi_sync(spi, &message);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed data read res=%d\n", chip->name, res);
			mutex_unlock(&chip->lock);
			return res;
		}
		buf += toread;
		len -= toread;
		*retlen += toread;

		/*
		 * For the next page, increment the row and always start at column 0
		 */
		column = 0;
		row++;
	}

	mutex_unlock(&chip->lock);
	return retval;
}

/*
 * nand_spi_er_write
 */
#define NOT_ALIGNED(x) ((x & (device->write_size - 1)) != 0)
static int nand_spi_er_write(struct mtd_info *mtd, loff_t to, size_t len,
				size_t *retlen, const u_char *buf)
{
	struct nand_spi_er *chip = mtd->priv;
	struct spi_device *spi = chip->spi;
	const struct nand_spi_er_device *device = chip->device;
	uint32_t row;
	uint32_t col;
	uint8_t txbuf[4];
	int res;
	size_t towrite;

	DEBUG(MTD_DEBUG_LEVEL2, "%s: write block to %llx len %d from %p\n", chip->name, to, len, buf);

	*retlen = 0;

	/*
	 * nothing to write
	 */
	if (!len) {
		return 0;
	}

	/*
	 * Reject writes which go over the end of the flash
	 */
	if ((to + len) > mtd->size) {
		return -EINVAL;
	}

	/*
	 * Check to see if everything is page aligned
	 */
	if (NOT_ALIGNED(to) || NOT_ALIGNED(len)) {
		printk(KERN_NOTICE "nand_spi_er_write: Attempt to write non page aligned data\n");
		return -EINVAL;
	}

	mutex_lock(&chip->lock);
	chip->last_row = NAND_SPI_ER_LAST_ROW_INVALID;

	/*
	 * If the first write is a partial write then write at most the number of
	 * bytes to get us page aligned and then the remainder will be
	 * page aligned.  The last bit may be a partial page as well.
	 */
	col = to & (device->page_size - 1);
	towrite = device->page_size - col;
	if (towrite > len) {
		towrite = len;
	}

	/*
	 * Write the data
	 */
	row = to >> 11;
	while (len) {
		struct spi_message message;
		struct spi_transfer x[2];
		uint8_t stat;

		DEBUG(MTD_DEBUG_LEVEL3, "%s: write %p to row:%x col:%x len:%x rem:%x\n", chip->name, buf, row, col, towrite, len);

		/*
		 * Write enable
		 */
		txbuf[0] = 0x06;
		res = spi_write(spi, txbuf, 1);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write enable res=%d\n", chip->name, res);
			mutex_unlock(&chip->lock);
			return res;
		}

		/*
		 * Write the data into the cache
		 */
		spi_message_init(&message);
		memset(x, 0, sizeof(x));
		txbuf[0] = 0x02;
		txbuf[1] = col >> 8;
		txbuf[2] = col & 0xFF;
		x[0].tx_buf = txbuf;
		x[0].len = 3;
		spi_message_add_tail(&x[0], &message);
		x[1].tx_buf = buf;
		x[1].len = towrite;
		spi_message_add_tail(&x[1], &message);
		res = spi_sync(spi, &message);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed cache write res=%d\n", chip->name, res);
			goto done;
		}

		/*
		 * Program execute
		 */
		txbuf[0] = 0x10;
		txbuf[1] = 0x00;
		txbuf[2] = row >> 8;
		txbuf[3] = row & 0xFF;
		res = spi_write(spi, txbuf, 4);
		if (res) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: failed prog execute res=%d\n", chip->name, res);
			goto done;
		}

		/*
		 * Wait
		 */
		res = nand_spi_er_busywait(chip, &stat);
		if (res || (stat & NAND_SPI_ER_STATUS_OIP)) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive res=%d stat=%02x\n", chip->name, res, stat);
			if (res) {
				goto done;
			}

			/*
			 * Chip is stuck?
			 */
			res = -EIO;
			goto done;
		}

		if (stat & (1 << 3)) {
			res = -EBADMSG;
			goto done;
		}

		row++;
		buf += towrite;
		len -= towrite;
		*retlen += towrite;

		/*
		 * At this point, we are always page aligned so start at column 0.
		 * Note we may not have a full page to write at the end, hence the
		 * check if towrite > len.
		 */
		col = 0;
		towrite = device->page_size;
		if (towrite > len) {
			towrite = len;
		}
	}

	mutex_unlock(&chip->lock);
	return res;

done:
	/*
	 * Write disable
	 */
	txbuf[0] = 0x04;
	res = spi_write(spi, txbuf, 1);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write disable res=%d\n", chip->name, res);
	}

	mutex_unlock(&chip->lock);

	return res;
}

/*
 * nand_spi_er_isbad
 */
static int nand_spi_er_isbad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_spi_er *chip = mtd->priv;
	uint32_t block;

	if (ofs & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: address not aligned %llx\n", chip->name, ofs);
		return -EINVAL;
	}

	block = ofs >> 17;

	return test_bit(block, chip->bbt);
}

/*
 * nand_spi_er_markbad
 */
static int nand_spi_er_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_spi_er *chip = mtd->priv;
	struct spi_device *spi = chip->spi;
	uint32_t block;
	uint32_t row;
	uint8_t txbuf[7];
	int res;
	uint8_t stat;

	if (ofs & (chip->device->erase_size - 1)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: address not aligned %llx\n", chip->name, ofs);
		return -EINVAL;
	}

	block = ofs >> 17;

	/*
	 * If it's already marked bad, no need to mark it
	 */
	if (test_bit(block, chip->bbt)) {
		return 0;
	}

	/*
	 * Mark it in our cache
	 */
	__set_bit(block, chip->bbt);

	/*
	 * Write the user bad block mark.  If it fails, then we really
	 * can't do anything about it.
	 */
	mutex_lock(&chip->lock);
	chip->last_row = NAND_SPI_ER_LAST_ROW_INVALID;

	/*
	 * Write enable
	 */
	txbuf[0] = 0x06;
	res = spi_write(spi, txbuf, 1);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write enable res=%d\n", chip->name, res);
		mutex_unlock(&chip->lock);
		return res;
	}

	/*
	 * Write the mark
	 */
	txbuf[0] = 0x84;
	txbuf[1] = 0x08;
	txbuf[2] = NAND_SPI_ER_BAD_BLOCK_MARK_OFFSET;
	txbuf[3] = 0xde;
	txbuf[4] = 0xad;
	txbuf[5] = 0xbe;
	txbuf[6] = 0xef;
	res = spi_write(spi, txbuf, 7);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write mark res=%d\n", chip->name, res);
		goto done;
	}

	/*
	 * Program execute
	 */
	row = ofs >> 11;
	txbuf[0] = 0x10;
	txbuf[1] = 0x00;
	txbuf[2] = row >> 8;
	txbuf[3] = row & 0xFF;
	res = spi_write(spi, txbuf, 4);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed program execute res=%d\n", chip->name, res);
		goto done;
	}

	/*
	 * Wait
	 */
	res = nand_spi_er_busywait(chip, &stat);
	if (res || (stat & NAND_SPI_ER_STATUS_OIP)) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive res=%d stat=%02x\n", chip->name, res, stat);
		if (res) {
			goto done;
		}

		/*
		 * Chip is stuck?
		 */
		res = -EIO;
		goto done;
	}

	if (stat & (1 << 3)) {
		res = -EBADMSG;
	}

done:
	/*
	 * Write disable
	 */
	txbuf[0] = 0x04;
	res = spi_write(spi, txbuf, 1);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed write disable res=%d\n", chip->name, res);
	}

	mutex_unlock(&chip->lock);

	return res;
}

/*
 * nand_spi_er_read_bbt
 */
static int nand_spi_er_read_bbt(struct nand_spi_er *chip)
{
	int j;
	for (j = 0; j < chip->device->blocks; j++) {
		uint8_t txbuf[4];
		uint8_t rxbuf[16];
		uint32_t bbmark;
		int res;
		unsigned short row = j << 6;
		uint8_t stat;

		/*
		 * Read Page
		 */
		txbuf[0] = 0x13;
		txbuf[1] = 0x00;
		txbuf[2] = row >> 8;
		txbuf[3] = row & 0xFF;
		res = spi_write(chip->spi, txbuf, 4);
		if (res) {
			return res;
		}

		/*
		 * Wait
		 */
		res = nand_spi_er_busywait(chip, &stat);
		if (res || (stat & NAND_SPI_ER_STATUS_OIP)) {
			DEBUG(MTD_DEBUG_LEVEL1, "%s: chip is busy or nonresponsive res=%d stat=%02x\n", chip->name, res, stat);
			if (res) {
				return res;
			}

			/*
			 * Chip is stuck?
			 */
			return -EIO;
		}

		/*
		 * Check factory bad block mark
		 */
		txbuf[0] = 0x03;
		txbuf[1] = 0x08;
		txbuf[2] = 0x00;
		txbuf[3] = 0x00;
		res = spi_write_then_read(chip->spi, txbuf, 4, rxbuf, 16);
		if (res) {
			return res;
		}
		if (rxbuf[0] != 0xFF) {
			chip->nbb++;
			__set_bit(j, chip->bbt);
			continue;
		}

		memcpy(&bbmark, &rxbuf[8], 4);
		if (bbmark == 0xdeadbeef) {
			chip->nbb++;
			__set_bit(j, chip->bbt);
		}
	}

#if defined(CONFIG_MTD_DEBUG) && (MTD_DEBUG_LEVEL3 <= CONFIG_MTD_DEBUG_VERBOSE)
	printk("%s: Bad Block Table:", chip->name);
	for (j = 0; j < chip->device->blocks; j++) {
		if ((j % 64) == 0) {
			printk("\n%s: block %03x: ", chip->name, j);
		}
		printk("%c", test_bit(j, chip->bbt) ? 'X' : '.');
	}
	printk("\n%s: Bad Block Numbers: ", chip->name);
	for (j = 0; j < chip->device->blocks; j++) {
		if (test_bit(j, chip->bbt)) {
			printk("%x ", j);
		}
	}
	printk("\n");
#endif

	return 0;
}

#ifndef MODULE
/*
 * Called at boot time:
 *
 * nand_spi_er=read_only
 *	if read_only specified then do not unlock device
 */
static int __init nand_spi_er_setup(char *str)
{
	if (str && (strncasecmp(str, "read_only", 9) == 0)) {
		read_only = 1;
	}
	return 0;
}

__setup("nand_spi_er=", nand_spi_er_setup);
#endif

/*
 * nand_spi_er_probe
 *	Detect and initialize nand_spi_er device.
 */
static int __devinit nand_spi_er_probe(struct spi_device *spi)
{
	uint8_t txbuf[3];
	uint8_t rxbuf[2];
	int i;
	int res;
	size_t bbt_bytes;
	struct nand_spi_er *chip;
	const struct nand_spi_er_device *device;

	res = spi_setup(spi);
	if (res) {
		return res;
	}

	/*
	 * Reset
	 */
	for (i = 0; i < 2; i++) {
		txbuf[0] = 0xFF;
		res = spi_write(spi, txbuf, 1);
		if (res) {
			return res;
		}
		udelay(250);
	}
	udelay(1000);

	/*
	 * Read ID
	 */
	txbuf[0] = 0x9F;
	txbuf[1] = 0x00;
	res = spi_write_then_read(spi, txbuf, 2, rxbuf, 2);
	if (res) {
		return res;
	}

	device = nand_spi_er_devices;
	for (i = 0; i < ARRAY_SIZE(nand_spi_er_devices); i++) {
		if ((device->id0 == rxbuf[0]) && (device->id1 == rxbuf[1])) {
			break;
		}
		device++;
	}
	if (i == ARRAY_SIZE(nand_spi_er_devices)) {
		return -ENODEV;
	}

	/*
	 * Initialize our chip structure
	 */
	bbt_bytes = DIV_ROUND_UP(device->blocks, BITS_PER_BYTE);
	chip = kzalloc(sizeof(struct nand_spi_er) + bbt_bytes, GFP_KERNEL);
	if (!chip) {
		return -ENOMEM;
	}
	snprintf(chip->name, sizeof(chip->name), "%s.%d.%d", device->name, spi->master->bus_num, spi->chip_select);

	chip->spi = spi;
	chip->device = device;
	chip->last_row = NAND_SPI_ER_LAST_ROW_INVALID;

	mutex_init(&chip->lock);

	chip->mtd.type = MTD_NANDFLASH;
	chip->mtd.flags = MTD_WRITEABLE;

	/*
	 * #blocks * block size * n blocks
	 */
	chip->mtd.size = device->blocks * device->pages_per_block * device->page_size;
	chip->mtd.erasesize = device->erase_size;

	/*
	 * 1 page, optionally we can support partial write (512)
	 */
	chip->mtd.writesize = device->write_size;
	chip->mtd.name = device->name;
	chip->mtd.erase = nand_spi_er_erase;
	chip->mtd.read = nand_spi_er_read;
	chip->mtd.write = nand_spi_er_write;
	chip->mtd.block_isbad = nand_spi_er_isbad;
	chip->mtd.block_markbad = nand_spi_er_markbad;
	chip->mtd.priv = chip;

	/*
	 * Cache the bad block table
	 */
	res = nand_spi_er_read_bbt(chip);
	if (res) {
		kfree(chip);
		return res;
	}

	/*
	 * Un/lock the chip
	 */
	txbuf[0] = 0x1F;
	txbuf[1] = 0xA0;
	if (read_only) {
		txbuf[2] = 0x38;
	} else {
		txbuf[2] = 0x00;
	}
	res = spi_write(spi, txbuf, 3);
	if (res) {
		DEBUG(MTD_DEBUG_LEVEL1, "%s: failed lock operation res=%d\n", chip->name, res);
		mutex_unlock(&chip->lock);
		return res;
	}

	spi_set_drvdata(spi, chip);

	printk(KERN_INFO "%s: added device %s size: %u KBytes %u bad blocks %s\n", spi->dev.bus_id, chip->mtd.name, DIV_ROUND_UP(chip->mtd.size, 1024), chip->nbb, read_only ? "[read only]" : "");
	return add_mtd_device(&chip->mtd);
}

/*
 * nand_spi_er_remove
 */
static int __devexit nand_spi_er_remove(struct spi_device *spi)
{
	struct nand_spi_er *chip = spi_get_drvdata(spi);
	int status = 0;

	DEBUG(MTD_DEBUG_LEVEL1, "%s: remove\n", spi->dev.bus_id);
	status = del_mtd_device(&chip->mtd);
	if (status == 0)
		kfree(chip);
	return status;
}

static struct spi_driver nand_spi_er_driver = {
	.driver = {
		.name		= "nand-spi-er",
		.bus		= &spi_bus_type,
		.owner		= THIS_MODULE,
	},

	.probe		= nand_spi_er_probe,
	.remove		= __devexit_p(nand_spi_er_remove),

	/* FIXME:  investigate suspend and resume... */
};

/*
 * nand_spi_er_init
 */
static int __init nand_spi_er_init(void)
{
	return spi_register_driver(&nand_spi_er_driver);
}
module_init(nand_spi_er_init);

/*
 * nand_spi_er_exit
 */
static void __exit nand_spi_er_exit(void)
{
	spi_unregister_driver(&nand_spi_er_driver);
}
module_exit(nand_spi_er_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("MTD nand_spi_er driver");
