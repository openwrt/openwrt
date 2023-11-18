/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#include "dle.h"
#include "mac_priv.h"

/* PCIE 64 */
static struct dle_size_t wde_size0 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	4095, /* lnk_pge_num */
	1, /* unlnk_pge_num */
};

/* SDIO, PCIE STF, USB */
static struct dle_size_t wde_size1 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	768, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* PCIE 128 */
static struct dle_size_t wde_size2 = {
	MAC_AX_WDE_PG_128, /* pge_size */
	2016, /* lnk_pge_num */
	32, /* unlnk_pge_num */
};

/* PCIE SU TP */
static struct dle_size_t wde_size3 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	496, /* lnk_pge_num */
	3600, /* unlnk_pge_num */
};

/* DLFW */
static struct dle_size_t wde_size4 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	0, /* lnk_pge_num */
	4096, /* unlnk_pge_num */
};

/* PCIE BCN TEST */
static struct dle_size_t wde_size5 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	3904, /* lnk_pge_num */
	64, /* unlnk_pge_num */
};

/* PCIE 64 */
static struct dle_size_t wde_size6 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	512, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* PCIE 128 */
static struct dle_size_t wde_size7 = {
	MAC_AX_WDE_PG_128, /* pge_size */
	960, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* PCIE STF, USB */
static struct dle_size_t wde_size8 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	256, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* DLFW */
static struct dle_size_t wde_size9 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	0, /* lnk_pge_num */
	1024, /* unlnk_pge_num */
};

/* LA-PCIE */
static struct dle_size_t wde_size10 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	1408, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* LA-PCIE */
static struct dle_size_t wde_size11 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	256, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* LA-SDIO */
static struct dle_size_t wde_size12 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	328, /* lnk_pge_num */
	56, /* unlnk_pge_num */
};

/* SDIO SCC */
static struct dle_size_t wde_size13 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	128, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* SDIO LA */
static struct dle_size_t wde_size14 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	124, /* lnk_pge_num */
	4, /* unlnk_pge_num */
};

/* LA-USB 8852A*/
static struct dle_size_t wde_size15 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	384, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* LA-USB 8852B*/
static struct dle_size_t wde_size16 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	124, /* lnk_pge_num */
	4, /* unlnk_pge_num */
};

/* 8852C USB3.0 */
static struct dle_size_t wde_size17 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	354, /* lnk_pge_num */
	30, /* unlnk_pge_num */
};

/* 8852C DLFW */
static struct dle_size_t wde_size18 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	0, /* lnk_pge_num */
	2048, /* unlnk_pge_num */
};

/* 8852C PCIE SCC */
static struct dle_size_t wde_size19 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	3328, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* 8852C PCIE DBCC */
static struct dle_size_t wde_size20 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	3328, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* 8852C PCIE SCC/DBCC STF */
static struct dle_size_t wde_size21 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	256, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* 8852C PCIE LA */
static struct dle_size_t wde_size22 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	3224, /* lnk_pge_num */
	104, /* unlnk_pge_num */
};

/* PCIE SCC TURBO */
static struct dle_size_t wde_size23 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	1024, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* 8852B USB CABV*/
static struct dle_size_t wde_size24 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	216, /* lnk_pge_num */
	40, /* unlnk_pge_num */
};

/* 8852B USB2.0/USB3.0 SCC */
static struct dle_size_t wde_size25 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	162, /* lnk_pge_num */
	94, /* unlnk_pge_num */
};

/* 8852AU ccv*/
static struct dle_size_t wde_size26 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	760, /* lnk_pge_num */
	8, /* unlnk_pge_num */
};

/* 8852C DBCC USB */
static struct dle_size_t wde_size27 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	374, /* lnk_pge_num */
	10, /* unlnk_pge_num */
};

/* 1115E PCIE DBCC & DLFW */
static struct dle_size_t wde_size28 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	0, /* lnk_pge_num */
	3328, /* unlnk_pge_num */
};

/* 8852B USB2.0 SCC */
static struct dle_size_t wde_size30 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	220, /* lnk_pge_num */
	36, /* unlnk_pge_num */
};

/* 8852C USB2.0 */
static struct dle_size_t wde_size31 = {
	MAC_AX_WDE_PG_64, /* pge_size */
	384, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* PCIE */
static struct dle_size_t ple_size0 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1520, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* SDIO, USB */
static struct dle_size_t ple_size1 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3184, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* PCIE STF */
static struct dle_size_t ple_size2 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3184, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* PCIE SU TP */
static struct dle_size_t ple_size3 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	330, /* lnk_pge_num */
	1206, /* unlnk_pge_num */
};

/* DLFW */
static struct dle_size_t ple_size4 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	64, /* lnk_pge_num */
	1472, /* unlnk_pge_num */
};

/* PCIE BCN TEST */
static struct dle_size_t ple_size5 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1520, /* lnk_pge_num */
	80, /* unlnk_pge_num */
};

/* PCIE 64 */
static struct dle_size_t ple_size6 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	496, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* PCIE STF, USB */
static struct dle_size_t ple_size7 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1392, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* DLFW */
static struct dle_size_t ple_size8 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	64, /* lnk_pge_num */
	960, /* unlnk_pge_num */
};

/* PCIE 128 */
static struct dle_size_t ple_size9 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	576, /* lnk_pge_num */
	0, /* unlnk_pge_num */
};

/* LA-PCIE 8852A*/
static struct dle_size_t ple_size10 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	816, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* LA-PCIE */
static struct dle_size_t ple_size11 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	368, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* LA-SDIO 8852A*/
static struct dle_size_t ple_size12 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1328, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* SDIO SCC */
static struct dle_size_t ple_size13 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	688, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* SDIO LA */
static struct dle_size_t ple_size14 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	432, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* LA-USB 8852A */
static struct dle_size_t ple_size15 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1328, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* LA-USB 8852B */
static struct dle_size_t ple_size16 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	424, /* lnk_pge_num */
	24, /* unlnk_pge_num */
};

/* 8852C USB */
static struct dle_size_t ple_size17 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3368, /* lnk_pge_num */
	24, /* unlnk_pge_num */
};

/* 8852C DLFW*/
static struct dle_size_t ple_size18 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	2544, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

#ifdef PHL_FEATURE_AP
/* 8852C PCIE SCC PLE 256 */
static struct dle_size_t ple_size19 = {
	MAC_AX_PLE_PG_256, /* pge_size */
	952, /* lnk_pge_num */
	8, /* unlnk_pge_num */
};
#else
/* 8852C PCIE SCC */
static struct dle_size_t ple_size19 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1904, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};
#endif

/* 8852C PCIE DBCC */
static struct dle_size_t ple_size20 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1904, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 8852C PCIE SCC/DBCC STF */
static struct dle_size_t ple_size21 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3440, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 8852C PCIE LA */
static struct dle_size_t ple_size22 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1904, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 8852B PCIE TURBO AB */
static struct dle_size_t ple_size23 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	496, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* PCIE SCC TURBO */
static struct dle_size_t ple_size24 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1008, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 8852B USB2.0 TURBO*/
static struct dle_size_t ple_size27 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1396, /* lnk_pge_num */
	12, /* unlnk_pge_num */
};

/* 8852C DBCC USB2.0 */
static struct dle_size_t ple_size28 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3347, /* lnk_pge_num */
	45, /* unlnk_pge_num */
};

/* SDIO SCC TURBO */
static struct dle_size_t ple_size29 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1456, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 1115E PCIE DBCC & DLFW */
static struct dle_size_t ple_size30 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	2672, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* 8852B USB3.0 TURBO*/
static struct dle_size_t ple_size31 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	1386, /* lnk_pge_num */
	22, /* unlnk_pge_num */
};

/* 8852B USB2.0 SCC*/
static struct dle_size_t ple_size32 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	620, /* lnk_pge_num */
	20, /* unlnk_pge_num */
};

/* 8852B USB3.0 SCC*/
static struct dle_size_t ple_size33 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	632, /* lnk_pge_num */
	8, /* unlnk_pge_num */
};

/* 8852C  USB2.0 */
static struct dle_size_t ple_size34 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3374, /* lnk_pge_num */
	18, /* unlnk_pge_num */
};

/* 8852C DBCC USB3.0 */
static struct dle_size_t ple_size35 = {
	MAC_AX_PLE_PG_128, /* pge_size */
	3376, /* lnk_pge_num */
	16, /* unlnk_pge_num */
};

/* PCIE 64 */
static struct wde_quota_t wde_qt0 = {
	3792, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	107, /* cpu_io */
};

/* SDIO, PCIE STF, USB */
static struct wde_quota_t wde_qt1 = {
	512, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	60, /* cpu_io */
};

/* PCIE 128 */
static struct wde_quota_t wde_qt2 = {
	1896, /* hif */
	98, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	22, /* cpu_io */
};

/* PCIE SU TP */
static struct wde_quota_t wde_qt3 = {
	256, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	44, /* cpu_io */
};

/* DLFW */
static struct wde_quota_t wde_qt4 = {
	0, /* hif */
	0, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	0, /* cpu_io */
};

/* PCIE BCN TEST */
static struct wde_quota_t wde_qt5 = {
	3666, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	44, /* cpu_io */
};

/* PCIE 64 */
static struct wde_quota_t wde_qt6 = {
	448, /* hif */
	48, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	16, /* cpu_io */
};

/* PCIE 128 */
static struct wde_quota_t wde_qt7 = {
	896, /* hif */
	56, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* PCIE STF, USB */
static struct wde_quota_t wde_qt8 = {
	204, /* hif */
	44, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* LA-PCIE 8852A*/
static struct wde_quota_t wde_qt9 = {
	1172, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* LA-PCIE */
static struct wde_quota_t wde_qt10 = {
	200, /* hif */
	48, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* LA-SDIO 8852A*/
static struct wde_quota_t wde_qt11 = {
	128, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	4, /* cpu_io */
};

/* SDIO SCC */
static struct wde_quota_t wde_qt12 = {
	112, /* hif */
	8, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* SDIO LA */
static struct wde_quota_t wde_qt13 = {
	112, /* hif */
	4, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* LA USB 8852A */
static struct wde_quota_t wde_qt14 = {
	256, /* hif */
	118, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	10, /* cpu_io */
};

/* LA USB 8852B */
static struct wde_quota_t wde_qt15 = {
	112, /* hif */
	4, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/*8852C   USB3.0 */
static struct wde_quota_t wde_qt16 = {
	344, /* hif */
	2, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/*8852C & 8192XB & 1115E DLFW */
static struct wde_quota_t wde_qt17 = {
	0, /* hif */
	0, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	0, /* cpu_io */
};

/* 8852C PCIE SCC */
static struct wde_quota_t wde_qt18 = {
	3228, /* hif */
	60, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* 8852C PCIE DBCC */
static struct wde_quota_t wde_qt19 = {
	3218, /* hif */
	60, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	50, /* cpu_io */
};

/* 8852C PCIE SCC STF */
static struct wde_quota_t wde_qt20 = {
	216, /* hif */
	20, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	20, /* cpu_io */
};

/* 8852C PCIE DBCC STF */
static struct wde_quota_t wde_qt21 = {
	152, /* hif */
	64, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* 8852C PCIE LA */
static struct wde_quota_t wde_qt22 = {
	3120, /* hif */
	64, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* PCIE SCC TRUBO */
static struct wde_quota_t wde_qt23 = {
	960, /* hif */
	48, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	16, /* cpu_io */
};

/* 8852B USB CABV */
static struct wde_quota_t wde_qt24 = {
	164, /* hif */
	44, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* 8852B USB2.0/USB3.0 SCC*/
static struct wde_quota_t wde_qt25 = {
	152, /* hif */
	2, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/* 8852AU ccv */
static struct wde_quota_t wde_qt26 = {
	504, /* hif */
	196, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	60, /* cpu_io */
};

/*8852C   USB DBCC USB2.0 */
static struct wde_quota_t wde_qt27 = {
	328, /* hif */
	6, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* 1115E PCIE DBCC */
static struct wde_quota_t wde_qt28 = {
	3218, /* hif */
	60, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	50, /* cpu_io */
};

/* 1115E PCIE SCC */
static struct wde_quota_t wde_qt29 = {
	3228, /* hif */
	60, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* 8852B USB2.0/USB3.0 SCC TURBO*/
static struct wde_quota_t wde_qt30 = {
	210, /* hif */
	2, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	8, /* cpu_io */
};

/*8852C   USB2.0 */
static struct wde_quota_t wde_qt31 = {
	338, /* hif */
	6, /* wcpu */
	0, /* dcpu */
	0, /* pkt_in */
	40, /* cpu_io */
};

/* PCIE DBCC */
static struct ple_quota_t ple_qt0 = {
	264, /* cmac0_tx */
	66, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE DBCC */
static struct ple_quota_t ple_qt1 = {
	264, /* cmac0_tx */
	66, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	941, /* cmac0_dma */
	679, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	240, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO */
static struct ple_quota_t ple_qt2 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	26, /* mpdu_proc */
	360, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO */
static struct ple_quota_t ple_qt3 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	1149, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	1159, /* mpdu_proc */
	1493, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	120, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE SCC */
static struct ple_quota_t ple_qt4 = {
	264, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE SCC */
static struct ple_quota_t ple_qt5 = {
	264, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	1101, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	120, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO */
static struct ple_quota_t ple_qt6 = {
	2048, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	26, /* mpdu_proc */
	360, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO */
static struct ple_quota_t ple_qt7 = {
	2048, /* cmac0_tx */
	0, /* cmac1_tx */
	530, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	540, /* mpdu_proc */
	874, /* cmac0_dma */
	608, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	240, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF SCC */
static struct ple_quota_t ple_qt8 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF SCC */
static struct ple_quota_t ple_qt9 = {
	2686, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	1506, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	120, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF DBCC */
static struct ple_quota_t ple_qt10 = {
	2272, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF DBCC */
static struct ple_quota_t ple_qt11 = {
	2579, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	663, /* cmac0_dma */
	401, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	240, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE SU TP */
static struct ple_quota_t ple_qt12 = {
	66, /* cmac0_tx */
	66, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	25, /* cmac0_dma */
	25, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* DLFW */
static struct ple_quota_t ple_qt13 = {
	0, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	0, /* wcpu */
	0, /* mpdu_proc */
	0, /* cmac0_dma */
	0, /* cma1_dma */
	0, /* bb_rpt */
	0, /* wd_rel */
	0, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE BCN TEST */
static struct ple_quota_t ple_qt14 = {
	588, /* cmac0_tx */
	147, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	26, /* mpdu_proc */
	356, /* cmac0_dma */
	89, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	80, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE BCN TEST */
static struct ple_quota_t ple_qt15 = {
	688, /* cmac0_tx */
	247, /* cmac1_tx */
	116, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	126, /* mpdu_proc */
	456, /* cmac0_dma */
	189, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	80, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB DBCC */
static struct ple_quota_t ple_qt16 = {
	2048, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	360, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB DBCC */
static struct ple_quota_t ple_qt17 = {
	2048, /* cmac0_tx */
	0, /* cmac1_tx */
	515, /* c2h */
	48, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	859, /* cmac0_dma */
	593, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	240, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE 64 */
static struct ple_quota_t ple_qt18 = {
	147, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	17, /* wcpu */
	13, /* mpdu_proc */
	89, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE 64 */
static struct ple_quota_t ple_qt19 = {
	147, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	580, /* wcpu */
	13, /* mpdu_proc */
	745, /* cmac0_dma */
	0, /* cma1_dma */
	599, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF */
static struct ple_quota_t ple_qt20 = {
	962, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	88, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE STF */
static struct ple_quota_t ple_qt21 = {
	1023, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	149, /* wcpu */
	13, /* mpdu_proc */
	239, /* cmac0_dma */
	0, /* cma1_dma */
	93, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE 128 */
static struct ple_quota_t ple_qt22 = {
	269, /* cmac0_tx */
	0, /* cmac1_tx */
	18, /* c2h */
	20, /* h2c */
	15, /* wcpu */
	28, /* mpdu_proc */
	180, /* cmac0_dma */
	0, /* cma1_dma */
	34, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8852A*/
static struct ple_quota_t ple_qt23 = {
	104, /* cmac0_tx */
	26, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8852A*/
static struct ple_quota_t ple_qt24 = {
	104, /* cmac0_tx */
	26, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	437, /* cmac0_dma */
	175, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	89, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB SCC */
static struct ple_quota_t ple_qt25 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	360, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB SCC */
static struct ple_quota_t ple_qt26 = {
	1536, /* cmac0_tx */
	0, /* cmac1_tx */
	1134, /* c2h */
	48, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	1478, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	120, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB3.0 52B TURBO */
static struct ple_quota_t ple_qt27 = {
	1040, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB3.0 52B TURBO*/
static struct ple_quota_t ple_qt28 = {
	1040, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	37, /* wcpu */
	13, /* mpdu_proc */
	211, /* cmac0_dma */
	0, /* cma1_dma */
	65, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE SU TP */
static struct ple_quota_t ple_qt29 = {
	66, /* cmac0_tx */
	66, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	1224, /* cmac0_dma */
	1224, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	240, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE LA */
static struct ple_quota_t ple_qt30 = {
	51, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE LA */
static struct ple_quota_t ple_qt31 = {
	74, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	36, /* wcpu */
	13, /* mpdu_proc */
	201, /* cmac0_dma */
	0, /* cma1_dma */
	55, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA SDIO 8852A*/
static struct ple_quota_t ple_qt32 = {
	500, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	26, /* mpdu_proc */
	360, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA SDIO 8852A*/
static struct ple_quota_t ple_qt33 = {
	500, /* cmac0_tx */
	0, /* cmac1_tx */
	222, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	232, /* mpdu_proc */
	566, /* cmac0_dma */
	300, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	214, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO SCC */
static struct ple_quota_t ple_qt34 = {
	320, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	16, /* h2c */
	26, /* wcpu */
	0, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	16, /* bb_rpt */
	1, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO SCC */
static struct ple_quota_t ple_qt35 = {
	320, /* cmac0_tx */
	0, /* cmac1_tx */
	123, /* c2h */
	16, /* h2c */
	133, /* wcpu */
	0, /* mpdu_proc */
	285, /* cmac0_dma */
	0, /* cma1_dma */
	123, /* bb_rpt */
	1, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO LA */
static struct ple_quota_t ple_qt36 = {
	86, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	52, /* wcpu */
	26, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO LA */
static struct ple_quota_t ple_qt37 = {
	86, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	52, /* wcpu */
	26, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA USB 8852A*/
static struct ple_quota_t ple_qt38 = {
	512, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	60, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	360, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA USB 8852A*/
static struct ple_quota_t ple_qt39 = {
	512, /* cmac0_tx */
	0, /* cmac1_tx */
	184, /* c2h */
	60, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	527, /* cmac0_dma */
	261, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	175, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA USB 8852B*/
static struct ple_quota_t ple_qt40 = {
	130, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	52, /* wcpu */
	13, /* mpdu_proc */
	66, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA USB 8852B*/
static struct ple_quota_t ple_qt41 = {
	175, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	97, /* wcpu */
	13, /* mpdu_proc */
	111, /* cmac0_dma */
	0, /* cma1_dma */
	77, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB 52C USB3.0*/
static struct ple_quota_t ple_qt42 = {
	1068, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	16, /* bb_rpt */
	1, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* USB 52C USB3.0*/
static struct ple_quota_t ple_qt43 = {
	3068, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	16, /* bb_rpt */
	1, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* DLFW 8852C & 8192XB & 1115E */
static struct ple_quota_t ple_qt44 = {
	0, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	256, /* h2c */
	0, /* wcpu */
	0, /* mpdu_proc */
	0, /* cmac0_dma */
	0, /* cma1_dma */
	0, /* bb_rpt */
	0, /* wd_rel */
	0, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* DLFW 8852C & 8192XB & 1115E */
static struct ple_quota_t ple_qt45 = {
	0, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	256, /* h2c */
	0, /* wcpu */
	0, /* mpdu_proc */
	0, /* cmac0_dma */
	0, /* cma1_dma */
	0, /* bb_rpt */
	0, /* wd_rel */
	0, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

#ifdef PHL_FEATURE_AP
/* 8852C PCIE SCC PLE 256 */
static struct ple_quota_t ple_qt46 = {
	100, /* cmac0_tx */
	0, /* cmac1_tx */
	8, /* c2h */
	10, /* h2c */
	7, /* wcpu */
	7, /* mpdu_proc */
	90, /* cmac0_dma */
	0, /* cma1_dma */
	16, /* bb_rpt */
	31, /* wd_rel */
	4, /* cpu_io */
	8, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE SCC PLE 256 */
static struct ple_quota_t ple_qt47 = {
	771, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	10, /* h2c */
	678, /* wcpu */
	7, /* mpdu_proc */
	761, /* cmac0_dma */
	0, /* cma1_dma */
	687, /* bb_rpt */
	31, /* wd_rel */
	80, /* cpu_io */
	679, /* tx_rpt */
	0, /* h2d */
};
#else
/* 8852C PCIE SCC */
static struct ple_quota_t ple_qt46 = {
	525, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	62, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE SCC */
static struct ple_quota_t ple_qt47 = {
	525, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	1034, /* wcpu */
	13, /* mpdu_proc */
	1199, /* cmac0_dma */
	0, /* cma1_dma */
	1053, /* bb_rpt */
	62, /* wd_rel */
	160, /* cpu_io */
	1037, /* tx_rpt */
	0, /* h2d */
};
#endif

/* 8852C PCIE DBCC */
static struct ple_quota_t ple_qt48 = {
	525, /* cmac0_tx */
	200, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	178, /* cma1_dma */
	32, /* bb_rpt */
	62, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE DBCC */
static struct ple_quota_t ple_qt49 = {
	525, /* cmac0_tx */
	200, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	656, /* wcpu */
	13, /* mpdu_proc */
	821, /* cmac0_dma */
	821, /* cma1_dma */
	675, /* bb_rpt */
	62, /* wd_rel */
	160, /* cpu_io */
	659, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE SCC STF */
static struct ple_quota_t ple_qt50 = {
	1248, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	130, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	38, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE SCC STF */
static struct ple_quota_t ple_qt51 = {
	3154, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	1919, /* wcpu */
	13, /* mpdu_proc */
	2036, /* cmac0_dma */
	0, /* cma1_dma */
	1938, /* bb_rpt */
	38, /* wd_rel */
	40, /* cpu_io */
	1922, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE DBCC STF */
static struct ple_quota_t ple_qt52 = {
	1664, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	130, /* cmac0_dma */
	130, /* cma1_dma */
	32, /* bb_rpt */
	38, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE DBCC STF */
static struct ple_quota_t ple_qt53 = {
	3024, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	1373, /* wcpu */
	13, /* mpdu_proc */
	1490, /* cmac0_dma */
	1490, /* cma1_dma */
	1392, /* bb_rpt */
	38, /* wd_rel */
	160, /* cpu_io */
	1376, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE LA */
static struct ple_quota_t ple_qt54 = {
	300, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	62, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* 8852C PCIE LA */
static struct ple_quota_t ple_qt55 = {
	300, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	1081, /* wcpu */
	13, /* mpdu_proc */
	1424, /* cmac0_dma */
	0, /* cma1_dma */
	1100, /* bb_rpt */
	62, /* wd_rel */
	160, /* cpu_io */
	1084, /* tx_rpt */
	0, /* h2d */
};

/* 8852B PCIE AB */
static struct ple_quota_t ple_qt56 = {
	147, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	68, /* wcpu */
	13, /* mpdu_proc */
	233, /* cmac0_dma */
	0, /* cma1_dma */
	87, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE SCC TURBO */
static struct ple_quota_t ple_qt57 = {
	147, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* PCIE 64 */
static struct ple_quota_t ple_qt58 = {
	147, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	157, /* wcpu */
	13, /* mpdu_proc */
	229, /* cmac0_dma */
	0, /* cma1_dma */
	172, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
};

/* USB2.0 52B TURBO */
static struct ple_quota_t ple_qt61 = {
	858, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	370, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB2.0 52B TURBO*/
static struct ple_quota_t ple_qt62 = {
	858, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	37, /* wcpu */
	13, /* mpdu_proc */
	403, /* cmac0_dma */
	0, /* cma1_dma */
	65, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* 8852AU ccv */
static struct ple_quota_t ple_qt63 = {
	2654, /* cmac0_tx */
	0, /* cmac1_tx */
	1134, /* c2h */
	48, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	1478, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	128, /* wd_rel */
	120, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/*8852C USB DBCC USB2.0*/
static struct ple_quota_t ple_qt64 = {
	442, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	13, /* wcpu */
	14, /* mpdu_proc */
	390, /* cmac0_dma */
	390, /* cma1_dma */
	32, /* bb_rpt */
	38, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/*8852C USB DBCC USB2.0*/
static struct ple_quota_t ple_qt65 = {
	442, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	1953, /* wcpu */
	14, /* mpdu_proc */
	2330, /* cmac0_dma */
	2330, /* cma1_dma */
	1972, /* bb_rpt */
	38, /* wd_rel */
	120, /* cpu_io */
	1956, /* tx_rpt */
	0, /* h2d */
};

/* SDIO SCC TURBO */
static struct ple_quota_t ple_qt66 = {
	836, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	16, /* h2c */
	26, /* wcpu */
	0, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	16, /* bb_rpt */
	1, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* SDIO SCC TURBO */
static struct ple_quota_t ple_qt67 = {
	836, /* cmac0_tx */
	0, /* cmac1_tx */
	375, /* c2h */
	16, /* h2c */
	385, /* wcpu */
	0, /* mpdu_proc */
	537, /* cmac0_dma */
	0, /* cma1_dma */
	375, /* bb_rpt */
	1, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8852C modified*/
static struct ple_quota_t ple_qt68 = {
	104, /* cmac0_tx */
	26, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	94, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8852C modified*/
static struct ple_quota_t ple_qt69 = {
	104, /* cmac0_tx */
	26, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	421, /* cmac0_dma */
	159, /* cma1_dma */
	64, /* bb_rpt */
	105, /* wd_rel */
	73, /* cpu_io */
	81, /* tx_rpt */
	0, /* h2d */
};

/* 1115E PCIE DBCC */
static struct ple_quota_t ple_qt70 = {
	501, /* cmac0_tx */
	901, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	300, /* cmac0_dma */
	600, /* cma1_dma */
	32, /* bb_rpt */
	62, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	13, /* h2d */
};

/* 1115E PCIE DBCC */
static struct ple_quota_t ple_qt71 = {
	501, /* cmac0_tx */
	901, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	190, /* wcpu */
	13, /* mpdu_proc */
	477, /* cmac0_dma */
	777, /* cma1_dma */
	209, /* bb_rpt */
	62, /* wd_rel */
	128, /* cpu_io */
	193, /* tx_rpt */
	190, /* h2d */
};

/* USB2.0 52B SCC*/
static struct ple_quota_t ple_qt72 = {
	130, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	322, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB2.0 52B 92K*/
static struct ple_quota_t ple_qt73 = {
	130, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	37, /* wcpu */
	13, /* mpdu_proc */
	355, /* cmac0_dma */
	0, /* cma1_dma */
	65, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB3.0 52B 92K*/
static struct ple_quota_t ple_qt74 = {
	286, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	4, /* wcpu */
	13, /* mpdu_proc */
	178, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	14, /* wd_rel */
	8, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* USB3.0 52B 92K*/
static struct ple_quota_t ple_qt75 = {
	286, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	37, /* wcpu */
	13, /* mpdu_proc */
	211, /* cmac0_dma */
	0, /* cma1_dma */
	65, /* bb_rpt */
	14, /* wd_rel */
	24, /* cpu_io */
	0, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8192XB modified*/
static struct ple_quota_t ple_qt76 = {
	104, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	20, /* h2c */
	26, /* wcpu */
	13, /* mpdu_proc */
	356, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	40, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* LA PCIE 8192XB modified*/
static struct ple_quota_t ple_qt77 = {
	104, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	20, /* h2c */
	64, /* wcpu */
	13, /* mpdu_proc */
	541, /* cmac0_dma */
	0, /* cma1_dma */
	64, /* bb_rpt */
	105, /* wd_rel */
	193, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* USB2.0 52C */
static struct ple_quota_t ple_qt78 = {
	1560, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	390, /* cmac0_dma */
	0, /* cma1_dma */
	32, /* bb_rpt */
	38, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/* USB2.0 52C */
static struct ple_quota_t ple_qt79 = {
	1560, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	1253, /* wcpu */
	13, /* mpdu_proc */
	1630, /* cmac0_dma */
	0, /* cma1_dma */
	1272, /* bb_rpt */
	38, /* wd_rel */
	120, /* cpu_io */
	1256, /* tx_rpt */
	0, /* h2d */
};

/*8852C USB DBCC USB3.0*/
static struct ple_quota_t ple_qt80 = {
	1112, /* cmac0_tx */
	0, /* cmac1_tx */
	16, /* c2h */
	48, /* h2c */
	13, /* wcpu */
	13, /* mpdu_proc */
	390, /* cmac0_dma */
	390, /* cma1_dma */
	32, /* bb_rpt */
	38, /* wd_rel */
	8, /* cpu_io */
	16, /* tx_rpt */
	0, /* h2d */
};

/*8852C USB DBCC USB3.0*/
static struct ple_quota_t ple_qt81 = {
	2392, /* cmac0_tx */
	0, /* cmac1_tx */
	32, /* c2h */
	48, /* h2c */
	1313, /* wcpu */
	13, /* mpdu_proc */
	390, /* cmac0_dma */
	390, /* cma1_dma */
	1332, /* bb_rpt */
	38, /* wd_rel */
	120, /* cpu_io */
	1316, /* tx_rpt */
	0, /* h2d */
};

#if MAC_AX_PCIE_SUPPORT
static struct dle_mem_t dle_mem_pcie_8852a[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size0, &ple_size0, /* wde_size, ple_size */
	 &wde_qt0, &wde_qt0, /* wde_min_qt, wde_max_qt */
	 &ple_qt4, &ple_qt5}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size0, &ple_size0, /* wde_size, ple_size */
	 &wde_qt0, &wde_qt0, /* wde_min_qt, wde_max_qt */
	 &ple_qt0, &ple_qt1}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size1, &ple_size2, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt8, &ple_qt9}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC_STF, /* qta_mode */
	 &wde_size1, &ple_size2, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt10, &ple_qt11}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SU_TP, /* qta_mode */
	 &wde_size3, &ple_size3, /* wde_size, ple_size */
	 &wde_qt3, &wde_qt3, /* wde_min_qt, wde_max_qt */
	 &ple_qt12, &ple_qt29}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size4, &ple_size4, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size10, &ple_size10, /* wde_size, ple_size */
	 &wde_qt9, &wde_qt9, /* wde_min_qt, wde_max_qt */
	 &ple_qt23, &ple_qt24}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8852b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size6, &ple_size6, /* wde_size, ple_size */
	 &wde_qt6, &wde_qt6, /* wde_min_qt, wde_max_qt */
	 &ple_qt18, &ple_qt58}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size23, &ple_size24, /* wde_size, ple_size */
	 &wde_qt23, &wde_qt23, /* wde_min_qt, wde_max_qt */
	 &ple_qt57, &ple_qt19}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size8, &ple_size7, /* wde_size, ple_size */
	 &wde_qt8, &wde_qt8, /* wde_min_qt, wde_max_qt */
	 &ple_qt20, &ple_qt21}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size11, &ple_size11, /* wde_size, ple_size */
	 &wde_qt10, &wde_qt10, /* wde_min_qt, wde_max_qt */
	 &ple_qt30, &ple_qt31}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8852c[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size19, &ple_size19, /* wde_size, ple_size */
	 &wde_qt18, &wde_qt18, /* wde_min_qt, wde_max_qt */
	 &ple_qt46, &ple_qt47}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size20, &ple_size20, /* wde_size, ple_size */
	 &wde_qt19, &wde_qt19, /* wde_min_qt, wde_max_qt */
	 &ple_qt48, &ple_qt49}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt20, &wde_qt20, /* wde_min_qt, wde_max_qt */
	 &ple_qt50, &ple_qt51}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt21, &wde_qt21, /* wde_min_qt, wde_max_qt */
	 &ple_qt52, &ple_qt53}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size10, &ple_size10, /* wde_size, ple_size */
	 &wde_qt9, &wde_qt9, /* wde_min_qt, wde_max_qt */
	 &ple_qt68, &ple_qt69}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8192xb[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size19, &ple_size19, /* wde_size, ple_size */
	 &wde_qt18, &wde_qt18, /* wde_min_qt, wde_max_qt */
	 &ple_qt46, &ple_qt47}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt20, &wde_qt20, /* wde_min_qt, wde_max_qt */
	 &ple_qt50, &ple_qt51}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size10, &ple_size10, /* wde_size, ple_size */
	 &wde_qt9, &wde_qt9, /* wde_min_qt, wde_max_qt */
	 &ple_qt76, &ple_qt77}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8851b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size6, &ple_size6, /* wde_size, ple_size */
	 &wde_qt6, &wde_qt6, /* wde_min_qt, wde_max_qt */
	 &ple_qt18, &ple_qt58}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size23, &ple_size24, /* wde_size, ple_size */
	 &wde_qt23, &wde_qt23, /* wde_min_qt, wde_max_qt */
	 &ple_qt57, &ple_qt19}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size8, &ple_size7, /* wde_size, ple_size */
	 &wde_qt8, &wde_qt8, /* wde_min_qt, wde_max_qt */
	 &ple_qt20, &ple_qt21}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size11, &ple_size11, /* wde_size, ple_size */
	 &wde_qt10, &wde_qt10, /* wde_min_qt, wde_max_qt */
	 &ple_qt30, &ple_qt31}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8851e[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size19, &ple_size19, /* wde_size, ple_size */
	 &wde_qt18, &wde_qt18, /* wde_min_qt, wde_max_qt */
	 &ple_qt46, &ple_qt47}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size20, &ple_size20, /* wde_size, ple_size */
	 &wde_qt19, &wde_qt19, /* wde_min_qt, wde_max_qt */
	 &ple_qt48, &ple_qt49}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt20, &wde_qt20, /* wde_min_qt, wde_max_qt */
	 &ple_qt50, &ple_qt51}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt21, &wde_qt21, /* wde_min_qt, wde_max_qt */
	 &ple_qt52, &ple_qt53}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size10, &ple_size10, /* wde_size, ple_size */
	 &wde_qt9, &wde_qt9, /* wde_min_qt, wde_max_qt */
	 &ple_qt68, &ple_qt69}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_8852d[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size19, &ple_size19, /* wde_size, ple_size */
	 &wde_qt18, &wde_qt18, /* wde_min_qt, wde_max_qt */
	 &ple_qt46, &ple_qt47}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size20, &ple_size20, /* wde_size, ple_size */
	 &wde_qt19, &wde_qt19, /* wde_min_qt, wde_max_qt */
	 &ple_qt48, &ple_qt49}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_SCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt20, &wde_qt20, /* wde_min_qt, wde_max_qt */
	 &ple_qt50, &ple_qt51}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC_STF, /* qta_mode */
	 &wde_size21, &ple_size21, /* wde_size, ple_size */
	 &wde_qt21, &wde_qt21, /* wde_min_qt, wde_max_qt */
	 &ple_qt52, &ple_qt53}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size10, &ple_size10, /* wde_size, ple_size */
	 &wde_qt9, &wde_qt9, /* wde_min_qt, wde_max_qt */
	 &ple_qt68, &ple_qt69}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_pcie_1115e[] = {
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size28, &ple_size30, /* wde_size, ple_size */
	 &wde_qt28, &wde_qt28, /* wde_min_qt, wde_max_qt */
	 &ple_qt70, &ple_qt71}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size28, &ple_size30, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

#if MAC_AX_USB_SUPPORT
static struct dle_mem_t dle_mem_usb_8852a[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size1, &ple_size1, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt25, &ple_qt26}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size1, &ple_size1, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt16, &ple_qt17}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size4, &ple_size4, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size15, &ple_size15, /* wde_size, ple_size */
	 &wde_qt14, &wde_qt14, /* wde_min_qt, wde_max_qt */
	 &ple_qt38, &ple_qt39}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb_8852a_ccv[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size26, &ple_size1, /* wde_size, ple_size */
	 &wde_qt26, &wde_qt26, /* wde_min_qt, wde_max_qt */
	 &ple_qt25, &ple_qt63}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size1, &ple_size1, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt16, &ple_qt17}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size4, &ple_size4, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size15, &ple_size15, /* wde_size, ple_size */
	 &wde_qt14, &wde_qt14, /* wde_min_qt, wde_max_qt */
	 &ple_qt38, &ple_qt39}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb2_8852b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size25, &ple_size32, /* wde_size, ple_size */
	 &wde_qt25, &wde_qt25, /* wde_min_qt, wde_max_qt */
	 &ple_qt72, &ple_qt73}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size30, &ple_size31, /* wde_size, ple_size */
	 &wde_qt30, &wde_qt30, /* wde_min_qt, wde_max_qt */
	 &ple_qt27, &ple_qt28}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb3_8852b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size25, &ple_size33, /* wde_size, ple_size */
	 &wde_qt25, &wde_qt25, /* wde_min_qt, wde_max_qt */
	 &ple_qt74, &ple_qt75}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size30, &ple_size31, /* wde_size, ple_size */
	 &wde_qt30, &wde_qt30, /* wde_min_qt, wde_max_qt */
	 &ple_qt27, &ple_qt28}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb2_8852c[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size31, &ple_size34, /* wde_size, ple_size */
	 &wde_qt31, &wde_qt31, /* wde_min_qt, wde_max_qt */
	 &ple_qt78, &ple_qt79}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size28, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt64, &ple_qt65}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb3_8852c[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size17, &ple_size17, /* wde_size, ple_size */
	 &wde_qt16, &wde_qt16, /* wde_min_qt, wde_max_qt */
	 &ple_qt42, &ple_qt43}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size35, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt80, &ple_qt81}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb_8192xb[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size17, &ple_size17, /* wde_size, ple_size */
	 &wde_qt16, &wde_qt16, /* wde_min_qt, wde_max_qt */
	 &ple_qt42, &ple_qt43}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size28, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt64, &ple_qt65}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb2_8851b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size25, &ple_size32, /* wde_size, ple_size */
	 &wde_qt25, &wde_qt25, /* wde_min_qt, wde_max_qt */
	 &ple_qt72, &ple_qt73}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size30, &ple_size27, /* wde_size, ple_size */
	 &wde_qt30, &wde_qt30, /* wde_min_qt, wde_max_qt */
	 &ple_qt61, &ple_qt62}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb3_8851b[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size25, &ple_size33, /* wde_size, ple_size */
	 &wde_qt25, &wde_qt25, /* wde_min_qt, wde_max_qt */
	 &ple_qt74, &ple_qt75}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size30, &ple_size31, /* wde_size, ple_size */
	 &wde_qt30, &wde_qt30, /* wde_min_qt, wde_max_qt */
	 &ple_qt27, &ple_qt28}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb2_8851e[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size31, &ple_size34, /* wde_size, ple_size */
	 &wde_qt31, &wde_qt31, /* wde_min_qt, wde_max_qt */
	 &ple_qt78, &ple_qt79}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size28, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt64, &ple_qt65}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb3_8851e[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size17, &ple_size17, /* wde_size, ple_size */
	 &wde_qt16, &wde_qt16, /* wde_min_qt, wde_max_qt */
	 &ple_qt42, &ple_qt43}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size35, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt80, &ple_qt81}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb2_8852d[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size31, &ple_size34, /* wde_size, ple_size */
	 &wde_qt31, &wde_qt31, /* wde_min_qt, wde_max_qt */
	 &ple_qt78, &ple_qt79}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size28, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt64, &ple_qt65}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_usb3_8852d[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size17, &ple_size17, /* wde_size, ple_size */
	 &wde_qt16, &wde_qt16, /* wde_min_qt, wde_max_qt */
	 &ple_qt42, &ple_qt43}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size27, &ple_size35, /* wde_size, ple_size */
	 &wde_qt27, &wde_qt27, /* wde_min_qt, wde_max_qt */
	 &ple_qt80, &ple_qt81}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size18, &ple_size18, /* wde_size, ple_size */
	 &wde_qt17, &wde_qt17, /* wde_min_qt, wde_max_qt */
	 &ple_qt44, &ple_qt45}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size16, &ple_size16, /* wde_size, ple_size */
	 &wde_qt15, &wde_qt15, /* wde_min_qt, wde_max_qt */
	 &ple_qt40, &ple_qt41}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

#if MAC_AX_SDIO_SUPPORT
static struct dle_mem_t dle_mem_sdio_8852a[] = {
	{MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size1, &ple_size1, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt2, &ple_qt3}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DBCC, /* qta_mode */
	 &wde_size1, &ple_size1, /* wde_size, ple_size */
	 &wde_qt1, &wde_qt1, /* wde_min_qt, wde_max_qt */
	 &ple_qt6, &ple_qt7}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size4, &ple_size4, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size12, &ple_size12, /* wde_size, ple_size */
	 &wde_qt11, &wde_qt11, /* wde_min_qt, wde_max_qt */
	 &ple_qt32, &ple_qt33}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_sdio_8852b[] = {
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size13, &ple_size13, /* wde_size, ple_size */
	 &wde_qt12, &wde_qt12, /* wde_min_qt, wde_max_qt */
	 &ple_qt34, &ple_qt35}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size13, &ple_size29, /* wde_size, ple_size */
	 &wde_qt12, &wde_qt12, /* wde_min_qt, wde_max_qt */
	 &ple_qt66, &ple_qt67}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size14, &ple_size14, /* wde_size, ple_size */
	 &wde_qt13, &wde_qt13, /* wde_min_qt, wde_max_qt */
	 &ple_qt36, &ple_qt37}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};

static struct dle_mem_t dle_mem_sdio_8851b[] = {
	{MAC_AX_QTA_DLFW, /* qta_mode */
	 &wde_size9, &ple_size8, /* wde_size, ple_size */
	 &wde_qt4, &wde_qt4, /* wde_min_qt, wde_max_qt */
	 &ple_qt13, &ple_qt13}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC, /* qta_mode */
	 &wde_size13, &ple_size13, /* wde_size, ple_size */
	 &wde_qt12, &wde_qt12, /* wde_min_qt, wde_max_qt */
	 &ple_qt34, &ple_qt35}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_SCC_TURBO, /* qta_mode */
	 &wde_size13, &ple_size29, /* wde_size, ple_size */
	 &wde_qt12, &wde_qt12, /* wde_min_qt, wde_max_qt */
	 &ple_qt66, &ple_qt67}, /* ple_min_qt, ple_max_qt */
	 {MAC_AX_QTA_LAMODE, /* qta_mode */
	 &wde_size14, &ple_size14, /* wde_size, ple_size */
	 &wde_qt13, &wde_qt13, /* wde_min_qt, wde_max_qt */
	 &ple_qt36, &ple_qt37}, /* ple_min_qt, ple_max_qt */
	{MAC_AX_QTA_INVALID, NULL, NULL, NULL, NULL, NULL, NULL},
};
#endif

static u32 dle_rsvd_size(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode);
static u32 h2c_dle_qta_change(struct mac_ax_adapter *adapter);

u32 dle_dfi_freepg(struct mac_ax_adapter *adapter,
		   struct dle_dfi_freepg_t *freepg)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct dle_dfi_ctrl_t ctrl;
	u32 ret;

	ctrl.type = freepg->dle_type;
	ctrl.target = DLE_DFI_TYPE_FREEPG;
	ctrl.addr = DFI_TYPE_FREEPG_IDX;
	ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle_dfi_ctrl %d\n", ret);
		return ret;
	}
	freepg->free_headpg = ctrl.u.freepg.free_headpg;
	freepg->free_tailpg = ctrl.u.freepg.free_tailpg;

	ctrl.addr = DFI_TYPE_FREEPG_PUBNUM;
	ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle_dfi_ctrl %d\n", ret);
		return ret;
	}
	freepg->pub_pgnum = ctrl.u.freepg.pub_pgnum;

	return MACSUCCESS;
}

u32 dle_dfi_quota(struct mac_ax_adapter *adapter,
		  struct dle_dfi_quota_t *quota)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct dle_dfi_ctrl_t ctrl;
	u32 ret;

	ctrl.type = quota->dle_type;
	ctrl.target = DLE_DFI_TYPE_QUOTA;
	ctrl.addr = quota->qtaid;
	ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle_dfi_ctrl %d\n", ret);
		return ret;
	}

	quota->rsv_pgnum = ctrl.u.quota.rsv_pgnum;
	quota->use_pgnum = ctrl.u.quota.use_pgnum;

	return MACSUCCESS;
}

u32 dle_dfi_qempty(struct mac_ax_adapter *adapter,
		   struct dle_dfi_qempty_t *qempty)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct dle_dfi_ctrl_t ctrl;
	u32 ret;

	ctrl.type = qempty->dle_type;
	ctrl.target = DLE_DFI_TYPE_QEMPTY;
	ctrl.addr = qempty->grpsel;
	ret = p_ops->dle_dfi_ctrl(adapter, &ctrl);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle_dfi_ctrl %d\n", ret);
		return ret;
	}

	qempty->qempty = ctrl.u.qempty.qempty;

	return MACSUCCESS;
}

u32 mac_chk_allq_empty(struct mac_ax_adapter *adapter, u8 *empty)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u8 chk_cnt, txq_empty, rxq_empty;
	u32 ret;
	*empty = 1;

	for (chk_cnt = 0; chk_cnt < QUEUE_EMPTY_CHK_CNT; chk_cnt++) {
		ret = p_ops->dle_is_txq_empty(adapter, &txq_empty);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] TXQ chk fail %d\n", ret);
			return ret;
		}

		ret = p_ops->dle_is_rxq_empty(adapter, &rxq_empty);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] TXQ chk fail %d\n", ret);
			return ret;
		}

		if (!(txq_empty & rxq_empty)) {
			PLTFM_MSG_TRACE("[TRACE] CHK TXQ %d RXQ %d\n",
					txq_empty, rxq_empty);
			*empty = 0;
			break;
		}
	}

	return MACSUCCESS;
}

static u32 chk_preload_allow(struct mac_ax_adapter *adapter)
{
	u8 is_poh;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
	    adapter->hw_info->intf != MAC_AX_INTF_PCIE)
		return MACHWNOSUP;

	if (is_qta_poh(adapter, adapter->dle_info.qta_mode, &is_poh) != MACSUCCESS)
		return MACFUNCINPUT;

	if (!is_poh)
		return MACHWNOSUP;

	return MACSUCCESS;
}

u32 mac_preload_cfg(struct mac_ax_adapter *adapter, enum mac_ax_band band,
		    struct mac_ax_preld_cfg *cfg)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret;

	ret = chk_preload_allow(adapter);
	if (ret == MACHWNOSUP) {
		return MACSUCCESS;
	} else if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]check preload allow %d\n", ret);
		return ret;
	}

	if (cfg->hiq_en && cfg->hiq_port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]preload hiq p%d invalid\n", cfg->hiq_port);
		return MACFUNCINPUT;
	}

	if (cfg->hiq_en && cfg->hiq_port == MAC_AX_PORT_0 &&
	    cfg->hiq_mbid >= MAC_AX_P0_MBID_MAX) {
		PLTFM_MSG_ERR("[ERR]preload hiq p0mb%d invalid\n", cfg->hiq_mbid);
		return MACFUNCINPUT;
	}

	ret = p_ops->preload_cfg_set(adapter, band, cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]preload cfg set %d\n", ret);
		return ret;
	}

	return MACSUCCESS;
}

u32 preload_init(struct mac_ax_adapter *adapter, enum mac_ax_band band,
		 enum mac_ax_qta_mode mode)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret;

	ret = chk_preload_allow(adapter);
	if (ret == MACHWNOSUP) {
		return MACSUCCESS;
	} else if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]check preload allow %d\n", ret);
		return ret;
	}

	ret = p_ops->preload_init_set(adapter, band, mode);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]preload init set %d\n", ret);
		return ret;
	}

	return MACSUCCESS;
}

u32 get_dle_rsvd_info(struct mac_ax_adapter *adapter, enum DLE_RSVD_INFO *info)
{
	enum mac_ax_qta_mode mode = adapter->dle_info.qta_mode;
	u32 rsvd_size;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		if (mode == MAC_AX_QTA_LAMODE)
			*info = DLE_RSVD_INFO_LAMODE;
		else if (mode == MAC_AX_QTA_SCC)
			*info = DLE_RSVD_INFO_FW;
		else
			*info = DLE_RSVD_INFO_NONE;
	} else {
		if (mode == MAC_AX_QTA_LAMODE)
			*info = DLE_RSVD_INFO_LAMODE;
		else
			*info = DLE_RSVD_INFO_NONE;
	}

	rsvd_size = dle_rsvd_size(adapter, mode);
	if (rsvd_size != 0 && *info == DLE_RSVD_INFO_NONE) {
		PLTFM_MSG_ERR("[ERR]get dle_rsvd_info %d, rsvd_size %d\n",
			      *info, rsvd_size);
		return MACFFCFG;
	}

	return MACSUCCESS;
}

static u32 dle_used_size(struct dle_size_t *wde, struct dle_size_t *ple)
{
	return (u32)(wde->pge_size * (wde->lnk_pge_num + wde->unlnk_pge_num)) +
	       (u32)(ple->pge_size * (ple->lnk_pge_num + ple->unlnk_pge_num));
}

static u32 dle_rsvd_size(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode)
{
	if (mode == MAC_AX_QTA_LAMODE) {
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			return DLE_LAMODE_SIZE_8852A;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			return DLE_LAMODE_SIZE_8852B;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
			return DLE_LAMODE_SIZE_8852C;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
			return DLE_LAMODE_SIZE_8192XB;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			return DLE_LAMODE_SIZE_8851B;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
			return DLE_LAMODE_SIZE_8851E;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			return DLE_LAMODE_SIZE_8852D;
		else
			return 0;
	} else if (mode == MAC_AX_QTA_SCC) {
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			return DLE_SCC_RSVD_SIZE_8852B;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			return DLE_SCC_RSVD_SIZE_8851B;
		else
			return 0;
	} else {
		return 0;
	}
}

static struct dle_mem_t *get_dle_mem_cfg(struct mac_ax_adapter *adapter,
					 enum mac_ax_qta_mode mode)
{
	struct dle_mem_t *cfg;
	enum mac_ax_intf intf = adapter->hw_info->intf;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			cfg = dle_mem_sdio_8852a;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			cfg = dle_mem_sdio_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
			cfg = dle_mem_sdio_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
			cfg = dle_mem_sdio_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			cfg = dle_mem_sdio_8851b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
			cfg = dle_mem_sdio_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			cfg = dle_mem_sdio_8852b;
		else
			cfg = NULL;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			cfg = dle_mem_usb_8852a;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) &&
			 (get_usb_mode(adapter) == MAC_AX_USB2))
			cfg = dle_mem_usb3_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			cfg = dle_mem_usb3_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) &&
			 (get_usb_mode(adapter) == MAC_AX_USB2))
			cfg = dle_mem_usb2_8852c;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
			cfg = dle_mem_usb3_8852c;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
			cfg = dle_mem_usb_8192xb;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) &&
			 (get_usb_mode(adapter) == MAC_AX_USB2))
			cfg = dle_mem_usb2_8851b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			cfg = dle_mem_usb3_8851b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) &&
			 (get_usb_mode(adapter) == MAC_AX_USB2))
			cfg = dle_mem_usb2_8851e;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
			cfg = dle_mem_usb3_8851e;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) &&
			 (get_usb_mode(adapter) == MAC_AX_USB2))
			cfg = dle_mem_usb2_8852d;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			cfg = dle_mem_usb3_8852d;
		else
			cfg = NULL;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
			cfg = dle_mem_pcie_8852a;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B))
			cfg = dle_mem_pcie_8852b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C))
			cfg = dle_mem_pcie_8852c;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB))
			cfg = dle_mem_pcie_8192xb;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
			cfg = dle_mem_pcie_8851b;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8851E))
			cfg = dle_mem_pcie_8851e;
		else if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))
			cfg = dle_mem_pcie_8852d;
		else if (is_chip_id(adapter, MAC_BE_CHIP_ID_1115E))
			cfg = dle_mem_pcie_1115e;
		else
			cfg = NULL;
		break;
#endif
	default:
		cfg = NULL;
		break;
	}

	if (!cfg)
		return NULL;
	for (; cfg->mode != MAC_AX_QTA_INVALID; cfg++) {
		if (cfg->mode == mode) {
			adapter->dle_info.wde_pg_size = cfg->wde_size->pge_size;
			adapter->dle_info.ple_pg_size = cfg->ple_size->pge_size;
			adapter->dle_info.qta_mode = mode;
			adapter->dle_info.c0_rx_qta = cfg->ple_min_qt->cma0_dma;
			adapter->dle_info.c1_rx_qta = cfg->ple_min_qt->cma1_dma;
			adapter->dle_info.c0_tx_min = cfg->ple_min_qt->cma0_tx;
			adapter->dle_info.c0_tx_max = cfg->ple_max_qt->cma0_tx;
			adapter->dle_info.c1_tx_min = cfg->ple_min_qt->cma1_tx;
			adapter->dle_info.c1_tx_max = cfg->ple_max_qt->cma1_tx;
			adapter->dle_info.hif_min = cfg->wde_min_qt->hif;
			return cfg;
		}
	}

	return NULL;
}

static u32 dle_quota_cfg(struct mac_ax_adapter *adapter, struct dle_mem_t *cfg)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret;

	ret = p_ops->wde_quota_cfg(adapter, cfg->wde_min_qt, cfg->wde_max_qt);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]wde quota cfg %d\n", ret);
		return ret;
	}
	ret = p_ops->ple_quota_cfg(adapter, cfg->ple_min_qt, cfg->ple_max_qt);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]ple quota cfg %d\n", ret);
		return ret;
	}

	return ret;
}

u32 dle_quota_change(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode)
{
	u32 ret = MACSUCCESS;
	struct dle_mem_t *cfg;
	struct cpuio_buf_req_t buf_req;
	struct cpuio_ctrl_t ctrl_para;
	u32 val32_1, val32_2;

	cfg = get_dle_mem_cfg(adapter, mode);
	if (!cfg) {
		PLTFM_MSG_ERR("[ERR]wd/dle mem cfg\n");
		return MACNOITEM;
	}

	val32_1 = dle_used_size(cfg->wde_size, cfg->ple_size);
	val32_2 = adapter->hw_info->fifo_size - dle_rsvd_size(adapter, mode);
	if (val32_1 != val32_2) {
		PLTFM_MSG_ERR("[ERR]dle used size %d not match %d\n",
			      val32_1, val32_2);
		return MACFFCFG;
	}

	ret = dle_quota_cfg(adapter, cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle qutoa cfg %d\n", ret);
		return ret;
	}

	//Trigger change by enqueue packet
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		/* send h2c as workaround */
		ret = h2c_dle_qta_change(adapter);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]h2c_dle_qta_change %d\n", ret);
			return ret;
		}
	} else {
		// Use CPUIO as workaround (suggested by DD TM_LIN).
		//WD
		buf_req.len = 0x20; // chris comment
		ret = mac_dle_buf_req_wd(adapter, &buf_req);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]WDE DLE buf req %d\n", ret);
			return ret;
		}

		PLTFM_MEMSET((void *)&ctrl_para, 0, sizeof(ctrl_para));
		ctrl_para.cmd_type = CPUIO_OP_CMD_ENQ_TO_HEAD;
		ctrl_para.start_pktid = buf_req.pktid;
		ctrl_para.end_pktid = buf_req.pktid;
		ctrl_para.pkt_num = 0;
		ctrl_para.dst_pid = WDE_DLE_PORT_ID_WDRLS;
		ctrl_para.dst_qid = WDE_DLE_QUEID_NO_REPORT;
		ret = mac_set_cpuio_wd(adapter, &ctrl_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]WDE DLE enqueue to head %d\n", ret);
			return ret;
		}

		//PL
		buf_req.len = 0x20;
		ret = mac_dle_buf_req_pl(adapter, &buf_req);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]PLE DLE buf req %d\n", ret);
			return ret;
		}

		PLTFM_MEMSET((void *)&ctrl_para, 0, sizeof(ctrl_para));
		ctrl_para.cmd_type = CPUIO_OP_CMD_ENQ_TO_HEAD;
		ctrl_para.start_pktid = buf_req.pktid;
		ctrl_para.end_pktid = buf_req.pktid;
		ctrl_para.pkt_num = 0;
		ctrl_para.dst_pid = PLE_DLE_PORT_ID_PLRLS;
		ctrl_para.dst_qid = PLE_DLE_QUEID_NO_REPORT;
		ret = mac_set_cpuio_pl(adapter, &ctrl_para);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]PLE DLE enqueue to head %d\n", ret);
			return ret;
		}
	}

	return ret;
}

u32 dle_init(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
	     enum mac_ax_qta_mode ext_mode)
{
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret = MACSUCCESS, ret2;
	u32 val32_1, val32_2;
	struct dle_mem_t *cfg, *ext_cfg;
	struct dle_mem_t cfg_tmp;
	struct dle_size_t wde_size_tmp, ple_size_tmp;
	struct wde_quota_t wde_min_qt_tmp, wde_max_qt_tmp;
	struct ple_quota_t ple_min_qt_tmp, ple_max_qt_tmp;

	ret = check_mac_en(adapter, 0, MAC_AX_DMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]chk dmac en %d\n", ret);
		return ret;
	}

	cfg = get_dle_mem_cfg(adapter, mode);
	if (!cfg) {
		ret = MACNOITEM;
		PLTFM_MSG_ERR("[ERR]get_dle_mem_cfg %d\n", mode);
		goto error;
	}

	PLTFM_MEMCPY(&wde_size_tmp, cfg->wde_size, sizeof(struct dle_size_t));
	PLTFM_MEMCPY(&ple_size_tmp, cfg->ple_size, sizeof(struct dle_size_t));
	PLTFM_MEMCPY(&wde_min_qt_tmp, cfg->wde_min_qt, sizeof(struct wde_quota_t));
	PLTFM_MEMCPY(&wde_max_qt_tmp, cfg->wde_max_qt, sizeof(struct wde_quota_t));
	PLTFM_MEMCPY(&ple_min_qt_tmp, cfg->ple_min_qt, sizeof(struct ple_quota_t));
	PLTFM_MEMCPY(&ple_max_qt_tmp, cfg->ple_max_qt, sizeof(struct ple_quota_t));
	cfg_tmp.mode = cfg->mode;
	cfg_tmp.wde_size = &wde_size_tmp;
	cfg_tmp.ple_size = &ple_size_tmp;
	cfg_tmp.wde_min_qt = &wde_min_qt_tmp;
	cfg_tmp.wde_max_qt = &wde_max_qt_tmp;
	cfg_tmp.ple_min_qt = &ple_min_qt_tmp;
	cfg_tmp.ple_max_qt = &ple_max_qt_tmp;
	cfg = &cfg_tmp;

	if (mode == MAC_AX_QTA_DLFW) {
		ext_cfg = get_dle_mem_cfg(adapter, ext_mode);
		if (!ext_cfg) {
			ret = MACNOITEM;
			PLTFM_MSG_ERR("[ERR]get_dle_ext_mem_cfg %d\n",
				      ext_mode);
			goto error;
		}

		cfg->wde_min_qt->wcpu = ext_cfg->wde_min_qt->wcpu;
	}

	val32_1 = dle_used_size(cfg->wde_size, cfg->ple_size);
	val32_2 = adapter->hw_info->fifo_size - dle_rsvd_size(adapter, mode);
	if (val32_1 != val32_2) {
		PLTFM_MSG_ERR("[ERR]dle init used size %d not match %d\n",
			      val32_1, val32_2);
		ret = MACFFCFG;
		goto error;
	}

	ret = p_ops->dle_func_en(adapter, MAC_AX_FUNC_DIS);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle func disable %d\n", ret);
		goto error;
	}

	ret = p_ops->dle_clk_en(adapter, MAC_AX_FUNC_EN);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle clk enable %d\n", ret);
		goto error;
	}

	ret = p_ops->dle_mix_cfg(adapter, cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] dle mix cfg %d\n", ret);
		goto error;
	}

	ret = dle_quota_cfg(adapter, cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle qutoa cfg %d\n", ret);
		goto error;
	}

	ret = p_ops->dle_func_en(adapter, MAC_AX_FUNC_EN);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle func enable %d\n", ret);
		goto error;
	}

	ret = p_ops->chk_dle_rdy(adapter, DLE_CTRL_TYPE_WDE);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]check wde ready %d\n", ret);
		goto error;
	}

	ret = p_ops->chk_dle_rdy(adapter, DLE_CTRL_TYPE_PLE);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]check ple ready %d\n", ret);
		goto error;
	}

	return ret;
error:
	ret2 = p_ops->dle_func_en(adapter, MAC_AX_FUNC_DIS);
	if (ret2 != MACSUCCESS)
		PLTFM_MSG_ERR("[ERR]dle func disable %d\n", ret2);

	return ret;
}

u32 is_qta_dbcc(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
		u8 *is_dbcc)
{
	struct dle_mem_t *cfg;

	cfg = get_dle_mem_cfg(adapter, mode);
	if (!cfg) {
		PLTFM_MSG_ERR("[ERR]get_dle_mem_cfg\n");
		return MACNOITEM;
	}

	*is_dbcc = (cfg->ple_min_qt->cma1_dma && cfg->ple_max_qt->cma1_dma) ?
		   1 : 0;

	return MACSUCCESS;
}

u8 is_curr_dbcc(struct mac_ax_adapter *adapter)
{
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B))
		return 0;

	return adapter->dle_info.c1_rx_qta ? 1 : 0;
}

u32 is_qta_poh(struct mac_ax_adapter *adapter, enum mac_ax_qta_mode mode,
	       u8 *is_poh)
{
	*is_poh = (mode == MAC_AX_QTA_SCC_STF || mode == MAC_AX_QTA_DBCC_STF ||
		   adapter->hw_info->intf != MAC_AX_INTF_PCIE) ? 0 : 1;

	return MACSUCCESS;
}

u32 redu_wowlan_rx_qta(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u16 new_qta, qta_min, qta_max, rdu_pg_num;

	if (!(is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)))
		return MACSUCCESS;

	val32 = GET_FIELD(MAC_REG_R32(R_AX_PLE_PKTBUF_CFG), B_AX_PLE_PAGE_SEL);
	if (val32 == S_AX_PLE_PAGE_SEL_128) {
		rdu_pg_num = PLE_QTA_PG128B_12KB;
	} else if (val32 == S_AX_PLE_PAGE_SEL_256) {
		rdu_pg_num = PLE_QTA_PG128B_12KB / 2;
	} else {
		PLTFM_MSG_ERR("[ERR]PLE page sel %d unsupport\n", val32);
		return MACHWERR;
	}

	val32 = MAC_REG_R32(R_AX_PLE_QTA6_CFG);
	if (!val32) {
		PLTFM_MSG_ERR("[ERR]no rx 0 qta\n");
		return MACHWERR;
	}
	qta_min = GET_FIELD(val32, B_AX_PLE_Q6_MIN_SIZE);
	qta_max = GET_FIELD(val32, B_AX_PLE_Q6_MAX_SIZE);

	adapter->dle_info.c0_ori_max = qta_max;

	new_qta = (qta_max - qta_min) < rdu_pg_num ?
		  qta_min : (qta_max - rdu_pg_num);
	val32 = SET_CLR_WORD(val32, new_qta, B_AX_PLE_Q6_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA6_CFG, val32);

	val32 = MAC_REG_R32(R_AX_PLE_QTA7_CFG);
	if (!val32)
		return MACSUCCESS;
	qta_min = GET_FIELD(val32, B_AX_PLE_Q7_MIN_SIZE);
	qta_max = GET_FIELD(val32, B_AX_PLE_Q7_MAX_SIZE);

	adapter->dle_info.c1_ori_max = qta_max;

	new_qta = (qta_max - qta_min) < rdu_pg_num ?
		  qta_min : (qta_max - rdu_pg_num);
	val32 = SET_CLR_WORD(val32, new_qta, B_AX_PLE_Q7_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA7_CFG, val32);
#endif
	return MACSUCCESS;
}

u32 restr_wowlan_rx_qta(struct mac_ax_adapter *adapter)
{
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, w_val32;

	if (!(is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	      is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)))
		return MACSUCCESS;

	val32 = MAC_REG_R32(R_AX_PLE_QTA6_CFG);
	if (!val32) {
		PLTFM_MSG_ERR("[ERR]no rx 0 qta\n");
		return MACHWERR;
	}
	w_val32 = SET_CLR_WORD(val32, adapter->dle_info.c0_ori_max,
			       B_AX_PLE_Q6_MAX_SIZE);
	if (w_val32 != val32)
		MAC_REG_W32(R_AX_PLE_QTA6_CFG, w_val32);

	val32 = MAC_REG_R32(R_AX_PLE_QTA7_CFG);
	if (!val32)
		return MACSUCCESS;
	w_val32 = SET_CLR_WORD(val32, adapter->dle_info.c1_ori_max,
			       B_AX_PLE_Q7_MAX_SIZE);
	if (w_val32 != val32)
		MAC_REG_W32(R_AX_PLE_QTA7_CFG, w_val32);
#endif
	return MACSUCCESS;
}

static u32 h2c_dle_qta_change(struct mac_ax_adapter *adapter)
{
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	u32 ret;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, 0);
	if (!buf) {
		ret = MACNOBUF;
		goto dle_qta_change_fail;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_DLE_QTA_CHANGE,
			      0,
			      0);

	if (ret != MACSUCCESS)
		goto dle_qta_change_fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto dle_qta_change_fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret)
		goto dle_qta_change_fail;

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;

dle_qta_change_fail:
	h2cb_free(adapter, h2cb);
	return ret;
}
