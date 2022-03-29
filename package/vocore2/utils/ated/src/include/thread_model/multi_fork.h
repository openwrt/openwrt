#ifndef __MULTI_FORK_H_
#define	__MULTI_FORK_H_
/* Constant Define */
#define SEM_ACC_MODE 0600
#define WAIT_CMD_DELAY 5*1000
//#define SHM_NOT_POSIX 1
#include <semaphore.h>
/* struc define */
struct MULTI_PROC_FORKS {
	/* Common Part for Private Data*/
	struct cmd_queue q;
	/* Fork Specific */
	int pid;
	#ifdef SHM_NOT_POSIX /* For shm_get */
	int shmem_id;
	int lock; /* For semaphor Id */
	#else /* For shm_open */
	int shm_fd;
	sem_t lock;
	#endif
};


union semun {
	int val;                /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STAT & IPC_SET */
    unsigned short *array;          /* array for GETALL & SETALL */
    struct seminfo *__buf;  /* buffer for IPC_INFO */
    void *__pad;
};
/* Prototype */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic);
int fork_insert_q(void *dri_if, unsigned char *pkt, int len);
int fork_lock_q(void *dri_if);
int fork_unlock_q(void *dri_if);
int fork_wait_data(void *dri_if);
int fork_sig_data(void *dri_if);
int fork_close(void *dri_if);

extern struct MULTI_PROC_OPS fork_ops;
#endif /* __MULTI_FORK_H_*/
