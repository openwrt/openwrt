#ifndef AMAZON_H
#define AMAZON_H
/******************************************************************************
       Copyright (c) 2002, Infineon Technologies.  All rights reserved.

                               No Warranty
   Because the program is licensed free of charge, there is no warranty for 
   the program, to the extent permitted by applicable law.  Except when     
   otherwise stated in writing the copyright holders and/or other parties   
   provide the program "as is" without warranty of any kind, either         
   expressed or implied, including, but not limited to, the implied         
   warranties of merchantability and fitness for a particular purpose. The  
   entire risk as to the quality and performance of the program is with     
   you.  should the program prove defective, you assume the cost of all     
   necessary servicing, repair or correction.                               
                                                                            
   In no event unless required by applicable law or agreed to in writing    
   will any copyright holder, or any other party who may modify and/or      
   redistribute the program as permitted above, be liable to you for        
   damages, including any general, special, incidental or consequential     
   damages arising out of the use or inability to use the program           
   (including but not limited to loss of data or data being rendered        
   inaccurate or losses sustained by you or third parties or a failure of   
   the program to operate with any other programs), even if such holder or  
   other party has been advised of the possibility of such damages. 
******************************************************************************/

#define amazon_readl(a)					__raw_readl(((u32*)(a)))
#define amazon_writel(a,b)				__raw_writel(a, ((u32*)(b)))
#define amazon_writel_masked(a,b,c)		__raw_writel((__raw_readl(((u32*)(a))) & ~b) | (c & b), ((u32*)(a)))

unsigned int amazon_get_fpi_hz(void);

#define IOPORT_RESOURCE_START		0x10000000
#define IOPORT_RESOURCE_END		0xffffffff
#define IOMEM_RESOURCE_START		0x10000000
#define IOMEM_RESOURCE_END		0xffffffff

/* check ADSL link status */
#define AMAZON_CHECK_LINK
         
/***********************************************************************/
/*  Module      :  WDT register address and bits                       */
/***********************************************************************/
         
#define AMAZON_WDT                          (KSEG1+0x10100900)
/***********************************************************************/   
    
/***Reset Request Register***/ 
#define AMAZON_RST_REQ                      ((volatile u32*)(AMAZON_WDT+ 0x0010))
#define AMAZON_RST_REQ_PLL                            	(1 << 31)
#define AMAZON_RST_REQ_PCI_CORE                       	(1 << 13) 
#define AMAZON_RST_REQ_TPE                            	(1 << 12)
#define AMAZON_RST_REQ_AFE                            	(1 << 11)
#define AMAZON_RST_REQ_DMA                            	(1 << 9)
#define AMAZON_RST_REQ_SWITCH                         	(1 << 8)
#define AMAZON_RST_REQ_DFE                         	(1 << 7)
#define AMAZON_RST_REQ_PHY                            	(1 << 5)
#define AMAZON_RST_REQ_PCI                            	(1 << 4)
#define AMAZON_RST_REQ_FPI                            	(1 << 2)
#define AMAZON_RST_REQ_CPU                         	(1 << 1)
#define AMAZON_RST_REQ_HRST                           	(1 << 0)
#define AMAZON_RST_ALL				(AMAZON_RST_REQ_PLL	\
						|AMAZON_RST_REQ_PCI_CORE \
						|AMAZON_RST_REQ_TPE	\
						|AMAZON_RST_REQ_AFE	\
						|AMAZON_RST_REQ_DMA	\
						|AMAZON_RST_REQ_SWITCH	\
						|AMAZON_RST_REQ_DFE	\
						|AMAZON_RST_REQ_PHY	\
						|AMAZON_RST_REQ_PCI	\
						|AMAZON_RST_REQ_FPI	\
						|AMAZON_RST_REQ_CPU	\
						|AMAZON_RST_REQ_HRST)
      
/***Reset Status Register Power On***/ 
#define AMAZON_RST_SR                       ((volatile u32*)(AMAZON_WDT+ 0x0014))

/***Watchdog Timer Control Register 0***/ 
#define AMAZON_WDT_CON0                    ((volatile u32*)(AMAZON_WDT+ 0x0020))
      
/***Watchdog Timer Control Register 1***/ 
#define AMAZON_WDT_CON1                    ((volatile u32*)(AMAZON_WDT+ 0x0024))
#define AMAZON_WDT_CON1_WDTDR                          (1 << 3)
#define AMAZON_WDT_CON1_WDTIR                          (1 << 2)
      
/***Watchdog Timer Status Register***/ 
#define AMAZON_WDT_SR                       ((volatile u32*)(AMAZON_WDT+ 0x0028))
#define AMAZON_WDT_SR_WDTTIM(value)             (((( 1 << 16) - 1) & (value)) << 16)
#define AMAZON_WDT_SR_WDTPR                          (1 << 5)
#define AMAZON_WDT_SR_WDTTO                          (1 << 4)
#define AMAZON_WDT_SR_WDTDS                          (1 << 3)
#define AMAZON_WDT_SR_WDTIS                          (1 << 2)
#define AMAZON_WDT_SR_WDTOE                          (1 << 1)
#define AMAZON_WDT_SR_WDTAE                          (1 << 0)		

/***NMI Status Register***/ 
#define AMAZON_WDT_NMISR                        	((volatile u32*)(AMAZON_WDT+ 0x002C))
#define AMAZON_WDT_NMISR_NMIWDT                        (1 << 2)
#define AMAZON_WDT_NMISR_NMIPLL                        (1 << 1)
#define AMAZON_WDT_NMISR_NMIEXT                        (1 << 0)
      
#define AMAZON_WDT_RST_MON				((volatile u32*)(AMAZON_WDT+ 0x0030))

/***********************************************************************/
/*  Module      :  MCD register address and bits                       */
/***********************************************************************/
#define AMAZON_MCD                          		(KSEG1+0x1F106000)

/***Manufacturer Identification Register***/ 
#define AMAZON_MCD_MANID                        	((volatile u32*)(AMAZON_MCD+ 0x0024))
#define AMAZON_MCD_MANID_MANUF(value)              	(((( 1 << 11) - 1) & (value)) << 5)

/***Chip Identification Register***/ 
#define AMAZON_MCD_CHIPID                       	((volatile u32*)(AMAZON_MCD+ 0x0028))
#define AMAZON_MCD_CHIPID_VERSION_GET(value)             (((value) >> 28) & ((1 << 4) - 1))
#define AMAZON_MCD_CHIPID_VERSION_SET(value)             (((( 1 << 4) - 1) & (value)) << 28)
#define AMAZON_MCD_CHIPID_PART_NUMBER_GET(value)         (((value) >> 12) & ((1 << 16) - 1))
#define AMAZON_MCD_CHIPID_PART_NUMBER_SET(value)         (((( 1 << 16) - 1) & (value)) << 12)
#define AMAZON_MCD_CHIPID_MANID_GET(value)               (((value) >> 1) & ((1 << 11) - 1))
#define AMAZON_MCD_CHIPID_MANID_SET(value)               (((( 1 << 11) - 1) & (value)) << 1)

#define AMAZON_CHIPID_STANDARD				0x00EB
#define AMAZON_CHIPID_YANGTSE				0x00ED

/***Redesign Tracing Identification Register***/ 
#define AMAZON_MCD_RTID                         	((volatile u32*)(AMAZON_MCD+ 0x002C))
#define AMAZON_MCD_RTID_LC                              (1 << 15)
#define AMAZON_MCD_RTID_RIX(value)                	(((( 1 << 3) - 1) & (value)) << 0)

	          
/***********************************************************************/
/*  Module      :  CGU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_CGU                          (KSEG1+0x1F103000)
/***********************************************************************/   

/***CGU Clock Divider Select Register***/
#define AMAZON_CGU_DIV							(AMAZON_CGU + 0x0000)			
/***CGU PLL0 Status Register***/ 
#define AMAZON_CGU_PLL0SR						(AMAZON_CGU + 0x0004)
/***CGU PLL1 Status Register***/ 
#define AMAZON_CGU_PLL1SR						(AMAZON_CGU + 0x0008)
/***CGU Interface Clock Control Register***/ 
#define AMAZON_CGU_IFCCR						(AMAZON_CGU + 0x000c)
/***CGU Oscillator Control Register***/ 
#define AMAZON_CGU_OSCCR						(AMAZON_CGU + 0x0010)
/***CGU Memory Clock Delay Register***/ 
#define AMAZON_CGU_MCDEL						(AMAZON_CGU + 0x0014)
/***CGU CPU Clock Reduction Register***/ 
#define AMAZON_CGU_CPUCRD						(AMAZON_CGU + 0x0018)
/***CGU Test Register**/
#define AMAZON_CGU_TST							(AMAZON_CGU + 0x003c)

/***********************************************************************/
/*  Module      :  PMU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_PMU								AMAZON_CGU
/***********************************************************************/   

      
/***PMU Power Down Control Register***/ 
#define AMAZON_PMU_PWDCR                       ((volatile u32*)(AMAZON_PMU+ 0x001c))
#define AMAZON_PMU_PWDCR_TPE                            (1 << 13)
#define AMAZON_PMU_PWDCR_PLL                            (1 << 12)
#define AMAZON_PMU_PWDCR_XTAL                           (1 << 11)
#define AMAZON_PMU_PWDCR_EBU                            (1 << 10)
#define AMAZON_PMU_PWDCR_DFE                            (1 << 9)
#define AMAZON_PMU_PWDCR_SPI                            (1 << 8)
#define AMAZON_PMU_PWDCR_UART                           (1 << 7)
#define AMAZON_PMU_PWDCR_GPT                            (1 << 6)
#define AMAZON_PMU_PWDCR_DMA                            (1 << 5)
#define AMAZON_PMU_PWDCR_PCI                            (1 << 4)
#define AMAZON_PMU_PWDCR_SW                             (1 << 3)
#define AMAZON_PMU_PWDCR_IOR                      	(1 << 2)
#define AMAZON_PMU_PWDCR_FPI                         	(1 << 1)
#define AMAZON_PMU_PWDCR_EPHY                 		(1 << 0)
      
/***PMU Status Register***/ 
#define AMAZON_PMU_SR	                      ((volatile u32*)(AMAZON_PMU+ 0x0020))
#define AMAZON_PMU_SR_TPE                            	(1 << 13)
#define AMAZON_PMU_SR_PLL                            	(1 << 12)
#define AMAZON_PMU_SR_XTAL                           	(1 << 11)
#define AMAZON_PMU_SR_EBU                            	(1 << 10)
#define AMAZON_PMU_SR_DFE                            	(1 << 9)
#define AMAZON_PMU_SR_SPI                            	(1 << 8)
#define AMAZON_PMU_SR_UART                           	(1 << 7)
#define AMAZON_PMU_SR_GPT                            	(1 << 6)
#define AMAZON_PMU_SR_DMA                            	(1 << 5)
#define AMAZON_PMU_SR_PCI                            	(1 << 4)
#define AMAZON_PMU_SR_SW                             	(1 << 3)
#define AMAZON_PMU_SR_IOR                      		(1 << 2)
#define AMAZON_PMU_SR_FPI                         	(1 << 1)
#define AMAZON_PMU_SR_EPHY                 		(1 << 0)
         
/***********************************************************************/
/*  Module      :  BCU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_BCU                          (KSEG1+0x10100000)
/***********************************************************************/   

      
/***BCU Control Register (0010H)***/ 
#define AMAZON_BCU_CON                      ((volatile u32*)(AMAZON_BCU+ 0x0010))
#define AMAZON_BCU_CON_SPC(value)                (((( 1 << 8) - 1) & (value)) << 24)
#define AMAZON_BCU_CON_SPE                              (1 << 19)
#define AMAZON_BCU_CON_PSE                              (1 << 18)
#define AMAZON_BCU_CON_DBG                              (1 << 16)
#define AMAZON_BCU_CON_TOUT(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***BCU Error Control Capture Register (0020H)***/ 
#define AMAZON_BCU_ECON                    ((volatile u32*)(AMAZON_BCU+ 0x0020))
#define AMAZON_BCU_ECON_TAG(value)                (((( 1 << 4) - 1) & (value)) << 24)
#define AMAZON_BCU_ECON_RDN                              (1 << 23)
#define AMAZON_BCU_ECON_WRN                              (1 << 22)
#define AMAZON_BCU_ECON_SVM                              (1 << 21)
#define AMAZON_BCU_ECON_ACK(value)                (((( 1 << 2) - 1) & (value)) << 19)
#define AMAZON_BCU_ECON_ABT                              (1 << 18)
#define AMAZON_BCU_ECON_RDY                              (1 << 17)
#define AMAZON_BCU_ECON_TOUT                            (1 << 16)
#define AMAZON_BCU_ECON_ERRCNT(value)             (((( 1 << 16) - 1) & (value)) << 0)
#define AMAZON_BCU_ECON_OPC(value)                (((( 1 << 4) - 1) & (value)) << 28)
      
/***BCU Error Address Capture Register (0024 H)***/ 
#define AMAZON_BCU_EADD                    ((volatile u32*)(AMAZON_BCU+ 0x0024))
#define AMAZON_BCU_EADD_FPIADR
      
/***BCU Error Data Capture Register (0028H)***/ 
#define AMAZON_BCU_EDAT                    ((volatile u32*)(AMAZON_BCU+ 0x0028))
#define AMAZON_BCU_EDAT_FPIDAT		
         
/***********************************************************************/
/*  Module      :  Switch register address and bits                    */
/***********************************************************************/
         
#define AMAZON_SWITCH                       (KSEG1+0x10106000)
/***********************************************************************/   
#define AMAZON_SW_UN_DEST                 	AMAZON_SWITCH+0x00 /*Unknown destination register*/
#define AMAZON_SW_VLAN_CTRL              	AMAZON_SWITCH+0x04 /*VLAN control register*/
#define AMAZON_SW_PS_CTL                  	AMAZON_SWITCH+0x08 /*port status control register*/
#define AMAZON_SW_COS_CTL                 	AMAZON_SWITCH+0x0c /*Cos control register*/
#define	AMAZON_SW_VLAN_COS                	AMAZON_SWITCH+0x10 /*VLAN priority cos mapping register*/
#define AMAZON_SW_DSCP_COS3               	AMAZON_SWITCH+0x14 /*DSCP cos mapping register3*/
#define AMAZON_SW_DSCP_COS2               	AMAZON_SWITCH+0x18 /*DSCP cos mapping register2*/
#define AMAZON_SW_DSCP_COS1               	AMAZON_SWITCH+0x1c /*DSCP cos mapping register1*/
#define AMAZON_SW_DSCP_COS0               	AMAZON_SWITCH+0x20 /*DSCP cos mapping register*/
#define AMAZON_SW_ARL_CTL                 	AMAZON_SWITCH+0x24 /*ARL control register*/
#define AMAZON_SW_PKT_LEN                 	AMAZON_SWITCH+0x28 /*packet length register*/
#define AMAZON_SW_CPU_ACTL                	AMAZON_SWITCH+0x2c /*CPU control register1*/
#define AMAZON_SW_DATA1                   	AMAZON_SWITCH+0x30 /*CPU access control register1*/
#define AMAZON_SW_DATA2                   	AMAZON_SWITCH+0x34 /*CPU access control register2*/
#define AMAZON_SW_P2_PCTL                 	AMAZON_SWITCH+0x38 /*Port2 control register*/
#define AMAZON_SW_P0_TX_CTL               	AMAZON_SWITCH+0x3c /*port0 TX control register*/
#define AMAZON_SW_P1_TX_CTL               	AMAZON_SWITCH+0x40 /*port 1 TX control register*/
#define AMAZON_SW_P0_WM                   	AMAZON_SWITCH+0x44 /*port 0 watermark control register*/
#define AMAZON_SW_P1_WM                   	AMAZON_SWITCH+0x48 /*port 1 watermark control register*/
#define AMAZON_SW_P2_WM                   	AMAZON_SWITCH+0x4c /*port 2 watermark control register*/
#define AMAZON_SW_GBL_WM                  	AMAZON_SWITCH+0x50 /*Global watermark register*/
#define AMAZON_SW_PM_CTL                  	AMAZON_SWITCH+0x54 /*PM control register*/
#define AMAZON_SW_P2_CTL                  	AMAZON_SWITCH+0x58 /*PMAC control register*/
#define AMAZON_SW_P2_TX_IPG               	AMAZON_SWITCH+0x5c /*port2 TX IPG control register*/
#define AMAZON_SW_P2_RX_IPG               	AMAZON_SWITCH+0x60 /*prot2 RX IPG control register*/
#define AMAZON_SW_MDIO_ACC                	AMAZON_SWITCH+0x64 /*MDIO access register*/
#define AMAZON_SW_EPHY                    	AMAZON_SWITCH+0x68 /*Ethernet PHY register*/
#define AMAZON_SW_MDIO_CFG                	AMAZON_SWITCH+0x6c /*MDIO configuration register*/
#define AMAZON_SW_P0_RCV_DROP_CNT         	AMAZON_SWITCH+0x70 /*port0 receive drop counter */
#define AMAZON_SW_P0_RCV_FRAME_ERR_CNT    	AMAZON_SWITCH+0x74 /*port0 receive frame error conter*/
#define AMAZON_SW_P0_TX_COLL_CNT          	AMAZON_SWITCH+0x78 /*port0 transmit collision counter*/ 
#define AMAZON_SW_P0_TX_DROP_CNT          	AMAZON_SWITCH+0x7c /*port1 transmit drop counter*/
#define AMAZON_SW_P1_RCV_DROP_CNT         	AMAZON_SWITCH+0x80 /*port1 receive drop counter*/
#define AMAZON_SW_P1_RCV_FRAME_ERR_CNT   	AMAZON_SWITCH+0x84 /*port1 receive error counter*/
#define AMAZON_SW_P1_TX_COLL_CNT          	AMAZON_SWITCH+0x88 /*port1 transmit collision counter*/
#define AMAZON_SW_P1_TX_DROP_CNT          	AMAZON_SWITCH+0x8c /*port1 transmit drop counter*/


         
/***********************************************************************/
/*  Module      :  SSC register address and bits                      */
/***********************************************************************/
#define AMAZON_SSC_BASE_ADD_0      		(KSEG1+0x10100800)         

/*165001:henryhsu:20050603:Source add by Bing Tao*/

/*configuration/Status Registers in Bus Clock Domain*/
#define AMAZON_SSC_CLC                          ((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0000))
#define AMAZON_SSC_ID                           ((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0008))
#define AMAZON_SSC_CON                          ((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0010))
#define AMAZON_SSC_STATE			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0014))
#define AMAZON_SSC_WHBSTATE			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0018))
#define AMAZON_SSC_TB				((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0020))
#define AMAZON_SSC_RB				((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0024))
#define AMAZON_SSC_FSTAT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0038))	

/*Configuration/Status Registers in Kernel Clock Domain*/
#define AMAZON_SSC_PISEL			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0004))
#define AMAZON_SSC_RXFCON			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0030))
#define AMAZON_SSC_TXFCON			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0034))
#define AMAZON_SSC_BR				((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0040))
#define AMAZON_SSC_BRSTAT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0044))
#define AMAZON_SSC_SFCON			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0060))
#define AMAZON_SSC_SFSTAT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0064))
#define AMAZON_SSC_GPOCON			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0070))
#define AMAZON_SSC_GPOSTAT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0074))
#define AMAZON_SSC_WHBGPOSTAT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0078))
#define AMAZON_SSC_RXREQ			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0080))
#define AMAZON_SSC_RXCNT			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x0084))

/*DMA Registers in Bus Clock Domain*/
#define AMAZON_SSC_DMA_CON			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x00ec))

/*interrupt Node Registers in Bus Clock Domain*/
#define AMAZON_SSC_IRNEN			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x00F4))
#define AMAZON_SSC_IRNICR			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x00FC))
#define AMAZON_SSC_IRNCR			((volatile u32*)(AMAZON_SSC_BASE_ADD_0+0x00F8))

/*165001*/

/***********************************************************************/   

      
         
/***********************************************************************/
/*  Module      :  EBU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_EBU                          (KSEG1+0x10105300)
/***********************************************************************/   

      
/***EBU Clock Control Register***/ 
#define AMAZON_EBU_CLC                      ((volatile u32*)(AMAZON_EBU+ 0x0000))
#define AMAZON_EBU_CLC_DISS                            (1 << 1)
#define AMAZON_EBU_CLC_DISR                            (1 << 0)
      
/***EBU Global Control Register***/ 
#define AMAZON_EBU_CON                      ((volatile u32*)(AMAZON_EBU+ 0x0010))
#define AMAZON_EBU_CON_DTACS(value)              (((( 1 << 3) - 1) & (value)) << 20)
#define AMAZON_EBU_CON_DTARW(value)              (((( 1 << 3) - 1) & (value)) << 16)
#define AMAZON_EBU_CON_TOUTC(value)              (((( 1 << 8) - 1) & (value)) << 8)
#define AMAZON_EBU_CON_ARBMODE(value)            (((( 1 << 2) - 1) & (value)) << 6)
#define AMAZON_EBU_CON_ARBSYNC                      (1 << 5)
#define AMAZON_EBU_CON_1                              (1 << 3)
      
/***EBU Address Select Register 0***/ 
#define AMAZON_EBU_ADDSEL0                  ((volatile u32*)(AMAZON_EBU+ 0x0020))
#define AMAZON_EBU_ADDSEL0_BASE(value)               (((( 1 << 20) - 1) & (value)) << 12)
#define AMAZON_EBU_ADDSEL0_MASK(value)               (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_EBU_ADDSEL0_MIRRORE                      (1 << 1)
#define AMAZON_EBU_ADDSEL0_REGEN                          (1 << 0)
      
/***EBU Address Select Register 1***/ 
#define AMAZON_EBU_ADDSEL1                  ((volatile u32*)(AMAZON_EBU+ 0x0024))
#define AMAZON_EBU_ADDSEL1_BASE(value)               (((( 1 << 20) - 1) & (value)) << 12)
#define AMAZON_EBU_ADDSEL1_MASK(value)               (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_EBU_ADDSEL1_MIRRORE                      (1 << 1)
#define AMAZON_EBU_ADDSEL1_REGEN                          (1 << 0)
      
/***EBU Address Select Register 2***/ 
#define AMAZON_EBU_ADDSEL2                  ((volatile u32*)(AMAZON_EBU+ 0x0028))
#define AMAZON_EBU_ADDSEL2_BASE(value)               (((( 1 << 20) - 1) & (value)) << 12)
#define AMAZON_EBU_ADDSEL2_MASK(value)               (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_EBU_ADDSEL2_MIRRORE                      (1 << 1)
#define AMAZON_EBU_ADDSEL2_REGEN                          (1 << 0)
      
/***EBU Bus Configuration Register 0***/ 
#define AMAZON_EBU_BUSCON0                  ((volatile u32*)(AMAZON_EBU+ 0x0060))
#define AMAZON_EBU_BUSCON0_WRDIS                          (1 << 31)
#define AMAZON_EBU_BUSCON0_ALEC(value)               (((( 1 << 2) - 1) & (value)) << 29)
#define AMAZON_EBU_BUSCON0_BCGEN(value)              (((( 1 << 2) - 1) & (value)) << 27)
#define AMAZON_EBU_BUSCON0_AGEN(value)               (((( 1 << 2) - 1) & (value)) << 24)
#define AMAZON_EBU_BUSCON0_CMULTR(value)             (((( 1 << 2) - 1) & (value)) << 22)
#define AMAZON_EBU_BUSCON0_WAIT(value)               (((( 1 << 2) - 1) & (value)) << 20)
#define AMAZON_EBU_BUSCON0_WAITINV                      (1 << 19)
#define AMAZON_EBU_BUSCON0_SETUP                          (1 << 18)
#define AMAZON_EBU_BUSCON0_PORTW(value)              (((( 1 << 2) - 1) & (value)) << 16)
#define AMAZON_EBU_BUSCON0_WAITRDC(value)            (((( 1 << 7) - 1) & (value)) << 9)
#define AMAZON_EBU_BUSCON0_WAITWRC(value)            (((( 1 << 3) - 1) & (value)) << 6)
#define AMAZON_EBU_BUSCON0_HOLDC(value)              (((( 1 << 2) - 1) & (value)) << 4)
#define AMAZON_EBU_BUSCON0_RECOVC(value)             (((( 1 << 2) - 1) & (value)) << 2)
#define AMAZON_EBU_BUSCON0_CMULT(value)              (((( 1 << 2) - 1) & (value)) << 0)
      
/***EBU Bus Configuration Register 1***/ 
#define AMAZON_EBU_BUSCON1                  ((volatile u32*)(AMAZON_EBU+ 0x0064))
#define AMAZON_EBU_BUSCON1_WRDIS                          (1 << 31)
#define AMAZON_EBU_BUSCON1_ALEC(value)               (((( 1 << 2) - 1) & (value)) << 29)
#define AMAZON_EBU_BUSCON1_BCGEN(value)              (((( 1 << 2) - 1) & (value)) << 27)
#define AMAZON_EBU_BUSCON1_AGEN(value)               (((( 1 << 2) - 1) & (value)) << 24)
#define AMAZON_EBU_BUSCON1_CMULTR(value)             (((( 1 << 2) - 1) & (value)) << 22)
#define AMAZON_EBU_BUSCON1_WAIT(value)               (((( 1 << 2) - 1) & (value)) << 20)
#define AMAZON_EBU_BUSCON1_WAITINV                      (1 << 19)
#define AMAZON_EBU_BUSCON1_SETUP                          (1 << 18)
#define AMAZON_EBU_BUSCON1_PORTW(value)              (((( 1 << 2) - 1) & (value)) << 16)
#define AMAZON_EBU_BUSCON1_WAITRDC(value)            (((( 1 << 7) - 1) & (value)) << 9)
#define AMAZON_EBU_BUSCON1_WAITWRC(value)            (((( 1 << 3) - 1) & (value)) << 6)
#define AMAZON_EBU_BUSCON1_HOLDC(value)              (((( 1 << 2) - 1) & (value)) << 4)
#define AMAZON_EBU_BUSCON1_RECOVC(value)             (((( 1 << 2) - 1) & (value)) << 2)
#define AMAZON_EBU_BUSCON1_CMULT(value)              (((( 1 << 2) - 1) & (value)) << 0)
      
/***EBU Bus Configuration Register 2***/ 
#define AMAZON_EBU_BUSCON2                  ((volatile u32*)(AMAZON_EBU+ 0x0068))
#define AMAZON_EBU_BUSCON2_WRDIS                          (1 << 31)
#define AMAZON_EBU_BUSCON2_ALEC(value)               (((( 1 << 2) - 1) & (value)) << 29)
#define AMAZON_EBU_BUSCON2_BCGEN(value)              (((( 1 << 2) - 1) & (value)) << 27)
#define AMAZON_EBU_BUSCON2_AGEN(value)               (((( 1 << 2) - 1) & (value)) << 24)
#define AMAZON_EBU_BUSCON2_CMULTR(value)             (((( 1 << 2) - 1) & (value)) << 22)
#define AMAZON_EBU_BUSCON2_WAIT(value)               (((( 1 << 2) - 1) & (value)) << 20)
#define AMAZON_EBU_BUSCON2_WAITINV                      (1 << 19)
#define AMAZON_EBU_BUSCON2_SETUP                          (1 << 18)
#define AMAZON_EBU_BUSCON2_PORTW(value)              (((( 1 << 2) - 1) & (value)) << 16)
#define AMAZON_EBU_BUSCON2_WAITRDC(value)            (((( 1 << 7) - 1) & (value)) << 9)
#define AMAZON_EBU_BUSCON2_WAITWRC(value)            (((( 1 << 3) - 1) & (value)) << 6)
#define AMAZON_EBU_BUSCON2_HOLDC(value)              (((( 1 << 2) - 1) & (value)) << 4)
#define AMAZON_EBU_BUSCON2_RECOVC(value)             (((( 1 << 2) - 1) & (value)) << 2)
#define AMAZON_EBU_BUSCON2_CMULT(value)              (((( 1 << 2) - 1) & (value)) << 0)		
         
/***********************************************************************/
/*  Module      :  SDRAM register address and bits                     */
/***********************************************************************/
         
#define AMAZON_SDRAM                        (KSEG1+0x1F800000)
/***********************************************************************/   

      
/***MC Access Error Cause Register***/ 
#define AMAZON_SDRAM_MC_ERRCAUSE                  ((volatile u32*)(AMAZON_SDRAM+ 0x0010))
#define AMAZON_SDRAM_MC_ERRCAUSE_ERR                              (1 << 31)
#define AMAZON_SDRAM_MC_ERRCAUSE_PORT(value)               (((( 1 << 4) - 1) & (value)) << 16)
#define AMAZON_SDRAM_MC_ERRCAUSE_CAUSE(value)              (((( 1 << 2) - 1) & (value)) << 0)
#define AMAZON_SDRAM_MC_ERRCAUSE_Res(value)                (((( 1 << NaN) - 1) & (value)) << NaN)
      
/***MC Access Error Address Register***/ 
#define AMAZON_SDRAM_MC_ERRADDR                   ((volatile u32*)(AMAZON_SDRAM+ 0x0020))
#define AMAZON_SDRAM_MC_ERRADDR_ADDR
      
/***MC I/O General Purpose Register***/ 
#define AMAZON_SDRAM_MC_IOGP                      ((volatile u32*)(AMAZON_SDRAM+ 0x0100))
#define AMAZON_SDRAM_MC_IOGP_GPR6(value)               (((( 1 << 4) - 1) & (value)) << 28)
#define AMAZON_SDRAM_MC_IOGP_GPR5(value)               (((( 1 << 4) - 1) & (value)) << 24)
#define AMAZON_SDRAM_MC_IOGP_GPR4(value)               (((( 1 << 4) - 1) & (value)) << 20)
#define AMAZON_SDRAM_MC_IOGP_GPR3(value)               (((( 1 << 4) - 1) & (value)) << 16)
#define AMAZON_SDRAM_MC_IOGP_GPR2(value)               (((( 1 << 4) - 1) & (value)) << 12)
#define AMAZON_SDRAM_MC_IOGP_CPS                              (1 << 11)
#define AMAZON_SDRAM_MC_IOGP_CLKDELAY(value)          (((( 1 << 3) - 1) & (value)) << 8)
#define AMAZON_SDRAM_MC_IOGP_CLKRAT(value)             (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_SDRAM_MC_IOGP_RDDEL(value)              (((( 1 << 4) - 1) & (value)) << 0)
      
/***MC Self Refresh Register***/ 
#define AMAZON_SDRAM_MC_SELFRFSH                  ((volatile u32*)(AMAZON_SDRAM+ 0x01A0))
#define AMAZON_SDRAM_MC_SELFRFSH_PWDS                            (1 << 1)
#define AMAZON_SDRAM_MC_SELFRFSH_PWD                              (1 << 0)
#define AMAZON_SDRAM_MC_SELFRFSH_Res(value)                (((( 1 << 30) - 1) & (value)) << 2)
      
/***MC Enable Register***/ 
#define AMAZON_SDRAM_MC_CTRLENA                   ((volatile u32*)(AMAZON_SDRAM+ 0x0110))
#define AMAZON_SDRAM_MC_CTRLENA_ENA                              (1 << 0)
#define AMAZON_SDRAM_MC_CTRLENA_Res(value)                (((( 1 << 31) - 1) & (value)) << 1)
      
/***MC Mode Register Setup Code***/ 
#define AMAZON_SDRAM_MC_MRSCODE                   ((volatile u32*)(AMAZON_SDRAM+ 0x0120))
#define AMAZON_SDRAM_MC_MRSCODE_UMC(value)                (((( 1 << 5) - 1) & (value)) << 7)
#define AMAZON_SDRAM_MC_MRSCODE_CL(value)                (((( 1 << 3) - 1) & (value)) << 4)
#define AMAZON_SDRAM_MC_MRSCODE_WT                              (1 << 3)
#define AMAZON_SDRAM_MC_MRSCODE_BL(value)                (((( 1 << 3) - 1) & (value)) << 0)
      
/***MC Configuration Data-word Width Register***/ 
#define AMAZON_SDRAM_MC_CFGDW                    ((volatile u32*)(AMAZON_SDRAM+ 0x0130))
#define AMAZON_SDRAM_MC_CFGDW_DW(value)                (((( 1 << 4) - 1) & (value)) << 0)
#define AMAZON_SDRAM_MC_CFGDW_Res(value)                (((( 1 << 28) - 1) & (value)) << 4)
      
/***MC Configuration Physical Bank 0 Register***/ 
#define AMAZON_SDRAM_MC_CFGPB0                    ((volatile u32*)(AMAZON_SDRAM+ 0x140))
#define AMAZON_SDRAM_MC_CFGPB0_MCSEN0(value)             (((( 1 << 4) - 1) & (value)) << 12)
#define AMAZON_SDRAM_MC_CFGPB0_BANKN0(value)             (((( 1 << 4) - 1) & (value)) << 8)
#define AMAZON_SDRAM_MC_CFGPB0_ROWW0(value)              (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_SDRAM_MC_CFGPB0_COLW0(value)              (((( 1 << 4) - 1) & (value)) << 0)
#define AMAZON_SDRAM_MC_CFGPB0_Res(value)                (((( 1 << 16) - 1) & (value)) << 16)
      
/***MC Latency Register***/ 
#define AMAZON_SDRAM_MC_LATENCY                   ((volatile u32*)(AMAZON_SDRAM+ 0x0180))
#define AMAZON_SDRAM_MC_LATENCY_TRP(value)                (((( 1 << 4) - 1) & (value)) << 16)
#define AMAZON_SDRAM_MC_LATENCY_TRAS(value)               (((( 1 << 4) - 1) & (value)) << 12)
#define AMAZON_SDRAM_MC_LATENCY_TRCD(value)               (((( 1 << 4) - 1) & (value)) << 8)
#define AMAZON_SDRAM_MC_LATENCY_TDPL(value)               (((( 1 << 4) - 1) & (value)) << 4)
#define AMAZON_SDRAM_MC_LATENCY_TDAL(value)               (((( 1 << 4) - 1) & (value)) << 0)
#define AMAZON_SDRAM_MC_LATENCY_Res(value)                (((( 1 << 12) - 1) & (value)) << 20)
      
/***MC Refresh Cycle Time Register***/ 
#define AMAZON_SDRAM_MC_TREFRESH                  ((volatile u32*)(AMAZON_SDRAM+ 0x0190))
#define AMAZON_SDRAM_MC_TREFRESH_TREF(value)               (((( 1 << 13) - 1) & (value)) << 0)
#define AMAZON_SDRAM_MC_TREFRESH_Res(value)                (((( 1 << 19) - 1) & (value)) << 13)		
         
/***********************************************************************/
/*  Module      :  GPTU register address and bits                      */
/***********************************************************************/
         
#define AMAZON_GPTU                         (KSEG1+0x10100A00)
/***********************************************************************/   

      
/***GPT Clock Control Register***/ 
#define AMAZON_GPTU_CLC                      ((volatile u32*)(AMAZON_GPTU+ 0x0000))
#define AMAZON_GPTU_CLC_RMC(value)                (((( 1 << 8) - 1) & (value)) << 8)
#define AMAZON_GPTU_CLC_DISS                            (1 << 1)
#define AMAZON_GPTU_CLC_DISR                            (1 << 0)
      
/***GPT Timer 3 Control Register***/ 
#define AMAZON_GPTU_T3CON                    ((volatile u32*)(AMAZON_GPTU+ 0x0014))
#define AMAZON_GPTU_T3CON_T3RDIR                        (1 << 15)
#define AMAZON_GPTU_T3CON_T3CHDIR                      (1 << 14)
#define AMAZON_GPTU_T3CON_T3EDGE                        (1 << 13)
#define AMAZON_GPTU_T3CON_BPS1(value)               (((( 1 << 2) - 1) & (value)) << 11)
#define AMAZON_GPTU_T3CON_T3OTL                          (1 << 10)
#define AMAZON_GPTU_T3CON_T3UD                            (1 << 7)
#define AMAZON_GPTU_T3CON_T3R                              (1 << 6)
#define AMAZON_GPTU_T3CON_T3M(value)                (((( 1 << 3) - 1) & (value)) << 3)
#define AMAZON_GPTU_T3CON_T3I(value)                (((( 1 << 3) - 1) & (value)) << 0)
      
/***GPT Write Hardware Modified Timer 3 Control Register 
If set and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define AMAZON_GPTU_WHBT3CON                 ((volatile u32*)(AMAZON_GPTU+ 0x004C))
#define AMAZON_GPTU_WHBT3CON_SETT3CHDIR                (1 << 15)
#define AMAZON_GPTU_WHBT3CON_CLRT3CHDIR                (1 << 14)
#define AMAZON_GPTU_WHBT3CON_SETT3EDGE                  (1 << 13)
#define AMAZON_GPTU_WHBT3CON_CLRT3EDGE                  (1 << 12)
#define AMAZON_GPTU_WHBT3CON_SETT3OTL                  (1 << 11)
#define AMAZON_GPTU_WHBT3CON_CLRT3OTL                  (1 << 10)
      
/***GPT Timer 2 Control Register***/ 
#define AMAZON_GPTU_T2CON                    ((volatile u32*)(AMAZON_GPTU+ 0x0010))
#define AMAZON_GPTU_T2CON_TxRDIR                        (1 << 15)
#define AMAZON_GPTU_T2CON_TxCHDIR                      (1 << 14)
#define AMAZON_GPTU_T2CON_TxEDGE                        (1 << 13)
#define AMAZON_GPTU_T2CON_TxIRDIS                      (1 << 12)
#define AMAZON_GPTU_T2CON_TxRC                            (1 << 9)
#define AMAZON_GPTU_T2CON_TxUD                            (1 << 7)
#define AMAZON_GPTU_T2CON_TxR                              (1 << 6)
#define AMAZON_GPTU_T2CON_TxM(value)                (((( 1 << 3) - 1) & (value)) << 3)
#define AMAZON_GPTU_T2CON_TxI(value)                (((( 1 << 3) - 1) & (value)) << 0)
      
/***GPT Timer 4 Control Register***/ 
#define AMAZON_GPTU_T4CON                    ((volatile u32*)(AMAZON_GPTU+ 0x0018))
#define AMAZON_GPTU_T4CON_TxRDIR                        (1 << 15)
#define AMAZON_GPTU_T4CON_TxCHDIR                      (1 << 14)
#define AMAZON_GPTU_T4CON_TxEDGE                        (1 << 13)
#define AMAZON_GPTU_T4CON_TxIRDIS                      (1 << 12)
#define AMAZON_GPTU_T4CON_TxRC                            (1 << 9)
#define AMAZON_GPTU_T4CON_TxUD                            (1 << 7)
#define AMAZON_GPTU_T4CON_TxR                              (1 << 6)
#define AMAZON_GPTU_T4CON_TxM(value)                (((( 1 << 3) - 1) & (value)) << 3)
#define AMAZON_GPTU_T4CON_TxI(value)                (((( 1 << 3) - 1) & (value)) << 0)
      
/***GPT Write HW Modified Timer 2 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define AMAZON_GPTU_WHBT2CON                 ((volatile u32*)(AMAZON_GPTU+ 0x0048))
#define AMAZON_GPTU_WHBT2CON_SETTxCHDIR                (1 << 15)
#define AMAZON_GPTU_WHBT2CON_CLRTxCHDIR                (1 << 14)
#define AMAZON_GPTU_WHBT2CON_SETTxEDGE                  (1 << 13)
#define AMAZON_GPTU_WHBT2CON_CLRTxEDGE                  (1 << 12)
      
/***GPT Write HW Modified Timer 4 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define AMAZON_GPTU_WHBT4CON                 ((volatile u32*)(AMAZON_GPTU+ 0x0050))
#define AMAZON_GPTU_WHBT4CON_SETTxCHDIR                (1 << 15)
#define AMAZON_GPTU_WHBT4CON_CLRTxCHDIR                (1 << 14)
#define AMAZON_GPTU_WHBT4CON_SETTxEDGE                  (1 << 13)
#define AMAZON_GPTU_WHBT4CON_CLRTxEDGE                  (1 << 12)
      
/***GPT Capture Reload Register***/ 
#define AMAZON_GPTU_CAPREL                   ((volatile u32*)(AMAZON_GPTU+ 0x0030))
#define AMAZON_GPTU_CAPREL_CAPREL(value)             (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 2 Register***/ 
#define AMAZON_GPTU_T2                       ((volatile u32*)(AMAZON_GPTU+ 0x0034))
#define AMAZON_GPTU_T2_TVAL(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 3 Register***/ 
#define AMAZON_GPTU_T3                       ((volatile u32*)(AMAZON_GPTU+ 0x0038))
#define AMAZON_GPTU_T3_TVAL(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 4 Register***/ 
#define AMAZON_GPTU_T4                       ((volatile u32*)(AMAZON_GPTU+ 0x003C))
#define AMAZON_GPTU_T4_TVAL(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 5 Register***/ 
#define AMAZON_GPTU_T5                       ((volatile u32*)(AMAZON_GPTU+ 0x0040))
#define AMAZON_GPTU_T5_TVAL(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 6 Register***/ 
#define AMAZON_GPTU_T6                       ((volatile u32*)(AMAZON_GPTU+ 0x0044))
#define AMAZON_GPTU_T6_TVAL(value)               (((( 1 << 16) - 1) & (value)) << 0)
      
/***GPT Timer 6 Control Register***/ 
#define AMAZON_GPTU_T6CON                    ((volatile u32*)(AMAZON_GPTU+ 0x0020))
#define AMAZON_GPTU_T6CON_T6SR                            (1 << 15)
#define AMAZON_GPTU_T6CON_T6CLR                          (1 << 14)
#define AMAZON_GPTU_T6CON_BPS2(value)               (((( 1 << 2) - 1) & (value)) << 11)
#define AMAZON_GPTU_T6CON_T6OTL                          (1 << 10)
#define AMAZON_GPTU_T6CON_T6UD                            (1 << 7)
#define AMAZON_GPTU_T6CON_T6R                              (1 << 6)
#define AMAZON_GPTU_T6CON_T6M(value)                (((( 1 << 3) - 1) & (value)) << 3)
#define AMAZON_GPTU_T6CON_T6I(value)                (((( 1 << 3) - 1) & (value)) << 0)
      
/***GPT Write HW Modified Timer 6 Control Register If set
 and clear bit are written concurrently with 1, the associated bit is not changed.***/
#define AMAZON_GPTU_WHBT6CON                 ((volatile u32*)(AMAZON_GPTU+ 0x0054))
#define AMAZON_GPTU_WHBT6CON_SETT6OTL                  (1 << 11)
#define AMAZON_GPTU_WHBT6CON_CLRT6OTL                  (1 << 10)
      
/***GPT Timer 5 Control Register***/ 
#define AMAZON_GPTU_T5CON                    ((volatile u32*)(AMAZON_GPTU+ 0x001C))
#define AMAZON_GPTU_T5CON_T5SC                            (1 << 15)
#define AMAZON_GPTU_T5CON_T5CLR                          (1 << 14)
#define AMAZON_GPTU_T5CON_CI(value)                (((( 1 << 2) - 1) & (value)) << 12)
#define AMAZON_GPTU_T5CON_T5CC                            (1 << 11)
#define AMAZON_GPTU_T5CON_CT3                              (1 << 10)
#define AMAZON_GPTU_T5CON_T5RC                            (1 << 9)
#define AMAZON_GPTU_T5CON_T5UDE                          (1 << 8)
#define AMAZON_GPTU_T5CON_T5UD                            (1 << 7)
#define AMAZON_GPTU_T5CON_T5R                              (1 << 6)
#define AMAZON_GPTU_T5CON_T5M(value)                (((( 1 << 3) - 1) & (value)) << 3)
#define AMAZON_GPTU_T5CON_T5I(value)                (((( 1 << 3) - 1) & (value)) << 0)		
         

/***********************************************************************/
/*  Module      :  ASC register address and bits                       */
/***********************************************************************/
         
#define AMAZON_ASC                          (KSEG1+0x10100400)
/***********************************************************************/   

      
/***ASC Port Input Select Register***/ 
#define AMAZON_ASC_PISEL					(AMAZON_ASC+ 0x0004)
#define AMAZON_ASC_PISEL_RIS				(1 << 0)
      
/***ASC Control Register***/ 
#define AMAZON_ASC_CON						(AMAZON_ASC+ 0x0010)
#define AMAZON_ASC_CON_R					(1 << 15)
#define AMAZON_ASC_CON_LB					(1 << 14)
#define AMAZON_ASC_CON_BRS					(1 << 13)
#define AMAZON_ASC_CON_ODD					(1 << 12)
#define AMAZON_ASC_CON_FDE					(1 << 11)
#define AMAZON_ASC_CON_OE					(1 << 10)
#define AMAZON_ASC_CON_FE					(1 << 9)
#define AMAZON_ASC_CON_PE					(1 << 8)
#define AMAZON_ASC_CON_OEN					(1 << 7)
#define AMAZON_ASC_CON_FEN					(1 << 6)
#define AMAZON_ASC_CON_PENRXDI				(1 << 5)
#define AMAZON_ASC_CON_REN					(1 << 4)
#define AMAZON_ASC_CON_STP					(1 << 3)
#define AMAZON_ASC_CON_M(value)				(((( 1 << 3) - 1) & (value)) << 0)
 
/***ASC Write Hardware Modified Control Register***/ 
#define AMAZON_ASC_WHBCON					(AMAZON_ASC+ 0x0050)
#define AMAZON_ASC_WHBCON_SETOE				(1 << 13)
#define AMAZON_ASC_WHBCON_SETFE				(1 << 12)
#define AMAZON_ASC_WHBCON_SETPE				(1 << 11)
#define AMAZON_ASC_WHBCON_CLROE				(1 << 10)
#define AMAZON_ASC_WHBCON_CLRFE				(1 << 9)
#define AMAZON_ASC_WHBCON_CLRPE				(1 << 8)
#define AMAZON_ASC_WHBCON_SETREN			(1 << 5)
#define AMAZON_ASC_WHBCON_CLRREN			(1 << 4)
      
/***ASC Baudrate Timer/Reload Register***/ 
#define AMAZON_ASC_BTR						(AMAZON_ASC+ 0x0014)
#define AMAZON_ASC_BTR_BR_VALUE(value)		(((( 1 << 13) - 1) & (value)) << 0)
      
/***ASC Fractional Divider Register***/ 
#define AMAZON_ASC_FDV						(AMAZON_ASC+ 0x0018)
#define AMAZON_ASC_FDV_FD_VALUE(value)		(((( 1 << 9) - 1) & (value)) << 0)
      
/***ASC IrDA Pulse Mode/Width Register***/ 
#define AMAZON_ASC_PMW						(AMAZON_ASC+ 0x001C)
#define AMAZON_ASC_PMW_IRPW					(1 << 8)
#define AMAZON_ASC_PMW_PW_VALUE(value)		(((( 1 << 8) - 1) & (value)) << 0)
      
/***ASC Transmit Buffer Register***/ 
#define AMAZON_ASC_TBUF						(AMAZON_ASC+ 0x0020)
#define AMAZON_ASC_TBUF_TD_VALUE(value)		(((( 1 << 9) - 1) & (value)) << 0)
      
/***ASC Receive Buffer Register***/ 
#define AMAZON_ASC_RBUF						(AMAZON_ASC+ 0x0024)
#define AMAZON_ASC_RBUF_RD_VALUE(value)		(((( 1 << 9) - 1) & (value)) << 0)
      
/***ASC Autobaud Control Register***/ 
#define AMAZON_ASC_ABCON					(AMAZON_ASC+ 0x0030)
#define AMAZON_ASC_ABCON_RXINV				(1 << 11)
#define AMAZON_ASC_ABCON_TXINV				(1 << 10)
#define AMAZON_ASC_ABCON_ABEM(value)		(((( 1 << 2) - 1) & (value)) << 8)
#define AMAZON_ASC_ABCON_FCDETEN			(1 << 4)
#define AMAZON_ASC_ABCON_ABDETEN			(1 << 3)
#define AMAZON_ASC_ABCON_ABSTEN				(1 << 2)
#define AMAZON_ASC_ABCON_AUREN				(1 << 1)
#define AMAZON_ASC_ABCON_ABEN				(1 << 0)
      
/***Receive FIFO Control Register***/ 
#define AMAZON_ASC_RXFCON					(AMAZON_ASC+ 0x0040)
#define AMAZON_ASC_RXFCON_RXFITL(value)		(((( 1 << 6) - 1) & (value)) << 8)
#define AMAZON_ASC_RXFCON_RXTMEN			(1 << 2)
#define AMAZON_ASC_RXFCON_RXFFLU			(1 << 1)
#define AMAZON_ASC_RXFCON_RXFEN				(1 << 0)
      
/***Transmit FIFO Control Register***/ 
#define AMAZON_ASC_TXFCON					(AMAZON_ASC+ 0x0044)
#define AMAZON_ASC_TXFCON_TXFITL(value)		(((( 1 << 6) - 1) & (value)) << 8)
#define AMAZON_ASC_TXFCON_TXTMEN			(1 << 2)
#define AMAZON_ASC_TXFCON_TXFFLU			(1 << 1)
#define AMAZON_ASC_TXFCON_TXFEN				(1 << 0)
      
/***FIFO Status Register***/ 
#define AMAZON_ASC_FSTAT					(AMAZON_ASC+ 0x0048)
#define AMAZON_ASC_FSTAT_TXFFL(value)		(((( 1 << 6) - 1) & (value)) << 8)
#define AMAZON_ASC_FSTAT_RXFFL(value)		(((( 1 << 6) - 1) & (value)) << 0)
      
/***ASC Write HW Modified Autobaud Control Register***/ 
#define AMAZON_ASC_WHBABCON					(AMAZON_ASC+ 0x0054)
#define AMAZON_ASC_WHBABCON_SETABEN			(1 << 1)
#define AMAZON_ASC_WHBABCON_CLRABEN			(1 << 0)
      
/***ASC Autobaud Status Register***/ 
#define AMAZON_ASC_ABSTAT					(AMAZON_ASC+ 0x0034)
#define AMAZON_ASC_ABSTAT_DETWAIT			(1 << 4)
#define AMAZON_ASC_ABSTAT_SCCDET			(1 << 3)
#define AMAZON_ASC_ABSTAT_SCSDET			(1 << 2)
#define AMAZON_ASC_ABSTAT_FCCDET			(1 << 1)
#define AMAZON_ASC_ABSTAT_FCSDET			(1 << 0)
      
/***ASC Write HW Modified Autobaud Status Register***/ 
#define AMAZON_ASC_WHBABSTAT				(AMAZON_ASC+ 0x0058)
#define AMAZON_ASC_WHBABSTAT_SETDETWAIT		(1 << 9)
#define AMAZON_ASC_WHBABSTAT_CLRDETWAIT		(1 << 8)
#define AMAZON_ASC_WHBABSTAT_SETSCCDET		(1 << 7)
#define AMAZON_ASC_WHBABSTAT_CLRSCCDET		(1 << 6)
#define AMAZON_ASC_WHBABSTAT_SETSCSDET		(1 << 5)
#define AMAZON_ASC_WHBABSTAT_CLRSCSDET		(1 << 4)
#define AMAZON_ASC_WHBABSTAT_SETFCCDET		(1 << 3)
#define AMAZON_ASC_WHBABSTAT_CLRFCCDET		(1 << 2)
#define AMAZON_ASC_WHBABSTAT_SETFCSDET		(1 << 1)
#define AMAZON_ASC_WHBABSTAT_CLRFCSDET		(1 << 0)
      
/***ASC Clock Control Register***/ 
#define AMAZON_ASC_CLC						(AMAZON_ASC+ 0x0000)
#define AMAZON_ASC_CLC_RMC(value)			(((( 1 << 8) - 1) & (value)) << 8)
#define AMAZON_ASC_CLC_DISS					(1 << 1)
#define AMAZON_ASC_CLC_DISR					(1 << 0)		

/***ASC IRNCR0 **/
#define AMAZON_ASC_IRNCR0					(AMAZON_ASC+ 0x00FC)
/***ASC IRNCR1 **/
#define AMAZON_ASC_IRNCR1					(AMAZON_ASC+ 0x00F8)
#define ASC_IRNCR_TIR	0x1
#define ASC_IRNCR_RIR	0x2
#define ASC_IRNCR_EIR	0x4
/***********************************************************************/
/*  Module      :  DMA register address and bits                       */
/***********************************************************************/
         
#define AMAZON_DMA				(KSEG1+0x10103000)
/***********************************************************************/   
#define AMAZON_DMA_CH_ON        	AMAZON_DMA+0x28
#define AMAZON_DMA_CH_RST      		AMAZON_DMA+0x2c
#define AMAZON_DMA_CH0_ISR      	AMAZON_DMA+0x30
#define AMAZON_DMA_CH1_ISR      	AMAZON_DMA+0x34
#define AMAZON_DMA_CH2_ISR      	AMAZON_DMA+0x38
#define AMAZON_DMA_CH3_ISR     		AMAZON_DMA+0x3c
#define AMAZON_DMA_CH4_ISR      	AMAZON_DMA+0x40
#define AMAZON_DMA_CH5_ISR      	AMAZON_DMA+0x44
#define AMAZON_DMA_CH6_ISR      	AMAZON_DMA+0x48
#define AMAZON_DMA_CH7_ISR      	AMAZON_DMA+0x4c
#define AMAZON_DMA_CH8_ISR      	AMAZON_DMA+0x50
#define AMAZON_DMA_CH9_ISR      	AMAZON_DMA+0x54
#define AMAZON_DMA_CH10_ISR     	AMAZON_DMA+0x58
#define AMAZON_DMA_CH11_ISR     	AMAZON_DMA+0x5c
#define AMAZON_DMA_CH0_MSK      	AMAZON_DMA+0x60
#define AMAZON_DMA_CH1_MSK      	AMAZON_DMA+0x64
#define AMAZON_DMA_CH2_MSK     		AMAZON_DMA+0x68
#define AMAZON_DMA_CH3_MSK      	AMAZON_DMA+0x6c
#define AMAZON_DMA_CH4_MSK      	AMAZON_DMA+0x70
#define AMAZON_DMA_CH5_MSK      	AMAZON_DMA+0x74
#define AMAZON_DMA_CH6_MSK      	AMAZON_DMA+0x78
#define AMAZON_DMA_CH7_MSK      	AMAZON_DMA+0x7c
#define AMAZON_DMA_CH8_MSK      	AMAZON_DMA+0x80
#define AMAZON_DMA_CH9_MSK      	AMAZON_DMA+0x84
#define AMAZON_DMA_CH10_MSK     	AMAZON_DMA+0x88
#define AMAZON_DMA_CH11_MSK     	AMAZON_DMA+0x8c
#define AMAZON_DMA_Desc_BA      	AMAZON_DMA+0x90
#define AMAZON_DMA_CH0_DES_LEN  	AMAZON_DMA+0x94
#define AMAZON_DMA_CH1_DES_LEN  	AMAZON_DMA+0x98
#define AMAZON_DMA_CH2_DES_LEN  	AMAZON_DMA+0x9c
#define AMAZON_DMA_CH3_DES_LEN  	AMAZON_DMA+0xa0
#define AMAZON_DMA_CH4_DES_LEN  	AMAZON_DMA+0xa4
#define AMAZON_DMA_CH5_DES_LEN  	AMAZON_DMA+0xa8
#define AMAZON_DMA_CH6_DES_LEN  	AMAZON_DMA+0xac
#define AMAZON_DMA_CH7_DES_LEN  	AMAZON_DMA+0xb0
#define AMAZON_DMA_CH8_DES_LEN  	AMAZON_DMA+0xb4
#define AMAZON_DMA_CH9_DES_LEN  	AMAZON_DMA+0xb8
#define AMAZON_DMA_CH10_DES_LEN 	AMAZON_DMA+0xbc
#define AMAZON_DMA_CH11_DES_LEN 	AMAZON_DMA+0xc0
#define AMAZON_DMA_CH1_DES_OFST 	AMAZON_DMA+0xc4
#define AMAZON_DMA_CH2_DES_OFST 	AMAZON_DMA+0xc8
#define AMAZON_DMA_CH3_DES_OFST 	AMAZON_DMA+0xcc
#define AMAZON_DMA_CH4_DES_OFST 	AMAZON_DMA+0xd0
#define AMAZON_DMA_CH5_DES_OFST 	AMAZON_DMA+0xd4
#define AMAZON_DMA_CH6_DES_OFST 	AMAZON_DMA+0xd8
#define AMAZON_DMA_CH7_DES_OFST 	AMAZON_DMA+0xdc
#define AMAZON_DMA_CH8_DES_OFST 	AMAZON_DMA+0xe0
#define AMAZON_DMA_CH9_DES_OFST 	AMAZON_DMA+0xe4
#define AMAZON_DMA_CH10_DES_OFST 	AMAZON_DMA+0xe8
#define AMAZON_DMA_CH11_DES_OFST 	AMAZON_DMA+0xec
#define AMAZON_DMA_SW_BL          	AMAZON_DMA+0xf0
#define AMAZON_DMA_TPE_BL         	AMAZON_DMA+0xf4
#define AMAZON_DMA_DPlus2FPI_BL   	AMAZON_DMA+0xf8
#define AMAZON_DMA_GRX_BUF_LEN    	AMAZON_DMA+0xfc
#define AMAZON_DMA_DMA_ECON_REG   	AMAZON_DMA+0x100
#define AMAZON_DMA_POLLING_REG     	AMAZON_DMA+0x104
#define AMAZON_DMA_CH_WGT         	AMAZON_DMA+0x108
#define AMAZON_DMA_TX_WGT         	AMAZON_DMA+0x10c
#define AMAZON_DMA_DPLus2FPI_CLASS 	AMAZON_DMA+0x110
#define AMAZON_DMA_COMB_ISR        	AMAZON_DMA+0x114

//channel reset
#define SWITCH1_RST_MASK	0x83	/* Switch1 channel mask */
#define SWITCH2_RST_MASK	0x10C	/* Switch1 channel mask */
#define TPE_RST_MASK		0x630	/* TPE channel mask */
#define DPlus2FPI_RST_MASK	0x840	/*  DPlusFPI channel mask */

//ISR
#define DMA_ISR_RDERR			0x20
#define DMA_ISR_CMDCPT			0x10
#define DMA_ISR_CPT			0x8
#define DMA_ISR_DURR			0x4
#define DMA_ISR_EOP			0x2
#define DMA_DESC_BYTEOFF_SHIFT		23

#define DMA_POLLING_ENABLE		0x80000000
#define DMA_POLLING_CNT			0x50	/*minimum 0x10, max 0xfff0*/

/***********************************************************************/
/*  Module      :  Debug register address and bits                     */
/***********************************************************************/
         
#define AMAZON_DEBUG                        (KSEG1+0x1F106000)
/***********************************************************************/   

      
/***MCD Break System Control Register***/ 
#define AMAZON_DEBUG_MCD_BSCR                   	((volatile u32*)(AMAZON_DEBUG+ 0x0000))

/***PMC Performance Counter Control Register0***/
#define AMAZON_DEBUG_PMC_PCCR0                          ((volatile u32*)(AMAZON_DEBUG+ 0x0010))

/***PMC Performance Counter Control Register1***/
#define AMAZON_DEBUG_PMC_PCCR1                          ((volatile u32*)(AMAZON_DEBUG+ 0x0014))

/***PMC Performance Counter Register0***/
#define AMAZON_DEBUG_PMC_PCR0                          	((volatile u32*)(AMAZON_DEBUG+ 0x0018))

/*165001:henryhsu:20050603:Source modified by Bing Tao*/

/***PMC Performance Counter Register1***/
//#define AMAZON_DEBUG_PMC_PCR1                          ((volatile u32*)(AMAZON_DEBUG+ 0x0020))
#define AMAZON_DEBUG_PMC_PCR1                            ((volatile u32*)(AMAZON_DEBUG+ 0x001c))

/*165001*/



/***MCD Suspend Mode Control Register***/
#define AMAZON_DEBUG_MCD_SMCR                           ((volatile u32*)(AMAZON_DEBUG+ 0x0024))

/***********************************************************************/
/*  Module      :  GPIO register address and bits                     */
/***********************************************************************/
         
#define AMAZON_GPIO                        (KSEG1+0x10100B00)
/***********************************************************************/   

      
/***Port 0 Data Output Register (0010H)***/ 
#define AMAZON_GPIO_P0_OUT                       ((volatile u32*)(AMAZON_GPIO+ 0x0010))                      
      
/***Port 1 Data Output Register (0040H)***/ 
#define AMAZON_GPIO_P1_OUT                       ((volatile u32*)(AMAZON_GPIO+ 0x0040))                      
      
/***Port 0 Data Input Register (0014H)***/ 
#define AMAZON_GPIO_P0_IN                        ((volatile u32*)(AMAZON_GPIO+ 0x0014))                      
      
/***Port 1 Data Input Register (0044H)***/ 
#define AMAZON_GPIO_P1_IN                        ((volatile u32*)(AMAZON_GPIO+ 0x0044))
                            
/***Port 0 Direction Register (0018H)***/ 
#define AMAZON_GPIO_P0_DIR                       ((volatile u32*)(AMAZON_GPIO+ 0x0018))
      
/***Port 1 Direction Register (0048H)***/ 
#define AMAZON_GPIO_P1_DIR                       ((volatile u32*)(AMAZON_GPIO+ 0x0048))
      
/***Port 0 Alternate Function Select Register 0 (001C H) ***/
#define AMAZON_GPIO_P0_ALTSEL0                    ((volatile u32*)(AMAZON_GPIO+ 0x001C))                      
      
/***Port 1 Alternate Function Select Register 0 (004C H) ***/
#define AMAZON_GPIO_P1_ALTSEL0                    ((volatile u32*)(AMAZON_GPIO+ 0x004C))
                      
/***Port 0 Alternate Function Select Register 1 (0020 H) ***/
#define AMAZON_GPIO_P0_ALTSEL1                    ((volatile u32*)(AMAZON_GPIO+ 0x0020))
      
/***Port 1 Alternate Function Select Register 0 (0050 H) ***/
#define AMAZON_GPIO_P1_ALTSEL1                    ((volatile u32*)(AMAZON_GPIO+ 0x0050))
      
/***Port 0 Open Drain Control Register (0024H)***/ 
#define AMAZON_GPIO_P0_OD                        ((volatile u32*)(AMAZON_GPIO+ 0x0024))                      
      
/***Port 1 Open Drain Control Register (0054H)***/ 
#define AMAZON_GPIO_P1_OD                        ((volatile u32*)(AMAZON_GPIO+ 0x0054))
                      
/***Port 0 Input Schmitt-Trigger Off Register (0028 H) ***/
#define AMAZON_GPIO_P0_STOFF                    ((volatile u32*)(AMAZON_GPIO+ 0x0028))
      
/***Port 1 Input Schmitt-Trigger Off Register (0058 H) ***/
#define AMAZON_GPIO_P1_STOFF                    ((volatile u32*)(AMAZON_GPIO+ 0x0058))
                      
/***Port 0 Pull Up/Pull Down Select Register (002C H)***/ 
#define AMAZON_GPIO_P0_PUDSEL                    ((volatile u32*)(AMAZON_GPIO+ 0x002C))
                      
/***Port 1 Pull Up/Pull Down Select Register (005C H)***/ 
#define AMAZON_GPIO_P1_PUDSEL                    ((volatile u32*)(AMAZON_GPIO+ 0x005C))
                      
/***Port 0 Pull Up Device Enable Register (0030 H)***/ 
#define AMAZON_GPIO_P0_PUDEN                    ((volatile u32*)(AMAZON_GPIO+ 0x0030))
                      
/***Port 1 Pull Up Device Enable Register (0060 H)***/ 
#define AMAZON_GPIO_P1_PUDEN                    ((volatile u32*)(AMAZON_GPIO+ 0x0060))
                                            		                  
/***********************************************************************/
/*  Module      :  BIU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_BIU                          (KSEG1+0x1FA80000)
/***********************************************************************/   

      
/***BIU Identification Register***/ 
#define AMAZON_BIU_ID                       ((volatile u32*)(AMAZON_BIU+ 0x0000))
#define AMAZON_BIU_ID_ARCH                            (1 << 16)
#define AMAZON_BIU_ID_ID(value)                (((( 1 << 8) - 1) & (value)) << 8)
#define AMAZON_BIU_ID_REV(value)                (((( 1 << 8) - 1) & (value)) << 0)
      
/***BIU Access Error Cause Register***/ 
#define AMAZON_BIU_ERRCAUSE                 ((volatile u32*)(AMAZON_BIU+ 0x0100))
#define AMAZON_BIU_ERRCAUSE_ERR                              (1 << 31)
#define AMAZON_BIU_ERRCAUSE_PORT(value)               (((( 1 << 4) - 1) & (value)) << 16)
#define AMAZON_BIU_ERRCAUSE_CAUSE(value)              (((( 1 << 2) - 1) & (value)) << 0)
      
/***BIU Access Error Address Register***/ 
#define AMAZON_BIU_ERRADDR                  ((volatile u32*)(AMAZON_BIU+ 0x0108))
#define AMAZON_BIU_ERRADDR_ADDR		
         
/***********************************************************************/
/*  Module      :  ICU register address and bits                       */
/***********************************************************************/
         
#define AMAZON_ICU								(KSEG1+0x1F101000)
/***********************************************************************/   

/***IM0 Interrupt Status Register***/ 
#define AMAZON_ICU_IM0_ISR						(AMAZON_ICU + 0x0010)
#define AMAZON_ICU_IM1_ISR						(AMAZON_ICU + 0x0020)
#define AMAZON_ICU_IM2_ISR						(AMAZON_ICU + 0x0030)
#define AMAZON_ICU_IM3_ISR						(AMAZON_ICU + 0x0040)
#define AMAZON_ICU_IM4_ISR						(AMAZON_ICU + 0x0050)
	                
/***IM0 Interrupt Enable Register***/ 
#define AMAZON_ICU_IM0_IER						(AMAZON_ICU + 0x0014)
#define AMAZON_ICU_IM1_IER						(AMAZON_ICU + 0x0024)
#define AMAZON_ICU_IM2_IER						(AMAZON_ICU + 0x0034)
#define AMAZON_ICU_IM3_IER						(AMAZON_ICU + 0x0044)
#define AMAZON_ICU_IM4_IER						(AMAZON_ICU + 0x0054)

/***IM0 Interrupt Output Status Register***/ 
#define AMAZON_ICU_IM0_IOSR						(AMAZON_ICU + 0x0018)
#define AMAZON_ICU_IM1_IOSR						(AMAZON_ICU + 0x0028)
#define AMAZON_ICU_IM2_IOSR						(AMAZON_ICU + 0x0038)
#define AMAZON_ICU_IM3_IOSR						(AMAZON_ICU + 0x0048)
#define AMAZON_ICU_IM4_IOSR						(AMAZON_ICU + 0x0058)

/***IM0 Interrupt Request Set Register***/ 
#define AMAZON_ICU_IM0_IRSR						(AMAZON_ICU + 0x001c)
#define AMAZON_ICU_IM1_IRSR						(AMAZON_ICU + 0x002c)
#define AMAZON_ICU_IM2_IRSR						(AMAZON_ICU + 0x003c)
#define AMAZON_ICU_IM3_IRSR						(AMAZON_ICU + 0x004c)
#define AMAZON_ICU_IM4_IRSR						(AMAZON_ICU + 0x005c)

/***Interrupt Vector Value Register***/
#define AMAZON_ICU_IM_VEC						(AMAZON_ICU + 0x0060)

/***Interrupt Vector Value Mask***/
#define AMAZON_ICU_IM0_VEC_MASK					0x0000001f
#define AMAZON_ICU_IM1_VEC_MASK					0x000003e0
#define AMAZON_ICU_IM2_VEC_MASK					0x00007c00
#define AMAZON_ICU_IM3_VEC_MASK					0x000f8000
#define AMAZON_ICU_IM4_VEC_MASK					0x01f00000

/***DMA Interrupt Mask Value***/
#define AMAZON_DMA_H_MASK						0x00000fff
                                                                                       
/***External Interrupt Control Register***/
#define AMAZON_ICU_EXTINTCR						((volatile u32*)(AMAZON_ICU + 0x0000))
#define AMAZON_ICU_IRNICR						((volatile u32*)(AMAZON_ICU + 0x0004))  
#define AMAZON_ICU_IRNCR						((volatile u32*)(AMAZON_ICU + 0x0008))  
#define AMAZON_ICU_IRNEN						((volatile u32*)(AMAZON_ICU + 0x000c))

/***********************************************************************/
/*  Module      :   PCI/Card-BUS/PC-Card register address and bits     */
/***********************************************************************/
         
#define AMAZON_PCI				(KSEG1+0x10105400)
#define AMAZON_PCI_CFG_BASE      		(KSEG1+0x11000000)
#define AMAZON_PCI_MEM_BASE      		(KSEG1+0x12000000)

#define CLOCK_CONTROL           		AMAZON_PCI + 0x00000000
#define ARB_CTRL_bit        			1
#define IDENTIFICATION         			AMAZON_PCI + 0x00000004
#define SOFTRESET               		AMAZON_PCI + 0x00000010
#define PCI_FPI_ERROR_ADDRESS   		AMAZON_PCI + 0x00000014
#define FPI_PCI_ERROR_ADDRESS   		AMAZON_PCI + 0x00000018
#define FPI_ERROR_TAG           		AMAZON_PCI + 0x0000001c
#define IRR                     		AMAZON_PCI + 0x00000020
#define IRA_IR                  		AMAZON_PCI + 0x00000024
#define IRM                     		AMAZON_PCI + 0x00000028
#define DMA_COMPLETE_BIT      			0
#define PCI_POWER_CHANGE_BIT  			16
#define PCI_MASTER0_BROKEN_INT_BIT   		24
#define PCI_MASTER1_BROKEN_INT_BIT   		25
#define PCI_MASTER2_BROKEN_INT_BIT   		26
#define EOI                     		AMAZON_PCI + 0x0000002c
#define PCI_MODE                		AMAZON_PCI + 0x00000030
#define PCI_MODE_cfgok_bit      		24
#define DEVICE_VENDOR_ID        		AMAZON_PCI + 0x00000034
#define SUBSYSTEM_VENDOR_ID     		AMAZON_PCI + 0x00000038
#define POWER_MANAGEMENT        		AMAZON_PCI + 0x0000003c
#define CLASS_CODE1             		AMAZON_PCI + 0x00000040
#define BAR11_MASK              		AMAZON_PCI + 0x00000044
#define BAR12_MASK              		AMAZON_PCI + 0x00000048
#define BAR13_MASK              		AMAZON_PCI + 0x0000004c
#define BAR14_MASK              		AMAZON_PCI + 0x00000050
#define BAR15_MASK              		AMAZON_PCI + 0x00000054
#define BAR16_MASK              		AMAZON_PCI + 0x00000058
#define CARDBUS_CIS_POINTER1    		AMAZON_PCI + 0x0000005c
#define SUBSYSTEM_ID1          			AMAZON_PCI + 0x00000060
#define PCI_ADDRESS_MAP_11      		AMAZON_PCI + 0x00000064
#define PCI_ADDRESS_MAP_12      		AMAZON_PCI + 0x00000068
#define PCI_ADDRESS_MAP_13      		AMAZON_PCI + 0x0000006c
#define PCI_ADDRESS_MAP_14      		AMAZON_PCI + 0x00000070
#define PCI_ADDRESS_MAP_15      		AMAZON_PCI + 0x00000074
#define PCI_ADDRESS_MAP_16      		AMAZON_PCI + 0x00000078
#define FPI_SEGMENT_ENABLE     			AMAZON_PCI + 0x0000007c
#define CLASS_CODE2             		AMAZON_PCI + 0x00000080
#define BAR21_MASK              		AMAZON_PCI + 0x00000084
#define BAR22_MASK              		AMAZON_PCI + 0x00000088
#define BAR23_MASK              		AMAZON_PCI + 0x0000008c
#define BAR24_MASK              		AMAZON_PCI + 0x00000090
#define BAR25_MASK              		AMAZON_PCI + 0x00000094
#define BAR26_MASK              		AMAZON_PCI + 0x00000098
#define CARDBUS_CIS_POINTER2    		AMAZON_PCI + 0x0000009c
#define SUBSYSTEM_ID2           		AMAZON_PCI + 0x000000a0
#define PCI_ADDRESS_MAP_21      		AMAZON_PCI + 0x000000a4
#define PCI_ADDRESS_MAP_22      		AMAZON_PCI + 0x000000a8
#define PCI_ADDRESS_MAP_23      		AMAZON_PCI + 0x000000ac
#define PCI_ADDRESS_MAP_24      		AMAZON_PCI + 0x000000b0
#define PCI_ADDRESS_MAP_25      		AMAZON_PCI + 0x000000b4
#define PCI_ADDRESS_MAP_26      		AMAZON_PCI + 0x000000b8
#define FPI_ADDRESS_MASK11LOW   		AMAZON_PCI + 0x000000bc
#define FPI_ADDRESS_MAP_0       		AMAZON_PCI + 0x000000c0
#define FPI_ADDRESS_MAP_1       		AMAZON_PCI + 0x000000c4
#define FPI_ADDRESS_MAP_2       		AMAZON_PCI + 0x000000c8
#define FPI_ADDRESS_MAP_3       		AMAZON_PCI + 0x000000cc
#define FPI_ADDRESS_MAP_4       		AMAZON_PCI + 0x000000d0
#define FPI_ADDRESS_MAP_5       		AMAZON_PCI + 0x000000d4
#define FPI_ADDRESS_MAP_6       		AMAZON_PCI + 0x000000d8
#define FPI_ADDRESS_MAP_7       		AMAZON_PCI + 0x000000dc
#define FPI_ADDRESS_MAP_11LOW   		AMAZON_PCI + 0x000000e0
#define FPI_ADDRESS_MAP_11HIGH  		AMAZON_PCI + 0x000000e4
#define FPI_BURST_LENGTH        		AMAZON_PCI + 0x000000e8
#define SET_PCI_SERR            		AMAZON_PCI + 0x000000ec
#define DMA_FPI_START_ADDR      		AMAZON_PCI + 0x000000f0
#define DMA_PCI_START_ADDR      		AMAZON_PCI + 0x000000f4
#define DMA_TRANSFER_COUNT      		AMAZON_PCI + 0x000000f8
#define DMA_CONTROL_STATUS      		AMAZON_PCI + 0x000000fc

#define EXT_PCI1_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x0800
#define EXT_PCI2_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x1000
#define EXT_PCI3_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x1800
#define EXT_PCI4_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x2000
#define EXT_PCI5_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x2800
#define EXT_PCI6_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x3000
#define EXT_PCI7_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x3800
#define EXT_PCI8_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x4000
#define EXT_PCI9_CONFIG_SPACE_BASE_ADDR    	AMAZON_PCI_CFG_BASE + 0x4800
#define EXT_PCI10_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x5000
#define EXT_PCI11_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x5800
#define EXT_PCI12_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x6000
#define EXT_PCI13_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x6800
#define EXT_PCI14_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x7000
#define EXT_PCI15_CONFIG_SPACE_BASE_ADDR   	AMAZON_PCI_CFG_BASE + 0x7800
#define EXT_CARDBUS_CONFIG_SPACE_BASE_ADDR  	AMAZON_PCI_CFG_BASE + 0XF000
#define EXT_PCI_BAR1_ADDR                  	0x10
#define EXT_PCI_BAR2_ADDR                  	0x14
#define EXT_PCI_BAR3_ADDR                  	0x18
#define EXT_PCI_BAR4_ADDR                  	0x1C
#define EXT_PCI_BAR5_ADDR                  	0x20
#define EXT_PCI_BAR6_ADDR                  	0x24
                                                                                                             
#define DEVICE_ID_VECDOR_ID_ADDR    		AMAZON_PCI_CFG_BASE + 0x0
#define STATUS_COMMAND_ADDR         		AMAZON_PCI_CFG_BASE + 0x4
#define BUS_MASTER_ENABLE_BIT       		2
#define MEM_SPACE_ENABLE_BIT       		1
#define CLASS_CODE_REVISION_ADDR    		AMAZON_PCI_CFG_BASE + 0x8
#define BIST_HEADER_TYPE_LATENCY_CAHCE_ADDR 	AMAZON_PCI_CFG_BASE + 0xC
#define BAR1_ADDR                   		AMAZON_PCI_CFG_BASE + 0x10
#define BAR2_ADDR                   		AMAZON_PCI_CFG_BASE + 0x14
#define BAR3_ADDR                   		AMAZON_PCI_CFG_BASE + 0x18
#define BAR4_ADDR                   		AMAZON_PCI_CFG_BASE + 0x1C
#define BAR3_ADDR                   		AMAZON_PCI_CFG_BASE + 0x18
#define BAR4_ADDR                   		AMAZON_PCI_CFG_BASE + 0x1C
#define BAR5_ADDR                   		AMAZON_PCI_CFG_BASE + 0x20
#define BAR6_ADDR                   		AMAZON_PCI_CFG_BASE + 0x24
#define CARDBUS_CIS_POINTER_ADDR    		AMAZON_PCI_CFG_BASE + 0x28
#define SUBSYSTEM_ID_VENDOR_ID_ADDR 		AMAZON_PCI_CFG_BASE + 0x2C
#define EXPANSION_ROM_BASE_ADDR     		AMAZON_PCI_CFG_BASE + 0x30
#define CAPABILITIES_POINTER_ADDR   		AMAZON_PCI_CFG_BASE + 0x34
#define RESERVED_0x38               		AMAZON_PCI_CFG_BASE + 0x38
#define MAX_LAT_MIN_GNT_INT_PIN_LINE_ADDR 	AMAZON_PCI_CFG_BASE + 0x3C
#define POWER_MNGT_NEXT_POINTER_CAP_ID_ADDR  	AMAZON_PCI_CFG_BASE + 0x40
#define POWER_MANAGEMENT_CTRL_STATUS_ADDR  	AMAZON_PCI_CFG_BASE + 0x44
#define RESERVED_0x48               		AMAZON_PCI_CFG_BASE + 0x48
#define RESERVED_0x4C               		AMAZON_PCI_CFG_BASE + 0x4C
#define ERROR_ADDR_PCI_FPI_ADDR     		AMAZON_PCI_CFG_BASE + 0x50
#define ERROR_ADdR_FPI_PCI_ADDR     		AMAZON_PCI_CFG_BASE + 0x54
#define ERROR_TAG_FPI_PCI_ADDR      		AMAZON_PCI_CFG_BASE + 0x58
#define PCI_ARB_CTRL_STATUS_ADDR    		AMAZON_PCI_CFG_BASE + 0x5C
#define INTERNAL_ARB_ENABLE_BIT         	0
#define ARB_SCHEME_BIT                  	1
#define PCI_MASTER0_PRIOR_2BITS         	2
#define PCI_MASTER1_PRIOR_2BITS         	4
#define PCI_MASTER2_PRIOR_2BITS         	6
#define PCI_MASTER0_REQ_MASK_2BITS      	8
#define PCI_MASTER1_REQ_MASK_2BITS      	10
#define PCI_MASTER2_REQ_MASK_2BITS      	12
#define PCI_MASTER0_GNT_MASK_2BITS      	14
#define PCI_MASTER1_GNT_MASK_2BITS      	16
#define PCI_MASTER2_GNT_MASK_2BITS      	18
#define FPI_PCI_INT_STATUS_ADDR     		AMAZON_PCI_CFG_BASE + 0x60
#define FPI_PCI_INT_ACK_ADDR        		AMAZON_PCI_CFG_BASE + 0x64
#define FPI_PCI_INT_MASK_ADDR       		AMAZON_PCI_CFG_BASE + 0x68
#define CARDBUS_CTRL_STATUS_ADDR    		AMAZON_PCI_CFG_BASE + 0x6C
#define CARDBUS_CFRAME_ENABLE           	0
                                                                                                             
#define CLOCK_CONTROL_default                	0x00000000
#define CLOCK_CONTROL_mask                   	0x00000003
                                                                                                             
#define IDENTIFICATION_default               	0x0011C002
#define IDENTIFICATION_mask                  	0x00000000
                                                                                                             
#define SOFTRESET_default                    	0x00000000
// SOFTRESET bit 0 is writable but will be reset to 0 after software reset is over
#define SOFTRESET_mask                       	0x00000000
                                                                                                             
#define PCI_FPI_ERROR_ADDRESS_default        	0xFFFFFFFF
#define PCI_FPI_ERROR_ADDRESS_mask           	0x00000000
                                                                                                             
#define FPI_PCI_ERROR_ADDRESS_default        	0xFFFFFFFF
#define FPI_PCI_ERROR_ADDRESS_mask           	0x00000000
                                                                                                             
#define FPI_ERROR_TAG_default                	0x0000000F
#define FPI_ERROR_TAG_mask                   	0x00000000
                                                                                                             
#define IRR_default                          	0x00000000
#define IRR_mask                             	0x07013b2F
                                                                                                             
#define IRA_IR_default                       	0x00000000
#define IRA_IR_mask                          	0x07013b2F
                                                                                                             
#define IRM_default                          	0x00000000
#define IRM_mask                             	0xFFFFFFFF
                                                                                                             
#define EOI_default                          	0x00000000
#define EOI_mask                             	0x00000000
                                                                                                             
#define PCI_MODE_default                     	0x01000103
#define PCI_MODE_mask                        	0x1107070F
                                                                                                             
#define DEVICE_VENDOR_ID_default             	0x000C15D1
#define DEVICE_VENDOR_ID_mask                	0xFFFFFFFF
                                                                                                             
#define SUBSYSTEM_VENDOR_ID_default          	0x000015D1
#define SUBSYSTEM_VENDOR_ID_mask             	0x0000FFFF
                                                                                                             
#define POWER_MANAGEMENT_default             	0x0000001B
#define POWER_MANAGEMENT_mask                	0x0000001F
                                                                                                             
#define CLASS_CODE1_default                  	0x00028000
#define CLASS_CODE1_mask                     	0x00FFFFFF
                                                                                                             
#define BAR11_MASK_default                   	0x0FF00008
#define BAR11_MASK_mask                      	0x8FF00008
                                                                                                             
#define BAR12_MASK_default                   	0x80001800
#define BAR12_MASK_mask                      	0x80001F08
                                                                                                             
#define BAR13_MASK_default                   	0x8FF00008
#define BAR13_MASK_mask                      	0x8FF00008
                                                                                                             
#define BAR14_MASK_default                   	0x8F000000
#define BAR14_MASK_mask                      	0x8FFFFF08
                                                                                                             
#define BAR15_MASK_default                   	0x80000000
#define BAR15_MASK_mask                      	0x8FFFFF08
                                                                                                             
#define BAR16_MASK_default                   	0x80000001
// bit 0 and bit 3 is mutually exclusive
#define BAR16_MASK_mask                      	0x8FFFFFF9
                                                                                                             
#define CARDBUS_CIS_POINTER1_default         	0x00000000
#define CARDBUS_CIS_POINTER1_mask            	0x03FFFFFF
                                                                                                             
#define SUBSYSTEM_ID1_default                	0x0000000C
#define SUBSYSTEM_ID1_mask                   	0x0000FFFF
                                                                                                             
#define PCI_ADDRESS_MAP_11_default           	0x18000000
#define PCI_ADDRESS_MAP_11_mask              	0x7FFFFFF1
                                                                                                             
#define PCI_ADDRESS_MAP_12_default           	0x18100000
#define PCI_ADDRESS_MAP_12_mask              	0x7FFFFF01
                                                                                                             
#define PCI_ADDRESS_MAP_13_default           	0x18200000
#define PCI_ADDRESS_MAP_13_mask              	0x7FF00001
                                                                                                             
#define PCI_ADDRESS_MAP_14_default           	0x70000000
#define PCI_ADDRESS_MAP_14_mask              	0x7FFFFF01
                                             	                                                                
#define PCI_ADDRESS_MAP_15_default           	0x00000001
#define PCI_ADDRESS_MAP_15_mask              	0x7FFFFF01
                                                                                                             
#define PCI_ADDRESS_MAP_16_default           	0x60000000
#define PCI_ADDRESS_MAP_16_mask              	0x7FF00001
                                                                                                             
#define FPI_SEGMENT_ENABLE_default           	0x000003FF
#define FPI_SEGMENT_ENABLE_mask              	0x000003FF
                                                                                                             
#define CLASS_CODE2_default                  	0x00FF0000
#define CLASS_CODE2_mask                     	0x00FFFFFF
                                                                                                             
#define BAR21_MASK_default                   	0x80000008
#define BAR21_MASK_mask                      	0x8FFFFFF8
                                                                                                             
#define BAR22_MASK_default                   	0x80000008
#define BAR22_MASK_mask                      	0x80001F08
                                                                                                             
#define BAR23_MASK_default                   	0x80000008
#define BAR23_MASK_mask                      	0x8FF00008
                                                                                                             
#define BAR24_MASK_default                   	0x8FE00000
#define BAR24_MASK_mask                      	0x8FFFFF08
                                                                                                             
#define BAR25_MASK_default                   	0x8FFFF000
#define BAR25_MASK_mask                      	0x8FFFFF08
                                                                                                             
#define BAR26_MASK_default                   	0x8FFFFFE1
#define BAR26_MASK_mask                      	0x8FFFFFF1
                                                                                                             
#define CARDBUS_CIS_POINTER2_default         	0x00000000
#define CARDBUS_CIS_POINTER2_mask            	0x03FFFFFF

#define SUBSYSTEM_ID2_default                	0x0000000C
#define SUBSYSTEM_ID2_mask                   	0x0000FFFF
                                                                                                             
#define PCI_ADDRESS_MAP_21_default           	0x3FE00000
#define PCI_ADDRESS_MAP_21_mask              	0x7FFFFFF1
                                                                                                             
#define PCI_ADDRESS_MAP_22_default           	0x68000000
#define PCI_ADDRESS_MAP_22_mask             	0x7FFFFF01
                                                                                                             
#define PCI_ADDRESS_MAP_23_default           	0x20000000
#define PCI_ADDRESS_MAP_23_mask              	0x7FF00001
                                                                                                             
#define PCI_ADDRESS_MAP_24_default           	0x70000001
#define PCI_ADDRESS_MAP_24_mask              	0x7FFFFF01
                                                                                                             
#define PCI_ADDRESS_MAP_25_default           	0x78000001
#define PCI_ADDRESS_MAP_25_mask              	0x7FFFFF01
                                                                                                             
#define PCI_ADDRESS_MAP_26_default           	0x20000000
#define PCI_ADDRESS_MAP_26_mask              	0x7FF00001
                                                                                                             
#define FPI_ADDRESS_MASK11LOW_default        	0x00000000
#define FPI_ADDRESS_MASK11LOW_mask           	0x00070000
                                                                                                             
#define FPI_ADDRESS_MAP_0_default            	0x00000000
#define FPI_ADDRESS_MAP_0_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_1_default            	0x10000000
#define FPI_ADDRESS_MAP_1_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_2_default            	0x20000000
#define FPI_ADDRESS_MAP_2_mask               	0xFFF00000

#define FPI_ADDRESS_MAP_3_default            	0x30000000
#define FPI_ADDRESS_MAP_3_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_4_default            	0x40000000
#define FPI_ADDRESS_MAP_4_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_5_default            	0x50000000
#define FPI_ADDRESS_MAP_5_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_6_default            	0x60000000
#define FPI_ADDRESS_MAP_6_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_7_default            	0x70000000
#define FPI_ADDRESS_MAP_7_mask               	0xFFF00000
                                                                                                             
#define FPI_ADDRESS_MAP_11LOW_default        	0xB0000000
#define FPI_ADDRESS_MAP_11LOW_mask           	0xFFFF0000
                                                                                                             
#define FPI_ADDRESS_MAP_11HIGH_default       	0xB8000000
#define FPI_ADDRESS_MAP_11HIGH_mask          	0xFFF80000
                                                                                                             
#define FPI_BURST_LENGTH_default             	0x00000000
#define FPI_BURST_LENGTH_mask                	0x00000303
                                                                                                             
#define SET_PCI_SERR_default                 	0x00000000
#define SET_PCI_SERR_mask                    	0x00000000
                                                                                                             
#define DMA_FPI_START_ADDRESS_default        	0x00000000
#define DMA_FPI_START_ADDRESS_mask           	0xFFFFFFFF
                                                                                                             
#define DMA_PCI_START_ADDRESS_default        	0x00000000
#define DMA_PCI_START_ADDRESS_mask           	0xFFFFFFFF
                                                                                                             
#define DMA_TRANSFER_COUNT_default           	0x00000000
#define DMA_TRANSFER_COUNT_mask              	0x0000FFFF

#define DMA_CONTROL_STATUS_default           	0x00000000
#define DMA_CONTROL_STATUS_mask              	0x00000000   // bit 0,1 is writable

/***********************************************************************/	
#undef IKOS_MINI_BOOT		//don't run a full booting
#ifdef CONFIG_USE_IKOS	
#define CONFIG_USE_VENUS		//Faster, 10M CPU and 192k baudrate
#ifdef CONFIG_USE_VENUS
#define IKOS_CPU_SPEED		10000000										   
#else
#define IKOS_CPU_SPEED		180000	//IKOS is slow										   
#endif 
#endif //CONFIG_USE_IKOS

/* 165001:henryhsu:20050603:Source Modify form Bing Tao */

#if defined(CONFIG_NET_WIRELESS_SPURS) || defined(CONFIG_NET_WIRELESS_SPURS_MODULE)
#define EBU_PCI_SOFTWARE_ARBITOR
#endif

#define  AMAZON_B11
#ifdef AMAZON_B11
#define SWITCH_BUF_FPI_ADDR		(0x10110000)
#define SWITCH_BUF_ADDR			(KSEG1+SWITCH_BUF_FPI_ADDR)
#define SWITCH_BUF_SIZE			(0x2800)
#define AMAZON_B11_CBM_QD_ADDR		(SWITCH_BUF_ADDR+0x0)
#define AMAZON_B11_BOND_CELL_ADDR	(SWITCH_BUF_ADDR+0x000)
#endif
#define  AMAZON_REFERENCE_BOARD
//for AMAZON ATM bonding application
#ifdef AMAZON_REFERENCE_BOARD
#define  GPIO_DETECT_LOW
#else
#undef  GPIO_DETECT_LOW
#endif

/* 165001 */

#undef AMAZON_IKOS_DEBUG_MSG
#undef AMAZON_INT_DEBUG_MSG
#undef AMAZON_ATM_DEBUG_MSG
#undef AMAZON_DMA_DEBUG_MSG
#undef AMAZON_SW_DEBUG_MSG
#undef AMAZON_WDT_DEBUG_MSG
#undef AMAZON_MTD_DEBUG_MSG
#undef AMAZON_SSC_DEBUG_MSG
#undef AMAZON_MEI_DEBUG_MSG

#ifdef AMAZON_IKOS_DEBUG_MSG
#define AMAZON_IKOS_DMSG(fmt,args...) printk("%s:" fmt, __FUNCTION__, ##args)
#else
#define AMAZON_IKOS_DMSG(fmt,args...)
#endif

#ifdef AMAZON_WDT_DEBUG_MSG 
#define AMAZON_WDT_DMSG(fmt, args...) printk( "%s: " fmt, __FUNCTION__ , ##args)
#else 
#define AMAZON_WDT_DMSG(fm,args...)
#endif

#ifdef AMAZON_SSC_DEBUG_MSG 
#define AMAZON_SSC_DMSG(fmt, args...) printk( "%s: " fmt, __FUNCTION__ , ##args)
#else 
#define AMAZON_SSC_DMSG(fm,args...)
#endif

#ifdef AMAZON_DMA_DEBUG_MSG 
#define AMAZON_DMA_DMSG(fmt, args...) printk( "%s: " fmt, __FUNCTION__ , ##args)
#else 
#define AMAZON_DMA_DMSG(fm,args...)
#endif

#ifdef AMAZON_ATM_DEBUG_MSG
#define AMAZON_TPE_DMSG(fmt, args...) printk( "%s: " fmt, __FUNCTION__ , ##args)
#else //not AMAZON_ATM_DEBUG
#define	AMAZON_TPE_DMSG(fmt, args...)
#endif //AMAZON_ATM_DEBUG

#ifdef AMAZON_SW_DEBUG_MSG
#define AMAZON_SW_DMSG(fmt,args...) printk("%s: " fmt, __FUNCTION__ , ##args)
#else
#define AMAZON_SW_DMSG(fmt,args...)
#endif	

#ifdef AMAZON_MTD_DEBUG_MSG
#define AMAZON_MTD_DMSG(fmt,args...) printk("%s: " fmt, __FUNCTION__ , ##args)
#else
#define AMAZON_MTD_DMSG(fmt,args...)
#endif	

#ifdef AMAZON_INT_DEBUG_MSG
#define AMAZON_INT_DMSG(x...) printk(x)
#else
#define AMAZON_INT_DMSG(x...)
#endif 

#ifdef AMAZON_MEI_DEBUG_MSG
#define AMAZON_MEI_DMSG(fmt,args...) printk("%s:" fmt, __FUNCTION__, ##args)
#else
#define AMAZON_MEI_DMSG(fmt,args...)
#endif

#endif //AMAZON_H 
