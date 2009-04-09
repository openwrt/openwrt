#ifndef _LINUX_ETRAXI2C_H
#define _LINUX_ETRAXI2C_H

/* etraxi2c _IOC_TYPE, bits 8 to 15 in ioctl cmd */

#define ETRAXI2C_IOCTYPE 44

/* supported ioctl _IOC_NR's */

/* in write operations, the argument contains both i2c
 * slave, register and value.
 */

#define I2C_WRITEARG(slave, reg, value) (((slave) << 16) | ((reg) << 8) | (value))
#define I2C_READARG(slave, reg) (((slave) << 16) | ((reg) << 8))

#define I2C_ARGSLAVE(arg) ((arg) >> 16)
#define I2C_ARGREG(arg) (((arg) >> 8) & 0xff)
#define I2C_ARGVALUE(arg) ((arg) & 0xff)

#define I2C_WRITEREG    0x1   /* write to an I2C register */
#define I2C_READREG     0x2   /* read from an I2C register */

/*
EXAMPLE usage:

    i2c_arg = I2C_WRITEARG(STA013_WRITE_ADDR, reg, val);
    ioctl(fd, _IO(ETRAXI2C_IOCTYPE, I2C_WRITEREG), i2c_arg);

    i2c_arg = I2C_READARG(STA013_READ_ADDR, reg);
    val = ioctl(fd, _IO(ETRAXI2C_IOCTYPE, I2C_READREG), i2c_arg);

*/

/* Extended part */
#define I2C_READ        0x4     /* reads from I2C device */
#define I2C_WRITE       0x3     /* writes to I2C device */
#define I2C_WRITEREAD   0x5	    /* writes to I2C device where to start reading */

typedef struct _I2C_DATA 
{
    unsigned char slave;        /* I2C address (8-bit representation) of slave device */
    unsigned char wbuf[256];    /* Write buffer (length = 256 bytes) */
	unsigned int  wlen;         /* Number of bytes to write from wbuf[] */   
	unsigned char rbuf[256];    /* Read buffer (length = 256 bytes) */
    unsigned int  rlen;         /* Number of bytes to read into rbuf[] */
} I2C_DATA;

#endif
