/*!***************************************************************************
*!
*! FILE NAME  : serial.c
*!
*! DESCRIPTION: Serial boot stuff for e100boot.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! 1996         Ronny Raneup       Initial version
*! 2002 05 02   Ronny Ranerup      Moved it into this file
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999-2003 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/

/**************************  Include files  ********************************/

#include <conf.h>
#include <e100boot.h>
#include <serial.h>
#include <common.h>

/**************************  Type definitions  *****************************/

/**************************  Global variables  *****************************/

FILE *serialFd;
int set_baudrate = 9600;   /* Baudrate for serial boot (after bootloader). */

/**************************  Constants and macros  *************************/

/**************************  Function prototypes  **************************/

int SetControlMode(FILE *fd, int value, int onoff);
int SetInputMode(FILE *fd, int value, int onoff);
int SetLocalMode(FILE *fd, int value, int onoff);
int SetOutputMode(FILE *fd, int value, int onoff);
int WaitForRead(int s, int usecs);
int WaitForWrite(int s, int usecs);
void setBaudRate(unsigned int b);

/****************************************************************************
*#
*#  FUNCTION NAME: SerBoot
*#
*#  DESCRIPTION: The main serial boot routine.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2000 01 07   ronny    Initial version
*#
*#***************************************************************************/

void
SerBoot(void)
{
  int                   seq = 0;
  struct packet_buf     *p;
  int                   got_new_ack = 0;
  int                   connected = 0;
  char                  rec_buf;

  while ((p = create_packet(seq))) {
    int offset = 0;
    int tr_len = 0;
    static char fName[MAX_STRING_LEN] = {0,};

    if (strcmp(fName, p->boot_file->fileName) != 0) {
      while (connected && got_new_ack == 0) {
        WaitForRead(fileno(serialFd), 1000000); /* Did we get data? */

        if(fread(&rec_buf, 1, 1, serialFd) > 0) {
          fwrite(&rec_buf, 1, 1, stderr);
          if (rec_buf == '+') {
            printf("# Got ack.\n");
            got_new_ack = 1;
          }
        }
      }

      strcpy(fName, p->boot_file->fileName);
      printf("# transmitting file '%s'\n", fName);
      got_new_ack = 0;
    }

    printf("# transmitting %d bytes at baudrate %d.\n", (int)p->size, p->baud_rate);

    // setBaudRate(p->baud_rate);

    while (tr_len != p->size) {

      if (WaitForRead(fileno(serialFd), 0)) { /* Did we get data? */
        //      static int connected = 0;
        char rec_buf = 0;
        int rec_len = 0;

        if((rec_len = fread(&rec_buf, 1, 1, serialFd)) > 0) {
          if (rec_buf == '+') {
            if (!connected++) {
              printf("# Got ack.\n");
            }
            else {
              got_new_ack = 1;
              printf("# Got ack.\n");
            }
            fwrite(&rec_buf, rec_len, 1, stderr);
          }
        }
      }

      if (tr_len < p->size) {   /* send one byte */
        while (!(WaitForWrite(fileno(serialFd), 1000000))) { }
        fwrite(&p->data[offset+tr_len], 1, 1, serialFd);
        tr_len++;
      }
    }

    seq++; /* We have transmitted entire packet. */
  }

  printf("# Transmission done. Just listening now.\n");

  while(1) {
    if (WaitForRead(fileno(serialFd), 10000000)) { /* Did we get data? */
      char rec_buf;
      int rec_len = 0;

      if((rec_len = fread(&rec_buf, 1, 1, serialFd)) > 0) {
        fwrite(&rec_buf, rec_len, 1, stderr);
      }
    }
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: SetupSerial
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  970325   ronny    Initial version
*#
*#***************************************************************************/

int
SetupSerial(void)
{
  int baudRates[] = {0, 50, 75, 110, 134, 150, 200, 300, 600,
                     1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
  struct termios ti;
  int baudrate;

  if ((serialFd = fopen(device, "r+")) == NULL) {
    perror("Can't open modem");
    return FALSE;
  }

  /* Important. Remember this!!!!!!! */
  /*  setvbuf(serialFd, NULL, _IONBF, 0); */

  setvbuf(serialFd, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);

  if (fcntl(fileno(serialFd), F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl");
    return FALSE;
  }

  tcflush(fileno(serialFd), TCIOFLUSH);

  if (tcgetattr(fileno(serialFd), &ti)) {
    perror("tcgetattr");
    return FALSE;
  }

  ti.c_cc[VMIN]   = 1;
  ti.c_cc[VTIME]  = 0;
  ti.c_cc[VSTART] = 0x11;
  ti.c_cc[VSTOP]  = 0x13;

  if (tcsetattr(fileno(serialFd), TCSANOW, &ti)) {
    perror("tcsetattr");
    return FALSE;
  }

  //if (db3) printf("outspeed: %d, inspeed: %d\n", baudRates[cfgetospeed(&ti)], baudRates[cfgetispeed(&ti)]);
  {
    int new_baudrate = 0;

    if (set_baudrate <= 50) {
      baudrate = B50;
      new_baudrate = 50;
    }
    else if (set_baudrate <= 75) {
      baudrate = B75;
      new_baudrate = 75;
    }
    else if (set_baudrate <= 110) {
      baudrate = B110;
      new_baudrate = 110;
    }
    else if (set_baudrate <= 134) {
      baudrate = B134;
      new_baudrate = 134;
    }
    else if (set_baudrate <= 150) {
      baudrate = B150;
      new_baudrate = 150;
    }
    else if (set_baudrate <= 200) {
      baudrate = B200;
      new_baudrate = 200;
    }
    else if (set_baudrate <= 300) {
      baudrate = B300;
      new_baudrate = 300;
    }
    else if (set_baudrate <= 600) {
      baudrate = B600;
      new_baudrate = 600;
    }
    else if (set_baudrate <= 1200) {
      baudrate = B1200;
      new_baudrate = 1200;
    }
    else if (set_baudrate <= 1800) {
      baudrate = B1800;
      new_baudrate = 1800;
    }
    else if (set_baudrate <= 2400) {
      baudrate = B2400;
      new_baudrate = 2400;
    }
    else if (set_baudrate <= 4800) {
      baudrate = B4800;
      new_baudrate = 4800;
    }
    else if (set_baudrate <= 9600) {
      baudrate = B9600;
      new_baudrate = 9600;
    }
    else if (set_baudrate <= 19200) {
      baudrate = B19200;
      new_baudrate = 19200;
    }
    else if (set_baudrate <= 38400)
      baudrate = B38400;
#if defined(B57600)            /* POSIX only defines <= 38400 */
    else if (set_baudrate <= 57600) {
      baudrate = B57600;
      new_baudrate = 57600;
    }
#endif
#if defined(B115200)
    else if (set_baudrate <= 115200) {
      baudrate = B115200;
      new_baudrate = 115200;
    }
#endif
#if defined(B230400)
    else if (set_baudrate <= 230400) {
      baudrate = B230400;
      new_baudrate = 230400;
    }
#endif
    else {
      baudrate = B38400;
    }

    cfsetispeed(&ti, B9600);
    cfsetospeed(&ti, baudrate);
    tcsetattr(fileno(serialFd), TCSANOW, &ti);

    //    printf("* baudrate set to %d\n", new_baudrate);
    set_baudrate = new_baudrate;

    if (db3) printf("outspeed: %d, inspeed: %d\n", baudRates[cfgetospeed(&ti)], baudRates[cfgetispeed(&ti)]);
  }

  cfmakeraw(&ti);

  SetInputMode(serialFd, IXON,   FALSE);
  SetInputMode(serialFd, IXOFF,  FALSE);
  SetInputMode(serialFd, IXANY,  FALSE);
  SetInputMode(serialFd, ICRNL,  FALSE);
  SetInputMode(serialFd, IGNPAR, TRUE);
  SetInputMode(serialFd, IGNBRK, TRUE);
  SetInputMode(serialFd, IGNPAR, TRUE);
  SetInputMode(serialFd, INPCK, FALSE);

  SetOutputMode(serialFd, OPOST,  FALSE);
  SetOutputMode(serialFd, ONLCR,  FALSE);
#ifdef OXTABS
  SetOutputMode(serialFd, OXTABS, FALSE);
#endif

#ifdef ENOEOT
  SetOutputMode(serialFd, ENOEOT, FALSE);
#endif

  SetLocalMode(serialFd, ECHO,   FALSE);
  SetLocalMode(serialFd, ISIG,   FALSE);

  SetLocalMode(serialFd, ICANON, FALSE);

  SetControlMode(serialFd, PARENB, FALSE);
  SetControlMode(serialFd, CLOCAL, TRUE);
  SetControlMode(serialFd, CREAD, TRUE);

  /* Disable hardware flow control. */
#if defined(CRTSCTS)

  SetControlMode(serialFd, CRTSCTS, FALSE);
#if defined(CRTSXOFF)
  SetControlMode(serialFd, CRTSXOFF, FALSE);
#endif

#elif defined(CCTS_OFLOW)

  SetControlMode(serialFd, CCTS_OFLOW, FALSE);
  SetControlMode(serialFd, CRTS_IFLOW, FALSE);

#elif defined(CNEW_RTSCTS)

  SetControlMode(serialFd, CNEW_RTSCTS, FALSE);

#else
  printf("Cannot set hardware flow control. Set it manually with a terminal program.\n");
#endif

  return TRUE;
}

void
setBaudRate(unsigned int b)
{
  int baudRates[] = {0, 50, 75, 110, 134, 150, 200, 300, 600,
                     1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
  struct termios ti;

  if (tcgetattr(fileno(serialFd), &ti)) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: SetInputMode
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  970325   ronny    Initial version
*#
*#***************************************************************************/

int
SetInputMode(FILE *fd, int value, int onoff)
{
  struct termios settings;
  int result;

  result = tcgetattr(fileno(fd), &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  if (onoff)
    settings.c_iflag |= value;
  else
    settings.c_iflag &= ~value;

  result = tcsetattr(fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  return 1;
}

/****************************************************************************
*#
*#  FUNCTION NAME: SetOutputMode
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE          NAME     CHANGES
*#  ----          ----     -------
*#  2001 May 16   ronny    Initial version
*#
*#***************************************************************************/

int
SetOutputMode(FILE *fd, int value, int onoff)
{
  struct termios settings;
  int result;

  result = tcgetattr(fileno(fd), &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  if (onoff)
    settings.c_oflag |= value;
  else
    settings.c_oflag &= ~value;

  result = tcsetattr(fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  return 1;
}

/****************************************************************************
*#
*#  FUNCTION NAME: SetControlMode
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  970325   ronny    Initial version
*#
*#***************************************************************************/

int
SetControlMode(FILE *fd, int value, int onoff)
{
#if !defined(_WIN32)
  struct termios settings;
  int result;

  result = tcgetattr(fileno(fd), &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  if (onoff)
    settings.c_cflag |= value;
  else
    settings.c_cflag &= ~value;

  result = tcsetattr(fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }
#endif

  return TRUE;
}
/****************************************************************************
*#
*#  FUNCTION NAME: SetLocalMode
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  970325   ronny    Initial version
*#
*#***************************************************************************/

int
SetLocalMode(FILE *fd, int value, int onoff)
{
#if !defined(_WIN32)
  struct termios settings;
  int result;

  result = tcgetattr(fileno(fd), &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }

  if (onoff)
    settings.c_lflag |= value;
  else
    settings.c_lflag &= ~value;

  result = tcsetattr(fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror("error in tcgetattr");
      return FALSE;
    }
#endif

  return TRUE;
}

/****************************************************************************
*#
*#  FUNCTION NAME: WaitForRead
*#
*#  PARAMETERS: Timeout in usecs.
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961001   ronny    Initial version
*#  971204   ronny    usecs was a only one tenth of what it should be.
*#
*#***************************************************************************/

int
WaitForRead(int s, int usecs)
{
  fd_set mask;
  int ret;
  struct timeval tm;

  tm.tv_sec  = usecs/1000000;
  tm.tv_usec = usecs%1000000;

  FD_ZERO(&mask);
  FD_SET(s, &mask);

  if ((ret = select(FD_SETSIZE, &mask, NULL, NULL, &tm)) == -1) {
    printf("<<< select failed with return: %d.\n", ret);
    return FALSE;
  }
  else {
    if (db3) printf("* select returned OK:%d\n", ret);
  }

  if (FD_ISSET(s, &mask) == 0) {
    if (db2) printf("<<< WaitForRead. No data to read.\n");
    return FALSE;
  }

  if (db3) printf("<<< WaitForRead OK. Returned:%d.\n", ret);

  return TRUE;
}

/****************************************************************************
*#
*#  FUNCTION NAME: WaitForWrite
*#
*#  PARAMETERS:
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2000-11-20   ronny    Initial version
*#
*#***************************************************************************/

int
WaitForWrite(int s, int usecs)
{
  fd_set mask;
  int ret;
  struct timeval tm;

  tm.tv_sec  = usecs/1000000;
  tm.tv_usec = usecs%1000000;

  FD_ZERO(&mask);
  FD_SET(s, &mask);

  if ((ret = select(FD_SETSIZE, NULL, &mask, NULL, &tm)) == -1) {
    printf("<<< select failed with return: %d.\n", ret);
    return FALSE;
  }
  else {
    if (db3) printf("* select returned OK:%d\n", ret);
  }

  if (FD_ISSET(s, &mask) == 0) {
    if (db2) printf("<<< WaitForWrite. No data to read.\n");
    return FALSE;
  }

  if (db3) printf("<<< WaitForWrite OK. Returned:%d.\n", ret);

  return TRUE;
}
