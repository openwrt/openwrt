/*
 * arch/ubicom32/kernel/module.c
 *   Ubicom32 architecture loadable module support.
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
#include <linux/moduleloader.h>
#include <linux/bug.h>
#include <linux/elf.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/ocm-alloc.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(fmt...)
#endif

static void _module_free_ocm(struct module *mod)
{
	printk(KERN_INFO "module arch cleanup %s: OCM instruction memory free "
	       " of %d @%p\n", mod->name, mod->arch.ocm_inst_size,
	       mod->arch.ocm_inst);

	if (mod->arch.ocm_inst) {
		ocm_inst_free(mod->arch.ocm_inst);
		mod->arch.ocm_inst = 0;
		mod->arch.ocm_inst_size = 0;
	}
}

void *module_alloc(unsigned long size)
{
	if (size == 0)
		return NULL;
	return vmalloc(size);
}


/* Free memory returned from module_alloc */
void module_free(struct module *mod, void *module_region)
{
	vfree(module_region);
	/* FIXME: If module_region == mod->init_region, trim exception
	   table entries. */

	/*
	 * This is expected to be final module free, use this to prune the
	 * ocm
	 */
	if (module_region && module_region == mod->module_core)
		_module_free_ocm(mod);

}

/*
 * module_frob_arch_sections()
 *	Called from kernel/module.c allowing arch specific handling of
 *	sections/headers.
 */
int module_frob_arch_sections(Elf_Ehdr *hdr,
			      Elf_Shdr *sechdrs,
			      char *secstrings,
			      struct module *mod)
{
	Elf_Shdr *s, *sechdrs_end;
	void *ocm_inst = NULL;
	int ocm_inst_size = 0;

	/*
	 * Ubicom32 v3 and v4 are almost binary compatible but not completely.
	 * To be safe check that the module was compiled with the correct -march
	 * which is flags.
	 */
#ifdef CONFIG_UBICOM32_V4
	if ((hdr->e_flags & 0xFFFF) != EF_UBICOM32_V4) {
		printk(KERN_WARNING "Module %s was not compiled for "
		       "ubicom32v4, elf_flags:%x,\n",
		       mod->name, hdr->e_flags);
		return -ENOEXEC;
	}
#elif defined CONFIG_UBICOM32_V3
	if ((hdr->e_flags & 0xFFFF) != EF_UBICOM32_V3) {
		printk(KERN_WARNING "Module %s was not compiled for "
		       "ubicom32v3, elf_flags:%x\n",
		       mod->name, hdr->e_flags);
		return -ENOEXEC;
	}
#else
#error Unknown/Unsupported ubicom32 architecture.
#endif

	/*
	 * XXX: sechdrs are vmalloced in kernel/module.c
	 * and would be vfreed just after module is loaded,
	 * so we hack to keep the only information we needed
	 * in mod->arch to correctly free L1 I/D sram later.
	 * NOTE: this breaks the semantic of mod->arch structure.
	 */
	sechdrs_end = sechdrs + hdr->e_shnum;
	for (s = sechdrs; s < sechdrs_end; ++s) {
		if (strncmp(".ocm_text", secstrings + s->sh_name, 9) == 0)
			ocm_inst_size += s->sh_size;
	}

	if (!ocm_inst_size)
		return 0;

	ocm_inst = ocm_inst_alloc(ocm_inst_size, 0 /* internal */);
	if (ocm_inst == NULL) {
#ifdef CONFIG_OCM_MODULES_FALLBACK_TO_DDR
		printk(KERN_WARNING
		       "module %s: OCM instruction memory allocation of %d"
		       "failed, fallback to DDR\n", mod->name, ocm_inst_size);
		return 0;
#else
		printk(KERN_ERR
		       "module %s: OCM instruction memory allocation of %d"
		       "failed.\n", mod->name, ocm_inst_size);
		return -ENOMEM;
#endif
	}

	mod->arch.ocm_inst = ocm_inst;
	mod->arch.ocm_inst_size = ocm_inst_size;

	printk(KERN_INFO
	       "module %s: OCM instruction memory allocation of %d @%p\n",
	       mod->name, mod->arch.ocm_inst_size, mod->arch.ocm_inst);

	for (s = sechdrs; s < sechdrs_end; ++s) {
		if (strncmp(".ocm_text", secstrings + s->sh_name, 9) == 0) {
			memcpy(ocm_inst, (void *)s->sh_addr, s->sh_size);
			s->sh_flags &= ~SHF_ALLOC;
			s->sh_addr = (unsigned long)ocm_inst;
			ocm_inst += s->sh_size;
		}
	}

	return 0;
}

int apply_relocate(Elf32_Shdr *sechdrs,
		   const char *strtab,
		   unsigned int symindex,
		   unsigned int relsec,
		   struct module *me)
{
	DEBUGP("Invalid Applying relocate section %u to %u\n", relsec,
	       sechdrs[relsec].sh_info);
	return -EINVAL;
}

int apply_relocate_add(Elf32_Shdr *sechdrs,
		       const char *strtab,
		       unsigned int symindex,
		       unsigned int relsec,
		       struct module *me)
{
	unsigned int i;
	Elf32_Rela *rel = (void *)sechdrs[relsec].sh_addr;
	Elf32_Sym *sym;
	uint32_t *location;
	uint32_t insn;

	DEBUGP("Applying relocate_add section %u to %u\n", relsec,
	       sechdrs[relsec].sh_info);
	for (i = 0; i < sechdrs[relsec].sh_size / sizeof(*rel); i++) {
		uint32_t v;
		const int elf32_rtype = ELF32_R_TYPE(rel[i].r_info);

		/* This is where to make the change */
		location = (void *)sechdrs[sechdrs[relsec].sh_info].sh_addr
			+ rel[i].r_offset;
		/* This is the symbol it is referring to.  Note that all
		   undefined symbols have been resolved.  */
		sym = (Elf32_Sym *)sechdrs[symindex].sh_addr
			+ ELF32_R_SYM(rel[i].r_info);

		v = rel[i].r_addend + sym->st_value;


		switch (elf32_rtype) {
		case R_UBICOM32_32:
		{
			/*
			 * Store the 32 bit relocation as is.
			 */
			*location = v;
			break;
		}
		case R_UBICOM32_HI24:
		{
			/*
			 * 24 bit relocation that is part of the MOVEAI
			 * instruction. The 24 bits come from bits 7 - 30 of the
			 * relocation. Theses bits eventually get split into 2
			 * fields in the instruction encoding.
			 *
			 * - Bits 7 - 27 of the relocation are encoded into bits
			 * 0 - 20 of the instruction.
			 *
			 *  - Bits 28 - 30 of the relocation are encoded into
			 *  bit 24 - 26 of the instruction.
			 */
			uint32_t valid24 = (v >> 7) & 0xffffff;
			insn = *location;

			insn &= ~(0x1fffff | (0x7 << 24));
			insn |= (valid24 & 0x1fffff);
			insn |= ((valid24 & 0xe00000) << 3);
			*location = insn;
		}
		break;
		case R_UBICOM32_LO7_S:
		case R_UBICOM32_LO7_2_S:
		case R_UBICOM32_LO7_4_S:
		{
			/*
			 * Bits 0 - 6 of the relocation are encoded into the
			 * 7bit unsigned immediate fields of the SOURCE-1 field
			 * of the instruction.  The immediate value is left
			 * shifted by (0, 1, 2) based on the operand size.
			 */
			uint32_t valid7 = v & 0x7f;
			insn = *location;

			if (elf32_rtype == R_UBICOM32_LO7_2_S) {
				valid7 >>= 1;
			} else if (elf32_rtype == R_UBICOM32_LO7_4_S) {
				valid7 >>= 2;
			}

			insn &= ~(0x1f | (0x3 << 8));
			insn |= (valid7 & 0x1f);
			insn |= ((valid7 & 0x60) << 3);
			*location = insn;
		}
		break;
		case R_UBICOM32_LO7_D:
		case R_UBICOM32_LO7_2_D:
		case R_UBICOM32_LO7_4_D:
		{
			/*
			 * Bits 0 - 6 of the relocation are encoded into the
			 * 7bit unsigned immediate fields of the DESTINATION
			 * field of the instruction.  The immediate value is
			 * left shifted by (0, 1, 2) based on the operand size.
			 */
			uint32_t valid7 = v & 0x7f;
			insn = *location;

			if (elf32_rtype == R_UBICOM32_LO7_2_D) {
				valid7 >>= 1;
			} else if (elf32_rtype == R_UBICOM32_LO7_4_D) {
				valid7 >>= 2;
			}

			insn &= ~((0x1f | (0x3 << 8)) << 16);
			insn |= ((valid7 & 0x1f) << 16);
			insn |= ((valid7 & 0x60) << 19);
			*location = insn;
		}
		break;
		case R_UBICOM32_LO7_CALLI:
		case R_UBICOM32_LO16_CALLI:
		{
			/*
			 * Extract the offset for a CALLI instruction. The
			 * offsets can be either 7 bits or 18 bits. Since all
			 * instructions in ubicom32 architecture are at work
			 * aligned addresses the truncated offset is right
			 * shifted by 2 before being encoded in the instruction.
			 */
			uint32_t val;
			if (elf32_rtype == R_UBICOM32_LO7_CALLI) {
				val  = v & 0x7f;
			} else {
				val  = v & 0x3ffff;
			}

			val >>= 2;

			insn = *location;

			insn &= ~0x071f071f;
			insn |= (val & 0x1f) << 0;
			val >>= 5;
			insn |= (val & 0x07) << 8;
			val >>= 3;
			insn |= (val & 0x1f) << 16;
			val >>= 5;
			insn |= (val & 0x07) << 24;
			*location = insn;
		}
		break;
		case R_UBICOM32_24_PCREL:
		{
			/*
			 * Extract 26 bit signed PC relative offset for CALL
			 * instructions. Since instruction addresses are word
			 * aligned the offset is right shited by 2 before
			 * encoding into instruction.
			 */
			int32_t val = v - (int32_t)location;

			/*
			 * Check that the top 7 bits are all equal to the sign
			 * bit (26), i.e all 0's or all 1's.  If they are not then
			 * the absolute difference is greater than 25 bits.
			 */
			if (((uint32_t)val & 0xFE000000) != 0xFE000000 &&
				((uint32_t)val & 0xFE000000) != 0x0) {
				/*
				 * The relocation is beyond our addressable
				 * range with a 26 bit call.
				 */
				printk(KERN_ERR "module %s: PC Relative "
					"relocation out of range: "
					"%u (%x->%x, %x)\n",
					me->name, elf32_rtype,
					v, (uint32_t) location, val);
				return -ENOEXEC;
			}

			val = (val & 0x3ffffff) >> 2;
			insn = *location;
			insn = insn & 0xf8e00000;

			insn |= (val >> 21) << 24;
			insn |= (val & 0x1fffff);
			*location = insn;
		}
		break;
		case R_UBICOM32_LO16:
		case R_UBICOM32_HI16:
		{
			/*
			 * 16 bit immediate value that is encoded into bit 0 -
			 * 15 of the instruction.
			 */
			uint32_t val;

			if (elf32_rtype == R_UBICOM32_LO16) {
				val  = v & 0xffff;
			} else {
				val  = (v >> 16) & 0xffff;
			}

			insn = *location;
			insn &= 0xffff0000;

			insn |= val;
			*location = insn;
		}
		break;
		case R_UBICOM32_21_PCREL:
		{
			/*
			 * Extract 23 bit signed PC relative offset for JMP<cc>
			 * instructions. Since instruction addresses are word
			 * aligned the offset is right shited by 2 before
			 * encoding into instruction.
			 */
			int32_t val = v - (int32_t)location;

			val = (val & 0x7fffff) >> 2;
			insn = *location;
			insn = insn & 0xffe00000;

			insn |= (val >> 21) << 24;
			insn |= val;
			*location = insn;
		}
		break;
		default:
			BUG();
			printk(KERN_ERR "module %s: Unknown relocation: %u\n",
			       me->name, elf32_rtype);
			return -ENOEXEC;
		}
	}
	return 0;
}

int module_finalize(const Elf_Ehdr *hdr,
		    const Elf_Shdr *sechdrs,
		    struct module *mod)
{
	unsigned int i, strindex = 0, symindex = 0;
	char *secstrings;
	int err;

	err = module_bug_finalize(hdr, sechdrs, mod);
	if (err)
		return err;

	if (!mod->arch.ocm_inst) {
		/*
		 * No OCM code, so nothing more to do.
		 */
		return 0;
	}

	secstrings = (void *)hdr + sechdrs[hdr->e_shstrndx].sh_offset;

	for (i = 1; i < hdr->e_shnum; i++) {
		/* Internal symbols and strings. */
		if (sechdrs[i].sh_type == SHT_SYMTAB) {
			symindex = i;
			strindex = sechdrs[i].sh_link;
		}
	}

	for (i = 1; i < hdr->e_shnum; i++) {
		const char *strtab = (char *)sechdrs[strindex].sh_addr;
		unsigned int info = sechdrs[i].sh_info;

		/* Not a valid relocation section? */
		if (info >= hdr->e_shnum)
			continue;

		if ((sechdrs[i].sh_type == SHT_RELA) &&
		    (strncmp(".rela.ocm_text",
			     secstrings + sechdrs[i].sh_name, 5 + 9) == 0)) {
			err = apply_relocate_add((Elf_Shdr *) sechdrs, strtab,
						 symindex, i, mod);
			if (err)
				return err;
		}
	}

	return 0;
}

void module_arch_cleanup(struct module *mod)
{
	module_bug_cleanup(mod);
}
