#ifndef _DPNS_H_
#define _DPNS_H_

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_COMMON)
extern int dpns_common_probe(struct platform_device *pdev);
extern void dpns_common_remove(struct platform_device *pdev);
#else
static inline int dpns_common_probe(struct platform_device *pdev){return 0;}
static inline void dpns_common_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_ACL)
extern int dpns_acl_probe(struct platform_device *pdev);
extern void dpns_acl_remove(struct platform_device *pdev);
#else
static inline int dpns_acl_probe(struct platform_device *pdev){return 0;}
static inline void dpns_acl_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_VLAN)
extern int dpns_vlan_probe(struct platform_device *pdev);
extern void dpns_vlan_remove(struct platform_device *pdev);
#else
static inline int dpns_vlan_probe(struct platform_device *pdev){return 0;}
static inline void dpns_vlan_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_L2)
extern int dpns_mac_probe(struct platform_device *pdev);
extern void dpns_mac_remove(struct platform_device *pdev);
#else
static inline int dpns_mac_probe(struct platform_device *pdev){return 0;}
static inline void dpns_mac_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_NAT)
extern int dpns_nat_probe(struct platform_device *pdev);
extern void dpns_nat_remove(struct platform_device *pdev);
#else
static inline int dpns_nat_probe(struct platform_device *pdev){return 0;}
static inline void dpns_nat_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_ROUTER)
extern int dpns_router_probe(struct platform_device *pdev);
extern void dpns_router_remove(struct platform_device *pdev);
#else
static inline int dpns_router_probe(struct platform_device *pdev) {return 0;}
static inline void dpns_router_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_TMU)
extern int dpns_tmu_probe(struct platform_device *pdev);
extern void dpns_tmu_remove(struct platform_device *pdev);
#else
static inline int dpns_tmu_probe(struct platform_device *pdev) {return 0;}
static inline void dpns_tmu_remove(struct platform_device *pdev) {}
#endif

#if IS_ENABLED(CONFIG_SIFLOWER_DPNS_MCAST)
extern int dpns_mcast_probe(struct platform_device *pdev);
extern void dpns_mcast_remove(struct platform_device *pdev);
#else
static inline int dpns_mcast_probe(struct platform_device *pdev){return 0;}
static inline void dpns_mcast_remove(struct platform_device *pdev) {}
#endif

#endif

