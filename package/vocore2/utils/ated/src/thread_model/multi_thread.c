#include "precomp.h"
/* Variables */
//static struct MULTI_PROC_THREADS *threads = NULL;
//static struct MULTI_PROC_THREADS **a_threads;
struct MULTI_PROC_OPS thread_ops = {
	.multi_proc_init = multi_proc_init,
	.multi_insert_q = thread_insert_q,
	.multi_lock_q = thread_lock_q,
    .multi_unlock_q = thread_unlock_q,
	.multi_wait_data = thread_wait_data,
	.multi_sig_data = thread_sig_data,
	.multi_proc_close = thread_close,
};
/* Functions */
int multi_proc_init(void **dri_if, int idx, int dri_if_num, void *pkt_proc_logic)
{
	int ret = 0;
	int i = 0;
	struct DRI_IF **a_ifs = (struct DRI_IF **)dri_if;
	struct DRI_IF *p_if = a_ifs[idx];
	struct MULTI_PROC_THREADS *t = NULL;
	pthread_attr_t attr;
	if(t == NULL){
		t = malloc(sizeof(struct MULTI_PROC_THREADS));
		if(t == NULL){
			ate_printf(MSG_ERROR, "Cannot allocate memory for multi-threading\n");
			return -1;
		}
	//	a_threads = &threads;
	}
	/* Init parameter in Queue */
	t->q.un_served = 0;
	t->q.served = 0;
	for(i=0;i<CMD_QUEUE_SIZE;i++){
		os_memset(t->q.cmd_arr[i], 0, PKT_BUF_SIZE);
	}
	/* Lock Init */	
	pthread_cond_init(&t->cond, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_mutex_init(&t->lock,NULL)!=0){
		ate_printf(MSG_DEBUG, "Lock in messege queue is failed to initialize(Dri_if[%s])\n",p_if->ifname);
	}
	ate_printf(MSG_DEBUG, "Lock in messege queue is success to initialize(Dri_if[%s])\n",p_if->ifname);
	/* ptread init */
	ret = pthread_create(&t->thread, &attr, pkt_proc_logic, (void *)p_if);
	if(ret != 0){
		ate_printf(MSG_DEBUG, "Thread initial fail (%s)\n",p_if->ifname);
		return -1;		
	}
//	p_if->ops = &thread_ops;
	p_if->priv_data = t;
	return ret;
}

int thread_insert_q(void *dri_if, unsigned char *pkt, int len)
{
	struct DRI_IF *p_if = dri_if;
	struct COMMON_PRIV *priv_data;
	struct cmd_queue *q;
	priv_data = (struct COMMON_PRIV *)p_if->priv_data;
	q = &priv_data->q;
	os_memcpy(q->cmd_arr[q->un_served],pkt,len);
	return len;
}

int thread_lock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_THREADS *t_thread = (struct MULTI_PROC_THREADS *)p_if->priv_data;
	ate_printf(MSG_ERROR,"Lock\n");
	return pthread_mutex_lock(&t_thread->lock);
}

int thread_unlock_q(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_THREADS *t_thread = (struct MULTI_PROC_THREADS *)p_if->priv_data;
	ate_printf(MSG_ERROR,"Un-lock\n");
	return pthread_mutex_unlock(&t_thread->lock);
}

int thread_wait_data(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_THREADS *t_thread = (struct MULTI_PROC_THREADS *)p_if->priv_data;
	return pthread_cond_wait(&t_thread->cond, &t_thread->lock);
}

int thread_sig_data(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_THREADS *t_thread = (struct MULTI_PROC_THREADS *)p_if->priv_data;
	if(p_if->status == 0)
		return	pthread_cond_signal(&t_thread->cond);
	return 1;	/* For nothing is serving */
}

int thread_close(void *dri_if)
{
	struct DRI_IF *p_if = (struct DRI_IF *)dri_if;
	struct MULTI_PROC_THREADS *t_thread = (struct MULTI_PROC_THREADS *)p_if->priv_data;
	pthread_mutex_unlock(&t_thread->lock);
	pthread_mutex_destroy(&t_thread->lock);
	ate_printf(MSG_DEBUG,"Leave thread(%s)\n",p_if->ifname);
	return 0;
}
