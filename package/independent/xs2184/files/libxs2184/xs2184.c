/*******************************************************************************
 * File name : xs2184.c
 * Date : 2021.12.29 Create
 * Versions : v1.0
 * Author :	ChenMing <ccai93296@gmail.com>
 * explain : For read the state of each port,
 *           and realize the switch control of a single port
 *******************************************************************************/

#include "xs2184.h"

#define MAX_CHIPS 2
#define MAX_PORTS 8
#define MAX_AVERAGE 15

static int xs_i2c_addrs[MAX_CHIPS] = {
	XS2184_1_IIC_ADDR,
	XS2184_2_IIC_ADDR,
};

static int xs_port_to_i2c[MAX_PORTS + 1] = {
	-1, 0, 0, 0, 0, 1, 1, 1, 1,
    };

typedef struct {
	float* watts;
	u8 watch_counter;
	u8 rollback;
	u8 mark_has_pd;
} port_watt_t;

static port_watt_t g_pwatts[MAX_PORTS + 1];
static uint32_t statistic_inteval = 1000; //ms
static uint32_t max_average_watts = MAX_AVERAGE;

#define HZ (1000 / statistic_inteval)   // intv per second

#ifndef strrev
void strrev(unsigned char *str)
{
	int i;
	int j;
	unsigned char a;
	unsigned len = strlen((const char *)str);
	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}
#endif

#ifndef itoa
int itoa(int num, unsigned char* str, int len, int base)
{
	int sum = num;
	int i = 0;
	int digit;

	if (len == 0)
		return -1;
	do
	{
		digit = sum % base;
		if (digit < 0xA)
			str[i++] = '0' + digit;
		else
			str[i++] = 'A' + digit - 0xA;
		sum /= base;
	} while (sum && (i < (len - 1)));
	if (i == (len - 1) && sum)
		return -1;
	str[i] = '\0';
	strrev(str);
	return 0;
}
#endif

static int xs_port_to_i2c_addr(u8 port_num)
{
	int idx = xs_port_to_i2c[port_num];

	return xs_i2c_addrs[idx];
}

static int read_reg(u8 chip_addr, u8 reg_addr, u8 *reg_val)
{
	int file;
	char file_name[64];

	file = open_i2c_dev(BUS_NUM, file_name, sizeof(file_name), 0);
	if (file < 0)
		fprintf(stderr, "open dev error.\n");
	open_chip(file, chip_addr);

	*reg_val = i2c_smbus_read_byte_data(file, reg_addr);
	if (*reg_val < 0) {
		fprintf(stderr, "read failed\n");
		return -1;
	}

	close(file);

	return 0;
}

static int port_reg_read(u8 port_num, u8 reg_addr, u8 *reg_val)
{
	return read_reg(xs_port_to_i2c_addr(port_num), reg_addr, reg_val);
}

static int write_reg(u8 chip_addr, u8 reg_addr, u8 reg_val)
{
	int file;
	char file_name[20];

	file = open_i2c_dev(BUS_NUM, file_name, sizeof(file_name), 0);
	if (file < 0)
		fprintf(stderr, "open dev error.\n");
	open_chip(file, chip_addr);

	int res = i2c_smbus_write_byte_data(file, reg_addr, reg_val);
	if (res < 0) {
		fprintf(stderr, "write failed\n");
		return -1;
	}

	close(file);

	return 0;
}

static int port_reg_write(u8 port_num, u8 reg_addr, u8 reg_val)
{
	return write_reg(xs_port_to_i2c_addr(port_num), reg_addr, reg_val);
}

int open_chip(u8 file, u8 chip_addr)
{
	if (ioctl(file, I2C_SLAVE, chip_addr) < 0) {
		close(file);
		return -1;
	}

	return 0;
}

int chip_found( u8 chip_addr)
{
	u8 data = 0;
	u8 timeOut = 0;

	do {
		data = 0;
		read_reg(chip_addr, PSE_ID_REG, &data);
		timeOut++;
	} while((data != XS2184_ID_VAL)&&(timeOut < 5));

	if((timeOut > 4) || (data != XS2184_ID_VAL))
		return -1;
	else
		return 0;
}

float port_current(char port_num)
{
	float curr;
	uint32_t cur_msb = 0, cur_lsb = 0;

	port_reg_read(port_num, CURT_LSB(port_num), (u8*)&cur_lsb);
	port_reg_read(port_num, CURT_MSB(port_num), (u8*)&cur_msb);

	curr = ((float)(cur_msb << 8 | cur_lsb) / 1000.0) * ((float)CURRENT_PARA / 1000.0); //mA

	// fprintf(stdout, "port %u, current %u-%u, %.3f\n", port_num, cur_msb, cur_lsb, curr);
	return curr;
}

typedef int (* ps_callback_t)(u8 en, u8 port_num, float volt, float curt);

float port_voltage(char port_num)
{
	float volt;
	uint32_t vol_msb = 0, vol_lsb = 0;

	port_reg_read(port_num, VOLT_LSB(port_num), (u8*)&vol_lsb);
	port_reg_read(port_num, VOLT_MSB(port_num), (u8*)&vol_msb);

	volt = ((float)(vol_msb << 8 | vol_lsb) / 1000.0) * ((float)VOLTAGE_PARA / 1000.0); //uV - mV - V

	// fprintf(stdout, "port %u, volt %u-%u, %.3f\n", port_num, vol_lsb, vol_msb, volt);
	return volt;
}

int port_status(ps_callback_t cb)
{
	u8 reg = 0;
	int i;
	char bs[40] = "\0";

	for(i=0; i<MAX_CHIPS; i++) {
		u8 port_num;
		int addr = xs_i2c_addrs[i];

		if(addr < 0)
			continue;

		(void)read_reg(addr, POWER_STA_REG, &reg);
		itoa(reg, bs, sizeof(bs), 2);
		if(!cb)
			fprintf(stdout, "chip on 0x%02x state b'%s'\n", addr, bs);
		for(port_num=1; port_num<=4; port_num++) {
			float volt, curt;
			u8 en = (PORT_MASK(port_num) & reg);
			u8 vp = i*4 + port_num;

			if(!en) {
				if(cb)
					cb(en, vp, 0, 0);
				continue;
			}

			volt = port_voltage(vp);    //V
			curt = port_current(vp);    //mA
			if(cb) {
				cb(en, vp, volt, curt);
			} else {
				fprintf(stderr, "port %u volt/V %.2f curt/mA %.2f m-watts %.3f\n",
				        vp, volt, curt, volt * curt);
			}
		}
	}

	return 0;
}

int enable_port(char port_num)
{
	u8 reg;
	// char bs[40] = "\0";

	reg = CLASS_EN(port_num) | DET_EN(port_num);
	port_reg_write(port_num, DETECT_CLASS_EN_REG, reg);

	// reg = 0;
	// port_reg_read(port_num, DETECT_CLASS_EN_REG, &reg);
	// itoa(reg, bs, sizeof(bs), 2);

	// fprintf(stderr, "port %d: %x enable\n", port_num, CLASS_EN(port_num) | DET_EN(port_num));

	return 0;
}

int disable_port(char port_num)
{
	u8 reg;
	// char bs[40] = "\0";

	reg = PWR_OFF(port_num);
	port_reg_write(port_num, POWER_EN_REG, reg);

	// port_reg_read(port_num, POWER_EN_REG, &reg);
	// itoa(reg, bs, sizeof(bs), 2);

	// fprintf(stderr, "port %d: %x disable\n", port_num, PWR_OFF(port_num));

	return 0;
}

enum {
	CMD_NILL = -1,
	CMD_STATUS = 0,
	CMD_POWER_UP,
	CMD_POWER_DOWN,
	CMD_MONITOR,
};

#define MIN_LOAD_TRIG_mW   (1200)

int port_monitor(u8 en, u8 vp, float volt, float curt)
{
	float mWatt = volt * curt;
	port_watt_t* pw = &g_pwatts[vp];

	pw->watts[pw->watch_counter++] = mWatt;
	if(en) {
		int i, counter;
		float av_mWatt = 0.0;

		for(i=0; i<max_average_watts; i++) {
			av_mWatt += pw->watts[i];
		}
		counter = pw->rollback ? max_average_watts : pw->watch_counter;
		av_mWatt /= counter;
		if(av_mWatt < MIN_LOAD_TRIG_mW && pw->rollback && pw->mark_has_pd) {
			/* no load */
			fprintf(stdout, "port %u closed with avg %.3f mW\n", vp, av_mWatt);
			disable_port(vp);
		} else if(av_mWatt >= MIN_LOAD_TRIG_mW) {
			pw->mark_has_pd = 1;
			/** TODO: store port-N, power average useage to /tmp/file */
			fprintf(stdout, "port %u, current %u has %0.2f mW, round avg %.3f mW\n",
			        vp, counter, mWatt, av_mWatt);
		} else {
			/** port en, but no load, wait until one round rollback. */
			fprintf(stderr, "port %u, current %.2f mW, avg %.3f mW\n", vp, mWatt, av_mWatt);
		}
		if(pw->watch_counter == max_average_watts) {
			pw->watch_counter = 0;
			pw->rollback = 1;
		}
	} else {
		if(pw->watch_counter == max_average_watts) {
			pw->watch_counter = 0;
			pw->rollback = 0;
			/* power on & wait PD */
			fprintf(stdout, "port %u enable again PD detection\n", vp);
			enable_port(vp);
			sleep(1);   /* must keep it. */
		}
	}

	return 0;
}

static int run_monitor(int intval)
{
	int i;

	memset(&g_pwatts, 0, sizeof(g_pwatts));
	for(i=0; i<sizeof(g_pwatts)/sizeof(g_pwatts[0]); i++) {
		float *pwts = malloc(sizeof(float) * max_average_watts);
		if(!pwts) {
			exit(-1);
		}
		memset(pwts, 0, max_average_watts * sizeof(float));
		g_pwatts[i].watts = pwts;
	}

	while(1) {
		port_status((ps_callback_t)port_monitor);
		usleep(statistic_inteval * 1000);
		fflush(stdout);
		fflush(stderr);
	}

	return 0;
}

static void help()
{
	fprintf(stdout, "xs2184 : View port status and switch control of a single port\n" \
	        "usage: xs2184 [option] [port num | param]\n" \
	        "\toption : -c : Command for viewing port status\n" \
	        "\t         -u : Single port enabled\n" \
	        "\t         -d : Single port disabled\n" \
	        "\t         -m : monitor as ms intval\n" \
	        "\t         -s : average statistiacs count\n" \
	        "\tport num : 1-%d Counting from left, one port at a time\n", MAX_PORTS + 1);
}

int main(int argc, char *argv[])
{
	int c, i;
	int port = -1;
	int cmd = CMD_NILL;

	opterr = 0;

	for(i=0; i<ARRAY_SIZE(xs_i2c_addrs); i++) {
		if(chip_found(xs_i2c_addrs[i]) < 0) {
			xs_i2c_addrs[i] = -1;
		} else {
			fprintf(stderr, "found chip[%d] on 0x%02x\n", i, xs_i2c_addrs[i]);
		}
	}

	while ((c = getopt(argc, argv, "d:u:m:c")) != -1) {
		switch (c) {
		case 'c':
			cmd = CMD_STATUS;
			break;
		case 'u':
			cmd = CMD_POWER_UP;
			port = atoi(optarg);
			break;
		case 'd':
			cmd = CMD_POWER_DOWN;
			port = atoi(optarg);
			break;
		case 'm':
			cmd = CMD_MONITOR;
			statistic_inteval = atoi(optarg);
			break;
		case 's':
			max_average_watts = atoi(optarg);
			break;
		case '?':
			help();
			return -1;
		default:
			help();
			return -1;
		}
	}

	if(port > MAX_PORTS) {
		help();
		return -1;
	}

	if(statistic_inteval < 1000 || statistic_inteval > 10000) {
		statistic_inteval = 1000;
	}

	switch(cmd) {
	case CMD_STATUS:
		if (port_status(NULL) < 0)
			fprintf(stderr, "no PD on port.\n");
		break;
	case CMD_POWER_UP:
		if (enable_port(port) < 0);
		break;
	case CMD_POWER_DOWN:
		if (disable_port(port) < 0);
		break;
	case CMD_MONITOR:
		return run_monitor(statistic_inteval);
	default:
		fprintf(stderr, "Parameter is wrong.\n");
		(void)help();
		return -1;
	}

	return 0;
}

