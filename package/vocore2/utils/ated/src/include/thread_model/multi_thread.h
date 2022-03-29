#ifndef __MULTI_THREAD_H_
#define	__MULTI_THREAD_H_

extern struct MULTI_PROC_OPS thread_ops; 
/* struc define */
struct MULTI_PROC_THREADS {
	/* Common Part for Private Data*/
	struct cmd_queue q;
	/* pthread Specific */
	pthread_t thread;
	pthread_mutex_t lock;
	pthread_cond_t cond;
};
/* Prototype */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic);
int thread_insert_q(void *dri_if, unsigned char *pkt, int len);
int thread_lock_q(void *dri_if);
int thread_unlock_q(void *dri_if);
int thread_wait_data(void *dri_if);
int thread_sig_data(void *dri_if);
int thread_close(void *dri_if);
/* Variables */
//extern struct MULTI_PROC_OPS thread_ops;
#if 0
= {
	.multi_proc_init = thread_init,
	.multi_insert_q = thread_insert_q,
	.multi_lock_q = thread_lock_q,
    .multi_unlock_q = thread_unlock_q,
	.multi_wait_data = thread_wait_data,
	.multi_sig_data = thread_sig_data,
	.multi_proc_close = thread_close,
};
#endif

#endif /* End of MULTI_THREAD_H define */
