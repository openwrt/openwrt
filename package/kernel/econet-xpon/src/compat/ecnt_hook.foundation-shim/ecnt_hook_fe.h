/*
 * compat shim for <ecnt_hook/ecnt_hook_fe.h>  (forwarding-engine / FhNet)
 *
 * Provides the FhNet vlan-mapping hook function-pointer typedefs + the
 * route/bridge skb-mark predicates.  The FHNet_* payload structs themselves
 * are defined in the driver's pwan/gpon_wan.h, which we pull in here so the
 * typedefs resolve regardless of include order at the call site.
 */
#ifndef _COMPAT_ECNT_HOOK_FE_H_
#define _COMPAT_ECNT_HOOK_FE_H_
#include "ecnt_hook/ecnt_common.h"
#include <pwan/gpon_wan.h>		/* FHNet_* payload structs */

typedef int (*P_FHNET_DS_VLAN_HOOK)(FHNet_Mapping_Vlan_Para_In_t *in,
				    unsigned char *dropFlag);
typedef int (*P_FHNET_L3_DS_VLAN_HOOK)(FHNet_L3Pkt_Mapping_Vlan_Para_In_t *in,
				       unsigned char *dropFlag);
typedef int (*P_FHNET_US_VLAN_MAPPING_HOOK)(FHNet_Mapping_Vlan_Para_In_t *in,
					    FHNet_Mapping_Result_Out_t *out);
typedef int (*P_FHNET_L3_US_VLAN_MAPPING_HOOK)(FHNet_L3Pkt_Mapping_Vlan_Para_In_t *in,
					       FHNet_Mapping_Result_Out_t *out);

/* skb-mark predicates -- hwNAT/FhNet route classification is OFF.
 * TODO EN7528: define against the real skb->mark bit layout. */
#ifndef FHWC_IS_MARKED_ROUTE_PKT
#define FHWC_IS_MARKED_ROUTE_PKT(skb)	(0)
#endif
#ifndef FHWC_MARK_BRIDGE_PKT
#define FHWC_MARK_BRIDGE_PKT(skb)	do { (void)(skb); } while (0)
#endif

#endif /* _COMPAT_ECNT_HOOK_FE_H_ */
