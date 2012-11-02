#include <linux/types.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/paccess.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/platform_device.h>

#define CONFIG_IFX_PCIE_1ST_CORE

#include "pcie-lantiq.h"

#define IFX_PCIE_IR                     (INT_NUM_IM4_IRL0 + 25)
#define IFX_PCIE_INTA                   (INT_NUM_IM4_IRL0 + 8)
#define IFX_PCIE_INTB                   (INT_NUM_IM4_IRL0 + 9)
#define IFX_PCIE_INTC                   (INT_NUM_IM4_IRL0 + 10)
#define IFX_PCIE_INTD                   (INT_NUM_IM4_IRL0 + 11)
#define MS(_v, _f)  (((_v) & (_f)) >> _f##_S)
#define SM(_v, _f)  (((_v) << _f##_S) & (_f))
#define IFX_REG_SET_BIT(_f, _r) \
    IFX_REG_W32((IFX_REG_R32((_r)) &~ (_f)) | (_f), (_r))
#define IFX_PCIE_LTSSM_ENABLE_TIMEOUT 10
#define IFX_PCIE_PHY_LINK_UP_TIMEOUT  1000
#define IFX_PCIE_PHY_LOOP_CNT  5

static DEFINE_SPINLOCK(ifx_pcie_lock);

int pcibios_1st_host_bus_nr(void);

unsigned int g_pcie_debug_flag = PCIE_MSG_ANY & (~PCIE_MSG_CFG);

static ifx_pcie_irq_t pcie_irqs[IFX_PCIE_CORE_NR] = {
    {
        .ir_irq = {
            .irq  = IFX_PCIE_IR,
            .name = "ifx_pcie_rc0",
        },

        .legacy_irq = {
            {
                .irq_bit = PCIE_IRN_INTA,
                .irq     = IFX_PCIE_INTA,
            },
            {
                .irq_bit = PCIE_IRN_INTB,
                .irq     = IFX_PCIE_INTB,
            },
            {
                .irq_bit = PCIE_IRN_INTC,
                .irq     = IFX_PCIE_INTC,
            },
            {
                .irq_bit = PCIE_IRN_INTD,
                .irq     = IFX_PCIE_INTD,
            },
        },
    },
};

static inline int pcie_ltssm_enable(int pcie_port)
{
	int i;

	IFX_REG_W32(PCIE_RC_CCR_LTSSM_ENABLE, PCIE_RC_CCR(pcie_port)); /* Enable LTSSM */

	/* Wait for the link to come up */
	for (i = 0; i < IFX_PCIE_LTSSM_ENABLE_TIMEOUT; i++) {
		if (!(IFX_REG_R32(PCIE_LCTLSTS(pcie_port)) & PCIE_LCTLSTS_RETRAIN_PENDING)) {
			break;
		}
		udelay(10);
	}
	if (i >= IFX_PCIE_LTSSM_ENABLE_TIMEOUT) {
		IFX_PCIE_PRINT(PCIE_MSG_INIT, "%s link timeout!!!!!\n", __func__);
		return -1;
	}
	return 0;
}

static inline void pcie_status_register_clear(int pcie_port)
{
	IFX_REG_W32(0, PCIE_RC_DR(pcie_port));
	IFX_REG_W32(0, PCIE_PCICMDSTS(pcie_port));
	IFX_REG_W32(0, PCIE_DCTLSTS(pcie_port));
	IFX_REG_W32(0, PCIE_LCTLSTS(pcie_port));
	IFX_REG_W32(0, PCIE_SLCTLSTS(pcie_port));
	IFX_REG_W32(0, PCIE_RSTS(pcie_port));
	IFX_REG_W32(0, PCIE_UES_R(pcie_port));
	IFX_REG_W32(0, PCIE_UEMR(pcie_port));
	IFX_REG_W32(0, PCIE_UESR(pcie_port));
	IFX_REG_W32(0, PCIE_CESR(pcie_port));
	IFX_REG_W32(0, PCIE_CEMR(pcie_port));
	IFX_REG_W32(0, PCIE_RESR(pcie_port));
	IFX_REG_W32(0, PCIE_PVCCRSR(pcie_port));
	IFX_REG_W32(0, PCIE_VC0_RSR0(pcie_port));
	IFX_REG_W32(0, PCIE_TPFCS(pcie_port));
	IFX_REG_W32(0, PCIE_TNPFCS(pcie_port));
	IFX_REG_W32(0, PCIE_TCFCS(pcie_port));
	IFX_REG_W32(0, PCIE_QSR(pcie_port));
	IFX_REG_W32(0, PCIE_IOBLSECS(pcie_port));
}

static inline int ifx_pcie_link_up(int pcie_port)
{
	return (IFX_REG_R32(PCIE_PHY_SR(pcie_port)) & PCIE_PHY_SR_PHY_LINK_UP) ? 1 : 0;
}

static inline void pcie_mem_io_setup(int pcie_port)
{
	unsigned int reg;
	/*
	 * BAR[0:1] readonly register 
	 * RC contains only minimal BARs for packets mapped to this device 
	 * Mem/IO filters defines a range of memory occupied by memory mapped IO devices that
	 * reside on the downstream side fo the bridge.
	 */
	reg = SM((PCIE_MEM_PHY_PORT_TO_END(pcie_port) >> 20), PCIE_MBML_MEM_LIMIT_ADDR)
		| SM((PCIE_MEM_PHY_PORT_TO_BASE(pcie_port) >> 20), PCIE_MBML_MEM_BASE_ADDR);
	IFX_REG_W32(reg, PCIE_MBML(pcie_port));

	/* PCIe_PBML, same as MBML */
	IFX_REG_W32(IFX_REG_R32(PCIE_MBML(pcie_port)), PCIE_PMBL(pcie_port));

	/* IO Address Range */
	reg = SM((PCIE_IO_PHY_PORT_TO_END(pcie_port) >> 12), PCIE_IOBLSECS_IO_LIMIT_ADDR)
		| SM((PCIE_IO_PHY_PORT_TO_BASE(pcie_port) >> 12), PCIE_IOBLSECS_IO_BASE_ADDR);
	reg |= PCIE_IOBLSECS_32BIT_IO_ADDR;
		IFX_REG_W32(reg, PCIE_IOBLSECS(pcie_port));

	reg = SM((PCIE_IO_PHY_PORT_TO_END(pcie_port) >> 16), PCIE_IO_BANDL_UPPER_16BIT_IO_LIMIT)
		| SM((PCIE_IO_PHY_PORT_TO_BASE(pcie_port) >> 16), PCIE_IO_BANDL_UPPER_16BIT_IO_BASE);
	IFX_REG_W32(reg, PCIE_IO_BANDL(pcie_port));
}

static inline void pcie_msi_setup(int pcie_port)
{
	unsigned int reg;

	/* XXX, MSI stuff should only apply to EP */
	/* MSI Capability: Only enable 32-bit addresses */
	reg = IFX_REG_R32(PCIE_MCAPR(pcie_port));
	reg &= ~PCIE_MCAPR_ADDR64_CAP;
	reg |= PCIE_MCAPR_MSI_ENABLE;

	/* Disable multiple message */
	reg &= ~(PCIE_MCAPR_MULTI_MSG_CAP | PCIE_MCAPR_MULTI_MSG_ENABLE);
	IFX_REG_W32(reg, PCIE_MCAPR(pcie_port));
}

static inline void pcie_pm_setup(int pcie_port)
{
	unsigned int reg;

	/* Enable PME, Soft reset enabled */
	reg = IFX_REG_R32(PCIE_PM_CSR(pcie_port));
	reg |= PCIE_PM_CSR_PME_ENABLE | PCIE_PM_CSR_SW_RST;
	IFX_REG_W32(reg, PCIE_PM_CSR(pcie_port));
}

static inline void pcie_bus_setup(int pcie_port)
{
	unsigned int reg;

	reg = SM(0, PCIE_BNR_PRIMARY_BUS_NUM) | SM(1, PCIE_PNR_SECONDARY_BUS_NUM) | SM(0xFF, PCIE_PNR_SUB_BUS_NUM);
	IFX_REG_W32(reg, PCIE_BNR(pcie_port));
}

static inline void pcie_device_setup(int pcie_port)
{
	unsigned int reg;

	/* Device capability register, set up Maximum payload size */
	reg = IFX_REG_R32(PCIE_DCAP(pcie_port));
	reg |= PCIE_DCAP_ROLE_BASE_ERR_REPORT;
	reg |= SM(PCIE_MAX_PAYLOAD_128, PCIE_DCAP_MAX_PAYLOAD_SIZE);

	/* Only available for EP */
	reg &= ~(PCIE_DCAP_EP_L0S_LATENCY | PCIE_DCAP_EP_L1_LATENCY);
	IFX_REG_W32(reg, PCIE_DCAP(pcie_port));

	/* Device control and status register */
	/* Set Maximum Read Request size for the device as a Requestor */
	reg = IFX_REG_R32(PCIE_DCTLSTS(pcie_port));

	/* 
	 * Request size can be larger than the MPS used, but the completions returned 
	 * for the read will be bounded by the MPS size.
	 * In our system, Max request size depends on AHB burst size. It is 64 bytes.
	 * but we set it as 128 as minimum one.
	 */
	reg |= SM(PCIE_MAX_PAYLOAD_128, PCIE_DCTLSTS_MAX_READ_SIZE)
		| SM(PCIE_MAX_PAYLOAD_128, PCIE_DCTLSTS_MAX_PAYLOAD_SIZE);

	/* Enable relaxed ordering, no snoop, and all kinds of errors */
	reg |= PCIE_DCTLSTS_RELAXED_ORDERING_EN | PCIE_DCTLSTS_ERR_EN | PCIE_DCTLSTS_NO_SNOOP_EN;

	IFX_REG_W32(reg, PCIE_DCTLSTS(pcie_port));
}

static inline void pcie_link_setup(int pcie_port)
{
	unsigned int reg;

	/*
	 * XXX, Link capability register, bit 18 for EP CLKREQ# dynamic clock management for L1, L2/3 CPM 
	 * L0s is reported during link training via TS1 order set by N_FTS
	 */
	reg = IFX_REG_R32(PCIE_LCAP(pcie_port));
	reg &= ~PCIE_LCAP_L0S_EIXT_LATENCY;
	reg |= SM(3, PCIE_LCAP_L0S_EIXT_LATENCY);
	IFX_REG_W32(reg, PCIE_LCAP(pcie_port));

	/* Link control and status register */
	reg = IFX_REG_R32(PCIE_LCTLSTS(pcie_port));

	/* Link Enable, ASPM enabled  */
	reg &= ~PCIE_LCTLSTS_LINK_DISABLE;

#ifdef CONFIG_PCIEASPM
	/*  
	 * We use the same physical reference clock that the platform provides on the connector 
	 * It paved the way for ASPM to calculate the new exit Latency
	 */
	reg |= PCIE_LCTLSTS_SLOT_CLK_CFG;
	reg |= PCIE_LCTLSTS_COM_CLK_CFG;
	/*
	 * We should disable ASPM by default except that we have dedicated power management support
	 * Enable ASPM will cause the system hangup/instability, performance degration
	 */
	reg |= PCIE_LCTLSTS_ASPM_ENABLE;
#else
	reg &= ~PCIE_LCTLSTS_ASPM_ENABLE;
#endif /* CONFIG_PCIEASPM */

	/* 
	 * The maximum size of any completion with data packet is bounded by the MPS setting 
	 * in  device control register 
	 */
	/* RCB may cause multiple split transactions, two options available, we use 64 byte RCB */
	reg &= ~ PCIE_LCTLSTS_RCB128;
	IFX_REG_W32(reg, PCIE_LCTLSTS(pcie_port));
}

static inline void pcie_error_setup(int pcie_port)
{
	unsigned int reg;

	/* 
	 * Forward ERR_COR, ERR_NONFATAL, ERR_FATAL to the backbone 
	 * Poisoned write TLPs and completions indicating poisoned TLPs will set the PCIe_PCICMDSTS.MDPE 
	 */
	reg = IFX_REG_R32(PCIE_INTRBCTRL(pcie_port));
	reg |= PCIE_INTRBCTRL_SERR_ENABLE | PCIE_INTRBCTRL_PARITY_ERR_RESP_ENABLE;

	IFX_REG_W32(reg, PCIE_INTRBCTRL(pcie_port));

	/* Uncorrectable Error Mask Register, Unmask <enable> all bits in PCIE_UESR */
	reg = IFX_REG_R32(PCIE_UEMR(pcie_port));
	reg &= ~PCIE_ALL_UNCORRECTABLE_ERR;
	IFX_REG_W32(reg, PCIE_UEMR(pcie_port));

	/* Uncorrectable Error Severity Register, ALL errors are FATAL */
	IFX_REG_W32(PCIE_ALL_UNCORRECTABLE_ERR, PCIE_UESR(pcie_port));

	/* Correctable Error Mask Register, unmask <enable> all bits */
	reg = IFX_REG_R32(PCIE_CEMR(pcie_port));
	reg &= ~PCIE_CORRECTABLE_ERR;
	IFX_REG_W32(reg, PCIE_CEMR(pcie_port));

	/* Advanced Error Capabilities and Control Registr */
	reg = IFX_REG_R32(PCIE_AECCR(pcie_port));
	reg |= PCIE_AECCR_ECRC_CHECK_EN | PCIE_AECCR_ECRC_GEN_EN;
	IFX_REG_W32(reg, PCIE_AECCR(pcie_port));

	/* Root Error Command Register, Report all types of errors */
	reg = IFX_REG_R32(PCIE_RECR(pcie_port));
	reg |= PCIE_RECR_ERR_REPORT_EN;
	IFX_REG_W32(reg, PCIE_RECR(pcie_port));

	/* Clear the Root status register */
	reg = IFX_REG_R32(PCIE_RESR(pcie_port));
	IFX_REG_W32(reg, PCIE_RESR(pcie_port));
}

static inline void pcie_root_setup(int pcie_port)
{
	unsigned int reg;

	/* Root control and capabilities register */
	reg = IFX_REG_R32(PCIE_RCTLCAP(pcie_port));
	reg |= PCIE_RCTLCAP_SERR_ENABLE | PCIE_RCTLCAP_PME_INT_EN;
	IFX_REG_W32(reg, PCIE_RCTLCAP(pcie_port));
}

static inline void pcie_vc_setup(int pcie_port)
{
	unsigned int reg;

	/* Port VC Capability Register 2 */
	reg = IFX_REG_R32(PCIE_PVC2(pcie_port));
	reg &= ~PCIE_PVC2_VC_ARB_WRR;
	reg |= PCIE_PVC2_VC_ARB_16P_FIXED_WRR;
	IFX_REG_W32(reg, PCIE_PVC2(pcie_port));

	/* VC0 Resource Capability Register */
	reg = IFX_REG_R32(PCIE_VC0_RC(pcie_port));
	reg &= ~PCIE_VC0_RC_REJECT_SNOOP;
	IFX_REG_W32(reg, PCIE_VC0_RC(pcie_port));
}

static inline void pcie_port_logic_setup(int pcie_port)
{
	unsigned int reg;

	/* FTS number, default 12, increase to 63, may increase time from/to L0s to L0  */
	reg = IFX_REG_R32(PCIE_AFR(pcie_port));
	reg &= ~(PCIE_AFR_FTS_NUM | PCIE_AFR_COM_FTS_NUM);
	reg |= SM(PCIE_AFR_FTS_NUM_DEFAULT, PCIE_AFR_FTS_NUM)
		| SM(PCIE_AFR_FTS_NUM_DEFAULT, PCIE_AFR_COM_FTS_NUM);
	/* L0s and L1 entry latency */
	reg &= ~(PCIE_AFR_L0S_ENTRY_LATENCY | PCIE_AFR_L1_ENTRY_LATENCY);
	reg |= SM(PCIE_AFR_L0S_ENTRY_LATENCY_DEFAULT, PCIE_AFR_L0S_ENTRY_LATENCY)
		| SM(PCIE_AFR_L1_ENTRY_LATENCY_DEFAULT, PCIE_AFR_L1_ENTRY_LATENCY);
	IFX_REG_W32(reg, PCIE_AFR(pcie_port));

	/* Port Link Control Register */
	reg = IFX_REG_R32(PCIE_PLCR(pcie_port));
	reg |= PCIE_PLCR_DLL_LINK_EN;  /* Enable the DLL link */
	IFX_REG_W32(reg, PCIE_PLCR(pcie_port));

	/* Lane Skew Register */
	reg = IFX_REG_R32(PCIE_LSR(pcie_port));
	/* Enable ACK/NACK and FC */
	reg &= ~(PCIE_LSR_ACKNAK_DISABLE | PCIE_LSR_FC_DISABLE); 
	IFX_REG_W32(reg, PCIE_LSR(pcie_port));

	/* Symbol Timer Register and Filter Mask Register 1 */
	reg = IFX_REG_R32(PCIE_STRFMR(pcie_port));

	/* Default SKP interval is very accurate already, 5us */
	/* Enable IO/CFG transaction */
	reg |= PCIE_STRFMR_RX_CFG_TRANS_ENABLE | PCIE_STRFMR_RX_IO_TRANS_ENABLE;
	/* Disable FC WDT */
	reg &= ~PCIE_STRFMR_FC_WDT_DISABLE;
	IFX_REG_W32(reg, PCIE_STRFMR(pcie_port));

	/* Filter Masker Register 2 */
	reg = IFX_REG_R32(PCIE_FMR2(pcie_port));
	reg |= PCIE_FMR2_VENDOR_MSG1_PASSED_TO_TRGT1 | PCIE_FMR2_VENDOR_MSG0_PASSED_TO_TRGT1;
	IFX_REG_W32(reg, PCIE_FMR2(pcie_port));

	/* VC0 Completion Receive Queue Control Register */
	reg = IFX_REG_R32(PCIE_VC0_CRQCR(pcie_port));
	reg &= ~PCIE_VC0_CRQCR_CPL_TLP_QUEUE_MODE;
	reg |= SM(PCIE_VC0_TLP_QUEUE_MODE_BYPASS, PCIE_VC0_CRQCR_CPL_TLP_QUEUE_MODE);
	IFX_REG_W32(reg, PCIE_VC0_CRQCR(pcie_port));
}

static inline void pcie_rc_cfg_reg_setup(int pcie_port)
{
	/* diable ltssm */
	IFX_REG_W32(0, PCIE_RC_CCR(pcie_port));

	pcie_mem_io_setup(pcie_port);
	pcie_msi_setup(pcie_port);
	pcie_pm_setup(pcie_port);
	pcie_bus_setup(pcie_port);
	pcie_device_setup(pcie_port);
	pcie_link_setup(pcie_port);
	pcie_error_setup(pcie_port);
	pcie_root_setup(pcie_port);
	pcie_vc_setup(pcie_port);
	pcie_port_logic_setup(pcie_port);
}

static int ifx_pcie_wait_phy_link_up(int pcie_port)
{
	int i;

	/* Wait for PHY link is up */
	for (i = 0; i < IFX_PCIE_PHY_LINK_UP_TIMEOUT; i++) {
		if (ifx_pcie_link_up(pcie_port)) {
			break;
		}
		udelay(100);
	}
	if (i >= IFX_PCIE_PHY_LINK_UP_TIMEOUT) {
		printk(KERN_ERR "%s timeout\n", __func__);
		return -1;
	}

	/* Check data link up or not */
	if (!(IFX_REG_R32(PCIE_RC_DR(pcie_port)) & PCIE_RC_DR_DLL_UP)) {
		printk(KERN_ERR "%s DLL link is still down\n", __func__);
		return -1;
	}

	/* Check Data link active or not */
	if (!(IFX_REG_R32(PCIE_LCTLSTS(pcie_port)) & PCIE_LCTLSTS_DLL_ACTIVE)) {
		printk(KERN_ERR "%s DLL is not active\n", __func__);
		return -1;
	}
	return 0;
}

static inline int pcie_app_loigc_setup(int pcie_port)
{
	IFX_REG_W32(PCIE_AHB_CTRL_BUS_ERROR_SUPPRESS, PCIE_AHB_CTRL(pcie_port));

	/* Pull PCIe EP out of reset */
	pcie_device_rst_deassert(pcie_port);

	/* Start LTSSM training between RC and EP */
	pcie_ltssm_enable(pcie_port);

	/* Check PHY status after enabling LTSSM */
	if (ifx_pcie_wait_phy_link_up(pcie_port) != 0) {
		return -1;
	}
	return 0;
}

/* 
 * Must be done after ltssm due to based on negotiated link 
 * width and payload size
 * Update the Replay Time Limit. Empirically, some PCIe 
 * devices take a little longer to respond than expected under 
 * load. As a workaround for this we configure the Replay Time 
 * Limit to the value expected for a 512 byte MPS instead of 
 * our actual 128 byte MPS. The numbers below are directly 
 * from the PCIe spec table 3-4/5. 
 */
static inline void pcie_replay_time_update(int pcie_port)
{
	unsigned int reg;
	int nlw;
	int rtl;

	reg = IFX_REG_R32(PCIE_LCTLSTS(pcie_port));

	nlw = MS(reg, PCIE_LCTLSTS_NEGOTIATED_LINK_WIDTH);
	switch (nlw) {
	case PCIE_MAX_LENGTH_WIDTH_X1:
		rtl = 1677;
		break;
	case PCIE_MAX_LENGTH_WIDTH_X2:
		rtl = 867;
		break;
	case PCIE_MAX_LENGTH_WIDTH_X4:
		rtl = 462;
		break;
	case PCIE_MAX_LENGTH_WIDTH_X8:
		rtl = 258;
		break;
	default:
		rtl = 1677;
		break;
	}
	reg = IFX_REG_R32(PCIE_ALTRT(pcie_port));
	reg &= ~PCIE_ALTRT_REPLAY_TIME_LIMIT;
	reg |= SM(rtl, PCIE_ALTRT_REPLAY_TIME_LIMIT);
	IFX_REG_W32(reg, PCIE_ALTRT(pcie_port));

	IFX_PCIE_PRINT(PCIE_MSG_REG, "%s PCIE_ALTRT 0x%08x\n",
		__func__, IFX_REG_R32(PCIE_ALTRT(pcie_port)));
}

/*
 * Table 359 Enhanced Configuration Address Mapping1)
 * 1) This table is defined in Table 7-1, page 341, PCI Express Base Specification v1.1
 * Memory Address PCI Express Configuration Space
 * A[(20+n-1):20] Bus Number 1 < n < 8
 * A[19:15] Device Number
 * A[14:12] Function Number
 * A[11:8] Extended Register Number
 * A[7:2] Register Number
 * A[1:0] Along with size of the access, used to generate Byte Enables
 * For VR9, only the address bits [22:0] are mapped to the configuration space:
 * . Address bits [22:20] select the target bus (1-of-8)1)
 * . Address bits [19:15] select the target device (1-of-32) on the bus
 * . Address bits [14:12] select the target function (1-of-8) within the device.
 * . Address bits [11:2] selects the target dword (1-of-1024) within the selected function.s configuration space
 * . Address bits [1:0] define the start byte location within the selected dword.
 */
static inline unsigned int pcie_bus_addr(u8 bus_num, u16 devfn, int where)
{
	unsigned int addr;
	u8  bus;

	if (!bus_num) {
		/* type 0 */
		addr = ((PCI_SLOT(devfn) & 0x1F) << 15) | ((PCI_FUNC(devfn) & 0x7) << 12) | ((where & 0xFFF)& ~3);
	} else {
		bus = bus_num;
		/* type 1, only support 8 buses  */
		addr = ((bus & 0x7) << 20) | ((PCI_SLOT(devfn) & 0x1F) << 15) |
			((PCI_FUNC(devfn) & 0x7) << 12) | ((where & 0xFFF) & ~3);
	}
	IFX_PCIE_PRINT(PCIE_MSG_CFG, "%s: bus addr : %02x:%02x.%01x/%02x, addr=%08x\n",
		__func__, bus_num, PCI_SLOT(devfn), PCI_FUNC(devfn), where, addr);
	return addr;
}

static int pcie_valid_config(int pcie_port, int bus, int dev)
{
	/* RC itself */
	if ((bus == 0) && (dev == 0))
		return 1;

	/* No physical link */
	if (!ifx_pcie_link_up(pcie_port))
		return 0;

	/* Bus zero only has RC itself
	* XXX, check if EP will be integrated 
	*/
	if ((bus == 0) && (dev != 0))
		return 0;

	/* Maximum 8 buses supported for VRX */
	if (bus > 9)
		return 0;

	/* 
	 * PCIe is PtP link, one bus only supports only one device 
	 * except bus zero and PCIe switch which is virtual bus device
	 * The following two conditions really depends on the system design
	 * and attached the device.
	 * XXX, how about more new switch
	 */
	if ((bus == 1) && (dev != 0))
		return 0;

	if ((bus >= 3) && (dev != 0))
		return 0;
	return 1;
}

static inline unsigned int ifx_pcie_cfg_rd(int pcie_port, unsigned int reg)
{
	return IFX_REG_R32((volatile unsigned int *)(PCIE_CFG_PORT_TO_BASE(pcie_port) + reg));
}

static inline void ifx_pcie_cfg_wr(int pcie_port, unsigned int reg, unsigned int val)
{
	IFX_REG_W32( val, (volatile unsigned int *)(PCIE_CFG_PORT_TO_BASE(pcie_port) + reg));
}

static inline unsigned int ifx_pcie_rc_cfg_rd(int pcie_port, unsigned int reg)
{
	return IFX_REG_R32((volatile unsigned int *)(PCIE_RC_PORT_TO_BASE(pcie_port) + reg));
}

static inline void ifx_pcie_rc_cfg_wr(int pcie_port, unsigned int reg, unsigned int val)
{
	IFX_REG_W32(val, (volatile unsigned int *)(PCIE_RC_PORT_TO_BASE(pcie_port) + reg));
}

unsigned int ifx_pcie_bus_enum_read_hack(int where, unsigned int value)
{
	unsigned int tvalue = value;

	if (where == PCI_PRIMARY_BUS) {
		u8 primary, secondary, subordinate;

		primary = tvalue & 0xFF;
		secondary = (tvalue >> 8) & 0xFF;
		subordinate = (tvalue >> 16) & 0xFF;
		primary += pcibios_1st_host_bus_nr();
		secondary += pcibios_1st_host_bus_nr();
		subordinate += pcibios_1st_host_bus_nr();
		tvalue = (tvalue & 0xFF000000) | (unsigned int)primary | (unsigned int)(secondary << 8) | (unsigned int)(subordinate << 16);
	}
	return tvalue;
}

unsigned int ifx_pcie_bus_enum_write_hack(int where, unsigned int value)
{
	unsigned int tvalue = value;

	if (where == PCI_PRIMARY_BUS) {
		u8 primary, secondary, subordinate;

		primary = tvalue & 0xFF;
		secondary = (tvalue >> 8) & 0xFF;
		subordinate = (tvalue >> 16) & 0xFF;
		if (primary > 0 && primary != 0xFF)
			primary -= pcibios_1st_host_bus_nr();
		if (secondary > 0 && secondary != 0xFF)
			secondary -= pcibios_1st_host_bus_nr();
		if (subordinate > 0 && subordinate != 0xFF)
			subordinate -= pcibios_1st_host_bus_nr();
		tvalue = (tvalue & 0xFF000000) | (unsigned int)primary | (unsigned int)(secondary << 8) | (unsigned int)(subordinate << 16);
	} else if (where == PCI_SUBORDINATE_BUS) {
		u8 subordinate = tvalue & 0xFF;
		subordinate = subordinate > 0 ? subordinate - pcibios_1st_host_bus_nr() : 0;
		tvalue = subordinate;
	}
	return tvalue;
}

/** 
 * \fn static int ifx_pcie_read_config(struct pci_bus *bus, unsigned int devfn, 
 *                   int where, int size, unsigned int *value)
 * \brief Read a value from configuration space 
 * 
 * \param[in] bus    Pointer to pci bus
 * \param[in] devfn  PCI device function number
 * \param[in] where  PCI register number 
 * \param[in] size   Register read size
 * \param[out] value    Pointer to return value
 * \return  PCIBIOS_BAD_REGISTER_NUMBER Invalid register number
 * \return  PCIBIOS_FUNC_NOT_SUPPORTED  PCI function not supported
 * \return  PCIBIOS_DEVICE_NOT_FOUND    PCI device not found
 * \return  PCIBIOS_SUCCESSFUL          OK
 * \ingroup IFX_PCIE_OS
 */
static int ifx_pcie_read_config(struct pci_bus *bus, unsigned int devfn, int where, int size, unsigned int *value)
{
	unsigned int data = 0;
	int bus_number = bus->number;
	static const unsigned int mask[8] = {0, 0xff, 0xffff, 0, 0xffffffff, 0, 0, 0};
	int ret = PCIBIOS_SUCCESSFUL;
	struct ifx_pci_controller *ctrl = bus->sysdata;
	int pcie_port = ctrl->port;

	if (unlikely(size != 1 && size != 2 && size != 4)){
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}

	/* Make sure the address is aligned to natural boundary */
	if (unlikely(((size - 1) & where))) {
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}

	/* 
	 * If we are second controller, we have to cheat OS so that it assume 
	 * its bus number starts from 0 in host controller
	 */
	bus_number = ifx_pcie_bus_nr_deduct(bus_number, pcie_port);

	/* 
	 * We need to force the bus number to be zero on the root 
	 * bus. Linux numbers the 2nd root bus to start after all 
	 * busses on root 0. 
	 */
	if (bus->parent == NULL)
		bus_number = 0;

	/* 
	 * PCIe only has a single device connected to it. It is 
	 * always device ID 0. Don't bother doing reads for other 
	 * device IDs on the first segment. 
	 */
	if ((bus_number == 0) && (PCI_SLOT(devfn) != 0)) {
		ret = PCIBIOS_FUNC_NOT_SUPPORTED;
		goto out;
	}

	if (pcie_valid_config(pcie_port, bus_number, PCI_SLOT(devfn)) == 0) {
		*value = 0xffffffff;
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		goto out;
	}

	IFX_PCIE_PRINT(PCIE_MSG_READ_CFG, "%s: %02x:%02x.%01x/%02x:%01d\n", __func__, bus_number,
		PCI_SLOT(devfn), PCI_FUNC(devfn), where, size);

	PCIE_IRQ_LOCK(ifx_pcie_lock);
	if (bus_number == 0) { /* RC itself */
		unsigned int t;

		t = (where & ~3);
		data = ifx_pcie_rc_cfg_rd(pcie_port, t);
		IFX_PCIE_PRINT(PCIE_MSG_READ_CFG, "%s: rd local cfg, offset:%08x, data:%08x\n",
			__func__, t, data);
	} else {
		unsigned int addr = pcie_bus_addr(bus_number, devfn, where);

		data = ifx_pcie_cfg_rd(pcie_port, addr);
		if (pcie_port == IFX_PCIE_PORT0) {
#ifdef CONFIG_IFX_PCIE_HW_SWAP
			data = le32_to_cpu(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		} else {
#ifdef CONFIG_IFX_PCIE1_HW_SWAP
			data = le32_to_cpu(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		}
	}
	/* To get a correct PCI topology, we have to restore the bus number to OS */
	data = ifx_pcie_bus_enum_hack(bus, devfn, where, data, pcie_port, 1);

	PCIE_IRQ_UNLOCK(ifx_pcie_lock);
	IFX_PCIE_PRINT(PCIE_MSG_READ_CFG, "%s: read config: data=%08x raw=%08x\n",
		__func__, (data >> (8 * (where & 3))) & mask[size & 7], data); 

	*value = (data >> (8 * (where & 3))) & mask[size & 7];
out:
	return ret;
}

static unsigned int ifx_pcie_size_to_value(int where, int size, unsigned int data, unsigned int value)
{
	unsigned int shift;
	unsigned int tdata = data;

	switch (size) {
	case 1:
		shift = (where & 0x3) << 3;
		tdata &= ~(0xffU << shift);
		tdata |= ((value & 0xffU) << shift);
		break;
	case 2:
		shift = (where & 3) << 3;
		tdata &= ~(0xffffU << shift);
		tdata |= ((value & 0xffffU) << shift);
		break;
	case 4:
		tdata = value;
		break;
	}
	return tdata;
}

/** 
 * \fn static static int ifx_pcie_write_config(struct pci_bus *bus, unsigned int devfn,
 *                 int where, int size, unsigned int value)
 * \brief Write a value to PCI configuration space 
 * 
 * \param[in] bus    Pointer to pci bus
 * \param[in] devfn  PCI device function number
 * \param[in] where  PCI register number 
 * \param[in] size   The register size to be written
 * \param[in] value  The valule to be written
 * \return PCIBIOS_BAD_REGISTER_NUMBER Invalid register number
 * \return PCIBIOS_DEVICE_NOT_FOUND    PCI device not found
 * \return PCIBIOS_SUCCESSFUL          OK
 * \ingroup IFX_PCIE_OS
 */
static int ifx_pcie_write_config(struct pci_bus *bus, unsigned int devfn, int where, int size, unsigned int value)
{
	int bus_number = bus->number;
	int ret = PCIBIOS_SUCCESSFUL;
	struct ifx_pci_controller *ctrl = bus->sysdata;
	int pcie_port = ctrl->port;
	unsigned int tvalue = value;
	unsigned int data;

	/* Make sure the address is aligned to natural boundary */
	if (unlikely(((size - 1) & where))) {
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}
	/* 
	 * If we are second controller, we have to cheat OS so that it assume 
	 * its bus number starts from 0 in host controller
 	 */
	bus_number = ifx_pcie_bus_nr_deduct(bus_number, pcie_port);

	/* 
	 * We need to force the bus number to be zero on the root 
	 * bus. Linux numbers the 2nd root bus to start after all 
	 * busses on root 0. 
	 */
	if (bus->parent == NULL)
		bus_number = 0;

	if (pcie_valid_config(pcie_port, bus_number, PCI_SLOT(devfn)) == 0) {
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		goto out;
	}

	IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG, "%s: %02x:%02x.%01x/%02x:%01d value=%08x\n", __func__,
		bus_number, PCI_SLOT(devfn), PCI_FUNC(devfn), where, size, value);

	/* XXX, some PCIe device may need some delay */
	PCIE_IRQ_LOCK(ifx_pcie_lock);

	/* 
	 * To configure the correct bus topology using native way, we have to cheat Os so that
	 * it can configure the PCIe hardware correctly.
	 */
	tvalue = ifx_pcie_bus_enum_hack(bus, devfn, where, value, pcie_port, 0);

	if (bus_number == 0) { /* RC itself */
		unsigned int t;

		t = (where & ~3);
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: wr local cfg, offset:%08x, fill:%08x\n", __func__, t, value);
		data = ifx_pcie_rc_cfg_rd(pcie_port, t);
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: rd local cfg, offset:%08x, data:%08x\n", __func__, t, data);

		data = ifx_pcie_size_to_value(where, size, data, tvalue);

		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: wr local cfg, offset:%08x, value:%08x\n", __func__, t, data);
		ifx_pcie_rc_cfg_wr(pcie_port, t, data);
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: rd local cfg, offset:%08x, value:%08x\n",
			__func__, t, ifx_pcie_rc_cfg_rd(pcie_port, t));
	} else {
		unsigned int addr = pcie_bus_addr(bus_number, devfn, where);

		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: wr cfg, offset:%08x, fill:%08x\n", __func__, addr, value);
		data = ifx_pcie_cfg_rd(pcie_port, addr);
		if (pcie_port == IFX_PCIE_PORT0) {
#ifdef CONFIG_IFX_PCIE_HW_SWAP
			data = le32_to_cpu(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		} else {
#ifdef CONFIG_IFX_PCIE1_HW_SWAP
			data = le32_to_cpu(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		}
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG,"%s: rd cfg, offset:%08x, data:%08x\n", __func__, addr, data);

		data = ifx_pcie_size_to_value(where, size, data, tvalue);
		if (pcie_port == IFX_PCIE_PORT0) {
#ifdef CONFIG_IFX_PCIE_HW_SWAP
			data = cpu_to_le32(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		} else {
#ifdef CONFIG_IFX_PCIE1_HW_SWAP
			data = cpu_to_le32(data);
#endif /* CONFIG_IFX_PCIE_HW_SWAP */
		}
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG, "%s: wr cfg, offset:%08x, value:%08x\n", __func__, addr, data);
		ifx_pcie_cfg_wr(pcie_port, addr, data);
		IFX_PCIE_PRINT(PCIE_MSG_WRITE_CFG, "%s: rd cfg, offset:%08x, value:%08x\n", 
			__func__, addr, ifx_pcie_cfg_rd(pcie_port, addr));
	}
	PCIE_IRQ_UNLOCK(ifx_pcie_lock);
out:
	return ret;
}

static struct resource ifx_pcie_io_resource = {
	.name  = "PCIe0 I/O space",
	.start = PCIE_IO_PHY_BASE,
	.end = PCIE_IO_PHY_END,
	.flags = IORESOURCE_IO,
};

static struct resource ifx_pcie_mem_resource = {
	.name = "PCIe0 Memory space",
	.start = PCIE_MEM_PHY_BASE,
	.end = PCIE_MEM_PHY_END,
	.flags = IORESOURCE_MEM,
};

static struct pci_ops ifx_pcie_ops = {
	.read = ifx_pcie_read_config,
	.write = ifx_pcie_write_config,
};

static struct ifx_pci_controller ifx_pcie_controller[IFX_PCIE_CORE_NR] = {
	{
		.pcic = {
			.pci_ops = &ifx_pcie_ops,
			.mem_resource = &ifx_pcie_mem_resource,
			.io_resource = &ifx_pcie_io_resource,
		},
		.port = IFX_PCIE_PORT0,
	},
};

static inline void pcie_core_int_clear_all(int pcie_port)
{
	unsigned int reg;
	reg = IFX_REG_R32(PCIE_IRNCR(pcie_port));
	reg &= PCIE_RC_CORE_COMBINED_INT;
	IFX_REG_W32(reg, PCIE_IRNCR(pcie_port));
}

static irqreturn_t pcie_rc_core_isr(int irq, void *dev_id)
{
	struct ifx_pci_controller *ctrl = (struct ifx_pci_controller *)dev_id;
	int pcie_port = ctrl->port;

	IFX_PCIE_PRINT(PCIE_MSG_ISR, "PCIe RC error intr %d\n", irq);
	pcie_core_int_clear_all(pcie_port);
	return IRQ_HANDLED;
}

static int pcie_rc_core_int_init(int pcie_port)
{
	int ret;

	/* Enable core interrupt */
	IFX_REG_SET_BIT(PCIE_RC_CORE_COMBINED_INT, PCIE_IRNEN(pcie_port));

	/* Clear it first */
	IFX_REG_SET_BIT(PCIE_RC_CORE_COMBINED_INT, PCIE_IRNCR(pcie_port));
	ret = request_irq(pcie_irqs[pcie_port].ir_irq.irq, pcie_rc_core_isr, IRQF_DISABLED,
	pcie_irqs[pcie_port].ir_irq.name, &ifx_pcie_controller[pcie_port]);
	if (ret)
		printk(KERN_ERR "%s request irq %d failed\n", __func__, IFX_PCIE_IR);

	return ret;
}

int ifx_pcie_bios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	unsigned int irq_bit = 0;
	int irq = 0;
	struct ifx_pci_controller *ctrl = dev->bus->sysdata;
	int pcie_port = ctrl->port;

	IFX_PCIE_PRINT(PCIE_MSG_FIXUP, "%s port %d dev %s slot %d pin %d \n", __func__, pcie_port, pci_name(dev), slot, pin);

	if ((pin == PCIE_LEGACY_DISABLE) || (pin > PCIE_LEGACY_INT_MAX)) {
		printk(KERN_WARNING "WARNING: dev %s: invalid interrupt pin %d\n", pci_name(dev), pin);
		return -1;
	}
	/* Pin index so minus one */
	irq_bit = pcie_irqs[pcie_port].legacy_irq[pin - 1].irq_bit;
	irq = pcie_irqs[pcie_port].legacy_irq[pin - 1].irq;
	IFX_REG_SET_BIT(irq_bit, PCIE_IRNEN(pcie_port));
	IFX_REG_SET_BIT(irq_bit, PCIE_IRNCR(pcie_port));
	IFX_PCIE_PRINT(PCIE_MSG_FIXUP, "%s dev %s irq %d assigned\n", __func__, pci_name(dev), irq);
	return irq;
}

/** 
 * \fn int ifx_pcie_bios_plat_dev_init(struct pci_dev *dev)
 * \brief Called to perform platform specific PCI setup 
 * 
 * \param[in] dev The Linux PCI device structure for the device to map
 * \return OK
 * \ingroup IFX_PCIE_OS
 */ 
int ifx_pcie_bios_plat_dev_init(struct pci_dev *dev)
{
	u16 config;
	unsigned int dconfig;
	int pos;
	/* Enable reporting System errors and parity errors on all devices */ 
	/* Enable parity checking and error reporting */ 
	pci_read_config_word(dev, PCI_COMMAND, &config);
	config |= PCI_COMMAND_PARITY | PCI_COMMAND_SERR /*| PCI_COMMAND_INVALIDATE |
	PCI_COMMAND_FAST_BACK*/;
	pci_write_config_word(dev, PCI_COMMAND, config);

	if (dev->subordinate) {
		/* Set latency timers on sub bridges */
		pci_write_config_byte(dev, PCI_SEC_LATENCY_TIMER, 0x40); /* XXX, */
		/* More bridge error detection */
		pci_read_config_word(dev, PCI_BRIDGE_CONTROL, &config);
		config |= PCI_BRIDGE_CTL_PARITY | PCI_BRIDGE_CTL_SERR;
		pci_write_config_word(dev, PCI_BRIDGE_CONTROL, config);
	}
	/* Enable the PCIe normal error reporting */
	pos = pci_find_capability(dev, PCI_CAP_ID_EXP);
	if (pos) {
		/* Disable system error generation in response to error messages */
		pci_read_config_word(dev, pos + PCI_EXP_RTCTL, &config);
		config &= ~(PCI_EXP_RTCTL_SECEE | PCI_EXP_RTCTL_SENFEE | PCI_EXP_RTCTL_SEFEE);
		pci_write_config_word(dev, pos + PCI_EXP_RTCTL, config);

		/* Clear PCIE Capability's Device Status */
		pci_read_config_word(dev, pos + PCI_EXP_DEVSTA, &config);
		pci_write_config_word(dev, pos + PCI_EXP_DEVSTA, config);

		/* Update Device Control */
		pci_read_config_word(dev, pos + PCI_EXP_DEVCTL, &config);
		/* Correctable Error Reporting */
		config |= PCI_EXP_DEVCTL_CERE;
		/* Non-Fatal Error Reporting */
		config |= PCI_EXP_DEVCTL_NFERE;
		/* Fatal Error Reporting */
		config |= PCI_EXP_DEVCTL_FERE;
		/* Unsupported Request */
		config |= PCI_EXP_DEVCTL_URRE;
		pci_write_config_word(dev, pos + PCI_EXP_DEVCTL, config);
	}

	/* Find the Advanced Error Reporting capability */
	pos = pci_find_ext_capability(dev, PCI_EXT_CAP_ID_ERR);
	if (pos) {
		/* Clear Uncorrectable Error Status */
		pci_read_config_dword(dev, pos + PCI_ERR_UNCOR_STATUS, &dconfig);
		pci_write_config_dword(dev, pos + PCI_ERR_UNCOR_STATUS, dconfig);
		/* Enable reporting of all uncorrectable errors */
		/* Uncorrectable Error Mask - turned on bits disable errors */
		pci_write_config_dword(dev, pos + PCI_ERR_UNCOR_MASK, 0);
		/* 
		 * Leave severity at HW default. This only controls if 
		 * errors are reported as uncorrectable or 
		 * correctable, not if the error is reported. 
		 */
		/* PCI_ERR_UNCOR_SEVER - Uncorrectable Error Severity */
		/* Clear Correctable Error Status */
		pci_read_config_dword(dev, pos + PCI_ERR_COR_STATUS, &dconfig);
		pci_write_config_dword(dev, pos + PCI_ERR_COR_STATUS, dconfig);
		/* Enable reporting of all correctable errors */
		/* Correctable Error Mask - turned on bits disable errors */
		pci_write_config_dword(dev, pos + PCI_ERR_COR_MASK, 0);
		/* Advanced Error Capabilities */
		pci_read_config_dword(dev, pos + PCI_ERR_CAP, &dconfig);
		/* ECRC Generation Enable */
		if (dconfig & PCI_ERR_CAP_ECRC_GENC)
			dconfig |= PCI_ERR_CAP_ECRC_GENE;
		/* ECRC Check Enable */
		if (dconfig & PCI_ERR_CAP_ECRC_CHKC)
			dconfig |= PCI_ERR_CAP_ECRC_CHKE;
		pci_write_config_dword(dev, pos + PCI_ERR_CAP, dconfig);

		/* PCI_ERR_HEADER_LOG - Header Log Register (16 bytes) */
		/* Enable Root Port's interrupt in response to error messages */
		pci_write_config_dword(dev, pos + PCI_ERR_ROOT_COMMAND,
			PCI_ERR_ROOT_CMD_COR_EN |
			PCI_ERR_ROOT_CMD_NONFATAL_EN |
			PCI_ERR_ROOT_CMD_FATAL_EN);
		/* Clear the Root status register */
		pci_read_config_dword(dev, pos + PCI_ERR_ROOT_STATUS, &dconfig);
		pci_write_config_dword(dev, pos + PCI_ERR_ROOT_STATUS, dconfig);
	}
	/* WAR, only 128 MRRS is supported, force all EPs to support this value */
	pcie_set_readrq(dev, 128);
	return 0;
}

static void pcie_phy_rst(int pcie_port)
{
	pcie_phy_rst_assert(pcie_port);
	pcie_phy_rst_deassert(pcie_port);
	/* Make sure PHY PLL is stable */
	udelay(20);
}

static int pcie_rc_initialize(int pcie_port)
{
	int i;

	pcie_rcu_endian_setup(pcie_port);

	pcie_ep_gpio_rst_init(pcie_port);

	/* 
	 * XXX, PCIe elastic buffer bug will cause not to be detected. One more 
	 * reset PCIe PHY will solve this issue 
	 */
	for (i = 0; i < IFX_PCIE_PHY_LOOP_CNT; i++) {
		/* Disable PCIe PHY Analog part for sanity check */
		pcie_phy_pmu_disable(pcie_port);
		pcie_phy_rst(pcie_port);
		/* PCIe Core reset enabled, low active, sw programmed */
		pcie_core_rst_assert(pcie_port);
		/* Put PCIe EP in reset status */
		pcie_device_rst_assert(pcie_port);
		/* PCI PHY & Core reset disabled, high active, sw programmed */
		pcie_core_rst_deassert(pcie_port);
		/* Already in a quiet state, program PLL, enable PHY, check ready bit */
		pcie_phy_clock_mode_setup(pcie_port);
		/* Enable PCIe PHY and Clock */
		pcie_core_pmu_setup(pcie_port);
		/* Clear status registers */
		pcie_status_register_clear(pcie_port);
#ifdef CONFIG_PCI_MSI
		pcie_msi_init(pcie_port);
#endif /* CONFIG_PCI_MSI */
		pcie_rc_cfg_reg_setup(pcie_port);

		/* Once link is up, break out */
		if (pcie_app_loigc_setup(pcie_port) == 0)
			break;
	}
	if (i >= IFX_PCIE_PHY_LOOP_CNT) {
		printk(KERN_ERR "%s link up failed!!!!!\n", __func__);
		return -EIO;
	}
	/* NB, don't increase ACK/NACK timer timeout value, which will cause a lot of COR errors */
	pcie_replay_time_update(pcie_port);
	return 0;
}

static int inline ifx_pcie_startup_port_nr(void)
{
	int pcie_port = IFX_PCIE_PORT0;

	pcie_port = IFX_PCIE_PORT0;
	return pcie_port;
}

/** 
 * \fn static int __init ifx_pcie_bios_init(void)
 * \brief Initialize the IFX PCIe controllers
 *
 * \return -EIO    PCIe PHY link is not up
 * \return -ENOMEM Configuration/IO space failed to map
 * \return 0       OK
 * \ingroup IFX_PCIE_OS
 */ 
extern int (*ltqpci_plat_arch_init)(struct pci_dev *dev);
extern int (*ltqpci_map_irq)(const struct pci_dev *dev, u8 slot, u8 pin);
static int __devinit ltq_pcie_probe(struct platform_device *pdev)
{
	char ver_str[128] = {0};
	void __iomem *io_map_base;
	int pcie_port;
	int startup_port;
	ltqpci_map_irq = ifx_pcie_bios_map_irq;
	ltqpci_plat_arch_init = ifx_pcie_bios_plat_dev_init;
	/* Enable AHB Master/ Slave */
	pcie_ahb_pmu_setup();

	startup_port = ifx_pcie_startup_port_nr();

	ltq_gpio_request(&pdev->dev, IFX_PCIE_GPIO_RESET, 0, 1, "pcie-reset");

	for (pcie_port = startup_port; pcie_port < IFX_PCIE_CORE_NR; pcie_port++){
		if (pcie_rc_initialize(pcie_port) == 0) {
			/* Otherwise, warning will pop up */
			io_map_base = ioremap(PCIE_IO_PHY_PORT_TO_BASE(pcie_port), PCIE_IO_SIZE);
			if (io_map_base == NULL)
				return -ENOMEM;
			ifx_pcie_controller[pcie_port].pcic.io_map_base = (unsigned long)io_map_base;
			register_pci_controller(&ifx_pcie_controller[pcie_port].pcic);
			/* XXX, clear error status */
			pcie_rc_core_int_init(pcie_port);
		}
	}

	printk(KERN_INFO "%s", ver_str);
return 0;
}

static struct platform_driver ltq_pcie_driver = {
	.probe = ltq_pcie_probe,
	.driver = {
		.name = "pcie-xway",
		.owner = THIS_MODULE,
	},
};

int __init pciebios_init(void)
{
	return platform_driver_register(&ltq_pcie_driver);
}

arch_initcall(pciebios_init);
