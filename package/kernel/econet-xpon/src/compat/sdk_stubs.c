/*
 * sdk_stubs.c -- definitions for vendor-SDK symbols + the MTK sw-NAT hook
 * pointers.  All hwNAT hooks are NULL (offload disabled) so the driver's
 * `if (hook != NULL)` guards short-circuit.
 */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include "sdk_stubs.h"
#include <linux/foe_hook.h>

/* factory flash window base -- TODO EN7528: resolve from mtd/nvmem */
unsigned long flash_base;

/* ---- FE / QDMA globals (aliased by ecnt_hook_qdma_type.h) ----
 * storm_ctrl_shrehold / qdma_fwd_timer expand to *_wan; provided by the FE
 * (econet_eth) driver in the real system.  Stubbed here for the milestone. */
int storm_ctrl_shrehold_wan = 0;
int qdma_wan_fwd_timer = 0;

/* ---- EPON symbols referenced by the shared xpondrv proc handlers ----
 * EPON is dropped from this GPON-only build; these definitions satisfy the
 * references (the EPON "epon ..." debug sub-commands are effectively no-ops). */
__u8 eponStaticRptEnable = 0;
int  eponHwDygaspCtrl(__u8 flag) { (void)flag; return 0; }
void eponDetectPhyReady(void) { }

/* ---- misc platform globals the vendor SDK/board layer provided ---- */
int is_hwnat_dont_clean = 0;	/* hwNAT table-clean suppression flag        */
int masko_on_off        = 0;	/* MASKO (multicast) on/off debug toggle     */
int xpon_los_status     = 0;	/* optical LOS status mirror (set by PHY)    */

/* dbgtoMem(): vendor "log to ring buffer" debug sink -- no-op here. */
void dbgtoMem(__u32 debugLevel, char *fmt, ...) { (void)debugLevel; (void)fmt; }

/* gpon_dvt_pcp_check(): lives in gpon_dvt.c, which is intentionally NOT built
 * (DVT/self-test).  gpon_wan.c references it on the dvtPcpCheck debug path. */
int gpon_dvt_pcp_check(unsigned char mode, struct sk_buff *skb)
{
	(void)mode; (void)skb;
	return 0;
}

/* ---- optical-PHY serial-interface (SIF) I2C/SCCB primitives ----
 * SIF_X_Read / SIF_X_Write are now the REAL EN7528 SIF controller driver,
 * reverse-engineered from stock sif.ko into compat/sif_port.c (base 0xbfbf8000).
 * They replace the former "return byteCnt" stubs so the optical transceiver
 * DDM (SFF-8472 Rx power) can be read. */

/* ---- libgcc soft-float helpers (STUBS) ----
 * The kernel has NO floating-point runtime.  mt7570.c (the EN7570 optical
 * transceiver RSSI/APD/temperature calibration) still uses float/double math,
 * which emits these libgcc helpers -- unresolved in a loadable module.  These
 * stubs let econet-xpon.ko pass modpost and load on EN7528, where the EN7570
 * companion is absent and the mt7570 calibration path does NOT execute.  The
 * float paths that DO run (gpon_dev.c TOD, phy_init.c BER) were already
 * converted to integer fixed-point.  TODO (en7571 laser-bias): rework mt7570.c
 * to fixed point and delete these stubs.  Bodies avoid FP arithmetic (no
 * recursion) -- they return an operand or a constant. */
float __addsf3(float a, float b) { (void)b; return a; }
float __subsf3(float a, float b) { (void)b; return a; }
float __mulsf3(float a, float b) { (void)b; return a; }
float __divsf3(float a, float b) { (void)b; return a; }
float __negsf2(float a)          { return a; }
double __adddf3(double a, double b) { (void)b; return a; }
double __subdf3(double a, double b) { (void)b; return a; }
double __muldf3(double a, double b) { (void)b; return a; }
double __divdf3(double a, double b) { (void)b; return a; }
double __negdf2(double a)         { return a; }
double __extendsfdf2(float a)    { (void)a; return 0.0; }
float  __truncdfsf2(double a)    { (void)a; return 0.0f; }
int          __fixsfsi(float a)    { (void)a; return 0; }
int          __fixdfsi(double a)   { (void)a; return 0; }
unsigned int __fixunssfsi(float a) { (void)a; return 0; }
unsigned int __fixunsdfsi(double a){ (void)a; return 0; }
float  __floatsisf(int a)          { (void)a; return 0.0f; }
double __floatsidf(int a)          { (void)a; return 0.0; }
float  __floatunsisf(unsigned int a){ (void)a; return 0.0f; }
double __floatunsidf(unsigned int a){ (void)a; return 0.0; }
/* comparison helpers return int (0 == "equal"/false-ish) */
int __cmpsf2(float a, float b)  { (void)a; (void)b; return 0; }
int __unordsf2(float a, float b){ (void)a; (void)b; return 0; }
int __eqsf2(float a, float b)   { (void)a; (void)b; return 0; }
int __nesf2(float a, float b)   { (void)a; (void)b; return 0; }
int __gesf2(float a, float b)   { (void)a; (void)b; return 0; }
int __ltsf2(float a, float b)   { (void)a; (void)b; return 0; }
int __lesf2(float a, float b)   { (void)a; (void)b; return 0; }
int __gtsf2(float a, float b)   { (void)a; (void)b; return 0; }
int __cmpdf2(double a, double b)  { (void)a; (void)b; return 0; }
int __unorddf2(double a, double b){ (void)a; (void)b; return 0; }
int __eqdf2(double a, double b)   { (void)a; (void)b; return 0; }
int __nedf2(double a, double b)   { (void)a; (void)b; return 0; }
int __gedf2(double a, double b)   { (void)a; (void)b; return 0; }
int __ltdf2(double a, double b)   { (void)a; (void)b; return 0; }
int __ledf2(double a, double b)   { (void)a; (void)b; return 0; }
int __gtdf2(double a, double b)   { (void)a; (void)b; return 0; }

/* ---- libgcc 64-bit integer divide (REAL, not a stub) ----
 * 32-bit MIPS emits these for u64/u64; the kernel does not export them to
 * modules.  Binary long division, using only shift/compare/sub on u64 (which
 * GCC inlines) so there is no recursion back into __udivdi3/__umoddi3. */
unsigned long long __udivdi3(unsigned long long n, unsigned long long d)
{
	unsigned long long q = 0, r = 0;
	int i;
	if (!d)
		return ~0ULL;
	for (i = 63; i >= 0; i--) {
		r = (r << 1) | ((n >> i) & 1ULL);
		if (r >= d) { r -= d; q |= (1ULL << i); }
	}
	return q;
}
unsigned long long __umoddi3(unsigned long long n, unsigned long long d)
{
	unsigned long long r = 0;
	int i;
	if (!d)
		return n;
	for (i = 63; i >= 0; i--) {
		r = (r << 1) | ((n >> i) & 1ULL);
		if (r >= d)
			r -= d;
	}
	return r;
}

/* board MAC -- TODO EN7528: pull the real MAC (econet_eth / nvmem) */
static u8 econet_xpon_mac[ETH_ALEN];
uint8_t *GetMacAddr(void)
{
	return econet_xpon_mac;
}

/* FE WAN2LAN transmit entry (tcphy macSend) -- no-op until the econet_eth FE
 * datapath seam is wired.  Free the cloned skb so the stub doesn't leak. */
#include "econet_eth_seam.h"
/* FE WAN2LAN transmit entry (tcphy macSend). Wired to the econet-eth PON QDMA
 * engine (qdma[1]) via the exported seam: build a minimal desc_msg egressing to
 * GDMA2 (the GPON WAN port) and submit the skb. en75_qdma_xmit() takes ownership
 * on success (ret>=0; freed by TX-done); we free on error (ret<0).
 * TODO: derive channel/queue/gem from XPON_SKB(skb) once GEM mapping is wired. */
void macSend(unsigned int chanId, struct sk_buff *skb)
{
	struct en75_qdma *q = en75_xpon_pon_qdma();
	union desc_msg msg = { 0 };

	(void)chanId;
	if (!skb)
		return;
	if (!q || skb_linearize(skb)) {
		dev_kfree_skb_any(skb);
		return;
	}
	/* GEM tag: vendor gem (word0 bits 23:12) = low 12 bits of etx sp_tag (27:12). */
	set_etx_sp_tag(&msg.etx, XPON_SKB(skb)->gem_port & 0xFFF);
	set_etx_fport(&msg.etx, ETX_FPORT_GDM2);	/* GPON WAN egress */
	/* Direct-to-engine submit: detach the netdev so the qdma1 TX-completion NAPI
	 * (en75_qdma_unuse) SKIPS BQL/dql accounting for this frame. Without this, the
	 * completion calls netdev_tx_completed_queue() on a queue that was never
	 * netdev_tx_sent_queue()'d (macSend bypasses the port ndo_start_xmit) -> a
	 * dql_completed() BUG_ON underflow -> kernel panic in interrupt. The completion
	 * path (econet_qdma.c) already guards this with `if (skb->dev)`. */
	skb->dev = NULL;
	if (en75_qdma_xmit(q, skb, &msg, 0) < 0)
		dev_kfree_skb_any(skb);
}

/* ---- MTK sw-NAT / flow-offload hooks (NULL = hwNAT off) ---- */
int  (*ra_sw_nat_hook_rx)(struct sk_buff *skb);
int  (*ra_sw_nat_hook_tx)(struct sk_buff *skb, struct port_info *info, int magic);
int  (*ra_sw_nat_hook_magic)(struct sk_buff *skb, int magic);
void (*ra_sw_nat_hook_clean_table)(void);
void (*ra_sw_nat_hook_drop_packet)(struct sk_buff *skb);
void (*ra_sw_nat_hook_rxinfo)(struct sk_buff *skb, struct port_info *info);
void (*ra_sw_nat_hook_rx_set_l2lu)(struct sk_buff *skb);
void (*ra_sw_nat_hook_xfer)(struct sk_buff *skb);
void (*restore_offload_info_hook)(struct sk_buff *skb, struct port_info *info, int magic);

/* ---- ECNT hook framework: NO-OP stubs ----
 * The real dispatch (kernel_hook/ecnt_hook_register.c) is proprietary + 2.6.36.
 * For the compile/link milestone these are no-ops; functional wiring becomes
 * direct PON_PHY/PON_MAC calls into the optical-PHY / econet_eth seam (TODO). */
#include <ecnt_hook/ecnt_hook.h>

struct list_head ecnt_hooks[ECNT_NUM_MAINTYPE][ECNT_MAX_SUBTYPE];
void ecnt_hook_init(void) {}
int __ECNT_HOOK(unsigned int maintype, unsigned int subtype, struct ecnt_data *in_data)
{
	return ECNT_RETURN;	/* no hook ran -> proceed normally */
}
int ecnt_register_hook(struct ecnt_hook_ops *reg) { return 0; }
void ecnt_unregister_hook(struct ecnt_hook_ops *reg) {}
int ecnt_register_hooks(struct ecnt_hook_ops *reg, unsigned int n) { return 0; }
void ecnt_unregister_hooks(struct ecnt_hook_ops *reg, unsigned int n) {}
int show_all_ecnt_hookfn(void) { return 0; }
int set_ecnt_hookfn_execute_or_not(unsigned int mt, unsigned int st,
				   unsigned int hid, unsigned int ex) { return 0; }
int ecnt_ops_unregister(unsigned int mt, unsigned int st, unsigned int hid) { return 0; }
int get_ecnt_hookfn(unsigned int mt, unsigned int st) { return 0; }
