#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/mtd/spi-nor.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/cfi.h>
#include <linux/mtd/partitions.h>

static const char * const probes[] = { "ofpart", "bcm47xxpart", NULL };

struct bcm53xxsf {
	struct spi_device *spi;
	struct mtd_info mtd;
	struct spi_nor nor;
};

/**************************************************
 * spi-nor API
 **************************************************/

static int bcm53xxspiflash_read_reg(struct spi_nor *nor, u8 opcode, u8 *buf,
				   int len)
{
	struct bcm53xxsf *b53sf = nor->priv;

	return spi_write_then_read(b53sf->spi, &opcode, 1, buf, len);
}

static int bcm53xxspiflash_write_reg(struct spi_nor *nor, u8 opcode, u8 *buf,
				     int len, int write_enable)
{
	struct bcm53xxsf *b53sf = nor->priv;
	u8 *cmd = kzalloc(len + 1, GFP_KERNEL);
	int err;

	if (!cmd)
		return -ENOMEM;

	cmd[0] = opcode;
	memcpy(&cmd[1], buf, len);
	err = spi_write(b53sf->spi, cmd, len + 1);

	kfree(cmd);

	return err;
}

static int bcm53xxspiflash_read(struct spi_nor *nor, loff_t from, size_t len,
				size_t *retlen, u_char *buf)
{
	struct bcm53xxsf *b53sf = nor->priv;
	struct spi_message m;
	struct spi_transfer t[2] = { { 0 }, { 0 } };
	unsigned char cmd[5];
	int cmd_len = 0;
	int err;

	spi_message_init(&m);

	cmd[cmd_len++] = SPINOR_OP_READ;
	if (b53sf->mtd.size > 0x1000000)
		cmd[cmd_len++] = (from & 0xFF000000) >> 24;
	cmd[cmd_len++] = (from & 0x00FF0000) >> 16;
	cmd[cmd_len++] = (from & 0x0000FF00) >> 8;
	cmd[cmd_len++] = (from & 0x000000FF) >> 0;

	t[0].tx_buf = cmd;
	t[0].len = cmd_len;
	spi_message_add_tail(&t[0], &m);

	t[1].rx_buf = buf;
	t[1].len = len;
	spi_message_add_tail(&t[1], &m);

	err = spi_sync(b53sf->spi, &m);
	if (err)
		return err;

	if (retlen && m.actual_length > cmd_len)
		*retlen = m.actual_length - cmd_len;

	return 0;
}

static void bcm53xxspiflash_write(struct spi_nor *nor, loff_t to, size_t len,
				  size_t *retlen, const u_char *buf)
{
	struct bcm53xxsf *b53sf = nor->priv;
	struct spi_message m;
	struct spi_transfer t = { 0 };
	u8 *cmd = kzalloc(len + 5, GFP_KERNEL);
	int cmd_len = 0;
	int err;

	if (!cmd)
		return;

	spi_message_init(&m);

	cmd[cmd_len++] = nor->program_opcode;
	if (b53sf->mtd.size > 0x1000000)
		cmd[cmd_len++] = (to & 0xFF000000) >> 24;
	cmd[cmd_len++] = (to & 0x00FF0000) >> 16;
	cmd[cmd_len++] = (to & 0x0000FF00) >> 8;
	cmd[cmd_len++] = (to & 0x000000FF) >> 0;
	memcpy(&cmd[cmd_len], buf, len);

	t.tx_buf = cmd;
	t.len = cmd_len + len;
	spi_message_add_tail(&t, &m);

	err = spi_sync(b53sf->spi, &m);
	if (err)
		goto out;

	if (retlen && m.actual_length > cmd_len)
		*retlen += m.actual_length - cmd_len;

out:
	kfree(cmd);
}

static int bcm53xxspiflash_erase(struct spi_nor *nor, loff_t offs)
{
	struct bcm53xxsf *b53sf = nor->priv;
	unsigned char cmd[5];
	int i;

	i = 0;
	cmd[i++] = nor->erase_opcode;
	if (b53sf->mtd.size > 0x1000000)
		cmd[i++] = (offs & 0xFF000000) >> 24;
	cmd[i++] = ((offs & 0x00FF0000) >> 16);
	cmd[i++] = ((offs & 0x0000FF00) >> 8);
	cmd[i++] = ((offs & 0x000000FF) >> 0);

	return spi_write(b53sf->spi, cmd, i);
}

static const char *bcm53xxspiflash_chip_name(struct spi_nor *nor)
{
	struct bcm53xxsf *b53sf = nor->priv;
	struct device *dev = &b53sf->spi->dev;
	unsigned char cmd[4];
	unsigned char resp[2];
	int err;

	/* SST and Winbond/NexFlash specific command */
	cmd[0] = 0x90; /* Read Manufacturer / Device ID */
	cmd[1] = 0;
	cmd[2] = 0;
	cmd[3] = 0;
	err = spi_write_then_read(b53sf->spi, cmd, 4, resp, 2);
	if (err < 0) {
		dev_err(dev, "error reading SPI flash id\n");
		return ERR_PTR(-EBUSY);
	}
	switch (resp[0]) {
	case 0xef: /* Winbond/NexFlash */
		switch (resp[1]) {
		case 0x17:
			return "w25q128";
		}
		dev_err(dev, "Unknown Winbond/NexFlash flash: %02X %02X\n",
			resp[0], resp[1]);
		return NULL;
	}

	/* TODO: Try more ID commands */

	return NULL;
}

/**************************************************
 * SPI driver
 **************************************************/

static int bcm53xxspiflash_probe(struct spi_device *spi)
{
	struct mtd_part_parser_data parser_data = {};
	struct bcm53xxsf *b53sf;
	struct spi_nor *nor;
	int err;

	b53sf = devm_kzalloc(&spi->dev, sizeof(*b53sf), GFP_KERNEL);
	if (!b53sf)
		return -ENOMEM;
	spi_set_drvdata(spi, b53sf);

	nor = &b53sf->nor;
	b53sf->spi = spi;
	b53sf->mtd.priv = &b53sf->nor;

	nor->mtd = &b53sf->mtd;
	nor->dev = &spi->dev;
	nor->read_reg = bcm53xxspiflash_read_reg;
	nor->write_reg = bcm53xxspiflash_write_reg;
	nor->read = bcm53xxspiflash_read;
	nor->write = bcm53xxspiflash_write;
	nor->erase = bcm53xxspiflash_erase;
	nor->priv = b53sf;

	err = spi_nor_scan(&b53sf->nor, bcm53xxspiflash_chip_name(nor),
			   SPI_NOR_NORMAL);
	if (err)
		return err;

	parser_data.of_node = spi->master->dev.parent->of_node;
	err = mtd_device_parse_register(&b53sf->mtd, probes, &parser_data,
					NULL, 0);
	if (err)
		return err;

	return 0;
}

static int bcm53xxspiflash_remove(struct spi_device *spi)
{
	return 0;
}

static struct spi_driver bcm53xxspiflash_driver = {
	.driver = {
		.name	= "bcm53xxspiflash",
		.owner	= THIS_MODULE,
	},
	.probe		= bcm53xxspiflash_probe,
	.remove		= bcm53xxspiflash_remove,
};

module_spi_driver(bcm53xxspiflash_driver);
