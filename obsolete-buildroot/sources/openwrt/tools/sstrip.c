/* http://www.muppetlabs.com/~breadbox/software/elfkickers.html */

/* sstrip: Copyright (C) 1999-2001 by Brian Raiter, under the GNU
 * General Public License. No warranty. See COPYING for details.
 */

#define __MIPSEL__ 1
#define _MIPS_SZLONG 32

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<elf.h>
#include	<linux/bitops.h>

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef signed long long s64;
typedef unsigned long long u64;

#include	<asm/elf.h>

#ifndef TRUE
#define	TRUE		1
#define	FALSE		0
#endif

#if ELF_CLASS == ELFCLASS32
#define	Elf_Ehdr	Elf32_Ehdr
#define	Elf_Phdr	Elf32_Phdr
#else
#define	Elf_Ehdr	Elf64_Ehdr
#define	Elf_Phdr	Elf64_Phdr
#endif

/* The name of the program.
 */
static char const      *progname;

/* The name of the current file.
 */
static char const      *filename;


/* A simple error-handling function. FALSE is always returned for the
 * convenience of the caller.
 */
static int err(char const *errmsg)
{
    fprintf(stderr, "%s: %s: %s\n", progname, filename, errmsg);
    return FALSE;
}

/* A macro for I/O errors: The given error message is used only when
 * errno is not set.
 */
#define	ferr(msg)	(err(errno ? strerror(errno) : (msg)))

/* readelfheader() reads the ELF header into our global variable, and
 * checks to make sure that this is in fact a file that we should be
 * munging.
 */
static int readelfheader(int fd, Elf_Ehdr *ehdr)
{
    errno = 0;
    if (read(fd, ehdr, sizeof *ehdr) != sizeof *ehdr)
	return ferr("missing or incomplete ELF header.");

    /* Check the ELF signature.
     */
    if (!(ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
	  ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
	  ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
	  ehdr->e_ident[EI_MAG3] == ELFMAG3))
	return err("missing ELF signature.");

    /* Compare the file's class and endianness with the program's.
     */
    if (ehdr->e_ident[EI_DATA] != ELF_DATA)
	return err("ELF file has different endianness.");
    if (ehdr->e_ident[EI_CLASS] != ELF_CLASS)
	return err("ELF file has different word size.");

    /* Check the target architecture.
     */
    if (ehdr->e_machine != ELF_ARCH)
	return err("ELF file created for different architecture.");
    /* Verify the sizes of the ELF header and the program segment
     * header table entries.
     */
    if (ehdr->e_ehsize != sizeof(Elf_Ehdr))
	return err("unrecognized ELF header size.");
    if (ehdr->e_phentsize != sizeof(Elf_Phdr))
	return err("unrecognized program segment header size.");

    /* Finally, check the file type.
     */
    if (ehdr->e_type != ET_EXEC && ehdr->e_type != ET_DYN)
	return err("not an executable or shared-object library.");

    return TRUE;
}

/* readphdrtable() loads the program segment header table into memory.
 */
static int readphdrtable(int fd, Elf_Ehdr const *ehdr, Elf_Phdr **phdrs)
{
    size_t	size;

    if (!ehdr->e_phoff || !ehdr->e_phnum)
	return err("ELF file has no program header table.");

    size = ehdr->e_phnum * sizeof **phdrs;
    if (!(*phdrs = malloc(size)))
	return err("Out of memory!");

    errno = 0;
    if (read(fd, *phdrs, size) != (ssize_t)size)
	return ferr("missing or incomplete program segment header table.");

    return TRUE;
}

/* getmemorysize() determines the offset of the last byte of the file
 * that is referenced by an entry in the program segment header table.
 * (Anything in the file after that point is not used when the program
 * is executing, and thus can be safely discarded.)
 */
static int getmemorysize(Elf_Ehdr const *ehdr, Elf_Phdr const *phdrs,
			 unsigned long *newsize)
{
    Elf32_Phdr const   *phdr;
    unsigned long	size, n;
    int			i;

    /* Start by setting the size to include the ELF header and the
     * complete program segment header table.
     */
    size = ehdr->e_phoff + ehdr->e_phnum * sizeof *phdrs;
    if (size < sizeof *ehdr)
	size = sizeof *ehdr;

    /* Then keep extending the size to include whatever data the
     * program segment header table references.
     */
    for (i = 0, phdr = phdrs ; i < ehdr->e_phnum ; ++i, ++phdr) {
	if (phdr->p_type != PT_NULL) {
	    n = phdr->p_offset + phdr->p_filesz;
	    if (n > size)
		size = n;
	}
    }

    *newsize = size;
    return TRUE;
}

/* truncatezeros() examines the bytes at the end of the file's
 * size-to-be, and reduces the size to exclude any trailing zero
 * bytes.
 */
static int truncatezeros(int fd, unsigned long *newsize)
{
    unsigned char	contents[1024];
    unsigned long	size, n;

    size = *newsize;
    do {
	n = sizeof contents;
	if (n > size)
	    n = size;
	if (lseek(fd, size - n, SEEK_SET) == (off_t)-1)
	    return ferr("cannot seek in file.");
	if (read(fd, contents, n) != (ssize_t)n)
	    return ferr("cannot read file contents");
	while (n && !contents[--n])
	    --size;
    } while (size && !n);

    /* Sanity check.
     */
    if (!size)
	return err("ELF file is completely blank!");

    *newsize = size;
    return TRUE;
}

/* modifyheaders() removes references to the section header table if
 * it was stripped, and reduces program header table entries that
 * included truncated bytes at the end of the file.
 */
static int modifyheaders(Elf_Ehdr *ehdr, Elf_Phdr *phdrs,
			 unsigned long newsize)
{
    Elf32_Phdr *phdr;
    int		i;

    /* If the section header table is gone, then remove all references
     * to it in the ELF header.
     */
    if (ehdr->e_shoff >= newsize) {
	ehdr->e_shoff = 0;
	ehdr->e_shnum = 0;
	ehdr->e_shentsize = 0;
	ehdr->e_shstrndx = 0;
    }

    /* The program adjusts the file size of any segment that was
     * truncated. The case of a segment being completely stripped out
     * is handled separately.
     */
    for (i = 0, phdr = phdrs ; i < ehdr->e_phnum ; ++i, ++phdr) {
	if (phdr->p_offset >= newsize) {
	    phdr->p_offset = newsize;
	    phdr->p_filesz = 0;
	} else if (phdr->p_offset + phdr->p_filesz > newsize) {
	    phdr->p_filesz = newsize - phdr->p_offset;
	}
    }

    return TRUE;
}

/* commitchanges() writes the new headers back to the original file
 * and sets the file to its new size.
 */
static int commitchanges(int fd, Elf_Ehdr const *ehdr, Elf_Phdr *phdrs,
			 unsigned long newsize)
{
    size_t	n;

    /* Save the changes to the ELF header, if any.
     */
    if (lseek(fd, 0, SEEK_SET))
	return ferr("could not rewind file");
    errno = 0;
    if (write(fd, ehdr, sizeof *ehdr) != sizeof *ehdr)
	return err("could not modify file");

    /* Save the changes to the program segment header table, if any.
     */
    if (lseek(fd, ehdr->e_phoff, SEEK_SET) == (off_t)-1) {
	err("could not seek in file.");
	goto warning;
    }
    n = ehdr->e_phnum * sizeof *phdrs;
    if (write(fd, phdrs, n) != (ssize_t)n) {
	err("could not write to file");
	goto warning;
    }

    /* Eleventh-hour sanity check: don't truncate before the end of
     * the program segment header table.
     */
    if (newsize < ehdr->e_phoff + n)
	newsize = ehdr->e_phoff + n;

    /* Chop off the end of the file.
     */
    if (ftruncate(fd, newsize)) {
	err("could not resize file");
	goto warning;
    }

    return TRUE;

  warning:
    return err("ELF file may have been corrupted!");
}

/* main() loops over the cmdline arguments, leaving all the real work
 * to the other functions.
 */
int main(int argc, char *argv[])
{
    int			fd;
    Elf_Ehdr		ehdr;
    Elf_Phdr	       *phdrs;
    unsigned long	newsize;
    char	      **arg;
    int			failures = 0;

    if (argc < 2 || argv[1][0] == '-') {
	printf("Usage: sstrip FILE...\n"
	       "sstrip discards all nonessential bytes from an executable.\n\n"
	       "Version 2.0 Copyright (C) 2000,2001 Brian Raiter.\n"
	       "This program is free software, licensed under the GNU\n"
	       "General Public License. There is absolutely no warranty.\n");
	return EXIT_SUCCESS;
    }

    progname = argv[0];

    for (arg = argv + 1 ; *arg != NULL ; ++arg) {
	filename = *arg;

	fd = open(*arg, O_RDWR);
	if (fd < 0) {
	    ferr("can't open");
	    ++failures;
	    continue;
	}

	if (!(readelfheader(fd, &ehdr)			&&
	      readphdrtable(fd, &ehdr, &phdrs)		&&
	      getmemorysize(&ehdr, phdrs, &newsize)	&&
	      truncatezeros(fd, &newsize)		&&
	      modifyheaders(&ehdr, phdrs, newsize)	&&
	      commitchanges(fd, &ehdr, phdrs, newsize)))
	    ++failures;

	close(fd);
    }

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
