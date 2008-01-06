/*
 * Misc utility routines for accessing PMU corerev specific features
 * of the SiliconBackplane-based Broadcom chips.
 *
 * Copyright 2007, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 * $Id$
 */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <sbutils.h>
#include <bcmdevs.h>
#include <sbconfig.h>
#include <sbchipc.h>
#include <hndpmu.h>

/* debug/trace */
#define	PMU_ERROR(args)

#ifdef BCMDBG
#define	PMU_MSG(args)	printf args
#else
#define	PMU_MSG(args)
#endif /* BCMDBG */

/* PMU & control */
/* PMU rev 0 pll control for BCM4328 and BCM5354 */
static void sb_pmu0_pllinit0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc,
			      uint32 xtal);
static uint32 sb_pmu0_alpclk0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc);
static uint32 sb_pmu0_cpuclk0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc);
/* PMU rev 0 pll control for BCM4325 BCM4329 */
static void sb_pmu1_pllinit0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc,
			      uint32 xtal);
static uint32 sb_pmu1_cpuclk0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc);
static uint32 sb_pmu1_alpclk0 (sb_t * sbh, osl_t * osh, chipcregs_t * cc);

/* Setup switcher voltage */
void
BCMINITFN (sb_pmu_set_switcher_voltage) (sb_t * sbh, osl_t * osh,
					 uint8 bb_voltage, uint8 rf_voltage)
{
  chipcregs_t *cc;
  uint origidx;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  W_REG (osh, &cc->regcontrol_addr, 0x01);
  W_REG (osh, &cc->regcontrol_data, (uint32) (bb_voltage & 0x1f) << 22);

  W_REG (osh, &cc->regcontrol_addr, 0x00);
  W_REG (osh, &cc->regcontrol_data, (uint32) (rf_voltage & 0x1f) << 14);

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}

void
sb_pmu_set_ldo_voltage (sb_t * sbh, osl_t * osh, uint8 ldo, uint8 voltage)
{
  uint8 sr_cntl_shift, rc_shift, shift, mask;
  uint32 addr;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
    case BCM5354_CHIP_ID:
      switch (ldo)
	{
	case SET_LDO_VOLTAGE_LDO1:
	  addr = 2;
	  sr_cntl_shift = 8;
	  rc_shift = 17;
	  mask = 0xf;
	  break;
	case SET_LDO_VOLTAGE_LDO2:
	  addr = 3;
	  sr_cntl_shift = 0;
	  rc_shift = 1;
	  mask = 0xf;
	  break;
	case SET_LDO_VOLTAGE_LDO3:
	  addr = 3;
	  sr_cntl_shift = 0;
	  rc_shift = 9;
	  mask = 0xf;
	  break;
	case SET_LDO_VOLTAGE_PAREF:
	  addr = 3;
	  sr_cntl_shift = 0;
	  rc_shift = 17;
	  mask = 0x3f;
	  break;
	default:
	  ASSERT (FALSE);
	  return;
	}
      break;
    case BCM4312_CHIP_ID:
      switch (ldo)
	{
	case SET_LDO_VOLTAGE_PAREF:
	  addr = 0;
	  sr_cntl_shift = 0;
	  rc_shift = 21;
	  mask = 0x3f;
	  break;
	default:
	  ASSERT (FALSE);
	  return;
	}
      break;
    default:
      ASSERT (FALSE);
      return;
    }

  shift = sr_cntl_shift + rc_shift;

  sb_corereg (sbh, SB_CC_IDX, OFFSETOF (chipcregs_t, regcontrol_addr),
	      ~0, addr);
  sb_corereg (sbh, SB_CC_IDX, OFFSETOF (chipcregs_t, regcontrol_data),
	      mask << shift, (voltage & mask) << shift);
}

void
sb_pmu_paref_ldo_enable (sb_t * sbh, osl_t * osh, bool enable)
{
  uint ldo = 0;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      ldo = RES4328_PA_REF_LDO;
      break;
    case BCM5354_CHIP_ID:
      ldo = RES5354_PA_REF_LDO;
      break;
    case BCM4312_CHIP_ID:
      ldo = RES4312_PA_REF_LDO;
      break;
    default:
      return;
    }

  sb_corereg (sbh, SB_CC_IDX, OFFSETOF (chipcregs_t, min_res_mask),
	      PMURES_BIT (ldo), enable ? PMURES_BIT (ldo) : 0);
}

uint16 BCMINITFN (sb_pmu_fast_pwrup_delay) (sb_t * sbh, osl_t * osh)
{
  uint16 delay = PMU_MAX_TRANSITION_DLY;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      delay = 7000;
      break;

    case BCM4325_CHIP_ID:
    case BCM4312_CHIP_ID:
#ifdef BCMQT
      delay = 70;
#else
      delay = 2800;
#endif
      break;

    default:
      PMU_MSG (("No PMU fast power up delay specified "
		"for chip %x rev %d, using default %d us\n",
		sbh->chip, sbh->chiprev, delay));
      break;
    }

  return delay;
}

uint32 BCMINITFN (sb_pmu_force_ilp) (sb_t * sbh, osl_t * osh, bool force)
{
  chipcregs_t *cc;
  uint origidx;
  uint32 oldpmucontrol;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  oldpmucontrol = R_REG (osh, &cc->pmucontrol);
  if (force)
    W_REG (osh, &cc->pmucontrol, oldpmucontrol &
	   ~(PCTL_HT_REQ_EN | PCTL_ALP_REQ_EN));
  else
    W_REG (osh, &cc->pmucontrol, oldpmucontrol |
	   (PCTL_HT_REQ_EN | PCTL_ALP_REQ_EN));

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);

  return oldpmucontrol;
}

/* Setup min/max resources and up/down timers */
typedef struct
{
  uint8 resnum;
  uint16 updown;
} pmu_res_updown_t;

typedef struct
{
  uint8 resnum;
  int8 action;			/* 0 - set, 1 - add, -1 - remove */
  uint32 depend_mask;
} pmu_res_depend_t;

static const pmu_res_updown_t
BCMINITDATA (bcm4328a0_res_updown)[] =
{
  {
  RES4328_EXT_SWITCHER_PWM, 0x0101},
  {
  RES4328_BB_SWITCHER_PWM, 0x1f01},
  {
  RES4328_BB_SWITCHER_BURST, 0x010f},
  {
  RES4328_BB_EXT_SWITCHER_BURST, 0x0101},
  {
  RES4328_ILP_REQUEST, 0x0202},
  {
  RES4328_RADIO_SWITCHER_PWM, 0x0f01},
  {
  RES4328_RADIO_SWITCHER_BURST, 0x0f01},
  {
  RES4328_ROM_SWITCH, 0x0101},
  {
  RES4328_PA_REF_LDO, 0x0f01},
  {
  RES4328_RADIO_LDO, 0x0f01},
  {
  RES4328_AFE_LDO, 0x0f01},
  {
  RES4328_PLL_LDO, 0x0f01},
  {
  RES4328_BG_FILTBYP, 0x0101},
  {
  RES4328_TX_FILTBYP, 0x0101},
  {
  RES4328_RX_FILTBYP, 0x0101},
  {
  RES4328_XTAL_PU, 0x0101},
  {
  RES4328_XTAL_EN, 0xa001},
  {
  RES4328_BB_PLL_FILTBYP, 0x0101},
  {
  RES4328_RF_PLL_FILTBYP, 0x0101},
  {
  RES4328_BB_PLL_PU, 0x0701}
};

static const pmu_res_depend_t
BCMINITDATA (bcm4328a0_res_depend)[] =
{
  /* Adjust ILP request resource not to force ext/BB switchers into burst mode */
  {
  RES4328_ILP_REQUEST, 0,
      PMURES_BIT (RES4328_EXT_SWITCHER_PWM) |
      PMURES_BIT (RES4328_BB_SWITCHER_PWM)}
};

#ifdef BCMQT			/* for power save on slow QT/small beacon interval */
static const pmu_res_updown_t
BCMINITDATA (bcm4325a0_res_updown_qt)[] =
{
  {
  RES4325_HT_AVAIL, 0x0300},
  {
  RES4325_BBPLL_PWRSW_PU, 0x0101},
  {
  RES4325_RFPLL_PWRSW_PU, 0x0101},
  {
  RES4325_ALP_AVAIL, 0x0100},
  {
  RES4325_XTAL_PU, 0x1000},
  {
  RES4325_LNLDO1_PU, 0x0800},
  {
  RES4325_CLDO_CBUCK_PWM, 0x0101},
  {
  RES4325_CBUCK_PWM, 0x0803}
};
#else
static const pmu_res_updown_t
BCMINITDATA (bcm4325a0_res_updown)[] =
{
  {
  RES4325_XTAL_PU, 0x1501}
};
#endif /* !BCMQT */

static const pmu_res_depend_t
BCMINITDATA (bcm4325a0_res_depend)[] =
{
  /* Adjust HT Avail resource dependencies */
  {
  RES4325_HT_AVAIL, 1,
      PMURES_BIT (RES4325_RX_PWRSW_PU) | PMURES_BIT (RES4325_TX_PWRSW_PU) |
      PMURES_BIT (RES4325_LOGEN_PWRSW_PU) | PMURES_BIT (RES4325_AFE_PWRSW_PU)}
};

void BCMINITFN (sb_pmu_res_init) (sb_t * sbh, osl_t * osh)
{
  chipcregs_t *cc;
  uint origidx;
  const pmu_res_updown_t *pmu_res_updown_table = NULL;
  int pmu_res_updown_table_sz = 0;
  const pmu_res_depend_t *pmu_res_depend_table = NULL;
  int pmu_res_depend_table_sz = 0;
  uint32 min_mask = 0, max_mask = 0;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      /* Down to ILP request excluding ROM */
      min_mask = PMURES_BIT (RES4328_EXT_SWITCHER_PWM) |
	PMURES_BIT (RES4328_BB_SWITCHER_PWM) | PMURES_BIT (RES4328_XTAL_EN);
#ifdef BCMROMOFFLOAD
      /* Including ROM */
      min_mask |= PMURES_BIT (RES4328_ROM_SWITCH);
#endif
      /* Allow (but don't require) PLL to turn on */
      max_mask = 0xfffff;
      pmu_res_updown_table = bcm4328a0_res_updown;
      pmu_res_updown_table_sz = ARRAYSIZE (bcm4328a0_res_updown);
      pmu_res_depend_table = bcm4328a0_res_depend;
      pmu_res_depend_table_sz = ARRAYSIZE (bcm4328a0_res_depend);
      break;
    case BCM4312_CHIP_ID:
      /* keep default
       * min_mask = 0xcbb; max_mask = 0x7ffff;
       * pmu_res_updown_table_sz = 0;
       * pmu_res_depend_table_sz = 0;
       */
      break;
    case BCM5354_CHIP_ID:
      /* Allow (but don't require) PLL to turn on */
      max_mask = 0xfffff;
      break;

    case BCM4325_CHIP_ID:
      /* Leave OTP powered up and power it down later. */
      min_mask =
	PMURES_BIT (RES4325_CBUCK_BURST) | PMURES_BIT (RES4325_LNLDO2_PU);
      if (((sbh->chipst & CST4325_PMUTOP_2B_MASK) >>
	   CST4325_PMUTOP_2B_SHIFT) == 1)
	min_mask |= PMURES_BIT (RES4325_CLDO_CBUCK_BURST);
      /* Allow (but don't require) PLL to turn on */
      max_mask = 0x3fffff;
#ifdef BCMQT
      pmu_res_updown_table = bcm4325a0_res_updown_qt;
      pmu_res_updown_table_sz = ARRAYSIZE (bcm4325a0_res_updown_qt);
#else
      pmu_res_updown_table = bcm4325a0_res_updown;
      pmu_res_updown_table_sz = ARRAYSIZE (bcm4325a0_res_updown);
      pmu_res_depend_table = bcm4325a0_res_depend;
      pmu_res_depend_table_sz = ARRAYSIZE (bcm4325a0_res_depend);
#endif
      break;

    default:
      break;
    }

  /* Program up/down timers */
  while (pmu_res_updown_table_sz--)
    {
      ASSERT (pmu_res_updown_table);
      W_REG (osh, &cc->res_table_sel,
	     pmu_res_updown_table[pmu_res_updown_table_sz].resnum);
      W_REG (osh, &cc->res_updn_timer,
	     pmu_res_updown_table[pmu_res_updown_table_sz].updown);
    }

  /* Program resource dependencies table */
  while (pmu_res_depend_table_sz--)
    {
      ASSERT (pmu_res_depend_table);
      W_REG (osh, &cc->res_table_sel,
	     pmu_res_depend_table[pmu_res_depend_table_sz].resnum);
      switch (pmu_res_depend_table[pmu_res_depend_table_sz].action)
	{
	case 0:
	  W_REG (osh, &cc->res_dep_mask,
		 pmu_res_depend_table[pmu_res_depend_table_sz].depend_mask);
	  break;
	case 1:
	  OR_REG (osh, &cc->res_dep_mask,
		  pmu_res_depend_table[pmu_res_depend_table_sz].depend_mask);
	  break;
	case -1:
	  AND_REG (osh, &cc->res_dep_mask,
		   ~pmu_res_depend_table[pmu_res_depend_table_sz].
		   depend_mask);
	  break;
	default:
	  ASSERT (0);
	  break;
	}
    }

  /* program min resource mask */
  if (min_mask)
    {
      PMU_MSG (("Changing min_res_mask to 0x%x\n", min_mask));
      W_REG (osh, &cc->min_res_mask, min_mask);
    }
  /* program max resource mask */
  if (max_mask)
    {
      PMU_MSG (("Changing max_res_mask to 0x%x\n", max_mask));
      W_REG (osh, &cc->max_res_mask, max_mask);
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}

/* setup pll and query clock speed */
typedef struct
{
  uint16 freq;
  uint8 xf;
  uint8 wbint;
  uint32 wbfrac;
} pmu0_xtaltab0_t;

/* the following table is based on 880Mhz Fvco */
#define PMU0_PLL0_FVCO	880000	/* Fvco 880Mhz */
static const pmu0_xtaltab0_t
BCMINITDATA (pmu0_xtaltab0)[] =
{
  {
  12000, 1, 73, 349525},
  {
  13000, 2, 67, 725937},
  {
  14400, 3, 61, 116508},
  {
  15360, 4, 57, 305834},
  {
  16200, 5, 54, 336579},
  {
  16800, 6, 52, 399457},
  {
  19200, 7, 45, 873813},
  {
  19800, 8, 44, 466033},
  {
  20000, 9, 44, 0},
  {
  25000, 10, 70, 419430},
  {
  26000, 11, 67, 725937},
  {
  30000, 12, 58, 699050},
  {
  38400, 13, 45, 873813},
  {
  40000, 14, 45, 0},
  {
  0, 0, 0, 0}
};

#ifdef BCMUSBDEV
#define	PMU0_XTAL0_DEFAULT	11
#else
#define PMU0_XTAL0_DEFAULT	8
#endif

#ifdef BCMUSBDEV
/*
 * Set new backplane PLL clock frequency
 */
static void BCMINITFN (sb_pmu0_sbclk4328) (sb_t * sbh, int freq)
{
  uint32 tmp, oldmax, oldmin, origidx;
  chipcregs_t *cc;

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  /* Set new backplane PLL clock */
  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL0);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  tmp &= ~(PMU0_PLL0_PC0_DIV_ARM_MASK);
  tmp |= freq << PMU0_PLL0_PC0_DIV_ARM_SHIFT;
  W_REG (osh, &cc->pllcontrol_data, tmp);

  /* Power cycle BB_PLL_PU by disabling/enabling it to take on new freq */
  /* Disable PLL */
  oldmin = R_REG (osh, &cc->min_res_mask);
  oldmax = R_REG (osh, &cc->max_res_mask);
  W_REG (osh, &cc->min_res_mask, oldmin & ~PMURES_BIT (RES4328_BB_PLL_PU));
  W_REG (osh, &cc->max_res_mask, oldmax & ~PMURES_BIT (RES4328_BB_PLL_PU));

  /* It takes over several hundred usec to re-enable the PLL since the
   * sequencer state machines run on ILP clock. Set delay at 450us to be safe.
   *
   * Be sure PLL is powered down first before re-enabling it.
   */

  OSL_DELAY (PLL_DELAY);
  SPINWAIT ((R_REG (osh, &cc->res_state) & PMURES_BIT (RES4328_BB_PLL_PU)),
	    PLL_DELAY * 3);

  if (R_REG (osh, &cc->res_state) & PMURES_BIT (RES4328_BB_PLL_PU))
    {
      /* If BB_PLL not powered down yet, new backplane PLL clock
       *  may not take effect.
       *
       * Still early during bootup so no serial output here.
       */
      PMU_ERROR (("Fatal: BB_PLL not power down yet!\n"));
      ASSERT (!
	      (R_REG (osh, &cc->res_state) & PMURES_BIT (RES4328_BB_PLL_PU)));
    }

  /* Enable PLL */
  W_REG (osh, &cc->max_res_mask, oldmax);

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}
#endif /* BCMUSBDEV */

/* Set up PLL registers in the PMU as per the crystal speed.
 * Uses xtalfreq variable, or passed-in default.
 */
static void
BCMINITFN (sb_pmu0_pllinit0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc,
			      uint32 xtal)
{
  uint32 tmp;
  const pmu0_xtaltab0_t *xt;

  if ((sb_chip (sbh) == BCM5354_CHIP_ID) && (xtal == 0))
    {
      /* 5354 has xtal freq of 25MHz */
      xtal = 25000;
    }

  /* Find the frequency in the table */
  for (xt = pmu0_xtaltab0; xt->freq; xt++)
    if (xt->freq == xtal)
      break;
  if (xt->freq == 0)
    xt = &pmu0_xtaltab0[PMU0_XTAL0_DEFAULT];

  PMU_MSG (("XTAL %d (%d)\n", xtal, xt->xf));

  /* Check current PLL state */
  tmp = (R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
    PCTL_XTALFREQ_SHIFT;
  if (tmp == xt->xf)
    {
      PMU_MSG (("PLL already programmed for %d.%d MHz\n",
		(xt->freq / 1000), (xt->freq % 1000)));

#ifdef BCMUSBDEV
      if (sbh->chip == BCM4328_CHIP_ID)
	sb_pmu0_sbclk4328 (sbh, PMU0_PLL0_PC0_DIV_ARM_88MHZ);
#endif
      return;
    }

  if (tmp)
    {
      PMU_MSG (("Reprogramming PLL for %d.%d MHz (was %d.%dMHz)\n",
		(xt->freq / 1000), (xt->freq % 1000),
		(pmu0_xtaltab0[tmp - 1].freq / 1000),
		(pmu0_xtaltab0[tmp - 1].freq % 1000)));
    }
  else
    {
      PMU_MSG (("Programming PLL for %d.%d MHz\n", (xt->freq / 1000),
		(xt->freq % 1000)));
    }

  /* Make sure the PLL is off */
  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      AND_REG (osh, &cc->min_res_mask, ~PMURES_BIT (RES4328_BB_PLL_PU));
      AND_REG (osh, &cc->max_res_mask, ~PMURES_BIT (RES4328_BB_PLL_PU));
      break;
    case BCM5354_CHIP_ID:
      AND_REG (osh, &cc->min_res_mask, ~PMURES_BIT (RES5354_BB_PLL_PU));
      AND_REG (osh, &cc->max_res_mask, ~PMURES_BIT (RES5354_BB_PLL_PU));
      break;
    default:
      ASSERT (0);
    }
  SPINWAIT (R_REG (osh, &cc->clk_ctl_st) & CCS0_HTAVAIL,
	    PMU_MAX_TRANSITION_DLY);
  ASSERT (!(R_REG (osh, &cc->clk_ctl_st) & CCS0_HTAVAIL));

  PMU_MSG (("Done masking\n"));

  /* Write PDIV in pllcontrol[0] */
  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL0);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  if (xt->freq >= PMU0_PLL0_PC0_PDIV_FREQ)
    tmp |= PMU0_PLL0_PC0_PDIV_MASK;
  else
    tmp &= ~PMU0_PLL0_PC0_PDIV_MASK;
  W_REG (osh, &cc->pllcontrol_data, tmp);

  /* Write WILD in pllcontrol[1] */
  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL1);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  tmp =
    ((tmp & ~(PMU0_PLL0_PC1_WILD_INT_MASK | PMU0_PLL0_PC1_WILD_FRAC_MASK)) |
     (((xt->
	wbint << PMU0_PLL0_PC1_WILD_INT_SHIFT) & PMU0_PLL0_PC1_WILD_INT_MASK)
      | ((xt->wbfrac << PMU0_PLL0_PC1_WILD_FRAC_SHIFT) &
	 PMU0_PLL0_PC1_WILD_FRAC_MASK)));
  if (xt->wbfrac == 0)
    tmp |= PMU0_PLL0_PC1_STOP_MOD;
  else
    tmp &= ~PMU0_PLL0_PC1_STOP_MOD;
  W_REG (osh, &cc->pllcontrol_data, tmp);

  /* Write WILD in pllcontrol[2] */
  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL2);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  tmp = ((tmp & ~PMU0_PLL0_PC2_WILD_INT_MASK) |
	 ((xt->wbint >> PMU0_PLL0_PC2_WILD_INT_SHIFT) &
	  PMU0_PLL0_PC2_WILD_INT_MASK));
  W_REG (osh, &cc->pllcontrol_data, tmp);

  PMU_MSG (("Done pll\n"));

  /* Write XtalFreq. Set the divisor also. */
  tmp = R_REG (osh, &cc->pmucontrol);
  tmp = ((tmp & ~PCTL_ILP_DIV_MASK) |
	 (((((xt->freq + 127) / 128) - 1) << PCTL_ILP_DIV_SHIFT) &
	  PCTL_ILP_DIV_MASK));
  tmp = ((tmp & ~PCTL_XTALFREQ_MASK) |
	 ((xt->xf << PCTL_XTALFREQ_SHIFT) & PCTL_XTALFREQ_MASK));
  W_REG (osh, &cc->pmucontrol, tmp);
}

static uint32
BCMINITFN (sb_pmu0_alpclk0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc)
{
  const pmu0_xtaltab0_t *xt;
  uint32 xf;

  /* Find the frequency in the table */
  xf = (R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
    PCTL_XTALFREQ_SHIFT;
  for (xt = pmu0_xtaltab0; xt->freq; xt++)
    if (xt->xf == xf)
      break;
  if (xt->freq == 0)
    xt = &pmu0_xtaltab0[PMU0_XTAL0_DEFAULT];

  return xt->freq * 1000;
}

static uint32
BCMINITFN (sb_pmu0_cpuclk0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc)
{
  const pmu0_xtaltab0_t *xt;
  uint32 xf, tmp, divarm;
#ifdef BCMDBG
  uint32 pdiv, wbint, wbfrac, fvco;
#endif

  if (sb_chip (sbh) == BCM5354_CHIP_ID)
    {
      /* 5354 gets sb clock of 120MHz from main pll */
      return 120000000;
    }

  /* Find the xtal frequency in the table */
  xf = (R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
    PCTL_XTALFREQ_SHIFT;
  for (xt = pmu0_xtaltab0; xt->freq; xt++)
    if (xt->xf == xf)
      break;
  if (xt->freq == 0)
    xt = &pmu0_xtaltab0[PMU0_XTAL0_DEFAULT];

  /* Read divarm from pllcontrol[0] */
  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL0);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  divarm = (tmp & PMU0_PLL0_PC0_DIV_ARM_MASK) >> PMU0_PLL0_PC0_DIV_ARM_SHIFT;

#ifdef BCMDBG
  /* Calculate Fvco based on xtal freq, pdiv, and wild */
  pdiv = tmp & PMU0_PLL0_PC0_PDIV_MASK;

  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL1);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  wbfrac =
    (tmp & PMU0_PLL0_PC1_WILD_FRAC_MASK) >> PMU0_PLL0_PC1_WILD_FRAC_SHIFT;
  wbint = (tmp & PMU0_PLL0_PC1_WILD_INT_MASK) >> PMU0_PLL0_PC1_WILD_INT_SHIFT;

  W_REG (osh, &cc->pllcontrol_addr, PMU0_PLL0_PLLCTL2);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  wbint +=
    (tmp & PMU0_PLL0_PC2_WILD_INT_MASK) << PMU0_PLL0_PC2_WILD_INT_SHIFT;

  fvco = (xt->freq * wbint) << 8;
  fvco += (xt->freq * (wbfrac >> 10)) >> 2;
  fvco += (xt->freq * (wbfrac & 0x3ff)) >> 10;
  fvco >>= 8;
  fvco >>= pdiv;
  fvco /= 1000;
  fvco *= 1000;

  PMU_MSG (("sb_pmu0_cpuclk0: wbint %u wbfrac %u fvco %u\n",
	    wbint, wbfrac, fvco));
  ASSERT (fvco == PMU0_PLL0_FVCO);
#endif /* BCMDBG */

  /* Return ARM/SB clock */
  return PMU0_PLL0_FVCO / (divarm + PMU0_PLL0_PC0_DIV_ARM_BASE) * 1000;
}

/* PMU corerev 1 pll programming for BCM4325 */
/* setup pll and query clock speed */
typedef struct
{
  uint16 fref;
  uint8 xf;
  uint8 p1div;
  uint8 p2div;
  uint8 ndiv_int;
  uint32 ndiv_frac;
} pmu1_xtaltab0_t;

/* the following table is based on 880Mhz Fvco */
#define PMU1_PLL0_FVCO	880000	/* Fvco 880Mhz */
static const pmu1_xtaltab0_t
BCMINITDATA (pmu1_xtaltab0)[] =
{
  {
  12000, 1, 3, 22, 0x9, 0xFFFFEF},
  {
  13000, 2, 1, 6, 0xb, 0x483483},
  {
  14400, 3, 1, 10, 0xa, 0x1C71C7},
  {
  15360, 4, 1, 5, 0xb, 0x755555},
  {
  16200, 5, 1, 10, 0x5, 0x6E9E06},
  {
  16800, 6, 1, 10, 0x5, 0x3Cf3Cf},
  {
  19200, 7, 1, 9, 0x5, 0x17B425},
  {
  19800, 8, 1, 11, 0x4, 0xA57EB},
  {
  20000, 9, 1, 11, 0x4, 0x0},
  {
  24000, 10, 3, 11, 0xa, 0x0},
  {
  25000, 11, 5, 16, 0xb, 0x0},
  {
  26000, 12, 1, 2, 0x10, 0xEC4EC4},
  {
  30000, 13, 3, 8, 0xb, 0x0},
  {
  38400, 14, 1, 5, 0x4, 0x955555},
  {
  40000, 15, 1, 2, 0xb, 0},
  {
  0, 0, 0, 0, 0, 0}
};

/* Default to 15360Khz crystal */
#define PMU1_XTAL0_DEFAULT	3

static uint32
BCMINITFN (sb_pmu1_alpclk0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc)
{
  const pmu1_xtaltab0_t *xt;
  uint32 xf;

  /* Find the frequency in the table */
  xf = (R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
    PCTL_XTALFREQ_SHIFT;
  for (xt = pmu1_xtaltab0; xt->fref; xt++)
    if (xt->xf == xf)
      break;
  if (xt->fref == 0)
    xt = &pmu1_xtaltab0[PMU1_XTAL0_DEFAULT];

  return xt->fref * 1000;
}

/* Set up PLL registers in the PMU as per the crystal speed.
 * Uses xtalfreq variable, or passed-in default.
 */
static void
BCMINITFN (sb_pmu1_pllinit0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc,
			      uint32 xtal)
{
  const pmu1_xtaltab0_t *xt;
  uint32 tmp;
  uint32 buf_strength = 0;

  /* 4312: assume default works */
  if (sbh->chip == BCM4312_CHIP_ID)
    return;

  /* Find the frequency in the table */
  for (xt = pmu1_xtaltab0; xt->fref; xt++)
    if (xt->fref == xtal)
      break;
  if (xt->fref == 0)
    xt = &pmu1_xtaltab0[PMU1_XTAL0_DEFAULT];

  PMU_MSG (("XTAL %d (%d)\n", xtal, xt->xf));

  /* Check current PLL state */
  if (((R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
       PCTL_XTALFREQ_SHIFT) == xt->xf)
    {
      PMU_MSG (("PLL already programmed for %d.%d MHz\n",
		(xt->fref / 1000), (xt->fref % 1000)));
      return;
    }

  PMU_MSG (("Programming PLL for %d.%d MHz\n", (xt->fref / 1000),
	    (xt->fref % 1000)));

  /* Make sure the PLL is off */
  switch (sbh->chip)
    {
    case BCM4325_CHIP_ID:
      AND_REG (osh, &cc->min_res_mask,
	       ~(PMURES_BIT (RES4325_BBPLL_PWRSW_PU) |
		 PMURES_BIT (RES4325_HT_AVAIL)));
      AND_REG (osh, &cc->max_res_mask,
	       ~(PMURES_BIT (RES4325_BBPLL_PWRSW_PU) |
		 PMURES_BIT (RES4325_HT_AVAIL)));

      /* Change the BBPLL drive strength to 2 for all channels */
      buf_strength = 0x222222;
      break;
    default:
      ASSERT (0);
    }
  SPINWAIT (R_REG (osh, &cc->clk_ctl_st) & CCS_HTAVAIL,
	    PMU_MAX_TRANSITION_DLY);
  ASSERT (!(R_REG (osh, &cc->clk_ctl_st) & CCS_HTAVAIL));

  PMU_MSG (("Done masking\n"));

  /* Write p1div and p2div to pllcontrol[0] */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL0);
  tmp = R_REG (osh, &cc->pllcontrol_data) &
    ~(PMU1_PLL0_PC0_P1DIV_MASK | PMU1_PLL0_PC0_P2DIV_MASK);
  tmp |=
    ((xt->
      p1div << PMU1_PLL0_PC0_P1DIV_SHIFT) & PMU1_PLL0_PC0_P1DIV_MASK) | ((xt->
									  p2div
									  <<
									  PMU1_PLL0_PC0_P2DIV_SHIFT)
									 &
									 PMU1_PLL0_PC0_P2DIV_MASK);
  W_REG (osh, &cc->pllcontrol_data, tmp);

  /* Write ndiv_int and ndiv_mode to pllcontrol[2] */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL2);
  tmp = R_REG (osh, &cc->pllcontrol_data) &
    ~(PMU1_PLL0_PC2_NDIV_INT_MASK | PMU1_PLL0_PC2_NDIV_MODE_MASK);
  tmp |=
    ((xt->
      ndiv_int << PMU1_PLL0_PC2_NDIV_INT_SHIFT) & PMU1_PLL0_PC2_NDIV_INT_MASK)
    | ((1 << PMU1_PLL0_PC2_NDIV_MODE_SHIFT) & PMU1_PLL0_PC2_NDIV_MODE_MASK);
  W_REG (osh, &cc->pllcontrol_data, tmp);

  /* Write ndiv_frac to pllcontrol[3] */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL3);
  tmp = R_REG (osh, &cc->pllcontrol_data) & ~PMU1_PLL0_PC3_NDIV_FRAC_MASK;
  tmp |= ((xt->ndiv_frac << PMU1_PLL0_PC3_NDIV_FRAC_SHIFT) &
	  PMU1_PLL0_PC3_NDIV_FRAC_MASK);
  W_REG (osh, &cc->pllcontrol_data, tmp);

  if (buf_strength)
    {
      PMU_MSG (("Adjusting PLL buffer drive strength: %x\n", buf_strength));

      W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL5);
      tmp = R_REG (osh, &cc->pllcontrol_data) & ~PMU1_PLL0_PC5_CLK_DRV_MASK;
      tmp |= (buf_strength << PMU1_PLL0_PC5_CLK_DRV_SHIFT);
      W_REG (osh, &cc->pllcontrol_data, tmp);
    }

  PMU_MSG (("Done pll\n"));

  /* Write XtalFreq. Set the divisor also. */
  tmp = R_REG (osh, &cc->pmucontrol) &
    ~(PCTL_ILP_DIV_MASK | PCTL_XTALFREQ_MASK);
  tmp |= (((((xt->fref + 127) / 128) - 1) << PCTL_ILP_DIV_SHIFT) &
	  PCTL_ILP_DIV_MASK) |
    ((xt->xf << PCTL_XTALFREQ_SHIFT) & PCTL_XTALFREQ_MASK);
  W_REG (osh, &cc->pmucontrol, tmp);
}


static uint32
BCMINITFN (sb_pmu1_cpuclk0) (sb_t * sbh, osl_t * osh, chipcregs_t * cc)
{
  const pmu1_xtaltab0_t *xt;
  uint32 xf, tmp, m1div;
#ifdef BCMDBG
  uint32 ndiv_int, ndiv_frac, p2div, p1div, fvco;
#endif

  /* Find the xtal frequency in the table */
  xf = (R_REG (osh, &cc->pmucontrol) & PCTL_XTALFREQ_MASK) >>
    PCTL_XTALFREQ_SHIFT;
  for (xt = pmu1_xtaltab0; xt->fref; xt++)
    if (xt->xf == xf)
      break;
  if (xt->fref == 0)
    xt = &pmu1_xtaltab0[PMU1_XTAL0_DEFAULT];

  /* Read m1div from pllcontrol[1] */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL1);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  m1div = (tmp & PMU1_PLL0_PC1_M1DIV_MASK) >> PMU1_PLL0_PC1_M1DIV_SHIFT;

#ifdef BCMDBG
  /* Read p2div/p1div from pllcontrol[0] */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL0);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  p2div = (tmp & PMU1_PLL0_PC0_P2DIV_MASK) >> PMU1_PLL0_PC0_P2DIV_SHIFT;
  p1div = (tmp & PMU1_PLL0_PC0_P1DIV_MASK) >> PMU1_PLL0_PC0_P1DIV_SHIFT;

  /* Calculate Fvco based on xtal freq and ndiv and pdiv */
  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL2);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  ndiv_int =
    (tmp & PMU1_PLL0_PC2_NDIV_INT_MASK) >> PMU1_PLL0_PC2_NDIV_INT_SHIFT;

  W_REG (osh, &cc->pllcontrol_addr, PMU1_PLL0_PLLCTL3);
  tmp = R_REG (osh, &cc->pllcontrol_data);
  ndiv_frac =
    (tmp & PMU1_PLL0_PC3_NDIV_FRAC_MASK) >> PMU1_PLL0_PC3_NDIV_FRAC_SHIFT;

  fvco = (xt->fref * ndiv_int) << 8;
  fvco += (xt->fref * (ndiv_frac >> 12)) >> 4;
  fvco += (xt->fref * (ndiv_frac & 0xfff)) >> 12;
  fvco >>= 8;
  fvco *= p2div;
  fvco /= p1div;
  fvco /= 1000;
  fvco *= 1000;

  PMU_MSG (("sb_pmu0_cpuclk0: ndiv_int %u ndiv_frac %u "
	    "p2div %u p1div %u fvco %u\n",
	    ndiv_int, ndiv_frac, p2div, p1div, fvco));
  ASSERT (fvco == PMU1_PLL0_FVCO);
#endif /* BCMDBG */

  /* Return ARM/SB clock */
  return PMU1_PLL0_FVCO / m1div * 1000;
}

void BCMINITFN (sb_pmu_pll_init) (sb_t * sbh, osl_t * osh, uint xtalfreq)
{
  chipcregs_t *cc;
  uint origidx;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      sb_pmu0_pllinit0 (sbh, osh, cc, xtalfreq);
      break;
    case BCM5354_CHIP_ID:
      sb_pmu0_pllinit0 (sbh, osh, cc, xtalfreq);
      break;
    case BCM4325_CHIP_ID:
      sb_pmu1_pllinit0 (sbh, osh, cc, xtalfreq);
      break;
    case BCM4312_CHIP_ID:
      sb_pmu1_pllinit0 (sbh, osh, cc, xtalfreq);
      break;
    default:
      PMU_MSG (("No PLL init done for chip %x rev %d pmurev %d\n",
		sbh->chip, sbh->chiprev, sbh->pmurev));
      break;
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}

uint32 BCMINITFN (sb_pmu_alp_clock) (sb_t * sbh, osl_t * osh)
{
  chipcregs_t *cc;
  uint origidx;
  uint32 clock = ALP_CLOCK;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      clock = sb_pmu0_alpclk0 (sbh, osh, cc);
      break;
    case BCM5354_CHIP_ID:
      clock = sb_pmu0_alpclk0 (sbh, osh, cc);
      break;
    case BCM4325_CHIP_ID:
      clock = sb_pmu1_alpclk0 (sbh, osh, cc);
      break;
    case BCM4312_CHIP_ID:
      clock = sb_pmu1_alpclk0 (sbh, osh, cc);
      /* always 20Mhz */
      clock = 20000 * 1000;
      break;
    default:
      PMU_MSG (("No ALP clock specified "
		"for chip %x rev %d pmurev %d, using default %d Hz\n",
		sbh->chip, sbh->chiprev, sbh->pmurev, clock));
      break;
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
  return clock;
}

uint BCMINITFN (sb_pmu_cpu_clock) (sb_t * sbh, osl_t * osh)
{
  chipcregs_t *cc;
  uint origidx;
  uint32 clock = HT_CLOCK;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4328_CHIP_ID:
      clock = sb_pmu0_cpuclk0 (sbh, osh, cc);
      break;
    case BCM5354_CHIP_ID:
      clock = sb_pmu0_cpuclk0 (sbh, osh, cc);
      break;
    case BCM4325_CHIP_ID:
      clock = sb_pmu1_cpuclk0 (sbh, osh, cc);
      break;
    case BCM4312_CHIP_ID:
      clock = sb_pmu1_cpuclk0 (sbh, osh, cc);
      break;
    default:
      PMU_MSG (("No CPU clock specified "
		"for chip %x rev %d pmurev %d, using default %d Hz\n",
		sbh->chip, sbh->chiprev, sbh->pmurev, clock));
      break;
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
  return clock;
}

void BCMINITFN (sb_pmu_init) (sb_t * sbh, osl_t * osh)
{
  chipcregs_t *cc;
  uint origidx;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  if (sbh->pmurev >= 1)
    {
      if (sbh->chip == BCM4325_CHIP_ID && sbh->chiprev <= 1)
	AND_REG (osh, &cc->pmucontrol, ~PCTL_NOILP_ON_WAIT);
      else
	OR_REG (osh, &cc->pmucontrol, PCTL_NOILP_ON_WAIT);
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}

void BCMINITFN (sb_pmu_otp_power) (sb_t * sbh, osl_t * osh, bool on)
{
  chipcregs_t *cc;
  uint origidx;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4325_CHIP_ID:
      if (on)
	{
	  OR_REG (osh, &cc->min_res_mask, PMURES_BIT (RES4325_LNLDO2_PU));
	  if (sbh->boardflags & BFL_BUCKBOOST)
	    AND_REG (osh, &cc->min_res_mask,
		     ~PMURES_BIT (RES4325_BUCK_BOOST_PWM));
	  OSL_DELAY (500);
	}
      else
	{
	  if (sbh->boardflags & BFL_BUCKBOOST)
	    OR_REG (osh, &cc->min_res_mask,
		    PMURES_BIT (RES4325_BUCK_BOOST_PWM));
	  AND_REG (osh, &cc->min_res_mask, ~PMURES_BIT (RES4325_LNLDO2_PU));
	}
      break;
    default:
      break;
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}

void
sb_pmu_rcal (sb_t * sbh, osl_t * osh)
{
  chipcregs_t *cc;
  uint origidx;

  ASSERT (sbh->cccaps & CC_CAP_PMU);

  /* Remember original core before switch to chipc */
  origidx = sb_coreidx (sbh);
  cc = sb_setcore (sbh, SB_CC, 0);
  ASSERT (cc);

  switch (sbh->chip)
    {
    case BCM4325_CHIP_ID:
      {
	uint8 rcal_code;
	uint32 val;

	/* Kick RCal */
	W_REG (osh, &cc->chipcontrol_addr, 1);
	AND_REG (osh, &cc->chipcontrol_data, ~0x04);
	OR_REG (osh, &cc->chipcontrol_data, 0x04);

	/* Wait for completion */
	SPINWAIT (0 == (R_REG (osh, &cc->chipstatus) & 0x08),
		  10 * 1000 * 1000);
	ASSERT (R_REG (osh, &cc->chipstatus) & 0x08);

	/* Drop the LSB to convert from 5 bit code to 4 bit code */
	rcal_code = (uint8) (R_REG (osh, &cc->chipstatus) >> 5) & 0x0f;
	PMU_MSG (("RCal completed, status 0x%x, code 0x%x\n",
		  R_REG (osh, &cc->chipstatus), rcal_code));

	/* Write RCal code into pmu_vreg_ctrl[32:29] */
	W_REG (osh, &cc->regcontrol_addr, 0);
	val = R_REG (osh, &cc->regcontrol_data) & ~((uint32) 0x07 << 29);
	val |= (uint32) (rcal_code & 0x07) << 29;
	W_REG (osh, &cc->regcontrol_data, val);
	W_REG (osh, &cc->regcontrol_addr, 1);
	val = R_REG (osh, &cc->regcontrol_data) & ~(uint32) 0x01;
	val |= (uint32) ((rcal_code >> 3) & 0x01);
	W_REG (osh, &cc->regcontrol_data, val);

	/* Write RCal code into pmu_chip_ctrl[33:30] */
	W_REG (osh, &cc->chipcontrol_addr, 0);
	val = R_REG (osh, &cc->chipcontrol_data) & ~((uint32) 0x03 << 30);
	val |= (uint32) (rcal_code & 0x03) << 30;
	W_REG (osh, &cc->chipcontrol_data, val);
	W_REG (osh, &cc->chipcontrol_addr, 1);
	val = R_REG (osh, &cc->chipcontrol_data) & ~(uint32) 0x03;
	val |= (uint32) ((rcal_code >> 2) & 0x03);
	W_REG (osh, &cc->chipcontrol_data, val);

	/* Set override in pmu_chip_ctrl[29] */
	W_REG (osh, &cc->chipcontrol_addr, 0);
	OR_REG (osh, &cc->chipcontrol_data, (0x01 << 29));

	/* Power off RCal block */
	W_REG (osh, &cc->chipcontrol_addr, 1);
	AND_REG (osh, &cc->chipcontrol_data, ~0x04);

	break;
      }
    default:
      break;
    }

  /* Return to original core */
  sb_setcoreidx (sbh, origidx);
}
