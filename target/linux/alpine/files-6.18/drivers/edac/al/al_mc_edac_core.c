/*
 * Copyright 2013 Annapurna Labs Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/edac.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/uaccess.h>

#include <mach/al_fabric.h>
#include "al_hal_ddr.h"

#include "../edac_core.h"
#include "../edac_module.h"


#define DEVICE_ATTR_FLAGS	S_IRUGO | S_IWUSR

struct inject_addr {
	int col;
	int rank;
	int row;
	int bank;
};

struct al_mc_drvdata {
	struct mem_ctl_info	*mci;
	void __iomem		*vbase;
	struct inject_addr	inject;
	int			inject_enabled;
	struct delayed_work	handle_corr_err_work;
	struct delayed_work	handle_uncorr_err_work;
};

/* Memory Controller error handler */
static irqreturn_t al_mc_corr_err_handler(int irq, void *dev_id)
{
	struct mem_ctl_info *mci = dev_id;
	struct al_mc_drvdata *drvdata = mci->pvt_info;

	schedule_delayed_work(&drvdata->handle_corr_err_work,
			msecs_to_jiffies(1));

	return IRQ_HANDLED;
}

static irqreturn_t al_mc_uncorr_err_handler(int irq, void *dev_id)
{
	struct mem_ctl_info *mci = dev_id;
	struct al_mc_drvdata *drvdata = mci->pvt_info;

	schedule_delayed_work(&drvdata->handle_uncorr_err_work,
			msecs_to_jiffies(1));

	return IRQ_HANDLED;
}

static void al_mc_corr_err_work(struct work_struct *work)
{
	struct al_mc_drvdata *drvdata = container_of(work, struct al_mc_drvdata,
			handle_corr_err_work.work);
	struct mem_ctl_info *mci = drvdata->mci;
	struct al_ddr_ecc_status ecc_corr_status;
	al_phys_addr_t err_addr;

	/* get the ecc status */
	al_ddr_ecc_status_get(drvdata->vbase,
			&ecc_corr_status, NULL);

	al_ddr_ecc_corr_count_clear(drvdata->vbase);

	al_ddr_address_translate_dram2sys(drvdata->vbase, &err_addr,
			ecc_corr_status.rank, ecc_corr_status.bank,
			ecc_corr_status.col, ecc_corr_status.row);

	/* log the error */
	edac_mc_handle_error(HW_EVENT_ERR_CORRECTED, mci,
			ecc_corr_status.err_cnt,
			err_addr >> PAGE_SHIFT, err_addr & PAGE_MASK,
			ecc_corr_status.syndromes_31_0,
			ecc_corr_status.rank, -1, -1, mci->ctl_name, "");

	al_ddr_ecc_corr_int_clear(NULL, drvdata->vbase);
}

static void al_mc_uncorr_err_work(struct work_struct *work)
{
	struct al_mc_drvdata *drvdata = container_of(work, struct al_mc_drvdata,
			handle_uncorr_err_work.work);
	struct mem_ctl_info *mci = drvdata->mci;
	struct al_ddr_ecc_status ecc_uncorr_status;
	al_phys_addr_t err_addr;

	/* get the ecc status */
	al_ddr_ecc_status_get(drvdata->vbase,
			NULL, &ecc_uncorr_status);

	al_ddr_ecc_uncorr_count_clear(drvdata->vbase);

	al_ddr_address_translate_dram2sys(drvdata->vbase, &err_addr,
			ecc_uncorr_status.rank, ecc_uncorr_status.bank,
			ecc_uncorr_status.col, ecc_uncorr_status.row);

	/* log the error */
	edac_mc_handle_error(HW_EVENT_ERR_UNCORRECTED, mci,
			ecc_uncorr_status.err_cnt,
			err_addr >> PAGE_SHIFT, err_addr & PAGE_MASK, 0,
			ecc_uncorr_status.rank, -1, -1, mci->ctl_name, "");

	al_ddr_ecc_uncorr_int_clear(NULL, drvdata->vbase);
}


/*
 * The following functions implement the sysfs behavior
 */

/* Data injection physical address configuration */
static ssize_t al_inject_phys_addr_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	struct mem_ctl_info	*mci = dev_get_drvdata(dev);
	struct al_mc_drvdata	*drvdata = mci->pvt_info;
	unsigned long data_val;
	al_phys_addr_t addr;
	int rc;

	rc = kstrtoul(data, 16, &data_val);
	if (rc < 0)
		return -EIO;

	addr = (al_phys_addr_t)data_val;
	rc = al_ddr_address_translate_sys2dram(drvdata->vbase, addr,
			&drvdata->inject.rank, &drvdata->inject.bank,
			&drvdata->inject.col, &drvdata->inject.row);
	if (rc < 0)
		return -EIO;

	if (drvdata->inject_enabled == 1) {
		rc = al_ddr_ecc_data_poison_enable(
				drvdata->vbase,
				drvdata->inject.rank,
				drvdata->inject.bank,
				drvdata->inject.col,
				drvdata->inject.row);
		if (rc < 0)
			return -EIO;
	}

	return count;
}

static ssize_t al_inject_phys_addr_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	struct mem_ctl_info	*mci = dev_get_drvdata(dev);
	struct al_mc_drvdata	*drvdata = mci->pvt_info;
	al_phys_addr_t addr;

	al_ddr_address_translate_dram2sys(drvdata->vbase, &addr,
			drvdata->inject.rank, drvdata->inject.bank,
			drvdata->inject.col, drvdata->inject.row);

	return sprintf(data, "0x%llx\n", (unsigned long long)addr);
}


/* Data injection en/disable interface */
static ssize_t al_inject_enable_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	struct mem_ctl_info	*mci = dev_get_drvdata(dev);
	struct al_mc_drvdata	*drvdata = mci->pvt_info;
	unsigned long data_val;
	int rc;

	rc = kstrtoul(data, 10, &data_val);
	if (rc < 0)
		return -EIO;

	if (data_val == 1)
		rc = al_ddr_ecc_data_poison_enable(
				drvdata->vbase,
				drvdata->inject.rank,
				drvdata->inject.bank,
				drvdata->inject.col,
				drvdata->inject.row);
	else if (data_val == 0)
		rc = al_ddr_ecc_data_poison_disable(drvdata->vbase);
	else
		return -EIO;

	drvdata->inject_enabled = data_val;
	if (rc < 0)
		return rc;

	return count;
}

static ssize_t al_inject_enable_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	struct mem_ctl_info	*mci = dev_get_drvdata(dev);
	struct al_mc_drvdata	*drvdata = mci->pvt_info;

	return sprintf(data, "%d\n", drvdata->inject_enabled);
}

/* Data injection mechanism DDR-addressing configuration
 * the store/show functions share the same implementation, defined below
 */
#define al_inject_store_impl(_field)					\
	do {								\
		struct mem_ctl_info	*mci = dev_get_drvdata(dev);	\
		struct al_mc_drvdata	*drvdata = mci->pvt_info;	\
		unsigned long data_val;					\
		int rc;							\
									\
		rc = kstrtoul(data, 10, &data_val);		\
		if (rc < 0)						\
			return -EIO;					\
		drvdata->inject._field = data_val;			\
		/* user must issue enable cmd after changing addr */	\
		drvdata->inject_enabled = 0;				\
		rc = al_ddr_ecc_data_poison_disable(drvdata->vbase);	\
		if (rc < 0)						\
			return -EIO;					\
	} while(0)

#define al_inject_show_impl(_field)					\
	do {								\
		struct mem_ctl_info	*mci = dev_get_drvdata(dev);	\
		struct al_mc_drvdata	*drvdata = mci->pvt_info;	\
									\
		return sprintf(data, "%d\n", drvdata->inject._field);	\
	} while (0)

static ssize_t al_inject_col_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	al_inject_store_impl(col);

	return count;
}

static ssize_t al_inject_rank_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	al_inject_store_impl(rank);

	return count;
}

static ssize_t al_inject_row_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	al_inject_store_impl(row);

	return count;
}

static ssize_t al_inject_bank_store(
	struct device *dev,
	struct device_attribute *mattr,
	const char *data, size_t count)
{
	al_inject_store_impl(bank);

	return count;
}

static ssize_t al_inject_col_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	al_inject_show_impl(col);
}

static ssize_t al_inject_rank_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	al_inject_show_impl(rank);
}

static ssize_t al_inject_row_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	al_inject_show_impl(row);
}

static ssize_t al_inject_bank_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	al_inject_show_impl(bank);
}

/* show a short help for the sysfs attributes */
static ssize_t al_inject_help_show(
	struct device *dev,
	struct device_attribute *mattr,
	char *data)
{
	return sprintf(data,
		"inject help\n"
		"-----------\n"
		"All of the following attributes use the sysfs interface for\n"
		"setting/showing values:\n"
		"echo VALUE > ATTRIBUTE - set ATTRIBUTE to VALUE\n"
		"cat ATTRIBUTE          - show the current value of ATTRIBUTE\n"
		"\nAvailable commands:\n"
		"- inject_phys_addr\n"
		"\tset/show physical address for UC error injection\n"
		"- inject_col/rank/row/bank\n"
		"\tset/show ddr col/rank/row/bank value for UC error injection\n"
		"- inject_enable\n"
		"\tenable/disable the device (by setting to 1/0), or print\n"
	       	"\tcurrent state\n"
		"\t(*)when changing an address configuration, you need to\n"
	       	"\t   re-enable the interface i.o. to apply your changes\n"
		);
}

/* define and bind all of the store/show implementations with their
 * corresponding sysfs attributes */
static DEVICE_ATTR(inject_phys_addr, DEVICE_ATTR_FLAGS,
		al_inject_phys_addr_show,  al_inject_phys_addr_store);
static DEVICE_ATTR(inject_enable, DEVICE_ATTR_FLAGS,
		al_inject_enable_show,  al_inject_enable_store);
static DEVICE_ATTR(inject_col,DEVICE_ATTR_FLAGS,
		al_inject_col_show,  al_inject_col_store);
static DEVICE_ATTR(inject_rank,DEVICE_ATTR_FLAGS,
		al_inject_rank_show, al_inject_rank_store);
static DEVICE_ATTR(inject_row,DEVICE_ATTR_FLAGS,
		al_inject_row_show,  al_inject_row_store);
static DEVICE_ATTR(inject_bank,DEVICE_ATTR_FLAGS,
		al_inject_bank_show, al_inject_bank_store);
static DEVICE_ATTR(inject_help, S_IRUGO,
		al_inject_help_show, NULL);

static void al_delete_sysfs_devices(struct mem_ctl_info *mci)
{
	device_remove_file(&mci->dev, &dev_attr_inject_phys_addr);
	device_remove_file(&mci->dev, &dev_attr_inject_enable);
	device_remove_file(&mci->dev, &dev_attr_inject_col);
	device_remove_file(&mci->dev, &dev_attr_inject_rank);
	device_remove_file(&mci->dev, &dev_attr_inject_row);
	device_remove_file(&mci->dev, &dev_attr_inject_bank);
	device_remove_file(&mci->dev, &dev_attr_inject_help);
}

static int al_create_sysfs_devices(struct mem_ctl_info *mci)
{
	int rc;

	rc = device_create_file(&mci->dev, &dev_attr_inject_phys_addr);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_enable);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_col);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_rank);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_row);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_bank);
	if (rc < 0)
		goto err;
	rc = device_create_file(&mci->dev, &dev_attr_inject_help);
	if (rc < 0)
		goto err;

	return rc;
err:
	al_delete_sysfs_devices(mci);
	return rc;
}

/*
 * end of sysfs section
 */


static int al_mc_probe(struct platform_device *pdev)
{
	struct edac_mc_layer layers[1];
	struct mem_ctl_info *mci = NULL;
	struct al_mc_drvdata *drvdata;
	struct dimm_info *dimm;
	struct resource *r;
	struct al_ddr_ecc_cfg ecc_cfg;
	void __iomem *vbase;
	int ecc_corr_irq, ecc_uncorr_irq;
	unsigned int active_ranks, rank_addr_bits;
	int i, res = 0;

	if (!devres_open_group(&pdev->dev, NULL, GFP_KERNEL))
		return -ENOMEM;

	/* initialize the controller private database */
	/* set controller register base address */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		dev_err(&pdev->dev, "Unable to get mem resource\n");
		res = -ENODEV;
		goto err;
	}

	if (!devm_request_mem_region(&pdev->dev, r->start,
				     resource_size(r), dev_name(&pdev->dev))) {
		dev_err(&pdev->dev, "Error while requesting mem region\n");
		res = -EBUSY;
		goto err;
	}

	vbase = devm_ioremap(&pdev->dev, r->start, resource_size(r));
	if (!vbase) {
		dev_err(&pdev->dev, "Unable to map regs\n");
		res = -ENOMEM;
		goto err;
	}

	al_ddr_ecc_cfg_get(vbase, &ecc_cfg);
	if (!ecc_cfg.ecc_enabled) {
		dev_err(&pdev->dev, "No ECC present, or ECC disabled\n");
		res = -ENODEV;
		goto err;
	}

	active_ranks = al_ddr_active_ranks_get(vbase);
	if (!active_ranks) {
		dev_err(&pdev->dev, "Failed to detect active ranks\n");
		res = -ENODEV;
		goto err;
	}

	layers[0].type = EDAC_MC_LAYER_CHIP_SELECT;
	layers[0].size = active_ranks;
	layers[0].is_virt_csrow = true;
	mci = edac_mc_alloc(0, ARRAY_SIZE(layers), layers,
			    sizeof(struct al_mc_drvdata));
	if (!mci)
		return -ENOMEM;

	mci->pdev = &pdev->dev;
	drvdata = mci->pvt_info;
	drvdata->mci = mci;
	drvdata->vbase = vbase;
	platform_set_drvdata(pdev, mci);

	/* set default address for inject mechanism */
	drvdata->inject.col  = 0;
	drvdata->inject.rank = 0;
	drvdata->inject.row  = 0;
	drvdata->inject.bank = 0;
	drvdata->inject_enabled = 0;
	INIT_DELAYED_WORK(&drvdata->handle_corr_err_work, al_mc_corr_err_work);
	INIT_DELAYED_WORK(&drvdata->handle_uncorr_err_work, al_mc_uncorr_err_work);

	ecc_corr_irq =
		al_fabric_get_cause_irq(0, AL_FABRIC_IRQ_MCTL_ECC_CORR_ERR);
	ecc_uncorr_irq =
		al_fabric_get_cause_irq(0, AL_FABRIC_IRQ_MCTL_ECC_UNCORR_ERR);

	/*
	 * Configure the Memory Controller Info struct, according to the
	 * following:
	 *   - Use DDR3 type memory
	 *   - Single-bit Error Correction, Double-bit Error Detection (SECDED)
	 *   - Scrub status is set according to the controller's configuration
	 */
	mci->mtype_cap = MEM_FLAG_DDR3;
	mci->edac_ctl_cap = EDAC_FLAG_NONE | EDAC_FLAG_SECDED;
	mci->edac_cap = EDAC_FLAG_SECDED;
	mci->mod_name = dev_name(&pdev->dev);
	mci->mod_ver = "1";
	mci->ctl_name = dev_name(&pdev->dev);
	mci->scrub_mode = ecc_cfg.ecc_enabled ?
		(ecc_cfg.scrub_enabled ? SCRUB_HW_SRC : SCRUB_NONE) :
		SCRUB_UNKNOWN;

	rank_addr_bits = al_ddr_bits_per_rank_get(drvdata->vbase);
	/*
	 * Set dimm attributes
	 *   - Use DDR3 type memory
	 *   - Single-bit Error Correction, Double-bit Error Detection (SECDED)
	 *   - Number of pages can be calculated using rank size and page shift
	 *   - Granularity of reported errors (in bytes) according to data width
	 */
	for (i = 0 ; i < active_ranks ; i++) {
		dimm = mci->dimms[i];
		dimm->nr_pages = (1ULL << rank_addr_bits) >> PAGE_SHIFT;
		dimm->grain = (al_ddr_data_width_get(drvdata->vbase) ==
				AL_DDR_DATA_WIDTH_64_BITS) ? 8 : 4;
		dimm->dtype = DEV_UNKNOWN;
		dimm->mtype = MEM_DDR3;
		dimm->edac_mode = EDAC_SECDED;
	}

	res = edac_mc_add_mc(mci);
	if (res < 0)
		goto err;

	res = devm_request_irq(&pdev->dev, ecc_corr_irq,
			al_mc_corr_err_handler, 0, dev_name(&pdev->dev), mci);
	if (res < 0) {
		dev_err(&pdev->dev, "IRQ request failed (ecc corr irq) %d\n",
				ecc_corr_irq);
		goto err;
	}

	res = devm_request_irq(&pdev->dev, ecc_uncorr_irq,
			al_mc_uncorr_err_handler, 0, dev_name(&pdev->dev), mci);
	if (res < 0) {
		dev_err(&pdev->dev, "IRQ request failed (ecc uncorr irq) %d\n",
				ecc_uncorr_irq);
		goto err;
	}

	res = al_create_sysfs_devices(mci);
	if (res < 0)
		goto err;

	devres_close_group(&pdev->dev, NULL);
	return 0;
err:
	devres_release_group(&pdev->dev, NULL);
	if (mci)
		edac_mc_free(mci);
	return res;
}

static int al_mc_remove(struct platform_device *pdev)
{
	struct mem_ctl_info *mci = platform_get_drvdata(pdev);

	edac_mc_del_mc(&pdev->dev);
	al_delete_sysfs_devices(mci);
	edac_mc_free(mci);
	return 0;
}

static const struct of_device_id al_mc_of_match[] = {
	{ .compatible = "annapurna-labs,al-mc", },
	{},
};
MODULE_DEVICE_TABLE(of, al_mc_of_match);

static struct platform_driver al_mc_edac_driver = {
	.probe = al_mc_probe,
	.remove = al_mc_remove,
	.driver = {
		.name = "al_mc_edac",
		.of_match_table = al_mc_of_match,
	},
};

module_platform_driver(al_mc_edac_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Annapurna Labs Inc.");
MODULE_DESCRIPTION("EDAC Driver for Annapurna Labs MC");
