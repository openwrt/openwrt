#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_MSGS 16
#define MAX_LEN 512
#define I2C_RDWR_RETRIES 50
#define I2C_RDWR_RETRY_US 10000

struct msg_buf {
	struct i2c_msg msg;
	uint8_t data[MAX_LEN];
};

static void usage(const char *prog)
{
	fprintf(stderr, "usage: %s -y <bus> <wN[@addr]|rN[@addr]> [bytes...]\n", prog);
	exit(1);
}

static unsigned long parse_ulong(const char *s, const char *what)
{
	char *end;
	unsigned long val;

	errno = 0;
	val = strtoul(s, &end, 0);
	if (errno || *end) {
		fprintf(stderr, "invalid %s: %s\n", what, s);
		exit(1);
	}

	return val;
}

static int parse_desc(char *desc, uint16_t *addr, int *has_addr, uint16_t *len)
{
	char *at = strchr(desc, '@');
	char *num = desc + 1;
	unsigned long val;

	if (desc[0] != 'w' && desc[0] != 'r')
		return -1;

	if (at) {
		*at = '\0';
		*addr = parse_ulong(at + 1, "address");
		*has_addr = 1;
	} else {
		*has_addr = 0;
	}

	val = parse_ulong(num, "length");
	if (!val || val > MAX_LEN)
		return -1;

	*len = val;
	return desc[0];
}

static int i2c_rdwr_retry(int fd, struct i2c_rdwr_ioctl_data *rdwr)
{
	int ret;

	for (int i = 0; i < I2C_RDWR_RETRIES; i++) {
		ret = ioctl(fd, I2C_RDWR, rdwr);
		if (!ret)
			return 0;

		if (errno != ENXIO && errno != EREMOTEIO && errno != EIO &&
		    errno != ETIMEDOUT)
			return ret;

		usleep(I2C_RDWR_RETRY_US);
	}

	return ret;
}

int main(int argc, char **argv)
{
	struct msg_buf bufs[MAX_MSGS];
	struct i2c_msg msgs[MAX_MSGS];
	struct i2c_rdwr_ioctl_data rdwr;
	uint16_t current_addr = 0;
	int current_addr_valid = 0;
	int msg_count = 0;
	int arg = 1;
	int bus;
	char dev[32];
	int fd;

	if (argc < 4)
		usage(argv[0]);

	if (!strcmp(argv[arg], "-y"))
		arg++;

	if (arg >= argc)
		usage(argv[0]);

	bus = parse_ulong(argv[arg++], "bus");

	while (arg < argc) {
		uint16_t addr = 0;
		uint16_t len = 0;
		int has_addr = 0;
		int dir;
		int i;

		if (msg_count >= MAX_MSGS)
			usage(argv[0]);

		dir = parse_desc(argv[arg++], &addr, &has_addr, &len);
		if (dir < 0)
			usage(argv[0]);

		if (has_addr) {
			current_addr = addr;
			current_addr_valid = 1;
		}

		if (!current_addr_valid) {
			fprintf(stderr, "missing i2c address\n");
			return 1;
		}

		memset(&bufs[msg_count], 0, sizeof(bufs[msg_count]));
		bufs[msg_count].msg.addr = current_addr;
		bufs[msg_count].msg.len = len;
		bufs[msg_count].msg.buf = bufs[msg_count].data;

		if (dir == 'r') {
			bufs[msg_count].msg.flags = I2C_M_RD;
		} else {
			if (arg + len > argc) {
				fprintf(stderr, "not enough write bytes\n");
				return 1;
			}

			for (i = 0; i < len; i++)
				bufs[msg_count].data[i] = parse_ulong(argv[arg++], "byte");
		}

		msgs[msg_count] = bufs[msg_count].msg;
		msg_count++;
	}

	snprintf(dev, sizeof(dev), "/dev/i2c-%d", bus);
	fd = open(dev, O_RDWR);
	if (fd < 0) {
		perror(dev);
		return 1;
	}

	rdwr.msgs = msgs;
	rdwr.nmsgs = msg_count;

	if (i2c_rdwr_retry(fd, &rdwr) < 0) {
		perror("I2C_RDWR");
		close(fd);
		return 1;
	}

	for (int m = 0; m < msg_count; m++) {
		if (!(msgs[m].flags & I2C_M_RD))
			continue;

		for (int i = 0; i < msgs[m].len; i++) {
			if (i)
				putchar(' ');
			printf("0x%02x", bufs[m].data[i]);
		}
		putchar('\n');
	}

	close(fd);
	return 0;
}
