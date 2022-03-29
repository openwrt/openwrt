#include "precomp.h"
/* Variables */
struct MULTI_PROC_OPS fork_ops = {
	.multi_proc_init = multi_proc_init,
	.multi_insert_q = fork_insert_q,
	.multi_lock_q = fork_lock_q,
    .multi_unlock_q = fork_unlock_q,
	.multi_wait_data = fork_wait_data,
	.multi_sig_data = fork_sig_data,
	.multi_proc_close = fork_close,
};
#define SHM_FILE "./ated_shm"
/* Functions */
#ifdef SHM_NOT_POSIX /* For shm_get */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic)
{
	int tmp = 0;
	int shm_id = 0;
	int pid = -1;
	int shm_key;
	union semun init_val;
	struct DRI_IF **a_ifs = (struct DRI_IF **)dri_if;
	struct DRI_IF *p_if = a_ifs[idx];
	void *shm = (void *)0;
	struct MULTI_PROC_FORKS *priv_data = NULL;
	void (*handler)(void *arg) = pkt_proc_logic;	
	/* Allocate Share Memory */
	shm_key = ftok(p_if->ifname, idx);
	shm_id = shmget(shm_key, sizeof(struct MULTI_PROC_FORKS),  S_IRUSR | S_IWUSR |IPC_CREAT);
	if(shm_id < 0){
		ate_printf(MSG_ERROR,"shm_id allocation failed for size(%d), [%d]%s\n", sizeof(struct MULTI_PROC_FORKS), errno,strerror(errno));
		return -1;
	}

	shm = shmat(shm_id, (void *)NULL, 0);
	if((void *)-1 ==  shm){
		ate_printf(MSG_ERROR,"Share memory allocation failed with shmid: %d, [%d]%s\n", shm_id, errno,strerror(errno));
		return -1;
	}
	priv_data = (struct MULTI_PROC_FORKS *)shm;
	ate_printf(MSG_ERROR,"Share memory allocate Success for if(%s:%x)\n",p_if->ifname,priv_data);
	priv_data->shmem_id = shm_id;
	/* Semaphore Init */
	priv_data->lock = semget(IPC_PRIVATE, 1, SEM_ACC_MODE);
	init_val.val = 1;
	tmp = semctl(priv_data->lock, 0, SETVAL, init_val);
	if(tmp == -1){
		ate_printf(MSG_ERROR,"Semaphore allocation failed\n");
		return -1;
	}
	/* Queue Init */
	priv_data->q.un_served = 0;
	priv_data->q.served = 0;
	for(tmp=0;tmp<CMD_QUEUE_SIZE;tmp++){
		os_memset(priv_data->q.cmd_arr[tmp], 0, PKT_BUF_SIZE);
	}
	/* Ops Init */
	p_if->ops = &fork_ops;
	/* Fork Sub-process */
	p_if->priv_data = priv_data;
	pid = fork();
	
	switch(pid){
	case -1:
		ate_printf(MSG_ERROR,"Fork sub-process failed\n");
		break;
	case 0:
		ate_printf(MSG_INFO,"Fork sub-process success, pid:%d\n",pid);
		priv_data->pid = getpid();
		if(pkt_proc_logic != NULL)
			handler(p_if);
		ate_printf(MSG_DEBUG,"Fork sub-process prepares leaving, pid:%d\n",pid);
		exit(0);
		break;
	default:
		break;
	}
	return 0;
}
#if 0
int fork_insert_q(void *dri_if, unsigned char *pkt, int len)
{
	struct DRI_IF *p_if = dri_if;
	struct COMMON_PRIV *priv_data;
	struct cmd_queue *q;
	priv_data = (struct COMMON_PRIV *)p_if->priv_data;
	q = &priv_data->q;
	os_memcpy(q->cmd_arr[q->un_served],pkt,len);
	return len;

}
#endif

int fork_lock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	struct sembuf op;
  	int semid = priv_data->lock;
	op.sem_num = op.sem_flg = 0;
  	op.sem_op = -1; // dec count by 1
	if(semop(semid, &op, 1) == -1)
		ate_printf(MSG_ERROR,"Lock failed\n");
//	ate_printf(MSG_DEBUG,"Queue locked\n");
	return 0;
}

int fork_unlock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	struct sembuf op;
  	int semid = priv_data->lock;
	op.sem_num = op.sem_flg = 0;
	op.sem_op = 1; // inc count by 1
	if(semop(semid, &op, 1) == -1)
		ate_printf(MSG_ERROR,"Unlock failed\n");
//	ate_printf(MSG_DEBUG,"Queue un-locked\n");
	
	return 0;
}

int fork_wait_data(void *dri_if)
{
	fork_unlock_q(dri_if);
	usleep(WAIT_CMD_DELAY);
	fork_lock_q(dri_if);
	return 0;
}

int fork_sig_data(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	return 1;	/* For nothing is serving */
}

int fork_close(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	int shm_id = priv_data->shmem_id;
	int lock = priv_data->lock;
	int pid = priv_data->pid;
	int my_pid = getpid();
	if(my_pid == pid) {
		ate_printf(MSG_DEBUG,"Forked sub-process is terminating at (%s)\n",p_if->ifname);
		if(semctl(lock, 0, IPC_RMID) == -1)
			ate_printf(MSG_ERROR,"sem RMID failed\n");
		if((shmdt(priv_data))==-1)
			ate_printf(MSG_ERROR,"detaching shared memory failed\n");
		if(shmctl(shm_id, 0 ,IPC_RMID) == -1)
			ate_printf(MSG_ERROR,"shm RMID failed\n");
		ate_printf(MSG_ERROR,"detaching shared memory success\n");
		exit(0);
	} else {
		kill(pid, SIGTERM);
	}
}
#else	/* For shm_open */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic)
{
	int tmp = 0;
	int pid = -1;
	int shm_fd;
	int shm_size = 0;
	void *shm = NULL;
	struct DRI_IF **a_ifs = (struct DRI_IF **)dri_if;
	struct DRI_IF *p_if = a_ifs[idx];
	struct MULTI_PROC_FORKS *priv_data = NULL;
	void (*handler)(void *arg) = pkt_proc_logic;	
	/* Allocate Share Memory */
	shm_fd = shm_open(SHM_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(shm_fd < 0){
		ate_printf(MSG_ERROR,"shm_fd allocation failed for size(%d), [%d]%s\n", sizeof(struct MULTI_PROC_FORKS), errno,strerror(errno));
		return -1;
	}

	shm_size =  sizeof(struct MULTI_PROC_FORKS);
	if(ftruncate(shm_fd, shm_size) != 0){
		ate_printf(MSG_ERROR,"shm_fd resize failed\n");
		return -1;
	}
	shm = mmap(NULL, shm_size, 
    			PROT_WRITE | PROT_READ, MAP_SHARED, 
    			shm_fd, 0);
	if((void *)-1 ==  shm){
		ate_printf(MSG_ERROR,"Share memory allocation failed with shmid: %d, [%d]%s\n", shm_fd, errno,strerror(errno));
		return -1;
	}
	priv_data = (struct MULTI_PROC_FORKS *)shm;
	/* Semaphore Init */
	tmp = sem_init(&priv_data->lock, 1, 1);
	if(tmp == -1){
		ate_printf(MSG_ERROR,"Semaphore allocation failed\n");
		return -1;
	}
	/* Queue Init */
	priv_data->q.un_served = 0;
	priv_data->q.served = 0;
	for(tmp=0;tmp<CMD_QUEUE_SIZE;tmp++){
		os_memset(priv_data->q.cmd_arr[tmp], 0, PKT_BUF_SIZE);
	}
	/* Ops Init */
	p_if->ops = &fork_ops;
	/* Fork Sub-process */
	p_if->priv_data = priv_data;
	pid = fork();
	
	switch(pid){
	case -1:
		ate_printf(MSG_ERROR,"Fork sub-process failed\n");
		break;
	case 0:
		ate_printf(MSG_DEBUG,"Fork sub-process success, pid:%d\n",getpid());
		priv_data->pid = getpid();
		if(pkt_proc_logic != NULL)
			handler(p_if);
		ate_printf(MSG_DEBUG,"Fork sub-process prepares leaving, pid:%d\n",priv_data->pid);
		exit(0);
		break;
	default:
		break;
	}
	return 0;
}
#if 0
int fork_insert_q(void *dri_if)
{
}
#endif

int fork_lock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	if(sem_wait(&priv_data->lock) != 0)
		ate_printf(MSG_ERROR,"Lock failed\n");
//	ate_printf(MSG_DEBUG,"Queue locked\n");
	return 0;
}

int fork_unlock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	if(sem_post(&priv_data->lock) != 0)
		ate_printf(MSG_ERROR,"Unlock failed\n");
//	ate_printf(MSG_DEBUG,"Queue un-locked\n");
	
	return 0;
}

int fork_wait_data(void *dri_if)
{
	fork_unlock_q(dri_if);
	usleep(WAIT_CMD_DELAY);
	fork_lock_q(dri_if);
	return 0;
}

int fork_sig_data(void *dri_if)
{
	return 1;	/* For nothing is serving */
}


int fork_close(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_FORKS *priv_data = p_if->priv_data;
	int shm_fd = priv_data->shm_fd;
	int pid = priv_data->pid;
	int my_pid = getpid();
	int ret;
	if(my_pid == pid) {
		ate_printf(MSG_DEBUG,"Forked sub-process is terminating at (%s)\n",p_if->ifname);
		sem_destroy(&priv_data->lock);
		ret = munmap(priv_data, sizeof(struct MULTI_PROC_FORKS));
		ret = shm_unlink(SHM_FILE);
		exit(0);
	} else {
		kill(pid, SIGTERM);
	}
}
#endif

int fork_insert_q(void *dri_if, unsigned char *pkt, int len)
{
	struct DRI_IF *p_if = dri_if;
	struct COMMON_PRIV *priv_data;
	struct cmd_queue *q;
	priv_data = (struct COMMON_PRIV *)p_if->priv_data;
	q = &priv_data->q;
	os_memcpy(q->cmd_arr[q->un_served],pkt,len);
	return len;
}
