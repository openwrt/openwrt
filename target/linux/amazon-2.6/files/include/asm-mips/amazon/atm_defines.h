#ifndef ATM_DEFINES_H
#define	ATM_DEFINES_H
                	
//Registers Base Address
#define IO_BASE_ADDR				0xA0000000
#define	AAL5_BASE_ADDRESS               	0x10104400+IO_BASE_ADDR
#define	CBM_BASE_ADDRESS              		0x10104000+IO_BASE_ADDR
#define	HTU_BASE_ADDRESS              		0x10105100+IO_BASE_ADDR
#define	QSB_BASE_ADDRESS              		0x10105000+IO_BASE_ADDR
#define	SWIE_BASE_ADDRESS             		0x10105200+IO_BASE_ADDR

//AAL5 Registers
#define	AAL5_SISR0_ADDR                 	AAL5_BASE_ADDRESS+0x20
#define	AAL5_SIMR0_ADDR                 	AAL5_BASE_ADDRESS+0x24
#define	AAL5_SISR1_ADDR                 	AAL5_BASE_ADDRESS+0x28
#define	AAL5_SIMR1_ADDR                 	AAL5_BASE_ADDRESS+0x2C
#define	AAL5_SMFL_ADDR                  	AAL5_BASE_ADDRESS+0x30
#define	AAL5_SATMHD_ADDR                	AAL5_BASE_ADDRESS+0x34
#define	AAL5_SCON_ADDR                  	AAL5_BASE_ADDRESS+0x38
#define	AAL5_SCMD_ADDR                  	AAL5_BASE_ADDRESS+0x3C
#define	AAL5_RISR0_ADDR                 	AAL5_BASE_ADDRESS+0x40
#define	AAL5_RIMR0_ADDR                 	AAL5_BASE_ADDRESS+0x44
#define	AAL5_RISR1_ADDR                 	AAL5_BASE_ADDRESS+0x48
#define	AAL5_RIMR1_ADDR                 	AAL5_BASE_ADDRESS+0x4C
#define	AAL5_RMFL_ADDR                  	AAL5_BASE_ADDRESS+0x50
#define	AAL5_RINTINF0_ADDR              	AAL5_BASE_ADDRESS+0x54
#define	AAL5_RINTINF1_ADDR              	AAL5_BASE_ADDRESS+0x58
#define	AAL5_RES5C_ADDR                 	AAL5_BASE_ADDRESS+0x5C
#define AAL5_RIOL_ADDR				AAL5_BASE_ADDRESS+0x60
#define	AAL5_RIOM_ADDR                 		AAL5_BASE_ADDRESS+0x64
#define	AAL5_SOOL_ADDR                 		AAL5_BASE_ADDRESS+0x68
#define	AAL5_SOOM_ADDR                 		AAL5_BASE_ADDRESS+0x6C
#define	AAL5_RES70_ADDR                 	AAL5_BASE_ADDRESS+0x70
#define	AAL5_RES74_ADDR                 	AAL5_BASE_ADDRESS+0x74
#define	AAL5_RES78_ADDR                 	AAL5_BASE_ADDRESS+0x78
#define	AAL5_RES7C_ADDR                 	AAL5_BASE_ADDRESS+0x7C
#define	AAL5_RES80_ADDR                 	AAL5_BASE_ADDRESS+0x80
#define	AAL5_RES84_ADDR                 	AAL5_BASE_ADDRESS+0x84
#define	AAL5_RES88_ADDR                 	AAL5_BASE_ADDRESS+0x88
#define	AAL5_RES8C_ADDR                 	AAL5_BASE_ADDRESS+0x8C
#define	AAL5_RES90_ADDR                 	AAL5_BASE_ADDRESS+0x90
#define	AAL5_RES94_ADDR                 	AAL5_BASE_ADDRESS+0x94
#define	AAL5_RES98_ADDR                 	AAL5_BASE_ADDRESS+0x98
#define	AAL5_RES9C_ADDR                 	AAL5_BASE_ADDRESS+0x9C
#define	AAL5_RESA0_ADDR                 	AAL5_BASE_ADDRESS+0xA0
#define	AAL5_RESA4_ADDR                 	AAL5_BASE_ADDRESS+0xA4
#define	AAL5_RESA8_ADDR                 	AAL5_BASE_ADDRESS+0xA8
#define	AAL5_RESAC_ADDR                 	AAL5_BASE_ADDRESS+0xAC
#define	AAL5_RESB0_ADDR                 	AAL5_BASE_ADDRESS+0xB0
#define	AAL5_RESB4_ADDR                 	AAL5_BASE_ADDRESS+0xB4
#define	AAL5_RESB8_ADDR                 	AAL5_BASE_ADDRESS+0xB8
#define	AAL5_RESBC_ADDR                 	AAL5_BASE_ADDRESS+0xBC
#define	AAL5_RESC0_ADDR                 	AAL5_BASE_ADDRESS+0xC0
#define	AAL5_RESC4_ADDR                 	AAL5_BASE_ADDRESS+0xC4
#define	AAL5_RESC8_ADDR                 	AAL5_BASE_ADDRESS+0xC8
#define	AAL5_RESCC_ADDR                 	AAL5_BASE_ADDRESS+0xCC
#define	AAL5_RESD0_ADDR                 	AAL5_BASE_ADDRESS+0xD0
#define	AAL5_RESD4_ADDR                 	AAL5_BASE_ADDRESS+0xD4
#define	AAL5_RESD8_ADDR                 	AAL5_BASE_ADDRESS+0xD8
#define	AAL5_RESDC_ADDR                 	AAL5_BASE_ADDRESS+0xDC
#define	AAL5_RESE0_ADDR                 	AAL5_BASE_ADDRESS+0xE0
#define	AAL5_RESE4_ADDR                 	AAL5_BASE_ADDRESS+0xE4
#define	AAL5_RESE8_ADDR                 	AAL5_BASE_ADDRESS+0xE8
#define	AAL5_RESEC_ADDR                 	AAL5_BASE_ADDRESS+0xEC
#define	AAL5_SSRC0_ADDR                 	AAL5_BASE_ADDRESS+0xF0
#define	AAL5_SSRC1_ADDR                 	AAL5_BASE_ADDRESS+0xF4
#define	AAL5_RSRC0_ADDR                 	AAL5_BASE_ADDRESS+0xF8
#define	AAL5_RSRC1_ADDR                 	AAL5_BASE_ADDRESS+0xFC

#define	AAL5S_ISR_QID_MASK              	0xFF000000
#define	AAL5S_ISR_SAB                   	0x00000100
#define	AAL5S_ISR_SE                    	0x00000080
#define	AAL5S_ISR_MFLE                  	0x00000040
#define	AAL5S_ISR_SBE0                  	0x00000020
#define	AAL5S_ISR_SEG0                  	0x00000010
#define	AAL5S_ISR_TAB                   	0x00000004

#define	AAL5_SIMR_MASK                  	0x000001c7
#define	AAL5_SIMR_SAB                   	0x00000100
#define	AAL5_SIMR_SE                    	0x00000080
#define	AAL5_SIMR_MFLE                  	0x00000040
#define	AAL5_SIMR_TAB                   	0x00000004
#define	AAL5_SIMR_SBE0                  	0x00000002
#define	AAL5_SIMR_SEG0                  	0x00000001

#define	AAL5_SCMD_SEQCOUNT_MASK         	0x0000ff00
#define	AAL5_SCMD_MODE_POLL             	0x00000008
#define	AAL5_SCMD_MODE_COUNT            	0x00000000
#define	AAL5_SCMD_AS                    	0x00000004
#define	AAL5_SCMD_SS                    	0x00000002
#define	AAL5_SCMD_AR                    	0x00000001

#define	AAL5R_ISR_CID_MASK              	0xFF000000//ConnectionID
#define	AAL5R_ISR_DBC_MASK              	0x00FF0000//DiscardedByteCounter
#define	AAL5R_ISR_END                   	0x00002000//End
#define	AAL5R_ISR_ICID                  	0x00001000//InvalidConnectionID
#define	AAL5R_ISR_CLP                   	0x00000800//CellLossPriority
#define	AAL5R_ISR_CGST                  	0x00000400//Congestion
#define	AAL5R_ISR_UUE                   	0x00000200//CPCSUUError
#define	AAL5R_ISR_CPIE                  	0x00000100//CPIError
#define	AAL5R_ISR_FE                    	0x00000080//FrameEnd
#define	AAL5R_ISR_MFLE                  	0x00000040//MaximumFrameLengthExceeded
#define	AAL5R_ISR_DBCE                  	0x00000020//DiscardedByteCounterExceeded
#define	AAL5R_ISR_CRC                   	0x00000010//CRCError
#define	AAL5R_ISR_ILEN                  	0x00000008//InvalidLength
#define	AAL5R_ISR_RAB                   	0x00000004//ReceiveAbort

#define	AAL5_RIMR1_MASK                 	0x00003ffc
#define	AAL5_RIMR1_END                  	0x00002000//End
#define	AAL5_RIMR1_ICID                 	0x00001000//InvalidConnectionID
#define	AAL5_RIMR1_CLP                  	0x00000800//CellLossPriority
#define	AAL5_RIMR1_CGST                 	0x00000400//Congestion
#define	AAL5_RIMR1_UUE                  	0x00000200//CPCSUUError
#define	AAL5_RIMR1_CPIE                 	0x00000100//CPIError
#define	AAL5_RIMR1_FE                   	0x00000080//FrameEnd
#define	AAL5_RIMR1_MFLE                 	0x00000040//MaximumFrameLengthExceeded
#define	AAL5_RIMR1_DBCE                 	0x00000020//DiscardedByteCounterExceeded
#define	AAL5_RIMR1_CRC                  	0x00000010//CRCError
#define	AAL5_RIMR1_ILEN                 	0x00000008//InvalidLength
#define	AAL5_RIMR1_RAB                  	0x00000004//ReceiveAbort

//AAL5 Reassambly Errors
#define AAL5_STW1_MASK				0x33//Error mask
#define AAL5_STW0_MASK				0x5c//Error mask
#define AAL5_STW0_BE				0x3//padding bytes mask
#define	AAL5_STW1_CBM				0x20//Transfer from CBM to A5R abnormally ended
#define	AAL5_STW1_CH				0x10//Invalid Channel number error
#define	AAL5_STW1_CLP				0x8//CLP value of cells in packet is 1
#define	AAL5_STW1_CG				0x4//Cell in packet expired congestion
#define	AAL5_STW1_UU				0x2//CPCS-UU value error
#define	AAL5_STW1_CPI				0x1//CPI value error
#define	AAL5_STW0_FE				0x80//Frame end
#define	AAL5_STW0_MFL				0x40//Maximum frame length error
#define	AAL5_STW0_CRC				0x10//CRC error
#define	AAL5_STW0_IL				0x8//Invalid length
#define	AAL5_STW0_RA				0x4//Received abort



//CBM Registers
#define	CBM_NRTTHR_ADDR               	CBM_BASE_ADDRESS+0x10//NonRealTimeThreshold
#define	CBM_CLP0THR_ADDR              	CBM_BASE_ADDRESS+0x14//CLP0Threshold
#define	CBM_CLP1THR_ADDR              	CBM_BASE_ADDRESS+0x18//CLP1Threshold
#define	CBM_QDOFF_ADDR                	CBM_BASE_ADDRESS+0x1C//QueueDescriptorOffset
#define	CBM_CFG_ADDR                  	CBM_BASE_ADDRESS+0x20//Configuration
#define	CBM_HWEXPAR0_ADDR             	CBM_BASE_ADDRESS+0x24//HWExtractParameter0
#define	CBM_RES28_ADDR                	CBM_BASE_ADDRESS+0x28
#define	CBM_WMSTAT0_ADDR               	CBM_BASE_ADDRESS+0x2C
#define	CBM_HWEXCMD_ADDR              	CBM_BASE_ADDRESS+0x30//HWExtractCommand0
#define	CBM_RES34_ADDR                	CBM_BASE_ADDRESS+0x34
#define	CBM_HWEXSTAT0_ADDR            	CBM_BASE_ADDRESS+0x38//HWExtractStatus0
#define	CBM_RES3C_ADDR                	CBM_BASE_ADDRESS+0x3C
#define	CBM_RES40_ADDR                	CBM_BASE_ADDRESS+0x40
#define	CBM_CNT_ADDR                  	CBM_BASE_ADDRESS+0x44//CellCount
#define	CBM_RES48_ADDR                	CBM_BASE_ADDRESS+0x48
#define	CBM_LFR_ADDR                  	CBM_BASE_ADDRESS+0x4C//PointertolastCellinfreeCellQueue
#define	CBM_FFR_ADDR                  	CBM_BASE_ADDRESS+0x50//PointertofirstCellinfreeCellQueue
#define	CBM_RES54_ADDR                	CBM_BASE_ADDRESS+0x54
#define	CBM_RES58_ADDR                	CBM_BASE_ADDRESS+0x58
#define	CBM_RES5C_ADDR                	CBM_BASE_ADDRESS+0x5C
#define	CBM_RES60_ADDR                	CBM_BASE_ADDRESS+0x60
#define	CBM_RES64_ADDR                	CBM_BASE_ADDRESS+0x64
#define	CBM_RES68_ADDR                	CBM_BASE_ADDRESS+0x68
#define	CBM_RES6C_ADDR                	CBM_BASE_ADDRESS+0x6C
#define	CBM_RES70_ADDR                	CBM_BASE_ADDRESS+0x70
#define	CBM_RES74_ADDR                	CBM_BASE_ADDRESS+0x74
#define	CBM_RES78_ADDR                	CBM_BASE_ADDRESS+0x78
#define	CBM_RES7C_ADDR                	CBM_BASE_ADDRESS+0x7C
#define	CBM_RES80_ADDR                	CBM_BASE_ADDRESS+0x80
#define	CBM_RES84_ADDR                	CBM_BASE_ADDRESS+0x84
#define	CBM_RES88_ADDR                	CBM_BASE_ADDRESS+0x88
#define	CBM_RES8C_ADDR                	CBM_BASE_ADDRESS+0x8C
#define	CBM_RES90_ADDR                	CBM_BASE_ADDRESS+0x90
#define	CBM_RES94_ADDR                	CBM_BASE_ADDRESS+0x94
#define	CBM_RES98_ADDR                	CBM_BASE_ADDRESS+0x98
#define	CBM_RES9C_ADDR                	CBM_BASE_ADDRESS+0x9C
#define	CBM_RESA0_ADDR                	CBM_BASE_ADDRESS+0xA0
#define	CBM_RESA4_ADDR                	CBM_BASE_ADDRESS+0xA4
#define	CBM_RESA8_ADDR                	CBM_BASE_ADDRESS+0xA8
#define	CBM_RESAC_ADDR                	CBM_BASE_ADDRESS+0xAC
#define	CBM_RESB0_ADDR                	CBM_BASE_ADDRESS+0xB0
#define	CBM_RESB4_ADDR                	CBM_BASE_ADDRESS+0xB4
#define	CBM_RESB8_ADDR                	CBM_BASE_ADDRESS+0xB8
#define	CBM_RESBC_ADDR                	CBM_BASE_ADDRESS+0xBC
#define	CBM_INTINF0_ADDR              	CBM_BASE_ADDRESS+0xC0//InterruptInfo0
#define	CBM_INTCMD_ADDR               	CBM_BASE_ADDRESS+0xC4//InterruptCommand0
#define	CBM_IMR0_ADDR                 	CBM_BASE_ADDRESS+0xC8//InterruptMask
#define	CBM_SRC0_ADDR                 	CBM_BASE_ADDRESS+0xCC//ServiceRequestControl
#define	CBM_RESD0_ADDR                	CBM_BASE_ADDRESS+0xD0
#define	CBM_RESD4_ADDR                	CBM_BASE_ADDRESS+0xD4
#define	CBM_RESD8_ADDR                	CBM_BASE_ADDRESS+0xD8
#define	CBM_RESDC_ADDR                	CBM_BASE_ADDRESS+0xDC
#define	CBM_RESE0_ADDR                	CBM_BASE_ADDRESS+0xE0
#define	CBM_AAL5IDIS_ADDR             	CBM_BASE_ADDRESS+0xE4//MIB-No.EPDdiscardedpacketsupstream
#define	CBM_AAL5ODIS_ADDR             	CBM_BASE_ADDRESS+0xE8//MIB-No.PPDdiscardedpacketsupstream
#define	CBM_RESEC_ADDR                	CBM_BASE_ADDRESS+0xEC
#define	CBM_RESF0_ADDR                	CBM_BASE_ADDRESS+0xF0
#define	CBM_RESF4_ADDR                	CBM_BASE_ADDRESS+0xF4
#define	CBM_RESF8_ADDR                	CBM_BASE_ADDRESS+0xF8
#define	CBM_RESFC_ADDR                	CBM_BASE_ADDRESS+0xFC

//CBMCFG
#define	CBM_CFG_INTLCK0EN             	0x00000008
#define	CBM_CFG_INT0HLT               	0x00000004
#define	CBM_CFG_START                 	0x00000001

#define	CBM_HWEXPAR_PN_A5             	0x00002000
#define	CBM_HWEXPAR_PN_CM             	0x00000000
#define	CBM_HWEXPAR_SUBADD_PORTMASK   	0x00000070
#define	CBM_HWEXPAR_SUBADD_ADU        	0x00000000
#define	CBM_HWEXPAR_SUBADD_AAL2       	0x00000080
#define	CBM_HWEXPAR_SUBADD_SWIE       	0x00000100

#define	CBM_HWEXCMD_SFE2              	0x00000100
#define	CBM_HWEXCMD_FE2               	0x00000080
#define	CBM_HWEXCMD_SCE2              	0x00000040
#define	CBM_HWEXCMD_SFE1              	0x00000020
#define	CBM_HWEXCMD_FE1               	0x00000010
#define	CBM_HWEXCMD_SCE1              	0x00000008
#define	CBM_HWEXCMD_SFE0              	0x00000004
#define	CBM_HWEXCMD_FE0               	0x00000002
#define	CBM_HWEXCMD_SCE0              	0x00000001

#define	CBM_INTINF0_QID_MASK          	0xFF000000
#define	CBM_INTINF0_ORIGIN_MASK       	0x00F00000
#define	CBM_INTINF0_EF                	0x00004000
#define	CBM_INTINF0_ACA               	0x00002000
#define	CBM_INTINF0_ERR               	0x00001000
#define	CBM_INTINF0_DISC              	0x00000800
#define	CBM_INTINF0_QSBV              	0x00000400
#define	CBM_INTINF0_Q0E               	0x00000200
#define	CBM_INTINF0_Q0I               	0x00000100
#define	CBM_INTINF0_RDE               	0x00000080
#define	CBM_INTINF0_OPF               	0x00000040
#define	CBM_INTINF0_NFCA              	0x00000020
#define	CBM_INTINF0_CLP1TR            	0x00000010
#define	CBM_INTINF0_CLP0TR            	0x00000008
#define	CBM_INTINF0_NRTTR             	0x00000004
#define	CBM_INTINF0_QFD               	0x00000002
#define	CBM_INTINF0_QTR               	0x00000001
#define CBM_INTINF0_QID_SHIFT		24
//CBM QD Word 3
#define	CBM_QD_W3_QOS_0	        	0x00000000
#define	CBM_QD_W3_QOS_1	        	0x40000000
#define	CBM_QD_W3_QOS_2	        	0x80000000
#define	CBM_QD_W3_QOS_3	        	0xc0000000

#define	CBM_QD_W3_DIR_UP              	0x20000000
#define	CBM_QD_W3_DIR_DOWN             	0x00000000

#define	CBM_QD_W3_CLPt             	0x10000000
#define	CBM_QD_W3_RT                  	0x08000000
#define	CBM_QD_W3_AAL5                	0x04000000

#define	CBM_QD_W3_INT_NOINT           	0x00000000
#define	CBM_QD_W3_INT_ACA              	0x01000000
#define	CBM_QD_W3_INT_EOF             	0x02000000
#define	CBM_QD_W3_INT_BOTH           	0x03000000

#define	CBM_QD_W3_THRESHOLD_MASK      	0x00ff0000
#define CBM_QD_W3_WM_EN			0x00000010
#define	CBM_QD_W3_HCR             	0x00000008
#define	CBM_QD_W3_SBID_MASK           	0x00000001

#define	CBM_QD_W3_THRESHOLD_SHIFT     	16

//WATER MARK STATUS
#define	CBM_WM_NRT_MASK           	0x00040000
#define	CBM_WM_CLP0_MASK           	0x00020000
#define	CBM_WM_CLP1_MASK           	0x00010000

//CBMNRTTHR, CBMCLP0THR, CBMCLP0THR
#define CBM_NRT_WM_NONE			0x00000000//no water mark
#define CBM_WM_3_1			0x00010000//3/4 to set, 1/4 to release
#define CBM_WM_3_2			0x00020000//3/4 to set, 2/4 to release
#define CBM_WM_2_1			0x00030000//2/4 to set, 1/4 to release
#define CBM_THR_MASK			0x0000FFFF

#define	CBM_IMR_MASK                  	0x0000fbff
#define	CBM_IMR_reserved             	0xFFFF0400
#define	CBM_IMR_RFULL                  	0x00008000//EndofFrame
#define	CBM_IMR_EF                    	0x00004000//EndofFrame
#define	CBM_IMR_ACA                   	0x00002000//AnyCellArrived
#define	CBM_IMR_ERR             	0x00001000//FPI Error
#define	CBM_IMR_DISC                  	0x00000800//Discard
#define	CBM_IMR_reserved1              	0x00000400//reserved
#define	CBM_IMR_Q0E                   	0x00000200//Queue0Extract
#define	CBM_IMR_Q0I                   	0x00000100//Queue0Insert
#define	CBM_IMR_RDE                   	0x00000080//ReadEmptyQueue
#define	CBM_IMR_OPF                   	0x00000040//OncePerFrame
#define	CBM_IMR_NFCA                  	0x00000020//NoFreeCellAvailable
#define	CBM_IMR_CLP1TR                	0x00000010//CLP1ThresholdReached
#define	CBM_IMR_CLP0TR                	0x00000008//CLP0ThresholdReached
#define	CBM_IMR_NRTTR                 	0x00000004//NonRealTimeThresholdReached
#define	CBM_IMR_QFD                   	0x00000002//QueueFrameDiscard
#define	CBM_IMR_QTR                   	0x00000001//QueueThresholdReached

#define	CBM_EXSTAT_FB                 	0x00000010
#define	CBM_EXSTAT_SCB                	0x00000008
#define	CBM_EXSTAT_Q0                 	0x00000004
#define	CBM_EXSTAT_RDE                	0x00000002
#define	CBM_EXSTAT_QV                 	0x00000001

//HTU Registers
#define	HTU_RX0_ADDR                  	HTU_BASE_ADDRESS+0x10
#define	HTU_RX1_ADDR                  	HTU_BASE_ADDRESS+0x14
#define	HTU_RES18_ADDR                 	HTU_BASE_ADDRESS+0x18
#define	HTU_RES1C_ADDR                 	HTU_BASE_ADDRESS+0x1C
#define	HTU_RES20_ADDR                 	HTU_BASE_ADDRESS+0x20
#define	HTU_RES24_ADDR                 	HTU_BASE_ADDRESS+0x24
#define	HTU_RES28_ADDR                 	HTU_BASE_ADDRESS+0x28
#define	HTU_RES2C_ADDR                	HTU_BASE_ADDRESS+0x2C
#define	HTU_PCF0PAT_ADDR              	HTU_BASE_ADDRESS+0x30
#define	HTU_PCF1PAT_ADDR              	HTU_BASE_ADDRESS+0x34
#define	HTU_RES38_ADDR                	HTU_BASE_ADDRESS+0x38
#define	HTU_RES3C_ADDR                	HTU_BASE_ADDRESS+0x3C
#define	HTU_RES40_ADDR                	HTU_BASE_ADDRESS+0x40
#define	HTU_RES44_ADDR                	HTU_BASE_ADDRESS+0x44
#define	HTU_RES48_ADDR                	HTU_BASE_ADDRESS+0x48
#define	HTU_RES4C_ADDR                	HTU_BASE_ADDRESS+0x4C
#define	HTU_PCF0MASK_ADDR             	HTU_BASE_ADDRESS+0x50
#define	HTU_PCF1MASK_ADDR             	HTU_BASE_ADDRESS+0x54
#define	HTU_RES58_ADDR                	HTU_BASE_ADDRESS+0x58
#define	HTU_RES5C_ADDR                	HTU_BASE_ADDRESS+0x5C
#define	HTU_RES60_ADDR                	HTU_BASE_ADDRESS+0x60
#define	HTU_RES64_ADDR                	HTU_BASE_ADDRESS+0x64
#define	HTU_RES68_ADDR                	HTU_BASE_ADDRESS+0x68
#define	HTU_RES6C_ADDR                	HTU_BASE_ADDRESS+0x6C
#define	HTU_TIMEOUT_ADDR              	HTU_BASE_ADDRESS+0x70
#define	HTU_DESTOAM_ADDR              	HTU_BASE_ADDRESS+0x74
#define	HTU_DESTRM_ADDR               	HTU_BASE_ADDRESS+0x78
#define	HTU_DESTOTHER_ADDR            	HTU_BASE_ADDRESS+0x7C
#define	HTU_CFG_ADDR                  	HTU_BASE_ADDRESS+0x80
#define	HTU_RES84_ADDR                	HTU_BASE_ADDRESS+0x84
#define	HTU_RES88_ADDR                	HTU_BASE_ADDRESS+0x88
#define	HTU_RES8C_ADDR                	HTU_BASE_ADDRESS+0x8C
#define	HTU_INFNOENTRY_ADDR           	HTU_BASE_ADDRESS+0x90
#define	HTU_INFTIMEOUT_ADDR           	HTU_BASE_ADDRESS+0x94
#define	HTU_RES98_STAT                 	HTU_BASE_ADDRESS+0x98
#define	HTU_RES9C_ADDR                	HTU_BASE_ADDRESS+0x9C
#define	HTU_MIBCIUP	           	HTU_BASE_ADDRESS+0xA0//MIB Counter In Unknown Protoc Register
#define	HTU_CNTTIMEOUT_ADDR           	HTU_BASE_ADDRESS+0xA4
#define	HTU_RESA8_ADDR                	HTU_BASE_ADDRESS+0xA8
#define	HTU_RESAC_ADDR                	HTU_BASE_ADDRESS+0xAC
#define	HTU_RAMADDR_ADDR              	HTU_BASE_ADDRESS+0xB0
#define	HTU_RAMCMD_ADDR               	HTU_BASE_ADDRESS+0xB4
#define	HTU_RAMSTAT_ADDR              	HTU_BASE_ADDRESS+0xB8
#define	HTU_RESBC_ADDR                	HTU_BASE_ADDRESS+0xBC
#define	HTU_RAMDAT1_ADDR              	HTU_BASE_ADDRESS+0xC0
#define	HTU_RAMDAT2_ADDR              	HTU_BASE_ADDRESS+0xC4
#define	HTU_RESCC_ADDR                	HTU_BASE_ADDRESS+0xCC
#define	HTU_RESD0_ADDR                	HTU_BASE_ADDRESS+0xD0
#define	HTU_RESD4_ADDR                	HTU_BASE_ADDRESS+0xD4
#define	HTU_RESD8_ADDR                	HTU_BASE_ADDRESS+0xD8
#define	HTU_RESDC_ADDR                	HTU_BASE_ADDRESS+0xDC
#define	HTU_RESE0_ADDR                	HTU_BASE_ADDRESS+0xE0
#define	HTU_RESE4_ADDR                	HTU_BASE_ADDRESS+0xE4
#define	HTU_IMR0_ADDR                 	HTU_BASE_ADDRESS+0xE8
#define	HTU_RESEC_ADDR                 	HTU_BASE_ADDRESS+0xEC
#define	HTU_ISR0_ADDR                 	HTU_BASE_ADDRESS+0xF0
#define	HTU_RESF4_ADDR                 	HTU_BASE_ADDRESS+0xF4
#define	HTU_SRC0_ADDR                 	HTU_BASE_ADDRESS+0xF8
#define	HTU_RESFC_ADDR                 	HTU_BASE_ADDRESS+0xFC

//HTU_CFG
#define	HTU_CFG_START                 	0x00000001

#define	HTU_RAMCMD_RMW                	0x00000004
#define	HTU_RAMCMD_RD                 	0x00000002
#define	HTU_RAMCMD_WR                 	0x00000001

#define	HTU_RAMDAT1_VCON              	0x00000080//validconnection
#define	HTU_RAMDAT1_VCT               	0x00000040//vcivalueistransparent
#define	HTU_RAMDAT1_QIDS              	0x00000020//qid selects a cell in cbm
#define	HTU_RAMDAT1_VCI3              	0x00000010//vci3->oamqueue
#define	HTU_RAMDAT1_VCI4              	0x00000008//vci4->oamqueue
#define	HTU_RAMDAT1_VCI6              	0x00000004//vci6->rmqueue
#define	HTU_RAMDAT1_PTI4              	0x00000002//pti4->oamqueue
#define	HTU_RAMDAT1_PTI5              	0x00000001//pti5->oamqueue

#define	HTU_RAMDAT2_PTI6              	0x00000800
#define	HTU_RAMDAT2_PTI7              	0x00000400
#define	HTU_RAMDAT2_F4U               	0x00000200
#define	HTU_RAMDAT2_F5U               	0x00000100
#define HTU_RAMDAT2_QID_MASK		0x000000ff

#define	HTU_ISR_NE                    	0x00000001
#define	HTU_ISR_TORD                  	0x00000002
#define	HTU_ISR_IT                    	0x00000008
#define	HTU_ISR_OTOC                  	0x00000010
#define	HTU_ISR_ONEC                  	0x00000020
#define	HTU_ISR_PNE                  	0x00000040
#define	HTU_ISR_PT                  	0x00000080
#define	HTU_ISR_MASK                  	0x000000ff


//QSB Registers
#define	QSB_BIP0_ADDR                 	QSB_BASE_ADDRESS+0x00
#define	QSB_BIP1_ADDR                 	QSB_BASE_ADDRESS+0x04
#define	QSB_BIP2_ADDR                 	QSB_BASE_ADDRESS+0x08
#define	QSB_BIP3_ADDR                 	QSB_BASE_ADDRESS+0x0C
#define	QSB_RSVP_ADDR                 	QSB_BASE_ADDRESS+0x10
#define	QSB_TNOW_ADDR                 	QSB_BASE_ADDRESS+0x14
#define	QSB_TNOWCYC_ADDR              	QSB_BASE_ADDRESS+0x18
#define	QSB_TAU_ADDR                  	QSB_BASE_ADDRESS+0x1C
#define	QSB_L1BRS_ADDR                	QSB_BASE_ADDRESS+0x20
#define	QSB_SBL_ADDR                  	QSB_BASE_ADDRESS+0x24
#define	QSB_CONFIG_ADDR               	QSB_BASE_ADDRESS+0x28
#define	QSB_RTM_ADDR             	QSB_BASE_ADDRESS+0x2C
#define	QSB_RTD_ADDR             	QSB_BASE_ADDRESS+0x30
#define	QSB_RAMAC_ADDR                	QSB_BASE_ADDRESS+0x34
#define	QSB_ISR_ADDR                  	QSB_BASE_ADDRESS+0x38
#define	QSB_IMR_ADDR                  	QSB_BASE_ADDRESS+0x3C
#define	QSB_SRC_ADDR                  	QSB_BASE_ADDRESS+0x40

#define	QSB_TABLESEL_QVPT              	8
#define	QSB_TABLESEL_QPT              	1
#define	QSB_TABLESEL_SCT              	2
#define	QSB_TABLESEL_SPT              	3
#define	QSB_TABLESEL_CALENDARWFQ      	4/*notusedbyFW*/
#define	QSB_TABLESEL_L2WFQ            	5/*notusedbyFW*/
#define	QSB_TABLESEL_CALENDARRS       	6/*notusedbyFW*/
#define	QSB_TABLESEL_L2BITMAPRS       	7/*notusedbyFW*/
#define	QSB_TABLESEL_SHIFT            	24
#define	QSB_TWFQ_MASK                 	0x3FFF0000
#define	QSB_TPRS_MASK                 	0x0000FFFF
#define	QSB_SBID_MASK                 	0xF
#define	QSB_TWFQ_SHIFT                	16
#define	QSB_SCDRATE_MASK              	0x00007FFF
#define	QSB_SBVALID_MASK              	0x80000000

#define	QSB_ISR_WFQLE                 	0x00000001
#define	QSB_ISR_WFQBE                 	0x00000002
#define	QSB_ISR_RSLE                  	0x00000004
#define	QSB_ISR_RSBE                  	0x00000008
#define	QSB_ISR_MUXOV                 	0x00000010
#define	QSB_ISR_CDVOV                 	0x00000020
#define	QSB_ISR_PARAMI                	0x00000040
#define	QSB_ISR_SLOSS                 	0x00000080
#define	QSB_ISR_IIPS                  	0x00000100

#define	QSB_IMR_WFQLE                 	0x00000001
#define	QSB_IMR_WFQBE                 	0x00000002
#define	QSB_IMR_RSLE                  	0x00000004
#define	QSB_IMR_RSBE                  	0x00000008
#define	QSB_IMR_MUXOV                 	0x00000010
#define	QSB_IMR_CDVOV                 	0x00000020
#define	QSB_IMR_PARAMI                	0x00000040
#define	QSB_IMR_SLOSS                 	0x00000080
#define	QSB_IMR_IIPS                  	0x00000100

#define	QSB_READ                      	0x0
#define	QSB_WRITE                     	0x80000000
#define	QSB_READ_ALL                  	0xFFFFFFFF

#if 1 //some bug with QSB access mask
#define	QSB_QPT_SET_MASK              	0x0
#define QSB_QVPT_SET_MASK		0x0
#define	QSB_SET_SCT_MASK              	0x0
#define	QSB_SET_SPT_MASK              	0x0
#define	QSB_SET_SPT_SBVALID_MASK      	0x7FFFFFFF
#else //some bug with QSB access mask
#define	QSB_QPT_SET_MASK              	0x80000000
#define QSB_QVPT_SET_MASK		0x0
#define	QSB_SET_SCT_MASK              	0xFFFFFFE0
#define	QSB_SET_SPT_MASK              	0x7FF8C000
#define	QSB_SET_SPT_SBVALID_MASK      	0x7FFFFFFF
#endif //some bug with QSB access mask

#define	QSB_SPT_SBVALID               	0x80000000

#define	QSB_RAMAC_REG_LOW             	0x0
#define	QSB_RAMAC_REG_HIGH            	0x00010000

#define	SRC_SRE_ENABLE                	0x1000
#define SRC_CLRR			0x4000	//request clear bit



//SWIE Registers
#define	SWIE_IQID_ADDR                	SWIE_BASE_ADDRESS+0x0c//SWIEInsertQueueDescriptor
#define	SWIE_ICMD_ADDR                	SWIE_BASE_ADDRESS+0x10//SWIEInsertCommand
#define	SWIE_ISTAT_ADDR               	SWIE_BASE_ADDRESS+0x14//SWIEInsertStatus
#define	SWIE_ESTAT_ADDR               	SWIE_BASE_ADDRESS+0x18//SWIEExtractStatus
#define	SWIE_ISRC_ADDR                	SWIE_BASE_ADDRESS+0x74//SWIEInsertServiceRequestControl
#define	SWIE_ESRC_ADDR                	SWIE_BASE_ADDRESS+0x78//SWIEExtractServiceRequestControl
#define	SWIE_ICELL_ADDR               	SWIE_BASE_ADDRESS+0x80//SWIEInsertCell(0x80-0xb4)
#define	SWIE_ECELL_ADDR               	SWIE_BASE_ADDRESS+0xc0//SWIEExtractCell(0xc0-0xf4)

#define SWIE_ISTAT_DONE			0x1
#define SWIE_ESTAT_DONE			0x1
#define	SWIE_ICMD_START               	0x00000001//Startcommandforinsertion
#define	SWIE_CBM_SCE0                 	CBM_HWEXCMD_SCE0//CBMcommandforSingle-Cell-Extract
#define	SWIE_CBM_PID_SUBADDR          	0x00001000//CBMPortIDandSubAddressforUTOPIA

//Extracted cell format
//52bytes AAL0 PDU + "Input cell additional data"(14bits)
#define SWIE_ADDITION_DATA_MASK		0x7fff
#define SWIE_EPORT_MASK			0x7000//Source ID (000 AUB0, 001 AUB1)
#define SWIE_EF4USER_MASK		0x800
#define SWIE_EF5USER_MASK		0x400
#define SWIE_EOAM_MASK			0x200
#define SWIE_EAUU_MASK			0x100
#define SWIE_EVCI3_MASK			0x80
#define SWIE_EVCI4_MASK			0x40
#define SWIE_EVCI6_MASK			0x20
#define SWIE_EPTI4_MASK			0x10
#define SWIE_EPTI5_MASK			0x8
#define SWIE_EPTI6_MASK			0x4
#define SWIE_EPTI7_MASK			0x2
#define SWIE_ECRC10ERROR_MASK		0x1

#define CBM_CELL_SIZE			0x40
#define CBM_QD_SIZE			0x10
#define	AAL5R_TRAILER_LEN		12
#define AAL5S_INBOUND_HEADER		8

//constants
//TODO: to be finalized by system guys
//DMA QOS defined by ATM QoS Service type
#define DMA_RX_CH0			0
#define DMA_RX_CH1			1
#define DMA_TX_CH0			0
#define DMA_TX_CH1			1
#define CBR_DMA_QOS			CBM_QD_W3_QOS_0
#define VBR_RT_DMA_QOS			CBM_QD_W3_QOS_0
#define VBR_NRT_DMA_QOS			CBM_QD_W3_QOS_0
#define UBR_PLUS_DMA_QOS		CBM_QD_W3_QOS_0
#define UBR_DMA_QOS			CBM_QD_W3_QOS_0

#define	SRC_TOS_MIPS                  	0
#define	AAL5R_SRPN                      0x00000006//a5rneedshigherprioritythanDR
#define	AAL5S_SRPN                      0x00000005
#define	CBM_MIPS_SRPN                 	0x00000004
#define	QSB_SRPN                      	0x00000023
#define	HTU_SRPN1                     	0x00000022
#define	HTU_SRPN0                     	0x00000021

#endif //ATM_DEFINES_H

