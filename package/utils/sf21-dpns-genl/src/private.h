#ifndef __SF_GENL_USER_PRIVATE_H__
#define __SF_GENL_USER_PRIVATE_H__

#include <stdint.h>

#define pr_err printf
#define pr_info printf
#define pr_dbg printf

struct ubus_context *ubus_ctx_get(void);

uint64_t milli_time_get(void);
int is_gonna_exit(void);
void go_exit(void);

int sf_user_genl_init(void);
int sf_user_genl_exit(void);

int l2_mac_init(void);
int l2_mac_exit(void);

int nat_init(void);
int nat_exit(void);

int vlan_init(void);
int vlan_exit(void);

int common_init(void);
int common_exit(void);

int router_init(void);
int router_exit(void);

#ifdef CONFIG_SIFLOWER_GENL_TMU
int tmu_init(void);
int tmu_exit(void);
#else
static inline int tmu_init(void){ return 0; }
static inline int tmu_exit(void){ return 0; }
#endif

#ifdef CONFIG_SIFLOWER_GENL_MCAST
int mcast_init(void);
int mcast_exit(void);
#else
static inline int mcast_init(void){ return 0; }
static inline int mcast_exit(void){ return 0; }
#endif

#ifdef CONFIG_SIFLOWER_GENL_ACL
int acl_init(void);
int acl_exit(void);
#else
static inline int acl_init(void){ return 0; }
static inline int acl_exit(void){ return 0; }
#endif

#endif // __SF_GENL_USER_PRIVATE_H__
