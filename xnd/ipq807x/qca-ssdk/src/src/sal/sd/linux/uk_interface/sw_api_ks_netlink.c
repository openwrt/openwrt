/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */



#include "sw.h"
#include "sw_api.h"

#ifdef KVER26 /*Linux Kernel 2.6 */
#define __USER     __user
#else /*Linux Kernel 2.4 */
#include <asm/uaccess.h>
#define __USER
#define CLONE_KERNEL    (CLONE_FS | CLONE_FILES | CLONE_SIGHAND)
#define for_each_process(p) for_each_task(p)
#endif /*KVER26 */
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/version.h>
#include "api_access.h"
#include "sw_api_ks.h"

#if 0
#define dprintk(args...) aos_printk(args)
#else
#define dprintk(args...)
#endif

/*configurable value for max creating request of kernel thread*/
#define PID_THREADS_MAX  32

#define RSV_PID_LOC_0 (0)
#define RSV_PID_LOC_1 (1)

#define PID_TAB_MAX         PID_THREADS_MAX
#define PID_TAB_NOT_FOUND   PID_TAB_MAX+1

static pid_t pid_parents[PID_TAB_MAX] = {0};
static pid_t pid_childs[PID_TAB_MAX] = {0};
static wait_queue_head_t pid_child_wait[PID_TAB_MAX];
static struct semaphore pid_tab_sem;

static unsigned long *cmd_buf = NULL;
static struct semaphore api_sem;
static struct sock *ssdk_nl_sk = NULL;
static struct sk_buff * skb_array[PID_TAB_MAX] = {0};

static sw_error_t
input_parser(sw_api_param_t *p, a_uint32_t nr_param, a_uint32_t *args)
{
    a_uint32_t i = 0, buf_head = nr_param;

    for (i = 0; i < nr_param; i++)
    {
        if (p->param_type & SW_PARAM_PTR)
        {
            cmd_buf[i] = (a_uint32_t) & cmd_buf[buf_head];
            buf_head += (p->data_size + 3) / 4;

            if (buf_head > (SW_MAX_API_BUF / 4))
                return SW_NO_RESOURCE;

            if (p->param_type & SW_PARAM_IN)
            {
                if (copy_from_user((a_uint8_t*)(cmd_buf[i]), (void __USER *)args[i + 2], ((p->data_size + 3) >> 2) << 2))
                    return SW_NO_RESOURCE;
            }
        }
        else
        {
            cmd_buf[i] = args[i + 2];
        }
        p++;
    }
    return SW_OK;
}

static sw_error_t
output_parser(sw_api_param_t *p, a_uint32_t nr_param, a_uint32_t *args)
{
    a_uint32_t i =0;

    for (i = 0; i < nr_param; i++)
    {
        if (p->param_type & SW_PARAM_OUT)
        {
            if (copy_to_user
                    ((void __USER *) args[i + 2], (a_uint32_t *) cmd_buf[i], ((p->data_size + 3) >> 2) << 2))
                return SW_NO_RESOURCE;
        }
        p++;
    }

    return SW_OK;
}

static sw_error_t
sw_api_cmd(a_uint32_t * args)
{
    a_uint32_t *p = cmd_buf, api_id = args[0], nr_param = 0;
    sw_error_t(*func) (a_uint32_t, ...);
    sw_api_param_t *pp;
    sw_api_func_t *fp;
    sw_error_t rv;
    sw_api_t sw_api;

    down(&api_sem);

    sw_api.api_id = api_id;
    rv = sw_api_get(&sw_api);
    SW_OUT_ON_ERROR(rv);

    fp = sw_api.api_fp;
    pp = sw_api.api_pp;
    nr_param = sw_api.api_nr;

    rv = input_parser(pp, nr_param, args);
    SW_OUT_ON_ERROR(rv);
    func = fp->func;

    switch (nr_param)
    {
        case 1:
            rv = (func) (p[0]);
            break;
        case 2:
            rv = (func) (p[0], p[1]);
            break;
        case 3:
            rv = (func) (p[0], p[1], p[2]);
            break;
        case 4:
            rv = (func) (p[0], p[1], p[2], p[3]);
            break;
        case 5:
            rv = (func) (p[0], p[1], p[2], p[3], p[4]);
            break;
        case 6:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5]);
            break;
        case 7:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
            break;
        case 8:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
            break;
        case 9:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]);
            break;
        case 10:
            rv = (func) (p[0], p[1], p[2], p[3], p[4], p[5],
                         p[6], p[7], p[8], p[9]);
            break;
        default:
            rv = SW_OUT_OF_RANGE;
    }

    SW_OUT_ON_ERROR(rv);
    rv = output_parser(pp, nr_param, args);

out:
    up(&api_sem);
    return rv;
}

static inline int pid_find(pid_t pid, pid_t pids[])
{
    a_uint32_t i, loc = PID_TAB_NOT_FOUND;

    for(i = 0; i< PID_TAB_MAX; i++)
    {
        if(pids[i] == pid)
        {
            loc = i;
            break;
        }
    }
    return loc;
}

static inline a_bool_t  pid_exit(pid_t parent_pid)
{
    struct task_struct *p;
    a_bool_t rtn = A_TRUE;

    for_each_process(p)
    {
        if(parent_pid == p->pid)
        {
            rtn = A_FALSE;
            break;
        }
    }

    return rtn;
}

static inline void pid_free(a_uint32_t loc)
{
    if (down_interruptible(&pid_tab_sem))
        return;

    pid_childs[loc] = 0;
    pid_parents[loc] = 0;

    up(&pid_tab_sem);
}

static inline a_bool_t pid_full(void)
{
    return (pid_find(0, pid_parents) == PID_TAB_NOT_FOUND)?A_TRUE:A_FALSE;
}

static a_uint32_t pid_find_save (pid_t parent_pid, pid_t child_pid)
{
    a_uint32_t loc = PID_TAB_NOT_FOUND;

    if(!parent_pid && !child_pid)
    {
        dprintk("child and father can't both zero\n");
        return loc;
    }

    if (down_interruptible(&pid_tab_sem))
        return loc;

    if(!parent_pid)
    {
        /*find locate by child_pid*/
        loc = pid_find(child_pid, pid_childs);

    }
    else
    {
        /*find locate by parent_pid*/
        loc = pid_find(parent_pid, pid_parents);

        if(child_pid)
        {
            loc = pid_find(0, pid_parents);

            if(loc != PID_TAB_NOT_FOUND)
            {
                pid_childs[loc] = child_pid;
                pid_parents[loc] = parent_pid;
            }
        }
    }

    up(&pid_tab_sem);
    return loc;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
static void
sw_api_excep_ack(struct sock *sk, pid_t pid)
{
    sw_error_t rv = SW_NO_RESOURCE;
    a_uint32_t args[SW_MAX_API_PARAM], rtn;
    struct sk_buff *skb, *skb_first = NULL;
    struct nlmsghdr *nlh = NULL;

    while(1)
    {
#ifdef KVER26
        skb = skb_dequeue(&sk->sk_receive_queue);
#else
        skb = skb_dequeue(&sk->receive_queue);
#endif
        if (!skb)
        {
            dprintk("pid error: skb = null\n");
            return;
        }

        nlh = (struct nlmsghdr *)skb->data;
        if (!nlh)
        {
            dprintk("pid error: nlh = null\n");
            return;
        }

        if(nlh->nlmsg_pid == pid)
        {
            break;
        }

        if(!skb_first)
        {
            skb_first = skb;
        }
        else if (skb_first == skb)
        {
            dprintk("can't found my skb???\n");
            return;
        }

#ifdef KVER26
        skb_queue_tail(&sk->sk_receive_queue, skb);
#else
        skb_queue_tail(&sk->receive_queue, skb);
#endif
    }

	if(nlh->nlmsg_len < (SW_MAX_PAYLOAD + sizeof(nlmsghdr)))
	{
		dprintk("data length is less than %d bytes\n", SW_MAX_PAYLOAD);
		SW_OUT_ON_ERROR(SW_ABORTED);
	}
    aos_mem_copy(args, NLMSG_DATA(nlh), SW_MAX_PAYLOAD);
    /* return API result to user */
    rtn = (a_uint32_t) rv;
    if (copy_to_user
            ((void __USER *) args[1], (a_uint32_t *) & rtn, sizeof (a_uint32_t)))
    {
        rv = SW_NO_RESOURCE;
    }

    NETLINK_CB(skb).pid = 0;
    NETLINK_CB(skb).dst_pid = nlh->nlmsg_pid;
#ifdef KVER26
    NETLINK_CB(skb).dst_group = 0;
#else
    NETLINK_CB(skb).dst_groups = 0;
#endif

    netlink_unicast(sk, skb, nlh->nlmsg_pid, MSG_DONTWAIT);
}

static void
sw_api_exec(struct sock *sk, pid_t pid)
{
    sw_error_t rv = SW_NO_RESOURCE;
    a_uint32_t args[SW_MAX_API_PARAM], rtn;
    struct sk_buff *skb, *skb_first = NULL;
    struct nlmsghdr *nlh = NULL;

    while(1)
    {
#ifdef KVER26
        skb = skb_dequeue(&sk->sk_receive_queue);
#else
        skb = skb_dequeue(&sk->receive_queue);
#endif
        if (!skb)
        {
            dprintk("pid error: skb = null\n");
            return;
        }

        nlh = (struct nlmsghdr *)skb->data;
        if (!nlh)
        {
            dprintk("pid error: nlh = null\n");
            return;
        }

        if(nlh->nlmsg_pid == pid)
        {
            break;
        }

        if(!skb_first)
        {
            skb_first = skb;
        }
        else if (skb_first == skb)
        {
            dprintk("can't found my skb???\n");
            return;
        }

#ifdef KVER26
        skb_queue_tail(&sk->sk_receive_queue, skb);
#else
        skb_queue_tail(&sk->receive_queue, skb);
#endif
    }

	if(nlh->nlmsg_len < (SW_MAX_PAYLOAD + sizeof(nlmsghdr)))
	{
		dprintk("data length is less than %d bytes\n", SW_MAX_PAYLOAD);
		SW_OUT_ON_ERROR(SW_ABORTED);
	}
    aos_mem_copy(args, NLMSG_DATA(nlh), SW_MAX_PAYLOAD);

    rv = sw_api_cmd(args);
    /* return API result to user */
    rtn = (a_uint32_t) rv;
    if (copy_to_user
            ((void __USER *) args[1], (a_uint32_t *) & rtn, sizeof (a_uint32_t)))
    {
        rv = SW_NO_RESOURCE;
    }

    NETLINK_CB(skb).pid = 0;
    NETLINK_CB(skb).dst_pid = nlh->nlmsg_pid;
#ifdef KVER26
    NETLINK_CB(skb).dst_group = 0;
#else
    NETLINK_CB(skb).dst_groups = 0;
#endif

    netlink_unicast(sk, skb, nlh->nlmsg_pid, MSG_DONTWAIT);
}


static int sw_api_thread(void *sk)
{
    a_uint32_t loc, i;
    pid_t parent_pid = 0, child_pid = current->pid;

    while ((loc = pid_find_save(parent_pid, child_pid)) == PID_TAB_NOT_FOUND)
        schedule_timeout(1*HZ);

    parent_pid = pid_parents[loc];
    dprintk("thread child [%d] find parent [%d] at %d \n", child_pid, parent_pid, loc);

    if ((RSV_PID_LOC_0 == loc) || (RSV_PID_LOC_1 == loc))
    {
        for(i=0; ; i++)
        {
            if(i && !sleep_on_timeout(&pid_child_wait[loc], (5*HZ)))
            {
                if(pid_exit(parent_pid) == A_FALSE)
                    continue;

                pid_free(loc);
                dprintk("thread child[%d] exit!\n", child_pid);
                return 0;
            }

            sw_api_exec(sk, parent_pid);
        }
    }
    else
    {
        sw_api_exec(sk, parent_pid);
        pid_free(loc);
    }

    return 0;
}

static void
sw_api_netlink(struct sock *sk, int len)
{
    pid_t parent_pid = current->pid, child_pid = 0;
    a_uint32_t loc = pid_find_save (parent_pid, child_pid);

    if(loc == PID_TAB_NOT_FOUND)
    {
        if(pid_full())
        {
            dprintk("###threads exceed the max [%d] for pid [%d]!###\n", PID_TAB_MAX, parent_pid);
            sw_api_excep_ack(sk, parent_pid);
            return;
        }
#if 1
        struct task_struct *p;
        p = kthread_create(sw_api_thread, (void *)ssdk_nl_sk, "netlink_child");
        if (IS_ERR(p))
        {
            dprintk("thread can't be created for netlink\n");
            return;
        }
        child_pid = p->pid;
#else
        if ((child_pid = kernel_thread(sw_api_thread, ssdk_nl_sk, CLONE_KERNEL)) < 0)
        {
            dprintk("thread can't be created for netlink\n");
            return;
        }
#endif
        dprintk("[%d] create child [%d] at %d\n", parent_pid, child_pid, loc);
        pid_find_save(parent_pid, child_pid);
        wake_up_process(p);
    }
    else
    {
        dprintk("[%d] wake up child [%d] at %d\n", parent_pid, pid_childs[loc], loc);
        wake_up(&pid_child_wait[loc]);
    }

    return;

}

#else

static void
sw_api_excep_ack_26_22(struct sk_buff *skb)
{
    sw_error_t rv = SW_NO_RESOURCE;
    a_uint32_t args[SW_MAX_API_PARAM], rtn, size, dst_pid;
    struct nlmsghdr *nlh = NULL;
    struct sk_buff *rep;

    nlh = (struct nlmsghdr *)skb->data;
    if (!nlh)
    {
        dprintk("pid error: nlh = null\n");
        return;
    }
    dst_pid = nlh->nlmsg_pid;

	if(nlh->nlmsg_len < (SW_MAX_PAYLOAD + sizeof(nlmsghdr)))
	{
		dprintk("data length is less than %d bytes\n", SW_MAX_PAYLOAD);
		SW_OUT_ON_ERROR(SW_ABORTED);
	}
    aos_mem_copy(args, NLMSG_DATA(nlh), SW_MAX_PAYLOAD);
    /* return API result to user */
    rtn = (a_uint32_t) rv;
    if (copy_to_user
            ((void __USER *) args[1], (a_uint32_t *) & rtn, sizeof (a_uint32_t)))
    {
        rv = SW_NO_RESOURCE;
    }

    size = NLMSG_SPACE(0);
    rep = alloc_skb(size, GFP_ATOMIC);
    if (!rep)
    {
        dprintk("reply socket buffer allocation error... \n");
        return;
    }
    nlh = nlmsg_put(rep, 0, 0, 0, 0, 0);

    NETLINK_CB(rep).pid = 0;
    NETLINK_CB(rep).dst_group = 0;
    netlink_unicast(ssdk_nl_sk, rep, dst_pid, MSG_DONTWAIT);
}

static void
sw_api_exec_26_22(pid_t parent_pid)
{
    sw_error_t rv = SW_NO_RESOURCE;
    a_uint32_t loc, args[SW_MAX_API_PARAM], rtn, skblen, nlmsglen, size, dst_pid;
    struct nlmsghdr *nlh = NULL;
    struct sk_buff *skb;
    struct sk_buff *rep;

    loc = pid_find(parent_pid, pid_parents);
    if (PID_TAB_NOT_FOUND == loc)
    {
        dprintk("parent PID not found - (%d)\n", parent_pid);
        return;
    }

    skb = skb_array[loc];
    if (!skb)
    {
        dprintk("skb null pointer error\n");
        return;
    }

    skblen = skb->len;
    if (skb->len < sizeof(nlh))
    {
        dprintk("skb len error - (%d)\n", skb->len);
        SW_OUT_ON_ERROR(SW_ABORTED);
    }

    nlh = (struct nlmsghdr *)skb->data;
    if (!nlh)
    {
        dprintk("pid error: nlh = null\n");
        SW_OUT_ON_ERROR(SW_ABORTED);
    }

    nlmsglen = nlh->nlmsg_len;
    if (nlmsglen < sizeof(*nlh) || skblen < nlmsglen)
    {
        dprintk("nlmsglen error - (%d)\n", nlmsglen);
        SW_OUT_ON_ERROR(SW_ABORTED);
    }
    dst_pid = nlh->nlmsg_pid;

	if(nlmsglen < (SW_MAX_PAYLOAD + sizeof(nlmsghdr)))
	{
		dprintk("data length is less than %d bytes\n", SW_MAX_PAYLOAD);
		SW_OUT_ON_ERROR(SW_ABORTED);
	}
    aos_mem_copy(args, NLMSG_DATA(nlh), SW_MAX_PAYLOAD);
    rv = sw_api_cmd(args);

    /* return API result to user */
    rtn = (a_uint32_t) rv;
    if (copy_to_user
            ((void __USER *) args[1], (a_uint32_t *) & rtn, sizeof (a_uint32_t)))
    {
        rv = SW_NO_RESOURCE;
    }

    size = NLMSG_SPACE(0);
    rep = alloc_skb(size, GFP_ATOMIC);
    if (!rep)
    {
        dprintk("reply socket buffer allocation error... \n");
        SW_OUT_ON_ERROR(SW_ABORTED);
    }
    nlh = nlmsg_put(rep, 0, 0, 0, 0, 0);

    NETLINK_CB(rep).pid = 0;
    NETLINK_CB(rep).dst_group = 0;
    netlink_unicast(ssdk_nl_sk, rep, dst_pid, MSG_DONTWAIT);

out:
    skb_array[loc] = NULL;
    kfree_skb(skb);
}

static int
sw_api_thread_26_22(void * data)
{
    a_uint32_t loc, i;
    pid_t parent_pid = 0, child_pid = current->pid;

    while ((loc = pid_find_save(parent_pid, child_pid)) == PID_TAB_NOT_FOUND)
        schedule_timeout(1*HZ);

    parent_pid = pid_parents[loc];
    dprintk("thread child [%d] find parent [%d] at %d \n", child_pid, parent_pid, loc);

    if ((RSV_PID_LOC_0 == loc) || (RSV_PID_LOC_1 == loc))
    {
        for(i=0; ; i++)
        {
            if(i && !sleep_on_timeout(&pid_child_wait[loc], (5*HZ)))
            {
                if(pid_exit(parent_pid) == A_FALSE)
                    continue;

                pid_free(loc);
                dprintk("thread child[%d] exit!\n", child_pid);
                return 0;
            }

            sw_api_exec_26_22(parent_pid);
        }
    }
    else
    {
        sw_api_exec_26_22(parent_pid);
        pid_free(loc);
    }

    return 0;
}

static void
sw_api_netlink_26_22(struct sk_buff *skb)
{
    pid_t parent_pid = current->pid, child_pid = 0;
    a_uint32_t loc = pid_find_save (parent_pid, child_pid);

    if(loc == PID_TAB_NOT_FOUND)
    {
        if(pid_full())
        {
            dprintk("###threads exceed the max [%d] for pid [%d]!###\n", PID_TAB_MAX, parent_pid);
            sw_api_excep_ack_26_22(skb);
            return;
        }

        loc = pid_find_save(parent_pid, 0xffffffff);

#if 1
        struct task_struct *p;
        p = kthread_create(sw_api_thread_26_22, (void *)ssdk_nl_sk, "netlink_child");
        if (IS_ERR(p))
        {
            dprintk("thread can't be created for netlink\n");
            return;
        }

        skb_array[loc] = skb_get(skb);
        child_pid = p->pid;
        pid_childs[loc] = child_pid;
        wake_up_process(p);
#else
        if ((child_pid = kernel_thread(sw_api_thread_26_22, NULL, CLONE_KERNEL)) < 0)
        {
            dprintk("thread can't be created for netlink\n");
            return;
        }
#endif
        dprintk("[%d] create child [%d] at %d\n", parent_pid, child_pid, loc);
    }
    else
    {
        dprintk("[%d] wake up child [%d] at %d\n", parent_pid, pid_childs[loc], loc);
        skb_array[loc] = skb_get(skb);
        wake_up(&pid_child_wait[loc]);
    }

    return;
}
#endif

sw_error_t
sw_uk_init(a_uint32_t nl_prot)
{
    a_uint32_t i, protocol;

    if (!cmd_buf)
    {
        if((cmd_buf = (a_uint32_t *) aos_mem_alloc(SW_MAX_API_BUF)) == NULL)
            return SW_OUT_OF_MEM;
    }

    if (!ssdk_nl_sk)
    {
#if defined UK_NL_PROT
        protocol = UK_NL_PROT;
#else
        protocol = nl_prot;
#endif

#ifdef KVER26
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
        ssdk_nl_sk = netlink_kernel_create(&init_net, protocol, 0, sw_api_netlink_26_22, NULL, THIS_MODULE);
#else
        ssdk_nl_sk = netlink_kernel_create(protocol, 0, sw_api_netlink, THIS_MODULE);
#endif
#else
        ssdk_nl_sk = netlink_kernel_create(protocol, sw_api_netlink);
#endif
        if (!ssdk_nl_sk)
        {
            dprintk("netlink_kernel_create fail at nl_prot:[%d]\n", protocol);
            return SW_NO_RESOURCE;
        }
        else
        {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)
            dprintk("netlink_kernel_create succeeded at nl_prot: [%d] (>2.6.22)\n", protocol);
#else
            dprintk("netlink_kernel_create succeeded at nl_prot: [%d] (<2.6.22)\n", protocol);
#endif
        }
    }

    init_MUTEX(&pid_tab_sem);
    init_MUTEX(&api_sem);

    for(i = 0; i < PID_TAB_MAX; i++)
    {
        init_waitqueue_head(&pid_child_wait[i]);
    }

    return SW_OK;
}

sw_error_t
sw_uk_cleanup(void)
{
    if (cmd_buf)
    {
        aos_mem_free(cmd_buf);
        cmd_buf = NULL;
    }

    if (ssdk_nl_sk)
    {
#ifdef KVER26
        sock_release(ssdk_nl_sk->sk_socket);
#else
        sock_release(ssdk_nl_sk->socket);
#endif
        ssdk_nl_sk = NULL;
    }

    return SW_OK;
}

