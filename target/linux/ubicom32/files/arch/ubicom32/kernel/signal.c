/*
 * arch/ubicom32/kernel/signal.c
 *   Ubicom32 architecture signal handling implementation.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 * Copyright (C) 1991, 1992  Linus Torvalds
 * Linux/m68k support by Hamish Macdonald
 * 68060 fixes by Jesper Skov
 * 1997-12-01  Modified for POSIX.1b signals by Andreas Schwab
 * mathemu support by Roman Zippel
 * ++roman (07/09/96): implemented signal stacks
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
 *
 * mathemu support by Roman Zippel
 *  (Note: fpstate in the signal context is completely ignored for the emulator
 *         and the internal floating point format is put on stack)
 *
 * ++roman (07/09/96): implemented signal stacks (specially for tosemu on
 * Atari :-) Current limitation: Only one sigstack can be active at one time.
 * If a second signal with SA_ONSTACK set arrives while working on a sigstack,
 * SA_ONSTACK is ignored. This behaviour avoids lots of trouble with nested
 * signal handlers!
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/signal.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ptrace.h>
#include <linux/unistd.h>
#include <linux/stddef.h>
#include <linux/highuid.h>
#include <linux/tty.h>
#include <linux/personality.h>
#include <linux/binfmts.h>

#include <asm/setup.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/traps.h>
#include <asm/ucontext.h>

#define _BLOCKABLE (~(sigmask(SIGKILL) | sigmask(SIGSTOP)))

/*
 * asm signal return handlers.
 */
void ret_from_user_signal(void);
void ret_from_user_rt_signal(void);
asmlinkage int do_signal(sigset_t *oldset, struct pt_regs *regs);

/*
 * Common signal suspend implementation
 */
static int signal_suspend(sigset_t *saveset, struct pt_regs *regs)
{
	regs->dn[0] = -EINTR;
	while (1) {
		current->state = TASK_INTERRUPTIBLE;
		schedule();
		if (!do_signal(saveset, regs)) {
			continue;
		}
		/*
		 * If the current frame type is a signal trampoline we are
		 * actually going to call the signal handler so we return the
		 * desired d0 as the return value.
		 */
		if (regs->frame_type == UBICOM32_FRAME_TYPE_SIGTRAMP) {
			return regs->dn[0];
		}
		return -EINTR;
	}
	/*
	 * Should never get here
	 */
	BUG();
	return 0;
}

/*
 * Atomically swap in the new signal mask, and wait for a signal.
 */
asmlinkage int do_sigsuspend(struct pt_regs *regs)
{
	old_sigset_t mask = regs->dn[0];
	sigset_t saveset;

	mask &= _BLOCKABLE;
	spin_lock_irq(&current->sighand->siglock);
	saveset = current->blocked;
	siginitset(&current->blocked, mask);
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	/*
	 * Call common handler
	 */
	return signal_suspend(&saveset, regs);
}

asmlinkage int
do_rt_sigsuspend(struct pt_regs *regs)
{
	sigset_t *unewset = (sigset_t *)regs->dn[0];
	size_t sigsetsize = (size_t)regs->dn[1];
	sigset_t saveset, newset;

	/* XXX: Don't preclude handling different sized sigset_t's.  */
	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&newset, unewset, sizeof(newset)))
		return -EFAULT;
	sigdelsetmask(&newset, ~_BLOCKABLE);

	spin_lock_irq(&current->sighand->siglock);
	saveset = current->blocked;
	current->blocked = newset;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	/*
	 * Call common handler
	 */
	return signal_suspend(&saveset, regs);
}

asmlinkage int
sys_sigaction(int sig, const struct old_sigaction *act,
	      struct old_sigaction *oact)
{
	struct k_sigaction new_ka, old_ka;
	int ret;

	if (act) {
		old_sigset_t mask;
		if (!access_ok(VERIFY_READ, act, sizeof(*act)) ||
		    __get_user(new_ka.sa.sa_handler, &act->sa_handler) ||
		    __get_user(new_ka.sa.sa_restorer, &act->sa_restorer))
			return -EFAULT;
		__get_user(new_ka.sa.sa_flags, &act->sa_flags);
		__get_user(mask, &act->sa_mask);
		siginitset(&new_ka.sa.sa_mask, mask);
	}

	ret = do_sigaction(sig, act ? &new_ka : NULL, oact ? &old_ka : NULL);

	if (!ret && oact) {
		if (!access_ok(VERIFY_WRITE, oact, sizeof(*oact)) ||
		    __put_user(old_ka.sa.sa_handler, &oact->sa_handler) ||
		    __put_user(old_ka.sa.sa_restorer, &oact->sa_restorer))
			return -EFAULT;
		__put_user(old_ka.sa.sa_flags, &oact->sa_flags);
		__put_user(old_ka.sa.sa_mask.sig[0], &oact->sa_mask);
	}

	return ret;
}

asmlinkage int
do_sys_sigaltstack(struct pt_regs *regs)
{
	const stack_t *uss = (stack_t *) regs->dn[0];
	stack_t *uoss = (stack_t *)regs->dn[1];
	return do_sigaltstack(uss, uoss, regs->an[7]);
}

/*
 * fdpic_func_descriptor describes sa_handler when the application is FDPIC
 */
struct fdpic_func_descriptor {
	unsigned long	text;
	unsigned long	GOT;
};

/*
 * rt_sigframe is stored on the user stack immediately before (above)
 * the signal handlers stack.
 */
struct rt_sigframe
{
	unsigned long syscall_number;	/* This holds __NR_rt_sigreturn. */
	unsigned long restore_all_regs; /* This field gets set to 1 if the frame
					 * type is TRAP or INTERRUPT. */
	siginfo_t *info;
	struct ucontext uc;
	int sig;
	void *pretcode;
};

/*
 * Do a signal return; undo the signal stack.
 */
asmlinkage int do_sigreturn(unsigned long __unused)
{
	BUG();
	return 0;
}

asmlinkage int do_rt_sigreturn(struct pt_regs *regs)
{
	unsigned long usp = regs->an[7];
	struct rt_sigframe *frame = (struct rt_sigframe *)(usp);
	sigset_t set;

	if (!access_ok(VERIFY_READ, frame, sizeof(*frame)))
		goto badframe;
	if (__copy_from_user(&set, &frame->uc.uc_sigmask, sizeof(set)))
		goto badframe;

	sigdelsetmask(&set, ~_BLOCKABLE);
	spin_lock_irq(&current->sighand->siglock);
	current->blocked = set;
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);

	if (copy_from_user(regs, &frame->uc.uc_mcontext, sizeof(struct pt_regs)))
		goto badframe;
	return regs->dn[0];

badframe:
	force_sig(SIGSEGV, current);
	return 0;
}

static inline void *
get_sigframe(struct k_sigaction *ka, struct pt_regs *regs, size_t frame_size)
{
	unsigned long usp;

	/* Default to using normal stack.  */
	usp = regs->an[7];

	/* This is the X/Open sanctioned signal stack switching.  */
	if (ka->sa.sa_flags & SA_ONSTACK) {
		if (!sas_ss_flags(usp))
			usp = current->sas_ss_sp + current->sas_ss_size;
	}
	return (void *)((usp - frame_size) & ~0x3);
}

/*
 * signal_trampoline:  Defined in ubicom32_syscall.S
 */
asmlinkage void signal_trampoline(void)__attribute__((naked));

static void setup_rt_frame (int sig, struct k_sigaction *ka, siginfo_t *info,
			    sigset_t *set, struct pt_regs *regs)
{
	struct rt_sigframe *frame;
	int err = 0;

	frame = (struct rt_sigframe *) get_sigframe(ka, regs, sizeof(*frame));

	/*
	 * The 'err |=' have been may criticized as bad code style, but I
	 * strongly suspect that we want this code to be fast.  So for
	 * now it stays as is.
	 */
	err |= __put_user( (  (current_thread_info()->exec_domain)
			   && (current_thread_info()->exec_domain->signal_invmap)
			   && (sig < 32) )
			   ? current_thread_info()->exec_domain->signal_invmap[sig]
			   : sig, &frame->sig);
	err |= __put_user(info, &frame->info);

	/* Create the ucontext.  */
	err |= __put_user(0, &frame->uc.uc_flags);
	err |= __put_user(0, &frame->uc.uc_link);
	err |= __put_user((void *)current->sas_ss_sp,
			  &frame->uc.uc_stack.ss_sp);
	err |= __put_user(sas_ss_flags(regs->an[7]),
			  &frame->uc.uc_stack.ss_flags);
	err |= __put_user(current->sas_ss_size, &frame->uc.uc_stack.ss_size);
	err |= __put_user(__NR_rt_sigreturn, &frame->syscall_number);
	if ((regs->frame_type == UBICOM32_FRAME_TYPE_TRAP) ||
	    (regs->frame_type == UBICOM32_FRAME_TYPE_INTERRUPT)) {
		err |= __put_user(1, &frame->restore_all_regs);
	} else {
		err |= __put_user(0, &frame->restore_all_regs);
	}
	err |= copy_to_user (&frame->uc.uc_mcontext.sc_regs, regs, sizeof(struct pt_regs));
	err |= copy_to_user (&frame->uc.uc_sigmask, set, sizeof(*set));

	if (err)
		goto give_sigsegv;

	/*
	 * Set up registers for signal handler NOTE: Do not modify dn[14], it
	 * contains the userspace tls pointer, so it important that it carries
	 * over to the signal handler.
	 */
	regs->an[7] = (unsigned long)frame;
	regs->pc = (unsigned long) signal_trampoline;
	regs->an[5] = (unsigned long) signal_trampoline;
	regs->dn[0] = sig;
	regs->dn[1] = (unsigned long) frame->info;
	regs->dn[2] = (unsigned int) &frame->uc;

	/*
	 * If this is FDPIC then the signal handler is actually a function
	 * descriptor.
	 */
	if (current->personality & FDPIC_FUNCPTRS) {
		struct fdpic_func_descriptor __user *funcptr =
			(struct fdpic_func_descriptor *) ka->sa.sa_handler;
		err |= __get_user(regs->dn[3], &funcptr->text);
		err |= __get_user(regs->an[0], &funcptr->GOT);
		if (err)
			goto give_sigsegv;

		/*
		 * The funcdesc must be in a3 as this is required for the lazy
		 * resolver in ld.so, if the application is not FDPIC a3 is not
		 * used.
		 */
		regs->an[3] = (unsigned long) funcptr;

	} else {
		regs->dn[3] = (unsigned long)ka->sa.sa_handler;
		regs->an[0] = 0;
	}

	regs->frame_type =  UBICOM32_FRAME_TYPE_SIGTRAMP;

	return;

give_sigsegv:
	/* user space exception */
	force_sigsegv(sig, current);
}

static inline void
handle_restart(struct pt_regs *regs, struct k_sigaction *ka, int has_handler)
{
	switch (regs->dn[0]) {
	case -ERESTARTNOHAND:
		if (!has_handler)
			goto do_restart;
		regs->dn[0] = -EINTR;
		break;

	case -ERESTARTSYS:
		if (has_handler && !(ka->sa.sa_flags & SA_RESTART)) {
			regs->dn[0] = -EINTR;
			break;
		}
	/* fallthrough */
	case -ERESTARTNOINTR:
	do_restart:
		regs->dn[0] = regs->original_dn_0;
		regs->pc -= 8;
		regs->an[5] -= 8;
		break;
	}
}

/*
 * OK, we're invoking a handler
 */
static void
handle_signal(int sig, struct k_sigaction *ka, siginfo_t *info,
	      sigset_t *oldset, struct pt_regs *regs)
{
	/* are we from a system call? */
	if (regs->frame_type == -1)
		/* If so, check system call restarting.. */
		handle_restart(regs, ka, 1);

	/* set up the stack frame */
	setup_rt_frame(sig, ka, info, oldset, regs);

	if (ka->sa.sa_flags & SA_ONESHOT)
		ka->sa.sa_handler = SIG_DFL;

	spin_lock_irq(&current->sighand->siglock);
	sigorsets(&current->blocked,&current->blocked,&ka->sa.sa_mask);
	if (!(ka->sa.sa_flags & SA_NODEFER))
		sigaddset(&current->blocked,sig);
	recalc_sigpending();
	spin_unlock_irq(&current->sighand->siglock);
}

/*
 * Note that 'init' is a special process: it doesn't get signals it doesn't
 * want to handle. Thus you cannot kill init even with a SIGKILL even by
 * mistake.
 */
asmlinkage int do_signal(sigset_t *oldset, struct pt_regs *regs)
{
	struct k_sigaction ka;
	siginfo_t info;
	int signr;

	/*
	 * We want the common case to go fast, which
	 * is why we may in certain cases get here from
	 * kernel mode. Just return without doing anything
	 * if so.
	 */
	if (!user_mode(regs))
		return 1;

	if (!oldset)
		oldset = &current->blocked;

	signr = get_signal_to_deliver(&info, &ka, regs, NULL);
	if (signr > 0) {
		/* Whee!  Actually deliver the signal.  */
		handle_signal(signr, &ka, &info, oldset, regs);
		return 1;
	}

	/* Did we come from a system call? */
	if (regs->frame_type == -1) {
		/* Restart the system call - no handlers present */
		handle_restart(regs, NULL, 0);
	}

	return 0;
}

/*
 * sys_sigreturn()
 *	Return handler for signal clean-up.
 *
 * NOTE: Ubicom32 does not use this syscall.  Instead we rely
 * on do_rt_sigreturn().
 */
asmlinkage long sys_sigreturn(void)
{
	return -ENOSYS;
}
