/*
 *  arch/m68k/coldfire/entry.S
 *
 *  Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *  Matt Waddel Matt.Waddel@freescale.com
 *  Kurt Mahan kmahan@freescale.com
 *  Jason Jin Jason.Jin@freescale.com
 *  Shrek Wu B16972@freescale.com
 *
 * Based on:
 *
 *  arch/m68knommu/platform/5307/entry.S &
 *  arch/m68k/kernel/entry.S
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file README.legal in the main directory of this archive
 * for more details.
 */

#include <linux/sys.h>
#include <linux/linkage.h>
#include <asm/cf_entry.h>
#include <asm/errno.h>
#include <asm/setup.h>
#include <asm/segment.h>
#include <asm/traps.h>
#include <asm/unistd.h>

/*
 * TASK_INFO:
 *
 *  - TINFO_PREEMPT (struct thread_info / preempt_count)
 *      Used to keep track of preemptability
 *  - TINFO_FLAGS (struct thread_info / flags - include/asm-m68k/thread_info.h)
 *      Various bit flags that are checked for scheduling/tracing
 *	Bits 0-7  are checked every exception exit
 *	     8-15 are checked every syscall exit
 *
 *      TIF_SIGPENDING		6
 *      TIF_NEED_RESCHED	7
 *      TIF_DELAYED_TRACE	14
 *      TIF_SYSCALL_TRACE	15
 *      TIF_MEMDIE		16 (never checked here)
 */

.bss

sw_ksp:
.long	0

sw_usp:
.long	0

.text

.globl system_call
.globl buserr
.globl trap
.globl resume
.globl ret_from_exception
.globl ret_from_signal
.globl sys_call_table
.globl ret_from_interrupt
.globl inthandler

ENTRY(buserr)
#ifdef CONFIG_COLDFIRE_FOO
	movew	#0x2700,%sr		/* lock interrupts */
#endif
	SAVE_ALL_INT
#ifdef	CONFIG_COLDFIRE_FOO
	movew	PT_SR(%sp),%d3		/* get original %sr */
	oril	#0x2000,%d3		/* set supervisor mode in it */
	movew	%d3,%sr			/* recover irq state */
#endif
	GET_CURRENT(%d0)
	movel   %sp,%sp@-		/* stack frame pointer argument */
	jsr     buserr_c
	addql   #4,%sp
	jra     .Lret_from_exception

ENTRY(trap)
	SAVE_ALL_INT
	GET_CURRENT(%d0)
	movel   %sp,%sp@-		/* stack frame pointer argument */
	jsr     trap_c
	addql   #4,%sp
	jra     .Lret_from_exception

	/* After a fork we jump here directly from resume,
	   %d1 contains the previous task schedule_tail */
ENTRY(ret_from_fork)
	movel   %d1,%sp@-
	jsr     schedule_tail
	addql   #4,%sp
	jra     .Lret_from_exception

do_trace_entry:
	movel	#-ENOSYS,%d1		/* needed for strace */
	movel	%d1,%sp@(PT_D0)
	subql	#4,%sp
	SAVE_SWITCH_STACK
	jbsr	syscall_trace
	RESTORE_SWITCH_STACK
	addql	#4,%sp
	movel	%sp@(PT_ORIG_D0),%d0
	cmpl	#NR_syscalls,%d0
	jcs	syscall
badsys:
	movel	#-ENOSYS,%d1
	movel	%d1,%sp@(PT_D0)
	jra	ret_from_exception

do_trace_exit:
	subql	#4,%sp
	SAVE_SWITCH_STACK
	jbsr	syscall_trace
	RESTORE_SWITCH_STACK
	addql	#4,%sp
	jra	.Lret_from_exception

ENTRY(ret_from_signal)
	RESTORE_SWITCH_STACK
	addql	#4,%sp
	jra	.Lret_from_exception

ENTRY(system_call)
	SAVE_ALL_SYS

	GET_CURRENT(%d1)
	/* save top of frame */
	movel	%sp,%curptr@(TASK_THREAD+THREAD_ESP0)

	/* syscall trace */
	tstb	%curptr@(TASK_INFO+TINFO_FLAGS+2)
	jmi	do_trace_entry		/* SYSCALL_TRACE is set */
	cmpl	#NR_syscalls,%d0
	jcc	badsys
syscall:
	movel	#sys_call_table,%a0
	asll	#2,%d0
	addl	%d0,%a0
	movel	%a0@,%a0
	jsr	%a0@
	movel	%d0,%sp@(PT_D0)		/* save the return value */
ret_from_syscall:
	movew	%curptr@(TASK_INFO+TINFO_FLAGS+2),%d0
	jne	syscall_exit_work	/* flags set so process */
1:	RESTORE_ALL

syscall_exit_work:
	btst	#5,%sp@(PT_SR)		/* check if returning to kernel */
	bnes	1b			/* if so, skip resched, signals */

	btstl	#15,%d0			/* check if SYSCALL_TRACE */
	jne	do_trace_exit
	btstl	#14,%d0			/* check if DELAYED_TRACE */
	jne	do_delayed_trace
	btstl	#6,%d0			/* check if SIGPENDING */
	jne	do_signal_return
	pea	resume_userspace
	jra	schedule

ENTRY(ret_from_exception)
.Lret_from_exception:
	btst	#5,%sp@(PT_SR)		/* check if returning to kernel */
	bnes	1f			/* if so, skip resched, signals */
	movel	%d0,%sp@-		/* Only allow interrupts when we are  */
	move	%sr,%d0			/* last one on the kernel stack,      */
	andl	#ALLOWINT,%d0		/* otherwise stack overflow can occur */
	move	%d0,%sr			/* during heavy interrupt load.       */
	movel	%sp@+,%d0

resume_userspace:
	moveb	%curptr@(TASK_INFO+TINFO_FLAGS+3),%d0
	jne	exit_work	/* SIGPENDING and/or NEED_RESCHED set */
1:	RESTORE_ALL

exit_work:
	/* save top of frame */
	movel	%sp,%curptr@(TASK_THREAD+THREAD_ESP0)
	btstl	#6,%d0			/* check for SIGPENDING in flags */
	jne	do_signal_return
	pea	resume_userspace
	jra	schedule

do_signal_return:
	subql	#4,%sp			/* dummy return address */
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	clrl	%sp@-
	bsrl	do_signal
	addql	#8,%sp
	RESTORE_SWITCH_STACK
	addql	#4,%sp
	jbra	resume_userspace

do_delayed_trace:
	bclr	#7,%sp@(PT_SR)		/* clear trace bit in SR */
	pea	1			/* send SIGTRAP */
	movel	%curptr,%sp@-
	pea	LSIGTRAP
	jbsr	send_sig
	addql	#8,%sp
	addql	#4,%sp
	jbra	resume_userspace

/*
 * This is the interrupt handler (for all hardware interrupt
 * sources). It figures out the vector number and calls the appropriate
 * interrupt service routine directly.
 */
ENTRY(inthandler)
	SAVE_ALL_INT
	GET_CURRENT(%d0)
	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	addil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)
	/* put exception # in d0 */
	movel	%sp@(PT_VECTOR),%d0
	swap	%d0			/* extract bits 25:18 */
	lsrl	#2,%d0
	andl	#0x0ff,%d0

	movel	%sp,%sp@-
	movel	%d0,%sp@-		/* put vector # on stack */
auto_irqhandler_fixup = . + 2
	jbsr	process_int		/* process the IRQ */
	addql	#8,%sp			/* pop parameters off stack */

ret_from_interrupt:

	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	subil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)
	jeq	ret_from_last_interrupt
2:	RESTORE_ALL

	ALIGN
ret_from_last_interrupt:
	moveb	%sp@(PT_SR),%d0
	andl	#(~ALLOWINT>>8)&0xff,%d0
	jne	2b

	/* check if we need to do software interrupts */
	tstl	irq_stat+CPUSTAT_SOFTIRQ_PENDING
	jeq	.Lret_from_exception
	pea	ret_from_exception
	jra	do_softirq

ENTRY(user_inthandler)
	SAVE_ALL_INT
	GET_CURRENT(%d0)
	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	addil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)
	/* put exception # in d0 */
	movel	%sp@(PT_VECTOR),%d0
user_irqvec_fixup = . + 2
	swap	%d0			/* extract bits 25:18 */
	lsrl	#2,%d0
	andl	#0x0ff,%d0

	movel	%sp,%sp@-
	movel	%d0,%sp@-		/* put vector # on stack */
user_irqhandler_fixup = . + 2
	jbsr	process_int		/* process the IRQ */
	addql	#8,%sp			/* pop parameters off stack */

	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	subil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)
	jeq	ret_from_last_interrupt
        RESTORE_ALL

/* Handler for uninitialized and spurious interrupts */

ENTRY(bad_inthandler)
	SAVE_ALL_INT
	GET_CURRENT(%d0)
	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	addil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)

	movel	%sp,%sp@-
	jsr	handle_badint
	addql	#4,%sp

	movel   %curptr@(TASK_INFO+TINFO_PREEMPT),%d0
	subil   #0x10000,%d0
	movel   %d0,%curptr@(TASK_INFO+TINFO_PREEMPT)
	jeq	ret_from_last_interrupt
	RESTORE_ALL

ENTRY(sys_fork)
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	jbsr	m68k_fork
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_clone)
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	jbsr	m68k_clone
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_vfork)
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	jbsr	m68k_vfork
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_sigsuspend)
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	jbsr	do_sigsuspend
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_rt_sigsuspend)
	SAVE_SWITCH_STACK
	pea	%sp@(SWITCH_STACK_SIZE)
	jbsr	do_rt_sigsuspend
	addql	#4,%sp
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_sigreturn)
	SAVE_SWITCH_STACK
	jbsr	do_sigreturn
	RESTORE_SWITCH_STACK
	rts

ENTRY(sys_rt_sigreturn)
	SAVE_SWITCH_STACK
	jbsr	do_rt_sigreturn
	RESTORE_SWITCH_STACK
	rts

resume:
	/*
	 * Beware - when entering resume, prev (the current task) is
	 * in a0, next (the new task) is in a1,so don't change these
	 * registers until their contents are no longer needed.
	 */

	/* save sr */
	movew	%sr,%d0
	movew	%d0,%a0@(TASK_THREAD+THREAD_SR)

	/* save usp */
	/* Save USP via %a1 (which is saved/restored from %d0) */
	movel   %a1,%d0
	movel   %usp,%a1
	movel   %a1,%a0@(TASK_THREAD+THREAD_USP)
	movel   %d0,%a1

	/* save non-scratch registers on stack */
	SAVE_SWITCH_STACK

	/* save current kernel stack pointer */
	movel	%sp,%a0@(TASK_THREAD+THREAD_KSP)
#ifdef CONFIG_FPU
	/* save floating point context */
	fsave	%a0@(TASK_THREAD+THREAD_FPSTATE)

1:	tstb	%a0@(TASK_THREAD+THREAD_FPSTATE)
	jeq	3f
2:
	fmovemd	%fp0-%fp7,%a0@(TASK_THREAD+THREAD_FPREG)
3:
#endif
	/* Return previous task in %d1 */
	movel	%curptr,%d1

	/* switch to new task (a1 contains new task) */
	movel	%a1,%curptr
#ifdef CONFIG_FPU
	/* restore floating point context */
1:	tstb	%a1@(TASK_THREAD+THREAD_FPSTATE)
	jeq	3f
2:
	fmovemd	%a1@(TASK_THREAD+THREAD_FPREG),%fp0-%fp7
/*	frestore	%a1@(TASK_THREAD+THREAD_FPCNTL)*/
3:	frestore %a1@(TASK_THREAD+THREAD_FPSTATE)
#endif
	/* restore the kernel stack pointer */
	movel	%a1@(TASK_THREAD+THREAD_KSP),%sp

	/* restore non-scratch registers */
	RESTORE_SWITCH_STACK

	/* restore user stack pointer */
	movel	%a1@(TASK_THREAD+THREAD_USP),%a0
	movel	%a0,%usp

	/* restore status register */
	movew	%a1@(TASK_THREAD+THREAD_SR),%d0
	movew	%d0,%sr

	rts

.data
ALIGN
sys_call_table:
	.long sys_ni_syscall	/* 0  -  old "setup()" system call*/
	.long sys_exit
	.long sys_fork
	.long sys_read
	.long sys_write
	.long sys_open		/* 5 */
	.long sys_close
	.long sys_waitpid
	.long sys_creat
	.long sys_link
	.long sys_unlink	/* 10 */
	.long sys_execve
	.long sys_chdir
	.long sys_time
	.long sys_mknod
	.long sys_chmod		/* 15 */
	.long sys_chown16
	.long sys_ni_syscall			/* old break syscall holder */
	.long sys_stat
	.long sys_lseek
	.long sys_getpid	/* 20 */
	.long sys_mount
	.long sys_oldumount
	.long sys_setuid16
	.long sys_getuid16
	.long sys_stime		/* 25 */
	.long sys_ptrace
	.long sys_alarm
	.long sys_fstat
	.long sys_pause
	.long sys_utime		/* 30 */
	.long sys_ni_syscall			/* old stty syscall holder */
	.long sys_ni_syscall			/* old gtty syscall holder */
	.long sys_access
	.long sys_nice
	.long sys_ni_syscall	/* 35 */	/* old ftime syscall holder */
	.long sys_sync
	.long sys_kill
	.long sys_rename
	.long sys_mkdir
	.long sys_rmdir		/* 40 */
	.long sys_dup
	.long sys_pipe
	.long sys_times
	.long sys_ni_syscall			/* old prof syscall holder */
	.long sys_brk		/* 45 */
	.long sys_setgid16
	.long sys_getgid16
	.long sys_signal
	.long sys_geteuid16
	.long sys_getegid16	/* 50 */
	.long sys_acct
	.long sys_umount			/* recycled never used phys() */
	.long sys_ni_syscall			/* old lock syscall holder */
	.long sys_ioctl
	.long sys_fcntl		/* 55 */
	.long sys_ni_syscall			/* old mpx syscall holder */
	.long sys_setpgid
	.long sys_ni_syscall			/* old ulimit syscall holder */
	.long sys_ni_syscall
	.long sys_umask		/* 60 */
	.long sys_chroot
	.long sys_ustat
	.long sys_dup2
	.long sys_getppid
	.long sys_getpgrp	/* 65 */
	.long sys_setsid
	.long sys_sigaction
	.long sys_sgetmask
	.long sys_ssetmask
	.long sys_setreuid16	/* 70 */
	.long sys_setregid16
	.long sys_sigsuspend
	.long sys_sigpending
	.long sys_sethostname
	.long sys_setrlimit	/* 75 */
	.long sys_old_getrlimit
	.long sys_getrusage
	.long sys_gettimeofday
	.long sys_settimeofday
	.long sys_getgroups16	/* 80 */
	.long sys_setgroups16
	.long old_select
	.long sys_symlink
	.long sys_lstat
	.long sys_readlink	/* 85 */
	.long sys_uselib
	.long sys_swapon
	.long sys_reboot
	.long sys_old_readdir
	.long old_mmap		/* 90 */
	.long sys_munmap
	.long sys_truncate
	.long sys_ftruncate
	.long sys_fchmod
	.long sys_fchown16	/* 95 */
	.long sys_getpriority
	.long sys_setpriority
	.long sys_ni_syscall			/* old profil syscall holder */
	.long sys_statfs
	.long sys_fstatfs	/* 100 */
	.long sys_ni_syscall			/* ioperm for i386 */
	.long sys_socketcall
	.long sys_syslog
	.long sys_setitimer
	.long sys_getitimer	/* 105 */
	.long sys_newstat
	.long sys_newlstat
	.long sys_newfstat
	.long sys_ni_syscall
	.long sys_ni_syscall	/* 110 */	/* iopl for i386 */
	.long sys_vhangup
	.long sys_ni_syscall			/* obsolete idle() syscall */
	.long sys_ni_syscall			/* vm86old for i386 */
	.long sys_wait4
	.long sys_swapoff	/* 115 */
	.long sys_sysinfo
	.long sys_ipc
	.long sys_fsync
	.long sys_sigreturn
	.long sys_clone		/* 120 */
	.long sys_setdomainname
	.long sys_newuname
	.long sys_cacheflush			/* modify_ldt for i386 */
	.long sys_adjtimex
	.long sys_mprotect	/* 125 */
	.long sys_sigprocmask
	.long sys_ni_syscall			/* old "create_module" */
	.long sys_init_module
	.long sys_delete_module
	.long sys_ni_syscall	/* 130 - old "get_kernel_syms" */
	.long sys_quotactl
	.long sys_getpgid
	.long sys_fchdir
	.long sys_bdflush
	.long sys_sysfs		/* 135 */
	.long sys_personality
	.long sys_ni_syscall			/* for afs_syscall */
	.long sys_setfsuid16
	.long sys_setfsgid16
	.long sys_llseek	/* 140 */
	.long sys_getdents
	.long sys_select
	.long sys_flock
	.long sys_msync
	.long sys_readv		/* 145 */
	.long sys_writev
	.long sys_getsid
	.long sys_fdatasync
	.long sys_sysctl
	.long sys_mlock		/* 150 */
	.long sys_munlock
	.long sys_mlockall
	.long sys_munlockall
	.long sys_sched_setparam
	.long sys_sched_getparam	/* 155 */
	.long sys_sched_setscheduler
	.long sys_sched_getscheduler
	.long sys_sched_yield
	.long sys_sched_get_priority_max
	.long sys_sched_get_priority_min  /* 160 */
	.long sys_sched_rr_get_interval
	.long sys_nanosleep
	.long sys_mremap
	.long sys_setresuid16
	.long sys_getresuid16	/* 165 */
	.long sys_getpagesize
	.long sys_ni_syscall			/* old sys_query_module */
	.long sys_poll
	.long sys_nfsservctl
	.long sys_setresgid16	/* 170 */
	.long sys_getresgid16
	.long sys_prctl
	.long sys_rt_sigreturn
	.long sys_rt_sigaction
	.long sys_rt_sigprocmask	/* 175 */
	.long sys_rt_sigpending
	.long sys_rt_sigtimedwait
	.long sys_rt_sigqueueinfo
	.long sys_rt_sigsuspend
	.long sys_pread64	/* 180 */
	.long sys_pwrite64
	.long sys_lchown16;
	.long sys_getcwd
	.long sys_capget
	.long sys_capset	/* 185 */
	.long sys_sigaltstack
	.long sys_sendfile
	.long sys_ni_syscall			/* streams1 */
	.long sys_ni_syscall			/* streams2 */
	.long sys_vfork		/* 190 */
	.long sys_getrlimit
	.long sys_mmap2
	.long sys_truncate64
	.long sys_ftruncate64
	.long sys_stat64	/* 195 */
	.long sys_lstat64
	.long sys_fstat64
	.long sys_chown
	.long sys_getuid
	.long sys_getgid	/* 200 */
	.long sys_geteuid
	.long sys_getegid
	.long sys_setreuid
	.long sys_setregid
	.long sys_getgroups	/* 205 */
	.long sys_setgroups
	.long sys_fchown
	.long sys_setresuid
	.long sys_getresuid
	.long sys_setresgid	/* 210 */
	.long sys_getresgid
	.long sys_lchown
	.long sys_setuid
	.long sys_setgid
	.long sys_setfsuid	/* 215 */
	.long sys_setfsgid
	.long sys_pivot_root
	.long sys_ni_syscall
	.long sys_ni_syscall
	.long sys_getdents64	/* 220 */
	.long sys_gettid
	.long sys_tkill
	.long sys_setxattr
	.long sys_lsetxattr
	.long sys_fsetxattr	/* 225 */
	.long sys_getxattr
	.long sys_lgetxattr
	.long sys_fgetxattr
	.long sys_listxattr
	.long sys_llistxattr	/* 230 */
	.long sys_flistxattr
	.long sys_removexattr
	.long sys_lremovexattr
	.long sys_fremovexattr
	.long sys_futex		/* 235 */
	.long sys_sendfile64
	.long sys_mincore
	.long sys_madvise
	.long sys_fcntl64
	.long sys_readahead	/* 240 */
	.long sys_io_setup
	.long sys_io_destroy
	.long sys_io_getevents
	.long sys_io_submit
	.long sys_io_cancel	/* 245 */
	.long sys_fadvise64
	.long sys_exit_group
	.long sys_lookup_dcookie
	.long sys_epoll_create
	.long sys_epoll_ctl	/* 250 */
	.long sys_epoll_wait
	.long sys_remap_file_pages
	.long sys_set_tid_address
	.long sys_timer_create
	.long sys_timer_settime	/* 255 */
	.long sys_timer_gettime
	.long sys_timer_getoverrun
	.long sys_timer_delete
	.long sys_clock_settime
	.long sys_clock_gettime	/* 260 */
	.long sys_clock_getres
	.long sys_clock_nanosleep
	.long sys_statfs64
	.long sys_fstatfs64
	.long sys_tgkill	/* 265 */
	.long sys_utimes
	.long sys_fadvise64_64
	.long sys_mbind	
	.long sys_get_mempolicy
	.long sys_set_mempolicy	/* 270 */
	.long sys_mq_open
	.long sys_mq_unlink
	.long sys_mq_timedsend
	.long sys_mq_timedreceive
	.long sys_mq_notify	/* 275 */
	.long sys_mq_getsetattr
	.long sys_waitid
	.long sys_ni_syscall			/* for sys_vserver */
	.long sys_add_key
	.long sys_request_key	/* 280 */
	.long sys_keyctl
	.long sys_ioprio_set
	.long sys_ioprio_get
	.long sys_inotify_init
	.long sys_inotify_add_watch	/* 285 */
	.long sys_inotify_rm_watch
	.long sys_migrate_pages
	.long sys_openat
	.long sys_mkdirat
	.long sys_mknodat		/* 290 */
	.long sys_fchownat
	.long sys_futimesat
	.long sys_fstatat64
	.long sys_unlinkat
	.long sys_renameat		/* 295 */
	.long sys_linkat
	.long sys_symlinkat
	.long sys_readlinkat
	.long sys_fchmodat
	.long sys_faccessat		/* 300 */
	.long sys_ni_syscall			/* Reserved for pselect6 */
	.long sys_ni_syscall			/* Reserved for ppoll */
	.long sys_unshare
	.long sys_set_robust_list
	.long sys_get_robust_list	/* 305 */
	.long sys_splice
	.long sys_sync_file_range
	.long sys_tee
	.long sys_vmsplice
	.long sys_move_pages		/* 310 */
	.long sys_sched_setaffinity
	.long sys_sched_getaffinity
	.long sys_kexec_load
	.long sys_getcpu
	.long sys_epoll_pwait		/* 315 */
	.long sys_utimensat
	.long sys_signalfd
	.long sys_timerfd_create
	.long sys_eventfd
	.long sys_fallocate		/* 320 */
	.long sys_timerfd_settime
	.long sys_timerfd_gettime
	.long sys_signalfd4
	.long sys_eventfd2
	.long sys_epoll_create1		/* 325 */
	.long sys_dup3
	.long sys_pipe2
	.long sys_inotify_init1
	.long sys_ni_syscall			/* Reserved */
	.long sys_ni_syscall		/* 330     Reserved */
	.long sys_ni_syscall			/* Reserved */
	.long sys_ni_syscall			/* Reserved */
	.long sys_read_tp
	.long sys_write_tp
	.long sys_atomic_cmpxchg_32	/* 335 */
	.long sys_atomic_barrier
