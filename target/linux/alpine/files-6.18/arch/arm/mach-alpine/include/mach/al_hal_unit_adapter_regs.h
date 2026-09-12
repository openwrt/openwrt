/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __AL_HAL_UNIT_ADAPTER_REGS_H__
#define __AL_HAL_UNIT_ADAPTER_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AL_PCI_COMMAND		0x04	/* 16 bits */
#define  AL_PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  AL_PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  AL_PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */

#define PCI_CLASS_REVISION      0x08    /* High 24 bits are class, low 8 revision */

#define  AL_PCI_BASE_ADDRESS_SPACE_IO        0x01
#define  AL_PCI_BASE_ADDRESS_MEM_TYPE_64     0x04    /* 64 bit address */
#define  AL_PCI_BASE_ADDRESS_MEM_PREFETCH    0x08    /* prefetchable? */
#define  AL_PCI_BASE_ADDRESS_DEVICE_ID	     0x0c
#define  AL_PCI_BASE_ADDRESS_0        0x10    /* 32 bits */

#define  AL_PCI_BASE_ADDRESS_2               0x18
#define  AL_PCI_BASE_ADDRESS_4               0x20

#define  AL_PCI_AXI_CFG_AND_CTR_0           0x110
#define  AL_PCI_AXI_CFG_AND_CTR_1           0x130
#define  AL_PCI_AXI_CFG_AND_CTR_2           0x150
#define  AL_PCI_AXI_CFG_AND_CTR_3           0x170

#define  AL_PCI_APP_CONTROL                 0x220

#define  AL_PCI_SRIOV_TOTAL_AND_INITIAL_VFS 0x30c

#define  AL_PCI_VF_BASE_ADDRESS_0           0x324


#define AL_PCI_EXP_CAP_BASE	0x40
#define AL_PCI_EXP_DEVCAP          4       /* Device capabilities */
#define  AL_PCI_EXP_DEVCAP_PAYLOAD 0x07    /* Max_Payload_Size */
#define  AL_PCI_EXP_DEVCAP_PHANTOM 0x18    /* Phantom functions */
#define  AL_PCI_EXP_DEVCAP_EXT_TAG 0x20    /* Extended tags */
#define  AL_PCI_EXP_DEVCAP_L0S     0x1c0   /* L0s Acceptable Latency */
#define  AL_PCI_EXP_DEVCAP_L1      0xe00   /* L1 Acceptable Latency */
#define  AL_PCI_EXP_DEVCAP_ATN_BUT 0x1000  /* Attention Button Present */
#define  AL_PCI_EXP_DEVCAP_ATN_IND 0x2000  /* Attention Indicator Present */
#define  AL_PCI_EXP_DEVCAP_PWR_IND 0x4000  /* Power Indicator Present */
#define  AL_PCI_EXP_DEVCAP_RBER    0x8000  /* Role-Based Error Reporting */
#define  AL_PCI_EXP_DEVCAP_PWR_VAL 0x3fc0000 /* Slot Power Limit Value */
#define  AL_PCI_EXP_DEVCAP_PWR_SCL 0xc000000 /* Slot Power Limit Scale */
#define  AL_PCI_EXP_DEVCAP_FLR     0x10000000 /* Function Level Reset */
#define AL_PCI_EXP_DEVCTL          8       /* Device Control */
#define  AL_PCI_EXP_DEVCTL_CERE    0x0001  /* Correctable Error Reporting En. */
#define  AL_PCI_EXP_DEVCTL_NFERE   0x0002  /* Non-Fatal Error Reporting Enable */
#define  AL_PCI_EXP_DEVCTL_FERE    0x0004  /* Fatal Error Reporting Enable */
#define  AL_PCI_EXP_DEVCTL_URRE    0x0008  /* Unsupported Request Reporting En. */
#define  AL_PCI_EXP_DEVCTL_RELAX_EN 0x0010 /* Enable relaxed ordering */
#define  AL_PCI_EXP_DEVCTL_PAYLOAD 0x00e0  /* Max_Payload_Size */
#define  AL_PCI_EXP_DEVCTL_EXT_TAG 0x0100  /* Extended Tag Field Enable */
#define  AL_PCI_EXP_DEVCTL_PHANTOM 0x0200  /* Phantom Functions Enable */
#define  AL_PCI_EXP_DEVCTL_AUX_PME 0x0400  /* Auxiliary Power PM Enable */
#define  AL_PCI_EXP_DEVCTL_NOSNOOP_EN 0x0800  /* Enable No Snoop */
#define  AL_PCI_EXP_DEVCTL_READRQ  0x7000  /* Max_Read_Request_Size */
#define  AL_PCI_EXP_DEVCTL_BCR_FLR 0x8000  /* Bridge Configuration Retry / FLR */
#define AL_PCI_EXP_DEVSTA          0xA      /* Device Status */
#define  AL_PCI_EXP_DEVSTA_CED     0x01    /* Correctable Error Detected */
#define  AL_PCI_EXP_DEVSTA_NFED    0x02    /* Non-Fatal Error Detected */
#define  AL_PCI_EXP_DEVSTA_FED     0x04    /* Fatal Error Detected */
#define  AL_PCI_EXP_DEVSTA_URD     0x08    /* Unsupported Request Detected */
#define  AL_PCI_EXP_DEVSTA_AUXPD   0x10    /* AUX Power Detected */
#define  AL_PCI_EXP_DEVSTA_TRPND   0x20    /* Transactions Pending */
#define AL_PCI_EXP_LNKCAP	   0xC	   /* Link Capabilities */
#define  AL_PCI_EXP_LNKCAP_SLS	   0xf	   /* Supported Link Speeds */
#define  AL_PCI_EXP_LNKCAP_SLS_2_5GB 0x1   /* LNKCAP2 SLS Vector bit 0 (2.5GT/s) */
#define  AL_PCI_EXP_LNKCAP_SLS_5_0GB 0x2   /* LNKCAP2 SLS Vector bit 1 (5.0GT/s) */
#define  AL_PCI_EXP_LNKCAP_MLW	   0x3f0   /* Maximum Link Width */
#define  AL_PCI_EXP_LNKCAP_ASPMS   0xc00   /* ASPM Support */
#define  AL_PCI_EXP_LNKCAP_L0SEL   0x7000  /* L0s Exit Latency */
#define  AL_PCI_EXP_LNKCAP_L1EL	   0x38000 /* L1 Exit Latency */
#define  AL_PCI_EXP_LNKCAP_CLKPM   0x40000 /* L1 Clock Power Management */
#define  AL_PCI_EXP_LNKCAP_SDERC   0x80000 /* Surprise Down Error Reporting Capable */
#define  AL_PCI_EXP_LNKCAP_DLLLARC 0x100000 /* Data Link Layer Link Active Reporting Capable */
#define  AL_PCI_EXP_LNKCAP_LBNC	   0x200000 /* Link Bandwidth Notification Capability */
#define  AL_PCI_EXP_LNKCAP_PN	   0xff000000 /* Port Number */

#define AL_PCI_EXP_LNKSTA            0x12      /* Link Status */
#define  AL_PCI_EXP_LNKSTA_CLS       0x000f  /* Current Link Speed */
#define  AL_PCI_EXP_LNKSTA_CLS_2_5GB 0x01    /* Current Link Speed 2.5GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_5_0GB 0x02    /* Current Link Speed 5.0GT/s */
#define  AL_PCI_EXP_LNKSTA_CLS_8_0GB 0x03    /* Current Link Speed 8.0GT/s */
#define  AL_PCI_EXP_LNKSTA_NLW       0x03f0  /* Nogotiated Link Width */
#define  AL_PCI_EXP_LNKSTA_NLW_SHIFT 4       /* start of NLW mask in link status */
#define  AL_PCI_EXP_LNKSTA_LT        0x0800  /* Link Training */
#define  AL_PCI_EXP_LNKSTA_SLC       0x1000  /* Slot Clock Configuration */
#define  AL_PCI_EXP_LNKSTA_DLLLA     0x2000  /* Data Link Layer Link Active */
#define  AL_PCI_EXP_LNKSTA_LBMS      0x4000  /* Link Bandwidth Management Status */
#define  AL_PCI_EXP_LNKSTA_LABS      0x8000  /* Link Autonomous Bandwidth Status */

#define AL_PCI_EXP_LNKCTL2           0x30      /* Link Control 2 */

#define AL_PCI_MSIX_MSGCTRL                 0            /* MSIX message control reg */
#define AL_PCI_MSIX_MSGCTRL_TBL_SIZE        0x7ff        /* MSIX table size */
#define AL_PCI_MSIX_MSGCTRL_TBL_SIZE_SHIFT  16           /* MSIX table size shift */
#define AL_PCI_MSIX_MSGCTRL_EN              0x80000000   /* MSIX enable */
#define AL_PCI_MSIX_MSGCTRL_MASK            0x40000000   /* MSIX mask */

#define AL_PCI_MSIX_TABLE            0x4          /* MSIX table offset and bar reg */
#define AL_PCI_MSIX_TABLE_OFFSET     0xfffffff8   /* MSIX table offset */
#define AL_PCI_MSIX_TABLE_BAR        0x7          /* MSIX table BAR */

#define AL_PCI_MSIX_PBA              0x8          /* MSIX pba offset and bar reg */
#define AL_PCI_MSIX_PBA_OFFSET       0xfffffff8   /* MSIX pba offset */
#define AL_PCI_MSIX_PBA_BAR          0x7          /* MSIX pba BAR */


/* Adapter power management register 0 */
#define AL_ADAPTER_PM_0				0x80
#define  AL_ADAPTER_PM_0_PM_NEXT_CAP_MASK	0xff00
#define  AL_ADAPTER_PM_0_PM_NEXT_CAP_SHIFT	8
#define  AL_ADAPTER_PM_0_PM_NEXT_CAP_VAL_MSIX	0x90

/* Adapter power management register 1 */
#define AL_ADAPTER_PM_1			0x84
#define  AL_ADAPTER_PM_1_PME_EN		0x100	/* PM enable */
#define  AL_ADAPTER_PM_1_PWR_STATE_MASK	0x3	/* PM state mask */
#define  AL_ADAPTER_PM_1_PWR_STATE_D3	0x3	/* PM D3 state */

/*
 * Generic control register
 */
#define AL_ADAPTER_SMCC		0x110	/* Sub Master Configuration & Control */
#define AL_ADAPTER_GENERIC_CONTROL_0	0x1E0
/* Enable clock gating */
#define  AL_ADAPTER_GENERIC_CONTROL_0_CLK_GATE_EN	0x01
/* When set, all transactions through the PCI conf & mem BARs get timeout */
#define  AL_ADAPTER_GENERIC_CONTROL_0_ADAPTER_DIS	0x40
#define AL_ADAPTER_GENERIC_CONTROL_11	0x220	/* Generic Control registers */

#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC	AL_BIT(18)
#define AL_ADAPTER_GENERIC_CONTROL_0_ETH_RESET_1GMAC_ON_FLR	AL_BIT(26)

#define AL_ADAPTER_GENERIC_CONTROL_2		0x1E8
#define AL_ADAPTER_GENERIC_CONTROL_3		0x1EC

/*
 * Unit adapter inline functions
 */

/**
 * Perform function level reset and takes care for all needed PCIe config space
 * register save and restore.
 * Utilizes reading/writing to the pcie config space and for performing the
 * actual reset.
 *
 * @param	pcie_read_config_u32
 *		pointer to function that reads register from pcie config space
 * @param	pcie_write_config_u32
 *		register to pcie config space
 *
 * @param	pcie_flr
 *		pointer to function that makes the actual reset.
 *		That function is responsible for performing the post reset
 *		delay.
 *
 * @param	handle
 *		pointer passes to the above functions as first parameter
 */
static inline void al_pcie_perform_flr(int (* pcie_read_config_u32)(void *handle, int where, uint32_t *val),
		   int (* pcie_write_config_u32)(void *handle, int where, uint32_t val),
		   int (* pcie_flr)(void *handle),
		   void *handle)
{
	int i;
	uint32_t cfg_reg_store[11];

	i = 0;
	pcie_read_config_u32(handle, AL_PCI_COMMAND,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_BASE_ADDRESS_DEVICE_ID,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_BASE_ADDRESS_0,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_BASE_ADDRESS_2,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_BASE_ADDRESS_4,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_0,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_1,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_2,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_3,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_APP_CONTROL,
			&cfg_reg_store[i++]);
	pcie_read_config_u32(handle, AL_PCI_VF_BASE_ADDRESS_0,
			&cfg_reg_store[i++]);

	pcie_flr(handle);

	i = 0;
	pcie_write_config_u32(handle, AL_PCI_COMMAND,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_BASE_ADDRESS_DEVICE_ID,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_BASE_ADDRESS_0,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_BASE_ADDRESS_2,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_BASE_ADDRESS_4,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_0,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_1,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_2,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_AXI_CFG_AND_CTR_3,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_APP_CONTROL,
			cfg_reg_store[i++]);
	pcie_write_config_u32(handle, AL_PCI_VF_BASE_ADDRESS_0,
			cfg_reg_store[i++]);
}



#ifdef __cplusplus
}
#endif

#endif
