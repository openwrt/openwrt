// author: Vonger Chin
// date:   May.29, 2015
// compile: gcc mem.c -o mem

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE    4096

void bit_print(const unsigned char* s, FILE *fp)
{
	uint32_t d = *(uint32_t *)s;
	int i;

	for (i = 31; i >= 16; i--)
		fprintf(fp, "%02d:%d ", i, (d >> i) & 0x1);
	fprintf(fp, "\n");
	for (i = 15; i >= 0; i--)
		fprintf(fp, "%02d:%d ", i, (d >> i) & 0x1);
	fprintf(fp, "\n");
}

void hex_print(const unsigned char* s, int size, FILE *fp)
{
	int i = 0, n = 0, c = 0, p = 0;

	// default print to stdout.
	if (fp == NULL)
		fp = stdout;

	while(1) {
		n = size > 0x10 ? 0x10 : size;
		size -= n;

		p += fprintf(fp, "%08X: ", c);
		for(i = 0; i < n; i++) {
			if(i == 8) {
				p += fprintf(fp, " ");
			}
			p += fprintf(fp, "%02X ", *(s + c + i));
		}
		for(i = n; i < 0x10; i++) {
			if(i == 8) {
				p += fprintf(fp, " ");
			}
			p += fprintf(fp, "   ");
		}
		p += fprintf(fp, "   ");
		for(i = 0; i < n; i++) {
			if(*(s + c + i) >= 0x20 && *(s + c + i) <= 0x7E) {
				p += fprintf(fp, "%c", *(s + c + i));
			} else {
				p += fprintf(fp, ".");
			}
		}
		p += fprintf(fp, "\n");
		if(size <= 0) {
			break;
		}
		c += n;
	}
}

// if c is not a valid char, return 0.
#define hexchar2int(c) \
	((c >= '0' && c <= '9') ? (c - '0') : ((c >= 'A' && c <= 'F') ? \
	c - 'A' + 0xA : ((c >= 'a' && c <= 'f') ? c - 'a' + 0xA : 0)))

#define octchar2int(c) \
	((c >= '0' && c <= '7') ? (c - '0') : 0)

#define decchar2int(c) \
	((c >= '0' && c <= '9') ? (c - '0') : 0)


// convert string to unsigned int.
// hex: 0x, oct: 0, bin: ...b, dec: normal.
unsigned int atou(char* s)
{
	char *p = s, *e = s;
	unsigned int r = 0;

	while(*e != '\0')
		e++;

	if(*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
		// hex: skip first 0x or 0X header.
		p += 2;

		while(p != e) {
			r <<= 4;
			r += hexchar2int(*p);
			p++;
		}
	} else if((e - p) >= 2 && *(e - 1) == 'b') {
		// bin: ignore last b.
		e--;

		while(p != e) {
			r <<= 1;
			r += (*p == '0' ? 0 : 1);
			p++;
		}
	} else if(*p == '0') {
		// oct: skip first 0 header.
		p += 1;

		while(p != e) {
			r <<= 3;
			r += octchar2int(*p);
			p++;
		}
	} else {
		// dec: we do not need to deal the number.
		while(p != e) {
			r *= 10;
			r += decchar2int(*p);
			p++;
		}
	}

	return r;
}


// convert string to unsigned int.
// hex: 0x, oct: 0, bin: ...b, dec: normal.
unsigned int atox(char* s)
{
	char *p = s, *e = s;
	unsigned int r = 0;

	while(*e != '\0')
		e++;

	if(*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
		// hex: skip first 0x or 0X header.
		p += 2;

		while(p != e) {
			r <<= 4;
			r += ((*p >= '0' && *p <= '9') ? (*p - '0') : ((*p >= 'A' && *p <= 'F') ?
			    *p - 'A' + 0xA : ((*p >= 'a' && *p <= 'f') ? *p - 'a' + 0xA : 0)));
			p++;
		}
	} else if((e - p) >= 2 && *(e - 1) == 'b') {
		// bin: ignore last b.
		e--;

		while(p != e) {
			r <<= 1;
			r += (*p == '0' ? 0 : 1);
			p++;
		}
	} else if(*p == '0') {
		// oct: skip first 0 header.
		p += 1;

		while(p != e) {
			r <<= 3;
			r += ((*p >= '0' && *p <= '7') ? (*p - '0') : 0);
			p++;
		}
	} else {
		// dec: we do not need to deal the number.
		while(p != e) {
			r *= 10;
			r += ((*p >= '0' && *p <= '9') ? (*p - '0') : 0);
			p++;
		}
	}

	return r;
}


// get one page, system memory map by page number.
unsigned char *mmap_page(unsigned int page)
{
	int fd = -1;
	unsigned char *mem = NULL;

	fd = open("/dev/mem", O_RDWR);
	if(fd < 0)
		return NULL;

	mem = (unsigned char *)mmap(0, PAGE_SIZE,
				    PROT_READ | PROT_WRITE, MAP_SHARED, fd, page * PAGE_SIZE);
	close(fd);

	return mem;
}

int main(int argc, char *argv[])
{
	unsigned char *mem = NULL;
	unsigned int pos = 0, page = 0;
	unsigned int addr = 0, size = 0, data = 0;
	int bitmode = 0;

	if (argc == 1) {
		printf("usage: mem [addr] [value] [bit]\n");
		return 0;
	}

	// show address data in 32 bit, for read only.
	if (!strncmp(argv[argc - 1], "bit", 3)) {
		bitmode = 1;
		argc--;
	}

	switch(argc) {
	case 2: { // memory show mode.
		addr = atou(argv[1]);
		size = 0x40;

		page = addr / PAGE_SIZE;
		pos = addr % PAGE_SIZE;

		if(size + pos > PAGE_SIZE) {
			size = PAGE_SIZE - pos;
			printf("warning: size exceed one page, resize to %d.\n", size);
		}

		mem = mmap_page(page);
		if(mem == NULL) {
			printf("can not map memory.\n");
			return -1;
		}

		// print request memory area.
		if (bitmode)
			bit_print(mem + pos, stdout);
		else
			hex_print(mem + pos, size, stdout);

		munmap(mem, PAGE_SIZE);
		break; }

	case 3: // memory set mode.
		addr = atou(argv[1]);
		data = atou(argv[2]);

		page = addr / PAGE_SIZE;
		pos = addr % PAGE_SIZE;

		mem = mmap_page(page);
		if(mem == NULL) {
			printf("can not map memory.\n");
			return -1;
		}

		printf("0x%08X: 0x%08X => ", addr, *(unsigned int *)(mem + pos));
		*((unsigned int *)(mem + pos)) = data;
		printf("0x%08X\n", *(unsigned int *)(mem + pos));

		munmap(mem, PAGE_SIZE);
		break;

	default:
		printf("usage: \n"
		       "\tmem [addr]: show 0x40 bytes at addr.\n"
		       "\tmem [addr] [data]: set addr with data(32bit).\n");
		break;
	}
	return 0;
}

