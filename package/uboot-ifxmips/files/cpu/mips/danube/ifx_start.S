/*
 * IFX Platform Dependent CPU Initializations
 * - for Danube
 */

#define IFX_EBU_BOOTCFG_DWORD							\
	.word INFINEON_EBU_BOOTCFG; /* EBU init code, fetched during booting */	\
	.word 0x00000000;           /* phases of the flash */

#define IFX_MORE_RESERVED_VECTORS						\
	XVECENT(romExcHandle,0x400);	/* Int, CauseIV=1 */			\
	RVECENT(romReserved,129);						\
	RVECENT(romReserved,130);						\
	RVECENT(romReserved,131);						\
	RVECENT(romReserved,132);						\
	RVECENT(romReserved,133);						\
	RVECENT(romReserved,134);						\
	RVECENT(romReserved,135);						\
	RVECENT(romReserved,136);						\
	RVECENT(romReserved,137);						\
	RVECENT(romReserved,138);						\
	RVECENT(romReserved,139);						\
	RVECENT(romReserved,140);						\
	RVECENT(romReserved,141);						\
	RVECENT(romReserved,142);						\
	RVECENT(romReserved,143);						\
	RVECENT(romExcHandle,0x480);	/* EJTAG debug exception */

#define IFX_RESET_PRECHECK							\
	mfc0	k0, CP0_EBASE;							\
	and	k0, EBASEF_CPUNUM;						\
	bne	k0, zero, ifx_mips_handler_1;					\
	nop;

#define IFX_CPU_EXTRA_INIT							\
	mfc0	k0, CP0_CONFIG, 7;						\
	li	k1, 0x04;							\
	or	k0, k1;								\
	mtc0	k0, CP0_CONFIG, 7;

#define IFX_CACHE_OPER_MODE							\
	li	t0, CONF_CM_CACHABLE_NO_WA;

/*
 * Stop VCPU
 */
#define IFX_MIPS_HANDLER_1							\
	wait;									\
	b ifx_mips_handler_1;							\
	nop;

