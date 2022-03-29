#ifndef __MULTI_IPC_SOCK_H_
#define	__MULTI_IPC_SOCK_H_

extern struct MULTI_PROC_OPS ipc_sock_ops; 
#define PIPE_READ 0
#define PIPE_WRITE 1
/* struc define */
struct MULTI_PROC_SOCK {
	/* Common Part for Private Data*/
	struct cmd_queue q;
	/* IPC Socket Specific */
	int pid;
	int fd[2];	/* 0:Read, 1:Write*/
	int sock;
	int conn;
};
/* Prototype */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic);
int ipc_sock_insert_q(void *dri_if, unsigned char *pkt, int len);
int ipc_sock_lock_q(void *dri_if);
int ipc_sock_unlock_q(void *dri_if);
int ipc_sock_wait_data(void *dri_if);
int ipc_sock_sig_data(void *dri_if);
int ipc_sock_close(void *dri_if);

#endif /* End of MULTI_IPC_SOCK_H define */
