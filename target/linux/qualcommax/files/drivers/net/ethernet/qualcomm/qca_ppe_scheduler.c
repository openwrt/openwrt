// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

#include <linux/math64.h>
#include <net/dcbnl.h>
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
		for (pri = 0; pri < 16; pri++) {
			/* Three bands per port. On a user port the hash offset
			 * is added to the class for every packet, so a class
			 * must be as wide as the spread or the smear crosses
			 * into the next, and the third band is the second
			 * scheduler node's queues; the CPU port is given no
			 * spread, so its three are its first three queues.
			 * Either way l0_port0[] and ppe_l0_scheduler_init()
			 * have already put each band a strict priority above
			 * the one below, which is what the small-frame
			 * classifier in the ACL needs to be worth anything:
			 * without it every priority shares one queue, and a
			 * frame on its way to a Wi-Fi client waits behind
			 * whatever bulk that queue is holding.
			 *
			 * Priorities eight and up select the third band. No
			 * classifier resolves there on its own - the DSCP and
			 * PCP tables map below eight unless told otherwise -
			 * so the band carries exactly the traffic a rule names
			 * into it, and a shaper on it then governs that
			 * traffic and nothing else: the node's on a user port,
			 * the queue's own on the CPU port (cpu_port_rate).
			 */
			u8 cls = min(pri / PPE_FLOW_SPREAD_QUEUES, 2) *
				 (i ? PPE_FLOW_SPREAD_QUEUES : 1);

			if (i) {
				regmap_write(priv->regmap,
					     PPE_QM_UCAST_PRI_MAP(i * 16 + pri),
					     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
				continue;
			}

			/* Profiles 0 and 15 both resolve to the CPU port. */
			regmap_write(priv->regmap,
				     PPE_QM_UCAST_PRI_MAP(pri),
				     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
			regmap_write(priv->regmap,
				     PPE_QM_UCAST_PRI_MAP(15 * 16 + pri),
				     FIELD_PREP(PPE_QM_PRI_CLASS, cls));
		}
	}

	/* The 5-tuple hash picks the queue inside a band on every user port;
	 * profile id equals port id. Profiles 14/15 (CPU code, point offload)
	 * stay collapsed to offset 0 below.
	 */
	for (i = 1; i < PPE_NUM_PORTS; i++) {
		int h;

		for (h = 0; h < 256; h++)
			regmap_write(priv->regmap,
				     PPE_QM_UCAST_HASH_MAP(i * 256 + h),
				     h % PPE_FLOW_SPREAD_QUEUES);
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

	/* A multicast queue's limit is a static threshold, not the unicast
	 * queues' dynamic share, so its ceiling field carries the green
	 * threshold and the colour gaps under it are unused.
	 */
	for (i = 0; i < PPE_L0_QUEUES - PPE_L0_UCAST_QUEUES; i++) {
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W0(i),
			     PPE_AC_MUL_EN |
			     FIELD_PREP(PPE_AC_MUL_CEILING, d->qm_green_max));
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W1(i), 0);
		regmap_write(priv->regmap, PPE_QM_AC_MUL_W2(i),
			     FIELD_PREP(PPE_AC_MUL_GRN_RESUME_OFF, 36));
	}

	regmap_write(priv->regmap, PPE_QM_AC_GRP_W0(0), 0);
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W1(0),
		     FIELD_PREP(PPE_AC_GRP_LIMIT, d->qm_total_buf));
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W2(0), 0);

	regmap_update_bits(priv->regmap, PPE_EG_BRIDGE_CONFIG,
			   PPE_EG_QUEUE_CNT_EN, PPE_EG_QUEUE_CNT_EN);
}

/* The two buffer accountings a frame passes. On ingress the buffer manager
 * admits it against one of four shared groups, on egress the queue manager
 * holds it against one of four admission control groups, and both count in
 * PPE_BM_BUF_SIZE buffers. Those eight groups are the devlink pools, ingress
 * first: their limits are what the two inits above write once at probe, and
 * until now neither could be read back, let alone moved, without a rebuild.
 *
 * Nothing else in devlink's shared buffer model fits this hardware. A per-port
 * threshold would have to name the BM port a switch port sits behind, and the
 * vendor says two contradictory things about that - its own init treats BM
 * ports 8 to 13 as the physical ones, while its per-port counter API indexes
 * the very same tables with the switch port number. Occupancy is reported by
 * devlink as a current and a maximum together and this hardware keeps no
 * watermark, so the live counts the debugfs `bm` and `queues` files already
 * read are the current half and there is no honest second half.
 */
int qca_ppe_devlink_sb_setup(struct dsa_switch *ds)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	/* The queue manager's group limit is what the vendor calls the total
	 * buffer number, and no source states the buffer memory any other way.
	 */
	return devlink_sb_register(ds->devlink, PPE_DEVLINK_SB,
				   priv->data->qm_total_buf * PPE_BM_BUF_SIZE,
				   PPE_BM_SHARED_GROUPS,
				   FIELD_MAX(PPE_AC_GRP_ID) + 1, 0, 0);
}

int qca_ppe_devlink_sb_pool_get(struct dsa_switch *ds, unsigned int sb_index,
				u16 pool_index,
				struct devlink_sb_pool_info *pool_info)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 val;

	if (pool_index < PPE_BM_SHARED_GROUPS) {
		regmap_read(priv->regmap, PPE_BM_SHARED_GRP(pool_index), &val);
		val = FIELD_GET(PPE_BM_SHARED_LIMIT, val);
		pool_info->pool_type = DEVLINK_SB_POOL_TYPE_INGRESS;
	} else {
		regmap_read(priv->regmap,
			    PPE_QM_AC_GRP_W1(pool_index -
					     PPE_BM_SHARED_GROUPS), &val);
		val = FIELD_GET(PPE_AC_GRP_LIMIT, val);
		pool_info->pool_type = DEVLINK_SB_POOL_TYPE_EGRESS;
	}

	pool_info->size = val * PPE_BM_BUF_SIZE;
	pool_info->cell_size = PPE_BM_BUF_SIZE;
	/* What both blocks give a member of a group at probe. The queue
	 * manager swaps a shaped port's queues to a ceiling of their own,
	 * which is a per queue property this per pool field cannot carry.
	 */
	pool_info->threshold_type = DEVLINK_SB_THRESHOLD_TYPE_DYNAMIC;

	return 0;
}

int qca_ppe_devlink_sb_pool_set(struct dsa_switch *ds, unsigned int sb_index,
				u16 pool_index, u32 size,
				enum devlink_sb_threshold_type threshold_type,
				struct netlink_ext_ack *extack)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 bufs = DIV_ROUND_UP(size, PPE_BM_BUF_SIZE);
	u32 w[PPE_AC_GRP_WORDS];

	if (threshold_type != DEVLINK_SB_THRESHOLD_TYPE_DYNAMIC) {
		NL_SET_ERR_MSG_MOD(extack, "a group is shared dynamically or not at all");
		return -EOPNOTSUPP;
	}

	if (pool_index < PPE_BM_SHARED_GROUPS) {
		if (bufs > FIELD_MAX(PPE_BM_SHARED_LIMIT)) {
			NL_SET_ERR_MSG_MOD(extack, "larger than the buffer manager's group limit field");
			return -EINVAL;
		}

		regmap_write(priv->regmap, PPE_BM_SHARED_GRP(pool_index),
			     FIELD_PREP(PPE_BM_SHARED_LIMIT, bufs));

		return 0;
	}

	pool_index -= PPE_BM_SHARED_GROUPS;

	if (bufs > FIELD_MAX(PPE_AC_GRP_LIMIT)) {
		NL_SET_ERR_MSG_MOD(extack, "larger than the queue manager's group limit field");
		return -EINVAL;
	}

	/* The group entry takes effect on the write to its last word, so the
	 * whole entry goes back with only its limit changed.
	 */
	if (regmap_bulk_read(priv->regmap, PPE_QM_AC_GRP_W0(pool_index), w,
			     ARRAY_SIZE(w)))
		return -EIO;

	w[1] &= ~PPE_AC_GRP_LIMIT;
	w[1] |= FIELD_PREP(PPE_AC_GRP_LIMIT, bufs);

	regmap_write(priv->regmap, PPE_QM_AC_GRP_W0(pool_index), w[0]);
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W1(pool_index), w[1]);
	regmap_write(priv->regmap, PPE_QM_AC_GRP_W2(pool_index), w[2]);

	return 0;
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
				int pri = slot % PPE_MAX_SP_PRI;
				struct l0_cfg c;

				/* The three bands: queues of one band share one
				 * (SP, priority) and therefore one DRR list -
				 * the same layout the CPU port's RSS spread
				 * uses - draining round-robin at equal weight,
				 * so a hash bucket is served at no less than
				 * its share of the port. Band two sits a
				 * priority above band one; band three is the
				 * second node's, under the mcast slots that
				 * share that node.
				 */
				if (!k && slot < 3 * PPE_FLOW_SPREAD_QUEUES)
					pri = slot / PPE_FLOW_SPREAD_QUEUES == 1 ?
					      PPE_FLOW_SPREAD_QUEUES : 0;

				c = (struct l0_cfg) {
					.queue = bases[k] + j,
					.port = p->port,
					.sp = p->sp_base + slot / PPE_MAX_SP_PRI,
					.cpri = pri,
					.cdrr = p->cdrr_base + slot,
					.epri = pri,
					.edrr = p->cdrr_base + slot,
				};

				ppe_l0_entry_write(priv, &c);
			}
		}

		/* Which multicast queue the port floods a frame onto is the
		 * frame's internal priority, clamped to the queues the port
		 * has: left at reset every priority resolves to the first.
		 */
		for (j = 0; j < 16; j++)
			regmap_write(priv->regmap,
				     PPE_QM_MCAST_PRI_MAP(p->port, j),
				     min_t(u8, j, p->mcast_count - 1));
	}
}

static void ppe_queues_gate(struct qca_ppe_priv *priv, u16 base, u8 count,
			    bool en)
{
	int i;

	for (i = 0; i < count; i++) {
		regmap_write(priv->regmap, PPE_QM_ENQ_OPR(base + i),
			     en ? 0 : PPE_ENQ_DISABLE);
		regmap_write(priv->regmap, PPE_TM_DEQ_DIS(base + i),
			     en ? 0 : PPE_DEQ_DIS);
	}
}

/* The flush is refused unless the queue's enqueue is already stopped; the
 * vendor stops its dequeue with it.
 */
static void ppe_queue_flush(struct qca_ppe_priv *priv, int port, u16 queue)
{
	u32 val;
	int ret;

	regmap_update_bits(priv->regmap, PPE_QM_FLUSH_CFG,
			   PPE_FLUSH_QID | PPE_FLUSH_DST_PORT |
			   PPE_FLUSH_ALL_QUEUES | PPE_FLUSH_BUSY,
			   FIELD_PREP(PPE_FLUSH_QID, queue) |
			   FIELD_PREP(PPE_FLUSH_DST_PORT, port) |
			   PPE_FLUSH_BUSY);

	ret = regmap_read_poll_timeout(priv->regmap, PPE_QM_FLUSH_CFG, val,
				       !(val & PPE_FLUSH_BUSY), 10, 10000);
	if (ret || !(val & PPE_FLUSH_STATUS))
		dev_warn(priv->ds.dev, "port %d: queue %u did not flush\n",
			 port, queue);
}

/* What a port that goes down leaves behind: the frames already queued for it
 * stay charged to its admission group, and with the fabric gated nothing
 * dequeues them. Stop each of the port's queues at both ends and flush it, and
 * the buffers come back at once; port_enable is what reopens the gates.
 */
void ppe_port_queues_enable(struct qca_ppe_priv *priv, int port, bool en)
{
	const struct port_l0_params *p = NULL;
	int i;

	for (i = 0; i < ARRAY_SIZE(port_l0); i++)
		if (port_l0[i].port == port)
			p = &port_l0[i];
	if (!p)
		return;

	ppe_queues_gate(priv, p->ucast_base, p->ucast_count, en);
	ppe_queues_gate(priv, p->mcast_base, p->mcast_count, en);

	if (en)
		return;

	/* The vendor drops the QM clock gate around its own flush to
	 * accelerate it, and restores it after.
	 */
	regmap_clear_bits(priv->regmap, PPE_CLK_GATING_CTRL,
			  PPE_QM_CLK_GATE_EN);

	for (i = 0; i < p->ucast_count; i++)
		ppe_queue_flush(priv, port, p->ucast_base + i);
	for (i = 0; i < p->mcast_count; i++)
		ppe_queue_flush(priv, port, p->mcast_base + i);

	regmap_set_bits(priv->regmap, PPE_CLK_GATING_CTRL, PPE_QM_CLK_GATE_EN);
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
/* Which classifier decides a packet's priority is a precedence order per port,
 * and DCB's apptrust is that order in the other direction: the selectors a user
 * lists, most trusted first. Only the two this hardware can be told about are
 * offered - PCP and DSCP - and the classifiers a user cannot name keep the
 * ranking the driver gave them at probe.
 */
static const u8 ppe_apptrust_sel[] = { DCB_APP_SEL_PCP, IEEE_8021QAZ_APP_SEL_DSCP };

#define PPE_QOS_GROUP		0

static int ppe_apptrust_prec(struct qca_ppe_priv *priv, int port, u8 sel)
{
	u32 val, mask;

	regmap_read(priv->regmap, PPE_PORT_QOS_CTRL(port), &val);
	mask = sel == IEEE_8021QAZ_APP_SEL_DSCP ? PPE_QOS_DSCP_PREC :
						  PPE_QOS_PCP_PREC;

	return field_get(mask, val);
}

int qca_ppe_port_get_apptrust(struct dsa_switch *ds, int port, u8 *sel,
			      int *nsel)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	int dscp, pcp;

	dscp = ppe_apptrust_prec(priv, port, IEEE_8021QAZ_APP_SEL_DSCP);
	pcp = ppe_apptrust_prec(priv, port, DCB_APP_SEL_PCP);

	*nsel = 2;
	sel[0] = dscp > pcp ? IEEE_8021QAZ_APP_SEL_DSCP : DCB_APP_SEL_PCP;
	sel[1] = dscp > pcp ? DCB_APP_SEL_PCP : IEEE_8021QAZ_APP_SEL_DSCP;

	return 0;
}

int qca_ppe_port_set_apptrust(struct dsa_switch *ds, int port, const u8 *sel,
			      int nsel)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 dscp = 0, pcp = 0;
	int i, j;

	/* A precedence of zero is a rank and not an off switch: measured, a port
	 * whose DSCP precedence is zero classifies by DSCP exactly as it does at
	 * one. This generation can order the two and cannot take either out of
	 * the running, so a list that leaves one out is refused rather than
	 * answered with a port that still trusts it.
	 */
	if (nsel != ARRAY_SIZE(ppe_apptrust_sel)) {
		dev_err(priv->ds.dev,
			"port %d: both selectors have to be listed; this hardware ranks them and cannot untrust one\n",
			port);
		return -EOPNOTSUPP;
	}

	/* Most trusted first. The two keep the band the other classifiers leave
	 * them - ACL, preheader and flow sit above both - so ordering them is a
	 * choice between one and zero rather than a rank that could collide.
	 */
	for (i = 0; i < nsel; i++) {
		for (j = 0; j < ARRAY_SIZE(ppe_apptrust_sel); j++)
			if (sel[i] == ppe_apptrust_sel[j])
				break;
		if (j == ARRAY_SIZE(ppe_apptrust_sel))
			return -EOPNOTSUPP;

		if (sel[i] == IEEE_8021QAZ_APP_SEL_DSCP)
			dscp = i ? 0 : 1;
		else
			pcp = i ? 0 : 1;
	}

	regmap_update_bits(priv->regmap, PPE_PORT_QOS_CTRL(port),
			   PPE_QOS_DSCP_PREC | PPE_QOS_PCP_PREC,
			   FIELD_PREP(PPE_QOS_DSCP_PREC, dscp) |
			   FIELD_PREP(PPE_QOS_PCP_PREC, pcp));

	return 0;
}

/* One DSCP table serves every port - the hardware has two of them and selects
 * between them per port, which is not the per-port mapping DCB describes, so
 * the driver keeps every port on the same one and tells DSA the mapping is
 * global.
 */
int qca_ppe_port_get_dscp_prio(struct dsa_switch *ds, int port, u8 dscp)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	u32 val;

	regmap_read(priv->regmap, PPE_DSCP_QOS_GROUP(PPE_QOS_GROUP, dscp),
		    &val);

	return FIELD_GET(PPE_QOS_INFO_PRI, val);
}

int qca_ppe_port_add_dscp_prio(struct dsa_switch *ds, int port, u8 dscp,
			       u8 prio)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	if (prio > PPE_QOS_MAX_PRI)
		return -ERANGE;

	regmap_update_bits(priv->regmap,
			   PPE_DSCP_QOS_GROUP(PPE_QOS_GROUP, dscp),
			   PPE_QOS_INFO_PRI,
			   FIELD_PREP(PPE_QOS_INFO_PRI, prio));

	return 0;
}

/* `dcb app replace` adds the new entry before deleting the old one, so a delete
 * naming a priority the table no longer holds is that ordering and not a
 * request to undo anything.
 */
int qca_ppe_port_del_dscp_prio(struct dsa_switch *ds, int port, u8 dscp,
			       u8 prio)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);

	if (qca_ppe_port_get_dscp_prio(ds, port, dscp) != prio)
		return 0;

	regmap_update_bits(priv->regmap,
			   PPE_DSCP_QOS_GROUP(PPE_QOS_GROUP, dscp),
			   PPE_QOS_INFO_PRI, 0);

	return 0;
}

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

	/* Trusting PCP is only meaningful if the map behind it says something:
	 * it resets to zero, which would resolve every tagged frame to priority
	 * 0 and, because the classifier outranks DSCP once trusted, stop DSCP
	 * deciding as well. The table is indexed by PCP over both DEI halves.
	 */
	for (i = 0; i < PPE_PCP_QOS_ENTRIES; i++)
		regmap_write(priv->regmap, PPE_PCP_QOS_GROUP(0, i),
			     FIELD_PREP(PPE_QOS_INFO_PRI, i & 7));

	/* A flow entry names a profile, not a priority, so give the profiles
	 * the identity mapping and let the entry carry the number itself.
	 * Profile 0 is left at priority 0: it is what an entry given no
	 * priority holds, and leaving it there is what lets DSCP still decide
	 * those.
	 */
	for (i = 1; i <= PPE_QOS_MAX_PRI; i++)
		regmap_write(priv->regmap, PPE_FLOW_QOS_GROUP(0, i),
			     FIELD_PREP(PPE_QOS_INFO_PRI, i));
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
#define PPE_TOKEN_UNIT_MAX	16384
#define PPE_BUCKET_UNIT		65536
/* 12 byte inter-packet gap plus the 8 byte preamble and start delimiter: what
 * the wire costs per frame that the shaper is not otherwise shown.
 */
#define PPE_IPG_PREAMBLE_LEN	20

int ppe_token_bucket(unsigned long clk, u32 slot, u64 rate_bps, u32 burst,
		     u32 cir_max, u32 cbs_max, u32 *cir, u32 *cbs)
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

/* The CPU port's third band: the queue priorities eight and up select there
 * (ppe_qm_init()), and the one cpu_port_rate shapes.
 */
#define PPE_CPU_PORT_DL_QUEUE	2

static u32 ppe_ac_uni_static(struct qca_ppe_priv *priv, int port, u64 rate_bps,
			     u32 limit, u32 ceiling)
{
	u32 bufs, min_bufs;

	min_bufs = DIV_ROUND_UP(ppe_port_frame_len(priv, port) *
				PPE_AC_MIN_FRAMES, PPE_BM_BUF_SIZE);

	if (limit)
		bufs = limit / PPE_BM_BUF_SIZE;
	else
		bufs = div64_u64(rate_bps * PPE_AC_TARGET_US,
				 BITS_PER_BYTE * PPE_BM_BUF_SIZE *
				 (u64)USEC_PER_SEC);

	bufs = clamp_t(u32, bufs, min_bufs, ceiling);

	return PPE_AC_EN | PPE_AC_FORCE_AC_EN |
	       FIELD_PREP(PPE_AC_SHARED_WEIGHT, 4) |
	       FIELD_PREP(PPE_AC_SHARED_CEILING, bufs);
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
static void ppe_port_queue_limit_set(struct qca_ppe_priv *priv, int port)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];
	const struct port_l0_params *p = NULL;
	u32 w0;
	int i;

	/* A user port's download band is four hash queues, each at the
	 * ceiling; the CPU port's is one queue, so it takes the four's worth.
	 */
	w0 = sh->rate_bps ? ppe_ac_uni_static(priv, port, sh->rate_bps,
					      sh->limit,
					      port == QCA_PPE_CPU_PORT ?
					      PPE_FLOW_SPREAD_QUEUES *
					      priv->data->qm_ceiling :
					      priv->data->qm_ceiling) :
			    ppe_ac_uni_default(priv);

	/* The CPU port owns every queue below the first user port's base, and it
	 * is not in the table below because the driver does not build its
	 * scheduler. Its admission entries are still ours to size: the queue
	 * cpu_port_rate shapes is a bottleneck the way a shaped port is, and
	 * without this it keeps the dynamic limit every queue is probed with -
	 * which the comment above this function explains does not bind.
	 */
	if (port == QCA_PPE_CPU_PORT) {
		u32 def = ppe_ac_uni_default(priv);

		for (i = 0; i < port_l0[0].ucast_base; i++)
			ppe_ac_uni_write(priv, i,
					 i == PPE_CPU_PORT_DL_QUEUE ? w0 : def);
		return;
	}

	for (i = 0; i < ARRAY_SIZE(port_l0); i++)
		if (port_l0[i].port == port)
			p = &port_l0[i];
	if (!p)
		return;

	for (i = 0; i < p->ucast_count; i++) {
		u64 rate = i < ARRAY_SIZE(sh->queue_rate) ? sh->queue_rate[i] : 0;
		u32 w = w0;

		/* A queue given a ceiling of its own is a bottleneck the same
		 * way a shaped port is, and a tighter one, so the standing
		 * queue forms there and is sized from that rate instead: ten
		 * milliseconds of it, as the port the host is behind is
		 * given, because a millisecond is too shallow to hold one
		 * flow at the rate.
		 */
		if (rate)
			w = ppe_ac_uni_static(priv, port, rate,
					      div_u64(rate * 10,
						      BITS_PER_BYTE * 1000),
					      priv->data->qm_ceiling);
		else if (sh->rate_bps && i < 3 * PPE_FLOW_SPREAD_QUEUES)
			/* A band bucket holds a share of the flows and drains
			 * at no less than its share of the port, so it takes
			 * a share of the depth: what one bucket's flows wait
			 * behind stays bounded whatever the other buckets do.
			 */
			w = ppe_ac_uni_static(priv, port, sh->rate_bps,
					      sh->limit /
					      PPE_FLOW_SPREAD_QUEUES,
					      priv->data->qm_ceiling);

		ppe_ac_uni_write(priv, p->ucast_base + i, w);
	}
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

	/* Credit outlives the rate that filled it: what a re-armed meter would
	 * spend on its first frames was bought at a rate the port has lost.
	 */
	if (!rate_bps) {
		regmap_write(priv->regmap, PPE_PORT_METER_CRDT(port), 0);
		regmap_write(priv->regmap, PPE_PORT_METER_CRDT(port) + 0x4, 0);
	}

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

/* The port scheduler serves the port's queues strictly, so a class that never
 * runs dry starves every class below it, and the only thing on this silicon
 * that can bound one is a shaper on the scheduler node the class hangs off.
 * mqprio is where a class names a rate, and it names the queues the class
 * spans as well, which is the choice the hardware offers: a class of one queue
 * is that queue's own L0 node, and a class spanning every queue an L1 node
 * serves is that node. Both at once is a tree, which is what htb offload is
 * for, and nothing here has asked for one.
 */
#define PPE_L0_SHAPER_SLOT	300
#define PPE_L1_SHAPER_SLOT	64

/* A node offers its parent two inputs, committed and excess, and arming the
 * committed bucket alone is not a rate: the excess input carries whatever the
 * committed bucket refuses, unmetered, at the same priority, so the node passes
 * line rate with its committed credit pegged negative. A single rate is both
 * buckets - the rate in the committed one and the excess one armed empty.
 */
static int ppe_node_shaper_set(struct qca_ppe_priv *priv, u32 cfg, u32 credit,
			       u32 slot, u64 rate_bps, u32 burst)
{
	u32 cir = 0, cbs = 0;
	unsigned long clk;
	int sel = 0;

	if (rate_bps) {
		clk = ppe_clk_rate(priv);
		if (!clk)
			return -ENODEV;

		sel = ppe_token_bucket(clk, slot, rate_bps, burst,
				       FIELD_MAX(PPE_SHP_CIR),
				       FIELD_MAX(PPE_SHP_CBS), &cir, &cbs);
		if (sel < 0)
			return sel;
	}

	regmap_write(priv->regmap, cfg,
		     FIELD_PREP(PPE_SHP_CIR, cir) |
		     FIELD_PREP(PPE_SHP_CBS, cbs));
	regmap_write(priv->regmap, cfg + 0x4, 0);
	regmap_write(priv->regmap, cfg + 0x8,
		     FIELD_PREP(PPE_SHP_TOKEN_UNIT, sel) |
		     (rate_bps ? PPE_SHP_C_EN | PPE_SHP_E_EN : 0));

	/* Credit outlives the rate that filled it, and a bucket left negative
	 * holds the node off until the new rate has refilled it.
	 */
	regmap_write(priv->regmap, credit, 0);
	regmap_write(priv->regmap, credit + 0x4, 0);

	return 0;
}

static int ppe_l1_node(int port, u8 group)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(l1_cfg); i++)
		if (l1_cfg[i].port == port && l1_cfg[i].pri == group)
			return l1_cfg[i].index;

	return -ENOENT;
}

static void ppe_port_shapers_clear(struct qca_ppe_priv *priv,
				   const struct port_l0_params *p)
{
	struct ppe_port_shaper *sh = &priv->shaper[p->port];
	int i, node;

	for (i = 0; i < p->ucast_count; i++)
		ppe_node_shaper_set(priv, PPE_TM_L0_SHP_CFG(p->ucast_base + i),
				    PPE_TM_L0_SHP_CREDIT(p->ucast_base + i),
				    PPE_L0_SHAPER_SLOT, 0, 0);

	for (i = 0; (node = ppe_l1_node(p->port, i)) >= 0; i++)
		ppe_node_shaper_set(priv, PPE_TM_L1_SHP_CFG(node),
				    PPE_TM_L1_SHP_CREDIT(node),
				    PPE_L1_SHAPER_SLOT, 0, 0);

	memset(sh->queue_rate, 0, sizeof(sh->queue_rate));
	ppe_port_queue_limit_set(priv, p->port);
}

/* Where a class's rate goes: one queue's own node, or the node a whole group of
 * them hangs off. Nothing is programmed from here - the caller wants every
 * class checked before any of them is committed.
 */
static int ppe_class_node(int port, u16 base, u16 offset, u16 count,
			  struct ppe_class_shaper *out)
{
	u8 group, span;
	int node;

	if (count == 1) {
		out->cfg = PPE_TM_L0_SHP_CFG(base + offset);
		out->credit = PPE_TM_L0_SHP_CREDIT(base + offset);
		out->slot = PPE_L0_SHAPER_SLOT;
		return 0;
	}

	/* The queues an L1 node serves are decided at probe: PPE_MAX_SP_PRI of
	 * them, less what the priority ceiling takes off the last node. A class
	 * that spans part of a node has no shaper of its own to be given.
	 */
	group = offset / PPE_MAX_SP_PRI;
	span = min_t(u8, PPE_MAX_SP_PRI,
		     PPE_QOS_MAX_PRI + 1 - group * PPE_MAX_SP_PRI);
	node = ppe_l1_node(port, group);

	if (node < 0 || offset % PPE_MAX_SP_PRI || count != span)
		return -EINVAL;

	out->cfg = PPE_TM_L1_SHP_CFG(node);
	out->credit = PPE_TM_L1_SHP_CREDIT(node);
	out->slot = PPE_L1_SHAPER_SLOT;

	return 0;
}

int qca_ppe_setup_tc_mqprio(struct qca_ppe_priv *priv, int port,
			    struct tc_mqprio_qopt_offload *qopt)
{
	struct ppe_class_shaper prog[PPE_QOS_MAX_PRI + 1] = { };
	const struct tc_mqprio_qopt *q = &qopt->qopt;
	struct net_device *dev = dsa_to_port(&priv->ds, port)->user;
	const struct port_l0_params *p = NULL;
	unsigned long clk;
	int i, tc;

	for (i = 0; i < ARRAY_SIZE(port_l0); i++)
		if (port_l0[i].port == port)
			p = &port_l0[i];
	if (!p)
		return -EOPNOTSUPP;

	/* Nothing below touches the hardware: a request that turns out to be
	 * unbuildable half way through would otherwise leave the port with its
	 * shapers already cleared under a qdisc the qdisc layer keeps.
	 */
	if (q->num_tc > PPE_QOS_MAX_PRI + 1) {
		dev_err(priv->ds.dev,
			"port %d: %u classes, the port has %d priorities to give\n",
			port, q->num_tc, PPE_QOS_MAX_PRI + 1);
		return -EINVAL;
	}

	clk = ppe_clk_rate(priv);
	if (q->num_tc && !clk)
		return -ENODEV;

	/* Which queue a packet leaves on is the internal priority a classifier
	 * gave it, never the class the transmit queue it came from belongs to,
	 * so a map that disagrees with the classes' queue ranges describes
	 * something this hardware will not do.
	 */
	for (i = 0; q->num_tc && i <= PPE_QOS_MAX_PRI; i++) {
		tc = q->prio_tc_map[i];

		if (tc >= q->num_tc || i < q->offset[tc] ||
		    i >= q->offset[tc] + q->count[tc]) {
			dev_err(priv->ds.dev,
				"port %d: priority %d is mapped to class %d, which does not own queue %d\n",
				port, i, tc, i);
			return -EINVAL;
		}
	}

	for (tc = 0; tc < q->num_tc; tc++) {
		u16 offset = q->offset[tc], count = q->count[tc];
		u32 cir, cbs;

		if (qopt->min_rate[tc]) {
			dev_err(priv->ds.dev,
				"port %d: class %d asks for a floor; the scheduler is strict, it can only be given a ceiling\n",
				port, tc);
			return -EOPNOTSUPP;
		}

		if (ppe_class_node(port, p->ucast_base, offset, count,
				   &prog[tc])) {
			dev_err(priv->ds.dev,
				"port %d: class %d spans queues %u..%u, which is not a scheduler node; a class is one queue or all of a node's\n",
				port, tc, offset, offset + count - 1);
			return -EINVAL;
		}

		prog[tc].rate_bps = qopt->max_rate[tc] * BITS_PER_BYTE;
		if (!prog[tc].rate_bps)
			continue;

		if (ppe_token_bucket(clk, prog[tc].slot, prog[tc].rate_bps,
				     ppe_port_frame_len(priv, port),
				     FIELD_MAX(PPE_SHP_CIR),
				     FIELD_MAX(PPE_SHP_CBS), &cir, &cbs) < 0) {
			dev_err(priv->ds.dev,
				"port %d: class %d rate is outside the shaper's range\n",
				port, tc);
			return -ERANGE;
		}
	}

	ppe_port_shapers_clear(priv, p);
	netdev_reset_tc(dev);

	if (!q->num_tc)
		return 0;

	for (tc = 0; tc < q->num_tc; tc++) {
		if (!prog[tc].rate_bps)
			continue;

		/* A bucket that cannot hold one full frame stalls the node, and
		 * a port drains its queues as fast as they fill, so a frame is
		 * all the burst a class needs.
		 */
		ppe_node_shaper_set(priv, prog[tc].cfg, prog[tc].credit,
				    prog[tc].slot, prog[tc].rate_bps,
				    ppe_port_frame_len(priv, port));

		for (i = q->offset[tc]; i < q->offset[tc] + q->count[tc]; i++)
			priv->shaper[port].queue_rate[i] = prog[tc].rate_bps;
	}

	ppe_port_queue_limit_set(priv, port);

	/* Asking for the offload hands the driver the transmit queue mapping
	 * as well, which the qdisc otherwise sets itself.
	 */
	netdev_set_num_tc(dev, q->num_tc);
	for (tc = 0; tc < q->num_tc; tc++)
		netdev_set_tc_queue(dev, tc, q->count[tc], q->offset[tc]);

	return 0;
}

/* mqprio leaves it to the driver to check that the classes name a sane set of
 * queues unless the driver says otherwise; there is nothing here the core does
 * not check better, so ask it to.
 */
int qca_ppe_tc_query_caps(struct tc_query_caps_base *base)
{
	struct tc_mqprio_caps *caps = base->caps;

	if (base->type != TC_SETUP_QDISC_MQPRIO)
		return -EOPNOTSUPP;

	caps->validate_queue_counts = true;

	return 0;
}

/* The port's unicast scheduler is three strict bands built at probe and not
 * reconfigurable: priorities eight and up at the top, the classified
 * priorities under them, best-effort below both, and the queues inside a band
 * a hash spread served round robin rather than a ladder of their own. ETS and
 * prio are how that shape is expressed to tc, which numbers bands from the
 * top, so the top band is band 0. A shape this hardware cannot give is refused
 * rather than quietly flattened into one it can; nothing is programmed either
 * way, an accepted qdisc only gains the port's counters.
 */
#define PPE_QOS_BANDS		3

static int ppe_qos_bands_set(struct qca_ppe_priv *priv, int port, u32 handle,
			     unsigned int bands, const u8 *priomap)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];
	unsigned int i;

	if (bands != PPE_QOS_BANDS) {
		dev_err(priv->ds.dev,
			"port %d: %u bands, this scheduler is fixed at %d\n",
			port, bands, PPE_QOS_BANDS);
		return -EINVAL;
	}

	/* The priomap the qdisc fills in when the user gave none puts every
	 * priority on the last band, which is an absence of an opinion rather
	 * than a different one.
	 */
	for (i = 0; i < TC_PRIO_MAX + 1; i++)
		if (priomap[i] != bands - 1)
			break;

	if (i < TC_PRIO_MAX + 1) {
		for (i = 0; i < TC_PRIO_MAX + 1; i++) {
			/* The band split ppe_qm_init gives a user port. */
			u8 band = PPE_QOS_BANDS - 1 -
				  min_t(unsigned int,
					i / PPE_FLOW_SPREAD_QUEUES,
					PPE_QOS_BANDS - 1);

			if (priomap[i] == band)
				continue;

			dev_err(priv->ds.dev,
				"port %d: priority %u is mapped to band %u; this scheduler is fixed and gives it band %u\n",
				port, i, priomap[i], band);
			return -EOPNOTSUPP;
		}
	}

	sh->bands_handle = handle;
	ppe_port_tx_counters(priv, port, &sh->base_bytes, &sh->base_pkts,
			     &sh->base_drops);

	return 0;
}

int qca_ppe_setup_tc_ets(struct qca_ppe_priv *priv, int port,
			 struct tc_ets_qopt_offload *qopt)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];
	unsigned int i;

	if (qopt->parent != TC_H_ROOT)
		return -EOPNOTSUPP;

	switch (qopt->command) {
	case TC_ETS_REPLACE:
		/* sch_ets gives every band a quantum whether the user asked
		 * for one or not, so a non-zero quantum is not a request. Bands
		 * that differ are: refuse those, because the bands are strict
		 * and the only round robin under them is between a band's hash
		 * buckets, which no priority selects.
		 */
		for (i = 1; i < qopt->replace_params.bands; i++) {
			if (qopt->replace_params.quanta[i] ==
			    qopt->replace_params.quanta[0])
				continue;

			dev_err(priv->ds.dev,
				"port %d: band %u asks for a weight of its own; these bands are strict\n",
				port, i);
			return -EOPNOTSUPP;
		}

		return ppe_qos_bands_set(priv, port, qopt->handle,
					 qopt->replace_params.bands,
					 qopt->replace_params.priomap);
	case TC_ETS_DESTROY:
		if (qopt->handle == sh->bands_handle)
			sh->bands_handle = 0;
		return 0;
	case TC_ETS_STATS:
		if (!sh->bands_handle || qopt->handle != sh->bands_handle)
			return -EOPNOTSUPP;
		ppe_port_shaper_stats(priv, port, &qopt->stats);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

int qca_ppe_setup_tc_prio(struct qca_ppe_priv *priv, int port,
			  struct tc_prio_qopt_offload *qopt)
{
	struct ppe_port_shaper *sh = &priv->shaper[port];

	if (qopt->parent != TC_H_ROOT)
		return -EOPNOTSUPP;

	switch (qopt->command) {
	case TC_PRIO_REPLACE:
		return ppe_qos_bands_set(priv, port, qopt->handle,
					 qopt->replace_params.bands,
					 qopt->replace_params.priomap);
	case TC_PRIO_DESTROY:
		if (qopt->handle == sh->bands_handle)
			sh->bands_handle = 0;
		return 0;
	case TC_PRIO_STATS:
		if (!sh->bands_handle || qopt->handle != sh->bands_handle)
			return -EOPNOTSUPP;
		ppe_port_shaper_stats(priv, port, &qopt->stats);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
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
 * shared by every shaper in the block; the policer has a compensation of its
 * own, per port, which the vendor sets to the frame checksum alone.
 */
static void ppe_rate_limit_init(struct qca_ppe_priv *priv)
{
	int i;

	regmap_write(priv->regmap, PPE_TM_SHP_SLOT_PORT,
		     FIELD_PREP(PPE_PORT_SHP_SLOT_TIME, PPE_SHAPER_SLOT));
	regmap_write(priv->regmap, PPE_TM_SHP_SLOT_L0,
		     FIELD_PREP(PPE_SHP_SLOT_TIME, PPE_L0_SHAPER_SLOT));
	regmap_write(priv->regmap, PPE_TM_SHP_SLOT_L1,
		     FIELD_PREP(PPE_SHP_SLOT_TIME, PPE_L1_SHAPER_SLOT));
	regmap_write(priv->regmap, PPE_TM_IPG_PRE_LEN,
		     FIELD_PREP(PPE_IPG_PRE_LEN, PPE_IPG_PREAMBLE_LEN));
	regmap_write(priv->regmap, PPE_POLICER_TIME_SLOT,
		     FIELD_PREP(PPE_POLICER_SLOT_TIME, PPE_POLICER_SLOT));

	/* A frame already marked for drop is never forwarded, so metering it
	 * would spend the port's tokens on bandwidth nothing receives.
	 */
	regmap_write(priv->regmap, PPE_POLICER_DROP_BYPASS, PPE_DROP_BYPASS_EN);

	for (i = 0; i < priv->data->num_ports; i++)
		regmap_write(priv->regmap, PPE_POLICER_CMPST_LEN(i),
			     FIELD_PREP(PPE_CMPST_LENGTH, ETH_FCS_LEN));
}

/* The 5-tuple RSS hash every unicast packet carries into queue selection.
 * Out of reset the mask, seed and mix stages are zero and the hash is
 * degenerate - every flow one bucket - so the spread depends on this. The
 * seed is fixed rather than random so a flow's bucket survives a reboot.
 */
static void ppe_rss_hash_init(struct qca_ppe_priv *priv)
{
	static const u32 mix[5] = { 0x13, 0xb, 0x13, 0xb, 0x13 };
	static const u32 fin[5] = { 0x205, 0x264, 0x227, 0x245, 0x201 };
	int i;

	regmap_write(priv->regmap, PPE_RSS_HASH_MASK, 0xfff);
	regmap_write(priv->regmap, PPE_RSS_HASH_SEED, 0x5eedc0de);
	/* v6 mix: sip[0..3], dip[0..3], proto, dport, sport - one weight per
	 * field, alternating in that order.
	 */
	for (i = 0; i < 11; i++)
		regmap_write(priv->regmap, PPE_RSS_HASH_MIX(i), mix[i % 2]);
	for (i = 0; i < 5; i++)
		regmap_write(priv->regmap, PPE_RSS_HASH_FIN(i), fin[i]);

	regmap_write(priv->regmap, PPE_RSS_HASH_MASK_IPV4, 0xfff);
	regmap_write(priv->regmap, PPE_RSS_HASH_SEED_IPV4, 0x5eedc0de);
	for (i = 0; i < 5; i++)
		regmap_write(priv->regmap, PPE_RSS_HASH_MIX_IPV4(i), mix[i]);
	for (i = 0; i < 5; i++)
		regmap_write(priv->regmap, PPE_RSS_HASH_FIN_IPV4(i), fin[i]);
}

/* The port every packet on its way to the host leaves by, and the only one with
 * no netdev of its own: DSA gives a qdisc to the user ports and nothing to this
 * one, so neither a tbf nor an mqprio can name it. A frame bound for a Wi-Fi
 * client leaves here, which makes this the only queue in the switch that can
 * hold that traffic - a meter on the arriving port is the alternative, and a
 * meter drops where this delays.
 *
 * What is shaped is the port's third band, the queue priorities eight and up
 * select (ppe_qm_init()): the traffic a rule marks into it, which is a routed
 * download when the rule is on the uplink, and nothing else. A bridged
 * transfer to a Wi-Fi client crosses this port unmarked and is not held.
 */
static struct qca_ppe_priv *ppe_sched_priv;
static uint ppe_cpu_port_rate;

static int ppe_cpu_port_dl_set(struct qca_ppe_priv *priv, u64 rate_bps)
{
	struct ppe_port_shaper *sh = &priv->shaper[QCA_PPE_CPU_PORT];
	u32 depth = div_u64(rate_bps * 10, BITS_PER_BYTE * 1000);
	int ret;

	/* A user port's queue depth comes from its tbf limit. Nothing can put a
	 * tbf on this port, so the depth is the driver's to choose, and it is
	 * chosen the same way: ten milliseconds of the rate, which
	 * ppe_port_queue_limit_set() then clamps to the four queues' worth of
	 * admission a user port's download band gets. One millisecond - what a
	 * shaped port falls back to with no limit - is too shallow to hold a
	 * flow at the rate it was shaped to.
	 *
	 * The burst is the same ten milliseconds, and unlike a user port's
	 * class it cannot be a frame: this port drains only as fast as the
	 * host takes frames off the ring, and a bucket of one frame forgets
	 * every token that accrues between two of the host's visits.
	 */
	ret = ppe_node_shaper_set(priv, PPE_TM_L0_SHP_CFG(PPE_CPU_PORT_DL_QUEUE),
				  PPE_TM_L0_SHP_CREDIT(PPE_CPU_PORT_DL_QUEUE),
				  PPE_L0_SHAPER_SLOT, rate_bps, depth);
	if (ret)
		return ret;

	sh->rate_bps = rate_bps;
	sh->limit = depth;
	ppe_port_queue_limit_set(priv, QCA_PPE_CPU_PORT);

	return 0;
}

static int ppe_cpu_port_rate_apply(struct qca_ppe_priv *priv)
{
	return ppe_cpu_port_dl_set(priv, (u64)ppe_cpu_port_rate * 1000);
}

static int ppe_cpu_port_rate_set(const char *val,
				 const struct kernel_param *kp)
{
	int ret = param_set_uint(val, kp);

	if (ret || !ppe_sched_priv)
		return ret;

	return ppe_cpu_port_rate_apply(ppe_sched_priv);
}

static const struct kernel_param_ops ppe_cpu_port_rate_ops = {
	.set = ppe_cpu_port_rate_set,
	.get = param_get_uint,
};

module_param_cb(cpu_port_rate, &ppe_cpu_port_rate_ops, &ppe_cpu_port_rate,
		0644);
MODULE_PARM_DESC(cpu_port_rate,
		 "Shape the download band of the port the host is behind, in kbit/s (0 disables)");

/* The parameter writer reaches this driver through the global above and ends in
 * dsa_to_port(), so the global has to be gone before the switch is
 * unregistered, not merely before the teardown below. The parameter lock is
 * held across a whole set, so clearing it under that lock lets a writer already
 * inside finish first - the same ordering ppe_acl_exit() needs for its own
 * global.
 */
void ppe_scheduler_unready(void)
{
	kernel_param_lock(THIS_MODULE);
	ppe_sched_priv = NULL;
	kernel_param_unlock(THIS_MODULE);
}

/* The shaper goes with the driver, so the switch is left the way it was found. */
void ppe_scheduler_exit(struct qca_ppe_priv *priv)
{
	ppe_cpu_port_dl_set(priv, 0);
}

void ppe_scheduler_init(struct qca_ppe_priv *priv)
{
	ppe_tdm_init(priv);
	ppe_bm_init(priv);
	ppe_qm_init(priv);
	ppe_rss_hash_init(priv);
	ppe_l1_scheduler_init(priv);
	ppe_l0_scheduler_init(priv);
	ppe_edma_ring_map_init(priv);
	ppe_qos_init(priv);
	ppe_rate_limit_init(priv);
}

/* The parameter setter reaches the hardware through the global above, and its
 * path ends in dsa_to_port(), so the global cannot be published before the
 * switch is registered. A rate handed in at load time is applied here instead,
 * the way ppe_acl_init() applies its own.
 */
void ppe_scheduler_ready(struct qca_ppe_priv *priv)
{
	kernel_param_lock(THIS_MODULE);
	ppe_sched_priv = priv;
	if (ppe_cpu_port_rate)
		ppe_cpu_port_rate_apply(priv);
	kernel_param_unlock(THIS_MODULE);
}
