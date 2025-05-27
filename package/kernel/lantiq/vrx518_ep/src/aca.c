/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux driver
  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/
#define DEBUG
#include <linux/init.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/firmware.h>

#include <net/dc_ep.h>

#include "regs.h"
#include "ep.h"
#include "misc.h"
#include "aca.h"

#define ACA_FW_FILE		"aca_fw.bin"

#define set_mask_bit(val, set, mask, bits)		\
	(val = (((val) & (~((mask) << (bits))))	\
	| (((set) & (mask)) << (bits))))

static char soc_str[128];

static const char *const aca_img_type_str[ACA_IMG_MAX] = {
	"vrx518",
	"vrx618",
	"falcon-mx",
	"pmua",
};

static void soc_type_to_str(u32 soc)
{
	memset(soc_str, 0, sizeof(soc_str));

	if ((soc & ACA_SOC_XRX300))
		strcat(soc_str, "xrx300 ");

	if ((soc & ACA_SOC_XRX500))
		strcat(soc_str, "xrx500 ");

	if ((soc & ACA_SOC_PUMA))
		strcat(soc_str, "puma ");

	if ((soc & ACA_SOC_3RD_PARTY))
		strcat(soc_str, "third party SoC ");
}

static const char *fw_id_to_str(u32 fw_id)
{
	switch (fw_id) {
	case ACA_FW_TXIN:
		return "txin";

	case ACA_FW_TXOUT:
		return "txout";

	case ACA_FW_RXIN:
		return "rxin";

	case ACA_FW_RXOUT:
		return "rxout";

	case ACA_FW_GNRC:
		return "Genrisc";

	default:
		return "unknow";
	}
}

static const char * const sec_id_str[] = {
	"Unknown", "HIF", "GenRisc", "MAC_HT", "TXIN", "TXIN_PDRING", "TXOUT",
	"TXOUT_PDRING", "RXIN", "RXIN_PDRING", "RXOUT", "RXOUT_PDRING", "DMA",
	"FW_INIT",
};
static const char *sec_id_to_str(u32 sec_id)
{
	switch (sec_id) {
	case ACA_SEC_HIF:
	case ACA_SEC_GNR:
	case ACA_SEC_MAC_HT:
	case ACA_SEC_MEM_TXIN:
	case ACA_SEC_MEM_TXIN_PDRING:
	case ACA_SEC_MEM_TXOUT:
	case ACA_SEC_MEM_TXOUT_PDRING:
	case ACA_SEC_MEM_RXIN:
	case ACA_SEC_MEM_RXIN_PDRING:
	case ACA_SEC_MEM_RXOUT:
	case ACA_SEC_MEM_RXOUT_PDRING:
	case ACA_SEC_DMA:
	case ACA_SEC_FW_INIT:
		return sec_id_str[sec_id];
	case ACA_SEC_FW:
		return "ACA FW";

	default:
		return "unknown";
	}
}

static inline struct aca_fw_info *to_fw_info(struct dc_ep_priv *priv)
{
	return &priv->aca.fw_info;
}

static inline struct aca_fw_dl_addr *to_fw_addr(struct dc_ep_priv *priv)
{
	return &priv->aca.fw_info.fw_dl;
}

static inline struct aca_mem_layout *to_mem_layout(struct dc_ep_priv *priv)
{
	return &priv->aca.fw_info.mem_layout;
}

static inline struct aca_pdmem_layout *to_pdmem_layout(struct dc_ep_priv *priv)
{
	return &priv->aca.fw_info.pdmem_layout;
}

static inline struct aca_fw_param *to_aca_fw_param(struct dc_ep_priv *priv)
{
	return &priv->aca.fw_info.fw_param;
}

static inline struct aca_hif_params *to_hif_params(struct dc_ep_priv *priv)
{
	return priv->aca.hif_params;
}

static const struct firmware *aca_fetch_fw_file(struct dc_ep_priv *priv,
	char *dir, const char *file)
{
	int ret;
	char filename[100] = {0};
	const struct firmware *fw;

	if (file == NULL)
		return ERR_PTR(-ENOENT);

	if (dir == NULL)
		dir = ".";

	snprintf(filename, sizeof(filename), "%s/%s", dir, file);
	ret = request_firmware(&fw, filename, priv->dev);
	if (ret)
		return ERR_PTR(ret);

	return fw;
}

void dc_aca_free_fw_file(struct dc_ep_priv *priv)
{
	struct aca_fw_info *fw_info = to_fw_info(priv);

	if (fw_info->fw && !IS_ERR(fw_info->fw))
		release_firmware(fw_info->fw);

	fw_info->fw = NULL;
	fw_info->fw_data = NULL;
	fw_info->fw_len = 0;
}

static void aca_dma_parse(struct dc_ep_priv *priv, const char *data, int chn)
{
	int i;
	u32 cid, dbase;
	struct aca_fw_dma *fw_dma;
	struct aca_fw_info *fw_info = to_fw_info(priv);

	fw_info->chan_num = chn;

	for (i = 0; i < fw_info->chan_num; i++) {
		fw_dma = (struct aca_fw_dma *)(data + i * sizeof(*fw_dma));
		cid = be32_to_cpu(fw_dma->cid);
		dbase = be32_to_cpu(fw_dma->base);
		fw_info->adma_desc_base[cid] = dbase;
		dev_dbg(priv->dev, "dma channel %d desc base 0x%08x\n",
			cid, dbase);
	}
}

static void aca_sram_desc_parse(struct dc_ep_priv *priv, const char *data,
	u32 sid)
{
	u32 dbase, dnum;
	struct aca_sram_desc *desc_base;
	struct aca_mem_layout *mem_layout = to_mem_layout(priv);
	struct aca_pdmem_layout *pdmem = to_pdmem_layout(priv);

	desc_base = (struct aca_sram_desc *)data;
	dbase = be32_to_cpu(desc_base->dbase);
	dnum = be32_to_cpu(desc_base->dnum);

	dev_dbg(priv->dev, "Sec %s desc base 0x%08x, des_num: %d\n",
		sec_id_to_str(sid), dbase, dnum);

	switch (sid) {
	case ACA_SEC_MEM_TXIN:
		mem_layout->txin_host_desc_base = dbase;
		mem_layout->txin_host_dnum = dnum;
		break;

	case ACA_SEC_MEM_TXOUT:
		mem_layout->txout_host_desc_base = dbase;
		mem_layout->txout_host_dnum = dnum;
		break;

	case ACA_SEC_MEM_RXIN:
		mem_layout->rxin_host_desc_base = dbase;
		mem_layout->rxin_host_dnum = dnum;
		break;

	case ACA_SEC_MEM_RXOUT:
		mem_layout->rxout_host_desc_base = dbase;
		mem_layout->rxout_host_dnum = dnum;
		break;
	case ACA_SEC_MEM_TXIN_PDRING:
		pdmem->txin_pd_desc_base = dbase;
		pdmem->txin_pd_dnum = dnum;
		break;
	case ACA_SEC_MEM_TXOUT_PDRING:
		pdmem->txout_pd_desc_base = dbase;
		pdmem->txout_pd_dnum = dnum;
		break;
	case ACA_SEC_MEM_RXIN_PDRING:
		pdmem->rxin_pd_desc_base = dbase;
		pdmem->rxin_pd_dnum = dnum;
		break;
	case ACA_SEC_MEM_RXOUT_PDRING:
		pdmem->rxin_pd_desc_base = dbase;
		pdmem->rxin_pd_dnum = dnum;
		break;
	default:
		dev_err(priv->dev, "Unknow aca sram section %d\n", sid);
		break;
	}
}

static void aca_init_parse(struct dc_ep_priv *priv, const char *data,
	u32 sid)
{
	struct aca_fw_param *fw_param = to_aca_fw_param(priv);
	struct aca_fw_param *param;
	u32 hdr_sz, hdr_addr;

	param = (struct aca_fw_param *)data;
	hdr_sz = be32_to_cpu(param->st_sz);
	hdr_addr = be32_to_cpu(param->init_addr);

	fw_param->init_addr = hdr_addr;
	fw_param->st_sz = hdr_sz;
	dev_dbg(priv->dev, "init st size: %d, addr: 0x%x\n",
		hdr_sz, hdr_addr);
}

static void aca_fw_parse(struct dc_ep_priv *priv, const char *data,
	const char *fw_base, int fw_num)
{
	int i;
	size_t size;
	u32 id, offset, addr;
	struct aca_int_hdr *hdr;
	struct aca_fw_dl_addr *fw_dl = to_fw_addr(priv);

	fw_dl->fw_num = fw_num;

	for (i = 0; i < fw_dl->fw_num; i++) {
		hdr = (struct aca_int_hdr *)(data + i * sizeof(*hdr));
		id = be32_to_cpu(hdr->id);
		offset = be32_to_cpu(hdr->offset);
		size = be32_to_cpu(hdr->size);
		addr = be32_to_cpu(hdr->load_addr);

		fw_dl->fw_addr[i].fw_id = id;
		fw_dl->fw_addr[i].fw_load_addr = addr;
		fw_dl->fw_addr[i].fw_size = size;
		fw_dl->fw_addr[i].fw_base = fw_base + offset;
		dev_dbg(priv->dev,
			"aca %s fw offset 0x%x size %zd loc 0x%x fw base %p\n",
			fw_id_to_str(id), offset, size, addr, fw_base + offset);
	}
}

/* --------------------------------------------------------
  |              Fixed header (20Bytes)                   |
  ---------------------------------------------------------
  |              Variable header                          |
  |                ie / payload                           |
  |-------------------------------------------------------|
  |               Actual ACA FW                           |
  ---------------------------------------------------------
*/
static int aca_section_parse(struct dc_ep_priv *priv, const char *fw_data)
{
	int ret = 0;
	u32 fixed_hlen;
	u32 var_hlen;
	u32 ie_id;
	size_t ie_len, ie_hlen, ie_dlen;
	u32 fw_hlen;
	struct aca_fw_f_hdr *fw_f_hdr;
	struct aca_fw_ie *ie_hdr;
	struct aca_int_hdr *aca_hdr;
	const char *data = fw_data;
	const char *aca_fw_data;
	struct device *dev = priv->dev;

	fw_f_hdr = (struct aca_fw_f_hdr *)data;

	fw_hlen = be32_to_cpu(fw_f_hdr->hdr_size);
	fixed_hlen = sizeof(*fw_f_hdr);
	var_hlen = fw_hlen - fixed_hlen;
	ie_hlen = sizeof(*ie_hdr);

	/* Record actual ACA fw data pointer */
	aca_fw_data = data + fw_hlen;

	/* Point to variable header and parse them */
	data += fixed_hlen;

	while (var_hlen > ie_hlen) {
		/* Variable header information element */
		ie_hdr = (struct aca_fw_ie *)data;
		ie_id = be32_to_cpu(ie_hdr->id);
		ie_len = be32_to_cpu(ie_hdr->len);
		dev_dbg(dev, "Section %s ie_len %zd\n", sec_id_to_str(ie_id),
			ie_len);

		/* Variable header data conents */
		data += ie_hlen;
		var_hlen -= ie_hlen;

		switch (ie_id) {
		case ACA_SEC_HIF:
		case ACA_SEC_GNR:
		case ACA_SEC_MAC_HT:
			ie_dlen = ie_len * sizeof(struct aca_fw_reg);
			data += ie_dlen;
			var_hlen -= ie_dlen;

			break;

		case ACA_SEC_MEM_TXIN:
		case ACA_SEC_MEM_TXOUT:
		case ACA_SEC_MEM_RXIN:
		case ACA_SEC_MEM_RXOUT:
		case ACA_SEC_MEM_TXIN_PDRING:
		case ACA_SEC_MEM_TXOUT_PDRING:
		case ACA_SEC_MEM_RXIN_PDRING:
		case ACA_SEC_MEM_RXOUT_PDRING:
			aca_sram_desc_parse(priv, data, ie_id);
			ie_dlen = ie_len * sizeof(struct aca_sram_desc);
			data += ie_dlen;
			var_hlen -= ie_dlen;
			break;

		case ACA_SEC_DMA:
			if (ie_len > ACA_DMA_CHAN_MAX) {
				dev_err(dev, "invalid dma channel %d\n",
					ie_len);
				ret = -EINVAL;
				goto done;
			}
			aca_dma_parse(priv, data, ie_len);
			ie_dlen = ie_len * sizeof(struct aca_fw_dma);
			data += ie_dlen;
			var_hlen -= ie_dlen;
			break;

		case ACA_SEC_FW_INIT:
			aca_init_parse(priv, data, ie_id);
			ie_dlen = ie_len * sizeof(struct aca_fw_param);
			data += ie_dlen;
			var_hlen -= ie_dlen;
			break;

		case ACA_SEC_FW:
			if (ie_len > ACA_FW_MAX) {
				dev_err(dev, "Too many aca fws %d\n", ie_len);
				ret = -EINVAL;
				goto done;
			}
			aca_fw_parse(priv, data, aca_fw_data, ie_len);
			ie_dlen = ie_len * sizeof(*aca_hdr);
			data += ie_dlen;
			var_hlen -= ie_dlen;
			break;

		default:
			dev_warn(dev, "Unknown Sec id: %u\n", ie_id);
			break;
		}
	}
done:
	return ret;
}

static int aca_fetch_fw_api(struct dc_ep_priv *priv, const char *name)
{
	int ret;
	size_t hdr_len;
	const u8 *fw_data;
	size_t fw_len;
	char dir[8] = {0};
	union fw_ver ver;
	union img_soc_type type;
	struct device *dev = priv->dev;
	struct aca_fw_f_hdr *fw_f_hdr;
	struct aca_fw_info *fw_info = to_fw_info(priv);

	sprintf(dir, "%04x", priv->pdev->device);
	fw_info->fw = aca_fetch_fw_file(priv, dir, name);
	if (IS_ERR(fw_info->fw)) {
		dev_err(dev, "Could not fetch firmware file '%s': %ld\n",
			name, PTR_ERR(fw_info->fw));
		return PTR_ERR(fw_info->fw);
	}

	fw_data = fw_info->fw->data;
	fw_len = fw_info->fw->size;

	/* Parse the fixed header part */
	fw_f_hdr = (struct aca_fw_f_hdr *)fw_data;
	ver.all = be32_to_cpu(fw_f_hdr->ver);

	dev_info(dev, "ACA fw build %d branch %d major 0x%2x minor 0x%04x\n",
		ver.field.build, ver.field.branch,
		ver.field.major, ver.field.minor);

	type.all = be32_to_cpu(fw_f_hdr->type);

	if (type.field.img_type > (ACA_IMG_MAX - 1)
		|| ((type.field.soc_type & ACA_SOC_MASK) == 0)) {
		dev_err(dev, "Invalid aca fw img %d soc %d\n",
			type.field.img_type, type.field.soc_type);
		ret = -EINVAL;
		goto err;
	}

	soc_type_to_str(type.field.soc_type);

	dev_info(priv->dev, "ACA fw for %s supported SoC type %s\n",
		aca_img_type_str[type.field.img_type], soc_str);

	hdr_len = be32_to_cpu(fw_f_hdr->hdr_size);
	/* Sanity Check */
	if (fw_len < hdr_len) {
		dev_err(dev, "Invalid aca fw hdr len %zd fw len %zd\n",
			hdr_len, fw_len);
		ret = -EINVAL;
		goto err;
	}
	dev_dbg(dev, "Header size 0x%08x fw size 0x%08x\n",
		hdr_len, be32_to_cpu(fw_f_hdr->fw_size));
	dev_dbg(dev, "section number %d\n",
		be32_to_cpu(fw_f_hdr->num_section));

	aca_section_parse(priv, fw_data);
	return 0;
err:
	dc_aca_free_fw_file(priv);
	return ret;
}

static int aca_fetch_fw(struct dc_ep_priv *priv)
{
	return aca_fetch_fw_api(priv, ACA_FW_FILE);
}

static int aca_fw_download(struct dc_ep_priv *priv)
{
	int i, j;
	u32 val;
	size_t size;
	u32 id, load_addr;
	const char *fw_base;
	struct aca_fw_dl_addr *fw_dl = to_fw_addr(priv);

	for (i = 0; i < fw_dl->fw_num; i++) {
		id = fw_dl->fw_addr[i].fw_id;
		load_addr = fw_dl->fw_addr[i].fw_load_addr;
		size = fw_dl->fw_addr[i].fw_size;
		fw_base = fw_dl->fw_addr[i].fw_base;

		if (size % 4) {
			dev_err(priv->dev,
				"aca %s fw size is not a multiple of 4\n",
				fw_id_to_str(id));
			return -EINVAL;
		}

		for (j = 0; j < size; j += 4) {
			val = *((u32 *)(fw_base + j));
			wr32(cpu_to_be32(val), load_addr + j);
		}
		/* Write flush */
		rd32(load_addr);
	#ifdef DEBUG
		{
		u32 src, dst;

		for (j = 0; j < size; j += 4) {
			dst = rd32(load_addr + j);
			src = *((u32 *)(fw_base + j));
			if (dst != cpu_to_be32(src)) {
				dev_info(priv->dev,
					"dst 0x%08x != src 0x%08x\n", dst, src);
				return -EIO;
			}
		}
		}
	#endif /* DEBUG */
	}
	return 0;
}

static void aca_dma_ctrl_init(struct dc_ep_priv *priv)
{
	u32 val;
	struct dc_aca *aca = to_aca(priv);

	/* Global software reset CDMA */
	wr32_mask(0, BIT(CTRL_RST), ADMA_CTRL);
	while ((rd32(ADMA_CTRL) & BIT(CTRL_RST)))
		;

	val = rd32(ADMA_ID);
	/* Record max dma channels for later usage */
	aca->adma_chans = MS(val, ADMA_ID_CHNR);
	val = rd32(ADMA_CTRL);
	/*
	 * Enable Packet Arbitration
	 * Enable Meta data copy
	 * Enable Dedicated Descriptor port
	 */
	val |= BIT(CTRL_PKTARB) | BIT(CTRL_MDC) | BIT(CTRL_DSRAM);
	set_mask_bit(val, 1, 1, CTRL_ENBE); /* Enable byte enable */
	set_mask_bit(val, 1, 1, CTRL_DCNF); /* 2DW descriptor format */
	set_mask_bit(val, 1, 1, CTRL_DDBR); /* Descriptor read back */
	set_mask_bit(val, 1, 1, CTRL_DRB); /* Dynamic burst read */
	wr32(val, ADMA_CTRL);

	/* Polling cnt cfg */
	wr32(ADMA_CPOLL_EN | SM(ADMA_DEFAULT_POLL, ADMA_CPOLL_CNT),
		ADMA_CPOLL);
}

static void aca_dma_port_init(struct dc_ep_priv *priv)
{
	u32 val;

	/* Only one port /port 0 */
	wr32(0, ADMA_PS);
	val = rd32(ADMA_PCTRL);
	set_mask_bit(val, 1, 1, PCTRL_RXBL16);
	set_mask_bit(val, 1, 1, PCTRL_TXBL16);
	set_mask_bit(val, 0, 3, PCTRL_RXBL);
	set_mask_bit(val, 0, 3, PCTRL_TXBL);

	set_mask_bit(val, 0, 3, PCTRL_TXENDI);
	set_mask_bit(val, 0, 3, PCTRL_RXENDI);
	wr32(val, ADMA_PCTRL);
}

static void aca_dma_ch_init(struct dc_ep_priv *priv, u32 cid,
	u32 dbase, u32 dlen)
{
	/* Select channel */
	wr32(cid, ADMA_CS);

	/* Reset Channel */
	wr32_mask(0, BIT(CCTRL_RST), ADMA_CCTRL);
	while ((rd32(ADMA_CCTRL) & BIT(CCTRL_RST)))
		;

	/* Set descriptor list base and length */
	wr32(dbase, ADMA_CDBA);
	wr32(dlen, ADMA_CDLEN);

	/*Clear Intr */
	wr32(ADMA_CI_ALL, ADMA_CIS);
	/* Enable Intr */
	wr32(ADMA_CI_ALL, ADMA_CIE);

	/* Enable Channel */
	wr32_mask(0, BIT(CCTRL_ONOFF), ADMA_CCTRL);
	mb();
}

static void aca_dma_ch_off(struct dc_ep_priv *priv)
{
	int i;
	struct dc_aca *aca = to_aca(priv);

	/* Shared between OS and ACA FW. Stop ACA first */
	for (i = 0; i < aca->adma_chans; i++) {
		wr32(i, ADMA_CS);
		wr32_mask(BIT(CCTRL_ONOFF), 0, ADMA_CCTRL);
		while (rd32(ADMA_CCTRL) & BIT(CCTRL_ONOFF))
			;
	}
	dev_dbg(priv->dev, "aca dma channel done\n");
}

static void aca_xbar_ia_reject_set(struct dc_ep_priv *priv, int ia_id)
{
	u32 val;
	int timeout = 1000;
	struct device *dev = priv->dev;

	/* Set reject bit */
	wr32(XBAR_CTRL_REJECT, ACA_AGENT_CTRL(ia_id));

	/* Poll burst, readex, resp_waiting, req_active */
	val = XBAR_STAT_REQ_ACTIVE | XBAR_STAT_RESP_WAITING
		| XBAR_STAT_BURST | XBAR_STAT_READEX;
	while (--timeout && !!(rd32(ACA_AGENT_STATUS(ia_id)) & val))
		udelay(1);

	if (timeout <= 0) {
		dev_dbg(dev,
			"ACA XBAR IA: %d reset timeout, pending on 0x%x\n",
			ia_id, rd32(ACA_AGENT_STATUS(ia_id)));
		return;
	}
}

static void aca_xbar_ia_reject_clr(struct dc_ep_priv *priv, int ia_id)
{
	u32 val;

	/* Check reject bit */
	val = rd32(ACA_AGENT_CTRL(ia_id));
	if ((val & XBAR_CTRL_REJECT) == 0)
		return;

	/* Clear reject bit */
	val &= ~XBAR_CTRL_REJECT;
	wr32(val, ACA_AGENT_CTRL(ia_id));
	rd32(ACA_AGENT_CTRL(ia_id));
}

static void aca_xbar_ia_reset(struct dc_ep_priv *priv, int ia_id)
{
	/* ACA IA reset */
	wr32(XBAR_CTRL_CORE_RESET, ACA_AGENT_CTRL(ia_id));

	/* Read till status become 1 */
	while ((rd32(ACA_AGENT_STATUS(ia_id)) & XBAR_STAT_CORE_RESET) == 0)
		;

	/* Clear the IA Reset signal */
	wr32(0, ACA_AGENT_CTRL(ia_id));

	/* Read till status become 0 */
	while ((rd32(ACA_AGENT_STATUS(ia_id)) & XBAR_STAT_CORE_RESET) == 1)
		;

	dev_dbg(priv->dev, "ACA XBAR IA(%d) reset done\n", ia_id);
}

void dc_aca_shutdown(struct dc_ep_priv *priv)
{
	struct dc_aca *aca = to_aca(priv);

	if (aca->initialized) {
		aca_xbar_ia_reset(priv, ACA_ACC_IA04);
		aca_xbar_ia_reset(priv, ACA_M_IA06);
	}
}

static void aca_dma_init(struct dc_ep_priv *priv)
{
	int i;
	struct aca_fw_info *fw_info = to_fw_info(priv);

	aca_dma_ctrl_init(priv);
	aca_dma_port_init(priv);

	for (i = 0; i < fw_info->chan_num; i++) {
		aca_dma_ch_init(priv, i,
			fw_info->adma_desc_base[i] | priv->phymem,
			DESC_NUM_PER_CH);
	}

	dev_dbg(priv->dev, "aca dma init done\n");
}

static void aca_basic_init(struct dc_ep_priv *priv)
{
	u32 addr, mask;

	/* Low 32 is RX, High 32 is TX */
	wr32(0x1, UMT_ORDER_CFG);
	/* TXIN/TXOUT/RXIN/RXOUT All Controlled by Genrisc */
	wr32(0xF, HOST_TYPE);
	/* Enable Host Gate CLK */
	wr32(0x4000, HT_GCLK_ENABLE);
	/* Host Page/MASK */
	mask = ~priv->memsize + 1;
	addr = mask | ((priv->phymem & mask) >> 16);
	wr32(addr, AHB_ARB_HP_REG);
	wr32(addr, OCP_ARB_ACC_PAGE_REG);
	/* Stop all functions first */
	wr32(0, GNRC_EN_TASK_BITMAP);

	/* Enable XBAR */
	aca_xbar_ia_reject_clr(priv, ACA_ACC_IA04);
	aca_xbar_ia_reject_clr(priv, ACA_M_IA06);

	dev_dbg(priv->dev, "aca basic config done\n");
}

static int aca_hif_param_init(struct dc_ep_priv *priv)
{
	struct dc_aca *aca  = to_aca(priv);

	aca->hif_params = kzalloc(sizeof(struct aca_hif_params), GFP_KERNEL);
	if (!aca->hif_params)
		return -ENOMEM;
	aca->hif_params->task_mask = 0x0000000F;
	dev_dbg(priv->dev, "%s\n", __func__);
	return 0;
}

static void aca_hif_param_init_done(struct dc_ep_priv *priv)
{
	u32 addr;
	struct aca_hif_params *hif_params = to_hif_params(priv);
	struct aca_fw_param *fw_param = to_aca_fw_param(priv);

	/* wr32(ACA_HIF_PARAM_ADDR, ACA_HIF_LOC_POS);*/
	/* addr = rd32(ACA_HIF_LOC_POS);*/

	addr = fw_param->init_addr;
	dev_dbg(priv->dev, "init_addr: %x\n", addr);
	memcpy_toio(priv->mem + addr, hif_params, sizeof(*hif_params));
	kzfree(hif_params);
	dev_dbg(priv->dev, "%s\n", __func__);
}

static bool aca_hif_param_init_check(struct dc_ep_priv *priv)
{
	u32 addr;
	int timeout = ACA_LOOP_CNT;
	u32 offset = offsetof(struct aca_hif_params, magic);
	struct aca_fw_param *fw_param = to_aca_fw_param(priv);

	/* addr = rd32(ACA_HIF_LOC_POS);*/
	addr = fw_param->init_addr;
	while (--timeout && (rd32(addr + offset) != ACA_MAGIC))
		udelay(1);

	if (timeout <= 0) {
		dev_err(priv->dev, "aca hif params init failed\n");
		return false;
	}

	return true;
}

static void aca_txin_init(struct dc_ep_priv *priv,
	struct aca_cfg_param *aca_txin)
{
	u32 val = 0;
	struct aca_mem_layout *mem_layout = to_mem_layout(priv);
	struct aca_hif_params *hif_params = to_hif_params(priv);
	struct aca_hif_param *txin_param = &hif_params->txin;

	if (aca_txin->byteswap)
		val = BYTE_SWAP_EN;

	val |= (aca_txin->hd_size_in_dw - 1)
		| SM((aca_txin->pd_size_in_dw - 1), PD_DESC_IN_DW);
	wr32(val, TXIN_CONV_CFG);

	/* SoC cumulative counter address */
	wr32(aca_txin->soc_cmlt_cnt_addr, GNRC_TXIN_CMLT_CNT_ADDR);


	/* SoC descriptors */
	txin_param->soc_desc_base = aca_txin->soc_desc_base;
	txin_param->soc_desc_num = aca_txin->soc_desc_num;

	/* Ping/pong buffer */
	txin_param->pp_buf_base = priv->phymem
		+ mem_layout->txin_host_desc_base;

	txin_param->pp_buf_num = mem_layout->txin_host_dnum;

	/* PD ring */
	txin_param->pd_desc_base = priv->phymem
		+ aca_txin->pd_desc_base;
	txin_param->pd_desc_num = aca_txin->pd_desc_num;

	dev_dbg(priv->dev, "aca txin init done\n");
}

static void aca_txout_init(struct dc_ep_priv *priv,
	struct aca_cfg_param *aca_txout)
{
	u32 val = 0;
	struct aca_mem_layout *mem_layout = to_mem_layout(priv);
	struct aca_hif_params *hif_params = to_hif_params(priv);
	struct aca_hif_param *txout_param = &hif_params->txout;

	if (aca_txout->byteswap)
		val = BYTE_SWAP_EN;

	val |= (aca_txout->hd_size_in_dw - 1)
		| SM((aca_txout->pd_size_in_dw - 1), PD_DESC_IN_DW);
	wr32(val, TXOUT_CONV_CFG);

	/* SoC Ring size */
	val = aca_txout->soc_desc_num;
	wr32(val, TXOUT_RING_CFG);

	/* SoC cumulative counter address */
	wr32(aca_txout->soc_cmlt_cnt_addr, GNRC_TXOUT_CMLT_CNT_ADDR);
	/* SoC descriptors */
	txout_param->soc_desc_base = aca_txout->soc_desc_base;
	txout_param->soc_desc_num = aca_txout->soc_desc_num;

	/* Ping/pong buffer */
	txout_param->pp_buf_base = priv->phymem
		+mem_layout->txout_host_desc_base;

	txout_param->pp_buf_num = mem_layout->txout_host_dnum;

	/* PD ring */
	txout_param->pd_desc_base = priv->phymem
		+ aca_txout->pd_desc_base;
	txout_param->pd_desc_num = aca_txout->pd_desc_num;

	txout_param->pd_desc_threshold = aca_txout->pp_buf_desc_num;

	dev_dbg(priv->dev, "aca txout init done\n");
}

static void aca_rxin_init(struct dc_ep_priv *priv,
	struct aca_cfg_param *aca_rxin)
{
	u32 val = 0;
	struct aca_mem_layout *mem_layout = to_mem_layout(priv);
	struct aca_hif_params *hif_params = to_hif_params(priv);
	struct aca_hif_param *rxin_param = &hif_params->rxin;

	if (aca_rxin->byteswap)
		val = BYTE_SWAP_EN;

	val |= (aca_rxin->hd_size_in_dw - 1)
		| SM((aca_rxin->pd_size_in_dw - 1), PD_DESC_IN_DW);
	wr32(val, RXIN_CONV_CFG);

	/* SoC cumulative counter address */
	wr32(aca_rxin->soc_cmlt_cnt_addr, GNRC_RXIN_CMLT_CNT_ADDR);

    /* RXIN may not be used */
	if (!(aca_rxin->soc_desc_base))
		goto __RXIN_DONE;
	/* SoC descriptors */
	rxin_param->soc_desc_base = aca_rxin->soc_desc_base;
	rxin_param->soc_desc_num = aca_rxin->soc_desc_num;

	/* Ping/pong buffer */
	rxin_param->pp_buf_base = (u32)priv->phymem
		+ mem_layout->rxin_host_desc_base;

	rxin_param->pp_buf_num = mem_layout->rxin_host_dnum;

	/* PD ring */
	rxin_param->pd_desc_base = (u32)priv->phymem
		+ aca_rxin->pd_desc_base;
	rxin_param->pd_desc_num = aca_rxin->pd_desc_num;

	rxin_param->pd_desc_threshold = aca_rxin->pp_buf_desc_num;

__RXIN_DONE:
	dev_dbg(priv->dev, "aca rxin init done\n");
}

static void aca_rxout_init(struct dc_ep_priv *priv,
	struct aca_cfg_param *aca_rxout)
{
	u32 val = 0;
	struct aca_mem_layout *mem_layout = to_mem_layout(priv);
	struct aca_hif_params *hif_params = to_hif_params(priv);
	struct aca_hif_param *rxout_param = &hif_params->rxout;

	if (aca_rxout->byteswap)
		val = BYTE_SWAP_EN;

	val |= (aca_rxout->hd_size_in_dw - 1)
		| SM((aca_rxout->pd_size_in_dw - 1), PD_DESC_IN_DW);
	wr32(val, RXOUT_CONV_CFG);

	/* SoC Ring size */
	val = aca_rxout->soc_desc_num;
	wr32(val, RXOUT_RING_CFG);

	/* SoC cumulative counter address */
	wr32(aca_rxout->soc_cmlt_cnt_addr, GNRC_RXOUT_CMLT_CNT_ADDR);
	/* SoC descriptors */
	rxout_param->soc_desc_base = aca_rxout->soc_desc_base;
	rxout_param->soc_desc_num = aca_rxout->soc_desc_num;

	/* Ping/pong buffer */
	rxout_param->pp_buf_base = (u32)priv->phymem
		+ mem_layout->rxout_host_desc_base;

	rxout_param->pp_buf_num = mem_layout->rxout_host_dnum;

	/* PD ring */
	rxout_param->pd_desc_base = (u32)priv->phymem
		+ aca_rxout->pd_desc_base;
	rxout_param->pd_desc_num = aca_rxout->pd_desc_num;

	rxout_param->pd_desc_threshold = aca_rxout->pp_buf_desc_num;
	dev_dbg(priv->dev, "aca rxout init done\n");
}

static void aca_mdm_init(struct dc_ep_priv *priv, struct aca_modem_param *mdm)
{
	struct aca_proj_param *param;

	if (!mdm)
		return;

	param = &mdm->mdm_txout;
	wr32(param->stat | priv->phymem, GNRC_TXOUT_TGT_STAT);
	wr32(param->pd | priv->phymem, GNRC_TXOUT_TGT_PD_OFF);
	wr32(param->acc_cnt | priv->phymem, GNRC_TXOUT_TGT_ACCM_CNT);

	param = &mdm->mdm_rxin;
	wr32(param->stat | priv->phymem, GNRC_RXIN_TGT_STAT);
	wr32(param->pd | priv->phymem, GNRC_RXIN_TGT_PD_OFF);
	wr32(param->acc_cnt | priv->phymem, GNRC_RXIN_TGT_ACCM_CNT);

	param = &mdm->mdm_rxout;
	wr32(param->stat | priv->phymem, GNRC_RXOUT_TGT_STAT);
	wr32(param->pd | priv->phymem, GNRC_RXOUT_TGT_PD_OFF);
	wr32(param->acc_cnt | priv->phymem, GNRC_RXOUT_TGT_ACCM_CNT);
	dev_dbg(priv->dev, "aca mdm init done\n");
}

static void dc_aca_clk_on(struct dc_ep_priv *priv)
{
	dc_ep_clk_on(priv, PMU_ADMA);
}

static void dc_aca_clk_off(struct dc_ep_priv *priv)
{
	dc_ep_clk_off(priv, PMU_ADMA);
}

static void dc_aca_reset(struct dc_ep_priv *priv)
{
	dc_ep_reset_device(priv, RST_ACA_DMA | RST_ACA_HOSTIF);
}

static void aca_mem_clear(struct dc_ep_priv *priv)
{
	struct aca_fw_dl_addr *fw_dl = to_fw_addr(priv);

	memset_io(priv->mem + fw_dl->fw_addr[0].fw_load_addr,
		0, ACA_ACC_FW_SIZE);
	memset_io(priv->mem + ACA_SRAM_BASE, 0, ACA_SRAM_SIZE);
}

int dc_aca_start(struct dc_ep_priv *priv, u32 func, int start)
{
	if (!func)
		return -EINVAL;

	wr32_mask(0, func, GNRC_EN_TASK_BITMAP);

	/* Only do if requested by caller */
	if (start) {
		wr32(0x1, GNRC_START_OP); /* Any write will trigger */
		rd32(GNRC_START_OP);
		if (!aca_hif_param_init_check(priv))
			return -EIO;
	}
	return 0;
}

static void aca_sw_reset(struct dc_ep_priv *priv)
{
	u32 val = SW_RST_GENRISC | SW_RST_HOSTIF_REG | SW_RST_RXIN
		| SW_RST_RXOUT | SW_RST_TXIN | SW_RST_TXOUT;

	wr32(val, HT_SW_RST_ASSRT);
	udelay(1);
	wr32(val, HT_SW_RST_RELEASE);
	wmb();
}

int dc_aca_stop(struct dc_ep_priv *priv, u32 *func, int reset)
{
	u32 val = *func;
	u32 reg;

	if (!val)
		return 0;

	*func = 0;

	/* Only do it if reset is required. Otherwise, pending is fine */
	if (reset) {
		if (val & ACA_TXIN_EN) {
			reg = rd32(TXIN_COUNTERS);
			if (MS(reg, ACA_PENDING_JOB)
				|| (MS(reg, ACA_AVAIL_BUF) != ACA_PP_BUFS)) {
				*func = ACA_TXIN_EN;
				return -EBUSY;
			}
		}

		if (val & ACA_TXOUT_EN) {
			reg = rd32(TXOUT_COUNTERS);
			if (MS(reg, ACA_PENDING_JOB)
				|| (MS(reg, ACA_AVAIL_BUF) != ACA_PP_BUFS)) {
				*func = ACA_TXOUT_EN;
				return -EBUSY;
			}
		}


		if (val & ACA_RXIN_EN) {
			reg = rd32(RXIN_COUNTERS);
			if (MS(reg, ACA_PENDING_JOB)
				|| (MS(reg, ACA_AVAIL_BUF) != ACA_PP_BUFS)) {
				*func = ACA_RXIN_EN;
				return -EBUSY;
			}
		}

		if (val & ACA_RXOUT_EN) {
			reg = rd32(RXOUT_COUNTERS);
			if (MS(reg, ACA_PENDING_JOB)
				|| (MS(reg, ACA_AVAIL_BUF) != ACA_PP_BUFS)) {
				*func = ACA_RXOUT_EN;
				return -EBUSY;
			}
		}
	}

	wr32_mask(val, 0, GNRC_EN_TASK_BITMAP);

	if (reset) {
		aca_dma_ch_off(priv);
		aca_xbar_ia_reject_set(priv, ACA_ACC_IA04);
		aca_xbar_ia_reject_set(priv, ACA_M_IA06);
		aca_sw_reset(priv);
	}
	return 0;
}

#ifdef CONFIG_SOC_TYPE_XWAY
static void aca_grx330_init(struct dc_ep_priv *priv)
{
	wr32(0x0044001E, TXIN_CFG1);
	wr32(0x0040041F, TXIN_CFG2);
	wr32(0x007FE020, TXIN_CFG3);

	wr32(0x0044001F, TXOUT_CFG1);
	wr32(0x0040041F, TXOUT_CFG2);
	wr32(0x007BE020, TXOUT_CFG3);

	wr32(0x0044001F, RXOUT_CFG1);
	wr32(0x0040041F, RXOUT_CFG2);
	wr32(0x007BE020, RXOUT_CFG3);

	wr32(0x0044001E, RXIN_CFG1);
	wr32(0x0040041F, RXIN_CFG2);
	wr32(0x007FE020, RXIN_CFG3);

	wr32(0x1, TXIN_DST_OWWBIT_CFG4);
	wr32(0x1, TXOUT_DST_OWWBIT_CFG4);
	wr32(0x1, RXOUT_SRC_OWNBIT_CFG3);
	wr32(0x1, RXIN_SRC_OWNBIT_CFG3);

	wr32(0x0, GNRC_TXIN_BUF_PREFILL);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x4);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x8);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0xc);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x10);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x14);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x18);
	wr32(0x0, GNRC_TXIN_BUF_PREFILL + 0x1c);
}
#endif

int dc_aca_init(struct dc_ep_priv *priv, struct aca_param *param,
	struct aca_modem_param *mdm)
{
	int ret;
	struct dc_aca *aca = to_aca(priv);

	dc_aca_clk_on(priv);
	dc_aca_reset(priv);

	ret = aca_fetch_fw(priv);
	if (ret) {
		dev_err(priv->dev,
			"could not fetch firmware files %d\n", ret);
		dc_aca_clk_off(priv);
		return ret;
	}

	aca_mem_clear(priv);
	aca_dma_init(priv);
	aca_basic_init(priv);
	aca_fw_download(priv);
	aca_hif_param_init(priv);
	aca_txin_init(priv, &param->aca_txin);
	aca_txout_init(priv, &param->aca_txout);
	aca_rxout_init(priv, &param->aca_rxout);
	aca_rxin_init(priv, &param->aca_rxin);
	aca_hif_param_init_done(priv);
	aca_mdm_init(priv, mdm);
#ifdef CONFIG_SOC_TYPE_XWAY
	aca_grx330_init(priv);
#endif
	aca->initialized = true;
	dev_info(priv->dev, "aca init done\n");
	return 0;
}

static int aca_max_gpio(struct dc_ep_priv *priv)
{
	return fls(rd32(PADC_AVAIL));
}

void dc_aca_info_init(struct dc_ep_priv *priv)
{
	struct dc_aca *aca = to_aca(priv);

	aca->initialized = false;
	spin_lock_init(&aca->clk_lock);
	spin_lock_init(&aca->rcu_lock);
	mutex_init(&aca->pin_lock);
	aca->max_gpio = aca_max_gpio(priv);
}

#define ACA_ENDIAN_ADDR(addr, endian)		\
{						\
	if (endian == ACA_BIG_ENDIAN)		\
		return addr##_BE;		\
	else					\
		return addr;			\
}

u32 aca_umt_msg_addr(struct dc_ep_priv *priv, u32 endian, u32 type)
{
	switch (type) {
	case ACA_TXIN:
		ACA_ENDIAN_ADDR(TXIN_HD_ACCUM_ADD, endian);
	case ACA_RXIN:
		ACA_ENDIAN_ADDR(RXIN_HD_ACCUM_ADD, endian);
	case ACA_TXOUT:
		ACA_ENDIAN_ADDR(TXOUT_HD_ACCUM_SUB, endian);
	case ACA_RXOUT:
		ACA_ENDIAN_ADDR(RXOUT_HD_ACCUM_SUB, endian);
	default:
		ACA_ENDIAN_ADDR(RXIN_HD_ACCUM_ADD, endian);
	};
}

void dc_aca_event_addr_get(struct dc_ep_priv *priv,
	struct aca_event_reg_addr *regs)
{
	regs->txin_acc_sub = TXIN_ACA_ACCUM_SUB;
	regs->txout_acc_add = TXOUT_ACA_ACCUM_ADD;
	regs->rxin_acc_sub = RXIN_ACA_ACCUM_SUB;
	regs->rxout_acc_add = RXOUT_ACA_ACCUM_ADD;
}

void dc_aca_txin_sub_ack(struct dc_ep_priv *priv, u32 val)
{
	wr32(val, TXIN_ACA_ACCUM_SUB);
}

u32 dc_aca_txin_hd_cnt(struct dc_ep_priv *priv)
{
	return rd32(TXIN_ACA_HD_ACC_CNT);
}

