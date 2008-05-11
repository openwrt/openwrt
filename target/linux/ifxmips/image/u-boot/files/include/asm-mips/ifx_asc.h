/*****************************************************************************
 * DANUBE BootROM
 * Copyright (c) 2005, Infineon Technologies AG, All rights reserved
 * IFAP DC COM SD
 *****************************************************************************/
#ifndef __ASC_H
#define __ASC_H

#define DANUBEASC_TXFIFO_FL       		1
#define DANUBEASC_RXFIFO_FL       		1
#define DANUBEASC_TXFIFO_FULL     		16

/* channel operating modes */
#define	ASCOPT_CSIZE	0x00000003
#define	ASCOPT_CS7	0x00000001
#define	ASCOPT_CS8	0x00000002
#define	ASCOPT_PARENB	0x00000004
#define	ASCOPT_STOPB	0x00000008
#define	ASCOPT_PARODD	0x00000010
#define	ASCOPT_CREAD	0x00000020

#define ASC_OPTIONS		(ASCOPT_CREAD | ASCOPT_CS8)

/* ASC input select (0 or 1) */
#define CONSOLE_TTY	0

#define DANUBEASC_TXFIFO_FL       1
#define DANUBEASC_RXFIFO_FL       1
#define DANUBEASC_TXFIFO_FULL     16

/* interrupt lines masks for the ASC device interrupts*/
/* change these macroses if it's necessary */
#define DANUBEASC_IRQ_LINE_ALL        0x0000007f  /* all IRQs */

#define DANUBEASC_IRQ_LINE_TIR            0x00000001      /* Tx Int */
#define DANUBEASC_IRQ_LINE_TBIR           0x00000002      /* Tx Buffer Int */
#define DANUBEASC_IRQ_LINE_RIR            0x00000004      /* Rx Int */
#define DANUBEASC_IRQ_LINE_EIR            0x00000008      /* Error Int */
#define DANUBEASC_IRQ_LINE_ABSTIR         0x00000010      /* Autobaud Start Int */
#define DANUBEASC_IRQ_LINE_ABDETIP        0x00000020      /* Autobaud Detection Int */
#define DANUBEASC_IRQ_LINE_SFCIR          0x00000040      /* Software Flow Control Int */

/* interrupt controller access macros */
#define ASC_INTERRUPTS_ENABLE(X)  \
*((volatile unsigned int*) DANUBE_ICU_IM0_IER) |= X;
#define ASC_INTERRUPTS_DISABLE(X) \
*((volatile unsigned int*) DANUBE_ICU_IM0_IER) &= ~X;
#define ASC_INTERRUPTS_CLEAR(X)   \
*((volatile unsigned int*) DANUBE_ICU_IM0_ISR) = X;

/* CLC register's bits and bitfields */
#define ASCCLC_DISR        0x00000001
#define ASCCLC_DISS        0x00000002
#define ASCCLC_RMCMASK     0x0000FF00
#define ASCCLC_RMCOFFSET   8

/* CON register's bits and bitfields */
#define ASCCON_MODEMASK	0x0000000f
#define ASCCON_M_8ASYNC	0x0
#define ASCCON_M_8IRDA 	0x1
#define ASCCON_M_7ASYNC	0x2
#define ASCCON_M_7IRDA 	0x3
#define ASCCON_WLSMASK 	0x0000000c
#define ASCCON_WLSOFFSET	2
#define ASCCON_WLS_8BIT	0x0
#define ASCCON_WLS_7BIT	0x1
#define ASCCON_PEN     	0x00000010
#define ASCCON_ODD     	0x00000020
#define ASCCON_SP      	0x00000040
#define ASCCON_STP     	0x00000080
#define ASCCON_BRS     	0x00000100
#define ASCCON_FDE     	0x00000200
#define ASCCON_ERRCLK  	0x00000400
#define ASCCON_EMMASK  	0x00001800
#define ASCCON_EMOFFSET	11
#define ASCCON_EM_ECHO_OFF	0x0
#define ASCCON_EM_ECHO_AB	0x1
#define ASCCON_EM_ECHO_ON	0x2
#define ASCCON_LB       	0x00002000
#define ASCCON_ACO      	0x00004000
#define ASCCON_R        	0x00008000
#define ASCCON_PAL      	0x00010000
#define ASCCON_FEN      	0x00020000
#define ASCCON_RUEN     	0x00040000
#define ASCCON_ROEN     	0x00080000
#define ASCCON_TOEN     	0x00100000
#define ASCCON_BEN      	0x00200000
#define ASCCON_TXINV    	0x01000000
#define ASCCON_RXINV    	0x02000000
#define ASCCON_TXMSB    	0x04000000
#define ASCCON_RXMSB    	0x08000000

/* STATE register's bits and bitfields */
#define ASCSTATE_REN     	0x00000001
#define ASCSTATE_PE      	0x00010000
#define ASCSTATE_FE      	0x00020000
#define ASCSTATE_RUE     	0x00040000
#define ASCSTATE_ROE     	0x00080000
#define ASCSTATE_TOE     	0x00100000
#define ASCSTATE_BE      	0x00200000
#define ASCSTATE_TXBVMASK	0x07000000
#define ASCSTATE_TXBVOFFSET	24
#define ASCSTATE_TXEOM     	0x08000000
#define ASCSTATE_RXBVMASK	0x70000000
#define ASCSTATE_RXBVOFFSET	28
#define ASCSTATE_RXEOM     	0x80000000

/* WHBSTATE register's bits and bitfields */
#define ASCWHBSTATE_CLRREN    0x00000001
#define ASCWHBSTATE_SETREN    0x00000002
#define ASCWHBSTATE_CLRPE     0x00000004
#define ASCWHBSTATE_CLRFE     0x00000008
#define ASCWHBSTATE_CLRRUE    0x00000010
#define ASCWHBSTATE_CLRROE    0x00000020
#define ASCWHBSTATE_CLRTOE    0x00000040
#define ASCWHBSTATE_CLRBE     0x00000080
#define ASCWHBSTATE_SETPE     0x00000100
#define ASCWHBSTATE_SETFE     0x00000200
#define ASCWHBSTATE_SETRUE    0x00000400
#define ASCWHBSTATE_SETROE    0x00000800
#define ASCWHBSTATE_SETTOE    0x00001000
#define ASCWHBSTATE_SETBE     0x00002000

/* ABCON register's bits and bitfields */
#define ASCABCON_ABEN       0x0001
#define ASCABCON_AUREN      0x0002
#define ASCABCON_ABSTEN     0x0004
#define ASCABCON_ABDETEN    0x0008
#define ASCABCON_FCDETEN    0x0010

/* FDV register mask, offset and bitfields*/
#define ASCFDV_VALUE_MASK     0x000001FF

/* WHBABCON register's bits and bitfields */
#define ASCWHBABCON_CLRABEN     0x0001
#define ASCWHBABCON_SETABEN     0x0002

/* ABSTAT register's bits and bitfields */
#define ASCABSTAT_FCSDET    0x0001
#define ASCABSTAT_FCCDET    0x0002
#define ASCABSTAT_SCSDET    0x0004
#define ASCABSTAT_SCCDET    0x0008
#define ASCABSTAT_DETWAIT   0x0010

/* WHBABSTAT register's bits and bitfields */
#define ASCWHBABSTAT_CLRFCSDET  0x0001
#define ASCWHBABSTAT_SETFCSDET  0x0002
#define ASCWHBABSTAT_CLRFCCDET  0x0004
#define ASCWHBABSTAT_SETFCCDET  0x0008
#define ASCWHBABSTAT_CLRSCSDET  0x0010
#define ASCWHBABSTAT_SETSCSDET  0x0020
#define ASCWHBABSTAT_CLRSCCDET  0x0040
#define ASCWHBABSTAT_SETSCCDET  0x0080
#define ASCWHBABSTAT_CLRDETWAIT 0x0100
#define ASCWHBABSTAT_SETDETWAIT 0x0200

/* TXFCON register's bits and bitfields */
#define ASCTXFCON_TXFIFO1       0x00000400
#define ASCTXFCON_TXFEN         0x0001
#define ASCTXFCON_TXFFLU        0x0002
#define ASCTXFCON_TXFITLMASK    0x3F00
#define ASCTXFCON_TXFITLOFF     8

/* RXFCON register's bits and bitfields */
#define ASCRXFCON_RXFIFO1       0x00000400
#define ASCRXFCON_RXFEN         0x0001
#define ASCRXFCON_RXFFLU        0x0002
#define ASCRXFCON_RXFITLMASK    0x3F00
#define ASCRXFCON_RXFITLOFF     8

/* FSTAT register's bits and bitfields */
#define ASCFSTAT_RXFFLMASK      0x003F
#define ASCFSTAT_TXFFLMASK      0x3F00
#define ASCFSTAT_TXFFLOFF       8

typedef  struct         /* DanubeAsc_t */
{
	volatile unsigned long  asc_clc;                            /*0x0000*/
	volatile unsigned long  asc_pisel;                          /*0x0004*/
	volatile unsigned long  asc_id;                             /*0x0008*/
	volatile unsigned long  asc_rsvd1[1];   /* for mapping */   /*0x000C*/
	volatile unsigned long  asc_con;                           /*0x0010*/
	volatile unsigned long  asc_state;                          /*0x0014*/
	volatile unsigned long  asc_whbstate;                       /*0x0018*/
	volatile unsigned long  asc_rsvd2[1];   /* for mapping */   /*0x001C*/
	volatile unsigned long  asc_tbuf;                           /*0x0020*/
	volatile unsigned long  asc_rbuf;                           /*0x0024*/
	volatile unsigned long  asc_rsvd3[2];   /* for mapping */   /*0x0028*/
	volatile unsigned long  asc_abcon;                          /*0x0030*/
	volatile unsigned long  asc_abstat;     /* not used */      /*0x0034*/
	volatile unsigned long  asc_whbabcon;                       /*0x0038*/
	volatile unsigned long  asc_whbabstat;  /* not used */      /*0x003C*/
	volatile unsigned long  asc_rxfcon;                         /*0x0040*/
	volatile unsigned long  asc_txfcon;                         /*0x0044*/
	volatile unsigned long  asc_fstat;                          /*0x0048*/
	volatile unsigned long  asc_rsvd4[1];   /* for mapping */   /*0x004C*/
	volatile unsigned long  asc_bg;                             /*0x0050*/
	volatile unsigned long  asc_bg_timer;                       /*0x0054*/
	volatile unsigned long  asc_fdv;                            /*0x0058*/
	volatile unsigned long  asc_pmw;                            /*0x005C*/
	volatile unsigned long  asc_modcon;                         /*0x0060*/
	volatile unsigned long  asc_modstat;                        /*0x0064*/
	volatile unsigned long  asc_rsvd5[2];   /* for mapping */   /*0x0068*/
	volatile unsigned long  asc_sfcc;                           /*0x0070*/
	volatile unsigned long  asc_rsvd6[3];   /* for mapping */   /*0x0074*/
	volatile unsigned long  asc_eomcon;                         /*0x0080*/
	volatile unsigned long  asc_rsvd7[26];   /* for mapping */  /*0x0084*/
	volatile unsigned long  asc_dmacon;                         /*0x00EC*/
	volatile unsigned long  asc_rsvd8[1];   /* for mapping */   /*0x00F0*/
	volatile unsigned long  asc_irnen;                          /*0x00F4*/
	volatile unsigned long  asc_irnicr;                         /*0x00F8*/
	volatile unsigned long  asc_irncr;                          /*0x00FC*/
} DanubeAsc_t;

int asc_init (void);
void asc_puts (const char *s);
void asc_putc (const char c);
int asc_getc (void);

#endif /* __ASC_H */
