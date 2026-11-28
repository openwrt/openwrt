/* SPDX-License-Identifier: GPL-2.0-only */
/* Annapurna labs resume address. */

#ifndef AL_CPU_RESUME_H_
#define AL_CPU_RESUME_H_

struct al_cpu_resume_regs_per_cpu {
	/* Flags */
	uint32_t	flags;

	/* Resume address */
	uintptr_t	resume_addr;
};

struct al_cpu_resume_regs {
	/* Watermark for validating the CPU resume struct */
	uint32_t watermark;

	/* Various flags to control the resume behavior */
	uint32_t flags;

	/* Per cpu regs */
	struct al_cpu_resume_regs_per_cpu per_cpu[];
};

/* The expected magic number for validating the resume addresses */
#define AL_CPU_RESUME_MAGIC_NUM		0xf0e1d200
#define AL_CPU_RESUME_MAGIC_NUM_MASK	0xffffff00

/* The expected minimal version number for validating the capabilities */
#define AL_CPU_RESUME_MIN_VER		0x000000c3
#define AL_CPU_RESUME_MIN_VER_MASK	0x000000ff

/* Don't init anything outside the core */
#define AL_CPU_RESUME_FLG_PERCPU_CLUSTER_SKIP     (2 << 0)

#endif /* AL_CPU_RESUME_H_ */
