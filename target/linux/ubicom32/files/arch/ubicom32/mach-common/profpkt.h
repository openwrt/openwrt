
/*
 * arch/ubicom32/mach-common/profpkt.c
 *   Ubicom32 Profiler packet formats for communication between the linux proc driver and the profiler display tool
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

#define PROFILE_PORT 51080
#define PROFILE_POSIX_NAME_LENGTH 32

/*
 * profile UDP packet format for communicating between ip3k and host
 *
 * every packet starts with a header, followed by samples.
 * samples are only taken for non-hrt threads that are
 * active
 */
#define PROF_MAGIC 0x3ea0
#define PROF_MAGIC_COUNTERS 0x9ea0
#define PROF_MAGIC_MAPS 0xaea0

/*
 * Versions (31 max):
 * 1 to 4 were before 6.0 release,  development versions
 * 5 was forward compatible version, shipped with 6.0 and 6.1
 * 6 adds heap packets, and clock_freq to header, shipped with 6.2
 * 7 adds a sequence numbers to check for dropped packets, shipped with 6.3.5
 * 8 adds mqueue timing information, shipped with 6.3.5
 * 9 adds sdram heap size information, shipped with 6.4
 * 10 adds heapmem heap callers and long latency stack traces.  shipped with 6.4
 * 11 adds support for Mars (IP5K).  shipped with 6.10
 * 12 adds more support for Mars.  Shipped with 7.0
 * 13 adds per sample latency measurement.  Shipped with 7.2
 * 14 changes the heap format and adds a string packet.  Shipped with 7.4
 * 15 adds dsr stats and posix.  shipped with 7.6
 * 16 corrects maximum packet count for Ares.  ships with 7.9
 * 17 adds a5 register value to sample
 */

#define PROFILE_VERSION 17
#define PROFILE_MAX_PACKET_SIZE 1440

#define PROFILE_MAX_THREADS 16

/*
 * each packet starts with a profile_header, then sample_count samples
 * samples are gprof samples of pc, the return address, condition codes, and
 * active threads
 */
struct profile_header {
	u16_t magic;			/* magic number and version */
	u8_t header_size;		/* number of bytes in profile header */
	u8_t sample_count;		/* number of samples in the packet */
	u32_t clocks;			/* clock counter value */
	u32_t instruction_count[PROFILE_MAX_THREADS];
					/* instructions executed per thread */
	u32_t profile_instructions;	/* instructions executed by profiler mainline */
	u16_t enabled;			/* which threads are enabled */
	u16_t hrt;			/* which threads are hrt */
	u16_t high;			/* which threads are high priority */
	u16_t profiler_thread;		/* which thread runs the profiler */
	u32_t heap_free;		/* current free on-cihp heap space in bytes */
	u32_t heap_low_water;		/* on-chip heap low water mark */
	u32_t netpage_free;		/* number of free on-chip net pages */
	u32_t netpage_low_water;	/* low water mark on free on-chip netpages */
	u32_t min_sp[PROFILE_MAX_THREADS];
					/* stack pointer values per thread */
	u32_t clock_freq;		/* clock frequency (Hz) of system being analyzed */
	u32_t seq_num;			/* to detect dropped profiler packets */
	u32_t timing_sequence;		/* sample number since boot */
	u32_t timing_interval;		/* second per sample timing interval */
	u32_t timing_worst_time;	/* duration of longest finction called, in core clocks */
	u32_t timing_function;		/* address of longest function */
	u32_t timing_average;		/* average time of all functions in last interval */
	u32_t timing_count;		/* number of functions called in last interval */
	u32_t extheap_free;		/* current free extmem heap space in bytes */
	u32_t extheap_low_water;	/* extmem heap low water mark */
	u32_t cpu_id;			/* CHIP_ID register contents */
	u32_t perf_counters[4];		/* contents of the CPU performance counters */
	u8_t perf_config[4];		/* what is being counted */
	u32_t ddr_freq;			/* DDR clock frequency */
	u32_t extnetpage_free;		/* number of free off chip net pages */
	u32_t extnetpage_low_water;	/* low water mark on off-chip free netpages */
	u32_t dsr_max_latency;		/* max time to process a dsr interrupt, in clocks, since last packet */
	u32_t dsr_ave_latency;		/* average dsr latency over last DSR_STATS_RECENT_COUNT interrupts */
	u32_t dsr_count;		/* number of dsr interrupts since last packet */
};

struct profile_header_counters {
	u16_t magic;
	u16_t ultra_count;		/* how many ultra counters follow this */
	u32_t ultra_sample_time;	/* in chip clocks */
	u32_t linux_count;		/* how many linux counters follow this */
	u32_t linux_sample_time;
};

/*
 * values chosen so all counter values fit in a single 1400 byte UDP packet
 */
#define PROFILE_COUNTER_NAME_LENGTH 20
#define PROFILE_MAX_COUNTERS ((PROFILE_MAX_PACKET_SIZE - sizeof(struct profile_header_counters)) / (PROFILE_COUNTER_NAME_LENGTH + 4))

struct profile_counter_pkt {
	char name[PROFILE_COUNTER_NAME_LENGTH];
	unsigned int value;
};

/*
 * send memory maps from linux to profiler tool
 */

struct profile_header_maps {
	u16_t magic;
	u16_t count;
	u32_t page_shift;
};

#define PROFILE_MAP_NUM_TYPES 32

/* types 0-15: size field is order.  True size is 2^order */
#define PROFILE_MAP_TYPE_UNKNOWN 0
#define PROFILE_MAP_TYPE_FREE 1
#define PROFILE_MAP_TYPE_SMALL 2
#define PROFILE_MAP_TYPE_FS 3
/* types 16-31: size field is pages.  True size is (1 << PAGE_SHIFT) * size */
#define PROFILE_MAP_SIZE_TYPE 16
#define PROFILE_MAP_TYPE_TEXT 16
#define PROFILE_MAP_TYPE_STACK 17
#define PROFILE_MAP_TYPE_APP_DATA 18
#define PROFILE_MAP_TYPE_CACHE 19
#define PROFILE_MAP_RESERVED 24

#define PROFILE_MAP_TYPE_SHIFT 11
#define PROFILE_MAP_SIZE_MASK 0x7ff

struct profile_map {
	u16_t start;		/* start page number of segment, relative to start of DRAM */
	u16_t type_size;	/* type (4 bits) of the segment and size in pages (12 bits) */
};

#define PROFILE_MAX_MAPS (PROFILE_MAX_PACKET_SIZE - sizeof(struct profile_header_maps)) / sizeof(struct profile_map)
