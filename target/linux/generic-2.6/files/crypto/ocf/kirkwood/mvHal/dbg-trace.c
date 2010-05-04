#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/time.h>
#include "dbg-trace.h"

#define TRACE_ARR_LEN   800
#define STR_LEN         128
struct trace {
    struct timeval tv;
        char str[STR_LEN];
    unsigned int callback_val1;
    unsigned int callback_val2;
        char valid;
};
static unsigned int (*trc_callback1) (unsigned char) = NULL;
static unsigned int (*trc_callback2) (unsigned char) = NULL;
static unsigned char trc_param1 = 0;
static unsigned char trc_param2 = 0;
struct trace *trc_arr;
static int trc_index;
static int trc_active = 0;

void TRC_START()
{
    trc_active = 1;
}

void TRC_STOP()
{
    trc_active = 0;
}

void TRC_INIT(void *callback1, void *callback2, unsigned char callback1_param, unsigned char callback2_param)
{
    printk("Marvell debug tracing is on\n");
        trc_arr = (struct trace *)kmalloc(TRACE_ARR_LEN*sizeof(struct trace),GFP_KERNEL);
    if(trc_arr == NULL)
    {
        printk("Can't allocate Debug Trace buffer\n");
        return;
    }
        memset(trc_arr,0,TRACE_ARR_LEN*sizeof(struct trace));
        trc_index = 0;
    trc_callback1 = callback1;
    trc_callback2 = callback2;
    trc_param1 = callback1_param;
    trc_param2 = callback2_param;
}
void TRC_REC(char *fmt,...)
{
    va_list args;
        struct trace *trc = &trc_arr[trc_index];

    if(trc_active == 0)
        return;

    do_gettimeofday(&trc->tv);
    if(trc_callback1)
        trc->callback_val1 = trc_callback1(trc_param1);
    if(trc_callback2)
        trc->callback_val2 = trc_callback2(trc_param2);
    va_start(args, fmt);
    vsprintf(trc->str,fmt,args);
    va_end(args);
        trc->valid = 1;
        if((++trc_index) == TRACE_ARR_LEN) {
                trc_index = 0;
    }
}
void TRC_OUTPUT(void)
{
        int i,j;
        struct trace *p;
        printk("\n\nTrace %d items\n",TRACE_ARR_LEN);
        for(i=0,j=trc_index; i<TRACE_ARR_LEN; i++,j++) {
                if(j == TRACE_ARR_LEN)
                        j = 0;
                p = &trc_arr[j];
                if(p->valid) {
            unsigned long uoffs;
            struct trace *plast;
            if(p == &trc_arr[0])
                plast = &trc_arr[TRACE_ARR_LEN-1];
            else
                plast = p-1;
            if(p->tv.tv_sec == ((plast)->tv.tv_sec))
                uoffs = (p->tv.tv_usec - ((plast)->tv.tv_usec));
            else
                uoffs = (1000000 - ((plast)->tv.tv_usec)) +
                    ((p->tv.tv_sec - ((plast)->tv.tv_sec) - 1) * 1000000) + 
                    p->tv.tv_usec;
                        printk("%03d: [+%ld usec]", j, (unsigned long)uoffs);
            if(trc_callback1)
                printk("[%u]",p->callback_val1);
            if(trc_callback2)
                printk("[%u]",p->callback_val2);
            printk(": %s",p->str);
        }
                p->valid = 0;
        }
        memset(trc_arr,0,TRACE_ARR_LEN*sizeof(struct trace));
        trc_index = 0;
}
void TRC_RELEASE(void)
{
        kfree(trc_arr);
        trc_index = 0;
}


