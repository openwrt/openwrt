#ifndef __SF_GENL_USER_H__
#define __SF_GENL_USER_H__

#include "sf_genl_msg.h"

typedef int (*rsp_recv_cb_t)(void *buf, size_t buflen, void *arg);
typedef void (*rsp_expire_cb_t)(void *arg);

int is_genl_registered(void);

int sfgenl_msg_send(uint32_t comp_id, void *buf, size_t buflen);
int sfgenl_msg_send_cb(uint32_t comp_id, void *buf, size_t buflen,
                       rsp_recv_cb_t recv_cb, rsp_expire_cb_t exp_cb,
                       void *cb_arg, uint32_t expired_ms);
int sfgenl_msg_send_recv(uint32_t comp_id, void *sbuf, size_t slen,
                         void **rbuf, size_t *rlen, uint32_t expired_ms);

struct sfgenl_evt_ops {
        int (*event_recv)(int event_id, uint8_t *buf, size_t buflen);
};

int sfgenl_event_ops_register(int comp_id, struct sfgenl_evt_ops *ops);

int parse_mac(uint8_t *buf, const char *str);
#endif // __SF_GENL_USER_H__
