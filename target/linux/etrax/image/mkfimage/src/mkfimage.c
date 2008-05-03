#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
	unsigned char *buffer = malloc(64 * 1024);
	struct stat s;
	unsigned int size_vmlinux = 0, real_size_vmlinux = 0;
	const unsigned char *magic_str = "ACME_PART_MAGIC";
	unsigned int loop;
	unsigned char *magic;

	if(argc != 3){
		printf("%s in out\n", argv[0]);
		return 1;
	}

	printf("Generating image\n");

	FILE *vmlinux = fopen(argv[1], "r");
	FILE *vmlinux_out = fopen(argv[2], "w");
	if((!vmlinux) || (!vmlinux_out)){
		printf("Error opening a file\n");
		return 1;
	}

	stat(argv[1], &s);
	size_vmlinux = s.st_size;
	real_size_vmlinux = (size_vmlinux & 0xffff0000) + 0x10000;

	printf("vmlinux = 0x%.08X / 0x%.08X\n", size_vmlinux, real_size_vmlinux);

	unsigned int t = fread(buffer, 1, 64 * 1024, vmlinux);
	for(loop = 0; loop < (64 * 1024) - sizeof(magic_str); loop++){
		if(buffer[loop] == magic_str[0]){
			if((magic = strstr(&buffer[loop], magic_str))){
				printf("Magic at 0x%.08X %p %p\n", magic - buffer, magic, buffer);
				printf("Found Magic %X%X%X%X\n",
					buffer[loop + strlen(magic_str)],
					buffer[loop + strlen(magic_str) + 2],
					buffer[loop + strlen(magic_str) + 1],
					buffer[loop + strlen(magic_str) + 3]);

				buffer[loop + strlen(magic_str)] = real_size_vmlinux >> 24;
				buffer[loop + strlen(magic_str) + 2] = (real_size_vmlinux >> 16) & 0xff;
				buffer[loop + strlen(magic_str) + 1] = (real_size_vmlinux >> 8) & 0xff;
				buffer[loop + strlen(magic_str) + 3] = (real_size_vmlinux) & 0xff;

				printf("Replaced with %.02X%.02X%.02X%.02X\n",
					buffer[loop + strlen(magic_str)],
					buffer[loop + strlen(magic_str) + 2],
					buffer[loop + strlen(magic_str) + 1],
					buffer[loop + strlen(magic_str) + 3]);

			}
		}
	}

	fwrite(buffer, 1, 64 * 1024, vmlinux_out);
	real_size_vmlinux -= 64 * 1024;
	do {
		real_size_vmlinux -= 64 * 1024;
		memset(buffer, 0, 64 * 1024);
		fread(buffer, 1, 64 * 1024, vmlinux);
		fwrite(buffer, 1, 64 * 1024, vmlinux_out);
	} while (real_size_vmlinux);

	return 0;
}
