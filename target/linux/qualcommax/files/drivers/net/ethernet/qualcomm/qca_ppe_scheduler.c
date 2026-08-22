// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

#include <linux/math64.h>
#include <net/flow_offload.h>
#include <net/pkt_cls.h>

#include "qca_ppe.h"

struct psch_tdm_entry {
	u8 en_port;
	u8 de_port;
};

struct bm_tdm_entry {
	u8 port;
	u8 dir;
};

enum psch_tdm_port {
	TDM_PORT_CPU = 0,
	TDM_PORT_PHY_1,
	TDM_PORT_PHY_2,
	TDM_PORT_PHY_3,
	TDM_PORT_PHY_4,
	TDM_PORT_FAB_0,
	TDM_PORT_FAB_1,
	TDM_PORT_PHY_7,
};

enum bm_tdm_dir {
	TDM_DIR_INGRESS = 0,
	TDM_DIR_EGRESS,
};

/* CPPE (IPQ60xx) port scheduler TDM -- 50 entries */
const struct psch_tdm_entry cppe_psch_tdm[] = {
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_PHY_3, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_PHY_1, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_4 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_7 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_2 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_PHY_2, TDM_PORT_FAB_0 },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_4 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_7 },
	{ TDM_PORT_FAB_1, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_7, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_3 },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_4 },
	{ TDM_PORT_PHY_3, TDM_PORT_CPU },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_PHY_1, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_7 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_4 },
	{ TDM_PORT_FAB_1, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_2 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_2, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_7 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_4 },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_1 },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_0 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_PHY_3 },
};

/* HPPE (IPQ807x) port scheduler TDM -- 50 entries
 * Source: ssdk_hppe.c port_schedulerTDM_PORT_CPU_tbl[] */
const struct psch_tdm_entry hppe_psch_tdm[] = {
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_7 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_1 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_1, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_2 },
	{ TDM_PORT_FAB_1, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_2, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_7 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_PHY_3 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_3, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_4 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_4, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_CPU },
	{ TDM_PORT_FAB_1, TDM_PORT_PHY_7 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_7, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_1 },
	{ TDM_PORT_FAB_1, TDM_PORT_FAB_0 },
	{ TDM_PORT_PHY_1, TDM_PORT_CPU },
	{ TDM_PORT_FAB_0, TDM_PORT_FAB_1 },
	{ TDM_PORT_CPU, TDM_PORT_PHY_2 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_2, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_PHY_7 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_7, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_PHY_3 },
	{ TDM_PORT_FAB_1, TDM_PORT_CPU },
	{ TDM_PORT_PHY_3, TDM_PORT_FAB_0 },
	{ TDM_PORT_CPU, TDM_PORT_FAB_1 },
	{ TDM_PORT_FAB_0, TDM_PORT_PHY_4 },
};

/* CPPE buffer manager TDM -- 98 entries */
const struct bm_tdm_entry cppe_bm_tdm[] = {
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_2, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_3, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_1, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_2, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_3, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
};

/* HPPE buffer manager TDM -- 96 entries
 * Source: ssdk_hppe.c port_tdmTDM_PORT_CPU_tbl[] */
const struct bm_tdm_entry hppe_bm_tdm[] = {
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_3, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_2, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_4, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_3, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_2, TDM_DIR_EGRESS },
	{ TDM_PORT_CPU, TDM_DIR_INGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_0, TDM_DIR_EGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_INGRESS },
	{ TDM_PORT_CPU, TDM_DIR_EGRESS },
	{ TDM_PORT_PHY_7, TDM_DIR_INGRESS },
	{ TDM_PORT_FAB_1, TDM_DIR_EGRESS },
};

static void ppe_tdm_init(struct qca_ppe_priv *priv)
{
	const struct ppe_data *data = priv->data;
	const struct psch_tdm_entry *psch;
	const struct bm_tdm_entry *bm;
	int psch_num, bm_num;
	u8 prev_de_port;
	int i;

	psch = data->psch_tdm->entries;
	psch_num = data->psch_tdm->num;

	bm = data->bm_tdm->entries;
	bm_num = data->bm_tdm->num;

	/*
	 * The port scheduler TDM is circular with the bitmap
	 * composed of the
	 * NOT (dequeue port (de_port) OR the previous dequeue port)
	 * Each bit correspond to a port from 0 to 7.
	 *
	 * For the first element, we refer to the last dequeue port
	 * (since it's circular).
	 *
	 * Taking an example for the first element:
	 * We dequeue port 6 and the last element dequeue port 3
	 * So the bitmap will be ~(BIT(6) | BIT(3)) = 0xb7
	 *
	 * Example for the second element:
	 * We dequeque port 0 and previously we dequeued port 6
	 * So the bitmap will be ~(BIT(0) | BIT(6)) = 0xbe
	 */
	prev_de_port = psch[psch_num - 1].de_port;
	for (i = 0; i < psch_num; i++) {
		u8 bmp = ~(BIT(prev_de_port) | BIT(psch[i].de_port));

		regmap_write(priv->regmap, PPE_TM_PSCH_TDM(i),
			     FIELD_PREP(PPE_PSCH_ENS_PORT_BMP, bmp) |
			     FIELD_PREP(PPE_PSCH_ENS_PORT, psch[i].en_port) |
			     FIELD_PREP(PPE_PSCH_DES_PORT, psch[i].de_port));

		prev_de_port = psch[i].de_port;
	}

	regmap_write(priv->regmap, PPE_TM_TDM_DEPTH,
		     FIELD_PREP(PPE_TM_TDM_DEPTH_MASK, psch_num));

	for (i = 0; i < bm_num; i++)
		regmap_write(priv->regmap, PPE_PRX_TDM_CFG(i),
			     FIELD_PREP(PPE_TDM_PORT_NUM, bm[i].port) |
			     FIELD_PREP(PPE_TDM_DIR, bm[i].dir) |
			     PPE_TDM_VALID);

	regmap_write(priv->regmap, PPE_PRX_TDM_CTRL,
		     FIELD_PREP(PPE_TDM_DEPTH, bm_num) |
		     PPE_TDM_EN);
}

static void ppe_bm_init(struct qca_ppe_priv *priv)
{
	const struct ppe_data *d = priv->data;
	int i;

	for (i = 0; i < PPE_BM_PORTS; i++) {
		bool fc_en = (i < PPE_BM_PHY_START || i > d->bm_phy_end);

		regmap_write(priv->regmap, PPE_BM_FC_MODE(i),
			     fc_en ? PPE_BM_FC_EN : 0);
		regmap_write(priv->regmap, PPE_BM_GROUP_ID(i), 0);
	}

	regmap_write(priv->regmap, PPE_BM_SHARED_GRP(0),
		     FIELD_PREP(PPE_BM_SHARED_LIMIT, d->bm_group_buf));

	for (i = 0; i < PPE_BM_PORTS; i++) {
		u16 react;
		u32 w0, w1;

		if (i < PPE_BM_PHY_START)
			react = 100;
		else if (i >= d->bm_internal_start)
			react = 40;
		else
			react = 128;

		w0 = FIELD_PREP(PPE_BM_REACT_LIMIT, react) |
		     FIELD_PREP(PPE_BM_RESUME_OFF, 36) |
		     FIELD_PREP(PPE_BM_CEILING_LO, d->bm_ceiling & 0x7);
		w1 = FIELD_PREP(PPE_BM_CEILING_HI, d->bm_ceiling >> 3) |
		     FIELD_PREP(PPE_BM_WEIGHT, 4) |
		     PPE_BM_DYNAMIC;

		regmap_write(priv->regmap, PPE_BM_PORT_FC_W0(i), w0);
		regmap_write(priv->regmap, PPE_BM_PORT_FC_W1(i), w1);
	}
}

static void ppe_qm_map_set(struct qca_ppe_priv *priv, u32 index,
			    u8 queue_base, u8 profile)
{
	regmap_write(priv->regmap, PPE_QM_UCAST_MAP(index),
		     FIELD_PREP(PPE_QM_PROFILE_ID, profile) |
		     FIELD_PREP(PPE_QM_QUEUE_ID, queue_base));
}

static const u8 port_queue_base[PPE_NUM_PORTS] = {
	0, 144, 160, 176, 192, 208, 224, 240,
};

static const u8 port_l0_cdrr_num[PPE_NUM_PORTS] = {
	48, 16, 16, 16, 16, 16, 16, 16,
};

/* A unicast queue's admission control is four words that take effect on the
 * last one, so the whole entry goes back every time.
 */
static void ppe_ac_uni_write(struct qca_ppe_priv *priv, u32 queue, u32 w0)
{
	regmap_write(priv->regmap, PPE_QM_AC_UNI_W0(queue), w0);
	regmap_write(priv->regmap, PPE_QM_AC_UNI_W1(queue), 0);
	regmap_write(priv->regmap, PPE_QM_AC_UNI_W2(queue), 0);
	regmap_write(priv->regmap, PPE_QM_AC_UNI_W3(queue),
		     FIELD_PREP(PPE_AC_GRN_RESUME_OFF, 36));
}

static u32 ppe_ac_uni_default(struct qca_ppe_priv *priv)
{
	return PPE_AC_EN | PPE_AC_SHARED_DYNAMIC |
	       FIELD_PREP(PPE_AC_SHARED_WEIGHT, 4) |
	       FIELD_PREP(PPE_AC_SHARED_CEILING, priv->data->qm_ceiling);
}

static void ppe_qm_init(struct qca_ppe_priv *priv)
{
	const struct ppe_data *d = priv->data;
	int i, pri;

	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 2, 8, 0);
	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 3, 128, 8);
	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 4, 128, 8);
	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 5, 0, 0);
	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 6, 8, 0);
	ppe_qm_map_set(priv, QM_SERVICE_CODE_OFFSET + 7, 240, 0);

	for (i = 0; i < PPE_NUM_PORTS; i++)
		ppe_qm_map_set(priv, QM_VP_PORT_OFFSET + i,
				port_queue_base[i], i);

	for (i = 0; i < PPE_NUM_PORTS; i++) {
		u8 max_pri = port_l0_cdrr_num[i];
		u8 profile;

		if (max_pri > 16)
			max_pri = 1;

		for (pri = 0; pri < 16; pri++) {
			u8 cls = (pri >= max_pri) ? max_pri - 1 : pri;

			if (i == 0) {
				profile = 0;
				regmap_write(priv->regmap,
					     PPE_QM_UCAST_PRI_MAP(profile * 16 + pri),
					     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
				profile = 15;
				regmap_write(priv->regmap,
					     PPE_QM_UCAST_PRI_MAP(profile * 16 + pri),
					     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
			} else {
				regmap_write(priv->regmap,
					     PPE_QM_UCAST_PRI_MAP(i * 16 + pri),
					     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
			}
		}
	}

	for (i = 0; i < 256; i++) {
		regmap_write(priv->regmap, PPE_QM_UCAST_HASH_MAP(15 * 256 + i), 0);
		regmap_write(priv->regmap, PPE_QM_UCAST_HASH_MAP(14 * 256 + i), 0);
	}

	ppe_qm_map_set(priv, QM_CPU_CODE_OFFSET + 101,
			port_queue_base[0] + 0, 0);

	for (i = 0; i < PPE_MAX_SERVICE_CODES; i++) {
		u32 idx = QM_SERVICE_CODE_OFFSET + (1 << 8) + i;

		if (i == 2 || i == 6)
			ppe_qm_map_set(priv, idx, 8, 0);
		else if (i == 3 || i == 4)
			ppe_qm_map_set(priv, idx, 128, 8);
		else
			ppe_qm_map_set(priv, idx, 4, 0);
	}

	for (i = 0; i < PPE_MAX_CPU_CODES; i++)
		ppe_qm_map_set(priv, QM_CPU_CODE_OFFSET + (1 << 8) + i, 4, 0);

	for (i = 0; i < PPE_NUM_PORTS; i++)
		ppe_qm_map_set(priv, QM_VP_PORT_OFFSET + (1 << 8) + i,
				port_queue_base[i], i);

	for (i = PPE_NUM_PORTS; i < PPE_MAX_VPORT; i++)
		ppe_qm_map_set(priv, QM_VP_PORT_OFFSET + (1 << 8) + i, 4, 0);

	for (i = 0; i < PPE_L0_UCAST_QUEUES; i++)
		ppe_ac_uni_write(priv, i, ppe_ac_uni_default(priv));

	for (i = 0; i < PPE_L0_QUEUES - PPE_L0_UCAST_QUEUES; i++) {
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W0(i),
			     PPE_AC_MUL_EN |
			     FIELD_PREP(PPE_AC_MUL_CEILING, d->qm_ceiling) |
			     FIELD_PREP(PPE_AC_MUL_GRN_MAX_LO, d->qm_green_max & 0x1f));
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W1(i),
			     FIELD_PREP(PPE_AC_MUL_GRN_MAX_HI, d->qm_green_max >> 5));
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W2(i),
			     FIELD_PREP(PPE_AC_MUL_GRN_RESUME_HI, 36));
	}

	regmap_write(priv->regmap, PPE_QM_AC_GRP_W0(0), 0);
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W1(0),
		     FIELD_PREP(PPE_AC_GRP_LIMIT, d->qm_total_buf));
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W2(0), 0);

	regmap_update_bits(priv->regmap, PPE_EG_BRIDGE_CONFIG,
			   PPE_EG_QUEUE_CNT_EN, PPE_EG_QUEUE_CNT_EN);
}

struct l1_cfg {
	u8 index;
	u8 port;
	u8 pri;
	u8 drr;
};

static const struct l1_cfg l1_cfg[] = {
	{  0, 0, 0,  0 },
	{  1, 0, 0,  0 },
	{ 36, 1, 0,  8 },
	{ 37, 1, 1,  9 },
	{ 40, 2, 0, 12 },
	{ 41, 2, 1, 13 },
	{ 44, 3, 0, 16 },
	{ 45, 3, 1, 17 },
	{ 48, 4, 0, 20 },
	{ 49, 4, 1, 21 },
	{ 52, 5, 0, 24 },
	{ 53, 5, 1, 25 },
	{ 56, 6, 0, 28 },
	{ 57, 6, 1, 29 },
	{ 60, 7, 0, 32 },
	{ 61, 7, 1, 33 },
};

static void ppe_l1_scheduler_init(struct qca_ppe_priv *priv)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(l1_cfg); i++) {
		const struct l1_cfg *c = &l1_cfg[i];
		u32 sp_idx;

		regmap_write(priv->regmap, PPE_TM_L1_FLOW_MAP(c->index),
			     FIELD_PREP(PPE_L1_SP_ID, c->port) |
			     FIELD_PREP(PPE_L1_C_PRI, c->pri) |
			     FIELD_PREP(PPE_L1_E_PRI, c->pri) |
			     FIELD_PREP(PPE_L1_C_DRR_WT, 1) |
			     FIELD_PREP(PPE_L1_E_DRR_WT, 1));

		sp_idx = c->port * 8 + c->pri;
		regmap_write(priv->regmap, PPE_TM_L1_C_SP(sp_idx),
			     FIELD_PREP(PPE_L1_SP_DRR_ID, c->drr));

		regmap_write(priv->regmap, PPE_TM_L1_E_SP(sp_idx),
			     FIELD_PREP(PPE_L1_SP_DRR_ID, c->drr));

		regmap_write(priv->regmap, PPE_TM_L1_PORT_MAP(c->index),
			     FIELD_PREP(PPE_L1_PORT_NUM, c->port));
	}
}

struct l0_cfg {
	u16 queue;
	u8 port;
	u8 sp;
	u8 cpri;
	u8 cdrr;
	u8 epri;
	u8 edrr;
};

static const struct l0_cfg l0_port0[] = {
	{   0, 0, 0, 0, 0, 0, 0 }, {   4, 0, 0, 0, 0, 0, 0 },
	{   8, 0, 0, 0, 0, 0, 0 }, { 256, 0, 0, 0, 0, 0, 0 },
	{ 260, 0, 0, 0, 0, 0, 0 },
	{   1, 0, 0, 1, 1, 1, 1 }, {   5, 0, 0, 1, 1, 1, 1 },
	{   9, 0, 0, 1, 1, 1, 1 }, { 257, 0, 0, 1, 1, 1, 1 },
	{ 261, 0, 0, 1, 1, 1, 1 },
	{   2, 0, 0, 2, 2, 2, 2 }, {   6, 0, 0, 2, 2, 2, 2 },
	{  10, 0, 0, 2, 2, 2, 2 }, { 258, 0, 0, 2, 2, 2, 2 },
	{ 262, 0, 0, 2, 2, 2, 2 },
	{   3, 0, 0, 3, 3, 3, 3 }, {   7, 0, 0, 3, 3, 3, 3 },
	{  11, 0, 0, 3, 3, 3, 3 }, { 259, 0, 0, 3, 3, 3, 3 },
	{ 263, 0, 0, 3, 3, 3, 3 },
};

static void ppe_l0_entry_write(struct qca_ppe_priv *priv, const struct l0_cfg *c)
{
	u32 sp_idx;

	regmap_write(priv->regmap, PPE_TM_L0_FLOW_MAP(c->queue),
		     FIELD_PREP(PPE_L0_SP_ID, c->sp) |
		     FIELD_PREP(PPE_L0_C_PRI, c->cpri) |
		     FIELD_PREP(PPE_L0_E_PRI, c->epri) |
		     FIELD_PREP(PPE_L0_C_DRR_WT, 1) |
		     FIELD_PREP(PPE_L0_E_DRR_WT, 1));

	sp_idx = c->sp * 8 + c->cpri;
	regmap_write(priv->regmap, PPE_TM_L0_C_SP(sp_idx),
		     FIELD_PREP(PPE_L0_SP_DRR_ID, c->cdrr));

	sp_idx = c->sp * 8 + c->epri;
	regmap_write(priv->regmap, PPE_TM_L0_E_SP(sp_idx),
		     FIELD_PREP(PPE_L0_SP_DRR_ID, c->edrr));

	regmap_write(priv->regmap, PPE_TM_L0_PORT_MAP(c->queue),
		     FIELD_PREP(PPE_L0_PORT_NUM, c->port));
}

struct port_l0_params {
	u16 ucast_base;
	u8 ucast_count;
	u16 mcast_base;
	u8 mcast_count;
	u8 sp_base;
	u8 cdrr_base;
	u8 port;
};

static const struct port_l0_params port_l0[] = {
	{ 144, 16, 272, 4, 36,  48, 1 },
	{ 160, 16, 276, 4, 40,  64, 2 },
	{ 176, 16, 280, 4, 44,  80, 3 },
	{ 192, 16, 284, 4, 48,  96, 4 },
	{ 208, 16, 288, 4, 52, 112, 5 },
	{ 224, 16, 292, 4, 56, 128, 6 },
	{ 240, 16, 296, 1, 60, 144, 7 },
};

static void ppe_l0_scheduler_init(struct qca_ppe_priv *priv)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(l0_port0); i++)
		ppe_l0_entry_write(priv, &l0_port0[i]);

	for (i = 0; i < ARRAY_SIZE(port_l0); i++) {
		const struct port_l0_params *p = &port_l0[i];
		u16 bases[] = { p->ucast_base, p->mcast_base };
		u8 counts[] = { p->ucast_count, p->mcast_count };
		int k;

		/* Multicast queues take the port's top unicast slots, not
		 * 0..mcast_count-1: sharing a slot puts two queues on one DRR
		 * node, whose credit rotation can latch and freeze both until
		 * the node is rebuilt. The top slots idle unless skb->priority
		 * selects them, at the cost of sitting on the port's second SP,
		 * which puts flooding above best-effort unicast.
		 */
		for (k = 0; k < 2; k++) {
			for (j = 0; j < counts[k]; j++) {
				int slot = k ? p->ucast_count - counts[k] + j : j;
				struct l0_cfg c = {
					.queue = bases[k] + j,
					.port = p->port,
					.sp = p->sp_base + slot / PPE_MAX_SP_PRI,
					.cpri = slot % PPE_MAX_SP_PRI,
					.cdrr = p->cdrr_base + slot,
					.epri = slot % PPE_MAX_SP_PRI,
					.edrr = p->cdrr_base + slot,
				};

				ppe_l0_entry_write(priv, &c);
			}
		}
	}
}

static void ppe_edma_ring_map_init(struct qca_ppe_priv *priv)
{
	int i;

	regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(0), 0xf);
	for (i = 1; i < 10; i++)
		regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(0) + i * 4, 0);

	regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(3), 0xf0);
	for (i = 1; i < 10; i++)
		regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(3) + i * 4, 0);

	regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(1), 0xf00);
	for (i = 1; i < 10; i++)
		regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(1) + i * 4, 0);

	for (i = 0; i < 10; i++)
		regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(2) + i * 4, 0);
	regmap_write(priv->regmap, PPE_TM_RING_Q_MAP(2) + 4 * 4, 0xffff);
}

/* Which classifier's internal priority wins when several offer one: the flow
 * table first, then the CPU preheader, ACL, DSCP and last a VLAN's PCP.
 */
static void ppe_qos_init(struct qca_ppe_priv *priv)
{
	u32 prec;
	int i;

	prec = FIELD_PREP(PPE_QOS_FLOW_PREC, 4) |
	       FIELD_PREP(PPE_QOS_PREHEADER_PREC, 3) |
	       FIELD_PREP(PPE_QOS_ACL_PREC, 2) |
	       FIELD_PREP(PPE_QOS_DSCP_PREC, 1) |
	       FIELD_PREP(PPE_QOS_PCP_PREC, 0);

	for (i = 0; i < PPE_NUM_PORTS; i++)
		regmap_update_bits(priv->regmap, PPE_PORT_QOS_CTRL(i),
				   PPE_QOS_DSCP_PREC | PPE_QOS_PCP_PREC |
				   PPE_QOS_PREHEADER_PREC | PPE_QOS_FLOW_PREC |
				   PPE_QOS_ACL_PREC, prec);
}

const struct psch_tdm_data cppe_psch_tdm_data = {
	.entries = cppe_psch_tdm,
	.num = ARRAY_SIZE(cppe_psch_tdm),
};

const struct psch_tdm_data hppe_psch_tdm_data = {
	.entries = hppe_psch_tdm,
	.num = ARRAY_SIZE(hppe_psch_tdm),
};

const struct bm_tdm_data cppe_bm_tdm_data = {
	.entries = cppe_bm_tdm,
	.num = ARRAY_SIZE(cppe_bm_tdm),
};

const struct bm_tdm_data hppe_bm_tdm_data = {
	.entries = hppe_bm_tdm,
	.num = ARRAY_SIZE(hppe_bm_tdm),
};

/* Rate limiting. The port shaper meters what leaves a port and the port policer
 * meters what arrives on it; both are in the hardware datapath, which is the
 * whole point once a flow is offloaded and no qdisc on this box ever sees it
 * again.
 *
 * Both encode a rate the same way: a refresh count added to a token bucket once
 * every slot_time * 8 PPE clocks, in units of 1/token_unit byte, with the burst
 * as a bucket depth in units of 65536/token_unit bytes. The token unit is not a
 * choice to expose - the finest one whose two fields still hold the request is
 * the one that gets programmed, exactly as the vendor driver picks it.
 */
#define PPE_SHAPER_SLOT		8
#define PPE_POLICER_SLOT	600
#define PPE_TOKEN_UNIT_MAX	16384
#define PPE_BUCKET_UNIT		65536
/* 12 byte inter-packet gap plus the 8 byte preamble and start delimiter: what
 * the wire costs per frame that the shaper is not otherwise shown.
 */
#define PPE_IPG_PREAMBLE_LEN	20

static int ppe_token_bucket(unsigned long clk, u32 slot, u64 rate_bps,
			    u32 burst, u32 cir_max, u32 cbs_max,
			    u32 *cir, u32 *cbs)
{
	int sel;

	/* The refresh count is a u64 product, and a rate large enough to wrap
	 * it comes back out as a small one the loop would accept. Such a rate
	 * is out of range at every unit, so refuse it here rather than let it
	 * arrive as a plausible answer.
	 */
	if (rate_bps > div64_ul(U64_MAX, PPE_TOKEN_UNIT_MAX * (u64)slot))
		return -ERANGE;

	for (sel = 0; sel < 8; sel++) {
		u32 unit = PPE_TOKEN_UNIT_MAX >> (2 * sel);
		u64 c = div64_ul(rate_bps * unit * slot, clk);
		u64 b = mul_u32_u32(burst, unit) / PPE_BUCKET_UNIT;

		if (c > cir_max || b > cbs_max)
			continue;
		if (!c || !b)
			return -ERANGE;

		*cir = c;
		*cbs = b;
		return sel;
	}

	return -ERANGE;
}

/* A shaped port is the bottleneck by construction, so the standing queue lives
 * on its queues. Left as every queue is configured at probe - a dynamic limit
 * clamped at the SoC's ceiling, whose overrun asserts flow control rather than
 * dropping - the queue settles wherever the dynamic limit lands and the ceiling
 * has no effect at all: measured on IPQ8074 it holds around 250 buffers at any
 * ceiling from 400 down to 48. Enforcing the limit by dropping is what makes it
 * bind, and a limit worth binding at is one millisecond of the rate the shaper
 * was just given.
 */
/* What one full frame costs on the wire for this port, which is what a token
 * bucket has to be able to hold and what a queue's floor is counted in.
 */
static u32 ppe_port_frame_len(struct qca_ppe_priv *priv, int port)
{
	struct net_device *dev = dsa_to_port(&priv->ds, port)->user;

	return (dev ? dev->mtu : ETH_DATA_LEN) + ETH_HLEN + ETH_FCS_LEN;
}

#define PPE_AC_TARGET_US	1000
/* A queue that is the bottleneck cannot be shorter than a handful of full
 * frames and still keep a single flow at the rate it was shaped to: measured
 * on IPQ8074 under a 20 Mbit/s ceiling, two frames' worth of buffers costs a
 * third of the rate and eight frames' worth costs nothing. A frame is whatever
 * the port carries, so a jumbo port gets a jumbo floor.
 */
#define PPE_AC_MIN_FRAMES	8

static void ppe_port_queue_limit_set(struct qca_ppe_priv *priv, int port)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];
	const struct port_l0_params *p = NULL;
	u32 w0;
	int i;

	for (i = 0; i < ARRAY_SIZE(port_l0); i++)
		if (port_l0[i].port == port)
			p = &port_l0[i];
	if (!p)
		return;

	if (sh->rate_bps) {
		u32 bufs, min_bufs;

		min_bufs = DIV_ROUND_UP(ppe_port_frame_len(priv, port) *
					PPE_AC_MIN_FRAMES, PPE_BM_BUF_SIZE);

		if (sh->limit)
			bufs = sh->limit / PPE_BM_BUF_SIZE;
		else
			bufs = div64_u64(sh->rate_bps * PPE_AC_TARGET_US,
					 BITS_PER_BYTE * PPE_BM_BUF_SIZE *
					 (u64)USEC_PER_SEC);

		bufs = clamp_t(u32, bufs, min_bufs, priv->data->qm_ceiling);
		w0 = PPE_AC_EN | PPE_AC_FORCE_AC_EN |
		     FIELD_PREP(PPE_AC_SHARED_WEIGHT, 4) |
		     FIELD_PREP(PPE_AC_SHARED_CEILING, bufs);
	} else {
		w0 = ppe_ac_uni_default(priv);
	}

	for (i = 0; i < p->ucast_count; i++)
		ppe_ac_uni_write(priv, p->ucast_base + i, w0);
}

/* A rate of zero takes the shaper down. Its credit goes with it: a bucket left
 * negative from the old rate would hold the port off for as long as it takes
 * the new one to refill it.
 */
static int ppe_port_shaper_set(struct qca_ppe_priv *priv, int port,
			       u64 rate_bps, u32 burst)
{
	u32 cir = 0, cbs = 0;
	unsigned long clk;
	int sel = 0;

	if (rate_bps) {
		clk = ppe_clk_rate(priv);
		if (!clk)
			return -ENODEV;

		sel = ppe_token_bucket(clk, PPE_SHAPER_SLOT, rate_bps, burst,
				       FIELD_MAX(PPE_PSCH_SHP_CIR),
				       FIELD_MAX(PPE_PSCH_SHP_CBS),
				       &cir, &cbs);
		if (sel < 0) {
			dev_err(priv->ds.dev,
				"port %d: %llu bit/s burst %u bytes is outside the shaper's range\n",
				port, rate_bps, burst);
			return sel;
		}
	}

	regmap_write(priv->regmap, PPE_TM_PSCH_SHP_CFG_W0(port),
		     FIELD_PREP(PPE_PSCH_SHP_CIR, cir) |
		     FIELD_PREP(PPE_PSCH_SHP_CBS, cbs));
	regmap_write(priv->regmap, PPE_TM_PSCH_SHP_CFG_W1(port),
		     FIELD_PREP(PPE_PSCH_SHP_TOKEN_UNIT, sel) |
		     (rate_bps ? PPE_PSCH_SHP_EN : 0));

	if (!rate_bps) {
		regmap_write(priv->regmap, PPE_TM_PSCH_SHP_CREDIT(port), 0);
		regmap_write(priv->regmap, PPE_TM_PSCH_SHP_SIGN(port), 0);
	}

	priv->shaper[port].rate_bps = rate_bps;
	if (!rate_bps)
		priv->shaper[port].limit = 0;
	ppe_port_queue_limit_set(priv, port);

	return 0;
}

/* Metering mode 1 is RFC 2697: one rate, one bucket, and everything the
 * committed burst cannot hold is red. Leaving the excess burst at zero is what
 * makes red mean "over the rate" rather than "over the excess rate", and red is
 * dropped by the reset value of the violate command.
 */
static int ppe_port_policer_set(struct qca_ppe_priv *priv, int port,
				u64 rate_bps, u32 burst)
{
	u32 cir = 0, cbs = 0;
	unsigned long clk;
	int sel = 0;

	if (rate_bps) {
		clk = ppe_clk_rate(priv);
		if (!clk)
			return -ENODEV;

		sel = ppe_token_bucket(clk, PPE_POLICER_SLOT, rate_bps, burst,
				       FIELD_MAX(PPE_METER_CIR_HI) << 3 |
				       FIELD_MAX(PPE_METER_CIR_LO),
				       FIELD_MAX(PPE_METER_CBS),
				       &cir, &cbs);
		if (sel < 0) {
			dev_err(priv->ds.dev,
				"port %d: %llu bit/s burst %u bytes is outside the policer's range\n",
				port, rate_bps, burst);
			return sel;
		}
	}

	/* The entry spans four words and takes effect on the last one, so it is
	 * written in address order like every other multi-word PPE table.
	 */
	regmap_write(priv->regmap, PPE_PORT_METER_W0(port),
		     (rate_bps ? PPE_METER_EN : 0) |
		     PPE_METER_MODE |
		     FIELD_PREP(PPE_METER_FRAME_TYPE,
				FIELD_MAX(PPE_METER_FRAME_TYPE)) |
		     FIELD_PREP(PPE_METER_TOKEN_UNIT, sel) |
		     FIELD_PREP(PPE_METER_CBS, cbs) |
		     FIELD_PREP(PPE_METER_CIR_LO, cir));
	regmap_write(priv->regmap, PPE_PORT_METER_W1(port),
		     FIELD_PREP(PPE_METER_CIR_HI, cir >> 3));
	regmap_write(priv->regmap, PPE_PORT_METER_W2(port), 0);
	regmap_write(priv->regmap, PPE_PORT_METER_W3(port), 0);

	return 0;
}

int qca_ppe_port_policer_add(struct dsa_switch *ds, int port,
			     const struct flow_action_police *policer,
			     struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	/* One rate, one burst, drop on red is all this meter has. Everything
	 * else the police action can carry is refused rather than dropped on
	 * the floor: a filter that asked for a packet rate and got a byte
	 * meter, or asked for pass on exceed and got drop, would report
	 * offloaded and police something other than what it says.
	 */
	if (!policer->rate_bytes_ps ||
	    policer->peakrate_bytes_ps || policer->rate_pkt_ps ||
	    policer->burst_pkt || policer->avrate ||
	    policer->exceed.act_id != FLOW_ACTION_DROP ||
	    (policer->notexceed.act_id != FLOW_ACTION_PIPE &&
	     policer->notexceed.act_id != FLOW_ACTION_ACCEPT)) {
		NL_SET_ERR_MSG_MOD(extack, "the meter is one byte rate, one burst and drop on red");
		return -EOPNOTSUPP;
	}

	/* The meter counts the frame the port puts on the wire, so a link
	 * layer the filter wants accounted on top of it goes in the port's
	 * compensation length, which already carries the checksum.
	 */
	if (ETH_FCS_LEN + policer->overhead > FIELD_MAX(PPE_CMPST_LENGTH)) {
		NL_SET_ERR_MSG_MOD(extack, "larger than the port's compensation length field");
		return -EOPNOTSUPP;
	}

	regmap_write(priv->regmap, PPE_POLICER_CMPST_LEN(port),
		     FIELD_PREP(PPE_CMPST_LENGTH,
				ETH_FCS_LEN + policer->overhead));

	return ppe_port_policer_set(priv, port,
				    policer->rate_bytes_ps * BITS_PER_BYTE,
				    policer->burst);
}

void qca_ppe_port_policer_del(struct dsa_switch *ds, int port)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	regmap_write(priv->regmap, PPE_POLICER_CMPST_LEN(port),
		     FIELD_PREP(PPE_CMPST_LENGTH, ETH_FCS_LEN));
	ppe_port_policer_set(priv, port, 0, 0);
}

/* The counters a shaped port can answer with are the MAC's own transmit MIB and
 * the port's egress drop counter, which is where the queue limit above lands.
 * They are free-running, so what tc is handed is the delta since it last asked.
 */
static void ppe_port_tx_counters(struct qca_ppe_priv *priv, int port,
				 u64 *bytes, u32 *pkts, u32 *drops)
{
	regmap_read(priv->regmap, PPE_PORT_TX_DROP_CNT(port), drops);

	*bytes = ppe_mib_read(priv, port, PPE_MIB_TXBYTE_L);
	*pkts = ppe_mib_read(priv, port, PPE_MIB_TXUNI) +
		ppe_mib_read(priv, port, PPE_MIB_TXBROAD) +
		ppe_mib_read(priv, port, PPE_MIB_TXMULTI);
}

static void ppe_port_shaper_stats(struct qca_ppe_priv *priv, int port,
				  struct tc_qopt_offload_stats *stats)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];
	u64 bytes;
	u32 pkts, drops;

	ppe_port_tx_counters(priv, port, &bytes, &pkts, &drops);

	/* The packet and drop counters are 32 bits wide and wrap, so their
	 * deltas are taken in their own width.
	 */
	_bstats_update(stats->bstats, bytes - sh->base_bytes,
		       pkts - sh->base_pkts);
	stats->qstats->drops += (u32)(drops - sh->base_drops);

	sh->base_bytes = bytes;
	sh->base_pkts = pkts;
	sh->base_drops = drops;
}

/* The hardware has one shaper per port and nowhere to hang a class off it, so
 * only a root tbf is a rate this switch can keep. The qdisc's overhead, mpu and
 * linklayer are not carried into the hardware, which meters the frame it puts
 * on the wire including preamble, inter-packet gap and CRC.
 */
int qca_ppe_setup_tc_tbf(struct qca_ppe_priv *priv, int port,
			 struct tc_tbf_qopt_offload *qopt)
{
	int ret;

	if (qopt->parent != TC_H_ROOT)
		return -EOPNOTSUPP;

	switch (qopt->command) {
	case TC_TBF_REPLACE: {
		/* The qdisc's own limit is the depth it wants behind the
		 * shaper, and the queue limit is taken from it, so it is in
		 * place before the rate is programmed and put back if the
		 * rate is refused.
		 */
		u32 limit = priv->shaper[port].limit;

		priv->shaper[port].limit = qopt->replace_params.limit;
		ret = ppe_port_shaper_set(priv, port,
					  qopt->replace_params.rate.rate_bytes_ps *
					  BITS_PER_BYTE,
					  qopt->replace_params.max_size);
		if (ret) {
			priv->shaper[port].limit = limit;
			return ret;
		}

		priv->shaper[port].tbf_handle = qopt->handle;
		ppe_port_tx_counters(priv, port,
				     &priv->shaper[port].base_bytes,
				     &priv->shaper[port].base_pkts,
				     &priv->shaper[port].base_drops);
		return 0;
	}
	case TC_TBF_DESTROY:
		/* A replacement's destroy arrives after the new qdisc has
		 * already programmed the shaper, so only the qdisc that owns
		 * the rate may take it down.
		 */
		if (qopt->handle != priv->shaper[port].tbf_handle)
			return 0;

		priv->shaper[port].tbf_handle = 0;
		return ppe_port_shaper_set(priv, port, 0, 0);
	case TC_TBF_STATS:
		if (!priv->shaper[port].tbf_handle ||
		    qopt->handle != priv->shaper[port].tbf_handle)
			return -EOPNOTSUPP;
		ppe_port_shaper_stats(priv, port, &qopt->stats);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

/* Both token buckets refresh on a period the hardware keeps in its own
 * register, and the policer's comes up at zero, which is no period at all. The
 * preamble and gap the shaper never sees are added back from a third register,
 * shared by every shaper in the block.
 */
static void ppe_rate_limit_init(struct qca_ppe_priv *priv)
{
	regmap_write(priv->regmap, PPE_TM_SHP_SLOT_PORT,
		     FIELD_PREP(PPE_PORT_SHP_SLOT_TIME, PPE_SHAPER_SLOT));
	regmap_write(priv->regmap, PPE_TM_IPG_PRE_LEN,
		     FIELD_PREP(PPE_IPG_PRE_LEN, PPE_IPG_PREAMBLE_LEN));
	regmap_write(priv->regmap, PPE_POLICER_TIME_SLOT,
		     FIELD_PREP(PPE_POLICER_SLOT_TIME, PPE_POLICER_SLOT));
}

void ppe_scheduler_init(struct qca_ppe_priv *priv)
{
	ppe_tdm_init(priv);
	ppe_bm_init(priv);
	ppe_qm_init(priv);
	ppe_l1_scheduler_init(priv);
	ppe_l0_scheduler_init(priv);
	ppe_edma_ring_map_init(priv);
	ppe_qos_init(priv);
	ppe_rate_limit_init(priv);
}
