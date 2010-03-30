
#define IFX_CACHE_EXTRA_INVALID_TAG						\
	mtc0	zero, CP0_TAGLO, 1;						\
	mtc0	zero, CP0_TAGLO, 2;						\
	mtc0	zero, CP0_TAGLO, 3;						\
	mtc0	zero, CP0_TAGLO, 4;

#define IFX_CACHE_EXTRA_OPERATION						\
	/* set WST bit */							\
	mfc0	a0, CP0_ECC;							\
	li	a1, ECCF_WST;							\
	or	a0, a1;								\
	mtc0	a0, CP0_ECC;							\
										\
	li	a0, K0BASE;							\
	move	a2, t2;		/* icacheSize */				\
	move	a3, t4;		/* icacheLineSize */				\
	move	a1, a2;								\
	icacheop(a0,a1,a2,a3,(Index_Store_Tag_I));				\
										\
	/* clear WST bit */							\
	mfc0	a0, CP0_ECC;							\
	li	a1, ~ECCF_WST;							\
	and	a0, a1;								\
	mtc0	a0, CP0_ECC;							\
										\
	/* 1: initialise dcache tags. */					\
										\
	/* cache line size */							\
	li	a2, CFG_CACHELINE_SIZE;						\
	/* kseg0 mem address */							\
	li	a1, 0;								\
	li	a3, CFG_CACHE_SETS * CFG_CACHE_WAYS;				\
1:										\
	/* store tag (invalid, not locked) */					\
	cache 0x8, 0(a1);							\
	cache 0x9, 0(a1);							\
										\
	add	a3, -1;								\
	bne	a3, zero, 1b;							\
	add	a1, a2;								\
										\
	/* set WST bit */							\
	mfc0	a0, CP0_ECC;							\
	li	a1, ECCF_WST;							\
	or	a0, a1;								\
	mtc0	a0, CP0_ECC;							\
										\
	li	a0, K0BASE;							\
	move	a2, t3;		/* dcacheSize */				\
	move	a3, t5;		/* dcacheLineSize */				\
	move	a1, a2;								\
	icacheop(a0,a1,a2,a3,(Index_Store_Tag_D));				\
										\
	/* clear WST bit */							\
	mfc0	a0, CP0_ECC;							\
	li	a1, ~ECCF_WST;							\
	and	a0, a1;								\
	mtc0	a0, CP0_ECC;

