#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

#define db1 0
#define db2 0
#define db3 0
#define db4 0

#define XON       ('')
#define XOFF      ('')

#define START_STR_SIZE 100
#define SERIAL_BUF_SIZE 1

int  	GetNumberOption (int *argNr, int argCount, char *argVect[], int *ret, char *option);
int	GetStringOption (int *argNr, int argCount, char *argVect[], char *ret, char *option);
int	WaitForRead	(int s, int usecs);
int	WaitForWrite	(int s, int usecs);
int	SetupSerial	();
int	SetInputMode	(FILE *fd, int value, int onoff);
int	SetControlMode	(FILE *fd, int value, int onoff);
int	SetLocalMode	(FILE *fd, int value, int onoff);
void	ParseArgs	(int argc, char *argv[]);

FILE 		*modemFd;
char 		dev[100] 	= "/dev/ttyS0";
char 		filename[100] 	= "";
char 		serial_buf[SERIAL_BUF_SIZE];
int 		baudRates[] 	= {0, 50, 75, 110, 134, 150, 200, 300, 600, 
				   1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
int 		check		= FALSE;
int 		dotime 		= FALSE;
int 		got_start 	= TRUE;
int 		loop    	= FALSE;
int 		crtcts  	= FALSE;
int 		do_print 	= TRUE;
int 		received;
int 		set_baudrate 	= 9600;
int 		xonxoff 	= FALSE;
struct timeval	start;
struct timeval 	stop;


int
main(int argc, char *argv[])
{
  FILE 		*fd;
  char 		*buf;
  int 		ret;
  struct stat 	st;

  ParseArgs(argc, argv);
  SetupSerial();
    
  if (strcmp(filename, "") != 0) {

    /* Read file. */
    if ((fd = fopen(filename, "r")) == NULL) {
      perror("Can't open file\n");
      exit(0);
    }
    
    if (fstat(fileno(fd), &st) != 0) {
      perror("stat failed\n");
      exit(0);
    }
    
    buf = malloc(st.st_size);
    if (fread(buf, 1, st.st_size, fd) != st.st_size) {
      printf("fread failed\n");
    }
    
    fclose(fd);

    printf("# sendserial transmitting %d bytes at baudrate %d.\n", (int)st.st_size, set_baudrate);
    printf("# transmitting...\n");
    /* Write file. */
    {
      int tr_len = 0;
      int rec_len = 0;
      char rec_buf[1000];
      int connected = 0;

      //      while (tr_len != st.st_size) {
      while (1) {
	if (WaitForRead(fileno(modemFd), 0)) { 
	  if((rec_len = fread(rec_buf, 1, 1, modemFd)) > 0) {
	    if (!connected++) {
	      printf("# connected!\n");
	    }
	    fwrite(rec_buf, rec_len, 1, stdout);
	  }
	}
	if (tr_len < st.st_size) {
	  while (!(WaitForWrite(fileno(modemFd), 1000000))) { }
	  fwrite(&buf[tr_len], 1, 1, modemFd);
	  tr_len++;
	}
	else if (tr_len++ == st.st_size) {
	  printf("# ...transmission done.\n");
	}
      }
    }
    
    exit(0);

    do {
      ret = fwrite(buf, 1, st.st_size, modemFd);
      printf("Wrote %d bytes of %d.\n", ret, (int)st.st_size);
    } while(loop);
  }

    {
      int rec_len = 0;
      char rec_buf[1000];
      
      while (loop) {
	if (WaitForRead(fileno(modemFd), 1000000)) { 
	  if((rec_len = fread(rec_buf, 1, 1, modemFd)) > 0) {
	    fwrite(rec_buf, rec_len, 1, stdout);
	  }
	}
      }
    }
    
   return 1;
}

/****************************************************************************
*#
*#  FUNCTION NAME: ParseArgs
*#
*#  PARAMETERS: argc,argv
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES  
*#  ----     ----     -------
*#  960930   ronny    Initial version
*#
*#***************************************************************************/

void
ParseArgs (int argc, char *argv[])
{  
  int 		argi;
  
  for (argi=1; argi < argc; argi++) {
    
    if (strncmp(argv[argi], "--xonxoff", 9) == 0) {
      printf("xonxoff\n");
      xonxoff = TRUE;
    }
    
    else if (strncmp(argv[argi], "--crtcts", 8) == 0) {
      printf("crtcts\n");
      crtcts = TRUE;
    }
    
    else if (strncmp(argv[argi], "--loop", 6) == 0) {
      printf("loop\n");
      loop = TRUE;
    }
    
    else if (strncmp(argv[argi], "--time", 6) == 0) {
      dotime = TRUE;
      got_start = FALSE;
    }
    
    else if (strncmp(argv[argi], "--check", 6) == 0) {
      check = TRUE;
    }
    
    else if (strncmp(argv[argi], "--noprint", 9) == 0) {
      do_print = FALSE;
    }
    
    else if (strncmp(argv[argi], "--device", 8) == 0) {
      if (GetStringOption (&argi, argc, argv, dev, "--device") == 0) {
	printf("--device name\n");
	exit(0);
      }
    }
 
    else if (strncmp(argv[argi], "--file", 6) == 0) {
      if (GetStringOption (&argi, argc, argv, filename, "--file") == 0) {
	printf("--file name\n");
	exit(0);
      }
    }

    else if (strncmp(argv[argi], "--baudrate", 10) == 0) {
      if (GetNumberOption (&argi, argc, argv, &set_baudrate, "--baudrate") == 0) {
	printf("--baudrate baudrate\n");
	exit(0);
      }
    }
    else {
      printf("'%s' unknown command\n", argv[argi]);
      printf("\nsendserial --file fname [--device dname] [--baudrate baudrate] [--xonxoff] [--crtcts]\n\n");
      printf("%dbps on %s default. No handshake.\n", set_baudrate, dev); 
      exit(1);
    }
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: GetNumberOption
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
*#  960930   ronny    Initial version
*#
*#***************************************************************************/

int
GetNumberOption(int *argNr, int argCount, char *argVect[], int *ret, char *option)
{
  int startChar = strlen(option);
  
  if (strlen(argVect[*argNr]) <= (unsigned int) startChar) {
    (*argNr)++;
    startChar = 0;
  }
  
  if (*argNr > argCount) {
    printf("ERROR! The option '%s' needs a number argument.\n", option);
    return (0);
  }

  *ret = atoi(&argVect[*argNr][startChar]);
  return (1);
  
}

/****************************************************************************
*#
*#  FUNCTION NAME: GetStringOption
*#
*#  PARAMETERS: int *argNr      : Returns next argc here. 
*#              int argCount    : Index of last argument.
*#              char *argVect[] : argv.
*#              char *ret       : Copy string here.
*#              char *option    : Name of the option.
*#          
*#  DESCRIPTION: Extracts a string option from argv, and updates argnr.
*#  Returns TRUE/FALSE and string in *ret.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES  
*#  ----     ----     -------
*#  960930   ronny    Initial version
*#  961203   ronny    Handles filenames with spaces within 'file name'.
*#  
*#***************************************************************************/

int
GetStringOption(int *argNr, int argCount, char *argVect[], char *ret, char *option)
{
  int startChar = strlen(option);
  char tmp[256];

  strcpy(ret, "");
  
  /* Are there any more chars after option? If not skip to next argv. */
  if (strlen(argVect[*argNr]) <= (unsigned int)startChar) {
    (*argNr)++;
    startChar = 0;
  }
  
  /* Any args left? */
  if (*argNr >= argCount) {
    printf("ERROR! The option '%s' needs a string argument.\n", option);
    return (FALSE);
  }
  
  strcpy(ret, &argVect[*argNr][startChar]);
  
  if (ret[0] == '\'' || ret[0] == '\"') {
    if (db3) printf("Inside string\n");
    strcpy(tmp, &ret[1]);
    do {
      if (tmp[strlen(tmp) - 1 ] == '\'' || tmp[strlen(tmp) - 1] == '\"') {
        tmp[strlen(tmp) - 1] = '\0';
        break;
      }
      
      (*argNr)++;
      
      if (*argNr >= argCount) {
        printf("ERROR! Mismatched ' or \" in options.\n");
        return (FALSE);
      }
      
      strcat(tmp, &argVect[*argNr][0]);
      if (db3) printf("tmp %s\n", tmp);
    } while (1);
    
    strcpy(ret, tmp);
  }
  if (db4) printf("<<< GetStringOption %s\n", ret);
  return (TRUE);
  
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
SetupSerial()
{
  
  struct termios ti;
  int baudrate;
  
  if ((modemFd = fopen(dev, "r+")) == NULL) {
    perror("Can't open modem\n");
    return(FALSE);
  }
  
  /* Important. Remember this!!!!!!! */
  /*  setvbuf(modemFd, NULL, _IONBF, 0); */

  setvbuf(modemFd, NULL, _IONBF, 0); 
  setvbuf(stdout, NULL, _IONBF, 0); 

  if (fcntl(fileno(modemFd), F_SETFL, O_NONBLOCK) == -1) {
    perror("fcntl: ");
    exit(0);
  }
  
  tcflush(fileno(modemFd), TCIOFLUSH);

  if (tcgetattr (fileno(modemFd), &ti)) {
    perror("tcgetattr");
    return(FALSE);
  }
  
  if (db4) printf("VTIME:%d\n", ti.c_cc[VTIME]);
  if (db4) printf("VMIN:%d\n", ti.c_cc[VMIN]);
  
  ti.c_cc[VMIN]   = 1;
  ti.c_cc[VTIME]  = 0;
  ti.c_cc[VSTART] = XON;
  ti.c_cc[VSTOP]  = XOFF;
  
  if (tcsetattr (fileno(modemFd), TCSANOW, &ti)) {
    perror("tcsetattr");
    return(FALSE);
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
#if defined (B57600)		/* POSIX only defines >= 38400 */
    else if (set_baudrate <= 57600) {
      baudrate = B57600;
      new_baudrate = 57600;
    }
#endif
#if defined (B115200)
    else if (set_baudrate <= 115200) {
      baudrate = B115200;
      new_baudrate = 115200;
    }
#endif
#if defined (B230400)
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
    tcsetattr(fileno(modemFd), TCSANOW, &ti);

    //    printf("* baudrate set to %d\n", new_baudrate);
    set_baudrate = new_baudrate;

    if (db3) printf("outspeed: %d, inspeed: %d\n", baudRates[cfgetospeed(&ti)], baudRates[cfgetispeed(&ti)]);
  }
  
  cfmakeraw(&ti);
  
  SetInputMode(modemFd, IXON,   FALSE);
  SetInputMode(modemFd, IXOFF,  xonxoff); /* input */
  SetInputMode(modemFd, IXANY,  FALSE);
  SetInputMode(modemFd, ICRNL,  FALSE);
  SetInputMode(modemFd, IGNPAR, TRUE);
  SetInputMode(modemFd, IGNBRK, TRUE);
  SetInputMode(modemFd, IGNPAR, TRUE);
  SetInputMode(modemFd, INPCK, FALSE);
  
  SetLocalMode(modemFd, ECHO,   FALSE);
  SetLocalMode(modemFd, ISIG,   FALSE);
  
  SetLocalMode(modemFd, ICANON, FALSE);
  
  SetControlMode(modemFd, PARENB, FALSE);
  SetControlMode(modemFd, CLOCAL, TRUE);
  
  /* Disable hardware flow control. */
#if defined(CRTSCTS)

  SetControlMode(modemFd, CRTSCTS, crtcts);
#if defined(CRTSXOFF)
  SetControlMode(modemFd, CRTSXOFF, crtcts);
#endif

#elif defined(CCTS_OFLOW)

  SetControlMode(modemFd, CCTS_OFLOW, crtcts);
  SetControlMode(modemFd, CRTS_IFLOW, crtcts);

#elif defined(CNEW_RTSCTS)

  SetControlMode(modemFd, CNEW_RTSCTS, crtcts);

#else
  printf("Cannot set hardware flow control. Set it manually with a terminal program.\n");
#endif

  return(TRUE);

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
SetInputMode (FILE *fd, int value, int onoff)
{
  struct termios settings;
  int result;
  
  result = tcgetattr(fileno(fd), &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return (FALSE);
    }
  
  if (onoff)
    settings.c_iflag |= value;
  else
    settings.c_iflag &= ~value;
  
  result = tcsetattr(fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return(FALSE);
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
SetControlMode (FILE *fd, int value, int onoff)
{

#if !defined(_WIN32)
  
  struct termios settings;
  int result;
  
  result = tcgetattr (fileno(fd), &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return (FALSE);
    }
  
  if (onoff)
    settings.c_cflag |= value;
  else
    settings.c_cflag &= ~value;
  
  result = tcsetattr (fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return(FALSE);
    }
  
#endif
  
  return (TRUE);
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
SetLocalMode (FILE *fd, int value, int onoff)
{

#if !defined(_WIN32)
  
  struct termios settings;
  int result;
  
  result = tcgetattr (fileno(fd), &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return (FALSE);
    }
  
  if (onoff)
    settings.c_lflag |= value;
  else
    settings.c_lflag &= ~value;
  
  result = tcsetattr (fileno(fd), TCSANOW, &settings);
  if (result < 0)
    {
      perror ("error in tcgetattr");
      return(FALSE);
    }
  
#endif
  
  return (TRUE);
}

/****************************************************************************
*#
*#  FUNCTION NAME: WaitForRead
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
    return(FALSE);
  }
  else {
    if (db3) printf("* select returned OK:%d\n", ret);
  }
    
  if (FD_ISSET(s, &mask) == 0) {
    if (db2) printf("<<< WaitForRead. No data to read.\n");
    return (FALSE);
  }
  
  if (db3) printf("<<< WaitForRead OK. Returned:%d.\n", ret);
  
  return(TRUE);
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
    return(FALSE);
  }
  else {
    if (db3) printf("* select returned OK:%d\n", ret);
  }
    
  if (FD_ISSET(s, &mask) == 0) {
    if (db2) printf("<<< WaitForWrite. No data to read.\n");
    return (FALSE);
  }
  
  if (db3) printf("<<< WaitForWrite OK. Returned:%d.\n", ret);
  
  return(TRUE);
}

