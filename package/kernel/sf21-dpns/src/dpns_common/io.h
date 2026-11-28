#ifndef _IO_H_
#define _IO_H_


#define L2_MAC_NUM_MAX                   2048

#define LIF_LOOPBACK_CFG_ADDR            0x8c
#define RSTEN_CFG_ADDR                   0xC0
#define MRI_O_EN_O_ADDR                  0xE4
#define TOP_NPU_CLK_DIV_ADDR             0x98
#define TOP_CLK_RATIO_LOAD_ADDR          0x100
#define TOP_NPU_CLK_DIV_MASK             GENMASK(31, 24)

#define CLR_CTRL_RAM_ADDR		0x180004	// 16bit, clear ig,eg,vlan,pvlan,l2,tmu
#define CLR_CTRL_TCAM_ADDR		0x190004	// 5bit, clear tcam, 5 blocks
#define BMU_RD_CLR_EN			0x2800c0
#define TMU_ENQ_RELEASE_EN_BIT	BIT(12)
#define NPU_IACL_MPP_CFG0		0x2C000			// NPU iacl
#define NPU_IACL_MPP_CFG1		0x2c004
#define NPU_EACL_MPP_CFG0		0x30000
#define NPU_EACL_MPP_CFG1		0x30004
#define ACL_MPP_CFG0_BYPASS		BIT(16)
#define ACL_MPP_CFG1_CPU_PORT_ID	GENMASK(25, 20)
#define ACL_MPP_CFG1_OPORT_BITMAP	GENMASK(19, 10)
#define ACL_MPP_CFG1_IPORT_BITMAP	GENMASK(9, 0)

#define NPU_HW_PROC_PORT01_ADDR		0x80064
#define NPU_HW_PROC_PORT23_ADDR		0x80068
#define NPU_HW_PROC_PORT45_ADDR		0x8006c
#define NPU_HW_PROC_PORT67_ADDR		0x80070
#define NPU_HW_PROC_PORT89_ADDR		0x80074
#define PORT_ERR_PKT_PROC_ACT_L		GENMASK(8, 7)
#define PORT_ERR_PKT_PROC_ACT_H		GENMASK(24, 23)

#define NPU_MODEIFY_IPP_CORE_CFG	0x28000

/*npu mib addr*/
#define NPU_MIB_BASE_ADDR		0x380000
#define NPU_MIB_OFFSET			0x4
#define NPU_MIB_PKT_RCV_PORT		0x2000
#define NPU_MIB_NCI_RD_DATA2		0x301c
#define NPU_MIB_NCI_RD_DATA3		0x3020

#define CONFIG1_RGT_ADDR        0x18000C    // L2 Hash poly selection
#define CONFIG2_RGT_ADDR        0x180010    // global function en/disable
#define CONFIG2_MAC_TAB_MSK     BIT(6)
#define CONFIG2_INTF_VID_EN		BIT(7)

#define SE_IRAM_OPT_ADDR    0x18003c        // table operations
#define IRAM_OPT_BUSY       BIT(31)         // 31: 1
#define IRAM_OPT_WR     BIT(24)         // 24: 1
#define IRAM_OPT_ID     GENMASK(20, 16)     // 16: 5bit
#define IRAM_OPT_REQ_ADDR   GENMASK(15, 0)      // 16bit
#define IRAM_W_ADDR(idx)    0x180040 + ((idx)*4)    // 16 x 4
#define IRAM_R_ADDR(idx)    0x180080 + ((idx)*4)    // 16 x 4

#define SE_TCAM_OPT_ADDR    0x19003c
#define TCAM_OPT_BUSY       BIT(31)
#define TCAM_OPT_WR     BIT(24)
#define TCAM_OPT_ID     GENMASK(22, 16)
#define TCAM_OPT_REQ_ADDR   GENMASK(4, 0)
/* 3 x 4, size of the last register is one byte, thus only 9 bytes can be
 * accessed at once */
#define TCAM_SLICE_SIZE		9
#define TCAM_W_ADDR(idx)    (0x190040 + (idx)*4)
#define TCAM_R_ADDR(idx)    (0x190080 + (idx)*4)
#define TCAM_BLK_MODE_ID(BLK_ID) ((BLK_ID)*9+8)
#define TCAM_BLK_RAM_ID(BLK_ID, RAM_INDEX) ((BLK_ID)*9+(RAM_INDEX))

#define SE_TCAM_STATUS  0x190000
#define TCAM_STATUS_P4_BP_OVF BIT(4)
#define TCAM_STATUS_P3_BP_OVF BIT(3)
#define TCAM_STATUS_P2_BP_OVF BIT(2)
#define TCAM_STATUS_P1_BP_OVF BIT(1)
#define TCAM_STATUS_P0_BP_OVF BIT(0)

#define SE_TCAM_CLR 0x190004
#define TCAM_CLR_BLK4_TB BIT(4)
#define TCAM_CLR_BLK3_TB BIT(3)
#define TCAM_CLR_BLK2_TB BIT(2)
#define TCAM_CLR_BLK1_TB BIT(1)
#define TCAM_CLR_BLK0_TB BIT(0)

#define SE_TCAM_BLK_CONFIG0 0x190008
#define TCAM_BLK_CONFIG0_BLK4_CFG GENMASK(14, 12)
#define TCAM_BLK_CONFIG0_BLK3_CFG GENMASK(11, 9)
#define TCAM_BLK_CONFIG0_BLK2_CFG GENMASK(8, 6)
#define TCAM_BLK_CONFIG0_BLK1_CFG GENMASK(5, 3)
#define TCAM_BLK_CONFIG0_BLK0_CFG GENMASK(2, 0)

#define MODIFY_MHDRSD_CFG0_3        0x28004
#define MODIFY_MHDRSD_CFG4_7        0x28008
#define MODIFY_MHDRSD_CFG8_11       0x2800c
#define MODIFY_MHDRSD_CFG12_15      0x28010

#define EACL_MPP_CFG0_FIFO          0x30000
#define EACL_FIFO_AFULL_THRESH      GENMASK(15, 0)

#define PKT_ERR_STG_CFG2            0x80038
#define ARP_REQ_ERR_MODE            GENMASK(14, 12)
#define ARP_REQ_ERR_FWD_EN          BIT(12)
#define ARP_REQ_ERR_UP_EN           BIT(13)
#define ARP_REQ_ERR_DROP_EN         BIT(14)

#define CONFIG_COUNTER_ADDR         0x383024
#define CONFIG_RAM_FLUSH_EN         BIT(0)
#define CONFIG_ROLL_OVER_EN         BIT(4)
#define CONFIG_RD_CLR_EN            BIT(8)          // read_clear enable
#endif // _IO_H_
