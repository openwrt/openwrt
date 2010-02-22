#ifndef __IP_PROF_H__
#define __IP_PROF_H__

/* This number MUST match what is used in the ultra configuration! */
#define IPPROFILETIO_MAX_SAMPLES	600

/* Move to .h file used in both; avoid special types  */
struct profile_sample {
	unsigned int	pc;		/* PC value */
	unsigned int	parent;		/* a5 contents, to find the caller */
	unsigned char	cond_codes;	/* for branch prediction */
	unsigned char	thread;		/* I-blocked, D-blocked,
					   4-bit thread number */
	unsigned short	active;		/* which threads are active -
					   for accurate counting */
	unsigned short	blocked;	/* which threads are blocked due to
					   I or D cache misses */
	unsigned int	latency;	/* CPU clocks since the last message
					   dispatch in this thread
					   (thread 0 only for now) */
};


struct profilenode {
	struct devtree_node dn;
	volatile unsigned char enabled;	/* Is the tio enabled to
					   take samples? */
	volatile unsigned char busy;	/* set when the samples
					   are being read */
	volatile unsigned int mask;	/* Threads that change the MT_EN flag */
	volatile unsigned short rate;	/* What is the sampling rate? */
	volatile unsigned short head;	/* sample taker puts samples here */
	volatile unsigned short tail;	/* packet filler takes samples here */
	volatile unsigned short count;	/* number of valid samples */
	volatile unsigned short total;	/* Total samples */
	struct profile_sample samples[IPPROFILETIO_MAX_SAMPLES];
};

#endif
