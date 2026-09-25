/*
 * Function: bcm3384_zephyr_l1_init_uncached
 * Arguments: None
 * Returns: 0
 * Description: BCM3384 direct-Zephyr boot enters Linux without the stock
 *              firmware BMIPS5000 cache setup. Call this helper after
 *              switching the current execution path to uncached KSEG0. The
 *              helper then follows the stock BMIPS5000 l1_init ordering: enter
 *              the KSEG1 alias, set KSEG0 to write-back, initialize L1/L2 cache
 *              tags, and return through the cached KSEG0 alias.
 *              Secondary threads only enable ICE/DCE; they must not perform
 *              index tag clears while the boot thread is already running from
 *              cached memory.
 * Trashes: a0, v0, v1, t0, t1, t2, t8
 */
#define CP0_CONFIG2		$16, 2
#define CP0_D_SEC_CACHE_DATA_HI	$29, 3
#define CP0_CONFIG2_L2B_MASK	(1 << 12)
#define CP0_ZSC_STG_SEL_LRU	0x01000000
#define CP0_ZSC_STG_SEL_DIR	0x02000000
#define CP0_ZSC_MAX_ADDR	0x00040000
#define CP0_ZSC_MAX_ADDR_BY_INDEX	0x00008000
#define BCM3384_BRCM_CONFIG_TP1	(1 << 24)
#define BRCM_ZSC_CONFIG_LMB_RSP_BYP_EN	(1 << 28)
#define BRCM_ZSC_CONFIG_PF_DBL_EN	(1 << 27)
#define BRCM_ZSC_CONFIG_INST_PREFETCH_EN	(1 << 26)
#define BRCM_ZSC_CONFIG_ISD_SHORT	(1 << 25)
#define BRCM_ZSC_CONFIG_DATA_PREFETCH_EN	(1 << 22)
#define BRCM_ZSC_CONFIG_DSD_SHORT	(1 << 21)
#define BRCM_ZSC_CONFIG_DPF_MRU_LIMIT_EN	(1 << 20)
#define BRCM_ZSC_CONFIG_CRB_BYPASS_EN	(1 << 19)
#define BRCM_ZSC_CONFIG_RELAX_ORDER_EN	(1 << 17)
#define BRCM_ZSC_CONFIG_VALUE \
	(BRCM_ZSC_CONFIG_INST_PREFETCH_EN | BRCM_ZSC_CONFIG_ISD_SHORT | \
	 BRCM_ZSC_CONFIG_DATA_PREFETCH_EN | BRCM_ZSC_CONFIG_DPF_MRU_LIMIT_EN | \
	 BRCM_ZSC_CONFIG_RELAX_ORDER_EN | BRCM_ZSC_CONFIG_LMB1En | \
	 BRCM_ZSC_CONFIG_LMB0En | BRCM_ZSC_CONFIG_LMB_RSP_BYP_EN | \
	 BRCM_ZSC_CONFIG_PF_DBL_EN | BRCM_ZSC_CONFIG_DSD_SHORT | \
	 BRCM_ZSC_CONFIG_CRB_BYPASS_EN)
#define BRCM_ZSC_REQ_BUFFER_VALUE	0x77707711
#define BRCM_ZSC_RBUS_MAPPING0_VALUE	0x8f000d00
#define BRCM_ZSC_RBUS_MAPPING1_VALUE	0x82000100
#define BRCM_ZSC_RBUS_MAPPING2_VALUE	0x0fff0fff
#define BRCM_ZSC_SCB0_MAPPING0_VALUE	0x81000000
#define BRCM_ZSC_SCB0_MAPPING1_VALUE	0x89000200
#define BRCM_ZSC_SCB1_MAPPING0_VALUE	0x8d000900
#define BRCM_ZSC_SCB1_MAPPING1_VALUE	0x0fff0fff
	.globl	bcm3384_zephyr_l1_init_uncached
	.ent	bcm3384_zephyr_l1_init_uncached
	.set	noreorder
bcm3384_zephyr_l1_init_uncached:
	move	t8, ra

	/* Initialize I/D cache tag and data CP0 staging registers. */
	mtc0	zero, CP0_ICACHE_TAG_LO
	mtc0	zero, CP0_ICACHE_TAG_HI
	mtc0	zero, CP0_ICACHE_DATA_LO
	mtc0	zero, CP0_ICACHE_DATA_HI
	mtc0	zero, CP0_DCACHE_TAG_LO
	mtc0	zero, CP0_DCACHE_TAG_HI

	/*
	 * Cache index operations are ignored while ICE/DCE are disabled, but
	 * this function is executing from the uncached KSEG1 alias.
	 */
	jal	enable_ID
	nop

	mfc0	t0, CP0_BRCM_CONFIG0
	li	t1, BCM3384_BRCM_CONFIG_TP1
	and	t0, t0, t1
	bnez	t0, 2f
	nop

	jal	size_i_cache
	nop

	/* Run uncached in KSEG1 before changing the KSEG0 cache mode. */
	la	t0, 1f
	lui	t1, 0x2000
	or	t0, t1, t0
	jr	t0
	nop

1:
	mfc0	t0, CP0_CONFIG
	and	t0, t0, ~CP0_CONFIG_K0_MASK
	or	t0, t0, 3
	mtc0	t0, CP0_CONFIG
	nop
	nop
	nop

	li	a0, KSEG0
	cacheop(a0, v0, v1, Index_Store_Tag_I)

	jal	size_d_cache
	nop

	li	a0, KSEG0
	cacheop(a0, v0, v1, Index_Store_Tag_D)
	/* Enable L2/S-cache by clearing Config2.L2B. */
	mfc0	t0, CP0_CONFIG2
	li	t1, ~CP0_CONFIG2_L2B_MASK
	and	t0, t0, t1
	mtc0	t0, CP0_CONFIG2
	nop

	/* Initialize L2 tag state by index and way. */
	mtc0	zero, CP0_D_SEC_CACHE_DATA_HI
	mtc0	zero, CP0_D_SEC_CACHE_DATA_LO
	li	a0, KSEG0
	li	a1, CP0_ZSC_MAX_ADDR
	li	a2, 0x80
	cacheop(a0, a1, a2, Index_Store_Tag_SD)

	/* Initialize L2 LRU state by index. */
	mtc0	zero, CP0_D_SEC_CACHE_DATA_HI
	li	t0, 0xfac688
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	a0, KSEG0
	li	a1, CP0_ZSC_STG_SEL_LRU
	or	a0, a0, a1
	li	a1, CP0_ZSC_MAX_ADDR_BY_INDEX
	li	a2, 0x80
	cacheop(a0, a1, a2, Index_Store_Tag_SD)

	/* Initialize L2 directory by index and way. */
	mtc0	zero, CP0_D_SEC_CACHE_DATA_HI
	mtc0	zero, CP0_D_SEC_CACHE_DATA_LO
	li	a0, KSEG0
	li	a1, CP0_ZSC_STG_SEL_DIR
	or	a0, a0, a1
	li	a1, CP0_ZSC_MAX_ADDR_BY_INDEX
	li	a2, 0x80
	cacheop(a0, a1, a2, Index_Store_Tag_SD)

	/* Configure L2 attributes, request credits, and address maps. */
	li	t0, BRCM_ZSC_CONFIG_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_CONFIG_REG
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_REQ_BUFFER_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_REQ_BUFFER_REG
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_RBUS_MAPPING0_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_RBUS_ADDR_MAPPING_REG0
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_RBUS_MAPPING1_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_RBUS_ADDR_MAPPING_REG1
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_RBUS_MAPPING2_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_RBUS_ADDR_MAPPING_REG2
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_SCB0_MAPPING0_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_SCB0_ADDR_MAPPING_REG0
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_SCB0_MAPPING1_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_SCB0_ADDR_MAPPING_REG1
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_SCB1_MAPPING0_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_SCB1_ADDR_MAPPING_REG0
	sync
	cache	0xb, 0(t0)
	sync
	li	t0, BRCM_ZSC_SCB1_MAPPING1_VALUE
	mtc0	t0, CP0_D_SEC_CACHE_DATA_LO
	li	t0, 0x90000000 | BRCM_ZSC_ALL_REGS_SELECT | BRCM_ZSC_SCB1_ADDR_MAPPING_REG1
	sync
	cache	0xb, 0(t0)
	sync
	jal	set_zephyr
	nop
	li	a0, 1
	jal	set_llmb
	nop
	jal	set_branch_pred
	nop
	jal	set_luc
	nop
	jal	set_cwf_tse
	nop
	move	a0, zero
	jal	set_clock_ratio
	nop
	jal	set_other_config
	nop
	jal	clear_jump_target_buffer
	nop

2:
	/* Return from KSEG1 to the cached KSEG0 alias after cache init. */
	la	t0, 3f
	lui	t1, 0x2000
	or	t0, t1, t0
	xor	t0, t1, t0
	jr	t0
	nop

3:
	move	v0, zero
	mtc0	zero, CP0_CAUSE
	sync
	ehb
	jr.hb	t8
	nop

	.end	bcm3384_zephyr_l1_init_uncached
	.set	reorder
