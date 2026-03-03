/*
 * pcat2-display - PhotoniCAT 2 Mini Display Status Application
 *
 * Drives the GC9307 172x320 TFT LCD via SPI to show system status:
 * time/date, battery, WAN, WiFi, and system information.
 *
 * Hardware: GC9307 on SPI1.0 (6MHz), DC=GPIO3_PD1, RST=GPIO3_PD2, BL=GPIO3_C5
 * Display: 172x320 pixels, RGB565 color, rotation 180 deg, column offset 34
 * Backlight: active LOW (PWM polarity inverted per factory DTS)
 *
 * Copyright (C) 2026 - GPLv3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>

/* ------------------------------------------------------------------ */
/*  Configuration                                                      */
/* ------------------------------------------------------------------ */

#define SPI_DEVICE      "/dev/spidev1.0"
#define SPI_SPEED_HZ    6000000     /* 6 MHz */
#define SPI_CHUNK       4096        /* bytes per SPI transfer */

#define DISP_W          172
#define DISP_H          320
#define COL_OFFSET      34          /* GC9307 column offset for 172px panel */

/* GPIO chardev: /dev/gpiochipN where N = bank, offset within bank.
 * RK3576: gpio0..gpio4 map to /dev/gpiochip0../dev/gpiochip4. */
#define DC_BANK     3
#define DC_OFFSET   25              /* GPIO3_PD1 */
#define RST_BANK    3
#define RST_OFFSET  26              /* GPIO3_PD2 */
#define BL_BANK     3
#define BL_OFFSET   21              /* GPIO3_C5 – PWM backlight pin */

/* GC9307 / ST7789 registers */
#define CMD_SWRESET 0x01
#define CMD_SLPOUT  0x11
#define CMD_SLPIN   0x10
#define CMD_NORON   0x13
#define CMD_INVOFF  0x20
#define CMD_INVON   0x21
#define CMD_DISPOFF 0x28
#define CMD_DISPON  0x29
#define CMD_CASET   0x2A
#define CMD_RASET   0x2B
#define CMD_RAMWR   0x2C
#define CMD_COLMOD  0x3A
#define CMD_MADCTL  0x36
#define MADCTL_MX   0x40

/* BGR565 colour helpers – display byte order is big-endian */
#define RGB565(r,g,b) ( (uint16_t)( \
        (((uint16_t)((b)>>3))<<11) | \
        (((uint16_t)((g)>>2))<<5)  | \
         ((uint16_t)((r)>>3)) ))

/* pre-computed colours (host-endian, swapped on write) */
#define COL_BLACK   RGB565(0,0,0)
#define COL_WHITE   RGB565(255,255,255)
#define COL_RED     RGB565(255,40,40)
#define COL_GREEN   RGB565(40,255,40)
#define COL_CYAN    RGB565(0,220,220)
#define COL_YELLOW  RGB565(255,255,0)
#define COL_ORANGE  RGB565(255,165,0)
#define COL_GRAY    RGB565(100,100,100)
#define COL_DKGRAY  RGB565(40,40,40)
#define COL_NAVY    RGB565(0,8,30)

#define UPDATE_SEC  5               /* seconds between refreshes */

/* ------------------------------------------------------------------ */
/*  Adafruit GFX 5x7 bitmap font (column-major, LSB = top row)        */
/*  Covers ASCII 0x20 (' ') through 0x7E ('~')  –  95 characters      */
/* ------------------------------------------------------------------ */

static const unsigned char font5x7[95][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* 0x20 space */
    {0x00,0x00,0x5F,0x00,0x00}, /* ! */
    {0x00,0x07,0x00,0x07,0x00}, /* " */
    {0x14,0x7F,0x14,0x7F,0x14}, /* # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
    {0x23,0x13,0x08,0x64,0x62}, /* % */
    {0x36,0x49,0x55,0x22,0x50}, /* & */
    {0x00,0x05,0x03,0x00,0x00}, /* ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* ( */
    {0x00,0x41,0x22,0x1C,0x00}, /* ) */
    {0x14,0x08,0x3E,0x08,0x14}, /* * */
    {0x08,0x08,0x3E,0x08,0x08}, /* + */
    {0x00,0x50,0x30,0x00,0x00}, /* , */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x00,0x60,0x60,0x00,0x00}, /* . */
    {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
    {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 2 */
    {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1E}, /* 9 */
    {0x00,0x36,0x36,0x00,0x00}, /* : */
    {0x00,0x56,0x36,0x00,0x00}, /* ; */
    {0x08,0x14,0x22,0x41,0x00}, /* < */
    {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x00,0x41,0x22,0x14,0x08}, /* > */
    {0x02,0x01,0x51,0x09,0x06}, /* ? */
    {0x32,0x49,0x79,0x41,0x3E}, /* @ */
    {0x7E,0x11,0x11,0x11,0x7E}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */
    {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x22,0x1C}, /* D */
    {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */
    {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */
    {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7F,0x01,0x01}, /* T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}, /* Z */
    {0x00,0x7F,0x41,0x41,0x00}, /* [ */
    {0x02,0x04,0x08,0x10,0x20}, /* \ */
    {0x00,0x41,0x41,0x7F,0x00}, /* ] */
    {0x04,0x02,0x01,0x02,0x04}, /* ^ */
    {0x40,0x40,0x40,0x40,0x40}, /* _ */
    {0x00,0x01,0x02,0x04,0x00}, /* ` */
    {0x20,0x54,0x54,0x54,0x78}, /* a */
    {0x7F,0x48,0x44,0x44,0x38}, /* b */
    {0x38,0x44,0x44,0x44,0x20}, /* c */
    {0x38,0x44,0x44,0x48,0x7F}, /* d */
    {0x38,0x54,0x54,0x54,0x18}, /* e */
    {0x08,0x7E,0x09,0x01,0x02}, /* f */
    {0x0C,0x52,0x52,0x52,0x3E}, /* g */
    {0x7F,0x08,0x04,0x04,0x78}, /* h */
    {0x00,0x44,0x7D,0x40,0x00}, /* i */
    {0x20,0x40,0x44,0x3D,0x00}, /* j */
    {0x7F,0x10,0x28,0x44,0x00}, /* k */
    {0x00,0x41,0x7F,0x40,0x00}, /* l */
    {0x7C,0x04,0x18,0x04,0x78}, /* m */
    {0x7C,0x08,0x04,0x04,0x78}, /* n */
    {0x38,0x44,0x44,0x44,0x38}, /* o */
    {0x7C,0x14,0x14,0x14,0x08}, /* p */
    {0x08,0x14,0x14,0x18,0x7C}, /* q */
    {0x7C,0x08,0x04,0x04,0x08}, /* r */
    {0x48,0x54,0x54,0x54,0x20}, /* s */
    {0x04,0x3F,0x44,0x40,0x20}, /* t */
    {0x3C,0x40,0x40,0x20,0x7C}, /* u */
    {0x1C,0x20,0x40,0x20,0x1C}, /* v */
    {0x3C,0x40,0x30,0x40,0x3C}, /* w */
    {0x44,0x28,0x10,0x28,0x44}, /* x */
    {0x0C,0x50,0x50,0x50,0x3C}, /* y */
    {0x44,0x64,0x54,0x4C,0x44}, /* z */
    {0x00,0x08,0x36,0x41,0x00}, /* { */
    {0x00,0x00,0x7F,0x00,0x00}, /* | */
    {0x00,0x41,0x36,0x08,0x00}, /* } */
    {0x10,0x08,0x08,0x10,0x08}, /* ~ */
};

/* ------------------------------------------------------------------ */
/*  Global state                                                       */
/* ------------------------------------------------------------------ */

static volatile sig_atomic_t running = 1;
static int spi_fd = -1;

/* GPIO chardev line handles (fd from GPIO_GET_LINEHANDLE_IOCTL) */
static int dc_line_fd  = -1;
static int rst_line_fd = -1;
static int bl_line_fd  = -1;

/* framebuffer in display byte order (big-endian RGB565) */
static uint8_t fb[DISP_W * DISP_H * 2];

/* ------------------------------------------------------------------ */
/*  Utility: read a sysfs file into a buffer                           */
/* ------------------------------------------------------------------ */

static int read_sysfs(const char *path, char *buf, size_t len)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;
    ssize_t n = read(fd, buf, len - 1);
    close(fd);
    if (n < 0) return -1;
    buf[n] = '\0';
    while (n > 0 && (buf[n-1] == '\n' || buf[n-1] == '\r'))
        buf[--n] = '\0';
    return 0;
}

static int read_sysfs_int(const char *path)
{
    char buf[32];
    if (read_sysfs(path, buf, sizeof(buf)) < 0) return -1;
    return atoi(buf);
}

/* ------------------------------------------------------------------ */
/*  GPIO (chardev v2 API – CONFIG_GPIO_CDEV without V1 compat)         */
/* ------------------------------------------------------------------ */

/*
 * Request an output GPIO line on the given bank + offset using the
 * v2 chardev API (GPIO_V2_GET_LINE_IOCTL).  The v1 API is disabled
 * in OpenWrt's generic kernel config (CONFIG_GPIO_CDEV_V1 is not set).
 *
 * Returns the line request fd (>= 0) on success, -1 on failure.
 */
static int gpio_request_output(int bank, int offset,
                               const char *name, int initial)
{
    char path[64];
    snprintf(path, sizeof(path), "/dev/gpiochip%d", bank);

    int chip_fd = open(path, O_RDWR);
    if (chip_fd < 0) {
        fprintf(stderr, "gpio: cannot open %s: %s\n", path, strerror(errno));
        return -1;
    }

    struct gpio_v2_line_request req;
    memset(&req, 0, sizeof(req));
    req.offsets[0]  = offset;
    req.num_lines   = 1;
    strncpy(req.consumer, name, sizeof(req.consumer) - 1);

    req.config.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    if (initial) {
        /* set the default output value via a single-attribute */
        req.config.num_attrs = 1;
        req.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
        req.config.attrs[0].attr.values = 1;   /* bit 0 = line 0 value */
        req.config.attrs[0].mask = 1;           /* applies to line 0 */
    }

    if (ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &req) < 0) {
        fprintf(stderr, "gpio: V2_GET_LINE bank%d offset%d (%s): %s\n",
                bank, offset, name, strerror(errno));
        close(chip_fd);
        return -1;
    }

    close(chip_fd);     /* chip fd can be closed; line request fd stays valid */
    return req.fd;
}

static void gpio_set(int line_fd, int value)
{
    if (line_fd < 0) return;
    struct gpio_v2_line_values vals;
    memset(&vals, 0, sizeof(vals));
    vals.mask = 1;                  /* bit 0 = line 0 */
    vals.bits = value ? 1 : 0;
    ioctl(line_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, &vals);
}

/* ------------------------------------------------------------------ */
/*  SPI helpers                                                        */
/* ------------------------------------------------------------------ */

static int spi_init(void)
{
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) { perror(SPI_DEVICE); return -1; }

    uint8_t  mode = SPI_MODE_0;
    uint8_t  bits = 8;
    uint32_t speed = SPI_SPEED_HZ;

    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    return 0;
}

static void spi_write(const uint8_t *data, size_t len)
{
    while (len > 0) {
        size_t chunk = (len > SPI_CHUNK) ? SPI_CHUNK : len;
        struct spi_ioc_transfer tr = {
            .tx_buf        = (unsigned long)data,
            .len           = chunk,
            .speed_hz      = SPI_SPEED_HZ,
            .bits_per_word = 8,
        };
        if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr) < 0)
            perror("SPI_IOC_MESSAGE");
        data += chunk;
        len  -= chunk;
    }
}

/* ------------------------------------------------------------------ */
/*  GC9307 display commands                                            */
/* ------------------------------------------------------------------ */

static void disp_cmd(uint8_t cmd)
{
    gpio_set(dc_line_fd, 0);             /* command mode */
    spi_write(&cmd, 1);
}

static void disp_data(const uint8_t *data, size_t len)
{
    gpio_set(dc_line_fd, 1);             /* data mode */
    spi_write(data, len);
}

static void disp_data1(uint8_t val)
{
    disp_data(&val, 1);
}

static void disp_set_window(int x, int y, int w, int h)
{
    uint16_t xs = x + COL_OFFSET, xe = x + w - 1 + COL_OFFSET;
    uint16_t ys = y,              ye = y + h - 1;

    uint8_t ca[4] = { xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF };
    uint8_t ra[4] = { ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF };

    disp_cmd(CMD_CASET); disp_data(ca, 4);
    disp_cmd(CMD_RASET); disp_data(ra, 4);
}

static void disp_init(void)
{
    /* HW reset */
    gpio_set(rst_line_fd, 1); usleep(10000);
    gpio_set(rst_line_fd, 0); usleep(50000);
    gpio_set(rst_line_fd, 1); usleep(10000);

    disp_cmd(CMD_SWRESET); usleep(150000);
    disp_cmd(CMD_SLPOUT);  usleep(150000);

    /* 16-bit colour */
    disp_cmd(CMD_COLMOD); disp_data1(0x55); usleep(10000);

    /* rotation 180: MADCTL = MX (0x40) */
    disp_cmd(CMD_MADCTL); disp_data1(MADCTL_MX);

    /* display on – GC9307 uses INVOFF per factory driver */
    disp_cmd(CMD_INVOFF); usleep(10000);
    disp_cmd(CMD_NORON);  usleep(10000);
    disp_cmd(CMD_DISPON); usleep(10000);

    /* backlight on – active LOW (PWM polarity is inverted per factory DTS) */
    gpio_set(bl_line_fd, 0);
}

static void disp_sleep(void)
{
    gpio_set(bl_line_fd, 1);   /* backlight off (active LOW, so HIGH = off) */
    disp_cmd(CMD_DISPOFF); usleep(10000);
    disp_cmd(CMD_SLPIN);   usleep(10000);
}

/* ------------------------------------------------------------------ */
/*  Framebuffer primitives                                             */
/* ------------------------------------------------------------------ */

static void fb_clear(uint16_t col)
{
    uint8_t hi = col >> 8, lo = col & 0xFF;
    for (int i = 0; i < DISP_W * DISP_H; i++) {
        fb[i * 2]     = hi;
        fb[i * 2 + 1] = lo;
    }
}

static inline void fb_pixel(int x, int y, uint16_t col)
{
    if ((unsigned)x >= DISP_W || (unsigned)y >= DISP_H) return;
    int off = (y * DISP_W + x) * 2;
    fb[off]     = col >> 8;
    fb[off + 1] = col & 0xFF;
}

static void fb_rect(int x, int y, int w, int h, uint16_t col)
{
    for (int dy = 0; dy < h; dy++)
        for (int dx = 0; dx < w; dx++)
            fb_pixel(x + dx, y + dy, col);
}

static void fb_hline(int x, int y, int w, uint16_t col)
{
    for (int dx = 0; dx < w; dx++)
        fb_pixel(x + dx, y, col);
}

/* ------------------------------------------------------------------ */
/*  Text rendering (5x7 font, scale 1x or 2x)                         */
/* ------------------------------------------------------------------ */

/*
 * Draw one character.  The font is column-major: each byte is a
 * vertical column of 7 pixels (LSB = top).  We render the 5 data
 * columns plus 1 trailing blank column, and 7 data rows plus
 * 1 trailing blank row, all scaled by `s`.
 */
static void draw_char(int x, int y, char ch, uint16_t fg, uint16_t bg, int s)
{
    if (ch < 0x20 || ch > 0x7E) ch = '?';
    const unsigned char *glyph = font5x7[ch - 0x20];

    for (int col = 0; col < 6; col++) {
        uint8_t bits = (col < 5) ? glyph[col] : 0;
        for (int row = 0; row < 8; row++) {
            uint16_t c = (row < 7 && (bits & (1 << row))) ? fg : bg;
            for (int dy = 0; dy < s; dy++)
                for (int dx = 0; dx < s; dx++)
                    fb_pixel(x + col * s + dx, y + row * s + dy, c);
        }
    }
}

/* returns the X coordinate after the last character */
static int draw_str(int x, int y, const char *str,
                    uint16_t fg, uint16_t bg, int s)
{
    while (*str) {
        draw_char(x, y, *str, fg, bg, s);
        x += 6 * s;
        str++;
    }
    return x;
}

/* draw string right-aligned at xr (right edge) */
static void draw_str_r(int xr, int y, const char *str,
                       uint16_t fg, uint16_t bg, int s)
{
    int len = strlen(str);
    draw_str(xr - len * 6 * s, y, str, fg, bg, s);
}

/* draw string centred */
static void draw_str_c(int y, const char *str,
                       uint16_t fg, uint16_t bg, int s)
{
    int w = strlen(str) * 6 * s;
    draw_str((DISP_W - w) / 2, y, str, fg, bg, s);
}

/* ------------------------------------------------------------------ */
/*  Push framebuffer to display                                        */
/* ------------------------------------------------------------------ */

static void fb_flush(void)
{
    disp_set_window(0, 0, DISP_W, DISP_H);
    disp_cmd(CMD_RAMWR);
    gpio_set(dc_line_fd, 1);           /* data mode for pixel stream */
    spi_write(fb, sizeof(fb));
}

/* ------------------------------------------------------------------ */
/*  System data collection helpers                                     */
/* ------------------------------------------------------------------ */

static int get_battery_pct(void)
{
    return read_sysfs_int("/sys/class/power_supply/battery/capacity");
}

static void get_battery_status(char *buf, size_t len)
{
    if (read_sysfs("/sys/class/power_supply/battery/status", buf, len) < 0)
        strncpy(buf, "N/A", len);
}

static int get_battery_voltage_mv(void)
{
    int uv = read_sysfs_int("/sys/class/power_supply/battery/voltage_now");
    return (uv > 0) ? uv / 1000 : -1;
}

static int get_battery_current_ma(void)
{
    int ua = read_sysfs_int("/sys/class/power_supply/battery/current_now");
    return (ua >= 0) ? ua / 1000 : -1;
}

static int get_cpu_temp(void)
{
    /* try several thermal zones */
    static const char *paths[] = {
        "/sys/class/thermal/thermal_zone1/temp",
        "/sys/class/thermal/thermal_zone0/temp",
        NULL
    };
    for (int i = 0; paths[i]; i++) {
        int t = read_sysfs_int(paths[i]);
        if (t > 0) return t / 1000;
    }
    return -1;
}

static void get_uptime_str(char *buf, size_t len)
{
    char raw[64];
    if (read_sysfs("/proc/uptime", raw, sizeof(raw)) < 0) {
        strncpy(buf, "N/A", len); return;
    }
    int secs = atoi(raw);
    int d = secs / 86400, h = (secs % 86400) / 3600, m = (secs % 3600) / 60;
    if (d > 0)      snprintf(buf, len, "%dd %dh %dm", d, h, m);
    else if (h > 0) snprintf(buf, len, "%dh %dm", h, m);
    else            snprintf(buf, len, "%dm", m);
}

static void get_memory(int *used_mb, int *total_mb)
{
    FILE *f = fopen("/proc/meminfo", "r");
    *used_mb = *total_mb = 0;
    if (!f) return;
    long total = 0, avail = 0;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0)
            sscanf(line + 9, " %ld", &total);
        else if (strncmp(line, "MemAvailable:", 13) == 0)
            sscanf(line + 13, " %ld", &avail);
    }
    fclose(f);
    *total_mb = (int)(total / 1024);
    *used_mb  = (int)((total - avail) / 1024);
}

/* run a shell command, capture first line of output */
static void run_cmd(const char *cmd, char *buf, size_t len)
{
    buf[0] = '\0';
    FILE *f = popen(cmd, "r");
    if (!f) return;
    if (fgets(buf, len, f) == NULL) buf[0] = '\0';
    pclose(f);
    char *nl = strchr(buf, '\n');
    if (nl) *nl = '\0';
}

static void get_wan_info(char *iface, size_t ilen, char *ip, size_t iplen)
{
    strncpy(iface, "", ilen);
    strncpy(ip, "N/A", iplen);

    /* find default route interface */
    char line[256];
    run_cmd("ip route show default 2>/dev/null | head -1", line, sizeof(line));
    char *dev = strstr(line, "dev ");
    if (dev) sscanf(dev + 4, "%s", iface);

    if (iface[0] == '\0') return;

    char cmd[256];
    snprintf(cmd, sizeof(cmd),
        "ip -4 addr show %s 2>/dev/null | grep 'inet ' | awk '{print $2}' | cut -d/ -f1 | head -1",
        iface);
    run_cmd(cmd, ip, iplen);
}

static void get_wifi_ssid(char *buf, size_t len)
{
    run_cmd("uci -q get wireless.default_radio0.ssid 2>/dev/null", buf, len);
    if (buf[0] == '\0') strncpy(buf, "N/A", len);
}

static void get_wifi_band_chan(char *buf, size_t len)
{
    char band[16] = "", chan[16] = "";
    run_cmd("uci -q get wireless.radio0.band 2>/dev/null", band, sizeof(band));
    run_cmd("uci -q get wireless.radio0.channel 2>/dev/null", chan, sizeof(chan));

    if (band[0] && chan[0])
        snprintf(buf, len, "%s Ch%s", band, chan);
    else
        strncpy(buf, "N/A", len);
}

static int get_wifi_clients(void)
{
    char buf[16];
    run_cmd("iwinfo wlan0 assoclist 2>/dev/null | grep -c 'dBm'", buf, sizeof(buf));
    return atoi(buf);
}

/* ------------------------------------------------------------------ */
/*  Render the status screen                                           */
/* ------------------------------------------------------------------ */

static void render_screen(void)
{
    char tmp[128];
    int y;

    fb_clear(COL_BLACK);

    /* === TOP BAR (dark navy, y 0..23) === */
    fb_rect(0, 0, DISP_W, 24, COL_NAVY);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(tmp, sizeof(tmp), "%02d:%02d", t->tm_hour, t->tm_min);
    draw_str(6, 4, tmp, COL_WHITE, COL_NAVY, 2);

    int bpct = get_battery_pct();
    if (bpct >= 0) {
        snprintf(tmp, sizeof(tmp), "%d%%", bpct);
        uint16_t bcol = (bpct > 20) ? COL_GREEN : (bpct > 10 ? COL_YELLOW : COL_RED);
        draw_str_r(DISP_W - 6, 4, tmp, bcol, COL_NAVY, 2);
    }

    /* separator */
    fb_hline(0, 24, DISP_W, COL_CYAN);
    fb_hline(0, 25, DISP_W, COL_CYAN);

    /* === DATE === */
    static const char *wday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const char *mon[]  = {"Jan","Feb","Mar","Apr","May","Jun",
                                 "Jul","Aug","Sep","Oct","Nov","Dec"};
    snprintf(tmp, sizeof(tmp), "%s %s %02d, %d",
             wday[t->tm_wday], mon[t->tm_mon], t->tm_mday, t->tm_year + 1900);
    draw_str_c(30, tmp, COL_WHITE, COL_BLACK, 1);

    /* === BATTERY section === */
    y = 46;
    draw_str(4, y, "BATTERY", COL_CYAN, COL_BLACK, 1);
    fb_hline(4, y + 9, DISP_W - 8, COL_DKGRAY);
    y += 14;

    char status[32];
    get_battery_status(status, sizeof(status));
    uint16_t scol = COL_WHITE;
    if (strstr(status, "Charging"))     scol = COL_GREEN;
    else if (strstr(status, "Full"))    scol = COL_GREEN;
    else if (strstr(status, "Not"))     scol = COL_ORANGE;
    draw_str(8, y, status, scol, COL_BLACK, 1);
    y += 10;

    int mv = get_battery_voltage_mv();
    int ma = get_battery_current_ma();
    if (mv > 0 && ma >= 0)
        snprintf(tmp, sizeof(tmp), "%d.%02dV  %dmA", mv / 1000, (mv % 1000) / 10, ma);
    else if (mv > 0)
        snprintf(tmp, sizeof(tmp), "%d.%02dV", mv / 1000, (mv % 1000) / 10);
    else
        strncpy(tmp, "N/A", sizeof(tmp));
    draw_str(8, y, tmp, COL_WHITE, COL_BLACK, 1);

    /* === NETWORK section === */
    y += 18;
    draw_str(4, y, "NETWORK", COL_CYAN, COL_BLACK, 1);
    fb_hline(4, y + 9, DISP_W - 8, COL_DKGRAY);
    y += 14;

    char wan_if[32] = "", wan_ip[64] = "N/A";
    get_wan_info(wan_if, sizeof(wan_if), wan_ip, sizeof(wan_ip));
    if (wan_if[0]) {
        char opstate[16] = "?";
        snprintf(tmp, sizeof(tmp), "/sys/class/net/%s/operstate", wan_if);
        read_sysfs(tmp, opstate, sizeof(opstate));

        int is_up = (strcmp(opstate, "up") == 0 || strcmp(opstate, "unknown") == 0);
        snprintf(tmp, sizeof(tmp), "%s %s", wan_if, is_up ? "UP" : "DOWN");
        draw_str(8, y, tmp, is_up ? COL_GREEN : COL_RED, COL_BLACK, 1);
    } else {
        draw_str(8, y, "No default route", COL_RED, COL_BLACK, 1);
    }
    y += 10;

    snprintf(tmp, sizeof(tmp), "IP: %s", wan_ip);
    draw_str(8, y, tmp, COL_WHITE, COL_BLACK, 1);

    /* === WIFI section === */
    y += 18;
    draw_str(4, y, "WIFI", COL_CYAN, COL_BLACK, 1);
    fb_hline(4, y + 9, DISP_W - 8, COL_DKGRAY);
    y += 14;

    char ssid[64];
    get_wifi_ssid(ssid, sizeof(ssid));
    draw_str(8, y, ssid, COL_WHITE, COL_BLACK, 1);
    y += 10;

    char bandchan[32];
    get_wifi_band_chan(bandchan, sizeof(bandchan));
    int clients = get_wifi_clients();
    snprintf(tmp, sizeof(tmp), "%s  %dSTA", bandchan, clients);
    draw_str(8, y, tmp, COL_WHITE, COL_BLACK, 1);

    /* === SYSTEM section === */
    y += 18;
    draw_str(4, y, "SYSTEM", COL_CYAN, COL_BLACK, 1);
    fb_hline(4, y + 9, DISP_W - 8, COL_DKGRAY);
    y += 14;

    int cpu_c = get_cpu_temp();
    int mem_used, mem_total;
    get_memory(&mem_used, &mem_total);
    snprintf(tmp, sizeof(tmp), "CPU %dC  Mem %d/%dM",
             cpu_c, mem_used, mem_total);
    draw_str(8, y, tmp, COL_WHITE, COL_BLACK, 1);
    y += 10;

    char upstr[32];
    get_uptime_str(upstr, sizeof(upstr));
    snprintf(tmp, sizeof(tmp), "Up: %s", upstr);
    draw_str(8, y, tmp, COL_WHITE, COL_BLACK, 1);
}

/* ------------------------------------------------------------------ */
/*  Signal handler                                                     */
/* ------------------------------------------------------------------ */

static void sig_handler(int sig)
{
    (void)sig;
    running = 0;
}

/* ------------------------------------------------------------------ */
/*  main                                                               */
/* ------------------------------------------------------------------ */

int main(void)
{
    signal(SIGTERM, sig_handler);
    signal(SIGINT,  sig_handler);

    /* initialise SPI */
    if (spi_init() < 0) { fprintf(stderr, "SPI init failed\n"); return 1; }

    /* request GPIO lines via chardev (/dev/gpiochipN) */
    dc_line_fd  = gpio_request_output(DC_BANK,  DC_OFFSET,  "pcat2-dc",  0);
    rst_line_fd = gpio_request_output(RST_BANK, RST_OFFSET, "pcat2-rst", 1);
    bl_line_fd  = gpio_request_output(BL_BANK,  BL_OFFSET,  "pcat2-bl",  0);

    if (dc_line_fd < 0 || rst_line_fd < 0) {
        fprintf(stderr, "GPIO init failed (DC=%d RST=%d BL=%d)\n",
                dc_line_fd, rst_line_fd, bl_line_fd);
        return 1;
    }

    fprintf(stderr, "pcat2-display: GPIOs OK  DC=bank%d/%d  RST=bank%d/%d  BL=bank%d/%d\n",
            DC_BANK, DC_OFFSET, RST_BANK, RST_OFFSET, BL_BANK, BL_OFFSET);

    disp_init();
    fprintf(stderr, "pcat2-display: display initialised (%dx%d)\n",
            DISP_W, DISP_H);

    /* main loop */
    while (running) {
        render_screen();
        fb_flush();
        for (int i = 0; i < UPDATE_SEC * 10 && running; i++)
            usleep(100000);     /* sleep in 100 ms slices so we exit fast */
    }

    /* cleanup */
    disp_sleep();

    if (dc_line_fd  >= 0) close(dc_line_fd);
    if (rst_line_fd >= 0) close(rst_line_fd);
    if (bl_line_fd  >= 0) close(bl_line_fd);
    if (spi_fd      >= 0) close(spi_fd);

    fprintf(stderr, "pcat2-display: exited cleanly\n");
    return 0;
}
