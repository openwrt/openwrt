/*
 *  linux/arch/m68knommu/kernel/traps.c
 *
 *  Copyright Freescale Semiconductor, Inc. 2008-2009
 *  Jason Jin Jason.Jin@freescale.com
 *  Shrek Wu B16972@freescale.com
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive
 *  for more details.
 */

/*
 * Sets up all exception vectors
 */
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/a.out.h>
#include <linux/user.h>
#include <linux/string.h>
#include <linux/linkage.h>
#include <linux/init.h>
#include <linux/ptrace.h>
#include <linux/kallsyms.h>

#include <asm/setup.h>
#include <asm/fpu.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/traps.h>
#include <asm/pgtable.h>
#include <asm/machdep.h>
#include <asm/siginfo.h>

static char const * const vec_names[] = {
	"RESET SP", "RESET PC", "BUS ERROR", "ADDRESS ERROR",
	"ILLEGAL INSTRUCTION", "ZERO DIVIDE", "CHK", "TRAPcc",
	"PRIVILEGE VIOLATION", "TRACE", "LINE 1010", "LINE 1111",
	"UNASSIGNED RESERVED 12", "COPROCESSOR PROTOCOL VIOLATION",
	"FORMAT ERROR", "UNINITIALIZED INTERRUPT",
	"UNASSIGNED RESERVED 16", "UNASSIGNED RESERVED 17",
	"UNASSIGNED RESERVED 18", "UNASSIGNED RESERVED 19",
	"UNASSIGNED RESERVED 20", "UNASSIGNED RESERVED 21",
	"UNASSIGNED RESERVED 22", "UNASSIGNED RESERVED 23",
	"SPURIOUS INTERRUPT", "LEVEL 1 INT", "LEVEL 2 INT", "LEVEL 3 INT",
	"LEVEL 4 INT", "LEVEL 5 INT", "LEVEL 6 INT", "LEVEL 7 INT",
	"SYSCALL", "TRAP #1", "TRAP #2", "TRAP #3",
	"TRAP #4", "TRAP #5", "TRAP #6", "TRAP #7",
	"TRAP #8", "TRAP #9", "TRAP #10", "TRAP #11",
	"TRAP #12", "TRAP #13", "TRAP #14", "TRAP #15",
	"FPCP BSUN", "FPCP INEXACT", "FPCP DIV BY 0", "FPCP UNDERFLOW",
	"FPCP OPERAND ERROR", "FPCP OVERFLOW", "FPCP SNAN",
	"FPCP UNSUPPORTED OPERATION",
	"MMU CONFIGURATION ERROR"
};

asmlinkage int do_page_fault(struct pt_regs *regs, unsigned long address,
			     unsigned long error_code);
asmlinkage void trap_c(struct frame *fp);
extern void __init coldfire_trap_init(void);

void __init trap_init(void)
{
	coldfire_trap_init();
}

/* The following table converts the FS encoding of a ColdFire
   exception stack frame into the error_code value needed by
   do_fault. */

static const unsigned char fs_err_code[] = {
	0,  /* 0000 */
	0,  /* 0001 */
	0,  /* 0010 */
	0,  /* 0011 */
	1,  /* 0100 */
	0,  /* 0101 */
	0,  /* 0110 */
	0,  /* 0111 */
	2,  /* 1000 */
	3,  /* 1001 */
	2,  /* 1010 */
	0,  /* 1011 */
	1,  /* 1100 */
	1,  /* 1101 */
	0,  /* 1110 */
	0   /* 1111 */
};

#ifdef DEBUG
static const char *fs_err_msg[16] = {
	"Normal",
	"Reserved",
	"Interrupt during debug service routine",
	"Reserved",
	"X Protection",
	"TLB X miss (opword)",
	"TLB X miss (ext. word)",
	"IFP in emulator mode",
	"W Protection",
	"Write error",
	"TLB W miss",
	"Reserved",
	"R Protection",
	"R/RMW Protection",
	"TLB R miss",
	"OEP in emulator mode",
};
#endif

static inline void access_errorCF(struct frame *fp)
{
	unsigned long int mmusr, complainingAddress;
	unsigned int err_code, fs;
	int need_page_fault;

	mmusr = fp->ptregs.mmusr;
	complainingAddress = fp->ptregs.mmuar;
#ifdef DEBUG
	printk(KERN_DEBUG "pc %#lx, mmusr %#lx, complainingAddress %#lx\n", \
		fp->ptregs.pc, mmusr, complainingAddress);
#endif

	/*
	 * error_code:
	 *	bit 0 == 0 means no page found, 1 means protection fault
	 *	bit 1 == 0 means read, 1 means write
	 */

	fs = (fp->ptregs.fs2 << 2) | fp->ptregs.fs1;
	switch (fs) {
	case  5:  /* 0101 TLB opword X miss */
		need_page_fault = cf_tlb_miss(&fp->ptregs, 0, 0, 0);
		complainingAddress = fp->ptregs.pc;
		break;
	case  6:  /* 0110 TLB extension word X miss */
		need_page_fault = cf_tlb_miss(&fp->ptregs, 0, 0, 1);
		complainingAddress = fp->ptregs.pc + sizeof(long);
		break;
	case 10:  /* 1010 TLB W miss */
		need_page_fault = cf_tlb_miss(&fp->ptregs, 1, 1, 0);
		break;
	case 14: /* 1110 TLB R miss */
		need_page_fault = cf_tlb_miss(&fp->ptregs, 0, 1, 0);
		break;
	default:
		/* 0000 Normal  */
		/* 0001 Reserved */
		/* 0010 Interrupt during debug service routine */
		/* 0011 Reserved */
		/* 0100 X Protection */
		/* 0111 IFP in emulator mode */
		/* 1000 W Protection*/
		/* 1001 Write error*/
		/* 1011 Reserved*/
		/* 1100 R Protection*/
		/* 1101 R Protection*/
		/* 1111 OEP in emulator mode*/
		need_page_fault = 1;
		break;
	}

	if (need_page_fault) {
		err_code = fs_err_code[fs];
		if ((fs == 13) && (mmusr & MMUSR_WF)) /* rd-mod-wr access */
			err_code |= 2; /* bit1 - write, bit0 - protection */
		do_page_fault(&fp->ptregs, complainingAddress, err_code);
	}
}

void die_if_kernel(char *str, struct pt_regs *fp, int nr)
{
	if (!(fp->sr & PS_S))
		return;

	console_verbose();
	printk(KERN_EMERG "%s: %08x\n", str, nr);
	printk(KERN_EMERG "PC: [<%08lx>]", fp->pc);
	print_symbol(" %s", fp->pc);
	printk(KERN_EMERG "\nSR: %04x  SP: %p  a2: %08lx\n",
	       fp->sr, fp, fp->a2);
	printk(KERN_EMERG "d0: %08lx    d1: %08lx    d2: %08lx    d3: %08lx\n",
	       fp->d0, fp->d1, fp->d2, fp->d3);
	printk(KERN_EMERG "d4: %08lx    d5: %08lx    a0: %08lx    a1: %08lx\n",
	       fp->d4, fp->d5, fp->a0, fp->a1);

	printk(KERN_EMERG "Process %s (pid: %d, stackpage=%08lx)\n",
		current->comm, current->pid, PAGE_SIZE+(unsigned long)current);
	show_stack(NULL, (unsigned long *)fp);
	do_exit(SIGSEGV);
}

asmlinkage void buserr_c(struct frame *fp)
{
	unsigned int fs;

	/* Only set esp0 if coming from user mode */
	if (user_mode(&fp->ptregs))
		current->thread.esp0 = (unsigned long) fp;

	fs = (fp->ptregs.fs2 << 2) | fp->ptregs.fs1;
#if defined(DEBUG)
	printk(KERN_DEBUG "*** Bus Error *** (%x)%s\n", fs,
		fs_err_msg[fs & 0xf]);
#endif
	switch (fs) {
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x9:
	case 0xa:
	case 0xd:
	case 0xe:
	case 0xf:
		access_errorCF(fp);
		break;
	default:
		die_if_kernel("bad frame format", &fp->ptregs, 0);
#if defined(DEBUG)
		printk(KERN_DEBUG "Unknown SIGSEGV - 4\n");
#endif
		force_sig(SIGSEGV, current);
	}
}

void show_trace(unsigned long *stack)
{
	unsigned long *endstack;
	unsigned long addr;
	int i;

	printk("Call Trace:");
	addr = (unsigned long)stack + THREAD_SIZE - 1;
	endstack = (unsigned long *)(addr & -THREAD_SIZE);
	i = 0;
	while (stack + 1 <= endstack) {
		addr = *stack++;
		/*
		 * If the address is either in the text segment of the
		 * kernel, or in the region which contains vmalloc'ed
		 * memory, it *may* be the address of a calling
		 * routine; if so, print it so that someone tracing
		 * down the cause of the crash will be able to figure
		 * out the call path that was taken.
		 */
		if (__kernel_text_address(addr)) {
#ifndef CONFIG_KALLSYMS
			if (i % 5 == 0)
				printk("\n       ");
#endif
			printk(" [<%08lx>] %pS\n", addr, (void *)addr);
			i++;
		}
	}
	printk("\n");
}

int kstack_depth_to_print = 48;
void show_stack(struct task_struct *task, unsigned long *stack)
{
	unsigned long *p;
	unsigned long *endstack;
	int i;

	if (!stack) {
		if (task)
			stack = (unsigned long *)task->thread.esp0;
		else
			stack = (unsigned long *)&stack;
	}
	endstack = (unsigned long *)(((unsigned long)stack + THREAD_SIZE - 1) & -THREAD_SIZE);

	printk("Stack from %08lx:", (unsigned long)stack);
	p = stack;
	for (i = 0; i < kstack_depth_to_print; i++) {
		if (p + 1 > endstack)
			break;
		if (i % 8 == 0)
			printk("\n       ");
		printk(" %08lx", *p++);
	}
	printk("\n");
	show_trace(stack);
}

void bad_super_trap(struct frame *fp)
{
	console_verbose();
	if (fp->ptregs.vector < sizeof(vec_names)/sizeof(vec_names[0]))
		printk(KERN_WARNING "*** %s ***   FORMAT=%X\n",
			vec_names[fp->ptregs.vector],
			fp->ptregs.format);
	else
		printk(KERN_WARNING "*** Exception %d ***   FORMAT=%X\n",
			fp->ptregs.vector,
			fp->ptregs.format);
	printk(KERN_WARNING "Current process id is %d\n", current->pid);
	die_if_kernel("BAD KERNEL TRAP", &fp->ptregs, 0);
}

asmlinkage void trap_c(struct frame *fp)
{
	int sig;
	siginfo_t info;

	if (fp->ptregs.sr & PS_S) {
		if (fp->ptregs.vector == VEC_TRACE) {
			/* traced a trapping instruction */
			current->ptrace |= PT_DTRACE;
		} else
			bad_super_trap(fp);
		return;
	}

	/* send the appropriate signal to the user program */
	switch (fp->ptregs.vector) {
	case VEC_ADDRERR:
		info.si_code = BUS_ADRALN;
		sig = SIGBUS;
		break;
	case VEC_ILLEGAL:
	case VEC_LINE10:
	case VEC_LINE11:
		info.si_code = ILL_ILLOPC;
		sig = SIGILL;
		break;
	case VEC_PRIV:
		info.si_code = ILL_PRVOPC;
		sig = SIGILL;
		break;
	case VEC_COPROC:
		info.si_code = ILL_COPROC;
		sig = SIGILL;
		break;
	case VEC_TRAP1: /* gdbserver breakpoint */
		fp->ptregs.pc -= 2;
		info.si_code = TRAP_TRACE;
		sig = SIGTRAP;
		break;
	case VEC_TRAP2:
	case VEC_TRAP3:
	case VEC_TRAP4:
	case VEC_TRAP5:
	case VEC_TRAP6:
	case VEC_TRAP7:
	case VEC_TRAP8:
	case VEC_TRAP9:
	case VEC_TRAP10:
	case VEC_TRAP11:
	case VEC_TRAP12:
	case VEC_TRAP13:
	case VEC_TRAP14:
		info.si_code = ILL_ILLTRP;
		sig = SIGILL;
		break;
	case VEC_FPBRUC:
	case VEC_FPOE:
	case VEC_FPNAN:
		info.si_code = FPE_FLTINV;
		sig = SIGFPE;
		break;
	case VEC_FPIR:
		info.si_code = FPE_FLTRES;
		sig = SIGFPE;
		break;
	case VEC_FPDIVZ:
		info.si_code = FPE_FLTDIV;
		sig = SIGFPE;
		break;
	case VEC_FPUNDER:
		info.si_code = FPE_FLTUND;
		sig = SIGFPE;
		break;
	case VEC_FPOVER:
		info.si_code = FPE_FLTOVF;
		sig = SIGFPE;
		break;
	case VEC_ZERODIV:
		info.si_code = FPE_INTDIV;
		sig = SIGFPE;
		break;
	case VEC_CHK:
	case VEC_TRAP:
		info.si_code = FPE_INTOVF;
		sig = SIGFPE;
		break;
	case VEC_TRACE:		/* ptrace single step */
		info.si_code = TRAP_TRACE;
		sig = SIGTRAP;
		break;
	case VEC_TRAP15:		/* breakpoint */
		info.si_code = TRAP_BRKPT;
		sig = SIGTRAP;
		break;
	default:
		info.si_code = ILL_ILLOPC;
		sig = SIGILL;
		break;
	}
	info.si_signo = sig;
	info.si_errno = 0;
	switch (fp->ptregs.format) {
	default:
		info.si_addr = (void *) fp->ptregs.pc;
		break;
	case 2:
		info.si_addr = (void *) fp->un.fmt2.iaddr;
		break;
	case 7:
		info.si_addr = (void *) fp->un.fmt7.effaddr;
		break;
	case 9:
		info.si_addr = (void *) fp->un.fmt9.iaddr;
		break;
	case 10:
		info.si_addr = (void *) fp->un.fmta.daddr;
		break;
	case 11:
		info.si_addr = (void *) fp->un.fmtb.daddr;
		break;
	}
	force_sig_info(sig, &info, current);
}

asmlinkage void set_esp0(unsigned long ssp)
{
	current->thread.esp0 = ssp;
}

/*
 * The architecture-independent backtrace generator
 */
void dump_stack(void)
{
	unsigned long stack;

	show_stack(current, &stack);
}
EXPORT_SYMBOL(dump_stack);

#ifdef CONFIG_M68KFPU_EMU
asmlinkage void fpemu_signal(int signal, int code, void *addr)
{
	siginfo_t info;

	info.si_signo = signal;
	info.si_errno = 0;
	info.si_code = code;
	info.si_addr = addr;
	force_sig_info(signal, &info, current);
}
#endif
