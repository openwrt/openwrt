#ifndef _AR2313_MSG_H_
#define _AR2313_MSG_H_

#define AR2313_MTU                     1692
#define AR2313_PRIOS                   1
#define AR2313_QUEUES                  (2*AR2313_PRIOS)

#define AR2313_DESCR_ENTRIES           64

typedef struct {
    volatile unsigned int	status;		// OWN, Device control and status.
    volatile unsigned int	devcs;		// pkt Control bits + Length
    volatile unsigned int	addr;		// Current Address.
    volatile unsigned int	descr;		// Next descriptor in chain.
} ar2313_descr_t;

#endif /* _AR2313_MSG_H_ */
