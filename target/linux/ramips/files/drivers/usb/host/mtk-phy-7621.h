#ifdef CONFIG_PROJECT_7621
#ifndef __MTK_PHY_7621_H
#define __MTK_PHY_7621_H

#define U2_SR_COEF_7621 28

///////////////////////////////////////////////////////////////////////////////

struct u2phy_reg {
	//0x0
	PHY_LE32 u2phyac0;
	PHY_LE32 u2phyac1;
	PHY_LE32 u2phyac2;
	PHY_LE32 reserve0;
	//0x10
	PHY_LE32 u2phyacr0;
	PHY_LE32 u2phyacr1;
	PHY_LE32 u2phyacr2;
	PHY_LE32 u2phyacr3;
	//0x20
	PHY_LE32 u2phyacr4;
	PHY_LE32 u2phyamon0;
	PHY_LE32 reserve1[2];
	//0x30~0x50
	PHY_LE32 reserve2[12];
	//0x60
	PHY_LE32 u2phydcr0;
	PHY_LE32 u2phydcr1;
	PHY_LE32 u2phydtm0;
	PHY_LE32 u2phydtm1;
	//0x70
	PHY_LE32 u2phydmon0;
	PHY_LE32 u2phydmon1;
	PHY_LE32 u2phydmon2;
	PHY_LE32 u2phydmon3;
	//0x80
	PHY_LE32 u2phybc12c;
	PHY_LE32 u2phybc12c1;
	PHY_LE32 reserve3[2];
	//0x90~0xe0
	PHY_LE32 reserve4[24];
	//0xf0
	PHY_LE32 reserve6[3];
	PHY_LE32 regfcom;
};

//U3D_U2PHYAC0
#define RG_USB20_USBPLL_DIVEN                     (0x7<<28) //30:28
#define RG_USB20_USBPLL_CKCTRL                    (0x3<<26) //27:26
#define RG_USB20_USBPLL_PREDIV                    (0x3<<24) //25:24
#define RG_USB20_USBPLL_FORCE_ON                  (0x1<<23) //23:23
#define RG_USB20_USBPLL_FBDIV                     (0x7f<<16) //22:16
#define RG_USB20_REF_EN                           (0x1<<15) //15:15
#define RG_USB20_INTR_EN                          (0x1<<14) //14:14
#define RG_USB20_BG_TRIM                          (0xf<<8) //11:8
#define RG_USB20_BG_RBSEL                         (0x3<<6) //7:6
#define RG_USB20_BG_RASEL                         (0x3<<4) //5:4
#define RG_USB20_BGR_DIV                          (0x3<<2) //3:2
#define RG_SIFSLV_CHP_EN                          (0x1<<1) //1:1
#define RG_SIFSLV_BGR_EN                          (0x1<<0) //0:0

//U3D_U2PHYAC1
#define RG_USB20_VRT_VREF_SEL                     (0x7<<28) //30:28
#define RG_USB20_TERM_VREF_SEL                    (0x7<<24) //26:24
#define RG_USB20_MPX_SEL                          (0xff<<16) //23:16
#define RG_USB20_MPX_OUT_SEL                      (0x3<<12) //13:12
#define RG_USB20_TX_PH_ROT_SEL                    (0x7<<8) //10:8
#define RG_USB20_USBPLL_ACCEN                     (0x1<<3) //3:3
#define RG_USB20_USBPLL_LF                        (0x1<<2) //2:2
#define RG_USB20_USBPLL_BR                        (0x1<<1) //1:1
#define RG_USB20_USBPLL_BP                        (0x1<<0) //0:0

//U3D_U2PHYAC2
#define RG_SIFSLV_MAC_BANDGAP_EN                  (0x1<<17) //17:17
#define RG_SIFSLV_MAC_CHOPPER_EN                  (0x1<<16) //16:16
#define RG_USB20_CLKREF_REV                       (0xff<<0) //7:0

//U3D_U2PHYACR0
#define RG_USB20_ICUSB_EN                         (0x1<<24) //24:24
#define RG_USB20_HSTX_SRCAL_EN                    (0x1<<23) //23:23
#define RG_USB20_HSTX_SRCTRL                      (0x7<<16) //18:16
#define RG_USB20_LS_CR                            (0x7<<12) //14:12
#define RG_USB20_FS_CR                            (0x7<<8) //10:8
#define RG_USB20_LS_SR                            (0x7<<4) //6:4
#define RG_USB20_FS_SR                            (0x7<<0) //2:0

//U3D_U2PHYACR1
#define RG_USB20_INIT_SQ_EN_DG                    (0x3<<28) //29:28
#define RG_USB20_SQD                              (0x3<<24) //25:24
#define RG_USB20_HSTX_TMODE_SEL                   (0x3<<20) //21:20
#define RG_USB20_HSTX_TMODE_EN                    (0x1<<19) //19:19
#define RG_USB20_PHYD_MONEN                       (0x1<<18) //18:18
#define RG_USB20_INLPBK_EN                        (0x1<<17) //17:17
#define RG_USB20_CHIRP_EN                         (0x1<<16) //16:16
#define RG_USB20_DM_ABIST_SOURCE_EN               (0x1<<15) //15:15
#define RG_USB20_DM_ABIST_SELE                    (0xf<<8) //11:8
#define RG_USB20_DP_ABIST_SOURCE_EN               (0x1<<7) //7:7
#define RG_USB20_DP_ABIST_SELE                    (0xf<<0) //3:0

//U3D_U2PHYACR2
#define RG_USB20_OTG_ABIST_SELE                   (0x7<<29) //31:29
#define RG_USB20_OTG_ABIST_EN                     (0x1<<28) //28:28
#define RG_USB20_OTG_VBUSCMP_EN                   (0x1<<27) //27:27
#define RG_USB20_OTG_VBUSTH                       (0x7<<24) //26:24
#define RG_USB20_DISC_FIT_EN                      (0x1<<22) //22:22
#define RG_USB20_DISCD                            (0x3<<20) //21:20
#define RG_USB20_DISCTH                           (0xf<<16) //19:16
#define RG_USB20_SQCAL_EN                         (0x1<<15) //15:15
#define RG_USB20_SQCAL                            (0xf<<8) //11:8
#define RG_USB20_SQTH                             (0xf<<0) //3:0

//U3D_U2PHYACR3
#define RG_USB20_HSTX_DBIST                       (0xf<<28) //31:28
#define RG_USB20_HSTX_BIST_EN                     (0x1<<26) //26:26
#define RG_USB20_HSTX_I_EN_MODE                   (0x3<<24) //25:24
#define RG_USB20_HSRX_TMODE_EN                    (0x1<<23) //23:23
#define RG_USB20_HSRX_BIAS_EN_SEL                 (0x3<<20) //21:20
#define RG_USB20_USB11_TMODE_EN                   (0x1<<19) //19:19
#define RG_USB20_TMODE_FS_LS_TX_EN                (0x1<<18) //18:18
#define RG_USB20_TMODE_FS_LS_RCV_EN               (0x1<<17) //17:17
#define RG_USB20_TMODE_FS_LS_MODE                 (0x1<<16) //16:16
#define RG_USB20_HS_TERM_EN_MODE                  (0x3<<13) //14:13
#define RG_USB20_PUPD_BIST_EN                     (0x1<<12) //12:12
#define RG_USB20_EN_PU_DM                         (0x1<<11) //11:11
#define RG_USB20_EN_PD_DM                         (0x1<<10) //10:10
#define RG_USB20_EN_PU_DP                         (0x1<<9) //9:9
#define RG_USB20_EN_PD_DP                         (0x1<<8) //8:8
#define RG_USB20_PHY_REV                          (0xff<<0) //7:0

//U3D_U2PHYACR4
#define RG_USB20_DP_100K_MODE                     (0x1<<18) //18:18
#define RG_USB20_DM_100K_EN                       (0x1<<17) //17:17
#define USB20_DP_100K_EN                          (0x1<<16) //16:16
#define USB20_GPIO_DM_I                           (0x1<<15) //15:15
#define USB20_GPIO_DP_I                           (0x1<<14) //14:14
#define USB20_GPIO_DM_OE                          (0x1<<13) //13:13
#define USB20_GPIO_DP_OE                          (0x1<<12) //12:12
#define RG_USB20_GPIO_CTL                         (0x1<<9) //9:9
#define USB20_GPIO_MODE                           (0x1<<8) //8:8
#define RG_USB20_TX_BIAS_EN                       (0x1<<5) //5:5
#define RG_USB20_TX_VCMPDN_EN                     (0x1<<4) //4:4
#define RG_USB20_HS_SQ_EN_MODE                    (0x3<<2) //3:2
#define RG_USB20_HS_RCV_EN_MODE                   (0x3<<0) //1:0

//U3D_U2PHYAMON0
#define RGO_USB20_GPIO_DM_O                       (0x1<<1) //1:1
#define RGO_USB20_GPIO_DP_O                       (0x1<<0) //0:0

//U3D_U2PHYDCR0
#define RG_USB20_CDR_TST                          (0x3<<30) //31:30
#define RG_USB20_GATED_ENB                        (0x1<<29) //29:29
#define RG_USB20_TESTMODE                         (0x3<<26) //27:26
#define RG_USB20_PLL_STABLE                       (0x1<<25) //25:25
#define RG_USB20_PLL_FORCE_ON                     (0x1<<24) //24:24
#define RG_USB20_PHYD_RESERVE                     (0xffff<<8) //23:8
#define RG_USB20_EBTHRLD                          (0x1<<7) //7:7
#define RG_USB20_EARLY_HSTX_I                     (0x1<<6) //6:6
#define RG_USB20_TX_TST                           (0x1<<5) //5:5
#define RG_USB20_NEGEDGE_ENB                      (0x1<<4) //4:4
#define RG_USB20_CDR_FILT                         (0xf<<0) //3:0

//U3D_U2PHYDCR1
#define RG_USB20_PROBE_SEL                        (0xff<<24) //31:24
#define RG_USB20_DRVVBUS                          (0x1<<23) //23:23
#define RG_DEBUG_EN                               (0x1<<22) //22:22
#define RG_USB20_OTG_PROBE                        (0x3<<20) //21:20
#define RG_USB20_SW_PLLMODE                       (0x3<<18) //19:18
#define RG_USB20_BERTH                            (0x3<<16) //17:16
#define RG_USB20_LBMODE                           (0x3<<13) //14:13
#define RG_USB20_FORCE_TAP                        (0x1<<12) //12:12
#define RG_USB20_TAPSEL                           (0xfff<<0) //11:0

//U3D_U2PHYDTM0
#define RG_UART_MODE                              (0x3<<30) //31:30
#define FORCE_UART_I                              (0x1<<29) //29:29
#define FORCE_UART_BIAS_EN                        (0x1<<28) //28:28
#define FORCE_UART_TX_OE                          (0x1<<27) //27:27
#define FORCE_UART_EN                             (0x1<<26) //26:26
#define FORCE_USB_CLKEN                           (0x1<<25) //25:25
#define FORCE_DRVVBUS                             (0x1<<24) //24:24
#define FORCE_DATAIN                              (0x1<<23) //23:23
#define FORCE_TXVALID                             (0x1<<22) //22:22
#define FORCE_DM_PULLDOWN                         (0x1<<21) //21:21
#define FORCE_DP_PULLDOWN                         (0x1<<20) //20:20
#define FORCE_XCVRSEL                             (0x1<<19) //19:19
#define FORCE_SUSPENDM                            (0x1<<18) //18:18
#define FORCE_TERMSEL                             (0x1<<17) //17:17
#define FORCE_OPMODE                              (0x1<<16) //16:16
#define UTMI_MUXSEL                               (0x1<<15) //15:15
#define RG_RESET                                  (0x1<<14) //14:14
#define RG_DATAIN                                 (0xf<<10) //13:10
#define RG_TXVALIDH                               (0x1<<9) //9:9
#define RG_TXVALID                                (0x1<<8) //8:8
#define RG_DMPULLDOWN                             (0x1<<7) //7:7
#define RG_DPPULLDOWN                             (0x1<<6) //6:6
#define RG_XCVRSEL                                (0x3<<4) //5:4
#define RG_SUSPENDM                               (0x1<<3) //3:3
#define RG_TERMSEL                                (0x1<<2) //2:2
#define RG_OPMODE                                 (0x3<<0) //1:0

//U3D_U2PHYDTM1
#define RG_USB20_PRBS7_EN                         (0x1<<31) //31:31
#define RG_USB20_PRBS7_BITCNT                     (0x3f<<24) //29:24
#define RG_USB20_CLK48M_EN                        (0x1<<23) //23:23
#define RG_USB20_CLK60M_EN                        (0x1<<22) //22:22
#define RG_UART_I                                 (0x1<<19) //19:19
#define RG_UART_BIAS_EN                           (0x1<<18) //18:18
#define RG_UART_TX_OE                             (0x1<<17) //17:17
#define RG_UART_EN                                (0x1<<16) //16:16
#define FORCE_VBUSVALID                           (0x1<<13) //13:13
#define FORCE_SESSEND                             (0x1<<12) //12:12
#define FORCE_BVALID                              (0x1<<11) //11:11
#define FORCE_AVALID                              (0x1<<10) //10:10
#define FORCE_IDDIG                               (0x1<<9) //9:9
#define FORCE_IDPULLUP                            (0x1<<8) //8:8
#define RG_VBUSVALID                              (0x1<<5) //5:5
#define RG_SESSEND                                (0x1<<4) //4:4
#define RG_BVALID                                 (0x1<<3) //3:3
#define RG_AVALID                                 (0x1<<2) //2:2
#define RG_IDDIG                                  (0x1<<1) //1:1
#define RG_IDPULLUP                               (0x1<<0) //0:0

//U3D_U2PHYDMON0
#define RG_USB20_PRBS7_BERTH                      (0xff<<0) //7:0

//U3D_U2PHYDMON1
#define USB20_UART_O                              (0x1<<31) //31:31
#define RGO_USB20_LB_PASS                         (0x1<<30) //30:30
#define RGO_USB20_LB_DONE                         (0x1<<29) //29:29
#define AD_USB20_BVALID                           (0x1<<28) //28:28
#define USB20_IDDIG                               (0x1<<27) //27:27
#define AD_USB20_VBUSVALID                        (0x1<<26) //26:26
#define AD_USB20_SESSEND                          (0x1<<25) //25:25
#define AD_USB20_AVALID                           (0x1<<24) //24:24
#define USB20_LINE_STATE                          (0x3<<22) //23:22
#define USB20_HST_DISCON                          (0x1<<21) //21:21
#define USB20_TX_READY                            (0x1<<20) //20:20
#define USB20_RX_ERROR                            (0x1<<19) //19:19
#define USB20_RX_ACTIVE                           (0x1<<18) //18:18
#define USB20_RX_VALIDH                           (0x1<<17) //17:17
#define USB20_RX_VALID                            (0x1<<16) //16:16
#define USB20_DATA_OUT                            (0xffff<<0) //15:0

//U3D_U2PHYDMON2
#define RGO_TXVALID_CNT                           (0xff<<24) //31:24
#define RGO_RXACTIVE_CNT                          (0xff<<16) //23:16
#define RGO_USB20_LB_BERCNT                       (0xff<<8) //15:8
#define USB20_PROBE_OUT                           (0xff<<0) //7:0

//U3D_U2PHYDMON3
#define RGO_USB20_PRBS7_ERRCNT                    (0xffff<<16) //31:16
#define RGO_USB20_PRBS7_DONE                      (0x1<<3) //3:3
#define RGO_USB20_PRBS7_LOCK                      (0x1<<2) //2:2
#define RGO_USB20_PRBS7_PASS                      (0x1<<1) //1:1
#define RGO_USB20_PRBS7_PASSTH                    (0x1<<0) //0:0

//U3D_U2PHYBC12C
#define RG_SIFSLV_CHGDT_DEGLCH_CNT                (0xf<<28) //31:28
#define RG_SIFSLV_CHGDT_CTRL_CNT                  (0xf<<24) //27:24
#define RG_SIFSLV_CHGDT_FORCE_MODE                (0x1<<16) //16:16
#define RG_CHGDT_ISRC_LEV                         (0x3<<14) //15:14
#define RG_CHGDT_VDATSRC                          (0x1<<13) //13:13
#define RG_CHGDT_BGVREF_SEL                       (0x7<<10) //12:10
#define RG_CHGDT_RDVREF_SEL                       (0x3<<8) //9:8
#define RG_CHGDT_ISRC_DP                          (0x1<<7) //7:7
#define RG_SIFSLV_CHGDT_OPOUT_DM                  (0x1<<6) //6:6
#define RG_CHGDT_VDAT_DM                          (0x1<<5) //5:5
#define RG_CHGDT_OPOUT_DP                         (0x1<<4) //4:4
#define RG_SIFSLV_CHGDT_VDAT_DP                   (0x1<<3) //3:3
#define RG_SIFSLV_CHGDT_COMP_EN                   (0x1<<2) //2:2
#define RG_SIFSLV_CHGDT_OPDRV_EN                  (0x1<<1) //1:1
#define RG_CHGDT_EN                               (0x1<<0) //0:0

//U3D_U2PHYBC12C1
#define RG_CHGDT_REV                              (0xff<<0) //7:0

//U3D_REGFCOM
#define RG_PAGE                                   (0xff<<24) //31:24
#define I2C_MODE                                  (0x1<<16) //16:16


/* OFFSET  */

//U3D_U2PHYAC0
#define RG_USB20_USBPLL_DIVEN_OFST                (28)
#define RG_USB20_USBPLL_CKCTRL_OFST               (26)
#define RG_USB20_USBPLL_PREDIV_OFST               (24)
#define RG_USB20_USBPLL_FORCE_ON_OFST             (23)
#define RG_USB20_USBPLL_FBDIV_OFST                (16)
#define RG_USB20_REF_EN_OFST                      (15)
#define RG_USB20_INTR_EN_OFST                     (14)
#define RG_USB20_BG_TRIM_OFST                     (8)
#define RG_USB20_BG_RBSEL_OFST                    (6)
#define RG_USB20_BG_RASEL_OFST                    (4)
#define RG_USB20_BGR_DIV_OFST                     (2)
#define RG_SIFSLV_CHP_EN_OFST                     (1)
#define RG_SIFSLV_BGR_EN_OFST                     (0)

//U3D_U2PHYAC1
#define RG_USB20_VRT_VREF_SEL_OFST                (28)
#define RG_USB20_TERM_VREF_SEL_OFST               (24)
#define RG_USB20_MPX_SEL_OFST                     (16)
#define RG_USB20_MPX_OUT_SEL_OFST                 (12)
#define RG_USB20_TX_PH_ROT_SEL_OFST               (8)
#define RG_USB20_USBPLL_ACCEN_OFST                (3)
#define RG_USB20_USBPLL_LF_OFST                   (2)
#define RG_USB20_USBPLL_BR_OFST                   (1)
#define RG_USB20_USBPLL_BP_OFST                   (0)

//U3D_U2PHYAC2
#define RG_SIFSLV_MAC_BANDGAP_EN_OFST             (17)
#define RG_SIFSLV_MAC_CHOPPER_EN_OFST             (16)
#define RG_USB20_CLKREF_REV_OFST                  (0)

//U3D_U2PHYACR0
#define RG_USB20_ICUSB_EN_OFST                    (24)
#define RG_USB20_HSTX_SRCAL_EN_OFST               (23)
#define RG_USB20_HSTX_SRCTRL_OFST                 (16)
#define RG_USB20_LS_CR_OFST                       (12)
#define RG_USB20_FS_CR_OFST                       (8)
#define RG_USB20_LS_SR_OFST                       (4)
#define RG_USB20_FS_SR_OFST                       (0)

//U3D_U2PHYACR1
#define RG_USB20_INIT_SQ_EN_DG_OFST               (28)
#define RG_USB20_SQD_OFST                         (24)
#define RG_USB20_HSTX_TMODE_SEL_OFST              (20)
#define RG_USB20_HSTX_TMODE_EN_OFST               (19)
#define RG_USB20_PHYD_MONEN_OFST                  (18)
#define RG_USB20_INLPBK_EN_OFST                   (17)
#define RG_USB20_CHIRP_EN_OFST                    (16)
#define RG_USB20_DM_ABIST_SOURCE_EN_OFST          (15)
#define RG_USB20_DM_ABIST_SELE_OFST               (8)
#define RG_USB20_DP_ABIST_SOURCE_EN_OFST          (7)
#define RG_USB20_DP_ABIST_SELE_OFST               (0)

//U3D_U2PHYACR2
#define RG_USB20_OTG_ABIST_SELE_OFST              (29)
#define RG_USB20_OTG_ABIST_EN_OFST                (28)
#define RG_USB20_OTG_VBUSCMP_EN_OFST              (27)
#define RG_USB20_OTG_VBUSTH_OFST                  (24)
#define RG_USB20_DISC_FIT_EN_OFST                 (22)
#define RG_USB20_DISCD_OFST                       (20)
#define RG_USB20_DISCTH_OFST                      (16)
#define RG_USB20_SQCAL_EN_OFST                    (15)
#define RG_USB20_SQCAL_OFST                       (8)
#define RG_USB20_SQTH_OFST                        (0)

//U3D_U2PHYACR3
#define RG_USB20_HSTX_DBIST_OFST                  (28)
#define RG_USB20_HSTX_BIST_EN_OFST                (26)
#define RG_USB20_HSTX_I_EN_MODE_OFST              (24)
#define RG_USB20_HSRX_TMODE_EN_OFST               (23)
#define RG_USB20_HSRX_BIAS_EN_SEL_OFST            (20)
#define RG_USB20_USB11_TMODE_EN_OFST              (19)
#define RG_USB20_TMODE_FS_LS_TX_EN_OFST           (18)
#define RG_USB20_TMODE_FS_LS_RCV_EN_OFST          (17)
#define RG_USB20_TMODE_FS_LS_MODE_OFST            (16)
#define RG_USB20_HS_TERM_EN_MODE_OFST             (13)
#define RG_USB20_PUPD_BIST_EN_OFST                (12)
#define RG_USB20_EN_PU_DM_OFST                    (11)
#define RG_USB20_EN_PD_DM_OFST                    (10)
#define RG_USB20_EN_PU_DP_OFST                    (9)
#define RG_USB20_EN_PD_DP_OFST                    (8)
#define RG_USB20_PHY_REV_OFST                     (0)

//U3D_U2PHYACR4
#define RG_USB20_DP_100K_MODE_OFST                (18)
#define RG_USB20_DM_100K_EN_OFST                  (17)
#define USB20_DP_100K_EN_OFST                     (16)
#define USB20_GPIO_DM_I_OFST                      (15)
#define USB20_GPIO_DP_I_OFST                      (14)
#define USB20_GPIO_DM_OE_OFST                     (13)
#define USB20_GPIO_DP_OE_OFST                     (12)
#define RG_USB20_GPIO_CTL_OFST                    (9)
#define USB20_GPIO_MODE_OFST                      (8)
#define RG_USB20_TX_BIAS_EN_OFST                  (5)
#define RG_USB20_TX_VCMPDN_EN_OFST                (4)
#define RG_USB20_HS_SQ_EN_MODE_OFST               (2)
#define RG_USB20_HS_RCV_EN_MODE_OFST              (0)

//U3D_U2PHYAMON0
#define RGO_USB20_GPIO_DM_O_OFST                  (1)
#define RGO_USB20_GPIO_DP_O_OFST                  (0)

//U3D_U2PHYDCR0
#define RG_USB20_CDR_TST_OFST                     (30)
#define RG_USB20_GATED_ENB_OFST                   (29)
#define RG_USB20_TESTMODE_OFST                    (26)
#define RG_USB20_PLL_STABLE_OFST                  (25)
#define RG_USB20_PLL_FORCE_ON_OFST                (24)
#define RG_USB20_PHYD_RESERVE_OFST                (8)
#define RG_USB20_EBTHRLD_OFST                     (7)
#define RG_USB20_EARLY_HSTX_I_OFST                (6)
#define RG_USB20_TX_TST_OFST                      (5)
#define RG_USB20_NEGEDGE_ENB_OFST                 (4)
#define RG_USB20_CDR_FILT_OFST                    (0)

//U3D_U2PHYDCR1
#define RG_USB20_PROBE_SEL_OFST                   (24)
#define RG_USB20_DRVVBUS_OFST                     (23)
#define RG_DEBUG_EN_OFST                          (22)
#define RG_USB20_OTG_PROBE_OFST                   (20)
#define RG_USB20_SW_PLLMODE_OFST                  (18)
#define RG_USB20_BERTH_OFST                       (16)
#define RG_USB20_LBMODE_OFST                      (13)
#define RG_USB20_FORCE_TAP_OFST                   (12)
#define RG_USB20_TAPSEL_OFST                      (0)

//U3D_U2PHYDTM0
#define RG_UART_MODE_OFST                         (30)
#define FORCE_UART_I_OFST                         (29)
#define FORCE_UART_BIAS_EN_OFST                   (28)
#define FORCE_UART_TX_OE_OFST                     (27)
#define FORCE_UART_EN_OFST                        (26)
#define FORCE_USB_CLKEN_OFST                      (25)
#define FORCE_DRVVBUS_OFST                        (24)
#define FORCE_DATAIN_OFST                         (23)
#define FORCE_TXVALID_OFST                        (22)
#define FORCE_DM_PULLDOWN_OFST                    (21)
#define FORCE_DP_PULLDOWN_OFST                    (20)
#define FORCE_XCVRSEL_OFST                        (19)
#define FORCE_SUSPENDM_OFST                       (18)
#define FORCE_TERMSEL_OFST                        (17)
#define FORCE_OPMODE_OFST                         (16)
#define UTMI_MUXSEL_OFST                          (15)
#define RG_RESET_OFST                             (14)
#define RG_DATAIN_OFST                            (10)
#define RG_TXVALIDH_OFST                          (9)
#define RG_TXVALID_OFST                           (8)
#define RG_DMPULLDOWN_OFST                        (7)
#define RG_DPPULLDOWN_OFST                        (6)
#define RG_XCVRSEL_OFST                           (4)
#define RG_SUSPENDM_OFST                          (3)
#define RG_TERMSEL_OFST                           (2)
#define RG_OPMODE_OFST                            (0)

//U3D_U2PHYDTM1
#define RG_USB20_PRBS7_EN_OFST                    (31)
#define RG_USB20_PRBS7_BITCNT_OFST                (24)
#define RG_USB20_CLK48M_EN_OFST                   (23)
#define RG_USB20_CLK60M_EN_OFST                   (22)
#define RG_UART_I_OFST                            (19)
#define RG_UART_BIAS_EN_OFST                      (18)
#define RG_UART_TX_OE_OFST                        (17)
#define RG_UART_EN_OFST                           (16)
#define FORCE_VBUSVALID_OFST                      (13)
#define FORCE_SESSEND_OFST                        (12)
#define FORCE_BVALID_OFST                         (11)
#define FORCE_AVALID_OFST                         (10)
#define FORCE_IDDIG_OFST                          (9)
#define FORCE_IDPULLUP_OFST                       (8)
#define RG_VBUSVALID_OFST                         (5)
#define RG_SESSEND_OFST                           (4)
#define RG_BVALID_OFST                            (3)
#define RG_AVALID_OFST                            (2)
#define RG_IDDIG_OFST                             (1)
#define RG_IDPULLUP_OFST                          (0)

//U3D_U2PHYDMON0
#define RG_USB20_PRBS7_BERTH_OFST                 (0)

//U3D_U2PHYDMON1
#define USB20_UART_O_OFST                         (31)
#define RGO_USB20_LB_PASS_OFST                    (30)
#define RGO_USB20_LB_DONE_OFST                    (29)
#define AD_USB20_BVALID_OFST                      (28)
#define USB20_IDDIG_OFST                          (27)
#define AD_USB20_VBUSVALID_OFST                   (26)
#define AD_USB20_SESSEND_OFST                     (25)
#define AD_USB20_AVALID_OFST                      (24)
#define USB20_LINE_STATE_OFST                     (22)
#define USB20_HST_DISCON_OFST                     (21)
#define USB20_TX_READY_OFST                       (20)
#define USB20_RX_ERROR_OFST                       (19)
#define USB20_RX_ACTIVE_OFST                      (18)
#define USB20_RX_VALIDH_OFST                      (17)
#define USB20_RX_VALID_OFST                       (16)
#define USB20_DATA_OUT_OFST                       (0)

//U3D_U2PHYDMON2
#define RGO_TXVALID_CNT_OFST                      (24)
#define RGO_RXACTIVE_CNT_OFST                     (16)
#define RGO_USB20_LB_BERCNT_OFST                  (8)
#define USB20_PROBE_OUT_OFST                      (0)

//U3D_U2PHYDMON3
#define RGO_USB20_PRBS7_ERRCNT_OFST               (16)
#define RGO_USB20_PRBS7_DONE_OFST                 (3)
#define RGO_USB20_PRBS7_LOCK_OFST                 (2)
#define RGO_USB20_PRBS7_PASS_OFST                 (1)
#define RGO_USB20_PRBS7_PASSTH_OFST               (0)

//U3D_U2PHYBC12C
#define RG_SIFSLV_CHGDT_DEGLCH_CNT_OFST           (28)
#define RG_SIFSLV_CHGDT_CTRL_CNT_OFST             (24)
#define RG_SIFSLV_CHGDT_FORCE_MODE_OFST           (16)
#define RG_CHGDT_ISRC_LEV_OFST                    (14)
#define RG_CHGDT_VDATSRC_OFST                     (13)
#define RG_CHGDT_BGVREF_SEL_OFST                  (10)
#define RG_CHGDT_RDVREF_SEL_OFST                  (8)
#define RG_CHGDT_ISRC_DP_OFST                     (7)
#define RG_SIFSLV_CHGDT_OPOUT_DM_OFST             (6)
#define RG_CHGDT_VDAT_DM_OFST                     (5)
#define RG_CHGDT_OPOUT_DP_OFST                    (4)
#define RG_SIFSLV_CHGDT_VDAT_DP_OFST              (3)
#define RG_SIFSLV_CHGDT_COMP_EN_OFST              (2)
#define RG_SIFSLV_CHGDT_OPDRV_EN_OFST             (1)
#define RG_CHGDT_EN_OFST                          (0)

//U3D_U2PHYBC12C1
#define RG_CHGDT_REV_OFST                         (0)

//U3D_REGFCOM
#define RG_PAGE_OFST                              (24)
#define I2C_MODE_OFST                             (16)


///////////////////////////////////////////////////////////////////////////////

struct u3phya_reg {
	//0x0
	PHY_LE32 reg0;
	PHY_LE32 reg1;
	PHY_LE32 reg2;
	PHY_LE32 reg3;
	//0x10
	PHY_LE32 reg4;
	PHY_LE32 reg5;
	PHY_LE32 reg6;
	PHY_LE32 reg7;
	//0x20
	PHY_LE32 reg8;
	PHY_LE32 reg9;
	PHY_LE32 rega;
	PHY_LE32 regb;
	//0x30
	PHY_LE32 regc;
	PHY_LE32 regd;
	PHY_LE32 rege;
};

//U3D_reg0
#define RG_SSUSB_BGR_EN                           (0x1<<31) //31:31
#define RG_SSUSB_CHPEN                            (0x1<<30) //30:30
#define RG_SSUSB_BG_DIV                           (0x3<<28) //29:28
#define RG_SSUSB_INTR_EN                          (0x1<<26) //26:26
#define RG_SSUSB_MPX_OUT_SEL                      (0x3<<24) //25:24
#define RG_SSUSB_MPX_SEL                          (0xff<<16) //23:16
#define RG_SSUSB_REF_EN                           (0x1<<15) //15:15
#define RG_SSUSB_VRT_VREF_SEL                     (0xf<<11) //14:11
#define RG_SSUSB_BG_RASEL                         (0x3<<9) //10:9
#define RG_SSUSB_BG_RBSEL                         (0x3<<7) //8:7
#define RG_SSUSB_BG_MONEN                         (0x1<<6) //6:6
#define RG_PCIE_CLKDRV_OFFSET                     (0x3<<0) //1:0

//U3D_reg1
#define RG_PCIE_CLKDRV_SLEW                       (0x3<<30) //31:30
#define RG_PCIE_CLKDRV_AMP                        (0x7<<27) //29:27
#define RG_SSUSB_XTAL_TST_A2DCK_EN                (0x1<<26) //26:26
#define RG_SSUSB_XTAL_MON_EN                      (0x1<<25) //25:25
#define RG_SSUSB_XTAL_HYS                         (0x1<<24) //24:24
#define RG_SSUSB_XTAL_TOP_RESERVE                 (0xffff<<8) //23:8
#define RG_SSUSB_SYSPLL_RESERVE                   (0xf<<4) //7:4
#define RG_SSUSB_SYSPLL_FBSEL                     (0x3<<2) //3:2
#define RG_SSUSB_SYSPLL_PREDIV                    (0x3<<0) //1:0

//U3D_reg2
#define RG_SSUSB_SYSPLL_LF                        (0x1<<31) //31:31
#define RG_SSUSB_SYSPLL_FBDIV                     (0x7f<<24) //30:24
#define RG_SSUSB_SYSPLL_POSDIV                    (0x3<<22) //23:22
#define RG_SSUSB_SYSPLL_VCO_DIV_SEL               (0x1<<21) //21:21
#define RG_SSUSB_SYSPLL_BLP                       (0x1<<20) //20:20
#define RG_SSUSB_SYSPLL_BP                        (0x1<<19) //19:19
#define RG_SSUSB_SYSPLL_BR                        (0x1<<18) //18:18
#define RG_SSUSB_SYSPLL_BC                        (0x1<<17) //17:17
#define RG_SSUSB_SYSPLL_DIVEN                     (0x7<<14) //16:14
#define RG_SSUSB_SYSPLL_FPEN                      (0x1<<13) //13:13
#define RG_SSUSB_SYSPLL_MONCK_EN                  (0x1<<12) //12:12
#define RG_SSUSB_SYSPLL_MONVC_EN                  (0x1<<11) //11:11
#define RG_SSUSB_SYSPLL_MONREF_EN                 (0x1<<10) //10:10
#define RG_SSUSB_SYSPLL_VOD_EN                    (0x1<<9) //9:9
#define RG_SSUSB_SYSPLL_CK_SEL                    (0x1<<8) //8:8

//U3D_reg3
#define RG_SSUSB_SYSPLL_TOP_RESERVE               (0xffff<<16) //31:16

//U3D_reg4
#define RG_SSUSB_SYSPLL_PCW_NCPO                  (0x7fffffff<<1) //31:1

//U3D_reg5
#define RG_SSUSB_SYSPLL_DDS_PI_C                  (0x7<<29) //31:29
#define RG_SSUSB_SYSPLL_DDS_HF_EN                 (0x1<<28) //28:28
#define RG_SSUSB_SYSPLL_DDS_PREDIV2               (0x1<<27) //27:27
#define RG_SSUSB_SYSPLL_DDS_POSTDIV2              (0x1<<26) //26:26
#define RG_SSUSB_SYSPLL_DDS_PI_PL_EN              (0x1<<25) //25:25
#define RG_SSUSB_SYSPLL_DDS_PI_RST_SEL            (0x1<<24) //24:24
#define RG_SSUSB_SYSPLL_DDS_MONEN                 (0x1<<23) //23:23
#define RG_SSUSB_SYSPLL_DDS_LPF_EN                (0x1<<22) //22:22
#define RG_SSUSB_SYSPLL_CLK_PH_INV                (0x1<<21) //21:21
#define RG_SSUSB_SYSPLL_DDS_SEL_EXT               (0x1<<20) //20:20
#define RG_SSUSB_SYSPLL_DDS_DMY                   (0xffff<<0) //15:0

//U3D_reg6
#define RG_SSUSB_TX250MCK_INVB                    (0x1<<31) //31:31
#define RG_SSUSB_IDRV_ITAILOP_EN                  (0x1<<30) //30:30
#define RG_SSUSB_IDRV_CALIB                       (0x3f<<24) //29:24
#define RG_SSUSB_TX_R50_FON                       (0x1<<23) //23:23
#define RG_SSUSB_TX_SR                            (0x7<<20) //22:20
#define RG_SSUSB_TX_EIDLE_CM                      (0xf<<16) //19:16
#define RG_SSUSB_RXDET_RSEL                       (0x3<<14) //15:14
#define RG_SSUSB_RXDET_VTHSEL                     (0x3<<12) //13:12
#define RG_SSUSB_CKMON_EN                         (0x1<<11) //11:11
#define RG_SSUSB_CKMON_SEL                        (0x7<<8) //10:8
#define RG_SSUSB_TX_VLMON_EN                      (0x1<<7) //7:7
#define RG_SSUSB_TX_VLMON_SEL                     (0x1<<6) //6:6
#define RG_SSUSB_RXLBTX_EN                        (0x1<<5) //5:5
#define RG_SSUSB_TXLBRX_EN                        (0x1<<4) //4:4

//U3D_reg7
#define RG_SSUSB_RESERVE                          (0xfffff<<12) //31:12
#define RG_SSUSB_PLL_CKCTRL                       (0x3<<10) //11:10
#define RG_SSUSB_PLL_POSDIV                       (0x3<<8) //9:8
#define RG_SSUSB_PLL_AUTOK_LOAD                   (0x1<<7) //7:7
#define RG_SSUSB_PLL_LOAD_RSTB                    (0x1<<6) //6:6
#define RG_SSUSB_PLL_EP_EN                        (0x1<<5) //5:5
#define RG_SSUSB_PLL_VOD_EN                       (0x1<<4) //4:4
#define RG_SSUSB_PLL_V11_EN                       (0x1<<3) //3:3
#define RG_SSUSB_PLL_MONREF_EN                    (0x1<<2) //2:2
#define RG_SSUSB_PLL_MONCK_EN                     (0x1<<1) //1:1
#define RG_SSUSB_PLL_MONVC_EN                     (0x1<<0) //0:0

//U3D_reg8
#define RG_SSUSB_PLL_RESERVE                      (0xffff<<0) //15:0

//U3D_reg9
#define RG_SSUSB_PLL_DDS_DMY                      (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_PRD                      (0xffff<<0) //15:0

//U3D_regA
#define RG_SSUSB_PLL_SSC_PHASE_INI                (0x1<<31) //31:31
#define RG_SSUSB_PLL_SSC_TRI_EN                   (0x1<<30) //30:30
#define RG_SSUSB_PLL_CLK_PH_INV                   (0x1<<29) //29:29
#define RG_SSUSB_PLL_DDS_LPF_EN                   (0x1<<28) //28:28
#define RG_SSUSB_PLL_DDS_VADJ                     (0x7<<21) //23:21
#define RG_SSUSB_PLL_DDS_MONEN                    (0x1<<20) //20:20
#define RG_SSUSB_PLL_DDS_PS_VADJ                  (0x7<<17) //19:17
#define RG_SSUSB_PLL_DDS_SEL_EXT                  (0x1<<16) //16:16
#define RG_SSUSB_CDR_PD_DIV_BYPASS                (0x1<<15) //15:15
#define RG_SSUSB_CDR_PD_DIV_SEL                   (0x1<<14) //14:14
#define RG_SSUSB_CDR_CPBIAS_SEL                   (0x1<<13) //13:13
#define RG_SSUSB_CDR_OSCDET_EN                    (0x1<<12) //12:12
#define RG_SSUSB_CDR_MONMUX                       (0x1<<11) //11:11
#define RG_SSUSB_CDR_CKCTRL                       (0x3<<9) //10:9
#define RG_SSUSB_CDR_ACCEN                        (0x1<<8) //8:8
#define RG_SSUSB_CDR_BYPASS                       (0x3<<6) //7:6
#define RG_SSUSB_CDR_PI_SLEW                      (0x3<<4) //5:4
#define RG_SSUSB_CDR_EPEN                         (0x1<<3) //3:3
#define RG_SSUSB_CDR_AUTOK_LOAD                   (0x1<<2) //2:2
#define RG_SSUSB_CDR_LOAD_RSTB                    (0x1<<1) //1:1
#define RG_SSUSB_CDR_MONEN                        (0x1<<0) //0:0

//U3D_regB
#define RG_SSUSB_CDR_MONEN_DIG                    (0x1<<31) //31:31
#define RG_SSUSB_CDR_REGOD                        (0x3<<29) //30:29
#define RG_SSUSB_RX_DAC_EN                        (0x1<<26) //26:26
#define RG_SSUSB_RX_DAC_PWD                       (0x1<<25) //25:25
#define RG_SSUSB_EQ_CURSEL                        (0x1<<24) //24:24
#define RG_SSUSB_RX_DAC_MUX                       (0x1f<<19) //23:19
#define RG_SSUSB_RX_R2T_EN                        (0x1<<18) //18:18
#define RG_SSUSB_RX_T2R_EN                        (0x1<<17) //17:17
#define RG_SSUSB_RX_50_LOWER                      (0x7<<14) //16:14
#define RG_SSUSB_RX_50_TAR                        (0x3<<12) //13:12
#define RG_SSUSB_RX_SW_CTRL                       (0xf<<7) //10:7
#define RG_PCIE_SIGDET_VTH                        (0x3<<5) //6:5
#define RG_PCIE_SIGDET_LPF                        (0x3<<3) //4:3
#define RG_SSUSB_LFPS_MON_EN                      (0x1<<2) //2:2

//U3D_regC
#define RG_SSUSB_RXAFE_DCMON_SEL                  (0xf<<28) //31:28
#define RG_SSUSB_CDR_RESERVE                      (0xff<<16) //23:16
#define RG_SSUSB_RXAFE_RESERVE                    (0xff<<8) //15:8
#define RG_PCIE_RX_RESERVE                        (0xff<<0) //7:0

//U3D_redD
#define RGS_SSUSB_CDR_NO_OSC                      (0x1<<8) //8:8
#define RGS_SSUSB_RX_DEBUG_RESERVE                (0xff<<0) //7:0

//U3D_regE
#define RG_SSUSB_INT_BIAS_SEL                     (0x1<<4) //4:4
#define RG_SSUSB_EXT_BIAS_SEL                     (0x1<<3) //3:3
#define RG_SSUSB_RX_P1_ENTRY_PASS                 (0x1<<2) //2:2
#define RG_SSUSB_RX_PD_RST                        (0x1<<1) //1:1
#define RG_SSUSB_RX_PD_RST_PASS                   (0x1<<0) //0:0


/* OFFSET */

//U3D_reg0
#define RG_SSUSB_BGR_EN_OFST                      (31)
#define RG_SSUSB_CHPEN_OFST                       (30)
#define RG_SSUSB_BG_DIV_OFST                      (28)
#define RG_SSUSB_INTR_EN_OFST                     (26)
#define RG_SSUSB_MPX_OUT_SEL_OFST                 (24)
#define RG_SSUSB_MPX_SEL_OFST                     (16)
#define RG_SSUSB_REF_EN_OFST                      (15)
#define RG_SSUSB_VRT_VREF_SEL_OFST                (11)
#define RG_SSUSB_BG_RASEL_OFST                    (9)
#define RG_SSUSB_BG_RBSEL_OFST                    (7)
#define RG_SSUSB_BG_MONEN_OFST                    (6)
#define RG_PCIE_CLKDRV_OFFSET_OFST                (0)

//U3D_reg1
#define RG_PCIE_CLKDRV_SLEW_OFST                  (30)
#define RG_PCIE_CLKDRV_AMP_OFST                   (27)
#define RG_SSUSB_XTAL_TST_A2DCK_EN_OFST           (26)
#define RG_SSUSB_XTAL_MON_EN_OFST                 (25)
#define RG_SSUSB_XTAL_HYS_OFST                    (24)
#define RG_SSUSB_XTAL_TOP_RESERVE_OFST            (8)
#define RG_SSUSB_SYSPLL_RESERVE_OFST              (4)
#define RG_SSUSB_SYSPLL_FBSEL_OFST                (2)
#define RG_SSUSB_SYSPLL_PREDIV_OFST               (0)

//U3D_reg2
#define RG_SSUSB_SYSPLL_LF_OFST                   (31)
#define RG_SSUSB_SYSPLL_FBDIV_OFST                (24)
#define RG_SSUSB_SYSPLL_POSDIV_OFST               (22)
#define RG_SSUSB_SYSPLL_VCO_DIV_SEL_OFST          (21)
#define RG_SSUSB_SYSPLL_BLP_OFST                  (20)
#define RG_SSUSB_SYSPLL_BP_OFST                   (19)
#define RG_SSUSB_SYSPLL_BR_OFST                   (18)
#define RG_SSUSB_SYSPLL_BC_OFST                   (17)
#define RG_SSUSB_SYSPLL_DIVEN_OFST                (14)
#define RG_SSUSB_SYSPLL_FPEN_OFST                 (13)
#define RG_SSUSB_SYSPLL_MONCK_EN_OFST             (12)
#define RG_SSUSB_SYSPLL_MONVC_EN_OFST             (11)
#define RG_SSUSB_SYSPLL_MONREF_EN_OFST            (10)
#define RG_SSUSB_SYSPLL_VOD_EN_OFST               (9)
#define RG_SSUSB_SYSPLL_CK_SEL_OFST               (8)

//U3D_reg3
#define RG_SSUSB_SYSPLL_TOP_RESERVE_OFST          (16)

//U3D_reg4
#define RG_SSUSB_SYSPLL_PCW_NCPO_OFST             (1)

//U3D_reg5
#define RG_SSUSB_SYSPLL_DDS_PI_C_OFST             (29)
#define RG_SSUSB_SYSPLL_DDS_HF_EN_OFST            (28)
#define RG_SSUSB_SYSPLL_DDS_PREDIV2_OFST          (27)
#define RG_SSUSB_SYSPLL_DDS_POSTDIV2_OFST         (26)
#define RG_SSUSB_SYSPLL_DDS_PI_PL_EN_OFST         (25)
#define RG_SSUSB_SYSPLL_DDS_PI_RST_SEL_OFST       (24)
#define RG_SSUSB_SYSPLL_DDS_MONEN_OFST            (23)
#define RG_SSUSB_SYSPLL_DDS_LPF_EN_OFST           (22)
#define RG_SSUSB_SYSPLL_CLK_PH_INV_OFST           (21)
#define RG_SSUSB_SYSPLL_DDS_SEL_EXT_OFST          (20)
#define RG_SSUSB_SYSPLL_DDS_DMY_OFST              (0)

//U3D_reg6
#define RG_SSUSB_TX250MCK_INVB_OFST               (31)
#define RG_SSUSB_IDRV_ITAILOP_EN_OFST             (30)
#define RG_SSUSB_IDRV_CALIB_OFST                  (24)
#define RG_SSUSB_TX_R50_FON_OFST                  (23)
#define RG_SSUSB_TX_SR_OFST                       (20)
#define RG_SSUSB_TX_EIDLE_CM_OFST                 (16)
#define RG_SSUSB_RXDET_RSEL_OFST                  (14)
#define RG_SSUSB_RXDET_VTHSEL_OFST                (12)
#define RG_SSUSB_CKMON_EN_OFST                    (11)
#define RG_SSUSB_CKMON_SEL_OFST                   (8)
#define RG_SSUSB_TX_VLMON_EN_OFST                 (7)
#define RG_SSUSB_TX_VLMON_SEL_OFST                (6)
#define RG_SSUSB_RXLBTX_EN_OFST                   (5)
#define RG_SSUSB_TXLBRX_EN_OFST                   (4)

//U3D_reg7
#define RG_SSUSB_RESERVE_OFST                     (12)
#define RG_SSUSB_PLL_CKCTRL_OFST                  (10)
#define RG_SSUSB_PLL_POSDIV_OFST                  (8)
#define RG_SSUSB_PLL_AUTOK_LOAD_OFST              (7)
#define RG_SSUSB_PLL_LOAD_RSTB_OFST               (6)
#define RG_SSUSB_PLL_EP_EN_OFST                   (5)
#define RG_SSUSB_PLL_VOD_EN_OFST                  (4)
#define RG_SSUSB_PLL_V11_EN_OFST                  (3)
#define RG_SSUSB_PLL_MONREF_EN_OFST               (2)
#define RG_SSUSB_PLL_MONCK_EN_OFST                (1)
#define RG_SSUSB_PLL_MONVC_EN_OFST                (0)

//U3D_reg8
#define RG_SSUSB_PLL_RESERVE_OFST                 (0)

//U3D_reg9
#define RG_SSUSB_PLL_DDS_DMY_OFST                 (16)
#define RG_SSUSB_PLL_SSC_PRD_OFST                 (0)

//U3D_regA
#define RG_SSUSB_PLL_SSC_PHASE_INI_OFST           (31)
#define RG_SSUSB_PLL_SSC_TRI_EN_OFST              (30)
#define RG_SSUSB_PLL_CLK_PH_INV_OFST              (29)
#define RG_SSUSB_PLL_DDS_LPF_EN_OFST              (28)
#define RG_SSUSB_PLL_DDS_VADJ_OFST                (21)
#define RG_SSUSB_PLL_DDS_MONEN_OFST               (20)
#define RG_SSUSB_PLL_DDS_PS_VADJ_OFST             (17)
#define RG_SSUSB_PLL_DDS_SEL_EXT_OFST             (16)
#define RG_SSUSB_CDR_PD_DIV_BYPASS_OFST           (15)
#define RG_SSUSB_CDR_PD_DIV_SEL_OFST              (14)
#define RG_SSUSB_CDR_CPBIAS_SEL_OFST              (13)
#define RG_SSUSB_CDR_OSCDET_EN_OFST               (12)
#define RG_SSUSB_CDR_MONMUX_OFST                  (11)
#define RG_SSUSB_CDR_CKCTRL_OFST                  (9)
#define RG_SSUSB_CDR_ACCEN_OFST                   (8)
#define RG_SSUSB_CDR_BYPASS_OFST                  (6)
#define RG_SSUSB_CDR_PI_SLEW_OFST                 (4)
#define RG_SSUSB_CDR_EPEN_OFST                    (3)
#define RG_SSUSB_CDR_AUTOK_LOAD_OFST              (2)
#define RG_SSUSB_CDR_LOAD_RSTB_OFST               (1)
#define RG_SSUSB_CDR_MONEN_OFST                   (0)

//U3D_regB
#define RG_SSUSB_CDR_MONEN_DIG_OFST               (31)
#define RG_SSUSB_CDR_REGOD_OFST                   (29)
#define RG_SSUSB_RX_DAC_EN_OFST                   (26)
#define RG_SSUSB_RX_DAC_PWD_OFST                  (25)
#define RG_SSUSB_EQ_CURSEL_OFST                   (24)
#define RG_SSUSB_RX_DAC_MUX_OFST                  (19)
#define RG_SSUSB_RX_R2T_EN_OFST                   (18)
#define RG_SSUSB_RX_T2R_EN_OFST                   (17)
#define RG_SSUSB_RX_50_LOWER_OFST                 (14)
#define RG_SSUSB_RX_50_TAR_OFST                   (12)
#define RG_SSUSB_RX_SW_CTRL_OFST                  (7)
#define RG_PCIE_SIGDET_VTH_OFST                   (5)
#define RG_PCIE_SIGDET_LPF_OFST                   (3)
#define RG_SSUSB_LFPS_MON_EN_OFST                 (2)

//U3D_regC
#define RG_SSUSB_RXAFE_DCMON_SEL_OFST             (28)
#define RG_SSUSB_CDR_RESERVE_OFST                 (16)
#define RG_SSUSB_RXAFE_RESERVE_OFST               (8)
#define RG_PCIE_RX_RESERVE_OFST                   (0)

//U3D_redD
#define RGS_SSUSB_CDR_NO_OSC_OFST                 (8)
#define RGS_SSUSB_RX_DEBUG_RESERVE_OFST           (0)

//U3D_regE
#define RG_SSUSB_INT_BIAS_SEL_OFST                (4)
#define RG_SSUSB_EXT_BIAS_SEL_OFST                (3)
#define RG_SSUSB_RX_P1_ENTRY_PASS_OFST            (2)
#define RG_SSUSB_RX_PD_RST_OFST                   (1)
#define RG_SSUSB_RX_PD_RST_PASS_OFST              (0)

///////////////////////////////////////////////////////////////////////////////

struct u3phya_da_reg {
	//0x0
	PHY_LE32 reg0;
	PHY_LE32 reg1;
	PHY_LE32 reg4;
	PHY_LE32 reg5;
	//0x10
	PHY_LE32 reg6;
	PHY_LE32 reg7;
	PHY_LE32 reg8;
	PHY_LE32 reg9;
	//0x20
	PHY_LE32 reg10;
	PHY_LE32 reg12;
	PHY_LE32 reg13;
	PHY_LE32 reg14;
	//0x30
	PHY_LE32 reg15;
	PHY_LE32 reg16;
	PHY_LE32 reg19;
	PHY_LE32 reg20;
	//0x40
	PHY_LE32 reg21;
	PHY_LE32 reg23;
	PHY_LE32 reg25;
	PHY_LE32 reg26;
	//0x50
	PHY_LE32 reg28;
	PHY_LE32 reg29;
	PHY_LE32 reg30;
	PHY_LE32 reg31;
	//0x60
	PHY_LE32 reg32;
	PHY_LE32 reg33;
};

//U3D_reg0
#define RG_PCIE_SPEED_PE2D                        (0x1<<24) //24:24
#define RG_PCIE_SPEED_PE2H                        (0x1<<23) //23:23
#define RG_PCIE_SPEED_PE1D                        (0x1<<22) //22:22
#define RG_PCIE_SPEED_PE1H                        (0x1<<21) //21:21
#define RG_PCIE_SPEED_U3                          (0x1<<20) //20:20
#define RG_SSUSB_XTAL_EXT_EN_PE2D                 (0x3<<18) //19:18
#define RG_SSUSB_XTAL_EXT_EN_PE2H                 (0x3<<16) //17:16
#define RG_SSUSB_XTAL_EXT_EN_PE1D                 (0x3<<14) //15:14
#define RG_SSUSB_XTAL_EXT_EN_PE1H                 (0x3<<12) //13:12
#define RG_SSUSB_XTAL_EXT_EN_U3                   (0x3<<10) //11:10
#define RG_SSUSB_CDR_REFCK_SEL_PE2D               (0x3<<8) //9:8
#define RG_SSUSB_CDR_REFCK_SEL_PE2H               (0x3<<6) //7:6
#define RG_SSUSB_CDR_REFCK_SEL_PE1D               (0x3<<4) //5:4
#define RG_SSUSB_CDR_REFCK_SEL_PE1H               (0x3<<2) //3:2
#define RG_SSUSB_CDR_REFCK_SEL_U3                 (0x3<<0) //1:0

//U3D_reg1
#define RG_USB20_REFCK_SEL_PE2D                   (0x1<<30) //30:30
#define RG_USB20_REFCK_SEL_PE2H                   (0x1<<29) //29:29
#define RG_USB20_REFCK_SEL_PE1D                   (0x1<<28) //28:28
#define RG_USB20_REFCK_SEL_PE1H                   (0x1<<27) //27:27
#define RG_USB20_REFCK_SEL_U3                     (0x1<<26) //26:26
#define RG_PCIE_REFCK_DIV4_PE2D                   (0x1<<25) //25:25
#define RG_PCIE_REFCK_DIV4_PE2H                   (0x1<<24) //24:24
#define RG_PCIE_REFCK_DIV4_PE1D                   (0x1<<18) //18:18
#define RG_PCIE_REFCK_DIV4_PE1H                   (0x1<<17) //17:17
#define RG_PCIE_REFCK_DIV4_U3                     (0x1<<16) //16:16
#define RG_PCIE_MODE_PE2D                         (0x1<<8) //8:8
#define RG_PCIE_MODE_PE2H                         (0x1<<3) //3:3
#define RG_PCIE_MODE_PE1D                         (0x1<<2) //2:2
#define RG_PCIE_MODE_PE1H                         (0x1<<1) //1:1
#define RG_PCIE_MODE_U3                           (0x1<<0) //0:0

//U3D_reg4
#define RG_SSUSB_PLL_DIVEN_PE2D                   (0x7<<22) //24:22
#define RG_SSUSB_PLL_DIVEN_PE2H                   (0x7<<19) //21:19
#define RG_SSUSB_PLL_DIVEN_PE1D                   (0x7<<16) //18:16
#define RG_SSUSB_PLL_DIVEN_PE1H                   (0x7<<13) //15:13
#define RG_SSUSB_PLL_DIVEN_U3                     (0x7<<10) //12:10
#define RG_SSUSB_PLL_BC_PE2D                      (0x3<<8) //9:8
#define RG_SSUSB_PLL_BC_PE2H                      (0x3<<6) //7:6
#define RG_SSUSB_PLL_BC_PE1D                      (0x3<<4) //5:4
#define RG_SSUSB_PLL_BC_PE1H                      (0x3<<2) //3:2
#define RG_SSUSB_PLL_BC_U3                        (0x3<<0) //1:0

//U3D_reg5
#define RG_SSUSB_PLL_BR_PE2D                      (0x7<<27) //29:27
#define RG_SSUSB_PLL_BR_PE2H                      (0x7<<24) //26:24
#define RG_SSUSB_PLL_BR_PE1D                      (0x7<<21) //23:21
#define RG_SSUSB_PLL_BR_PE1H                      (0x7<<18) //20:18
#define RG_SSUSB_PLL_BR_U3                        (0x7<<15) //17:15
#define RG_SSUSB_PLL_IC_PE2D                      (0x7<<12) //14:12
#define RG_SSUSB_PLL_IC_PE2H                      (0x7<<9) //11:9
#define RG_SSUSB_PLL_IC_PE1D                      (0x7<<6) //8:6
#define RG_SSUSB_PLL_IC_PE1H                      (0x7<<3) //5:3
#define RG_SSUSB_PLL_IC_U3                        (0x7<<0) //2:0

//U3D_reg6
#define RG_SSUSB_PLL_IR_PE2D                      (0xf<<24) //27:24
#define RG_SSUSB_PLL_IR_PE2H                      (0xf<<16) //19:16
#define RG_SSUSB_PLL_IR_PE1D                      (0xf<<8) //11:8
#define RG_SSUSB_PLL_IR_PE1H                      (0xf<<4) //7:4
#define RG_SSUSB_PLL_IR_U3                        (0xf<<0) //3:0

//U3D_reg7
#define RG_SSUSB_PLL_BP_PE2D                      (0xf<<24) //27:24
#define RG_SSUSB_PLL_BP_PE2H                      (0xf<<16) //19:16
#define RG_SSUSB_PLL_BP_PE1D                      (0xf<<8) //11:8
#define RG_SSUSB_PLL_BP_PE1H                      (0xf<<4) //7:4
#define RG_SSUSB_PLL_BP_U3                        (0xf<<0) //3:0

//U3D_reg8
#define RG_SSUSB_PLL_FBKSEL_PE2D                  (0x3<<24) //25:24
#define RG_SSUSB_PLL_FBKSEL_PE2H                  (0x3<<16) //17:16
#define RG_SSUSB_PLL_FBKSEL_PE1D                  (0x3<<8) //9:8
#define RG_SSUSB_PLL_FBKSEL_PE1H                  (0x3<<2) //3:2
#define RG_SSUSB_PLL_FBKSEL_U3                    (0x3<<0) //1:0

//U3D_reg9
#define RG_SSUSB_PLL_FBKDIV_PE2H                  (0x7f<<24) //30:24
#define RG_SSUSB_PLL_FBKDIV_PE1D                  (0x7f<<16) //22:16
#define RG_SSUSB_PLL_FBKDIV_PE1H                  (0x7f<<8) //14:8
#define RG_SSUSB_PLL_FBKDIV_U3                    (0x7f<<0) //6:0

//U3D_reg10
#define RG_SSUSB_PLL_PREDIV_PE2D                  (0x3<<26) //27:26
#define RG_SSUSB_PLL_PREDIV_PE2H                  (0x3<<24) //25:24
#define RG_SSUSB_PLL_PREDIV_PE1D                  (0x3<<18) //19:18
#define RG_SSUSB_PLL_PREDIV_PE1H                  (0x3<<16) //17:16
#define RG_SSUSB_PLL_PREDIV_U3                    (0x3<<8) //9:8
#define RG_SSUSB_PLL_FBKDIV_PE2D                  (0x7f<<0) //6:0

//U3D_reg12
#define RG_SSUSB_PLL_PCW_NCPO_U3                  (0x7fffffff<<0) //30:0

//U3D_reg13
#define RG_SSUSB_PLL_PCW_NCPO_PE1H                (0x7fffffff<<0) //30:0

//U3D_reg14
#define RG_SSUSB_PLL_PCW_NCPO_PE1D                (0x7fffffff<<0) //30:0

//U3D_reg15
#define RG_SSUSB_PLL_PCW_NCPO_PE2H                (0x7fffffff<<0) //30:0

//U3D_reg16
#define RG_SSUSB_PLL_PCW_NCPO_PE2D                (0x7fffffff<<0) //30:0

//U3D_reg19
#define RG_SSUSB_PLL_SSC_DELTA1_PE1H              (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_DELTA1_U3                (0xffff<<0) //15:0

//U3D_reg20
#define RG_SSUSB_PLL_SSC_DELTA1_PE2H              (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_DELTA1_PE1D              (0xffff<<0) //15:0

//U3D_reg21
#define RG_SSUSB_PLL_SSC_DELTA_U3                 (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_DELTA1_PE2D              (0xffff<<0) //15:0

//U3D_reg23
#define RG_SSUSB_PLL_SSC_DELTA_PE1D               (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_DELTA_PE1H               (0xffff<<0) //15:0

//U3D_reg25
#define RG_SSUSB_PLL_SSC_DELTA_PE2D               (0xffff<<16) //31:16
#define RG_SSUSB_PLL_SSC_DELTA_PE2H               (0xffff<<0) //15:0

//U3D_reg26
#define RG_SSUSB_PLL_REFCKDIV_PE2D                (0x1<<25) //25:25
#define RG_SSUSB_PLL_REFCKDIV_PE2H                (0x1<<24) //24:24
#define RG_SSUSB_PLL_REFCKDIV_PE1D                (0x1<<16) //16:16
#define RG_SSUSB_PLL_REFCKDIV_PE1H                (0x1<<8) //8:8
#define RG_SSUSB_PLL_REFCKDIV_U3                  (0x1<<0) //0:0

//U3D_reg28
#define RG_SSUSB_CDR_BPA_PE2D                     (0x3<<24) //25:24
#define RG_SSUSB_CDR_BPA_PE2H                     (0x3<<16) //17:16
#define RG_SSUSB_CDR_BPA_PE1D                     (0x3<<10) //11:10
#define RG_SSUSB_CDR_BPA_PE1H                     (0x3<<8) //9:8
#define RG_SSUSB_CDR_BPA_U3                       (0x3<<0) //1:0

//U3D_reg29
#define RG_SSUSB_CDR_BPB_PE2D                     (0x7<<24) //26:24
#define RG_SSUSB_CDR_BPB_PE2H                     (0x7<<16) //18:16
#define RG_SSUSB_CDR_BPB_PE1D                     (0x7<<6) //8:6
#define RG_SSUSB_CDR_BPB_PE1H                     (0x7<<3) //5:3
#define RG_SSUSB_CDR_BPB_U3                       (0x7<<0) //2:0

//U3D_reg30
#define RG_SSUSB_CDR_BR_PE2D                      (0x7<<24) //26:24
#define RG_SSUSB_CDR_BR_PE2H                      (0x7<<16) //18:16
#define RG_SSUSB_CDR_BR_PE1D                      (0x7<<6) //8:6
#define RG_SSUSB_CDR_BR_PE1H                      (0x7<<3) //5:3
#define RG_SSUSB_CDR_BR_U3                        (0x7<<0) //2:0

//U3D_reg31
#define RG_SSUSB_CDR_FBDIV_PE2H                   (0x7f<<24) //30:24
#define RG_SSUSB_CDR_FBDIV_PE1D                   (0x7f<<16) //22:16
#define RG_SSUSB_CDR_FBDIV_PE1H                   (0x7f<<8) //14:8
#define RG_SSUSB_CDR_FBDIV_U3                     (0x7f<<0) //6:0

//U3D_reg32
#define RG_SSUSB_EQ_RSTEP1_PE2D                   (0x3<<30) //31:30
#define RG_SSUSB_EQ_RSTEP1_PE2H                   (0x3<<28) //29:28
#define RG_SSUSB_EQ_RSTEP1_PE1D                   (0x3<<26) //27:26
#define RG_SSUSB_EQ_RSTEP1_PE1H                   (0x3<<24) //25:24
#define RG_SSUSB_EQ_RSTEP1_U3                     (0x3<<22) //23:22
#define RG_SSUSB_LFPS_DEGLITCH_PE2D               (0x3<<20) //21:20
#define RG_SSUSB_LFPS_DEGLITCH_PE2H               (0x3<<18) //19:18
#define RG_SSUSB_LFPS_DEGLITCH_PE1D               (0x3<<16) //17:16
#define RG_SSUSB_LFPS_DEGLITCH_PE1H               (0x3<<14) //15:14
#define RG_SSUSB_LFPS_DEGLITCH_U3                 (0x3<<12) //13:12
#define RG_SSUSB_CDR_KVSEL_PE2D                   (0x1<<11) //11:11
#define RG_SSUSB_CDR_KVSEL_PE2H                   (0x1<<10) //10:10
#define RG_SSUSB_CDR_KVSEL_PE1D                   (0x1<<9) //9:9
#define RG_SSUSB_CDR_KVSEL_PE1H                   (0x1<<8) //8:8
#define RG_SSUSB_CDR_KVSEL_U3                     (0x1<<7) //7:7
#define RG_SSUSB_CDR_FBDIV_PE2D                   (0x7f<<0) //6:0

//U3D_reg33
#define RG_SSUSB_RX_CMPWD_PE2D                    (0x1<<26) //26:26
#define RG_SSUSB_RX_CMPWD_PE2H                    (0x1<<25) //25:25
#define RG_SSUSB_RX_CMPWD_PE1D                    (0x1<<24) //24:24
#define RG_SSUSB_RX_CMPWD_PE1H                    (0x1<<23) //23:23
#define RG_SSUSB_RX_CMPWD_U3                      (0x1<<16) //16:16
#define RG_SSUSB_EQ_RSTEP2_PE2D                   (0x3<<8) //9:8
#define RG_SSUSB_EQ_RSTEP2_PE2H                   (0x3<<6) //7:6
#define RG_SSUSB_EQ_RSTEP2_PE1D                   (0x3<<4) //5:4
#define RG_SSUSB_EQ_RSTEP2_PE1H                   (0x3<<2) //3:2
#define RG_SSUSB_EQ_RSTEP2_U3                     (0x3<<0) //1:0


/* OFFSET  */

//U3D_reg0
#define RG_PCIE_SPEED_PE2D_OFST                   (24)
#define RG_PCIE_SPEED_PE2H_OFST                   (23)
#define RG_PCIE_SPEED_PE1D_OFST                   (22)
#define RG_PCIE_SPEED_PE1H_OFST                   (21)
#define RG_PCIE_SPEED_U3_OFST                     (20)
#define RG_SSUSB_XTAL_EXT_EN_PE2D_OFST            (18)
#define RG_SSUSB_XTAL_EXT_EN_PE2H_OFST            (16)
#define RG_SSUSB_XTAL_EXT_EN_PE1D_OFST            (14)
#define RG_SSUSB_XTAL_EXT_EN_PE1H_OFST            (12)
#define RG_SSUSB_XTAL_EXT_EN_U3_OFST              (10)
#define RG_SSUSB_CDR_REFCK_SEL_PE2D_OFST          (8)
#define RG_SSUSB_CDR_REFCK_SEL_PE2H_OFST          (6)
#define RG_SSUSB_CDR_REFCK_SEL_PE1D_OFST          (4)
#define RG_SSUSB_CDR_REFCK_SEL_PE1H_OFST          (2)
#define RG_SSUSB_CDR_REFCK_SEL_U3_OFST            (0)

//U3D_reg1
#define RG_USB20_REFCK_SEL_PE2D_OFST              (30)
#define RG_USB20_REFCK_SEL_PE2H_OFST              (29)
#define RG_USB20_REFCK_SEL_PE1D_OFST              (28)
#define RG_USB20_REFCK_SEL_PE1H_OFST              (27)
#define RG_USB20_REFCK_SEL_U3_OFST                (26)
#define RG_PCIE_REFCK_DIV4_PE2D_OFST              (25)
#define RG_PCIE_REFCK_DIV4_PE2H_OFST              (24)
#define RG_PCIE_REFCK_DIV4_PE1D_OFST              (18)
#define RG_PCIE_REFCK_DIV4_PE1H_OFST              (17)
#define RG_PCIE_REFCK_DIV4_U3_OFST                (16)
#define RG_PCIE_MODE_PE2D_OFST                    (8)
#define RG_PCIE_MODE_PE2H_OFST                    (3)
#define RG_PCIE_MODE_PE1D_OFST                    (2)
#define RG_PCIE_MODE_PE1H_OFST                    (1)
#define RG_PCIE_MODE_U3_OFST                      (0)

//U3D_reg4
#define RG_SSUSB_PLL_DIVEN_PE2D_OFST              (22)
#define RG_SSUSB_PLL_DIVEN_PE2H_OFST              (19)
#define RG_SSUSB_PLL_DIVEN_PE1D_OFST              (16)
#define RG_SSUSB_PLL_DIVEN_PE1H_OFST              (13)
#define RG_SSUSB_PLL_DIVEN_U3_OFST                (10)
#define RG_SSUSB_PLL_BC_PE2D_OFST                 (8)
#define RG_SSUSB_PLL_BC_PE2H_OFST                 (6)
#define RG_SSUSB_PLL_BC_PE1D_OFST                 (4)
#define RG_SSUSB_PLL_BC_PE1H_OFST                 (2)
#define RG_SSUSB_PLL_BC_U3_OFST                   (0)

//U3D_reg5
#define RG_SSUSB_PLL_BR_PE2D_OFST                 (27)
#define RG_SSUSB_PLL_BR_PE2H_OFST                 (24)
#define RG_SSUSB_PLL_BR_PE1D_OFST                 (21)
#define RG_SSUSB_PLL_BR_PE1H_OFST                 (18)
#define RG_SSUSB_PLL_BR_U3_OFST                   (15)
#define RG_SSUSB_PLL_IC_PE2D_OFST                 (12)
#define RG_SSUSB_PLL_IC_PE2H_OFST                 (9)
#define RG_SSUSB_PLL_IC_PE1D_OFST                 (6)
#define RG_SSUSB_PLL_IC_PE1H_OFST                 (3)
#define RG_SSUSB_PLL_IC_U3_OFST                   (0)

//U3D_reg6
#define RG_SSUSB_PLL_IR_PE2D_OFST                 (24)
#define RG_SSUSB_PLL_IR_PE2H_OFST                 (16)
#define RG_SSUSB_PLL_IR_PE1D_OFST                 (8)
#define RG_SSUSB_PLL_IR_PE1H_OFST                 (4)
#define RG_SSUSB_PLL_IR_U3_OFST                   (0)

//U3D_reg7
#define RG_SSUSB_PLL_BP_PE2D_OFST                 (24)
#define RG_SSUSB_PLL_BP_PE2H_OFST                 (16)
#define RG_SSUSB_PLL_BP_PE1D_OFST                 (8)
#define RG_SSUSB_PLL_BP_PE1H_OFST                 (4)
#define RG_SSUSB_PLL_BP_U3_OFST                   (0)

//U3D_reg8
#define RG_SSUSB_PLL_FBKSEL_PE2D_OFST             (24)
#define RG_SSUSB_PLL_FBKSEL_PE2H_OFST             (16)
#define RG_SSUSB_PLL_FBKSEL_PE1D_OFST             (8)
#define RG_SSUSB_PLL_FBKSEL_PE1H_OFST             (2)
#define RG_SSUSB_PLL_FBKSEL_U3_OFST               (0)

//U3D_reg9
#define RG_SSUSB_PLL_FBKDIV_PE2H_OFST             (24)
#define RG_SSUSB_PLL_FBKDIV_PE1D_OFST             (16)
#define RG_SSUSB_PLL_FBKDIV_PE1H_OFST             (8)
#define RG_SSUSB_PLL_FBKDIV_U3_OFST               (0)

//U3D_reg10
#define RG_SSUSB_PLL_PREDIV_PE2D_OFST             (26)
#define RG_SSUSB_PLL_PREDIV_PE2H_OFST             (24)
#define RG_SSUSB_PLL_PREDIV_PE1D_OFST             (18)
#define RG_SSUSB_PLL_PREDIV_PE1H_OFST             (16)
#define RG_SSUSB_PLL_PREDIV_U3_OFST               (8)
#define RG_SSUSB_PLL_FBKDIV_PE2D_OFST             (0)

//U3D_reg12
#define RG_SSUSB_PLL_PCW_NCPO_U3_OFST             (0)

//U3D_reg13
#define RG_SSUSB_PLL_PCW_NCPO_PE1H_OFST           (0)

//U3D_reg14
#define RG_SSUSB_PLL_PCW_NCPO_PE1D_OFST           (0)

//U3D_reg15
#define RG_SSUSB_PLL_PCW_NCPO_PE2H_OFST           (0)

//U3D_reg16
#define RG_SSUSB_PLL_PCW_NCPO_PE2D_OFST           (0)

//U3D_reg19
#define RG_SSUSB_PLL_SSC_DELTA1_PE1H_OFST         (16)
#define RG_SSUSB_PLL_SSC_DELTA1_U3_OFST           (0)

//U3D_reg20
#define RG_SSUSB_PLL_SSC_DELTA1_PE2H_OFST         (16)
#define RG_SSUSB_PLL_SSC_DELTA1_PE1D_OFST         (0)

//U3D_reg21
#define RG_SSUSB_PLL_SSC_DELTA_U3_OFST            (16)
#define RG_SSUSB_PLL_SSC_DELTA1_PE2D_OFST         (0)

//U3D_reg23
#define RG_SSUSB_PLL_SSC_DELTA_PE1D_OFST          (16)
#define RG_SSUSB_PLL_SSC_DELTA_PE1H_OFST          (0)

//U3D_reg25
#define RG_SSUSB_PLL_SSC_DELTA_PE2D_OFST          (16)
#define RG_SSUSB_PLL_SSC_DELTA_PE2H_OFST          (0)

//U3D_reg26
#define RG_SSUSB_PLL_REFCKDIV_PE2D_OFST           (25)
#define RG_SSUSB_PLL_REFCKDIV_PE2H_OFST           (24)
#define RG_SSUSB_PLL_REFCKDIV_PE1D_OFST           (16)
#define RG_SSUSB_PLL_REFCKDIV_PE1H_OFST           (8)
#define RG_SSUSB_PLL_REFCKDIV_U3_OFST             (0)

//U3D_reg28
#define RG_SSUSB_CDR_BPA_PE2D_OFST                (24)
#define RG_SSUSB_CDR_BPA_PE2H_OFST                (16)
#define RG_SSUSB_CDR_BPA_PE1D_OFST                (10)
#define RG_SSUSB_CDR_BPA_PE1H_OFST                (8)
#define RG_SSUSB_CDR_BPA_U3_OFST                  (0)

//U3D_reg29
#define RG_SSUSB_CDR_BPB_PE2D_OFST                (24)
#define RG_SSUSB_CDR_BPB_PE2H_OFST                (16)
#define RG_SSUSB_CDR_BPB_PE1D_OFST                (6)
#define RG_SSUSB_CDR_BPB_PE1H_OFST                (3)
#define RG_SSUSB_CDR_BPB_U3_OFST                  (0)

//U3D_reg30
#define RG_SSUSB_CDR_BR_PE2D_OFST                 (24)
#define RG_SSUSB_CDR_BR_PE2H_OFST                 (16)
#define RG_SSUSB_CDR_BR_PE1D_OFST                 (6)
#define RG_SSUSB_CDR_BR_PE1H_OFST                 (3)
#define RG_SSUSB_CDR_BR_U3_OFST                   (0)

//U3D_reg31
#define RG_SSUSB_CDR_FBDIV_PE2H_OFST              (24)
#define RG_SSUSB_CDR_FBDIV_PE1D_OFST              (16)
#define RG_SSUSB_CDR_FBDIV_PE1H_OFST              (8)
#define RG_SSUSB_CDR_FBDIV_U3_OFST                (0)

//U3D_reg32
#define RG_SSUSB_EQ_RSTEP1_PE2D_OFST              (30)
#define RG_SSUSB_EQ_RSTEP1_PE2H_OFST              (28)
#define RG_SSUSB_EQ_RSTEP1_PE1D_OFST              (26)
#define RG_SSUSB_EQ_RSTEP1_PE1H_OFST              (24)
#define RG_SSUSB_EQ_RSTEP1_U3_OFST                (22)
#define RG_SSUSB_LFPS_DEGLITCH_PE2D_OFST          (20)
#define RG_SSUSB_LFPS_DEGLITCH_PE2H_OFST          (18)
#define RG_SSUSB_LFPS_DEGLITCH_PE1D_OFST          (16)
#define RG_SSUSB_LFPS_DEGLITCH_PE1H_OFST          (14)
#define RG_SSUSB_LFPS_DEGLITCH_U3_OFST            (12)
#define RG_SSUSB_CDR_KVSEL_PE2D_OFST              (11)
#define RG_SSUSB_CDR_KVSEL_PE2H_OFST              (10)
#define RG_SSUSB_CDR_KVSEL_PE1D_OFST              (9)
#define RG_SSUSB_CDR_KVSEL_PE1H_OFST              (8)
#define RG_SSUSB_CDR_KVSEL_U3_OFST                (7)
#define RG_SSUSB_CDR_FBDIV_PE2D_OFST              (0)

//U3D_reg33
#define RG_SSUSB_RX_CMPWD_PE2D_OFST               (26)
#define RG_SSUSB_RX_CMPWD_PE2H_OFST               (25)
#define RG_SSUSB_RX_CMPWD_PE1D_OFST               (24)
#define RG_SSUSB_RX_CMPWD_PE1H_OFST               (23)
#define RG_SSUSB_RX_CMPWD_U3_OFST                 (16)
#define RG_SSUSB_EQ_RSTEP2_PE2D_OFST              (8)
#define RG_SSUSB_EQ_RSTEP2_PE2H_OFST              (6)
#define RG_SSUSB_EQ_RSTEP2_PE1D_OFST              (4)
#define RG_SSUSB_EQ_RSTEP2_PE1H_OFST              (2)
#define RG_SSUSB_EQ_RSTEP2_U3_OFST                (0)


///////////////////////////////////////////////////////////////////////////////

struct u3phyd_reg {
	//0x0
	PHY_LE32 phyd_mix0;
	PHY_LE32 phyd_mix1;
	PHY_LE32 phyd_lfps0;
	PHY_LE32 phyd_lfps1;
	//0x10
	PHY_LE32 phyd_impcal0;
	PHY_LE32 phyd_impcal1;
	PHY_LE32 phyd_txpll0;
	PHY_LE32 phyd_txpll1;
	//0x20
	PHY_LE32 phyd_txpll2;
	PHY_LE32 phyd_fl0;
	PHY_LE32 phyd_mix2;
	PHY_LE32 phyd_rx0;
	//0x30
	PHY_LE32 phyd_t2rlb;
	PHY_LE32 phyd_cppat;
	PHY_LE32 phyd_mix3;
	PHY_LE32 phyd_ebufctl;
	//0x40
	PHY_LE32 phyd_pipe0;
	PHY_LE32 phyd_pipe1;
	PHY_LE32 phyd_mix4;
	PHY_LE32 phyd_ckgen0;
	//0x50
	PHY_LE32 phyd_mix5;
	PHY_LE32 phyd_reserved;
	PHY_LE32 phyd_cdr0;
	PHY_LE32 phyd_cdr1;
	//0x60
	PHY_LE32 phyd_pll_0;
	PHY_LE32 phyd_pll_1;
	PHY_LE32 phyd_bcn_det_1;
	PHY_LE32 phyd_bcn_det_2;
	//0x70
	PHY_LE32 eq0;
	PHY_LE32 eq1;
	PHY_LE32 eq2;
	PHY_LE32 eq3;
	//0x80
	PHY_LE32 eq_eye0;
	PHY_LE32 eq_eye1;
	PHY_LE32 eq_eye2;
	PHY_LE32 eq_dfe0;
	//0x90
	PHY_LE32 eq_dfe1;
	PHY_LE32 eq_dfe2;
	PHY_LE32 eq_dfe3;
	PHY_LE32 reserve0;
	//0xa0
	PHY_LE32 phyd_mon0;
	PHY_LE32 phyd_mon1;
	PHY_LE32 phyd_mon2;
	PHY_LE32 phyd_mon3;
	//0xb0
	PHY_LE32 phyd_mon4;
	PHY_LE32 phyd_mon5;
	PHY_LE32 phyd_mon6;
	PHY_LE32 phyd_mon7;
	//0xc0
	PHY_LE32 phya_rx_mon0;
	PHY_LE32 phya_rx_mon1;
	PHY_LE32 phya_rx_mon2;
	PHY_LE32 phya_rx_mon3;
	//0xd0
	PHY_LE32 phya_rx_mon4;
	PHY_LE32 phya_rx_mon5;
	PHY_LE32 phyd_cppat2;
	PHY_LE32 eq_eye3;
	//0xe0
	PHY_LE32 kband_out;
	PHY_LE32 kband_out1;
};

//U3D_PHYD_MIX0
#define RG_SSUSB_P_P3_TX_NG                       (0x1<<31) //31:31
#define RG_SSUSB_TSEQ_EN                          (0x1<<30) //30:30
#define RG_SSUSB_TSEQ_POLEN                       (0x1<<29) //29:29
#define RG_SSUSB_TSEQ_POL                         (0x1<<28) //28:28
#define RG_SSUSB_P_P3_PCLK_NG                     (0x1<<27) //27:27
#define RG_SSUSB_TSEQ_TH                          (0x7<<24) //26:24
#define RG_SSUSB_PRBS_BERTH                       (0xff<<16) //23:16
#define RG_SSUSB_DISABLE_PHY_U2_ON                (0x1<<15) //15:15
#define RG_SSUSB_DISABLE_PHY_U2_OFF               (0x1<<14) //14:14
#define RG_SSUSB_PRBS_EN                          (0x1<<13) //13:13
#define RG_SSUSB_BPSLOCK                          (0x1<<12) //12:12
#define RG_SSUSB_RTCOMCNT                         (0xf<<8) //11:8
#define RG_SSUSB_COMCNT                           (0xf<<4) //7:4
#define RG_SSUSB_PRBSEL_CALIB                     (0xf<<0) //3:0

//U3D_PHYD_MIX1
#define RG_SSUSB_SLEEP_EN                         (0x1<<31) //31:31
#define RG_SSUSB_PRBSEL_PCS                       (0x7<<28) //30:28
#define RG_SSUSB_TXLFPS_PRD                       (0xf<<24) //27:24
#define RG_SSUSB_P_RX_P0S_CK                      (0x1<<23) //23:23
#define RG_SSUSB_P_TX_P0S_CK                      (0x1<<22) //22:22
#define RG_SSUSB_PDNCTL                           (0x3f<<16) //21:16
#define RG_SSUSB_TX_DRV_EN                        (0x1<<15) //15:15
#define RG_SSUSB_TX_DRV_SEL                       (0x1<<14) //14:14
#define RG_SSUSB_TX_DRV_DLY                       (0x3f<<8) //13:8
#define RG_SSUSB_BERT_EN                          (0x1<<7) //7:7
#define RG_SSUSB_SCP_TH                           (0x7<<4) //6:4
#define RG_SSUSB_SCP_EN                           (0x1<<3) //3:3
#define RG_SSUSB_RXANSIDEC_TEST                   (0x7<<0) //2:0

//U3D_PHYD_LFPS0
#define RG_SSUSB_LFPS_PWD                         (0x1<<30) //30:30
#define RG_SSUSB_FORCE_LFPS_PWD                   (0x1<<29) //29:29
#define RG_SSUSB_RXLFPS_OVF                       (0x1f<<24) //28:24
#define RG_SSUSB_P3_ENTRY_SEL                     (0x1<<23) //23:23
#define RG_SSUSB_P3_ENTRY                         (0x1<<22) //22:22
#define RG_SSUSB_RXLFPS_CDRSEL                    (0x3<<20) //21:20
#define RG_SSUSB_RXLFPS_CDRTH                     (0xf<<16) //19:16
#define RG_SSUSB_LOCK5G_BLOCK                     (0x1<<15) //15:15
#define RG_SSUSB_TFIFO_EXT_D_SEL                  (0x1<<14) //14:14
#define RG_SSUSB_TFIFO_NO_EXTEND                  (0x1<<13) //13:13
#define RG_SSUSB_RXLFPS_LOB                       (0x1f<<8) //12:8
#define RG_SSUSB_TXLFPS_EN                        (0x1<<7) //7:7
#define RG_SSUSB_TXLFPS_SEL                       (0x1<<6) //6:6
#define RG_SSUSB_RXLFPS_CDRLOCK                   (0x1<<5) //5:5
#define RG_SSUSB_RXLFPS_UPB                       (0x1f<<0) //4:0

//U3D_PHYD_LFPS1
#define RG_SSUSB_RX_IMP_BIAS                      (0xf<<28) //31:28
#define RG_SSUSB_TX_IMP_BIAS                      (0xf<<24) //27:24
#define RG_SSUSB_FWAKE_TH                         (0x3f<<16) //21:16
#define RG_SSUSB_RXLFPS_UDF                       (0x1f<<8) //12:8
#define RG_SSUSB_RXLFPS_P0IDLETH                  (0xff<<0) //7:0

//U3D_PHYD_IMPCAL0
#define RG_SSUSB_FORCE_TX_IMPSEL                  (0x1<<31) //31:31
#define RG_SSUSB_TX_IMPCAL_EN                     (0x1<<30) //30:30
#define RG_SSUSB_FORCE_TX_IMPCAL_EN               (0x1<<29) //29:29
#define RG_SSUSB_TX_IMPSEL                        (0x1f<<24) //28:24
#define RG_SSUSB_TX_IMPCAL_CALCYC                 (0x3f<<16) //21:16
#define RG_SSUSB_TX_IMPCAL_STBCYC                 (0x1f<<10) //14:10
#define RG_SSUSB_TX_IMPCAL_CYCCNT                 (0x3ff<<0) //9:0

//U3D_PHYD_IMPCAL1
#define RG_SSUSB_FORCE_RX_IMPSEL                  (0x1<<31) //31:31
#define RG_SSUSB_RX_IMPCAL_EN                     (0x1<<30) //30:30
#define RG_SSUSB_FORCE_RX_IMPCAL_EN               (0x1<<29) //29:29
#define RG_SSUSB_RX_IMPSEL                        (0x1f<<24) //28:24
#define RG_SSUSB_RX_IMPCAL_CALCYC                 (0x3f<<16) //21:16
#define RG_SSUSB_RX_IMPCAL_STBCYC                 (0x1f<<10) //14:10
#define RG_SSUSB_RX_IMPCAL_CYCCNT                 (0x3ff<<0) //9:0

//U3D_PHYD_TXPLL0
#define RG_SSUSB_TXPLL_DDSEN_CYC                  (0x1f<<27) //31:27
#define RG_SSUSB_TXPLL_ON                         (0x1<<26) //26:26
#define RG_SSUSB_FORCE_TXPLLON                    (0x1<<25) //25:25
#define RG_SSUSB_TXPLL_STBCYC                     (0x1ff<<16) //24:16
#define RG_SSUSB_TXPLL_NCPOCHG_CYC                (0xf<<12) //15:12
#define RG_SSUSB_TXPLL_NCPOEN_CYC                 (0x3<<10) //11:10
#define RG_SSUSB_TXPLL_DDSRSTB_CYC                (0x7<<0) //2:0

//U3D_PHYD_TXPLL1
#define RG_SSUSB_PLL_NCPO_EN                      (0x1<<31) //31:31
#define RG_SSUSB_PLL_FIFO_START_MAN               (0x1<<30) //30:30
#define RG_SSUSB_PLL_NCPO_CHG                     (0x1<<28) //28:28
#define RG_SSUSB_PLL_DDS_RSTB                     (0x1<<27) //27:27
#define RG_SSUSB_PLL_DDS_PWDB                     (0x1<<26) //26:26
#define RG_SSUSB_PLL_DDSEN                        (0x1<<25) //25:25
#define RG_SSUSB_PLL_AUTOK_VCO                    (0x1<<24) //24:24
#define RG_SSUSB_PLL_PWD                          (0x1<<23) //23:23
#define RG_SSUSB_RX_AFE_PWD                       (0x1<<22) //22:22
#define RG_SSUSB_PLL_TCADJ                        (0x3f<<16) //21:16
#define RG_SSUSB_FORCE_CDR_TCADJ                  (0x1<<15) //15:15
#define RG_SSUSB_FORCE_CDR_AUTOK_VCO              (0x1<<14) //14:14
#define RG_SSUSB_FORCE_CDR_PWD                    (0x1<<13) //13:13
#define RG_SSUSB_FORCE_PLL_NCPO_EN                (0x1<<12) //12:12
#define RG_SSUSB_FORCE_PLL_FIFO_START_MAN         (0x1<<11) //11:11
#define RG_SSUSB_FORCE_PLL_NCPO_CHG               (0x1<<9) //9:9
#define RG_SSUSB_FORCE_PLL_DDS_RSTB               (0x1<<8) //8:8
#define RG_SSUSB_FORCE_PLL_DDS_PWDB               (0x1<<7) //7:7
#define RG_SSUSB_FORCE_PLL_DDSEN                  (0x1<<6) //6:6
#define RG_SSUSB_FORCE_PLL_TCADJ                  (0x1<<5) //5:5
#define RG_SSUSB_FORCE_PLL_AUTOK_VCO              (0x1<<4) //4:4
#define RG_SSUSB_FORCE_PLL_PWD                    (0x1<<3) //3:3
#define RG_SSUSB_FLT_1_DISPERR_B                  (0x1<<2) //2:2

//U3D_PHYD_TXPLL2
#define RG_SSUSB_TX_LFPS_EN                       (0x1<<31) //31:31
#define RG_SSUSB_FORCE_TX_LFPS_EN                 (0x1<<30) //30:30
#define RG_SSUSB_TX_LFPS                          (0x1<<29) //29:29
#define RG_SSUSB_FORCE_TX_LFPS                    (0x1<<28) //28:28
#define RG_SSUSB_RXPLL_STB                        (0x1<<27) //27:27
#define RG_SSUSB_TXPLL_STB                        (0x1<<26) //26:26
#define RG_SSUSB_FORCE_RXPLL_STB                  (0x1<<25) //25:25
#define RG_SSUSB_FORCE_TXPLL_STB                  (0x1<<24) //24:24
#define RG_SSUSB_RXPLL_REFCKSEL                   (0x1<<16) //16:16
#define RG_SSUSB_RXPLL_STBMODE                    (0x1<<11) //11:11
#define RG_SSUSB_RXPLL_ON                         (0x1<<10) //10:10
#define RG_SSUSB_FORCE_RXPLLON                    (0x1<<9) //9:9
#define RG_SSUSB_FORCE_RX_AFE_PWD                 (0x1<<8) //8:8
#define RG_SSUSB_CDR_AUTOK_VCO                    (0x1<<7) //7:7
#define RG_SSUSB_CDR_PWD                          (0x1<<6) //6:6
#define RG_SSUSB_CDR_TCADJ                        (0x3f<<0) //5:0

//U3D_PHYD_FL0
#define RG_SSUSB_RX_FL_TARGET                     (0xffff<<16) //31:16
#define RG_SSUSB_RX_FL_CYCLECNT                   (0xffff<<0) //15:0

//U3D_PHYD_MIX2
#define RG_SSUSB_RX_EQ_RST                        (0x1<<31) //31:31
#define RG_SSUSB_RX_EQ_RST_SEL                    (0x1<<30) //30:30
#define RG_SSUSB_RXVAL_RST                        (0x1<<29) //29:29
#define RG_SSUSB_RXVAL_CNT                        (0x1f<<24) //28:24
#define RG_SSUSB_CDROS_EN                         (0x1<<18) //18:18
#define RG_SSUSB_CDR_LCKOP                        (0x3<<16) //17:16
#define RG_SSUSB_RX_FL_LOCKTH                     (0xf<<8) //11:8
#define RG_SSUSB_RX_FL_OFFSET                     (0xff<<0) //7:0

//U3D_PHYD_RX0
#define RG_SSUSB_T2RLB_BERTH                      (0xff<<24) //31:24
#define RG_SSUSB_T2RLB_PAT                        (0xff<<16) //23:16
#define RG_SSUSB_T2RLB_EN                         (0x1<<15) //15:15
#define RG_SSUSB_T2RLB_BPSCRAMB                   (0x1<<14) //14:14
#define RG_SSUSB_T2RLB_SERIAL                     (0x1<<13) //13:13
#define RG_SSUSB_T2RLB_MODE                       (0x3<<11) //12:11
#define RG_SSUSB_RX_SAOSC_EN                      (0x1<<10) //10:10
#define RG_SSUSB_RX_SAOSC_EN_SEL                  (0x1<<9) //9:9
#define RG_SSUSB_RX_DFE_OPTION                    (0x1<<8) //8:8
#define RG_SSUSB_RX_DFE_EN                        (0x1<<7) //7:7
#define RG_SSUSB_RX_DFE_EN_SEL                    (0x1<<6) //6:6
#define RG_SSUSB_RX_EQ_EN                         (0x1<<5) //5:5
#define RG_SSUSB_RX_EQ_EN_SEL                     (0x1<<4) //4:4
#define RG_SSUSB_RX_SAOSC_RST                     (0x1<<3) //3:3
#define RG_SSUSB_RX_SAOSC_RST_SEL                 (0x1<<2) //2:2
#define RG_SSUSB_RX_DFE_RST                       (0x1<<1) //1:1
#define RG_SSUSB_RX_DFE_RST_SEL                   (0x1<<0) //0:0

//U3D_PHYD_T2RLB
#define RG_SSUSB_EQTRAIN_CH_MODE                  (0x1<<28) //28:28
#define RG_SSUSB_PRB_OUT_CPPAT                    (0x1<<27) //27:27
#define RG_SSUSB_BPANSIENC                        (0x1<<26) //26:26
#define RG_SSUSB_VALID_EN                         (0x1<<25) //25:25
#define RG_SSUSB_EBUF_SRST                        (0x1<<24) //24:24
#define RG_SSUSB_K_EMP                            (0xf<<20) //23:20
#define RG_SSUSB_K_FUL                            (0xf<<16) //19:16
#define RG_SSUSB_T2RLB_BDATRST                    (0xf<<12) //15:12
#define RG_SSUSB_P_T2RLB_SKP_EN                   (0x1<<10) //10:10
#define RG_SSUSB_T2RLB_PATMODE                    (0x3<<8) //9:8
#define RG_SSUSB_T2RLB_TSEQCNT                    (0xff<<0) //7:0

//U3D_PHYD_CPPAT
#define RG_SSUSB_CPPAT_PROGRAM_EN                 (0x1<<24) //24:24
#define RG_SSUSB_CPPAT_TOZ                        (0x3<<21) //22:21
#define RG_SSUSB_CPPAT_PRBS_EN                    (0x1<<20) //20:20
#define RG_SSUSB_CPPAT_OUT_TMP2                   (0xf<<16) //19:16
#define RG_SSUSB_CPPAT_OUT_TMP1                   (0xff<<8) //15:8
#define RG_SSUSB_CPPAT_OUT_TMP0                   (0xff<<0) //7:0

//U3D_PHYD_MIX3
#define RG_SSUSB_CDR_TCADJ_MINUS                  (0x1<<31) //31:31
#define RG_SSUSB_P_CDROS_EN                       (0x1<<30) //30:30
#define RG_SSUSB_P_P2_TX_DRV_DIS                  (0x1<<28) //28:28
#define RG_SSUSB_CDR_TCADJ_OFFSET                 (0x7<<24) //26:24
#define RG_SSUSB_PLL_TCADJ_MINUS                  (0x1<<23) //23:23
#define RG_SSUSB_FORCE_PLL_BIAS_LPF_EN            (0x1<<20) //20:20
#define RG_SSUSB_PLL_BIAS_LPF_EN                  (0x1<<19) //19:19
#define RG_SSUSB_PLL_TCADJ_OFFSET                 (0x7<<16) //18:16
#define RG_SSUSB_FORCE_PLL_SSCEN                  (0x1<<15) //15:15
#define RG_SSUSB_PLL_SSCEN                        (0x1<<14) //14:14
#define RG_SSUSB_FORCE_CDR_PI_PWD                 (0x1<<13) //13:13
#define RG_SSUSB_CDR_PI_PWD                       (0x1<<12) //12:12
#define RG_SSUSB_CDR_PI_MODE                      (0x1<<11) //11:11
#define RG_SSUSB_TXPLL_SSCEN_CYC                  (0x3ff<<0) //9:0

//U3D_PHYD_EBUFCTL
#define RG_SSUSB_EBUFCTL                          (0xffffffff<<0) //31:0

//U3D_PHYD_PIPE0
#define RG_SSUSB_RXTERMINATION                    (0x1<<30) //30:30
#define RG_SSUSB_RXEQTRAINING                     (0x1<<29) //29:29
#define RG_SSUSB_RXPOLARITY                       (0x1<<28) //28:28
#define RG_SSUSB_TXDEEMPH                         (0x3<<26) //27:26
#define RG_SSUSB_POWERDOWN                        (0x3<<24) //25:24
#define RG_SSUSB_TXONESZEROS                      (0x1<<23) //23:23
#define RG_SSUSB_TXELECIDLE                       (0x1<<22) //22:22
#define RG_SSUSB_TXDETECTRX                       (0x1<<21) //21:21
#define RG_SSUSB_PIPE_SEL                         (0x1<<20) //20:20
#define RG_SSUSB_TXDATAK                          (0xf<<16) //19:16
#define RG_SSUSB_CDR_STABLE_SEL                   (0x1<<15) //15:15
#define RG_SSUSB_CDR_STABLE                       (0x1<<14) //14:14
#define RG_SSUSB_CDR_RSTB_SEL                     (0x1<<13) //13:13
#define RG_SSUSB_CDR_RSTB                         (0x1<<12) //12:12
#define RG_SSUSB_P_ERROR_SEL                      (0x3<<4) //5:4
#define RG_SSUSB_TXMARGIN                         (0x7<<1) //3:1
#define RG_SSUSB_TXCOMPLIANCE                     (0x1<<0) //0:0

//U3D_PHYD_PIPE1
#define RG_SSUSB_TXDATA                           (0xffffffff<<0) //31:0

//U3D_PHYD_MIX4
#define RG_SSUSB_CDROS_CNT                        (0x3f<<24) //29:24
#define RG_SSUSB_T2RLB_BER_EN                     (0x1<<16) //16:16
#define RG_SSUSB_T2RLB_BER_RATE                   (0xffff<<0) //15:0

//U3D_PHYD_CKGEN0
#define RG_SSUSB_RFIFO_IMPLAT                     (0x1<<27) //27:27
#define RG_SSUSB_TFIFO_PSEL                       (0x7<<24) //26:24
#define RG_SSUSB_CKGEN_PSEL                       (0x3<<8) //9:8
#define RG_SSUSB_RXCK_INV                         (0x1<<0) //0:0

//U3D_PHYD_MIX5
#define RG_SSUSB_PRB_SEL                          (0xffff<<16) //31:16
#define RG_SSUSB_RXPLL_STBCYC                     (0x7ff<<0) //10:0

//U3D_PHYD_RESERVED
#define RG_SSUSB_PHYD_RESERVE                     (0xffffffff<<0) //31:0
//#define RG_SSUSB_RX_SIGDET_SEL                    (0x1<<11)
//#define RG_SSUSB_RX_SIGDET_EN                     (0x1<<12)
//#define RG_SSUSB_RX_PI_CAL_MANUAL_SEL             (0x1<<9)
//#define RG_SSUSB_RX_PI_CAL_MANUAL_EN              (0x1<<10)

//U3D_PHYD_CDR0
#define RG_SSUSB_CDR_BIC_LTR                      (0xf<<28) //31:28
#define RG_SSUSB_CDR_BIC_LTD0                     (0xf<<24) //27:24
#define RG_SSUSB_CDR_BC_LTD1                      (0x1f<<16) //20:16
#define RG_SSUSB_CDR_BC_LTR                       (0x1f<<8) //12:8
#define RG_SSUSB_CDR_BC_LTD0                      (0x1f<<0) //4:0

//U3D_PHYD_CDR1
#define RG_SSUSB_CDR_BIR_LTD1                     (0x1f<<24) //28:24
#define RG_SSUSB_CDR_BIR_LTR                      (0x1f<<16) //20:16
#define RG_SSUSB_CDR_BIR_LTD0                     (0x1f<<8) //12:8
#define RG_SSUSB_CDR_BW_SEL                       (0x3<<6) //7:6
#define RG_SSUSB_CDR_BIC_LTD1                     (0xf<<0) //3:0

//U3D_PHYD_PLL_0
#define RG_SSUSB_FORCE_CDR_BAND_5G                (0x1<<28) //28:28
#define RG_SSUSB_FORCE_CDR_BAND_2P5G              (0x1<<27) //27:27
#define RG_SSUSB_FORCE_PLL_BAND_5G                (0x1<<26) //26:26
#define RG_SSUSB_FORCE_PLL_BAND_2P5G              (0x1<<25) //25:25
#define RG_SSUSB_P_EQ_T_SEL                       (0x3ff<<15) //24:15
#define RG_SSUSB_PLL_ISO_EN_CYC                   (0x3ff<<5) //14:5
#define RG_SSUSB_PLLBAND_RECAL                    (0x1<<4) //4:4
#define RG_SSUSB_PLL_DDS_ISO_EN                   (0x1<<3) //3:3
#define RG_SSUSB_FORCE_PLL_DDS_ISO_EN             (0x1<<2) //2:2
#define RG_SSUSB_PLL_DDS_PWR_ON                   (0x1<<1) //1:1
#define RG_SSUSB_FORCE_PLL_DDS_PWR_ON             (0x1<<0) //0:0

//U3D_PHYD_PLL_1
#define RG_SSUSB_CDR_BAND_5G                      (0xff<<24) //31:24
#define RG_SSUSB_CDR_BAND_2P5G                    (0xff<<16) //23:16
#define RG_SSUSB_PLL_BAND_5G                      (0xff<<8) //15:8
#define RG_SSUSB_PLL_BAND_2P5G                    (0xff<<0) //7:0

//U3D_PHYD_BCN_DET_1
#define RG_SSUSB_P_BCN_OBS_PRD                    (0xffff<<16) //31:16
#define RG_SSUSB_U_BCN_OBS_PRD                    (0xffff<<0) //15:0

//U3D_PHYD_BCN_DET_2
#define RG_SSUSB_P_BCN_OBS_SEL                    (0xfff<<16) //27:16
#define RG_SSUSB_BCN_DET_DIS                      (0x1<<12) //12:12
#define RG_SSUSB_U_BCN_OBS_SEL                    (0xfff<<0) //11:0

//U3D_EQ0
#define RG_SSUSB_EQ_DLHL_LFI                      (0x7f<<24) //30:24
#define RG_SSUSB_EQ_DHHL_LFI                      (0x7f<<16) //22:16
#define RG_SSUSB_EQ_DD0HOS_LFI                    (0x7f<<8) //14:8
#define RG_SSUSB_EQ_DD0LOS_LFI                    (0x7f<<0) //6:0

//U3D_EQ1
#define RG_SSUSB_EQ_DD1HOS_LFI                    (0x7f<<24) //30:24
#define RG_SSUSB_EQ_DD1LOS_LFI                    (0x7f<<16) //22:16
#define RG_SSUSB_EQ_DE0OS_LFI                     (0x7f<<8) //14:8
#define RG_SSUSB_EQ_DE1OS_LFI                     (0x7f<<0) //6:0

//U3D_EQ2
#define RG_SSUSB_EQ_DLHLOS_LFI                    (0x7f<<24) //30:24
#define RG_SSUSB_EQ_DHHLOS_LFI                    (0x7f<<16) //22:16
#define RG_SSUSB_EQ_STOPTIME                      (0x1<<14) //14:14
#define RG_SSUSB_EQ_DHHL_LF_SEL                   (0x7<<11) //13:11
#define RG_SSUSB_EQ_DSAOS_LF_SEL                  (0x7<<8) //10:8
#define RG_SSUSB_EQ_STARTTIME                     (0x3<<6) //7:6
#define RG_SSUSB_EQ_DLEQ_LF_SEL                   (0x7<<3) //5:3
#define RG_SSUSB_EQ_DLHL_LF_SEL                   (0x7<<0) //2:0

//U3D_EQ3
#define RG_SSUSB_EQ_DLEQ_LFI_GEN2                 (0xf<<28) //31:28
#define RG_SSUSB_EQ_DLEQ_LFI_GEN1                 (0xf<<24) //27:24
#define RG_SSUSB_EQ_DEYE0OS_LFI                   (0x7f<<16) //22:16
#define RG_SSUSB_EQ_DEYE1OS_LFI                   (0x7f<<8) //14:8
#define RG_SSUSB_EQ_TRI_DET_EN                    (0x1<<7) //7:7
#define RG_SSUSB_EQ_TRI_DET_TH                    (0x7f<<0) //6:0

//U3D_EQ_EYE0
#define RG_SSUSB_EQ_EYE_XOFFSET                   (0x7f<<25) //31:25
#define RG_SSUSB_EQ_EYE_MON_EN                    (0x1<<24) //24:24
#define RG_SSUSB_EQ_EYE0_Y                        (0x7f<<16) //22:16
#define RG_SSUSB_EQ_EYE1_Y                        (0x7f<<8) //14:8
#define RG_SSUSB_EQ_PILPO_ROUT                    (0x1<<7) //7:7
#define RG_SSUSB_EQ_PI_KPGAIN                     (0x7<<4) //6:4
#define RG_SSUSB_EQ_EYE_CNT_EN                    (0x1<<3) //3:3

//U3D_EQ_EYE1
#define RG_SSUSB_EQ_SIGDET                        (0x7f<<24) //30:24
#define RG_SSUSB_EQ_EYE_MASK                      (0x3ff<<7) //16:7

//U3D_EQ_EYE2
#define RG_SSUSB_EQ_RX500M_CK_SEL                 (0x1<<31) //31:31
#define RG_SSUSB_EQ_SD_CNT1                       (0x3f<<24) //29:24
#define RG_SSUSB_EQ_ISIFLAG_SEL                   (0x3<<22) //23:22
#define RG_SSUSB_EQ_SD_CNT0                       (0x3f<<16) //21:16

//U3D_EQ_DFE0
#define RG_SSUSB_EQ_LEQMAX                        (0xf<<28) //31:28
#define RG_SSUSB_EQ_DFEX_EN                       (0x1<<27) //27:27
#define RG_SSUSB_EQ_DFEX_LF_SEL                   (0x7<<24) //26:24
#define RG_SSUSB_EQ_CHK_EYE_H                     (0x1<<23) //23:23
#define RG_SSUSB_EQ_PIEYE_INI                     (0x7f<<16) //22:16
#define RG_SSUSB_EQ_PI90_INI                      (0x7f<<8) //14:8
#define RG_SSUSB_EQ_PI0_INI                       (0x7f<<0) //6:0

//U3D_EQ_DFE1
#define RG_SSUSB_EQ_REV                           (0xffff<<16) //31:16
#define RG_SSUSB_EQ_DFEYEN_DUR                    (0x7<<12) //14:12
#define RG_SSUSB_EQ_DFEXEN_DUR                    (0x7<<8) //10:8
#define RG_SSUSB_EQ_DFEX_RST                      (0x1<<7) //7:7
#define RG_SSUSB_EQ_GATED_RXD_B                   (0x1<<6) //6:6
#define RG_SSUSB_EQ_PI90CK_SEL                    (0x3<<4) //5:4
#define RG_SSUSB_EQ_DFEX_DIS                      (0x1<<2) //2:2
#define RG_SSUSB_EQ_DFEYEN_STOP_DIS               (0x1<<1) //1:1
#define RG_SSUSB_EQ_DFEXEN_SEL                    (0x1<<0) //0:0

//U3D_EQ_DFE2
#define RG_SSUSB_EQ_MON_SEL                       (0x1f<<24) //28:24
#define RG_SSUSB_EQ_LEQOSC_DLYCNT                 (0x7<<16) //18:16
#define RG_SSUSB_EQ_DLEQOS_LFI                    (0x1f<<8) //12:8
#define RG_SSUSB_EQ_LEQ_STOP_TO                   (0x3<<0) //1:0

//U3D_EQ_DFE3
#define RG_SSUSB_EQ_RESERVED                      (0xffffffff<<0) //31:0

//U3D_PHYD_MON0
#define RGS_SSUSB_BERT_BERC                       (0xffff<<16) //31:16
#define RGS_SSUSB_LFPS                            (0xf<<12) //15:12
#define RGS_SSUSB_TRAINDEC                        (0x7<<8) //10:8
#define RGS_SSUSB_SCP_PAT                         (0xff<<0) //7:0

//U3D_PHYD_MON1
#define RGS_SSUSB_RX_FL_OUT                       (0xffff<<0) //15:0

//U3D_PHYD_MON2
#define RGS_SSUSB_T2RLB_ERRCNT                    (0xffff<<16) //31:16
#define RGS_SSUSB_RETRACK                         (0xf<<12) //15:12
#define RGS_SSUSB_RXPLL_LOCK                      (0x1<<10) //10:10
#define RGS_SSUSB_CDR_VCOCAL_CPLT_D               (0x1<<9) //9:9
#define RGS_SSUSB_PLL_VCOCAL_CPLT_D               (0x1<<8) //8:8
#define RGS_SSUSB_PDNCTL                          (0xff<<0) //7:0

//U3D_PHYD_MON3
#define RGS_SSUSB_TSEQ_ERRCNT                     (0xffff<<16) //31:16
#define RGS_SSUSB_PRBS_ERRCNT                     (0xffff<<0) //15:0

//U3D_PHYD_MON4
#define RGS_SSUSB_RX_LSLOCK_CNT                   (0xf<<24) //27:24
#define RGS_SSUSB_SCP_DETCNT                      (0xff<<16) //23:16
#define RGS_SSUSB_TSEQ_DETCNT                     (0xffff<<0) //15:0

//U3D_PHYD_MON5
#define RGS_SSUSB_EBUFMSG                         (0xffff<<16) //31:16
#define RGS_SSUSB_BERT_LOCK                       (0x1<<15) //15:15
#define RGS_SSUSB_SCP_DET                         (0x1<<14) //14:14
#define RGS_SSUSB_TSEQ_DET                        (0x1<<13) //13:13
#define RGS_SSUSB_EBUF_UDF                        (0x1<<12) //12:12
#define RGS_SSUSB_EBUF_OVF                        (0x1<<11) //11:11
#define RGS_SSUSB_PRBS_PASSTH                     (0x1<<10) //10:10
#define RGS_SSUSB_PRBS_PASS                       (0x1<<9) //9:9
#define RGS_SSUSB_PRBS_LOCK                       (0x1<<8) //8:8
#define RGS_SSUSB_T2RLB_ERR                       (0x1<<6) //6:6
#define RGS_SSUSB_T2RLB_PASSTH                    (0x1<<5) //5:5
#define RGS_SSUSB_T2RLB_PASS                      (0x1<<4) //4:4
#define RGS_SSUSB_T2RLB_LOCK                      (0x1<<3) //3:3
#define RGS_SSUSB_RX_IMPCAL_DONE                  (0x1<<2) //2:2
#define RGS_SSUSB_TX_IMPCAL_DONE                  (0x1<<1) //1:1
#define RGS_SSUSB_RXDETECTED                      (0x1<<0) //0:0

//U3D_PHYD_MON6
#define RGS_SSUSB_SIGCAL_DONE                     (0x1<<30) //30:30
#define RGS_SSUSB_SIGCAL_CAL_OUT                  (0x1<<29) //29:29
#define RGS_SSUSB_SIGCAL_OFFSET                   (0x1f<<24) //28:24
#define RGS_SSUSB_RX_IMP_SEL                      (0x1f<<16) //20:16
#define RGS_SSUSB_TX_IMP_SEL                      (0x1f<<8) //12:8
#define RGS_SSUSB_TFIFO_MSG                       (0xf<<4) //7:4
#define RGS_SSUSB_RFIFO_MSG                       (0xf<<0) //3:0

//U3D_PHYD_MON7
#define RGS_SSUSB_FT_OUT                          (0xff<<8) //15:8
#define RGS_SSUSB_PRB_OUT                         (0xff<<0) //7:0

//U3D_PHYA_RX_MON0
#define RGS_SSUSB_EQ_DCLEQ                        (0xf<<24) //27:24
#define RGS_SSUSB_EQ_DCD0H                        (0x7f<<16) //22:16
#define RGS_SSUSB_EQ_DCD0L                        (0x7f<<8) //14:8
#define RGS_SSUSB_EQ_DCD1H                        (0x7f<<0) //6:0

//U3D_PHYA_RX_MON1
#define RGS_SSUSB_EQ_DCD1L                        (0x7f<<24) //30:24
#define RGS_SSUSB_EQ_DCE0                         (0x7f<<16) //22:16
#define RGS_SSUSB_EQ_DCE1                         (0x7f<<8) //14:8
#define RGS_SSUSB_EQ_DCHHL                        (0x7f<<0) //6:0

//U3D_PHYA_RX_MON2
#define RGS_SSUSB_EQ_LEQ_STOP                     (0x1<<31) //31:31
#define RGS_SSUSB_EQ_DCLHL                        (0x7f<<24) //30:24
#define RGS_SSUSB_EQ_STATUS                       (0xff<<16) //23:16
#define RGS_SSUSB_EQ_DCEYE0                       (0x7f<<8) //14:8
#define RGS_SSUSB_EQ_DCEYE1                       (0x7f<<0) //6:0

//U3D_PHYA_RX_MON3
#define RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0         (0xfffff<<0) //19:0

//U3D_PHYA_RX_MON4
#define RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1         (0xfffff<<0) //19:0

//U3D_PHYA_RX_MON5
#define RGS_SSUSB_EQ_DCLEQOS                      (0x1f<<8) //12:8
#define RGS_SSUSB_EQ_EYE_CNT_RDY                  (0x1<<7) //7:7
#define RGS_SSUSB_EQ_PILPO                        (0x7f<<0) //6:0

//U3D_PHYD_CPPAT2
#define RG_SSUSB_CPPAT_OUT_H_TMP2                 (0xf<<16) //19:16
#define RG_SSUSB_CPPAT_OUT_H_TMP1                 (0xff<<8) //15:8
#define RG_SSUSB_CPPAT_OUT_H_TMP0                 (0xff<<0) //7:0

//U3D_EQ_EYE3
#define RG_SSUSB_EQ_LEQ_SHIFT                     (0x7<<24) //26:24
#define RG_SSUSB_EQ_EYE_CNT                       (0xfffff<<0) //19:0

//U3D_KBAND_OUT
#define RGS_SSUSB_CDR_BAND_5G                     (0xff<<24) //31:24
#define RGS_SSUSB_CDR_BAND_2P5G                   (0xff<<16) //23:16
#define RGS_SSUSB_PLL_BAND_5G                     (0xff<<8) //15:8
#define RGS_SSUSB_PLL_BAND_2P5G                   (0xff<<0) //7:0

//U3D_KBAND_OUT1
#define RGS_SSUSB_CDR_VCOCAL_FAIL                 (0x1<<24) //24:24
#define RGS_SSUSB_CDR_VCOCAL_STATE                (0xff<<16) //23:16
#define RGS_SSUSB_PLL_VCOCAL_FAIL                 (0x1<<8) //8:8
#define RGS_SSUSB_PLL_VCOCAL_STATE                (0xff<<0) //7:0


/* OFFSET */

//U3D_PHYD_MIX0
#define RG_SSUSB_P_P3_TX_NG_OFST                  (31)
#define RG_SSUSB_TSEQ_EN_OFST                     (30)
#define RG_SSUSB_TSEQ_POLEN_OFST                  (29)
#define RG_SSUSB_TSEQ_POL_OFST                    (28)
#define RG_SSUSB_P_P3_PCLK_NG_OFST                (27)
#define RG_SSUSB_TSEQ_TH_OFST                     (24)
#define RG_SSUSB_PRBS_BERTH_OFST                  (16)
#define RG_SSUSB_DISABLE_PHY_U2_ON_OFST           (15)
#define RG_SSUSB_DISABLE_PHY_U2_OFF_OFST          (14)
#define RG_SSUSB_PRBS_EN_OFST                     (13)
#define RG_SSUSB_BPSLOCK_OFST                     (12)
#define RG_SSUSB_RTCOMCNT_OFST                    (8)
#define RG_SSUSB_COMCNT_OFST                      (4)
#define RG_SSUSB_PRBSEL_CALIB_OFST                (0)

//U3D_PHYD_MIX1
#define RG_SSUSB_SLEEP_EN_OFST                    (31)
#define RG_SSUSB_PRBSEL_PCS_OFST                  (28)
#define RG_SSUSB_TXLFPS_PRD_OFST                  (24)
#define RG_SSUSB_P_RX_P0S_CK_OFST                 (23)
#define RG_SSUSB_P_TX_P0S_CK_OFST                 (22)
#define RG_SSUSB_PDNCTL_OFST                      (16)
#define RG_SSUSB_TX_DRV_EN_OFST                   (15)
#define RG_SSUSB_TX_DRV_SEL_OFST                  (14)
#define RG_SSUSB_TX_DRV_DLY_OFST                  (8)
#define RG_SSUSB_BERT_EN_OFST                     (7)
#define RG_SSUSB_SCP_TH_OFST                      (4)
#define RG_SSUSB_SCP_EN_OFST                      (3)
#define RG_SSUSB_RXANSIDEC_TEST_OFST              (0)

//U3D_PHYD_LFPS0
#define RG_SSUSB_LFPS_PWD_OFST                    (30)
#define RG_SSUSB_FORCE_LFPS_PWD_OFST              (29)
#define RG_SSUSB_RXLFPS_OVF_OFST                  (24)
#define RG_SSUSB_P3_ENTRY_SEL_OFST                (23)
#define RG_SSUSB_P3_ENTRY_OFST                    (22)
#define RG_SSUSB_RXLFPS_CDRSEL_OFST               (20)
#define RG_SSUSB_RXLFPS_CDRTH_OFST                (16)
#define RG_SSUSB_LOCK5G_BLOCK_OFST                (15)
#define RG_SSUSB_TFIFO_EXT_D_SEL_OFST             (14)
#define RG_SSUSB_TFIFO_NO_EXTEND_OFST             (13)
#define RG_SSUSB_RXLFPS_LOB_OFST                  (8)
#define RG_SSUSB_TXLFPS_EN_OFST                   (7)
#define RG_SSUSB_TXLFPS_SEL_OFST                  (6)
#define RG_SSUSB_RXLFPS_CDRLOCK_OFST              (5)
#define RG_SSUSB_RXLFPS_UPB_OFST                  (0)

//U3D_PHYD_LFPS1
#define RG_SSUSB_RX_IMP_BIAS_OFST                 (28)
#define RG_SSUSB_TX_IMP_BIAS_OFST                 (24)
#define RG_SSUSB_FWAKE_TH_OFST                    (16)
#define RG_SSUSB_RXLFPS_UDF_OFST                  (8)
#define RG_SSUSB_RXLFPS_P0IDLETH_OFST             (0)

//U3D_PHYD_IMPCAL0
#define RG_SSUSB_FORCE_TX_IMPSEL_OFST             (31)
#define RG_SSUSB_TX_IMPCAL_EN_OFST                (30)
#define RG_SSUSB_FORCE_TX_IMPCAL_EN_OFST          (29)
#define RG_SSUSB_TX_IMPSEL_OFST                   (24)
#define RG_SSUSB_TX_IMPCAL_CALCYC_OFST            (16)
#define RG_SSUSB_TX_IMPCAL_STBCYC_OFST            (10)
#define RG_SSUSB_TX_IMPCAL_CYCCNT_OFST            (0)

//U3D_PHYD_IMPCAL1
#define RG_SSUSB_FORCE_RX_IMPSEL_OFST             (31)
#define RG_SSUSB_RX_IMPCAL_EN_OFST                (30)
#define RG_SSUSB_FORCE_RX_IMPCAL_EN_OFST          (29)
#define RG_SSUSB_RX_IMPSEL_OFST                   (24)
#define RG_SSUSB_RX_IMPCAL_CALCYC_OFST            (16)
#define RG_SSUSB_RX_IMPCAL_STBCYC_OFST            (10)
#define RG_SSUSB_RX_IMPCAL_CYCCNT_OFST            (0)

//U3D_PHYD_TXPLL0
#define RG_SSUSB_TXPLL_DDSEN_CYC_OFST             (27)
#define RG_SSUSB_TXPLL_ON_OFST                    (26)
#define RG_SSUSB_FORCE_TXPLLON_OFST               (25)
#define RG_SSUSB_TXPLL_STBCYC_OFST                (16)
#define RG_SSUSB_TXPLL_NCPOCHG_CYC_OFST           (12)
#define RG_SSUSB_TXPLL_NCPOEN_CYC_OFST            (10)
#define RG_SSUSB_TXPLL_DDSRSTB_CYC_OFST           (0)

//U3D_PHYD_TXPLL1
#define RG_SSUSB_PLL_NCPO_EN_OFST                 (31)
#define RG_SSUSB_PLL_FIFO_START_MAN_OFST          (30)
#define RG_SSUSB_PLL_NCPO_CHG_OFST                (28)
#define RG_SSUSB_PLL_DDS_RSTB_OFST                (27)
#define RG_SSUSB_PLL_DDS_PWDB_OFST                (26)
#define RG_SSUSB_PLL_DDSEN_OFST                   (25)
#define RG_SSUSB_PLL_AUTOK_VCO_OFST               (24)
#define RG_SSUSB_PLL_PWD_OFST                     (23)
#define RG_SSUSB_RX_AFE_PWD_OFST                  (22)
#define RG_SSUSB_PLL_TCADJ_OFST                   (16)
#define RG_SSUSB_FORCE_CDR_TCADJ_OFST             (15)
#define RG_SSUSB_FORCE_CDR_AUTOK_VCO_OFST         (14)
#define RG_SSUSB_FORCE_CDR_PWD_OFST               (13)
#define RG_SSUSB_FORCE_PLL_NCPO_EN_OFST           (12)
#define RG_SSUSB_FORCE_PLL_FIFO_START_MAN_OFST    (11)
#define RG_SSUSB_FORCE_PLL_NCPO_CHG_OFST          (9)
#define RG_SSUSB_FORCE_PLL_DDS_RSTB_OFST          (8)
#define RG_SSUSB_FORCE_PLL_DDS_PWDB_OFST          (7)
#define RG_SSUSB_FORCE_PLL_DDSEN_OFST             (6)
#define RG_SSUSB_FORCE_PLL_TCADJ_OFST             (5)
#define RG_SSUSB_FORCE_PLL_AUTOK_VCO_OFST         (4)
#define RG_SSUSB_FORCE_PLL_PWD_OFST               (3)
#define RG_SSUSB_FLT_1_DISPERR_B_OFST             (2)

//U3D_PHYD_TXPLL2
#define RG_SSUSB_TX_LFPS_EN_OFST                  (31)
#define RG_SSUSB_FORCE_TX_LFPS_EN_OFST            (30)
#define RG_SSUSB_TX_LFPS_OFST                     (29)
#define RG_SSUSB_FORCE_TX_LFPS_OFST               (28)
#define RG_SSUSB_RXPLL_STB_OFST                   (27)
#define RG_SSUSB_TXPLL_STB_OFST                   (26)
#define RG_SSUSB_FORCE_RXPLL_STB_OFST             (25)
#define RG_SSUSB_FORCE_TXPLL_STB_OFST             (24)
#define RG_SSUSB_RXPLL_REFCKSEL_OFST              (16)
#define RG_SSUSB_RXPLL_STBMODE_OFST               (11)
#define RG_SSUSB_RXPLL_ON_OFST                    (10)
#define RG_SSUSB_FORCE_RXPLLON_OFST               (9)
#define RG_SSUSB_FORCE_RX_AFE_PWD_OFST            (8)
#define RG_SSUSB_CDR_AUTOK_VCO_OFST               (7)
#define RG_SSUSB_CDR_PWD_OFST                     (6)
#define RG_SSUSB_CDR_TCADJ_OFST                   (0)

//U3D_PHYD_FL0
#define RG_SSUSB_RX_FL_TARGET_OFST                (16)
#define RG_SSUSB_RX_FL_CYCLECNT_OFST              (0)

//U3D_PHYD_MIX2
#define RG_SSUSB_RX_EQ_RST_OFST                   (31)
#define RG_SSUSB_RX_EQ_RST_SEL_OFST               (30)
#define RG_SSUSB_RXVAL_RST_OFST                   (29)
#define RG_SSUSB_RXVAL_CNT_OFST                   (24)
#define RG_SSUSB_CDROS_EN_OFST                    (18)
#define RG_SSUSB_CDR_LCKOP_OFST                   (16)
#define RG_SSUSB_RX_FL_LOCKTH_OFST                (8)
#define RG_SSUSB_RX_FL_OFFSET_OFST                (0)

//U3D_PHYD_RX0
#define RG_SSUSB_T2RLB_BERTH_OFST                 (24)
#define RG_SSUSB_T2RLB_PAT_OFST                   (16)
#define RG_SSUSB_T2RLB_EN_OFST                    (15)
#define RG_SSUSB_T2RLB_BPSCRAMB_OFST              (14)
#define RG_SSUSB_T2RLB_SERIAL_OFST                (13)
#define RG_SSUSB_T2RLB_MODE_OFST                  (11)
#define RG_SSUSB_RX_SAOSC_EN_OFST                 (10)
#define RG_SSUSB_RX_SAOSC_EN_SEL_OFST             (9)
#define RG_SSUSB_RX_DFE_OPTION_OFST               (8)
#define RG_SSUSB_RX_DFE_EN_OFST                   (7)
#define RG_SSUSB_RX_DFE_EN_SEL_OFST               (6)
#define RG_SSUSB_RX_EQ_EN_OFST                    (5)
#define RG_SSUSB_RX_EQ_EN_SEL_OFST                (4)
#define RG_SSUSB_RX_SAOSC_RST_OFST                (3)
#define RG_SSUSB_RX_SAOSC_RST_SEL_OFST            (2)
#define RG_SSUSB_RX_DFE_RST_OFST                  (1)
#define RG_SSUSB_RX_DFE_RST_SEL_OFST              (0)

//U3D_PHYD_T2RLB
#define RG_SSUSB_EQTRAIN_CH_MODE_OFST             (28)
#define RG_SSUSB_PRB_OUT_CPPAT_OFST               (27)
#define RG_SSUSB_BPANSIENC_OFST                   (26)
#define RG_SSUSB_VALID_EN_OFST                    (25)
#define RG_SSUSB_EBUF_SRST_OFST                   (24)
#define RG_SSUSB_K_EMP_OFST                       (20)
#define RG_SSUSB_K_FUL_OFST                       (16)
#define RG_SSUSB_T2RLB_BDATRST_OFST               (12)
#define RG_SSUSB_P_T2RLB_SKP_EN_OFST              (10)
#define RG_SSUSB_T2RLB_PATMODE_OFST               (8)
#define RG_SSUSB_T2RLB_TSEQCNT_OFST               (0)

//U3D_PHYD_CPPAT
#define RG_SSUSB_CPPAT_PROGRAM_EN_OFST            (24)
#define RG_SSUSB_CPPAT_TOZ_OFST                   (21)
#define RG_SSUSB_CPPAT_PRBS_EN_OFST               (20)
#define RG_SSUSB_CPPAT_OUT_TMP2_OFST              (16)
#define RG_SSUSB_CPPAT_OUT_TMP1_OFST              (8)
#define RG_SSUSB_CPPAT_OUT_TMP0_OFST              (0)

//U3D_PHYD_MIX3
#define RG_SSUSB_CDR_TCADJ_MINUS_OFST             (31)
#define RG_SSUSB_P_CDROS_EN_OFST                  (30)
#define RG_SSUSB_P_P2_TX_DRV_DIS_OFST             (28)
#define RG_SSUSB_CDR_TCADJ_OFFSET_OFST            (24)
#define RG_SSUSB_PLL_TCADJ_MINUS_OFST             (23)
#define RG_SSUSB_FORCE_PLL_BIAS_LPF_EN_OFST       (20)
#define RG_SSUSB_PLL_BIAS_LPF_EN_OFST             (19)
#define RG_SSUSB_PLL_TCADJ_OFFSET_OFST            (16)
#define RG_SSUSB_FORCE_PLL_SSCEN_OFST             (15)
#define RG_SSUSB_PLL_SSCEN_OFST                   (14)
#define RG_SSUSB_FORCE_CDR_PI_PWD_OFST            (13)
#define RG_SSUSB_CDR_PI_PWD_OFST                  (12)
#define RG_SSUSB_CDR_PI_MODE_OFST                 (11)
#define RG_SSUSB_TXPLL_SSCEN_CYC_OFST             (0)

//U3D_PHYD_EBUFCTL
#define RG_SSUSB_EBUFCTL_OFST                     (0)

//U3D_PHYD_PIPE0
#define RG_SSUSB_RXTERMINATION_OFST               (30)
#define RG_SSUSB_RXEQTRAINING_OFST                (29)
#define RG_SSUSB_RXPOLARITY_OFST                  (28)
#define RG_SSUSB_TXDEEMPH_OFST                    (26)
#define RG_SSUSB_POWERDOWN_OFST                   (24)
#define RG_SSUSB_TXONESZEROS_OFST                 (23)
#define RG_SSUSB_TXELECIDLE_OFST                  (22)
#define RG_SSUSB_TXDETECTRX_OFST                  (21)
#define RG_SSUSB_PIPE_SEL_OFST                    (20)
#define RG_SSUSB_TXDATAK_OFST                     (16)
#define RG_SSUSB_CDR_STABLE_SEL_OFST              (15)
#define RG_SSUSB_CDR_STABLE_OFST                  (14)
#define RG_SSUSB_CDR_RSTB_SEL_OFST                (13)
#define RG_SSUSB_CDR_RSTB_OFST                    (12)
#define RG_SSUSB_P_ERROR_SEL_OFST                 (4)
#define RG_SSUSB_TXMARGIN_OFST                    (1)
#define RG_SSUSB_TXCOMPLIANCE_OFST                (0)

//U3D_PHYD_PIPE1
#define RG_SSUSB_TXDATA_OFST                      (0)

//U3D_PHYD_MIX4
#define RG_SSUSB_CDROS_CNT_OFST                   (24)
#define RG_SSUSB_T2RLB_BER_EN_OFST                (16)
#define RG_SSUSB_T2RLB_BER_RATE_OFST              (0)

//U3D_PHYD_CKGEN0
#define RG_SSUSB_RFIFO_IMPLAT_OFST                (27)
#define RG_SSUSB_TFIFO_PSEL_OFST                  (24)
#define RG_SSUSB_CKGEN_PSEL_OFST                  (8)
#define RG_SSUSB_RXCK_INV_OFST                    (0)

//U3D_PHYD_MIX5
#define RG_SSUSB_PRB_SEL_OFST                     (16)
#define RG_SSUSB_RXPLL_STBCYC_OFST                (0)

//U3D_PHYD_RESERVED
#define RG_SSUSB_PHYD_RESERVE_OFST                (0)
//#define RG_SSUSB_RX_SIGDET_SEL_OFST               (11)
//#define RG_SSUSB_RX_SIGDET_EN_OFST                (12)
//#define RG_SSUSB_RX_PI_CAL_MANUAL_SEL_OFST        (9)
//#define RG_SSUSB_RX_PI_CAL_MANUAL_EN_OFST         (10)

//U3D_PHYD_CDR0
#define RG_SSUSB_CDR_BIC_LTR_OFST                 (28)
#define RG_SSUSB_CDR_BIC_LTD0_OFST                (24)
#define RG_SSUSB_CDR_BC_LTD1_OFST                 (16)
#define RG_SSUSB_CDR_BC_LTR_OFST                  (8)
#define RG_SSUSB_CDR_BC_LTD0_OFST                 (0)

//U3D_PHYD_CDR1
#define RG_SSUSB_CDR_BIR_LTD1_OFST                (24)
#define RG_SSUSB_CDR_BIR_LTR_OFST                 (16)
#define RG_SSUSB_CDR_BIR_LTD0_OFST                (8)
#define RG_SSUSB_CDR_BW_SEL_OFST                  (6)
#define RG_SSUSB_CDR_BIC_LTD1_OFST                (0)

//U3D_PHYD_PLL_0
#define RG_SSUSB_FORCE_CDR_BAND_5G_OFST           (28)
#define RG_SSUSB_FORCE_CDR_BAND_2P5G_OFST         (27)
#define RG_SSUSB_FORCE_PLL_BAND_5G_OFST           (26)
#define RG_SSUSB_FORCE_PLL_BAND_2P5G_OFST         (25)
#define RG_SSUSB_P_EQ_T_SEL_OFST                  (15)
#define RG_SSUSB_PLL_ISO_EN_CYC_OFST              (5)
#define RG_SSUSB_PLLBAND_RECAL_OFST               (4)
#define RG_SSUSB_PLL_DDS_ISO_EN_OFST              (3)
#define RG_SSUSB_FORCE_PLL_DDS_ISO_EN_OFST        (2)
#define RG_SSUSB_PLL_DDS_PWR_ON_OFST              (1)
#define RG_SSUSB_FORCE_PLL_DDS_PWR_ON_OFST        (0)

//U3D_PHYD_PLL_1
#define RG_SSUSB_CDR_BAND_5G_OFST                 (24)
#define RG_SSUSB_CDR_BAND_2P5G_OFST               (16)
#define RG_SSUSB_PLL_BAND_5G_OFST                 (8)
#define RG_SSUSB_PLL_BAND_2P5G_OFST               (0)

//U3D_PHYD_BCN_DET_1
#define RG_SSUSB_P_BCN_OBS_PRD_OFST               (16)
#define RG_SSUSB_U_BCN_OBS_PRD_OFST               (0)

//U3D_PHYD_BCN_DET_2
#define RG_SSUSB_P_BCN_OBS_SEL_OFST               (16)
#define RG_SSUSB_BCN_DET_DIS_OFST                 (12)
#define RG_SSUSB_U_BCN_OBS_SEL_OFST               (0)

//U3D_EQ0
#define RG_SSUSB_EQ_DLHL_LFI_OFST                 (24)
#define RG_SSUSB_EQ_DHHL_LFI_OFST                 (16)
#define RG_SSUSB_EQ_DD0HOS_LFI_OFST               (8)
#define RG_SSUSB_EQ_DD0LOS_LFI_OFST               (0)

//U3D_EQ1
#define RG_SSUSB_EQ_DD1HOS_LFI_OFST               (24)
#define RG_SSUSB_EQ_DD1LOS_LFI_OFST               (16)
#define RG_SSUSB_EQ_DE0OS_LFI_OFST                (8)
#define RG_SSUSB_EQ_DE1OS_LFI_OFST                (0)

//U3D_EQ2
#define RG_SSUSB_EQ_DLHLOS_LFI_OFST               (24)
#define RG_SSUSB_EQ_DHHLOS_LFI_OFST               (16)
#define RG_SSUSB_EQ_STOPTIME_OFST                 (14)
#define RG_SSUSB_EQ_DHHL_LF_SEL_OFST              (11)
#define RG_SSUSB_EQ_DSAOS_LF_SEL_OFST             (8)
#define RG_SSUSB_EQ_STARTTIME_OFST                (6)
#define RG_SSUSB_EQ_DLEQ_LF_SEL_OFST              (3)
#define RG_SSUSB_EQ_DLHL_LF_SEL_OFST              (0)

//U3D_EQ3
#define RG_SSUSB_EQ_DLEQ_LFI_GEN2_OFST            (28)
#define RG_SSUSB_EQ_DLEQ_LFI_GEN1_OFST            (24)
#define RG_SSUSB_EQ_DEYE0OS_LFI_OFST              (16)
#define RG_SSUSB_EQ_DEYE1OS_LFI_OFST              (8)
#define RG_SSUSB_EQ_TRI_DET_EN_OFST               (7)
#define RG_SSUSB_EQ_TRI_DET_TH_OFST               (0)

//U3D_EQ_EYE0
#define RG_SSUSB_EQ_EYE_XOFFSET_OFST              (25)
#define RG_SSUSB_EQ_EYE_MON_EN_OFST               (24)
#define RG_SSUSB_EQ_EYE0_Y_OFST                   (16)
#define RG_SSUSB_EQ_EYE1_Y_OFST                   (8)
#define RG_SSUSB_EQ_PILPO_ROUT_OFST               (7)
#define RG_SSUSB_EQ_PI_KPGAIN_OFST                (4)
#define RG_SSUSB_EQ_EYE_CNT_EN_OFST               (3)

//U3D_EQ_EYE1
#define RG_SSUSB_EQ_SIGDET_OFST                   (24)
#define RG_SSUSB_EQ_EYE_MASK_OFST                 (7)

//U3D_EQ_EYE2
#define RG_SSUSB_EQ_RX500M_CK_SEL_OFST            (31)
#define RG_SSUSB_EQ_SD_CNT1_OFST                  (24)
#define RG_SSUSB_EQ_ISIFLAG_SEL_OFST              (22)
#define RG_SSUSB_EQ_SD_CNT0_OFST                  (16)

//U3D_EQ_DFE0
#define RG_SSUSB_EQ_LEQMAX_OFST                   (28)
#define RG_SSUSB_EQ_DFEX_EN_OFST                  (27)
#define RG_SSUSB_EQ_DFEX_LF_SEL_OFST              (24)
#define RG_SSUSB_EQ_CHK_EYE_H_OFST                (23)
#define RG_SSUSB_EQ_PIEYE_INI_OFST                (16)
#define RG_SSUSB_EQ_PI90_INI_OFST                 (8)
#define RG_SSUSB_EQ_PI0_INI_OFST                  (0)

//U3D_EQ_DFE1
#define RG_SSUSB_EQ_REV_OFST                      (16)
#define RG_SSUSB_EQ_DFEYEN_DUR_OFST               (12)
#define RG_SSUSB_EQ_DFEXEN_DUR_OFST               (8)
#define RG_SSUSB_EQ_DFEX_RST_OFST                 (7)
#define RG_SSUSB_EQ_GATED_RXD_B_OFST              (6)
#define RG_SSUSB_EQ_PI90CK_SEL_OFST               (4)
#define RG_SSUSB_EQ_DFEX_DIS_OFST                 (2)
#define RG_SSUSB_EQ_DFEYEN_STOP_DIS_OFST          (1)
#define RG_SSUSB_EQ_DFEXEN_SEL_OFST               (0)

//U3D_EQ_DFE2
#define RG_SSUSB_EQ_MON_SEL_OFST                  (24)
#define RG_SSUSB_EQ_LEQOSC_DLYCNT_OFST            (16)
#define RG_SSUSB_EQ_DLEQOS_LFI_OFST               (8)
#define RG_SSUSB_EQ_LEQ_STOP_TO_OFST              (0)

//U3D_EQ_DFE3
#define RG_SSUSB_EQ_RESERVED_OFST                 (0)

//U3D_PHYD_MON0
#define RGS_SSUSB_BERT_BERC_OFST                  (16)
#define RGS_SSUSB_LFPS_OFST                       (12)
#define RGS_SSUSB_TRAINDEC_OFST                   (8)
#define RGS_SSUSB_SCP_PAT_OFST                    (0)

//U3D_PHYD_MON1
#define RGS_SSUSB_RX_FL_OUT_OFST                  (0)

//U3D_PHYD_MON2
#define RGS_SSUSB_T2RLB_ERRCNT_OFST               (16)
#define RGS_SSUSB_RETRACK_OFST                    (12)
#define RGS_SSUSB_RXPLL_LOCK_OFST                 (10)
#define RGS_SSUSB_CDR_VCOCAL_CPLT_D_OFST          (9)
#define RGS_SSUSB_PLL_VCOCAL_CPLT_D_OFST          (8)
#define RGS_SSUSB_PDNCTL_OFST                     (0)

//U3D_PHYD_MON3
#define RGS_SSUSB_TSEQ_ERRCNT_OFST                (16)
#define RGS_SSUSB_PRBS_ERRCNT_OFST                (0)

//U3D_PHYD_MON4
#define RGS_SSUSB_RX_LSLOCK_CNT_OFST              (24)
#define RGS_SSUSB_SCP_DETCNT_OFST                 (16)
#define RGS_SSUSB_TSEQ_DETCNT_OFST                (0)

//U3D_PHYD_MON5
#define RGS_SSUSB_EBUFMSG_OFST                    (16)
#define RGS_SSUSB_BERT_LOCK_OFST                  (15)
#define RGS_SSUSB_SCP_DET_OFST                    (14)
#define RGS_SSUSB_TSEQ_DET_OFST                   (13)
#define RGS_SSUSB_EBUF_UDF_OFST                   (12)
#define RGS_SSUSB_EBUF_OVF_OFST                   (11)
#define RGS_SSUSB_PRBS_PASSTH_OFST                (10)
#define RGS_SSUSB_PRBS_PASS_OFST                  (9)
#define RGS_SSUSB_PRBS_LOCK_OFST                  (8)
#define RGS_SSUSB_T2RLB_ERR_OFST                  (6)
#define RGS_SSUSB_T2RLB_PASSTH_OFST               (5)
#define RGS_SSUSB_T2RLB_PASS_OFST                 (4)
#define RGS_SSUSB_T2RLB_LOCK_OFST                 (3)
#define RGS_SSUSB_RX_IMPCAL_DONE_OFST             (2)
#define RGS_SSUSB_TX_IMPCAL_DONE_OFST             (1)
#define RGS_SSUSB_RXDETECTED_OFST                 (0)

//U3D_PHYD_MON6
#define RGS_SSUSB_SIGCAL_DONE_OFST                (30)
#define RGS_SSUSB_SIGCAL_CAL_OUT_OFST             (29)
#define RGS_SSUSB_SIGCAL_OFFSET_OFST              (24)
#define RGS_SSUSB_RX_IMP_SEL_OFST                 (16)
#define RGS_SSUSB_TX_IMP_SEL_OFST                 (8)
#define RGS_SSUSB_TFIFO_MSG_OFST                  (4)
#define RGS_SSUSB_RFIFO_MSG_OFST                  (0)

//U3D_PHYD_MON7
#define RGS_SSUSB_FT_OUT_OFST                     (8)
#define RGS_SSUSB_PRB_OUT_OFST                    (0)

//U3D_PHYA_RX_MON0
#define RGS_SSUSB_EQ_DCLEQ_OFST                   (24)
#define RGS_SSUSB_EQ_DCD0H_OFST                   (16)
#define RGS_SSUSB_EQ_DCD0L_OFST                   (8)
#define RGS_SSUSB_EQ_DCD1H_OFST                   (0)

//U3D_PHYA_RX_MON1
#define RGS_SSUSB_EQ_DCD1L_OFST                   (24)
#define RGS_SSUSB_EQ_DCE0_OFST                    (16)
#define RGS_SSUSB_EQ_DCE1_OFST                    (8)
#define RGS_SSUSB_EQ_DCHHL_OFST                   (0)

//U3D_PHYA_RX_MON2
#define RGS_SSUSB_EQ_LEQ_STOP_OFST                (31)
#define RGS_SSUSB_EQ_DCLHL_OFST                   (24)
#define RGS_SSUSB_EQ_STATUS_OFST                  (16)
#define RGS_SSUSB_EQ_DCEYE0_OFST                  (8)
#define RGS_SSUSB_EQ_DCEYE1_OFST                  (0)

//U3D_PHYA_RX_MON3
#define RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_0_OFST    (0)

//U3D_PHYA_RX_MON4
#define RGS_SSUSB_EQ_EYE_MONITOR_ERRCNT_1_OFST    (0)

//U3D_PHYA_RX_MON5
#define RGS_SSUSB_EQ_DCLEQOS_OFST                 (8)
#define RGS_SSUSB_EQ_EYE_CNT_RDY_OFST             (7)
#define RGS_SSUSB_EQ_PILPO_OFST                   (0)

//U3D_PHYD_CPPAT2
#define RG_SSUSB_CPPAT_OUT_H_TMP2_OFST            (16)
#define RG_SSUSB_CPPAT_OUT_H_TMP1_OFST            (8)
#define RG_SSUSB_CPPAT_OUT_H_TMP0_OFST            (0)

//U3D_EQ_EYE3
#define RG_SSUSB_EQ_LEQ_SHIFT_OFST                (24)
#define RG_SSUSB_EQ_EYE_CNT_OFST                  (0)

//U3D_KBAND_OUT
#define RGS_SSUSB_CDR_BAND_5G_OFST                (24)
#define RGS_SSUSB_CDR_BAND_2P5G_OFST              (16)
#define RGS_SSUSB_PLL_BAND_5G_OFST                (8)
#define RGS_SSUSB_PLL_BAND_2P5G_OFST              (0)

//U3D_KBAND_OUT1
#define RGS_SSUSB_CDR_VCOCAL_FAIL_OFST            (24)
#define RGS_SSUSB_CDR_VCOCAL_STATE_OFST           (16)
#define RGS_SSUSB_PLL_VCOCAL_FAIL_OFST            (8)
#define RGS_SSUSB_PLL_VCOCAL_STATE_OFST           (0)


///////////////////////////////////////////////////////////////////////////////

struct u3phyd_bank2_reg {
	//0x0
	PHY_LE32 b2_phyd_top1;
	PHY_LE32 b2_phyd_top2;
	PHY_LE32 b2_phyd_top3;
	PHY_LE32 b2_phyd_top4;
	//0x10
	PHY_LE32 b2_phyd_top5;
	PHY_LE32 b2_phyd_top6;
	PHY_LE32 b2_phyd_top7;
	PHY_LE32 b2_phyd_p_sigdet1;
	//0x20
	PHY_LE32 b2_phyd_p_sigdet2;
	PHY_LE32 b2_phyd_p_sigdet_cal1;
	PHY_LE32 b2_phyd_rxdet1;
	PHY_LE32 b2_phyd_rxdet2;
	//0x30
	PHY_LE32 b2_phyd_misc0;
	PHY_LE32 b2_phyd_misc2;
	PHY_LE32 b2_phyd_misc3;
	PHY_LE32 reserve0;
	//0x40
	PHY_LE32 b2_rosc_0;
	PHY_LE32 b2_rosc_1;
	PHY_LE32 b2_rosc_2;
	PHY_LE32 b2_rosc_3;
	//0x50
	PHY_LE32 b2_rosc_4;
	PHY_LE32 b2_rosc_5;
	PHY_LE32 b2_rosc_6;
	PHY_LE32 b2_rosc_7;
	//0x60
	PHY_LE32 b2_rosc_8;
	PHY_LE32 b2_rosc_9;
	PHY_LE32 b2_rosc_a;
	PHY_LE32 reserve1;
	//0x70~0xd0
	PHY_LE32 reserve2[28];
	//0xe0
	PHY_LE32 phyd_version;
	PHY_LE32 phyd_model;
};

//U3D_B2_PHYD_TOP1
#define RG_SSUSB_PCIE2_K_EMP                      (0xf<<28) //31:28
#define RG_SSUSB_PCIE2_K_FUL                      (0xf<<24) //27:24
#define RG_SSUSB_TX_EIDLE_LP_EN                   (0x1<<17) //17:17
#define RG_SSUSB_FORCE_TX_EIDLE_LP_EN             (0x1<<16) //16:16
#define RG_SSUSB_SIGDET_EN                        (0x1<<15) //15:15
#define RG_SSUSB_FORCE_SIGDET_EN                  (0x1<<14) //14:14
#define RG_SSUSB_CLKRX_EN                         (0x1<<13) //13:13
#define RG_SSUSB_FORCE_CLKRX_EN                   (0x1<<12) //12:12
#define RG_SSUSB_CLKTX_EN                         (0x1<<11) //11:11
#define RG_SSUSB_FORCE_CLKTX_EN                   (0x1<<10) //10:10
#define RG_SSUSB_CLK_REQ_N_I                      (0x1<<9) //9:9
#define RG_SSUSB_FORCE_CLK_REQ_N_I                (0x1<<8) //8:8
#define RG_SSUSB_RATE                             (0x1<<6) //6:6
#define RG_SSUSB_FORCE_RATE                       (0x1<<5) //5:5
#define RG_SSUSB_PCIE_MODE_SEL                    (0x1<<4) //4:4
#define RG_SSUSB_FORCE_PCIE_MODE_SEL              (0x1<<3) //3:3
#define RG_SSUSB_PHY_MODE                         (0x3<<1) //2:1
#define RG_SSUSB_FORCE_PHY_MODE                   (0x1<<0) //0:0

//U3D_B2_PHYD_TOP2
#define RG_SSUSB_FORCE_IDRV_6DB                   (0x1<<30) //30:30
#define RG_SSUSB_IDRV_6DB                         (0x3f<<24) //29:24
#define RG_SSUSB_FORCE_IDEM_3P5DB                 (0x1<<22) //22:22
#define RG_SSUSB_IDEM_3P5DB                       (0x3f<<16) //21:16
#define RG_SSUSB_FORCE_IDRV_3P5DB                 (0x1<<14) //14:14
#define RG_SSUSB_IDRV_3P5DB                       (0x3f<<8) //13:8
#define RG_SSUSB_FORCE_IDRV_0DB                   (0x1<<6) //6:6
#define RG_SSUSB_IDRV_0DB                         (0x3f<<0) //5:0

//U3D_B2_PHYD_TOP3
#define RG_SSUSB_TX_BIASI                         (0x7<<25) //27:25
#define RG_SSUSB_FORCE_TX_BIASI_EN                (0x1<<24) //24:24
#define RG_SSUSB_TX_BIASI_EN                      (0x1<<16) //16:16
#define RG_SSUSB_FORCE_TX_BIASI                   (0x1<<13) //13:13
#define RG_SSUSB_FORCE_IDEM_6DB                   (0x1<<8) //8:8
#define RG_SSUSB_IDEM_6DB                         (0x3f<<0) //5:0

//U3D_B2_PHYD_TOP4
#define RG_SSUSB_G1_CDR_BIC_LTR                   (0xf<<28) //31:28
#define RG_SSUSB_G1_CDR_BIC_LTD0                  (0xf<<24) //27:24
#define RG_SSUSB_G1_CDR_BC_LTD1                   (0x1f<<16) //20:16
#define RG_SSUSB_G1_CDR_BC_LTR                    (0x1f<<8) //12:8
#define RG_SSUSB_G1_CDR_BC_LTD0                   (0x1f<<0) //4:0

//U3D_B2_PHYD_TOP5
#define RG_SSUSB_G1_CDR_BIR_LTD1                  (0x1f<<24) //28:24
#define RG_SSUSB_G1_CDR_BIR_LTR                   (0x1f<<16) //20:16
#define RG_SSUSB_G1_CDR_BIR_LTD0                  (0x1f<<8) //12:8
#define RG_SSUSB_G1_CDR_BIC_LTD1                  (0xf<<0) //3:0

//U3D_B2_PHYD_TOP6
#define RG_SSUSB_G2_CDR_BIC_LTR                   (0xf<<28) //31:28
#define RG_SSUSB_G2_CDR_BIC_LTD0                  (0xf<<24) //27:24
#define RG_SSUSB_G2_CDR_BC_LTD1                   (0x1f<<16) //20:16
#define RG_SSUSB_G2_CDR_BC_LTR                    (0x1f<<8) //12:8
#define RG_SSUSB_G2_CDR_BC_LTD0                   (0x1f<<0) //4:0

//U3D_B2_PHYD_TOP7
#define RG_SSUSB_G2_CDR_BIR_LTD1                  (0x1f<<24) //28:24
#define RG_SSUSB_G2_CDR_BIR_LTR                   (0x1f<<16) //20:16
#define RG_SSUSB_G2_CDR_BIR_LTD0                  (0x1f<<8) //12:8
#define RG_SSUSB_G2_CDR_BIC_LTD1                  (0xf<<0) //3:0

//U3D_B2_PHYD_P_SIGDET1
#define RG_SSUSB_P_SIGDET_FLT_DIS                 (0x1<<31) //31:31
#define RG_SSUSB_P_SIGDET_FLT_G2_DEAST_SEL        (0x7f<<24) //30:24
#define RG_SSUSB_P_SIGDET_FLT_G1_DEAST_SEL        (0x7f<<16) //22:16
#define RG_SSUSB_P_SIGDET_FLT_P2_AST_SEL          (0x7f<<8) //14:8
#define RG_SSUSB_P_SIGDET_FLT_PX_AST_SEL          (0x7f<<0) //6:0

//U3D_B2_PHYD_P_SIGDET2
#define RG_SSUSB_P_SIGDET_RX_VAL_S                (0x1<<29) //29:29
#define RG_SSUSB_P_SIGDET_L0S_DEAS_SEL            (0x1<<28) //28:28
#define RG_SSUSB_P_SIGDET_L0_EXIT_S               (0x1<<27) //27:27
#define RG_SSUSB_P_SIGDET_L0S_EXIT_T_S            (0x3<<25) //26:25
#define RG_SSUSB_P_SIGDET_L0S_EXIT_S              (0x1<<24) //24:24
#define RG_SSUSB_P_SIGDET_L0S_ENTRY_S             (0x1<<16) //16:16
#define RG_SSUSB_P_SIGDET_PRB_SEL                 (0x1<<10) //10:10
#define RG_SSUSB_P_SIGDET_BK_SIG_T                (0x3<<8) //9:8
#define RG_SSUSB_P_SIGDET_P2_RXLFPS               (0x1<<6) //6:6
#define RG_SSUSB_P_SIGDET_NON_BK_AD               (0x1<<5) //5:5
#define RG_SSUSB_P_SIGDET_BK_B_RXEQ               (0x1<<4) //4:4
#define RG_SSUSB_P_SIGDET_G2_KO_SEL               (0x3<<2) //3:2
#define RG_SSUSB_P_SIGDET_G1_KO_SEL               (0x3<<0) //1:0

//U3D_B2_PHYD_P_SIGDET_CAL1
#define RG_SSUSB_P_SIGDET_CAL_OFFSET              (0x1f<<24) //28:24
#define RG_SSUSB_P_FORCE_SIGDET_CAL_OFFSET        (0x1<<16) //16:16
#define RG_SSUSB_P_SIGDET_CAL_EN                  (0x1<<8) //8:8
#define RG_SSUSB_P_FORCE_SIGDET_CAL_EN            (0x1<<3) //3:3
#define RG_SSUSB_P_SIGDET_FLT_EN                  (0x1<<2) //2:2
#define RG_SSUSB_P_SIGDET_SAMPLE_PRD              (0x1<<1) //1:1
#define RG_SSUSB_P_SIGDET_REK                     (0x1<<0) //0:0

//U3D_B2_PHYD_RXDET1
#define RG_SSUSB_RXDET_PRB_SEL                    (0x1<<31) //31:31
#define RG_SSUSB_FORCE_CMDET                      (0x1<<30) //30:30
#define RG_SSUSB_RXDET_EN                         (0x1<<29) //29:29
#define RG_SSUSB_FORCE_RXDET_EN                   (0x1<<28) //28:28
#define RG_SSUSB_RXDET_K_TWICE                    (0x1<<27) //27:27
#define RG_SSUSB_RXDET_STB3_SET                   (0x1ff<<18) //26:18
#define RG_SSUSB_RXDET_STB2_SET                   (0x1ff<<9) //17:9
#define RG_SSUSB_RXDET_STB1_SET                   (0x1ff<<0) //8:0

//U3D_B2_PHYD_RXDET2
#define RG_SSUSB_PHYD_TRAINDEC_FORCE_CGEN         (0x1<<31) //31:31
#define RG_SSUSB_PHYD_BERTLB_FORCE_CGEN           (0x1<<30) //30:30
#define RG_SSUSB_PHYD_T2RLB_FORCE_CGEN            (0x1<<29) //29:29
#define RG_SSUSB_PDN_T_SEL                        (0x3<<18) //19:18
#define RG_SSUSB_RXDET_STB3_SET_P3                (0x1ff<<9) //17:9
#define RG_SSUSB_RXDET_STB2_SET_P3                (0x1ff<<0) //8:0

//U3D_B2_PHYD_MISC0
#define RG_SSUSB_FORCE_PLL_DDS_HF_EN              (0x1<<22) //22:22
#define RG_SSUSB_PLL_DDS_HF_EN_MAN                (0x1<<21) //21:21
#define RG_SSUSB_RXLFPS_ENTXDRV                   (0x1<<20) //20:20
#define RG_SSUSB_RX_FL_UNLOCKTH                   (0xf<<16) //19:16
#define RG_SSUSB_LFPS_PSEL                        (0x1<<15) //15:15
#define RG_SSUSB_RX_SIGDET_EN                     (0x1<<14) //14:14
#define RG_SSUSB_RX_SIGDET_EN_SEL                 (0x1<<13) //13:13
#define RG_SSUSB_RX_PI_CAL_EN                     (0x1<<12) //12:12
#define RG_SSUSB_RX_PI_CAL_EN_SEL                 (0x1<<11) //11:11
#define RG_SSUSB_P3_CLS_CK_SEL                    (0x1<<10) //10:10
#define RG_SSUSB_T2RLB_PSEL                       (0x3<<8) //9:8
#define RG_SSUSB_PPCTL_PSEL                       (0x7<<5) //7:5
#define RG_SSUSB_PHYD_TX_DATA_INV                 (0x1<<4) //4:4
#define RG_SSUSB_BERTLB_PSEL                      (0x3<<2) //3:2
#define RG_SSUSB_RETRACK_DIS                      (0x1<<1) //1:1
#define RG_SSUSB_PPERRCNT_CLR                     (0x1<<0) //0:0

//U3D_B2_PHYD_MISC2
#define RG_SSUSB_FRC_PLL_DDS_PREDIV2              (0x1<<31) //31:31
#define RG_SSUSB_FRC_PLL_DDS_IADJ                 (0xf<<27) //30:27
#define RG_SSUSB_P_SIGDET_125FILTER               (0x1<<26) //26:26
#define RG_SSUSB_P_SIGDET_RST_FILTER              (0x1<<25) //25:25
#define RG_SSUSB_P_SIGDET_EID_USE_RAW             (0x1<<24) //24:24
#define RG_SSUSB_P_SIGDET_LTD_USE_RAW             (0x1<<23) //23:23
#define RG_SSUSB_EIDLE_BF_RXDET                   (0x1<<22) //22:22
#define RG_SSUSB_EIDLE_LP_STBCYC                  (0x1ff<<13) //21:13
#define RG_SSUSB_TX_EIDLE_LP_POSTDLY              (0x3f<<7) //12:7
#define RG_SSUSB_TX_EIDLE_LP_PREDLY               (0x3f<<1) //6:1
#define RG_SSUSB_TX_EIDLE_LP_EN_ADV               (0x1<<0) //0:0

//U3D_B2_PHYD_MISC3
#define RGS_SSUSB_DDS_CALIB_C_STATE               (0x7<<16) //18:16
#define RGS_SSUSB_PPERRCNT                        (0xffff<<0) //15:0

//U3D_B2_ROSC_0
#define RG_SSUSB_RING_OSC_CNTEND                  (0x1ff<<23) //31:23
#define RG_SSUSB_XTAL_OSC_CNTEND                  (0x7f<<16) //22:16
#define RG_SSUSB_RING_OSC_EN                      (0x1<<3) //3:3
#define RG_SSUSB_RING_OSC_FORCE_EN                (0x1<<2) //2:2
#define RG_SSUSB_FRC_RING_BYPASS_DET              (0x1<<1) //1:1
#define RG_SSUSB_RING_BYPASS_DET                  (0x1<<0) //0:0

//U3D_B2_ROSC_1
#define RG_SSUSB_RING_OSC_FRC_P3                  (0x1<<20) //20:20
#define RG_SSUSB_RING_OSC_P3                      (0x1<<19) //19:19
#define RG_SSUSB_RING_OSC_FRC_RECAL               (0x3<<17) //18:17
#define RG_SSUSB_RING_OSC_RECAL                   (0x1<<16) //16:16
#define RG_SSUSB_RING_OSC_SEL                     (0xff<<8) //15:8
#define RG_SSUSB_RING_OSC_FRC_SEL                 (0x1<<0) //0:0

//U3D_B2_ROSC_2
#define RG_SSUSB_RING_DET_STRCYC2                 (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_STRCYC1                 (0xffff<<0) //15:0

//U3D_B2_ROSC_3
#define RG_SSUSB_RING_DET_DETWIN1                 (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_STRCYC3                 (0xffff<<0) //15:0

//U3D_B2_ROSC_4
#define RG_SSUSB_RING_DET_DETWIN3                 (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_DETWIN2                 (0xffff<<0) //15:0

//U3D_B2_ROSC_5
#define RG_SSUSB_RING_DET_LBOND1                  (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_UBOND1                  (0xffff<<0) //15:0

//U3D_B2_ROSC_6
#define RG_SSUSB_RING_DET_LBOND2                  (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_UBOND2                  (0xffff<<0) //15:0

//U3D_B2_ROSC_7
#define RG_SSUSB_RING_DET_LBOND3                  (0xffff<<16) //31:16
#define RG_SSUSB_RING_DET_UBOND3                  (0xffff<<0) //15:0

//U3D_B2_ROSC_8
#define RG_SSUSB_RING_RESERVE                     (0xffff<<16) //31:16
#define RG_SSUSB_ROSC_PROB_SEL                    (0xf<<2) //5:2
#define RG_SSUSB_RING_FREQMETER_EN                (0x1<<1) //1:1
#define RG_SSUSB_RING_DET_BPS_UBOND               (0x1<<0) //0:0

//U3D_B2_ROSC_9
#define RGS_FM_RING_CNT                           (0xffff<<16) //31:16
#define RGS_SSUSB_RING_OSC_STATE                  (0x3<<10) //11:10
#define RGS_SSUSB_RING_OSC_STABLE                 (0x1<<9) //9:9
#define RGS_SSUSB_RING_OSC_CAL_FAIL               (0x1<<8) //8:8
#define RGS_SSUSB_RING_OSC_CAL                    (0xff<<0) //7:0

//U3D_B2_ROSC_A
#define RGS_SSUSB_ROSC_PROB_OUT                   (0xff<<0) //7:0

//U3D_PHYD_VERSION
#define RGS_SSUSB_PHYD_VERSION                    (0xffffffff<<0) //31:0

//U3D_PHYD_MODEL
#define RGS_SSUSB_PHYD_MODEL                      (0xffffffff<<0) //31:0


/* OFFSET */

//U3D_B2_PHYD_TOP1
#define RG_SSUSB_PCIE2_K_EMP_OFST                 (28)
#define RG_SSUSB_PCIE2_K_FUL_OFST                 (24)
#define RG_SSUSB_TX_EIDLE_LP_EN_OFST              (17)
#define RG_SSUSB_FORCE_TX_EIDLE_LP_EN_OFST        (16)
#define RG_SSUSB_SIGDET_EN_OFST                   (15)
#define RG_SSUSB_FORCE_SIGDET_EN_OFST             (14)
#define RG_SSUSB_CLKRX_EN_OFST                    (13)
#define RG_SSUSB_FORCE_CLKRX_EN_OFST              (12)
#define RG_SSUSB_CLKTX_EN_OFST                    (11)
#define RG_SSUSB_FORCE_CLKTX_EN_OFST              (10)
#define RG_SSUSB_CLK_REQ_N_I_OFST                 (9)
#define RG_SSUSB_FORCE_CLK_REQ_N_I_OFST           (8)
#define RG_SSUSB_RATE_OFST                        (6)
#define RG_SSUSB_FORCE_RATE_OFST                  (5)
#define RG_SSUSB_PCIE_MODE_SEL_OFST               (4)
#define RG_SSUSB_FORCE_PCIE_MODE_SEL_OFST         (3)
#define RG_SSUSB_PHY_MODE_OFST                    (1)
#define RG_SSUSB_FORCE_PHY_MODE_OFST              (0)

//U3D_B2_PHYD_TOP2
#define RG_SSUSB_FORCE_IDRV_6DB_OFST              (30)
#define RG_SSUSB_IDRV_6DB_OFST                    (24)
#define RG_SSUSB_FORCE_IDEM_3P5DB_OFST            (22)
#define RG_SSUSB_IDEM_3P5DB_OFST                  (16)
#define RG_SSUSB_FORCE_IDRV_3P5DB_OFST            (14)
#define RG_SSUSB_IDRV_3P5DB_OFST                  (8)
#define RG_SSUSB_FORCE_IDRV_0DB_OFST              (6)
#define RG_SSUSB_IDRV_0DB_OFST                    (0)

//U3D_B2_PHYD_TOP3
#define RG_SSUSB_TX_BIASI_OFST                    (25)
#define RG_SSUSB_FORCE_TX_BIASI_EN_OFST           (24)
#define RG_SSUSB_TX_BIASI_EN_OFST                 (16)
#define RG_SSUSB_FORCE_TX_BIASI_OFST              (13)
#define RG_SSUSB_FORCE_IDEM_6DB_OFST              (8)
#define RG_SSUSB_IDEM_6DB_OFST                    (0)

//U3D_B2_PHYD_TOP4
#define RG_SSUSB_G1_CDR_BIC_LTR_OFST              (28)
#define RG_SSUSB_G1_CDR_BIC_LTD0_OFST             (24)
#define RG_SSUSB_G1_CDR_BC_LTD1_OFST              (16)
#define RG_SSUSB_G1_CDR_BC_LTR_OFST               (8)
#define RG_SSUSB_G1_CDR_BC_LTD0_OFST              (0)

//U3D_B2_PHYD_TOP5
#define RG_SSUSB_G1_CDR_BIR_LTD1_OFST             (24)
#define RG_SSUSB_G1_CDR_BIR_LTR_OFST              (16)
#define RG_SSUSB_G1_CDR_BIR_LTD0_OFST             (8)
#define RG_SSUSB_G1_CDR_BIC_LTD1_OFST             (0)

//U3D_B2_PHYD_TOP6
#define RG_SSUSB_G2_CDR_BIC_LTR_OFST              (28)
#define RG_SSUSB_G2_CDR_BIC_LTD0_OFST             (24)
#define RG_SSUSB_G2_CDR_BC_LTD1_OFST              (16)
#define RG_SSUSB_G2_CDR_BC_LTR_OFST               (8)
#define RG_SSUSB_G2_CDR_BC_LTD0_OFST              (0)

//U3D_B2_PHYD_TOP7
#define RG_SSUSB_G2_CDR_BIR_LTD1_OFST             (24)
#define RG_SSUSB_G2_CDR_BIR_LTR_OFST              (16)
#define RG_SSUSB_G2_CDR_BIR_LTD0_OFST             (8)
#define RG_SSUSB_G2_CDR_BIC_LTD1_OFST             (0)

//U3D_B2_PHYD_P_SIGDET1
#define RG_SSUSB_P_SIGDET_FLT_DIS_OFST            (31)
#define RG_SSUSB_P_SIGDET_FLT_G2_DEAST_SEL_OFST   (24)
#define RG_SSUSB_P_SIGDET_FLT_G1_DEAST_SEL_OFST   (16)
#define RG_SSUSB_P_SIGDET_FLT_P2_AST_SEL_OFST     (8)
#define RG_SSUSB_P_SIGDET_FLT_PX_AST_SEL_OFST     (0)

//U3D_B2_PHYD_P_SIGDET2
#define RG_SSUSB_P_SIGDET_RX_VAL_S_OFST           (29)
#define RG_SSUSB_P_SIGDET_L0S_DEAS_SEL_OFST       (28)
#define RG_SSUSB_P_SIGDET_L0_EXIT_S_OFST          (27)
#define RG_SSUSB_P_SIGDET_L0S_EXIT_T_S_OFST       (25)
#define RG_SSUSB_P_SIGDET_L0S_EXIT_S_OFST         (24)
#define RG_SSUSB_P_SIGDET_L0S_ENTRY_S_OFST        (16)
#define RG_SSUSB_P_SIGDET_PRB_SEL_OFST            (10)
#define RG_SSUSB_P_SIGDET_BK_SIG_T_OFST           (8)
#define RG_SSUSB_P_SIGDET_P2_RXLFPS_OFST          (6)
#define RG_SSUSB_P_SIGDET_NON_BK_AD_OFST          (5)
#define RG_SSUSB_P_SIGDET_BK_B_RXEQ_OFST          (4)
#define RG_SSUSB_P_SIGDET_G2_KO_SEL_OFST          (2)
#define RG_SSUSB_P_SIGDET_G1_KO_SEL_OFST          (0)

//U3D_B2_PHYD_P_SIGDET_CAL1
#define RG_SSUSB_P_SIGDET_CAL_OFFSET_OFST         (24)
#define RG_SSUSB_P_FORCE_SIGDET_CAL_OFFSET_OFST   (16)
#define RG_SSUSB_P_SIGDET_CAL_EN_OFST             (8)
#define RG_SSUSB_P_FORCE_SIGDET_CAL_EN_OFST       (3)
#define RG_SSUSB_P_SIGDET_FLT_EN_OFST             (2)
#define RG_SSUSB_P_SIGDET_SAMPLE_PRD_OFST         (1)
#define RG_SSUSB_P_SIGDET_REK_OFST                (0)

//U3D_B2_PHYD_RXDET1
#define RG_SSUSB_RXDET_PRB_SEL_OFST               (31)
#define RG_SSUSB_FORCE_CMDET_OFST                 (30)
#define RG_SSUSB_RXDET_EN_OFST                    (29)
#define RG_SSUSB_FORCE_RXDET_EN_OFST              (28)
#define RG_SSUSB_RXDET_K_TWICE_OFST               (27)
#define RG_SSUSB_RXDET_STB3_SET_OFST              (18)
#define RG_SSUSB_RXDET_STB2_SET_OFST              (9)
#define RG_SSUSB_RXDET_STB1_SET_OFST              (0)

//U3D_B2_PHYD_RXDET2
#define RG_SSUSB_PHYD_TRAINDEC_FORCE_CGEN_OFST    (31)
#define RG_SSUSB_PHYD_BERTLB_FORCE_CGEN_OFST      (30)
#define RG_SSUSB_PHYD_T2RLB_FORCE_CGEN_OFST       (29)
#define RG_SSUSB_PDN_T_SEL_OFST                   (18)
#define RG_SSUSB_RXDET_STB3_SET_P3_OFST           (9)
#define RG_SSUSB_RXDET_STB2_SET_P3_OFST           (0)

//U3D_B2_PHYD_MISC0
#define RG_SSUSB_FORCE_PLL_DDS_HF_EN_OFST         (22)
#define RG_SSUSB_PLL_DDS_HF_EN_MAN_OFST           (21)
#define RG_SSUSB_RXLFPS_ENTXDRV_OFST              (20)
#define RG_SSUSB_RX_FL_UNLOCKTH_OFST              (16)
#define RG_SSUSB_LFPS_PSEL_OFST                   (15)
#define RG_SSUSB_RX_SIGDET_EN_OFST                (14)
#define RG_SSUSB_RX_SIGDET_EN_SEL_OFST            (13)
#define RG_SSUSB_RX_PI_CAL_EN_OFST                (12)
#define RG_SSUSB_RX_PI_CAL_EN_SEL_OFST            (11)
#define RG_SSUSB_P3_CLS_CK_SEL_OFST               (10)
#define RG_SSUSB_T2RLB_PSEL_OFST                  (8)
#define RG_SSUSB_PPCTL_PSEL_OFST                  (5)
#define RG_SSUSB_PHYD_TX_DATA_INV_OFST            (4)
#define RG_SSUSB_BERTLB_PSEL_OFST                 (2)
#define RG_SSUSB_RETRACK_DIS_OFST                 (1)
#define RG_SSUSB_PPERRCNT_CLR_OFST                (0)

//U3D_B2_PHYD_MISC2
#define RG_SSUSB_FRC_PLL_DDS_PREDIV2_OFST         (31)
#define RG_SSUSB_FRC_PLL_DDS_IADJ_OFST            (27)
#define RG_SSUSB_P_SIGDET_125FILTER_OFST          (26)
#define RG_SSUSB_P_SIGDET_RST_FILTER_OFST         (25)
#define RG_SSUSB_P_SIGDET_EID_USE_RAW_OFST        (24)
#define RG_SSUSB_P_SIGDET_LTD_USE_RAW_OFST        (23)
#define RG_SSUSB_EIDLE_BF_RXDET_OFST              (22)
#define RG_SSUSB_EIDLE_LP_STBCYC_OFST             (13)
#define RG_SSUSB_TX_EIDLE_LP_POSTDLY_OFST         (7)
#define RG_SSUSB_TX_EIDLE_LP_PREDLY_OFST          (1)
#define RG_SSUSB_TX_EIDLE_LP_EN_ADV_OFST          (0)

//U3D_B2_PHYD_MISC3
#define RGS_SSUSB_DDS_CALIB_C_STATE_OFST          (16)
#define RGS_SSUSB_PPERRCNT_OFST                   (0)

//U3D_B2_ROSC_0
#define RG_SSUSB_RING_OSC_CNTEND_OFST             (23)
#define RG_SSUSB_XTAL_OSC_CNTEND_OFST             (16)
#define RG_SSUSB_RING_OSC_EN_OFST                 (3)
#define RG_SSUSB_RING_OSC_FORCE_EN_OFST           (2)
#define RG_SSUSB_FRC_RING_BYPASS_DET_OFST         (1)
#define RG_SSUSB_RING_BYPASS_DET_OFST             (0)

//U3D_B2_ROSC_1
#define RG_SSUSB_RING_OSC_FRC_P3_OFST             (20)
#define RG_SSUSB_RING_OSC_P3_OFST                 (19)
#define RG_SSUSB_RING_OSC_FRC_RECAL_OFST          (17)
#define RG_SSUSB_RING_OSC_RECAL_OFST              (16)
#define RG_SSUSB_RING_OSC_SEL_OFST                (8)
#define RG_SSUSB_RING_OSC_FRC_SEL_OFST            (0)

//U3D_B2_ROSC_2
#define RG_SSUSB_RING_DET_STRCYC2_OFST            (16)
#define RG_SSUSB_RING_DET_STRCYC1_OFST            (0)

//U3D_B2_ROSC_3
#define RG_SSUSB_RING_DET_DETWIN1_OFST            (16)
#define RG_SSUSB_RING_DET_STRCYC3_OFST            (0)

//U3D_B2_ROSC_4
#define RG_SSUSB_RING_DET_DETWIN3_OFST            (16)
#define RG_SSUSB_RING_DET_DETWIN2_OFST            (0)

//U3D_B2_ROSC_5
#define RG_SSUSB_RING_DET_LBOND1_OFST             (16)
#define RG_SSUSB_RING_DET_UBOND1_OFST             (0)

//U3D_B2_ROSC_6
#define RG_SSUSB_RING_DET_LBOND2_OFST             (16)
#define RG_SSUSB_RING_DET_UBOND2_OFST             (0)

//U3D_B2_ROSC_7
#define RG_SSUSB_RING_DET_LBOND3_OFST             (16)
#define RG_SSUSB_RING_DET_UBOND3_OFST             (0)

//U3D_B2_ROSC_8
#define RG_SSUSB_RING_RESERVE_OFST                (16)
#define RG_SSUSB_ROSC_PROB_SEL_OFST               (2)
#define RG_SSUSB_RING_FREQMETER_EN_OFST           (1)
#define RG_SSUSB_RING_DET_BPS_UBOND_OFST          (0)

//U3D_B2_ROSC_9
#define RGS_FM_RING_CNT_OFST                      (16)
#define RGS_SSUSB_RING_OSC_STATE_OFST             (10)
#define RGS_SSUSB_RING_OSC_STABLE_OFST            (9)
#define RGS_SSUSB_RING_OSC_CAL_FAIL_OFST          (8)
#define RGS_SSUSB_RING_OSC_CAL_OFST               (0)

//U3D_B2_ROSC_A
#define RGS_SSUSB_ROSC_PROB_OUT_OFST              (0)

//U3D_PHYD_VERSION
#define RGS_SSUSB_PHYD_VERSION_OFST               (0)

//U3D_PHYD_MODEL
#define RGS_SSUSB_PHYD_MODEL_OFST                 (0)


///////////////////////////////////////////////////////////////////////////////

struct sifslv_chip_reg {
	PHY_LE32 xtalbias;
	PHY_LE32 syspll1;
	PHY_LE32 gpio_ctla;
	PHY_LE32 gpio_ctlb;
	PHY_LE32 gpio_ctlc;
};

//U3D_GPIO_CTLA
#define RG_C60802_GPIO_CTLA                       (0xffffffff<<0) //31:0

//U3D_GPIO_CTLB
#define RG_C60802_GPIO_CTLB                       (0xffffffff<<0) //31:0

//U3D_GPIO_CTLC
#define RG_C60802_GPIO_CTLC                       (0xffffffff<<0) //31:0

/* OFFSET */

//U3D_GPIO_CTLA
#define RG_C60802_GPIO_CTLA_OFST                  (0)

//U3D_GPIO_CTLB
#define RG_C60802_GPIO_CTLB_OFST                  (0)

//U3D_GPIO_CTLC
#define RG_C60802_GPIO_CTLC_OFST                  (0)

///////////////////////////////////////////////////////////////////////////////

struct sifslv_fm_feg {
	//0x0
	PHY_LE32 fmcr0;
	PHY_LE32 fmcr1;
	PHY_LE32 fmcr2;
	PHY_LE32 fmmonr0;
	//0x10
	PHY_LE32 fmmonr1;
};

//U3D_FMCR0
#define RG_LOCKTH                                 (0xf<<28) //31:28
#define RG_MONCLK_SEL                             (0x3<<26) //27:26
#define RG_FM_MODE                                (0x1<<25) //25:25
#define RG_FREQDET_EN                             (0x1<<24) //24:24
#define RG_CYCLECNT                               (0xffffff<<0) //23:0

//U3D_FMCR1
#define RG_TARGET                                 (0xffffffff<<0) //31:0

//U3D_FMCR2
#define RG_OFFSET                                 (0xffffffff<<0) //31:0

//U3D_FMMONR0
#define USB_FM_OUT                                (0xffffffff<<0) //31:0

//U3D_FMMONR1
#define RG_MONCLK_SEL_3                           (0x1<<9) //9:9
#define RG_FRCK_EN                                (0x1<<8) //8:8
#define USBPLL_LOCK                               (0x1<<1) //1:1
#define USB_FM_VLD                                (0x1<<0) //0:0


/* OFFSET */

//U3D_FMCR0
#define RG_LOCKTH_OFST                            (28)
#define RG_MONCLK_SEL_OFST                        (26)
#define RG_FM_MODE_OFST                           (25)
#define RG_FREQDET_EN_OFST                        (24)
#define RG_CYCLECNT_OFST                          (0)

//U3D_FMCR1
#define RG_TARGET_OFST                            (0)

//U3D_FMCR2
#define RG_OFFSET_OFST                            (0)

//U3D_FMMONR0
#define USB_FM_OUT_OFST                           (0)

//U3D_FMMONR1
#define RG_MONCLK_SEL_3_OFST                      (9)
#define RG_FRCK_EN_OFST                           (8)
#define USBPLL_LOCK_OFST                          (1)
#define USB_FM_VLD_OFST                           (0)


///////////////////////////////////////////////////////////////////////////////

PHY_INT32 phy_init(struct u3phy_info *info);
PHY_INT32 phy_change_pipe_phase(struct u3phy_info *info, PHY_INT32 phy_drv, PHY_INT32 pipe_phase);
PHY_INT32 eyescan_init(struct u3phy_info *info);
PHY_INT32 phy_eyescan(struct u3phy_info *info, PHY_INT32 x_t1, PHY_INT32 y_t1, PHY_INT32 x_br, PHY_INT32 y_br, PHY_INT32 delta_x, PHY_INT32 delta_y
		, PHY_INT32 eye_cnt, PHY_INT32 num_cnt, PHY_INT32 PI_cal_en, PHY_INT32 num_ignore_cnt);
PHY_INT32 u2_save_cur_en(struct u3phy_info *info);
PHY_INT32 u2_save_cur_re(struct u3phy_info *info);
PHY_INT32 u2_slew_rate_calibration(struct u3phy_info *info);

#endif
#endif
