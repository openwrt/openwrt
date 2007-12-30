.text
#include <linux/linkage.h>
#include <asm/segment.h>
#include <asm/page.h>

ALIGN
	.align 4096

wakeup_start:
#	jmp wakeup_start

	cli
	cld

	# Clear any dangerous flags
	
	pushl $0
	popfl
		
	# Set up %cr3
	movl $swsusp_pg_dir - __PAGE_OFFSET, %eax
	movl %eax, %cr3

	movl saved_cr4, %eax
	movl %eax, %cr4
	
	movl saved_cr0, %eax
	movl %eax, %cr0
	
	jmp 1f
1:	
	ljmpl $__KERNEL_CS,$wakeup_return
	

.org 0x1000

wakeup_return:
	movw    $__KERNEL_DS, %ax
	movw    %ax, %ss
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %fs
	movw    %ax, %gs
	
	lgdt    saved_gdt
	lidt    saved_idt
	lldt    saved_ldt
	ljmp    $(__KERNEL_CS),$1f
1:
	movl    %cr3, %eax
	movl    %eax, %cr3
	wbinvd

	# Go back to the return point
	jmp ret_point
	
save_registers:
	sgdt  saved_gdt
	sidt  saved_idt
	sldt  saved_ldt

	pushl %edx
	movl %cr4, %edx
	movl %edx, saved_cr4

	movl %cr0, %edx
	movl %edx, saved_cr0

	popl %edx
		
	
        movl %ebx, saved_context_ebx
        movl %ebp, saved_context_ebp
        movl %esi, saved_context_esi
        movl %edi, saved_context_edi

	pushfl
	popl saved_context_eflags
	
        ret


restore_registers:
        movl saved_context_ebp, %ebp
        movl saved_context_ebx, %ebx
        movl saved_context_esi, %esi
        movl saved_context_edi, %edi

	pushl saved_context_eflags
	popfl
	
        ret

	
ENTRY(do_olpc_suspend_lowlevel)
	call	save_processor_state
	call	save_registers

	# This is the stack context we want to remember
	movl %esp, saved_context_esp

	pushl	$3
	call	olpc_do_sleep
	
	jmp	wakeup_start
	.p2align 4,,7
ret_point:
	movl    saved_context_esp, %esp

	call	restore_registers
	call	restore_processor_state
	ret

.data
ALIGN

saved_gdt:     .long   0,0
saved_idt:     .long   0,0
saved_ldt:     .long   0
saved_cr4:     .long   0
saved_cr0:     .long   0

