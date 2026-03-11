// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

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

		prev_de_port = BIT(psch[i].de_port);
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

	for (i = 0; i < PPE_L0_UCAST_QUEUES; i++) {
		regmap_write(priv->regmap, PPE_QM_AC_UNI_W0(i),
			     PPE_AC_EN |
			     PPE_AC_SHARED_DYNAMIC |
			     FIELD_PREP(PPE_AC_SHARED_WEIGHT, 4) |
			     FIELD_PREP(PPE_AC_SHARED_CEILING, d->qm_ceiling));
		regmap_write(priv->regmap, PPE_QM_AC_UNI_W1(i), 0);
		regmap_write(priv->regmap, PPE_QM_AC_UNI_W2(i), 0);
		regmap_write(priv->regmap, PPE_QM_AC_UNI_W3(i),
			     FIELD_PREP(PPE_AC_GRN_RESUME_OFF, 36));
	}

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

		for (k = 0; k < 2; k++) {
			for (j = 0; j < counts[k]; j++) {
				struct l0_cfg c = {
					.queue = bases[k] + j,
					.port = p->port,
					.sp = p->sp_base + j / PPE_MAX_SP_PRI,
					.cpri = j % PPE_MAX_SP_PRI,
					.cdrr = p->cdrr_base + j,
					.epri = j % PPE_MAX_SP_PRI,
					.edrr = p->cdrr_base + j,
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

static void ppe_qos_init(struct qca_ppe_priv *priv)
{
	int i;
	u32 qos_bits;

	qos_bits = FIELD_PREP(PPE_QOS_PREHEADER_PREC, 3) |
		   FIELD_PREP(PPE_QOS_DSCP_PREC, 1) |
		   FIELD_PREP(PPE_QOS_FLOW_PREC, 4) |
		   FIELD_PREP(PPE_QOS_ACL_PREC, 2);

	for (i = 0; i < PPE_NUM_PORTS; i++)
		regmap_update_bits(priv->regmap, PPE_PRX_MRU_MTU_W1(i),
				   PPE_QOS_PCP_GRP | PPE_QOS_DSCP_GRP |
				   PPE_QOS_PREHEADER_PREC | PPE_QOS_PCP_PREC |
				   PPE_QOS_DSCP_PREC | PPE_QOS_FLOW_PREC |
				   PPE_QOS_ACL_PREC,
				   qos_bits);
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

void ppe_scheduler_init(struct qca_ppe_priv *priv)
{
	ppe_tdm_init(priv);
	ppe_bm_init(priv);
	ppe_qm_init(priv);
	ppe_l1_scheduler_init(priv);
	ppe_l0_scheduler_init(priv);
	ppe_edma_ring_map_init(priv);
	ppe_qos_init(priv);
}
