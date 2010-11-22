
#define   FEC_BASE_ADDR_FEC0                ((unsigned int)MCF_MBAR + 0x9000)
#define   FEC_BASE_ADDR_FEC1                ((unsigned int)MCF_MBAR + 0x9800)

/*
#define   FEC_INTC_IMRH_INT_MASK38          (0x00000040)
#define   FEC_INTC_IMRH_INT_MASK39          (0x00000080)
#define   FEC_INTC_ICR_FEC0                 (0x30)
#define   FEC_INTC_ICR_FEC1                 (0x31)
*/
#define   FEC_FECI2CIRQ                     (0xFFC0)
#define   FEC_GPIO_PAR_FECI2CIRQ            \
	(*(volatile unsigned short *)((unsigned int)MCF_MBAR + 0xA44))
/*
#define   FEC_INTC_ICRn(x)                  \
(*(volatile unsigned char *)(void*)
((unsigned int) MCF_MBAR + 0x000740+((x)*0x001)))
#define   FEC_INTC_IMRH                     \
 *(volatile unsigned int*)((unsigned int)MCF_MBAR + 0x000708)
*/
#define   FEC_ECR_DISABLE                   (0x00000000)

#define   FEC_ECR(x)                        \
	(*(volatile unsigned int *)(x + 0x024))
#define   FEC_EIR(x)                        \
	(*(volatile unsigned int *)(x + 0x004))
#define   FEC_PALR(x)                       \
	(*(volatile unsigned int *)(x + 0x0E4))
#define   FEC_PAUR(x)                       \
	(*(volatile unsigned int *)(x + 0x0E8))
#define   FEC_IALR(x)                       \
	(*(volatile unsigned int *)(x + 0x11C))
#define   FEC_IAUR(x)                       \
	(*(volatile unsigned int *)(x + 0x118))
#define   FEC_GALR(x)                       \
	(*(volatile unsigned int *)(x + 0x124))
#define   FEC_GAUR(x)                       \
	(*(volatile unsigned int *)(x + 0x120))
#define   FEC_RCR(x)                        \
	(*(volatile unsigned int *)(x + 0x084))
#define   FEC_FECRFCR(x)                    \
	(*(volatile unsigned int *)(x + 0x18C))
#define   FEC_FECRFAR(x)                    \
	(*(volatile unsigned int *)(x + 0x198))
#define   FEC_FECTFCR(x)                    \
	(*(volatile unsigned int *)(x + 0x1AC))
#define   FEC_FECTFAR(x)                    \
	(*(volatile unsigned int *)(x + 0x1B8))
#define   FEC_FECTFWR(x)                    \
	(*(volatile unsigned int *)(x + 0x144))
#define   FEC_CTCWR(x)                      \
	(*(volatile unsigned int *)(x + 0x1C8))
#define   FEC_EIMR(x)                       \
	(*(volatile unsigned int *)(x + 0x008))
#define   FEC_TCR(x)                        \
	(*(volatile unsigned int *)(x + 0x0C4))
#define   FEC_MIBC(x)                       \
	(*(volatile unsigned int *)(x + 0x064))
#define   FEC_MSCR(x)                       \
	(*(volatile unsigned int *)(x + 0x044))
#define   FEC_FECTFDR(x)                    \
	(*(volatile unsigned int *)(x + 0x1A4))
#define   FEC_FECRFDR(x)                    \
	(*(volatile unsigned int *)(x + 0x184))
#define   FEC_FECTFSR(x)                    \
	(*(volatile unsigned int *)(x + 0x1A8))
#define   FEC_FECRFSR(x)		    \
	(*(volatile unsigned int *)(x + 0x188))
#define   FECSTAT_RMON_R_PACKETS(x)         \
	(*(volatile unsigned int *)(x + 0x284))
#define   FECSTAT_RMON_T_PACKETS(x)         \
	(*(volatile unsigned int *)(x + 0x204))
#define   FECSTAT_RMON_R_OCTETS(x)          \
	(*(volatile unsigned int *)(x + 0x2C4))
#define   FECSTAT_RMON_T_OCTETS(x)          \
	(*(volatile unsigned int *)(x + 0x244))
#define   FECSTAT_RMON_R_UNDERSIZE(x)       \
	(*(volatile unsigned int *)(x + 0x294))
#define   FECSTAT_RMON_R_OVERSIZE(x)        \
	(*(volatile unsigned int *)(x + 0x298))
#define   FECSTAT_RMON_R_FRAG(x)            \
	(*(volatile unsigned int *)(x + 0x29C))
#define   FECSTAT_RMON_R_JAB(x)             \
	(*(volatile unsigned int *)(x + 0x2A0))
#define   FECSTAT_RMON_R_MC_PKT(x)          \
	(*(volatile unsigned int *)(x + 0x28C))
#define   FECSTAT_RMON_T_COL(x)             \
	(*(volatile unsigned int *)(x + 0x224))
#define   FECSTAT_IEEE_R_ALIGN(x)           \
	(*(volatile unsigned int *)(x + 0x2D4))
#define   FECSTAT_IEEE_R_CRC(x)             \
	(*(volatile unsigned int *)(x + 0x2D0))
#define   FECSTAT_IEEE_R_MACERR(x)          \
	(*(volatile unsigned int *)(x + 0x2D8))
#define   FECSTAT_IEEE_T_CSERR(x)           \
	(*(volatile unsigned int *)(x + 0x268))
#define   FECSTAT_IEEE_T_MACERR(x)          \
	(*(volatile unsigned int *)(x + 0x264))
#define   FECSTAT_IEEE_T_LCOL(x)            \
	(*(volatile unsigned int *)(x + 0x25C))
#define   FECSTAT_IEEE_R_OCTETS_OK(x)       \
	(*(volatile unsigned int *)(x + 0x2E0))
#define   FECSTAT_IEEE_T_OCTETS_OK(x)       \
	(*(volatile unsigned int *)(x + 0x274))
#define   FECSTAT_IEEE_R_DROP(x)            \
	(*(volatile unsigned int *)(x + 0x2C8))
#define   FECSTAT_IEEE_T_DROP(x)            \
	(*(volatile unsigned int *)(x + 0x248))
#define   FECSTAT_IEEE_R_FRAME_OK(x)        \
	(*(volatile unsigned int *)(x + 0x2CC))
#define   FECSTAT_IEEE_T_FRAME_OK(x)        \
	(*(volatile unsigned int *)(x + 0x24C))
#define   FEC_MMFR(x)                       \
	(*(volatile unsigned int *)(x + 0x040))
#define   FEC_FECFRST(x)                    \
	(*(volatile unsigned int *)(x + 0x1C4))

#define   FEC_MAX_FRM_SIZE                  (1518)
#define   FEC_MAXBUF_SIZE                   (1520)

/* Register values */
#define   FEC_ECR_RESET                     (0x00000001)
#define   FEC_EIR_CLEAR                     (0xFFFFFFFF)
#define   FEC_EIR_RL                        (0x00100000)
#define   FEC_EIR_HBERR                     (0x80000000)
#define   FEC_EIR_BABR			    (0x40000000)
/* babbling receive error */
#define   FEC_EIR_BABT			    (0x20000000)
/* babbling transmit error */
#define   FEC_EIR_TXF		            (0x08000000)
/* transmit frame interrupt */
#define   FEC_EIR_MII			    (0x00800000)
/* MII interrupt */
#define   FEC_EIR_LC			    (0x00200000)
/* late collision */
#define   FEC_EIR_XFUN			    (0x00080000)
/* transmit FIFO underrun */
#define   FEC_EIR_XFERR			    (0x00040000)
/* transmit FIFO error */
#define   FEC_EIR_RFERR			    (0x00020000)
/* receive FIFO error */
#define   FEC_RCR_MAX_FRM_SIZE              (FEC_MAX_FRM_SIZE << 16)
#define   FEC_RCR_MII                       (0x00000004)
#define   FEC_FECRFCR_FAE		    (0x00400000)
/* frame accept error */
#define   FEC_FECRFCR_RXW		    (0x00200000)
/* receive wait condition */
#define   FEC_FECRFCR_UF		    (0x00100000)
/* receive FIFO underflow */
#define   FEC_FECRFCR_FRM                   (0x08000000)
#define   FEC_FECRFCR_GR                    (0x7 << 24)

#define   FEC_EIMR_DISABLE		    (0x00000000)

#define   FEC_FECRFAR_ALARM                 (0x300)
#define   FEC_FECTFCR_FRM                   (0x08000000)
#define   FEC_FECTFCR_GR                    (0x7 << 24)
#define   FEC_FECTFCR_FAE		    (0x00400000)
/* frame accept error */
#define   FEC_FECTFCR_TXW		    (0x00040000)
/* transmit wait condition */
#define   FEC_FECTFCR_UF		    (0x00100000)
/* transmit FIFO underflow */
#define   FEC_FECTFCR_OF		    (0x00080000)
/* transmit FIFO overflow */

#define   FEC_FECTFAR_ALARM                 (0x100)
#define   FEC_FECTFWR_XWMRK                 (0x00000000)

#define   FEC_FECTFSR_MSK                   (0xC0B00000)
#define   FEC_FECTFSR_TXW                   (0x40000000)
/* transmit wait condition */
#define   FEC_FECTFSR_FAE                   (0x00800000)
/* frame accept error */
#define   FEC_FECTFSR_UF                    (0x00200000)
/* transmit FIFO underflow */
#define   FEC_FECTFSR_OF                    (0x00100000)
/* transmit FIFO overflow */

#define   FEC_FECRFSR_MSK                   (0x80F00000)
#define   FEC_FECRFSR_FAE                   (0x00800000)
/* frame accept error */
#define   FEC_FECRFSR_RXW                   (0x00400000)
/* receive wait condition */
#define   FEC_FECRFSR_UF                    (0x00200000)
/* receive FIFO underflow */

#define   FEC_CTCWR_TFCW_CRC                (0x03000000)
#define   FEC_TCR_FDEN                      (0x00000004)
#define   FEC_TCR_HBC                       (0x00000002)
#define   FEC_RCR_DRT                       (0x00000002)
#define   FEC_EIMR_MASK                     (FEC_EIR_RL | FEC_EIR_HBERR)
#define   FEC_ECR_ETHEREN                   (0x00000002)
#define   FEC_FECTFCR_MSK                   (0x00FC0000)
#define   FEC_FECRFCR_MSK                   (0x00F80000)
#define   FEC_EIR_GRA                       (0x10000000)
#define   FEC_TCR_GTS                       (0x00000001)
#define   FEC_MIBC_ENABLE                   (0x00000000)
#define   FEC_MIB_LEN                       (228)
#define   FEC_PHY_ADDR                      (0x01)

#define FEC_RX_DMA_PRI                      (6)
#define FEC_TX_DMA_PRI                      (6)

#define   FEC_TX_BUF_NUMBER                 (8)
#define   FEC_RX_BUF_NUMBER                 (64)

#define   FEC_TX_INDEX_MASK                 (0x7)
#define   FEC_RX_INDEX_MASK                 (0x3f)

#define   FEC_RX_DESC_FEC0                  SYS_SRAM_FEC_START
#define   FEC_TX_DESC_FEC0                  \
	(FEC_RX_DESC_FEC0 + FEC_RX_BUF_NUMBER * sizeof(MCD_bufDescFec))

#define   FEC_RX_DESC_FEC1                  \
	(SYS_SRAM_FEC_START + SYS_SRAM_FEC_SIZE/2)
#define   FEC_TX_DESC_FEC1                  \
	(FEC_RX_DESC_FEC1 + FEC_RX_BUF_NUMBER * sizeof(MCD_bufDescFec))

#define   FEC_EIR_MII                       (0x00800000)
#define   FEC_MMFR_READ                     (0x60020000)
#define   FEC_MMFR_WRITE                    (0x50020000)

#define   FEC_FLAGS_RX                      (0x00000001)

#define   FEC_CRCPOL                        (0xEDB88320)

#define   FEC_MII_TIMEOUT                   (2)
#define   FEC_GR_TIMEOUT                    (1)
#define   FEC_TX_TIMEOUT                    (1)
#define   FEC_RX_TIMEOUT                    (1)

#define   FEC_SW_RST                        0x2000000
#define   FEC_RST_CTL                       0x1000000

int fec_read_mii(unsigned int base_addr, unsigned int pa, unsigned int ra,
		 unsigned int *data);
int fec_write_mii(unsigned int base_addr, unsigned int pa, unsigned int ra,
		  unsigned int data);

#define FEC_MII_SPEED                   \
	((MCF_CLK / 2) / ((2500000 / 2) * 2))
