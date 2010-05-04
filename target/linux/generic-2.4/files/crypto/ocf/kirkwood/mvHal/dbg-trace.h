
#ifndef _MV_DBG_TRCE_H_
#define _MV_DBG_TRCE_H_

#ifdef CONFIG_MV_DBG_TRACE
void TRC_INIT(void *callback1, void *callback2, 
    unsigned char callback1_param, unsigned char callback2_param);
void TRC_REC(char *fmt,...);
void TRC_OUTPUT(void);
void TRC_RELEASE(void);
void TRC_START(void);
void TRC_STOP(void);

#else
#define TRC_INIT(x1,x2,x3,x4)
#define TRC_REC(X...)
#define TRC_OUTPUT()
#define TRC_RELEASE()
#define TRC_START()
#define TRC_STOP()
#endif


#endif
