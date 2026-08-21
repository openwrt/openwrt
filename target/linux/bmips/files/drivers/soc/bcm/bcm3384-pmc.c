// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3384 Power Management Controller command mailbox
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/slab.h>

#include <dt-bindings/power/bcm3384-power.h>

// BCHP_Control_PMC.PmcToMipsMailbox
#define PMC_MBOX_STATUS				0x0818
#define PMC_MBOX_STATUS_RESPONSE_READY		BIT(2)

// BCHP_Control_PMC.MipsToPmcMailbox
#define PMC_MBOX_TRIGGER			0x00b4
#define PMC_MBOX_TRIGGER_SEND			0xa000afc8

// BCHP_Control_PMC.MipsToPmcStatus
#define PMC_MBOX_BUSY				0x00b8

// BCHP_QUEUE_0_DATA_PMC / command words written by bootloader PMC command path
#define PMC_CMD_WORD0				0x0c10
#define PMC_CMD_WORD1				0x0c14
#define PMC_CMD_WORD2				0x0c18
#define PMC_CMD_WORD3				0x0c1c

// BCHP_QUEUE_1_DATA_PMC / response words read by bootloader PMC command path
#define PMC_RESP_WORD0				0x0c20
#define PMC_RESP_WORD1				0x0c24
#define PMC_RESP_WORD2				0x0c28
#define PMC_RESP_WORD3				0x0c2c

// Bootloader MCP_iBootPmcPowerDeviceOnCommand() uses command 0x81.
#define PMC_CMD_POWER_DEVICE			0x81
#define PMC_CMD_POWER_CYCLE_DEVICE		0x83
// Bootloader MCP_iBootPmcWriteBpcmRegCommand() uses command 0x0d.
#define PMC_CMD_WRITE_BPCM_REG			0x0d
#define PMC_CMD_TIMEOUT_US			1000000

#define PMC_BPCM_DEVICE_FPM			1
#define PMC_BPCM_DEVICE_MISC			2
#define PMC_BPCM_DEVICE_NETWORK			30
#define PMC_BPCM_DEVICE_PCIE0			33
#define PMC_BPCM_DEVICE_PCIE1			34
#define PMC_BPCM_DEVICE_INTERNAL_PHY		40

#define PMC_INTERNAL_PHY_MISC_BPCM_OFFSET	0x44
#define PMC_INTERNAL_PHY_BPCM_RESET_OFFSET	0x20

#define PMC_NUM_POWER_DOMAINS			4

struct bcm3384_pmc;

struct bcm3384_pmc_domain {
	struct generic_pm_domain genpd;
	struct bcm3384_pmc *pmc;
	int (*power_on)(struct bcm3384_pmc_domain *domain);
};

struct bcm3384_pmc {
	struct device *dev;
	void __iomem *base;
	struct genpd_onecell_data onecell;
	struct generic_pm_domain *genpds[PMC_NUM_POWER_DOMAINS];
	struct bcm3384_pmc_domain domains[PMC_NUM_POWER_DOMAINS];
	struct mutex lock;
	u8 sequence;
};

struct pmc_command {
	u8 command;
	u32 device;
	u32 zone;
	u32 arg0;
	u32 arg1;
	bool log_result_error;
};

static int pmc_send_command(struct bcm3384_pmc *pmc,
			    const struct pmc_command *command)
{
	if (command->device > 0x3ff || command->zone > 0x3ff)
		return -EINVAL;

	mutex_lock(&pmc->lock);

	u8 sequence = pmc->sequence++;
	u32 cmd0 = ((u32)sequence << 16) | command->command;
	u32 cmd1 = (command->device << 10) | command->zone;

	// Discard any stale responses from PMC before sending a new command.
	for (unsigned int i = 0; i < 8; i++) {
		u32 status = readl_be(pmc->base + PMC_MBOX_STATUS);

		if (!(status & PMC_MBOX_STATUS_RESPONSE_READY))
			break;

		readl_be(pmc->base + PMC_RESP_WORD0);
		readl_be(pmc->base + PMC_RESP_WORD1);
		readl_be(pmc->base + PMC_RESP_WORD2);
		readl_be(pmc->base + PMC_RESP_WORD3);
		writel_be(status | PMC_MBOX_STATUS_RESPONSE_READY,
			  pmc->base + PMC_MBOX_STATUS);
		udelay(1);
	}

	writel_be(cmd0, pmc->base + PMC_CMD_WORD0);
	writel_be(cmd1, pmc->base + PMC_CMD_WORD1);
	writel_be(command->arg0, pmc->base + PMC_CMD_WORD2);
	writel_be(command->arg1, pmc->base + PMC_CMD_WORD3);
	writel_be(PMC_MBOX_TRIGGER_SEND, pmc->base + PMC_MBOX_TRIGGER);

	// Wait for PMC to set the RESPONSE_READY bit in the status register.
	u32 status;
	int ret = read_poll_timeout(readl_be, status,
				    status & PMC_MBOX_STATUS_RESPONSE_READY,
				    1, PMC_CMD_TIMEOUT_US, false,
				    pmc->base + PMC_MBOX_STATUS);
	if (ret) {
		ret = dev_err_probe(pmc->dev, ret,
				    "PMC command 0x%02x device %u timed out, busy=0x%08x status=0x%08x\n",
				    command->command, command->device,
				    readl_be(pmc->base + PMC_MBOX_BUSY),
				    readl_be(pmc->base + PMC_MBOX_STATUS));
		goto out_unlock;
	}

	u32 resp0 = readl_be(pmc->base + PMC_RESP_WORD0);
	u8 resp_sequence = (resp0 >> 16) & 0xff;
	u8 result = (resp0 >> 8) & 0xff;
	status = readl_be(pmc->base + PMC_MBOX_STATUS);

	writel_be(status | PMC_MBOX_STATUS_RESPONSE_READY,
		  pmc->base + PMC_MBOX_STATUS);

	if (resp_sequence != sequence) {
		dev_err(pmc->dev,
			"PMC command 0x%02x device %u sequence mismatch: sent=%u resp=%u resp0=0x%08x\n",
			command->command, command->device, sequence,
			resp_sequence, resp0);
		ret = -EIO;
		goto out_unlock;
	}

	if (result) {
		if (command->log_result_error)
			dev_err(pmc->dev,
				"PMC command 0x%02x device %u failed: result=%u resp0=0x%08x resp1=0x%08x "
				"resp2=0x%08x resp3=0x%08x\n",
				command->command, command->device, result, resp0,
				readl_be(pmc->base + PMC_RESP_WORD1),
				readl_be(pmc->base + PMC_RESP_WORD2),
				readl_be(pmc->base + PMC_RESP_WORD3));
		ret = -EIO;
		goto out_unlock;
	}

	ret = 0;
	goto out_unlock;

out_unlock:
	mutex_unlock(&pmc->lock);
	return ret;
}

static int pmc_power_on_device(struct bcm3384_pmc *pmc, u32 device)
{
	const struct pmc_command command = {
		.command = PMC_CMD_POWER_DEVICE,
		.device = device,
		.arg0 = 1,
		.log_result_error = true,
	};

	return pmc_send_command(pmc, &command);
}

static int pmc_power_off_device(struct bcm3384_pmc *pmc, u32 device)
{
	const struct pmc_command command = {
		.command = PMC_CMD_POWER_DEVICE,
		.device = device,
		.log_result_error = true,
	};

	return pmc_send_command(pmc, &command);
}

static int pmc_power_cycle_device(struct bcm3384_pmc *pmc, u32 device)
{
	const struct pmc_command command = {
		.command = PMC_CMD_POWER_CYCLE_DEVICE,
		.device = device,
	};

	return pmc_send_command(pmc, &command);
}

static int pmc_write_bpcm_reg(struct bcm3384_pmc *pmc, u32 device,
			      u32 byte_offset, u32 value)
{
	const struct pmc_command command = {
		.command = PMC_CMD_WRITE_BPCM_REG,
		.device = device,
		.arg0 = byte_offset >> 2,
		.arg1 = value,
		.log_result_error = true,
	};

	return pmc_send_command(pmc, &command);
}

static int pmc_power_on_fpm(struct bcm3384_pmc_domain *domain)
{
	struct bcm3384_pmc *pmc = domain->pmc;
	int ret;

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_FPM);
	if (ret)
		return ret;

	msleep(1);

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_NETWORK);
	if (ret)
		return ret;

	msleep(1);

	/*
	 * The BCM3384 bootloader resets FPM before InitMem by cycling PMC
	 * device 30:
	 *   PowerDeviceOff(30);
	 *   delay 100ms;
	 *   PowerDeviceOn(30);
	 *   delay 1ms.
	 */
	ret = pmc_power_off_device(pmc, PMC_BPCM_DEVICE_NETWORK);
	if (ret)
		return ret;

	msleep(100);

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_NETWORK);
	if (ret)
		return ret;

	msleep(1);

	return 0;
}

static int pmc_power_on_pcie0(struct bcm3384_pmc_domain *domain)
{
	struct bcm3384_pmc *pmc = domain->pmc;
	int ret;

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_PCIE0);
	if (ret)
		return ret;

	ret = pmc_power_cycle_device(pmc, PMC_BPCM_DEVICE_PCIE0);
	if (ret)
		dev_dbg(pmc->dev,
			"PMC %s power-cycle returned %d, continuing like eCos\n",
			domain->genpd.name, ret);

	return 0;
}

static int pmc_power_on_pcie1(struct bcm3384_pmc_domain *domain)
{
	struct bcm3384_pmc *pmc = domain->pmc;
	int ret;

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_PCIE1);
	if (ret)
		return ret;

	ret = pmc_power_cycle_device(pmc, PMC_BPCM_DEVICE_PCIE1);
	if (ret)
		dev_dbg(pmc->dev,
			"PMC %s power-cycle returned %d, continuing like eCos\n",
			domain->genpd.name, ret);

	return 0;
}

static int pmc_power_on_internal_phy(struct bcm3384_pmc_domain *domain)
{
	struct bcm3384_pmc *pmc = domain->pmc;
	int ret;

	ret = pmc_power_off_device(pmc, PMC_BPCM_DEVICE_INTERNAL_PHY);
	if (ret)
		return ret;

	ret = pmc_power_on_device(pmc, PMC_BPCM_DEVICE_INTERNAL_PHY);
	if (ret)
		return ret;

	ret = pmc_write_bpcm_reg(pmc, PMC_BPCM_DEVICE_MISC,
				 PMC_INTERNAL_PHY_MISC_BPCM_OFFSET, 1);
	if (ret)
		return ret;

	mdelay(5);

	ret = pmc_write_bpcm_reg(pmc, PMC_BPCM_DEVICE_INTERNAL_PHY,
				 PMC_INTERNAL_PHY_BPCM_RESET_OFFSET, 1);
	if (ret)
		return ret;

	mdelay(1);

	ret = pmc_write_bpcm_reg(pmc, PMC_BPCM_DEVICE_INTERNAL_PHY,
				 PMC_INTERNAL_PHY_BPCM_RESET_OFFSET, 0);
	if (ret)
		return ret;

	return 0;
}

static int pmc_power_on_domain(struct generic_pm_domain *genpd)
{
	struct bcm3384_pmc_domain *domain =
		container_of(genpd, struct bcm3384_pmc_domain, genpd);

	if (!domain->power_on)
		return 0;

	return domain->power_on(domain);
}

static int pmc_init_domain(struct bcm3384_pmc *pmc, unsigned int id,
			   const char *name,
			   int (*power_on)(struct bcm3384_pmc_domain *domain))
{
	struct bcm3384_pmc_domain *domain = &pmc->domains[id];

	domain->pmc = pmc;
	domain->power_on = power_on;
	domain->genpd.name = name;
	domain->genpd.power_on = pmc_power_on_domain;
	pmc->genpds[id] = &domain->genpd;

	return pm_genpd_init(&domain->genpd, NULL, true);
}

static void pmc_remove_genpd_provider(void *data)
{
	of_genpd_del_provider(data);
}

static int pmc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3384_pmc *pmc = devm_kzalloc(dev, sizeof(*pmc), GFP_KERNEL);
	if (!pmc)
		return -ENOMEM;

	pmc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(pmc->base))
		return PTR_ERR(pmc->base);

	pmc->dev = dev;
	mutex_init(&pmc->lock);
	platform_set_drvdata(pdev, pmc);

	int ret = pmc_init_domain(pmc, BCM3384_POWER_DOMAIN_FPM, "fpm",
				  pmc_power_on_fpm);
	if (ret)
		return ret;

	ret = pmc_init_domain(pmc, BCM3384_POWER_DOMAIN_PCIE0, "pcie0",
			      pmc_power_on_pcie0);
	if (ret)
		return ret;

	ret = pmc_init_domain(pmc, BCM3384_POWER_DOMAIN_PCIE1, "pcie1",
			      pmc_power_on_pcie1);
	if (ret)
		return ret;

	ret = pmc_init_domain(pmc, BCM3384_POWER_DOMAIN_INTERNAL_PHY,
			      "internal-phy", pmc_power_on_internal_phy);
	if (ret)
		return ret;

	pmc->onecell.domains = pmc->genpds;
	pmc->onecell.num_domains = PMC_NUM_POWER_DOMAINS;

	ret = of_genpd_add_provider_onecell(dev->of_node, &pmc->onecell);
	if (ret)
		return ret;

	ret = devm_add_action_or_reset(dev, pmc_remove_genpd_provider,
				       dev->of_node);
	if (ret)
		return ret;

	dev_info(dev, "BCM3384 PMC power-domain provider ready\n");

	return 0;
}

static const struct of_device_id pmc_of_match[] = {
	{ .compatible = "brcm,bcm3384-pmc" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, pmc_of_match);

static struct platform_driver pmc_driver = {
	.probe = pmc_probe,
	.driver = {
		.name = "bcm3384-pmc",
		.of_match_table = pmc_of_match,
	},
};
module_platform_driver(pmc_driver);

MODULE_AUTHOR("Hang Zhou <929513338@qq.com>");
MODULE_DESCRIPTION("BCM3384 PMC command mailbox driver");
MODULE_LICENSE("GPL");
