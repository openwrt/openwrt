#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>

#define ZONE_SIZE 2576
#define STR_SIZE 256
#define MOD_ADLER 65521

#define OFF_CSUM  0x00
#define OFF_MAGIC 0x04
#define OFF_RS	0x08
#define OFF_BS	0x0C
#define OFF_STR_A 0x10
#define OFF_STR_B 0x110

#define ZMSH_MAGIC 0x5A4D5348

typedef struct {
	uint32_t checksum;
	uint32_t magic;
	uint32_t run_status;
	uint32_t boot_status;
	char str_a[STR_SIZE];
	char str_b[STR_SIZE];
} ZMSHLayout;

static uint32_t read_u32(const unsigned char *buf, int offset);
static void write_u32(unsigned char *buf, int offset, uint32_t val);

static void zms_layout_init(ZMSHLayout *layout) {
	memset(layout, 0, sizeof(*layout));
	layout->magic = ZMSH_MAGIC;
}

static void zms_layout_from_buf(const unsigned char *buf, ZMSHLayout *layout) {
	layout->checksum = read_u32(buf, OFF_CSUM);
	layout->magic = read_u32(buf, OFF_MAGIC);
	layout->run_status = read_u32(buf, OFF_RS);
	layout->boot_status = read_u32(buf, OFF_BS);
	memcpy(layout->str_a, buf + OFF_STR_A, STR_SIZE);
	memcpy(layout->str_b, buf + OFF_STR_B, STR_SIZE);
}

static void zms_layout_to_buf(unsigned char *buf, const ZMSHLayout *layout) {
	write_u32(buf, OFF_CSUM, layout->checksum);
	write_u32(buf, OFF_MAGIC, layout->magic);
	write_u32(buf, OFF_RS, layout->run_status);
	write_u32(buf, OFF_BS, layout->boot_status);
	memcpy(buf + OFF_STR_A, layout->str_a, STR_SIZE);
	memcpy(buf + OFF_STR_B, layout->str_b, STR_SIZE);
}

static uint32_t calculate_custom_checksum(const unsigned char *data, size_t len);

static uint32_t zms_layout_checksum(const unsigned char *base_buf, const ZMSHLayout *layout) {
	unsigned char calc_buf[ZONE_SIZE];
	memcpy(calc_buf, base_buf, ZONE_SIZE);
	memset(calc_buf, 0, 4);
	write_u32(calc_buf, OFF_MAGIC, layout->magic);
	write_u32(calc_buf, OFF_RS, layout->run_status);
	write_u32(calc_buf, OFF_BS, layout->boot_status);
	memcpy(calc_buf + OFF_STR_A, layout->str_a, STR_SIZE);
	memcpy(calc_buf + OFF_STR_B, layout->str_b, STR_SIZE);
	return calculate_custom_checksum(calc_buf + 4, ZONE_SIZE - 4);
}

#define ERR_PARAM		1
#define ERR_IO			2
#define ERR_INVALID		3
#define ERR_INVALID_VALUE	4
#define ERR_PARTITION_NOT_FOUND	5
#define ERR_INPUT_OPEN_FAILED   6
#define ERR_READ_FAILED		7
#define ERR_OUTPUT_OPEN_FAILED	8
#define ERR_WRITE_FAILED	9
#define ERR_FSYNC_FAILED	10
#define ERR_INVALID_MAGIC	11

uint32_t read_u32(const unsigned char *buf, int offset) {
	return buf[offset] | (buf[offset + 1] << 8) |
		   (buf[offset + 2] << 16) | (buf[offset + 3] << 24);
}

void write_u32(unsigned char *buf, int offset, uint32_t val) {
	buf[offset]	 = (unsigned char)(val & 0xFF);
	buf[offset + 1] = (unsigned char)((val >> 8) & 0xFF);
	buf[offset + 2] = (unsigned char)((val >> 16) & 0xFF);
	buf[offset + 3] = (unsigned char)((val >> 24) & 0xFF);
}

uint32_t safe_parse_u32(const char *str, int parsable) {
	char *endptr;
	errno = 0;
	unsigned long val = strtoul(str, &endptr, 0);

	if (errno != 0 || *endptr != '\0' || endptr == str || val > UINT32_MAX) {
		if (!parsable) fprintf(stderr, "Error: Invalid numeric value '%s'\n", str);
		exit(ERR_INVALID_VALUE);
	}
	return (uint32_t)val;
}

uint32_t calculate_custom_checksum(const unsigned char *data, size_t len) {
	uint32_t a = 0, b = 0;
	for (size_t i = 0; i < len; i++) {
		a = (a + data[i]) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}
	return (b << 16) | (a & 0xFFFF);
}

int resolve_partition(const char *input, char *out_path, size_t max_len) {
	struct stat st;
	if (input[0] != '\0' && stat(input, &st) == 0 && !S_ISDIR(st.st_mode)) {
		snprintf(out_path, max_len, "%s", input);
		return 0;
	}
	if (input[0] == '/') {
		snprintf(out_path, max_len, "%s", input);
		return 0;
	}
	FILE *fp = fopen("/proc/mtd", "r");
	if (!fp) return -1;
	char line[256];
	int mtd_num = -1;
	while (fgets(line, sizeof(line), fp)) {
		char mtd_name[128];
		int num;
		if (sscanf(line, "mtd%d: %*x %*x \"%127[^\"]\"", &num, mtd_name) == 2) {
			if (strcmp(mtd_name, input) == 0) {
				mtd_num = num;
				break;
			}
		}
	}
	fclose(fp);
	if (mtd_num >= 0) {
		snprintf(out_path, max_len, "/dev/mtd%d", mtd_num);
		return 0;
	}
	return -1;
}

void safe_read(int fd, unsigned char *buf, int parsable) {
	if (pread(fd, buf, ZONE_SIZE, 0) != ZONE_SIZE) {
		if (!parsable) fprintf(stderr, "Error: Failed to read %d bytes\n", ZONE_SIZE);
		exit(ERR_READ_FAILED);
	}
}

static int mtd_block_path(const char *path, char *out, size_t out_len) {
	const char *prefix = "/dev/mtd";
	size_t prefix_len = strlen(prefix);
	if (strncmp(path, prefix, prefix_len) != 0) return 0;
	snprintf(out, out_len, "/dev/mtdblock%s", path + prefix_len);
	return 1;
}

static int is_regular_file(const char *path) {
	struct stat st;
	return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

void safe_write(int fd, const unsigned char *buf, int parsable, const char *path) {
	char block_path[PATH_MAX];
	if (mtd_block_path(path, block_path, sizeof(block_path))) {
		int block_fd = open(block_path, O_WRONLY);
		if (block_fd >= 0) {
			if (pwrite(block_fd, buf, ZONE_SIZE, 0) == (ssize_t)ZONE_SIZE) {
				if (fsync(block_fd) < 0) {
					if (!parsable) perror("Error: fsync failed");
					close(block_fd);
					exit(ERR_FSYNC_FAILED);
				}
				close(block_fd);
				return;
			}
			close(block_fd);
		}
	}

	if (pwrite(fd, buf, ZONE_SIZE, 0) == (ssize_t)ZONE_SIZE) {
		if (fsync(fd) < 0) {
			if (!parsable) perror("Error: fsync failed");
			exit(ERR_FSYNC_FAILED);
		}
		return;
	}

	if (!parsable) perror("Error: Failed to write to output");
	exit(ERR_WRITE_FAILED);
}

void sign_buffer(ZMSHLayout *layout, const unsigned char *base_buf) {
	layout->checksum = zms_layout_checksum(base_buf, layout);
}

void print_usage(const char *prog) {
	fprintf(stderr, "Usage: %s [options] <command> <partition> [target] [value]\n\n"
	                "Options:\n"
	                "  -i, --input <file>    Read from file\n"
	                "  -o, --output <file>   Write to file\n"
	                "  -s, --parsable        Parsable output\n"
	                "  -f, --force           Force bypass magic check\n"
	                "  -h, --help            Show this help\n\n"
	                "Commands:\n"
	                "  validate              Check Magic & Checksum\n"
	                "  toggle                Flip BootStatus (0<->1)\n"
	                "  stage                 Set RunStatus to 1\n"
	                "  commit                Set RunStatus to 0\n"
	                "  get <target>          Read target (all|csum|magic|rs|bs|str_a|str_b)\n"
	                "  set <target> [val]    Write target (magic|rs|bs|str_a|str_b)\n"
	                "  new                   Create fresh image with default magic\n"
	                "  copy                  Raw copy (no signing, requires input)\n\n"
	                "Exit codes:\n"
	                "  0  success\n"
	                "  1  invalid parameters / usage\n"
	                "  2  I/O failure\n"
	                "  3  invalid image / validation failed\n"
	                "  4  invalid numeric value\n"
	                "  5  partition not found\n"
	                "  6  input open failed\n"
	                "  7  read failed\n"
	                "  8  output open failed\n"
	                "  9  write failed\n"
	                " 10  fsync failed\n"
	                " 11  invalid magic\n",
	        prog);
}

int main(int argc, char **argv) {
	int parsable = 0, force = 0;
	char *infile = NULL, *outfile = NULL;

	static struct option long_options[] = {
		{"input",	required_argument,	0, 'i'},
		{"output",	required_argument,	0, 'o'},
		{"parsable",	no_argument,		0, 's'},
		{"force",	no_argument,		0, 'f'},
		{"help",	no_argument,		0, 'h'},
		{0, 0, 0, 0}
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "i:o:sfh", long_options, NULL)) != -1) {
		switch (opt) {
			case 'i': infile = optarg; break;
			case 'o': outfile = optarg; break;
			case 's': parsable = 1; break;
			case 'f': force = 1; break;
			case 'h': print_usage(argv[0]); return 0;
			default: if (!parsable) print_usage(argv[0]); return ERR_PARAM;
		}
	}

	if (optind + 2 > argc) {
		if (!parsable) print_usage(argv[0]);
		return ERR_PARAM;
	}

	char *action	= argv[optind];
	char *partition = argv[optind + 1];
	char *target	= (optind + 2 < argc) ? argv[optind + 2] : "all";
	char *val_str   = (optind + 3 < argc) ? argv[optind + 3] : NULL;

	char resolved_path[PATH_MAX];
	if (resolve_partition(partition, resolved_path, sizeof(resolved_path)) != 0) {
		if (!parsable) fprintf(stderr, "Partition '%s' not found\n", partition);
		return ERR_PARTITION_NOT_FOUND;
	}

	char *read_path  = infile ? infile : resolved_path;
	char *write_path = outfile ? outfile : resolved_path;

	unsigned char buf[ZONE_SIZE];
	memset(buf, 0, ZONE_SIZE);
	ZMSHLayout layout;
	ZMSHLayout orig_layout;
	zms_layout_init(&layout);
	zms_layout_init(&orig_layout);

	int write_required = 0, read_success = 0;
	int is_writing_cmd = 0;
	int sign_required  = 1;

	/* Command Logic Setup */
	if (strcmp(action, "new") == 0) {
		if (!parsable) printf("Creating fresh image buffer\n");
		is_writing_cmd = 1;
		write_required = 1;
	} else {
		/* All other commands require an input file or partition */
		if (!parsable) printf("Reading from: %s\n", read_path);
		int in_fd = open(read_path, O_RDONLY);
		if (in_fd < 0) {
			if (!parsable) perror("Input open failed");
			return ERR_INPUT_OPEN_FAILED;
		}
		safe_read(in_fd, buf, parsable);
		close(in_fd);
		read_success = 1;
		zms_layout_from_buf(buf, &layout);
	}

	/* Snapshot original layout (pre-signing/modifications applied later) */
	orig_layout = layout;
	if (read_success && strcmp(action, "validate") != 0) {
		if (layout.magic != ZMSH_MAGIC && !force) {
			if (!parsable) fprintf(stderr, "Error: Invalid Magic (0x%08X). Use -f to bypass.\n", layout.magic);
			return ERR_INVALID_MAGIC;
		}
	}

	if (strcmp(action, "validate") == 0) {
		if (layout.magic != ZMSH_MAGIC && !force) {
			if (!parsable) printf("validation=failed (Invalid Magic: 0x%08X)\n", layout.magic);
			return ERR_INVALID;
		}
		uint32_t expected = zms_layout_checksum(buf, &layout);
		if (layout.checksum != expected) {
			if (!parsable) printf("validation=failed (Bad Checksum)\n");
			return ERR_INVALID;
		}
		if (!parsable) printf("validation=ok\n");
		return EXIT_SUCCESS;
	} else if (strcmp(action, "new") == 0) {
		/* Fresh image was already initialized above; nothing else to do. */
	} else if (strcmp(action, "get") == 0) {
		if (strcmp(target, "csum") == 0) {
			if (parsable) printf("0x%08X\n", layout.checksum); else printf("checksum=0x%08X\n", layout.checksum);
		} else if (strcmp(target, "magic") == 0) {
			if (parsable) printf("0x%08X\n", layout.magic); else printf("magic=0x%08X\n", layout.magic);
		} else if (strcmp(target, "rs") == 0) {
			if (parsable) printf("%u\n", layout.run_status); else printf("RunStatus=%u\n", layout.run_status);
		} else if (strcmp(target, "bs") == 0) {
			if (parsable) printf("%u\n", layout.boot_status); else printf("BootStatus=%u\n", layout.boot_status);
		} else if (strcmp(target, "str_a") == 0) {
			if (parsable) printf("%.256s\n", layout.str_a); else printf("stringA=%.256s\n", layout.str_a);
		} else if (strcmp(target, "str_b") == 0) {
			if (parsable) printf("%.256s\n", layout.str_b); else printf("stringB=%.256s\n", layout.str_b);
		} else if (strcmp(target, "all") == 0) {
			if (parsable) {
				printf("0x%08X\n0x%08X\n%u\n%u\n%.256s\n%.256s\n",
					   layout.checksum, layout.magic,
					   layout.run_status, layout.boot_status,
					   layout.str_a, layout.str_b);
			} else {
				printf("checksum=0x%08X\nmagic=0x%08X\nRunStatus=%u\nBootStatus=%u\nstringA=%.256s\nstringB=%.256s\n",
					   layout.checksum, layout.magic,
					   layout.run_status, layout.boot_status,
					   layout.str_a, layout.str_b);
			}
		} else { return ERR_PARAM; }
	} else if (strcmp(action, "set") == 0) {
		is_writing_cmd = 1;
		if (strcmp(target, "magic") == 0) {
			uint32_t val = val_str ? safe_parse_u32(val_str, parsable) : ZMSH_MAGIC;
			layout.magic = val;
		} else {
			if (!val_str) return ERR_PARAM;
			if (strcmp(target, "rs") == 0) layout.run_status = safe_parse_u32(val_str, parsable);
			else if (strcmp(target, "bs") == 0) layout.boot_status = safe_parse_u32(val_str, parsable);
			else if (strcmp(target, "str_a") == 0) {
				memset(layout.str_a, 0, STR_SIZE);
				strncpy(layout.str_a, val_str, STR_SIZE - 1);
			} else if (strcmp(target, "str_b") == 0) {
				memset(layout.str_b, 0, STR_SIZE);
				strncpy(layout.str_b, val_str, STR_SIZE - 1);
			} else return ERR_PARAM;
		}
		write_required = 1;
	} else if (strcmp(action, "toggle") == 0) {
		is_writing_cmd = 1;
		uint32_t new_bs = (layout.boot_status == 0) ? 1 : 0;
		layout.boot_status = new_bs;
		if (!parsable) printf("BootStatus=%u\n", new_bs);
		write_required = 1;
	} else if (strcmp(action, "stage") == 0) {
		is_writing_cmd = 1;
		layout.run_status = 1;
		if (!parsable) printf("RunStatus=1\n");
		write_required = 1;
	} else if (strcmp(action, "commit") == 0) {
		is_writing_cmd = 1;
		layout.run_status = 0;
		if (!parsable) printf("RunStatus=0\n");
		write_required = 1;
	} else if (strcmp(action, "copy") == 0) {
		if (!outfile) {
			if (!parsable) fprintf(stderr, "Error: copy command requires an output file (-o) to avoid accidental overwrites.\n");
			return ERR_PARAM;
		}
		is_writing_cmd = 1;
		write_required = 1;
		sign_required = 0;
	} else {
		print_usage(argv[0]);
		return ERR_PARAM;
	}

	if (outfile && !is_writing_cmd) {
		if (!parsable) fprintf(stderr, "Error: -o option is only supported for writing commands.\n");
		return ERR_PARAM;
	}

	if (is_writing_cmd && (write_required || outfile != NULL)) {
		unsigned char out_buf[ZONE_SIZE];
		if (strcmp(action, "copy") == 0) {
			memcpy(out_buf, buf, ZONE_SIZE);
		} else {
			memcpy(out_buf, buf, ZONE_SIZE);
			if (sign_required) sign_buffer(&layout, buf);
			zms_layout_to_buf(out_buf, &layout);
		}

		/* For copy, always write the original bytes; for other commands, skip if the layout is unchanged. */
		if (strcmp(action, "copy") != 0 && memcmp(&layout, &orig_layout, sizeof(layout)) == 0) {
			if (!parsable) printf("No changes detected; skipping write.\n");
			return EXIT_SUCCESS;
		}

		if (!parsable) printf("Writing to: %s\n", write_path);
		int flags = O_WRONLY | O_CREAT;
		if (is_regular_file(write_path)) {
			flags |= O_TRUNC;
		}
		int out_fd = open(write_path, flags, 0644);
		if (out_fd < 0) {
			if (!parsable) perror("Output open failed");
			return ERR_OUTPUT_OPEN_FAILED;
		}
		safe_write(out_fd, out_buf, parsable, write_path);
		close(out_fd);
	}
	return EXIT_SUCCESS;
}
