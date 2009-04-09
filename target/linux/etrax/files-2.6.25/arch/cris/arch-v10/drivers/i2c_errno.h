#ifndef _I2C_ERRNO_H
#define _I2C_ERRNO_H

#define EI2CNOERRORS  0     /* All fine */
#define EI2CBUSNFREE  1     /* I2C bus not free */
#define EI2CWADDRESS  2     /* Address write failed */
#define EI2CRADDRESS  3     /* Address read failed */
#define EI2CSENDDATA  4     /* Sending data failed */
#define EI2CRECVDATA  5     /* Receiving data failed */
#define EI2CSTRTCOND  6     /* Start condition failed */
#define EI2CRSTACOND  7     /* Repeated start condition failed */
#define EI2CSTOPCOND  8     /* Stop condition failed */
#define EI2CNOSNDBYT  9     /* Number of send bytes is 0, while there's a send buffer defined */ 
#define EI2CNOSNDBUF  10    /* No send buffer defined, while number of send bytes is not 0 */
#define EI2CNORCVBYT  11    /* Number of receive bytes is 0, while there's a receive buffer defined */
#define EI2CNORCVBUF  12    /* No receive buffer defined, while number of receive bytes is not 0 */
#define EI2CNOACKNLD  13    /* No acknowledge received from slave */
#define EI2CNOMNUMBR  14    /* No MAJOR number received from kernel while registering the device */

#endif /* _I2C_ERRNO_H */
