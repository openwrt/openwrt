/* incaAscSio.h - (AMAZON) ASC UART tty driver header */

#ifndef __AMAZON_ASC_H
#define __AMAZON_ASC_H

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

/* use fractional divider for baudrate settings */
#define AMAZONASC_USE_FDV

#ifdef AMAZONASC_USE_FDV
   #define AMAZONASC_FDV_LOW_BAUDRATE        71
#ifdef CONFIG_USE_IKOS
   #define AMAZONASC_FDV_HIGH_BAUDRATE       443      
#else
   #define AMAZONASC_FDV_HIGH_BAUDRATE       498      
#endif //CONFIG_USE_IKOS
#endif /*AMAZONASC_USE_FDV*/


#define AMAZONASC_TXFIFO_FL       1
#define AMAZONASC_RXFIFO_FL       1
#define AMAZONASC_TXFIFO_FULL     16

/* interrupt lines masks for the ASC device interrupts*/
/* change these macroses if it's necessary */
#define AMAZONASC_IRQ_LINE_ALL        0x000F0000  /* all IRQs */

#define AMAZONASC_IRQ_LINE_TIR            0x00010000      /* TIR - Tx */
#define AMAZONASC_IRQ_LINE_RIR            0x00020000      /* RIR - Rx */
#define AMAZONASC_IRQ_LINE_EIR            0x00040000      /* EIR - Err */
#define AMAZONASC_IRQ_LINE_TBIR           0x00080000      /* TBIR - Tx Buf*/

/* CLC register's bits and bitfields */
#define ASCCLC_DISR        0x00000001
#define ASCCLC_DISS        0x00000002
#define ASCCLC_RMCMASK     0x0000FF00
#define ASCCLC_RMCOFFSET   8

/* CON register's bits and bitfields */
#define ASCCON_MODEMASK 0x0007
    #define ASCCON_M_8SYNC          0x0
    #define ASCCON_M_8ASYNC         0x1
    #define ASCCON_M_8IRDAASYNC     0x2
    #define ASCCON_M_7ASYNCPAR      0x3
    #define ASCCON_M_9ASYNC         0x4
    #define ASCCON_M_8WAKEUPASYNC   0x5
    #define ASCCON_M_8ASYNCPAR      0x7
#define ASCCON_STP      0x0008
#define ASCCON_REN      0x0010
#define ASCCON_PEN      0x0020
#define ASCCON_FEN      0x0040
#define ASCCON_OEN      0x0080
#define ASCCON_PE       0x0100
#define ASCCON_FE       0x0200
#define ASCCON_OE       0x0400
#define ASCCON_FDE      0x0800
#define ASCCON_ODD      0x1000
#define ASCCON_BRS      0x2000
#define ASCCON_LB       0x4000
#define ASCCON_R        0x8000
#define ASCCON_ANY	(ASCCON_PE|ASCCON_FE|ASCCON_OE)

/* WHBCON register's bits and bitfields */
#define ASCWHBCON_CLRREN    0x0010
#define ASCWHBCON_SETREN    0x0020
#define ASCWHBCON_CLRPE     0x0100
#define ASCWHBCON_CLRFE     0x0200
#define ASCWHBCON_CLROE     0x0400
#define ASCWHBCON_SETPE     0x0800
#define ASCWHBCON_SETFE     0x1000
#define ASCWHBCON_SETOE     0x2000

/* ABCON register's bits and bitfields */
#define ASCABCON_ABEN       0x0001
#define ASCABCON_AUREN      0x0002
#define ASCABCON_ABSTEN     0x0004
#define ASCABCON_ABDETEN    0x0008
#define ASCABCON_FCDETEN    0x0010
#define ASCABCON_EMMASK     0x0300
    #define ASCABCON_EMOFF          8
        #define ASCABCON_EM_DISAB       0x0
        #define ASCABCON_EM_DURAB       0x1
        #define ASCABCON_EM_ALWAYS      0x2
#define ASCABCON_TXINV      0x0400
#define ASCABCON_RXINV      0x0800

/* FDV register mask, offset and bitfields*/
#define ASCFDV_VALUE_MASK     0x000001FF

/* WHBABCON register's bits and bitfields */
#define ASCWHBABCON_SETABEN     0x0001
#define ASCWHBABCON_CLRABEN     0x0002

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
#define ASCWHBABSTAT_SETSCCDET  0x0040
#define ASCWHBABSTAT_CLRSCCDET  0x0080
#define ASCWHBABSTAT_CLRDETWAIT 0x0100
#define ASCWHBABSTAT_SETDETWAIT 0x0200

/* TXFCON register's bits and bitfields */
#define ASCTXFCON_TXFEN         0x0001
#define ASCTXFCON_TXFFLU        0x0002
#define ASCTXFCON_TXTMEN        0x0004
#define ASCTXFCON_TXFITLMASK    0x3F00
#define ASCTXFCON_TXFITLOFF     8

/* RXFCON register's bits and bitfields */
#define ASCRXFCON_RXFEN         0x0001
#define ASCRXFCON_RXFFLU        0x0002
#define ASCRXFCON_RXTMEN        0x0004
#define ASCRXFCON_RXFITLMASK    0x3F00
#define ASCRXFCON_RXFITLOFF     8

/* FSTAT register's bits and bitfields */
#define ASCFSTAT_RXFFLMASK      0x003F
#define ASCFSTAT_TXFFLMASK      0x3F00
#define ASCFSTAT_TXFFLOFF       8

#endif /* __AMAZON_ASC_H */

