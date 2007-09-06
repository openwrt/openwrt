#ifndef AMAZON_ATM_MIB_H
#define AMAZON_ATM_MIB_H

#ifdef CONFIG_IFX_ATM_MIB
#include <asm/types.h>
#ifdef __KERNEL__
#include <linux/list.h>
#endif
#endif /* CONFIG_IFX_ATM_MIB */

#ifndef __KERNEL__
#include <atmMIB/local_list.h>
typedef unsigned int __u32;
#endif

typedef struct{
	__u32	ifHCInOctets_h;
	__u32	ifHCInOctets_l;
	__u32	ifHCOutOctets_h;
	__u32	ifHCOutOctets_l;
	__u32	ifInErrors;
	__u32	ifInUnknownProtos;
	__u32	ifOutErrors;
}atm_cell_ifEntry_t;

typedef struct{
	__u32	ifHCInOctets_h;
	__u32	ifHCInOctets_l;
	__u32	ifHCOutOctets_h;
	__u32	ifHCOutOctets_l;
	__u32	ifInUcastPkts;
	__u32	ifOutUcastPkts;
	__u32	ifInErrors;
	__u32	ifInDiscards;
	__u32	ifOutErros;
	__u32	ifOutDiscards;
}atm_aal5_ifEntry_t;

typedef struct{
	__u32	aal5VccCrcErrors;
	__u32	aal5VccSarTimeOuts;//no timer support yet
	__u32	aal5VccOverSizedSDUs;
}atm_aal5_vcc_t;

#if defined(CONFIG_IFX_ATM_MIB) || defined(IFX_CONFIG_SNMP_ATM_MIB)
/* ATM-MIB data structures */
typedef struct atmIfConfEntry {
	int ifIndex;
    int atmInterfaceMaxVpcs;
    int atmInterfaceMaxVccs;
    int atmInterfaceConfVpcs;
    int atmInterfaceConfVccs;
    int atmInterfaceMaxActiveVpiBits;
    int atmInterfaceMaxActiveVciBits;
    int atmInterfaceIlmiVpi;
    int atmInterfaceIlmiVci;
    int atmInterfaceAddressType;
    char atmInterfaceAdminAddress[40];
    unsigned long atmInterfaceMyNeighborIpAddress;
    char atmInterfaceMyNeighborIfName[20];
    int atmInterfaceCurrentMaxVpiBits;
    int atmInterfaceCurrentMaxVciBits;
    char atmInterfaceSubscrAddress[40];
    int flags;
}atmIfConfEntry;

typedef struct atmTrafficDescParamEntry {
	/* Following three parameters are used to update VCC QoS values */
	int ifIndex;
	short atmVclvpi;
	int atmVclvci;

	unsigned int atmTrafficParamIndex;
	unsigned char   traffic_class;  
	int     max_pcr;
	/* Subramani: Added min_pcr */
	int 	min_pcr;
	int     cdv;
	int     scr;
	int     mbs;
	int atmTrafficRowStatus;
	int atmTrafficFrameDiscard;
	struct list_head vpivci_head;
	struct list_head list;
}atmTrafficDescParamEntry;


typedef struct  atmVclEntry {
 	int ifIndex;
 	short atmVclvpi;
 	int atmVclvci;
 	char vpivci[20];
 	int  atmVclAdminStatus;
 	int atmVclOperStatus;
	unsigned long atmVclLastChange;
 	struct atmTrafficDescParamEntry *atmVclRxTrafficPtr;
 	struct atmTrafficDescParamEntry *atmVclTxTrafficPtr;
 	unsigned char atmVccAalType;
 	unsigned int atmVccAal5TxSduSize;
 	unsigned int atmVccAal5RxSduSize;
 	int atmVccAal5Encap;
 	int atmVclRowStatus;
 	int atmVclCastType;
 	int atmVclConnKind;
 	struct list_head list;
 	int flags;
}atmVclEntry;


typedef union union_atmptrs {
	struct atmIfConfEntry *atmIfConfEntry_ptr;
	struct atmTrafficDescParamEntry *atmTrafficDescParamEntry_ptr;
	struct atmVclEntry *atmVclEntry_ptr;
}union_atmptrs;

/* ATM Character device major number */
#define ATM_MEI_MAJOR	107

/* Protocol Constants */
#define IFX_PROTO_RAW               0
#define IFX_PROTO_BR2684            1
#define IFX_PROTO_PPPOATM           2
#define IFX_PROTO_CLIP              3

/* IOCTL Command Set for ATM-MIB */
#define GET_ATM_IF_CONF_DATA  0x0AB0
#define SET_ATM_IF_CONF_DATA  0x0AB1

#define SET_ATM_QOS_DATA      0x0BC0

#define GET_ATM_VCL_DATA      0x0CD0
#define SET_ATM_VCL_DATA      0x0CD1

#define FIND_VCC_IN_KERNEL	  0x0DE0

/* User defined flags for VCL Table */
#define ATMVCCAAL5CPCSTRANSMITSDUSIZE	9  
#define ATMVCCAAL5CPCSRECEIVESDUSIZE	10 

#endif /* CONFIG_IFX_ATM_MIB || IFX_CONFIG_SNMP_ATM_MIB */

#endif //AMAZON_ATM_MIB_H
