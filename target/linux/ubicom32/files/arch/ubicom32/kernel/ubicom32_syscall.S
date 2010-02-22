/*
 * arch/ubicom32/kernel/ubicom32_syscall.S
 *	<TODO: Replace with short file description>
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/sys.h>
#include <linux/linkage.h>
#include <linux/unistd.h>

#include <asm/ubicom32-common.h>
#include <asm/thread_info.h>
#include <asm/asm-offsets.h>
#include <asm/range-protect.h>

/*
 * __old_system_call()
 */
	.section .old_syscall_entry.text, "ax", @progbits
#ifdef CONFIG_OLD_40400010_SYSTEM_CALL
__old_system_call:
	call a3, system_call
	.size __old_system_call, . - __old_system_call ;
#else
	/*
	 * something that will crash the userspace application, but
	 * should not take down the kernel, if protection is enabled
	 * this will never even get executed.
	 */
	.long	0xFABBCCDE			; illegal instruction
	bkpt #-1				; we will never get here
#endif

/*
 * system_call()
 */
	.section .syscall_entry.text, "ax", @progbits
	.global system_call
system_call:
	/*
	 * Regular ABI rules for function calls apply for syscall.  d8 holds
	 * the syscall number. We will use that to index into the syscall table.
	 * d0 - d5 hold the parameters.
	 *
	 * First we get the current thread_info and swap to the kernel stack.
	 * This is done by reading the current thread and looking up the ksp
	 * from the sw_ksp array and storing it in a3.
	 *
	 * Then we reserve space for the syscall context a struct pt_regs and
	 * save it using a4 initially and later as sp.
	 * Once sp is set to the kernel sp we can leave the critical section.
	 *
	 * For the user case the kernel stack will have the following layout.
	 *
	 *  a3		 ksp[0] +-----------------------+
	 *			| Thread info area	|
	 *			| struct thread_info	|
	 *			+-----------------------+
	 *			:			:
	 *			|   Kernel Stack Area	|
	 *			|			|
	 *  a4 / sp >>>		+-----------------------+
	 *			| Context save area	|
	 *			| struct pt_reg		|
	 *  ksp[THREAD_SIZE-8]  +-----------------------+
	 *			| 8 Byte Buffer Zone	|
	 *  ksp[THREAD_SIZE]    +-----------------------+

	 *
	 * For kernel syscalls the layout is as follows.
	 *
	 *  a3		 ksp[0] +-----------------------+
	 *			| Thread info area	|
	 *			| struct thread_info	|
	 *			+-----------------------+
	 *			:			:
	 *			|   Kernel Stack Area	|
	 *			|			|
	 *  a4 / sp >>>		+-----------------------+
	 *			| Context save area	|
	 *			| struct pt_reg		|
	 * sp at syscall entry  +-----------------------+
	 *			| Callers Kernel Stack	|
	 *			:			:
	 *
	 * Once the context is saved we optionally call syscall_trace and setup
	 * the exit routine and jump to the syscall.
	 */

	/*
	 * load the base address for sw_ksp into a3
	 * Note.. we cannot access it just yet as protection is still on.
	 */
	moveai	a3, #%hi(sw_ksp)
	lea.1	a3, %lo(sw_ksp)(a3)

	/*
	 * Enter critical section .
	 *
	 * The 'critical' aspects here are the switching the to the ksp and
	 * changing the protection registers, these both use per thread
	 * information so we need to protect from a context switch. For now this
	 * is done using the global atomic lock.
	 */
	atomic_lock_acquire

	thread_get_self d15			; Load current thread number
#ifdef CONFIG_PROTECT_KERNEL
	lsl.4	d9, #1, d15			; Convert to thread bit
	enable_kernel_ranges d9
#endif
	/*
	 * in order to reduce the size of code in the syscall section we get
	 * out of it right now
	 */
	call a4, __system_call_bottom_half
	.size system_call, . - system_call

	.section .text.__system_call_bottom_half, "ax", @progbits
__system_call_bottom_half:

	/*
	 * We need to Determine if this is a kernel syscall or user syscall.
	 * Start by loading the pointer for the thread_info structure for the
	 * current process in to a3.
	 */
	move.4	a3, (a3, d15)			; a3 = sw_ksp[d15]

	/*
	 * Now if this is a kernel thread the same value can be a acheived by
	 * masking off the lower bits on the current stack pointer.
	 */
	movei	d9, #(~(ASM_THREAD_SIZE-1))	; load mask
	and.4	d9, sp, d9			; apply mask

	/*
	 * d9 now has the masked version of the sp. If this is identical to
	 * what is in a3 then don't switch to ksp as we are already in the
	 * kernel.
	 */
	sub.4	#0, a3, d9

	/*
	 * if d9 and a3 are not equal. We are usespace and have to shift to
	 * ksp.
	 */
	jmpne.t	1f

	/*
	 * Kernel Syscall.
	 *
	 * The kernel has called this routine. We have to pdec space for pt_regs
	 * from sp.
	 */
	pdec	a4, PT_SIZE(sp)			; a4 = ksp - PT_SIZE
	jmpt.t	2f

	/*
	 * Userspace Syscall.
	 *
	 * Add THREAD_SIZE and subtract PT_SIZE to create the proper ksp
	 */
1:	movei	d15, #(ASM_THREAD_SIZE - 8 - PT_SIZE)
	lea.1	a4, (a3, d15)			; a4 = ksp + d15

	/*
	 * Replace user stack pointer with kernel stack pointer (a4)
	 * Load -1 into frame_type in save area to indicate this is system call
	 * frame.
	 */
2:	move.4	PT_A7(a4), a7			; Save old sp/A7 on kernel stack
	move.4	PT_FRAME_TYPE(a4), #-1		; Set the frame type.
	move.4	sp, a4				; Change to ksp.
	/*
	 * We are now officially back in the kernel!
	 */

	/*
	 * Now that we are on the ksp we can leave the critical section
	 */
	atomic_lock_release

	/*
	 * We need to save a0 because we need to be able to restore it in
	 * the event that we need to handle a signal.  It's not generally
	 * a callee-saved register but is the GOT pointer.
	 */
	move.4	PT_A0(sp), a0			; Save A0 on kernel stack

	/*
	 * We still need to save d10-d13, a1, a2, a5, a6 in the kernel frame
	 * for this process, we also save the system call params in the case of
	 * syscall restart. (note a7 was saved above)
	 */
	move.4	PT_A1(sp), a1			; Save A1 on kernel stack
	move.4	PT_A2(sp), a2			; Save A2 on kernel stack
	move.4	PT_A5(sp), a5			; Save A5 on kernel stack
	move.4	PT_A6(sp), a6			; Save A6 on kernel stack
	move.4	PT_PC(sp), a5			; Save A5 at the PC location
	move.4	PT_D10(sp), d10			; Save D10 on kernel stack
	move.4	PT_D11(sp), d11			; Save D11 on kernel stack
	move.4	PT_D12(sp), d12			; Save D12 on kernel stack
	move.4	PT_D13(sp), d13			; Save D13 on kernel stack

	/*
	 * Now save the syscall parameters
	 */
	move.4	PT_D0(sp), d0			; Save d0 on kernel stack
	move.4	PT_ORIGINAL_D0(sp), d0		; Save d0 on kernel stack
	move.4	PT_D1(sp), d1			; Save d1 on kernel stack
	move.4	PT_D2(sp), d2			; Save d2 on kernel stack
	move.4	PT_D3(sp), d3			; Save d3 on kernel stack
	move.4	PT_D4(sp), d4			; Save d4 on kernel stack
	move.4	PT_D5(sp), d5			; Save d5 on kernel stack
	move.4	PT_D8(sp), d8			; Save d8 on kernel stack

	/*
	 * Test if syscalls are being traced and if they are jump to syscall
	 * trace (it will comeback here)
	 */
	btst	TI_FLAGS(a3), #ASM_TIF_SYSCALL_TRACE
	jmpne.f .Lsystem_call__trace
.Lsystem_call__trace_complete:
	/*
	 * Check for a valid call number [ 0 <= syscall_number < NR_syscalls ]
	 */
	cmpi	d8, #0
	jmplt.f 3f
	cmpi	d8, #NR_syscalls
	jmplt.t	4f

	/*
	 * They have passed an invalid number. Call sys_ni_syscall staring by
	 * load a4 with the base address of sys_ni_syscall
	 */
3:	moveai	a4, #%hi(sys_ni_syscall)
	lea.1	a4, %lo(sys_ni_syscall)(a4)
	jmpt.t	5f				; Jump to regular processing

	/*
	 * Validated syscall, load the syscall table base address into a3 and
	 * read the syscall ptr out.
	 */
4:	moveai	a3, #%hi(sys_call_table)
	lea.1	a3, %lo(sys_call_table)(a3)	; a3 = sys_call_table
	move.4	a4, (a3, d8)			; a4 = sys_call_table[d8]

	/*
	 * Before calling the syscall, setup a5 so that syscall_exit is called
	 * on return from syscall
	 */
5:	moveai	a5, #%hi(syscall_exit)		; Setup return address
	lea.1	a5, %lo(syscall_exit)(a5)	; from system call

	/*
	 * If the syscall is __NR_rt_rigreturn then we have to test d1 to
	 * figure out if we have to change change the return routine to restore
	 * all registers.
	 */
	cmpi	d8, #__NR_rt_sigreturn
	jmpeq.f	6f

	/*
	 * Launch system call (it will return through a5 - syscall_exit)
	 */
	calli	a3, 0(a4)

	/*
	 * System call is rt_sigreturn. Test d1. If it is 1 we have to
	 * change the return address to restore_all_registers
	 */
6:	cmpi	d1, #1
	jmpne.t	7f

	moveai	a5, #%hi(restore_all_registers)	 ; Setup return address
	lea.1	a5, %lo(restore_all_registers)(a5) ; to restore_all_registers.

	/*
	 * Launch system call  (it will return through a5)
	 */
7:	calli	a3, 0(a4)			 ; Launch system call

.Lsystem_call__trace:
	/*
	 * Syscalls are being traced.
	 * Call syscall_trace, (return here)
	 */
	call	a5, syscall_trace

	/*
	 * Restore syscall state (it would have been discarded during the
	 * syscall trace)
	 */
	move.4	d0, PT_D0(sp)			; Restore d0 from kernel stack
	move.4	d1, PT_D1(sp)			; Restore d1 from kernel stack
	move.4	d2, PT_D2(sp)			; Restore d2 from kernel stack
	move.4	d3, PT_D3(sp)			; Restore d3 from kernel stack
	move.4	d4, PT_D4(sp)			; Restore d4 from kernel stack
	move.4	d5, PT_D5(sp)			; Restore d5 from kernel stack
	/* add this back if we ever have a syscall with 7 args */
	move.4	d8, PT_D8(sp)			; Restore d8 from kernel stack

	/*
	 * return to syscall
	 */
	jmpt.t .Lsystem_call__trace_complete
	.size __system_call_bottom_half, . - __system_call_bottom_half

/*
 * syscall_exit()
 */
	.section .text.syscall_exit
	.global syscall_exit
syscall_exit:
	/*
	 * d0 contains the return value. We should move that into the kernel
	 * stack d0 location.  We will be transitioning from kernel to user
	 * mode. Test the flags and see if we have to call schedule. If we are
	 * going to truly exit then all that has to be done is that from the
	 * kernel stack we have to restore d0, a0, a1, a2, a5, a6 and sp (a7)bb
	 * and then return via a5.
	 */

	/*
	 * Save d0 to pt_regs
	 */
	move.4	PT_D0(sp), d0			; Save d0 into the kernel stack

	/*
	 * load the thread_info structure by masking off the THREAD_SIZE
	 * bits.
	 *
	 * Note: we used to push a1, but now we don't as we are going
	 * to eventually restore it to the userspace a1.
	 */
	movei	d9, #(~(ASM_THREAD_SIZE-1))
	and.4	a1, sp, d9

	/*
	 * Are any interesting bits set on TI flags, if there are jump
	 * aside to post_processing.
	 */
	move.4	d9, #(_TIF_SYSCALL_TRACE | _TIF_NEED_RESCHED | _TIF_SIGPENDING)
	and.4	#0, TI_FLAGS(a1), d9
	jmpne.f	.Lsyscall_exit__post_processing ; jump to handler
.Lsyscall_exit__post_processing_complete:

	move.4	d0, PT_D0(sp)			; Restore D0 from kernel stack
	move.4	d1, PT_D1(sp)			; Restore d1 from kernel stack
	move.4	d2, PT_D2(sp)			; Restore d2 from kernel stack
	move.4	d3, PT_D3(sp)			; Restore d3 from kernel stack
	move.4	d4, PT_D4(sp)			; Restore d4 from kernel stack
	move.4	d5, PT_D5(sp)			; Restore d5 from kernel stack
	move.4	d8, PT_D8(sp)			; Restore d8 from kernel stack
	move.4	d10, PT_D10(sp)			; Restore d10 from kernel stack
	move.4	d11, PT_D11(sp)			; Restore d11 from kernel stack
	move.4	d12, PT_D12(sp)			; Restore d12 from kernel stack
	move.4	d13, PT_D13(sp)			; Restore d13 from kernel stack
	move.4	a1, PT_A1(sp)			; Restore A1 from kernel stack
	move.4	a2, PT_A2(sp)			; Restore A2 from kernel stack
	move.4	a5, PT_A5(sp)			; Restore A5 from kernel stack
	move.4	a6, PT_A6(sp)			; Restore A6 from kernel stack
	move.4	a0, PT_A0(sp)			; Restore A6 from kernel stack

	/*
	 * this is only for debug, and could be removed for production builds
	 */
	move.4	PT_FRAME_TYPE(sp), #0		; invalidate frame_type

#ifdef CONFIG_PROTECT_KERNEL

	call a4, __syscall_exit_bottom_half

	.section .kernel_unprotected, "ax", @progbits
__syscall_exit_bottom_half:
	/*
	 * Enter critical section
	 */
	atomic_lock_acquire
	disable_kernel_ranges_for_current d15
#endif
	/*
	 * Lastly restore userspace stack ptr
	 *
	 * Note: that when protection is on we need to hold the lock around the
	 * stack swap as well because otherwise the protection could get
	 * inadvertently disabled again at the end of a context switch.
	 */
	move.4	a7, PT_A7(sp)			; Restore A7 from kernel stack

	/*
	 * We are now officially back in userspace!
	 */

#ifdef CONFIG_PROTECT_KERNEL
	/*
	 * Leave critical section and return to user space.
	 */
	atomic_lock_release
#endif
	calli	a5, 0(a5)			; Back to userspace code.

	bkpt #-1				; we will never get here

	/*
	 * Post syscall processing. (unlikely part of syscall_exit)
	 *
	 * Are we tracing syscalls. If TIF_SYSCALL_TRACE is set, call
	 * syscall_trace routine and return here.
	 */
	.section .text.syscall_exit, "ax", @progbits
.Lsyscall_exit__post_processing:
	btst	TI_FLAGS(a1), #ASM_TIF_SYSCALL_TRACE
	jmpeq.t	1f
	call	a5, syscall_trace

	/*
	 * Do we need to resched ie call schedule. If TIF_NEED_RESCHED is set,
	 * call the scheduler, it will come back here.
	 */
1:	btst	TI_FLAGS(a1), #ASM_TIF_NEED_RESCHED
	jmpeq.t	2f
	call	a5, schedule

	/*
	 * Do we need to post a signal, if TIF_SIGPENDING is set call the
	 * do_signal.
	 */
2:	btst	TI_FLAGS(a1), #ASM_TIF_SIGPENDING
	jmpeq.t	.Lsyscall_exit__post_processing_complete

	/*
	 * setup the do signal call
	 */
	move.4	d0, #0				; oldset pointer is NULL
	lea.1	d1, (sp)			; d1 is the regs pointer.
	call	a5, do_signal

	jmpt.t  .Lsyscall_exit__post_processing_complete

/*	.size syscall_exit, . - syscall_exit */

/*
 * kernel_execve()
 *	kernel_execv is called when we the kernel is starting a
 *	userspace application.
 */
	.section .kernel_unprotected, "ax", @progbits
	.global kernel_execve
kernel_execve:
	move.4	-4(sp)++, a5			; Save return address
	/*
	 * Call execve
	 */
	movei	d8, #__NR_execve		; call execve
	call	a5, system_call
	move.4	a5, (sp)4++

	/*
	 * protection was enabled again at syscall exit, but we want
	 * to return to kernel so we enable it again.
	 */
#ifdef CONFIG_PROTECT_KERNEL
	/*
	 * We are entering the kernel so we need to disable the protection.
	 * Enter critical section, disable ranges and leave critical section.
	 */
	call a3, __enable_kernel_ranges ;  and jump back to kernel
#else
	ret a5					; jump back to the kernel
#endif

	.size kernel_execve, . - kernel_execve

/*
 * signal_trampoline()
 *
 *	Deals with transitioning from to userspace signal handlers and returning
 *	to userspace, only called from the kernel.
 *
 */
	.section .kernel_unprotected, "ax", @progbits
	.global signal_trampoline
signal_trampoline:
	/*
	 * signal_trampoline is called when we are jumping from the kernel to
	 * the userspace signal handler.
	 *
	 * The following registers are relevant. (set setup_rt_frame)
	 *   sp is the user space stack not the kernel stack
	 *  d0 = signal number
	 *  d1 = siginfo_t *
	 *  d2 = ucontext *
	 *  d3 = the user space signal handler
	 *  a0 is set to the GOT if userspace application is FDPIC, otherwise 0
	 *  a3 is set to the FD for the signal if userspace application is FDPIC
	 */
#ifdef CONFIG_PROTECT_KERNEL
	/*
	 * We are leaving the kernel so we need to enable the protection.
	 * Enter critical section, disable ranges and leave critical section.
	 */
	atomic_lock_acquire			; Enter critical section
	disable_kernel_ranges_for_current d15	; disable kernel ranges
	atomic_lock_release			; Leave critical section
#endif
	/*
	 * The signal handler pointer is in register d3 so tranfer it to a4 and
	 * call it
	 */
	movea	a4, d3				; signal handler
	calli	a5, 0(a4)

	/*
	 * Return to userspace through rt_syscall which is stored on top of the
	 * stack d1 contains ret_via_interrupt status.
	 */
	move.4	d8, (sp)			; d8 (syscall #) = rt_syscall
	move.4	d1, 4(sp)			; d1 = ret_via_interrupt
	call	a5, system_call		; as we are 'in' the kernel
						; we can call kernel_syscall

	bkpt #-1				; will never get here.
	.size signal_trampoline, . - signal_trampoline

/*
 * kernel_thread_helper()
 *
 *	Entry point for kernel threads (only referenced by kernel_thread()).
 *
 *	On execution d0 will be 0, d1 will be the argument to be passed to the
 *	kernel function.
 *	d2 contains the kernel function that needs to get called.
 *	d3 will contain address to do_exit which needs to get moved into a5.
 *
 *	On return from fork the child thread d0 will be 0. We call this dummy
 *	function which in turn loads the argument
 */
	.section .kernel_unprotected, "ax", @progbits
	.global kernel_thread_helper
kernel_thread_helper:
	/*
	 * Create a kernel thread. This is called from ret_from_vfork (a
	 * userspace return routine) so we need to put it in an unprotected
	 * section and re-enable protection before calling the vector in d2.
	 */

#ifdef CONFIG_PROTECT_KERNEL
	/*
	 * We are entering the kernel so we need to disable the protection.
	 * Enter critical section, disable ranges and leave critical section.
	 */
	call a5, __enable_kernel_ranges
#endif
	/*
	 * Move argument for kernel function into d0, and set a5 return address
	 * (a5) to do_exit and return through a2
	 */
	move.4  d0, d1				; d0 = arg
	move.4  a5, d3				; a5 = do_exit
	ret	d2				; call function ptr in d2
	.size kernel_thread_helper, . - kernel_thread_helper

#ifdef CONFIG_PROTECT_KERNEL
	.section .kernel_unprotected, "ax", @progbits
__enable_kernel_ranges:
	atomic_lock_acquire			; Enter critical section
	enable_kernel_ranges_for_current d15
	atomic_lock_release			; Leave critical section
	calli a5, 0(a5)
	.size __enable_kernel_ranges, . - __enable_kernel_ranges

#endif

/*
 * The following system call intercept functions where we setup the
 * input to the real system call.  In all cases these are just taking
 * the current sp which is pointing to pt_regs and pushing it into the
 * last arg of the system call.
 *
 * i.e. the public definition of sys_execv is
 *	sys_execve(	char *name,
 *			char **argv,
 *			char **envp )
 * but process.c defines it as
 *	sys_execve(	char *name,
 *			char **argv,
 *			char **envp,
 *			struct pt_regs *regs )
 *
 * so execve_intercept needs to populate the 4th arg with pt_regs*,
 * which is the stack pointer as we know we must be coming out of
 * system_call
 *
 * The intercept vectors are referenced by syscalltable.S
 */

/*
 * execve_intercept()
 */
	.section .text.execve_intercept, "ax", @progbits
	.global execve_intercept
execve_intercept:
	move.4	d3, sp	; Save pt_regs address
	call	a3, sys_execve

	.size execve_intercept, . - execve_intercept

/*
 * vfork_intercept()
 */
	.section .text.vfork_intercept, "ax", @progbits
	.global vfork_intercept
vfork_intercept:
	move.4	d0, sp	; Save pt_regs address
	call	a3, sys_vfork

	.size vfork_intercept, . - vfork_intercept

/*
 * clone_intercept()
 */
	.section .text.clone_intercept, "ax", @progbits
	.global clone_intercept
clone_intercept:
	move.4	d2, sp	; Save pt_regs address
	call	a3, sys_clone

	.size clone_intercept, . - clone_intercept

/*
 * sys_sigsuspend()
 */
	.section .text.sigclone_intercept, "ax", @progbits
	.global sys_sigsuspend
sys_sigsuspend:
	move.4	d0, sp	; Pass pointer to pt_regs in d0
	call	a3, do_sigsuspend

	.size sys_sigsuspend, . - sys_sigsuspend

/*
 * sys_rt_sigsuspend()
 */
	.section .text.sys_rt_sigsuspend, "ax", @progbits
	.global sys_rt_sigsuspend
sys_rt_sigsuspend:
	move.4	d0, sp	; Pass pointer to pt_regs in d0
	call	a3, do_rt_sigsuspend

	.size sys_rt_sigsuspend, . - sys_rt_sigsuspend

/*
 * sys_rt_sigreturn()
 */
	.section .text.sys_rt_sigreturn, "ax", @progbits
	.global sys_rt_sigreturn
sys_rt_sigreturn:
	move.4	d0, sp	; Pass pointer to pt_regs in d0
	call	a3, do_rt_sigreturn

	.size sys_rt_sigreturn, . - sys_rt_sigreturn

/*
 * sys_sigaltstack()
 */
	.section .text.sys_sigaltstack, "ax", @progbits
	.global sys_sigaltstack
sys_sigaltstack:
	move.4	d0, sp	; Pass pointer to pt_regs in d0
	call	a3, do_sys_sigaltstack

	.size sys_sigaltstack, . - sys_sigaltstack
