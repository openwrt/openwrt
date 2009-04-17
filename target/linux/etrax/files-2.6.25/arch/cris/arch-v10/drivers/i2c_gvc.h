#ifndef _I2C_H
#define _I2C_H

int i2c_init(void);

/* High level I2C actions */
int i2c_writereg(unsigned char theSlave, unsigned char theReg, unsigned char theValue);
unsigned char i2c_readreg(unsigned char theSlave, unsigned char theReg);

/* Low level I2C */
int i2c_start(void);
int i2c_stop(void);
int i2c_outbyte(unsigned char x);
unsigned char i2c_inbyte(void);
int i2c_getack(void);
void i2c_sendack(void);
void i2c_sendnack(void);

/**GVC**/
/* New low level I2C functions */
int i2c_read( unsigned char slave, unsigned char* rbuf, unsigned char rlen );
int i2c_write( unsigned char slave, unsigned char* wbuf, unsigned char wlen );
int i2c_writeread( unsigned char  slave
                 , unsigned char* wbuf
                 , unsigned char  wlen
                 , unsigned char* rbuf
                 , unsigned char  rlen
                 );
/**END GVC**/
#endif /* _I2C_H */
