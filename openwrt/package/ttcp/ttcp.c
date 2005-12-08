/*
 *        T T C P . C
 *
 * Test TCP connection.  Makes a connection on port 5010
 * and transfers fabricated buffers or data copied from stdin.
 *
 * Usable on 4.2, 4.3, and 4.1a systems by defining one of
 * BSD42 BSD43 (BSD41a)
 * Machines using System V with BSD sockets should define SYSV.
 *
 * Modified for operation under 4.2BSD, 18 Dec 84
 *       T.C. Slattery, USNA
 * Minor improvements, Mike Muuss and Terry Slattery, 16-Oct-85.
 * Modified in 1989 at Silicon Graphics, Inc.
 *        catch SIGPIPE to be able to print stats when receiver has died 
 *        for tcp, don't look for sentinel during reads to allow small transfers
 *        increased default buffer size to 8K, nbuf to 2K to transfer 16MB
 *        moved default port to 5001, beyond IPPORT_USERRESERVED
 *        make sinkmode default because it is more popular, 
 *                -s now means don't sink/source 
 *        count number of read/write system calls to see effects of 
 *                blocking from full socket buffers
 *        for tcp, -D option turns off buffered writes (sets TCP_NODELAY sockopt)
 *        buffer alignment options, -A and -O
 *        print stats in a format that's a bit easier to use with grep & awk
 *        for SYSV, mimic BSD routines to use most of the existing timing code
 * Modified by Steve Miller of the University of Maryland, College Park
 *        -b sets the socket buffer size (SO_SNDBUF/SO_RCVBUF)
 * Modified Sept. 1989 at Silicon Graphics, Inc.
 *        restored -s sense at request of tcs@brl
 * Modified Oct. 1991 at Silicon Graphics, Inc.
 *        use getopt(3) for option processing, add -f and -T options.
 *        SGI IRIX 3.3 and 4.0 releases don't need #define SYSV.
 *
 * Modified --> Nov 1996 at CERN (Daniel DAVIDS)
 *        printout of the Socket-Buffer-Sizes 
 *        configured for HP-UX 9000 OS        
 *        configured for Windows NT OS 
 * Modified Dec 1996 at CERN (Jacques ROCHEZ)
 *        severe cleanup
 *        addaptation to the gcc compiler (ANSI)
 *        configured for Lynx OS 
 *        automatic format for the rate display (G/M/K/)bytes/sec
 *        added log (L) and more help (h) options.
 * Modified May 1997 at CERN (Jacques ROCHEZ) 
 *        removed the mes() function following err() function.
 *        changed the default port to 5010
 * Modified jul 1997 at CERN (Jacques ROCHEZ)
 *        adapted the timing calculation in microseconds 
 *        addapted the code for Vsisual C++ under NT4.0 
 * Modified aug 1997 at CERN (Jacques ROCHEZ)
 *        initialise to 0 the variables nbytes, numcalls  
 *        moved the buffer pre-load outside the measured timed area 
 * Distribution Status -
 *        Public Domain.  Distribution Unlimited.
 * Modified jun 1998 at Epigram
 *        - print hash marks to indicate prograss on transmitting side
 *        - sleep between bursts of buffers
 *        - set number of buffers/burst
 * Modified jul 1998 at Epigram
 *        - send random size buffers
 * Modified jan 1999 at Epigram (WES)
 *        - added start and end patterns to UDP start and end packets
 *        - added handling of missed start, end, or data packets for UDP tests
 */
#ifndef lint
static char RCSid[] = "ttcp.c $- CERN Revision: 3.8 (dev level) -$";
#endif

static char VersDate[] = "Epigram 15-jul-98";

/*                system dependent setting
 *                ------------------------
 * uname -a,gcc -v a.c are the tools used                              
 *
 * Platform/OS          #define         MACRO predefined 
 * -----------          -------   ---------------------------------------------------
 * SunOS OS             BSD43     __unix__   __sun__   __sparc__
 * SUN Solaris           SYSV     __unix__   __sun__   __sparc__   __svr4__ 
 * SGI-IRIX  < 3.3 SYSV     set as #define sgi
 * HP-UX 9000           SYSV      __unix__   __hpux__  __hp9k8__
 * OSF/1 V3.2           SYSV      __unix__   __osf__   __alpha__
 * OSF/1 V4.0           SYSV      __unix__   __osf__   __alpha__   _CFE
 * LynxOS               SYSV      __unix__   __lynx__  __powerpc__
 * Windows NT           SYSV                 __WINNT__ __i386__    __M_COFF__
 * AIX                  SYSV      _AIX       _AIX32    _POWER      _IBMR2   _ARCH_PWR
 

 * Micosoft Visual C++ compiler under WindowNT 4.0
 * Windows NT                    _WINDOWS    WIN32 

 * Unix BSD 41a         BSD41a
 *          42          BSD42
 *          43          BSD43
 
 * Machines using System V with BSD sockets should define SYSV.
 *
 *            Compiler commands 
 *            -----------------
 * LynxOS : gcc -c ttcp.c -o ttcp.o | gcc -o ttcp -O ttcp.o -lnetinet -lc_p -lc
 */

/* -------------attempt to set an automatic UNIX  OS family detection -------*/

#if defined(__hp9k8__) || defined(__osf__) || defined(__srv4__)  
#define SYSV
#endif
#if defined(__lynx__)  
#define SYSV
#endif
/* for solaris (__srv4__) the parameters SYSV is already set */

/* version A.09.01  'key words' differs from A.09.05 A */ 
#if defined(__hpux)
#define __hpux__
#endif

#if defined(__sun__)&&!defined(__srv4__)
#define BSD43                  /* changed by BSD42 if necessary */
#endif

#if defined(_WIN32)                /* Window NT 4.0 compiled with VisualC++ 4 */
#define __NTVIS__
#define SYSV
#endif 

#if defined(__FreeBSD__)
#define BSD43
#endif

#if defined(__linux__)
#define BSD43
#endif
/*--------------------------------------------------------------------------*/

#if !defined(UNDER_CE)
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#endif

#if defined(SYSV)

#if defined(__osf__) 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>           /* struct timeval */
#include <sys/resource.h>       /* definition of struct rusage */    

#else  /* else of __osf__ */ 
#if defined(__NTVIS__)
#include <winsock2.h>
#include <windows.h>            /* required for all Windows applications */
#include <ws2tcpip.h>
#include <memory.h>
#include <time.h>
#if !defined(UNDER_CE)
#include <sys\timeb.h> 
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <io.h>
struct rusage { struct timeval ru_utime, ru_stime; };
#define RUSAGE_SELF 0
#else  /* else of __NTVIS__ */  
#if defined(__lynx__)
#include <socket.h>             /* located in  /usr/include/..... */
#include <netinet/in.h>    
#include <netinet/tcp.h> 
#include <arpa/inet.h>     
#include <netdb.h>         
#include <time.h> 
#include <resource.h>           /* definition of struct rusage */    
#include <sys/times.h>
#define RUSAGE_SELF 0
#include <conf.h>               /* definition of TICKSPERSEC (HZ) */
#include <sys/param.h>     

#else  /* else of __Lynx__  */
#if defined(__svr4__)
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>           /* struct timeval */
#include <sys/resource.h>       /* definition of struct rusage */    
#include <sys/times.h>
#define RUSAGE_SELF 0
#include <sys/param.h>     

#else  /* else of __svr4__    all SYSV cases except those mentionned before */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>           /* struct timeval */
#include <sys/resource.h>       /* definition of struct rusage */    
#include <sys/times.h>
#define RUSAGE_SELF 0
#include <sys/param.h>     

#endif /* __svr4__  */
#endif /* __lynx__  */
#endif /* __NTVIS__ */
#endif /* __osf__   */

#else  /* else of SYSV      it is a BSD OS  */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#if defined(__linux__)
#include <time.h>           /* struct timeval */
#else
#include <sys/time.h>           /* struct timeval */
#endif
#include <sys/resource.h>       /* definition of struct rusage */
#include <unistd.h>		/* for usleep() - henry */
#define SOCKET_ERROR	-1

#endif /* SYSV */

#if defined(__NTVIS__)

#if defined(_DEBUG)     /* usual debug symbol for VC++ */
#define DEBUG 1
#endif

void usleep(unsigned int microseconds);

#define bcopy(a,b,n) memcpy((b), (a), (n))
#define bzero(a,n) memset((a), 0, (n))

#ifndef _GETOPT_
#define _GETOPT_
int getopt(int argc, char **argv, char *optstring);

extern char *optarg;                // returned arg to go with this option
extern int optind;                  // index to next argv element to process
extern int opterr;                  // should error messages be printed?
extern int optopt;                  //

#define BADCH ('?')
#endif // _GETOPT

/* get option letter from argument vector  */
int
        opterr = 1,                  // should error messages be printed?
        optind = 1,                  // index into parent argv vector
        optopt;                      // character checked for validity
char   *optarg;                      // argument associated with option

#define EMSG        ""
char *progname;                      // may also be defined elsewhere
#endif /*__NTVIS__*/

/* sockaddr_in ==  file server address structure 
 * 
 * Socket address, internet style.   declared in : /netinet/in.h
 * struct sockaddr_in {short   sin_family;
 *                     u_short sin_port;
 *                     struct  in_addr sin_addr;
 *                     char    sin_zero[8];
 *                    };
 *
 * Structure used by kernel to store most addresses. declared in ./sys/socket.h
 * struct sockaddr{u_short sa_family;       address family
 *                 char    sa_data[14];     up to 14 bytes of direct address 
 *                };
 * PS : sin stand for "socket internet number" 
 */

#ifndef __NTVIS__
#define FAR
#endif

#if     defined(__CONST_SOCKADDR_ARG)
#define SOCKADDR_CAST   (__CONST_SOCKADDR_ARG)
#elif  defined(__lynx__) || defined(__sun__) || defined(_AIX) || defined(__FreeBSD__) || defined(__NTVIS__)
#define SOCKADDR_CAST   (struct sockaddr FAR *)
#else
#define SOCKADDR_CAST
#endif

#if defined(__sun__)
struct sockaddr_in sockaddr;   /* done in ./X11/Xdmcp.h */
#endif 

struct sockaddr_in sinme;     /* is the socket struct. in the local host */
struct sockaddr_in sinhim;    /* is the socket struc. in the remote host */

#if defined(__lynx__) || defined(__svr4__) || defined(_AIX) 
struct sockaddr frominet;
#else
struct sockaddr_in frominet;
#endif /* __lynx__ */

int domain, fromlen;

#if !defined(__NTVIS__)
#define SOCKET int
#endif /* __NTVIS__ */
SOCKET fd;                             /* fd of network socket */

#if !defined(__lynx__)
extern int errno;
#endif 

#include <stdio.h>

FILE  *fplog = NULL;                   /* file pointer for the log file */
char  logfile[100];                    /* file name for the log */
static char logfile_head[] ="ttcp_log";   /* header  name for the log */
int   buflen = 8 * 1024;               /* length of buffer */
char  *buf;                            /* ptr to dynamic buffer */
int   nbuf = 2 * 1024;                 /* number of buffers to send in sinkmode */

#define START_PKT_LEN 4
#define START_PATTERN_LEN 4
#define END_PKT_LEN 8
#define END_PATTERN_LEN 4
#define MIN_UDP_LEN 5

char start_pattern[START_PATTERN_LEN] = {1, 2, 3, 4};
char end_pattern[END_PATTERN_LEN] = {2, 2, 2, 2};

int   bufoffset = 0;                   /* align buffer to this */
int   bufalign = 16*1024;              /* modulo this */

int   udp = 0;                         /* 0 = tcp, !0 = udp */
int   options = 0;                     /* socket options */
int   one = 1;                         /* for 4.3 BSD style setsockopt() */
short port = 5010;                     /* TCP port number */
char *host;                            /* ptr to name of host */
int  rndm = 0;                         /* 0="normal", !0=random data */
int  trans;                            /* 0=receive, !0=transmit mode */
int  timeout;                          /* read timeout in millisec */
int  debug = 0;                        /* 0=No-Debug, 1=Debug-Set-On */
int  sinkmode = 0;                     /* 0=normal I/O, !0=sink/source mode */
int  verbose = 0;                      /* 0=print basic info, 1=print cpu rate,
                                        * proc resource usage. */
int  nodelay = 0;                      /* set TCP_NODELAY socket option */
int  pri = 0;                          /* link layer priority */
int  b_flag = 0;                       /* use mread() */
int  log_cnt = 0;                      /* append result to a log */
int  sockbufsize = 0;                  /* socket buffer size to use */
char fmt = 'A';                        /* output format:
                                        *  b = bits,     B = bytes,
                                        *  k = kilobits, K = kilobytes,
                                        *  m = megabits, M = megabytes, 
                                        *  g = gigabits, G = gigabytes,
                                        *  A = automatic Xbytes (default) */
int  touchdata = 0;                    /* access data after reading */
int  seq_info = 0;                     /* report out of order seq nums */

int hash = 0;                          /* print hash marks for each buffer */
int udpsleep = 0;                      /* sleep in between udp buffers */
int burstn = 1;                        /* number of buffers per burst */
int bufmin = -1;                       /* minimum buffer size to use when
                                          sending random-size buffers */
unsigned int seed = 1;                 /* seed for random number generator
                                          used for random buffer lengths */
int no_start = 0;                      /* skip the start frames for UDP */
int no_data = 0;                       /* skip all data frames for UDP */
int no_end = 0;                        /* skip the end frames for UDP */

double   nbytes;                       /* bytes on net */
unsigned long numCalls;                /* # of I/O system calls */

struct hostent *addr;
extern int optind;
extern char *optarg;

#if defined(UNDER_CE)
static int errno;
static char *weekday[] ={"Sun", "Mon","Tues", "Wed", "Thurs", "Fri", "Sat"};
static char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "July", "Aug", "Sep", "Oct", "Nov", "Dec"};
#define perror printf
#endif

/*--------------------------------------------------------------------------*/
static struct timeval time0;    /* Time at which timing started */
static struct timeval time1;
static struct rusage ru0;       /* Resource utilization at the start */
static struct rusage ru1;
static struct tm *tms;        /* system time structure */


/*-----------Prototype functions definitions -------------------------------*/
/*--------------------------------------------------------------------------*/

                                   /* ANSI input/output functions (stdio.h) */

#if defined(__lynx__)
int  getopt(int, char**, char*);
int  gettimeofday(struct timeval *tp, struct timezone *tzp);

#else
#if defined(__svr4__)

#else 
#if defined(_AIX)

#else
#if defined(__hpux__)
#else
#if defined(__NTVIS__)
#else 
#if defined(BSD42) || defined(BSD43)
#else

int  printf( char*, ...);
int  fprintf(FILE*,char*, ...);
void perror(char*);
int  getopt(int, char**, char*);
int  gettimeofday(struct timeval *tp, struct timezone *tzp);
#endif  /* BSD42 || BSD43 */
#endif  /* __NTVIS__ */
#endif  /* __hpux__ */
#endif  /*  _AIX    */
#endif  /* __svr4__ */
#endif  /* __lynx__ */ 

int main(int argc, char* argv[]); 
#if ( (!(defined(BSD42))) && (!(defined(BSD43))) && (!(defined(__NTVIS__))) )
int  read(int, char*, int);
int  write(int, char*, int);
int  close(int);
#endif /* !(BSD42) && !(BSD43) */
int  fclose(FILE *stream);

#if !defined(BSD43) && !defined(__NTVIS__)
void bzero(char*,int);
void bcopy(char*, char*, int);
char strncpy(char *s1,char *s2,size_t n);
int  atoi(char*);
int  malloc(int);
#endif


                           /* ANSI socket functions prototype /sys/socket.h */
#if defined(__lynx__)
int  select(int, fd_set*, fd_set*, fd_set*, struct timeval*); 

#else 
#if defined(__svr4__) && !defined(sparc)

/*  informations in : /usr/include/sys/socket.h */
int  socket(int, int, int); 
int  connect(int, struct sockaddr *, int);
int  bind(int, struct sockaddr *, int);
int  listen(int, int);
int  accept(int, struct sockaddr *, int *);
int  sendto(int, const char *, int, int, const struct sockaddr *, int);
int  recvfrom(int, char *, int, int, struct sockaddr *, int *);
int  getpeername(int, struct sockaddr *, int *);
int  getsockopt(int, int, int, char *, int *);
int  select(int, fd_set*, fd_set*, fd_set*, struct timeval*); 

#else
#if defined(_AIX)
int  select(unsigned long, void *, void *, void *, struct timeval *);

#else
#if defined(__hpux__)
int getrusage(int who,struct rusage *rusage);

#else 
#if defined(__NTVIS__)

#else
#if defined(BSD42) || defined(BSD43)

#else

int  socket(int, int, int);
int  connect(int s,struct sockaddr_in *name, int namelen);
int  bind(int s,struct sockaddr *name,int namelen);
int  listen(int, int);
int  accept(int, struct sockaddr_in *, int *);
int  sendto(int, char *, int, int, struct sockaddr_in *, int);
int  recvfrom(int, char *, int, int, struct sockaddr_in *, int *);
int  getpeername(int, struct sockaddr *, int *);
int  setsockopt(int, int, int, char *, int);

int  getsockopt(int, int, int, char*, int*);
int  select(int, fd_set*, fd_set*, fd_set*, struct timeval*); 

#endif /* BSD42 || BSD43 */
#endif /* __hpux__ */
#endif /*  _AIX    */
#endif /* __svr4__ */
#endif /* __lynx__ */ 
#endif /* __NTVIS__ */

/* ttcp prototype functions */
void udp_rcv_test(void);
double calc_cpu_time(struct rusage *r0, struct rusage *r1);
double calc_real_time(struct timeval *t0, struct timeval *t1);
int  getrusage(int who,struct rusage *rusage);
void sockets_err(char*);
void err(char *);
void mes(char *);
void pattern(char *, int);
int  Nread(SOCKET, void *, int);
int  Nwrite(int, void *, int);
void delay(int);
int  mread(int, char *,unsigned);
char *outfmt(char format, double b);
void prep_timer(void);
void read_timer(void);
void result_summary(void);
void prusage(struct rusage*,  struct rusage*,
             struct timeval*, struct timeval*); 
void tvadd(struct timeval *tsum, struct timeval *t0,struct timeval *t1);
void tvsub(struct timeval *tdiff, struct timeval *t1, struct timeval *t0);
void psecs(int);
void open_log(void);
void close_log(void);
void do_Usage(void);
void do_help(void);

/*--------------------------------------------------------------------------*/
#if !defined(__NTVIS__)
void 
sigpipe()
{;
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int
main(int argc, char **argv)
{
  unsigned long addr_tmp;
  int c,k;
  int sockbufsndsize,sockbufrcvsize;
  int sockbuflen;
  struct sockaddr_in peer;
  int peerlen = sizeof(peer);

#if defined(__NTVIS__)
  extern char *optarg;
  WSADATA WSAData;
  WSAStartup(MAKEWORD(1,1), &WSAData);
#endif /* __NTVIS__ */

  if (argc < 2) { do_Usage(); exit(1); }

  while (1) {
    while ((c=getopt(argc, argv, "hidrstuvxHVBDTLb:f:l:n:p:A:O:S:N:P:R:I:w:")) != -1) {
      switch (c) {
        case 'w':
           timeout = atoi(optarg);
           break;
        case 'I':
           seed = atoi(optarg);
           break;
        case 'N':
           burstn = atoi(optarg);
           break;
	case 'P':
#if defined(__linux__)
	   pri = atoi(optarg);
	   break;
#else
	   fprintf(stderr, "ttcp: -P option not supported on this OS\n");
	   exit(1);
#endif
	case 'R':
	   bufmin = atoi(optarg);
	   break;
        case 'S':
           udpsleep = atoi(optarg);
           if (udpsleep < 0) udpsleep = 0;
           /* convert msec arg to usec for usleep, minimum 10ms */
           udpsleep = udpsleep * 1000;
           break;
        case 'H':
           hash = 1;
           break;
        case 'V':
           fprintf(stdout,"%s %s\n" , RCSid , VersDate );
           exit(0);
        case 'B':
          b_flag = 1;
          break;
        case 'L':
          log_cnt = 1;
          break;
        case 'h':
          do_help();
          exit(1);
          break;
        case 't':
          trans = 1;
          break;
        case 'r':
          trans = 0;
          break;
        case 'x':
          rndm = 1;
          break;
        case 'd':
          options |= SO_DEBUG;
          break;
        case 'D':
#ifdef TCP_NODELAY
        nodelay = 1;
#else
        fprintf(stderr, 
       "ttcp: -D option ignored: TCP_NODELAY socket option not supported\n");
#endif
          break;
        case 'n':
          nbuf = atoi(optarg);
          break;
        case 'l':
          buflen = atoi(optarg);
          break;
        case 's':
          sinkmode = !sinkmode;
          break;
        case 'p':
          port = atoi(optarg);
          break;
        case 'u':
          udp = 1;
          break;
        case 'v':
          verbose = 1;
          break;
        case 'A':
          bufalign = atoi(optarg);
          break;
        case 'O':
          bufoffset = atoi(optarg);
          break;
        case 'b':
#if defined(SO_SNDBUF) || defined(SO_RCVBUF)
          sockbufsize = atoi(optarg);
#else
          fprintf(stderr, 
"ttcp: -b option ignored: SO_SNDBUF/SO_RCVBUF socket options not supported\n");
#endif
          break;
        case 'f':
          fmt = *optarg;
          break;
        case 'T':
          touchdata = 1;
          break;
        case 'i':
          seq_info = 1;
          break;
          
      default:
          {do_Usage(); exit(1);}
       }/*switch */
    }/* while getopt() */

    argc -= optind;
    argv += optind;
    optind = 0;
    
    /* check for '--' args */
    if (argc == 0) {
      /* no more args */
      break; /* while (1) */
#if defined(DEBUG)
    } else if (!strcmp(argv[0], "--nostart")) {
      no_start = 1;
      argc--; argv++;
    } else if (!strcmp(argv[0], "--nodata")) {
      no_data = 1;
      argc--; argv++;
    } else if (!strcmp(argv[0], "--noend")) {
      no_end = 1;
      argc--; argv++;
    } else if (!strcmp(argv[0], "--debug")) {
      debug = 1;
      argc--; argv++;
#endif /* DEBUG */
    } else if (!strncmp(argv[0], "--", 2)) {
      fprintf(stderr, "ttcp: illegal option: %s\n", argv[0]);
      do_Usage(); exit(1);
    } else {
      /* the arg was not a '--' arg */
      break; /* while (1) */
    }
  } /* while (1) */
  
    
 /* ----------------------- main part ----------------------- */

#if defined(__NTVIS__) && !defined(UNDER_CE)
  /* Set "stdin" to have binary mode: */
  if (_setmode(_fileno(stdin), _O_BINARY) == -1)
    perror("%s: Error setting stdin to binary mode");
  /* Set "stdout" to have binary mode: */
  if (_setmode(_fileno(stdout), _O_BINARY) == -1)
    perror("%s: Error setting stdout to binary mode");
#endif /* __NTVIS__ */

if (log_cnt) open_log(); 

/* input error checking */
if (burstn > nbuf) {
    fprintf(stderr, "ttcp: buffers per burst must be less than or equal to "
        "total number of buffers\n");
    exit(1);
}
if (bufmin < -1) {
    fprintf(stderr, "ttcp: minimum buffer size specified with -R option must "
        "be non-negative\n");
    exit(1);
}
if (buflen > 65535) {
    fprintf(stderr, "ttcp: maximum buffer size specified with -l option must "
        "be <= 65536\n");
    exit(1);
}
if (bufmin > buflen) {
    fprintf(stderr, "ttcp: minimum buffer size specified with -R option must "
        "be less than or equal to (maximum) buffer size specified with -l\n");
    exit(1);
}

/* buffer allocation */

 if (udp && buflen < MIN_UDP_LEN) 
   buflen = MIN_UDP_LEN;        /* send more than the sentinel size */
 
 if ( (buf = (char *)malloc(buflen+bufalign)) == (char *)NULL) 
      err("malloc");
 if (bufalign != 0)
      buf +=(bufalign - ((int)buf % bufalign) + bufoffset) % bufalign;
 
 fprintf(stderr,"ttcp%s: buflen=%d, nbuf=%d, align=%d/%d, port=%d\n",
            trans?"-t":"-r",buflen, nbuf, bufalign, bufoffset, port);
 if (log_cnt)fprintf(fplog," %6d %6d %6d %6d %4d",
                      buflen, nbuf, bufalign, bufoffset, port);

 /* preload the buffer for the transmit condition */
 pattern( buf, buflen );

 /* seed the random number generator */
 if ((bufmin != -1) || (rndm != 0)) {
#if defined(__NTVIS__)
   srand(seed);
#else /* ! __NTVIS__ */
   srandom(seed);
#endif /* __NTVIS__ */
 }

 if(trans)  
   {/* xmitr */
     if (argc == 0) {
       fprintf(stderr, "ttcp: missing destination host arg\n");
       do_Usage(); exit(1);
     }
     
     bzero((char *)&sinhim, sizeof(sinhim));
     host = argv[0];
     if (atoi(host) > 0 )  
       {/* Numeric */
         sinhim.sin_family = AF_INET;
         sinhim.sin_addr.s_addr = inet_addr(host);
       } 
     else 
       {if ((addr=gethostbyname(host)) == NULL) sockets_err("bad hostname");
        sinhim.sin_family = addr->h_addrtype;
        bcopy(addr->h_addr,(char*)&addr_tmp, addr->h_length);
        sinhim.sin_addr.s_addr = addr_tmp;
       }
     sinhim.sin_port = htons(port);
     sinme.sin_port = 0;                /* free choice */
   } 
 else 
   {/* rcvr */
     sinme.sin_port =  htons(port);
   }
#if defined(__NTVIS__)
 sinme.sin_family = AF_INET;
#endif
 
 fd = socket(AF_INET, udp?SOCK_DGRAM:SOCK_STREAM, 0);

#if defined(__NTVIS__)
 if (fd == INVALID_SOCKET) sockets_err("socket");
#else
 if (fd < 0) sockets_err("socket");
#endif
 
 if (verbose) {
	char *label = trans?"ttcp-t":"ttcp-r";
#if defined(UNDER_CE)
	SYSTEMTIME SystemTime;
	char time_str[30];

	GetLocalTime(&SystemTime);
	sprintf(time_str, "%s %s %d %02d:%02d:%02d %d\n", weekday[SystemTime.wDayOfWeek], month[SystemTime.wMonth - 1],
		SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wYear);
#else
   time_t now;
   char *time_str; 
       
   time(&now);
   time_str = ctime(&now);
#endif
   fprintf(stdout,"%s: start time %s", label, time_str);
   fprintf(stdout,"%s: File-Descriptor 0x%x Opened\n", label, fd );
 }

#if defined(__NTVIS__)
 if (bind(fd, (struct sockaddr FAR *)&sinme, sizeof(sinme)) == SOCKET_ERROR) 
   sockets_err("bind");
#else
 if (bind(fd, SOCKADDR_CAST &sinme, sizeof(sinme)) < 0) 
   sockets_err("bind");
#endif /* __NTVIS__ */


#if defined(SO_SNDBUF) || defined(SO_RCVBUF)
 if (sockbufsize) 
   {
#if defined(__lynx__) || defined(__sun__)  || defined(__NTVIS__)
     if (trans) 
       {if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sockbufsize,
                       sizeof sockbufsize) < 0)  
        sockets_err("setsockopt: sndbuf");
       } 
     else 
       {if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&sockbufsize,
                       sizeof sockbufsize) < 0)  
        sockets_err("setsockopt: rcvbuf");
       }

#else
     if (trans) 

         {if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sockbufsize,
                       sizeof sockbufsize) < 0) 
        sockets_err("setsockopt: sndbuf");
       } 
     else 
       {if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sockbufsize,
                       sizeof sockbufsize) < 0) 
        sockets_err("setsockopt: rcvbuf");
       }
#endif /* __lynx__ __sun__ __NTVIS__ */ 
   }
 else
   {/*
    ** Added by Daniel Davids to Know Socket-Buffer-Sizes
    */
     sockbuflen = sizeof sockbufsndsize;
#if defined(__lynx__) || defined(__sun__) || defined(__NTVIS__)
     getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&sockbufsndsize, 
                &sockbuflen);
     sockbuflen = sizeof sockbufrcvsize;
     getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&sockbufrcvsize, 
                     &sockbuflen);
#else
     getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sockbufsndsize, &sockbuflen);
     sockbuflen = sizeof sockbufrcvsize;
     getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sockbufrcvsize, &sockbuflen);
#endif /* __lynx__ __sun__ __NTVIS__ */
     sockbufsize = ( sockbufsndsize + sockbufrcvsize ) / 2;

     if ( sockbufsndsize != sockbufrcvsize )
       {fprintf(stderr, "sockbufsndsize=%d, ", sockbufsndsize );
       fprintf(stderr, "sockbufrcvsize=%d, ", sockbufrcvsize );
       }
   }
#endif /* defined(SO_SNDBUF) || defined(SO_RCVBUF) */
 
 if (sockbufsize) fprintf(stderr, "sockbufsize=%d, \n", sockbufsize);

 if (log_cnt)
   {if (sockbufsize)fprintf(fplog," %6d",sockbufsize);
    else fprintf(fplog," 0");
   }

#if defined(__linux__)
 if (setsockopt(fd, SOL_SOCKET, SO_PRIORITY, (char *)&pri, sizeof pri) < 0)  
   sockets_err("setsockopt: priority");
#endif


 if (trans)  fprintf(stderr, "# %s sender -> %s #\n", udp?"udp":"tcp", host);
 else        fprintf(stderr, "# %s receiver #\n", udp?"udp":"tcp");
 
 if (!udp)  
   {
#if !defined(__NTVIS__)
     signal(SIGPIPE, sigpipe);
#endif /* !__NTVIS__ */

     if (trans) 
       {/* We are the client if transmitting */
         if (options)  
           {
#if defined(BSD42)
            if( setsockopt(fd, SOL_SOCKET, options, 0, 0) < 0)
#else /* BSD43 */
       if( setsockopt(fd, SOL_SOCKET, options, (char *)&one, sizeof(one)) < 0)
#endif /* BDS42 */
           sockets_err("setsockopt");
           }
#ifdef TCP_NODELAY
         if (nodelay) 
           {struct protoent *p;
            p = getprotobyname("tcp");
#if defined(__lynx__)  || defined(__sun__)        || defined(__NTVIS__)
                if( p && setsockopt(fd, p->p_proto, TCP_NODELAY, 
                                      (char *) &one, sizeof(one)) < 0)
#else 
            if( p && setsockopt(fd, p->p_proto, TCP_NODELAY, 
                                      &one, sizeof(one)) < 0)
#endif /* __lynx__  __sun__ __NTVIS__ */
            sockets_err("setsockopt: nodelay");
           }
#endif /* TCP_NODELAY */
         if (connect(fd, SOCKADDR_CAST &sinhim, sizeof(sinhim) ) < 0) 
         sockets_err("connect");
       }/* if (trans) */ 
     else 
       {/* otherwise, we are the server and should listen for connections */

errno = 0;

#if defined(sgi) || ( defined(__osf__) && !defined(_CFE) )
        if( listen(fd,1) <0 )   
#else
        if( listen(fd,0) <0 )   /* allow a queue of 0 */
#endif
        sockets_err("listen");

        if(options) {
          int err;
#if defined(BSD42)
          err = setsockopt(fd, SOL_SOCKET, options, 0, 0);
#elif defined(__lynx__) || defined(__sun__) || defined(__NTVIS__)
          err = setsockopt(fd, SOL_SOCKET, options, (char *) &one, sizeof(one));
#else
          err = setsockopt(fd, SOL_SOCKET, options, &one, sizeof(one));
#endif /* BSD42 */
          if (err < 0)
            sockets_err("setsockopt");
        }
        fromlen = sizeof(frominet);
        domain = AF_INET;
 if (timeout > 0) {
   fd_set readfds, exceptfds;
   struct timeval tv_timeout;
   int n;
   
   tv_timeout.tv_sec = timeout/1000;
   tv_timeout.tv_usec = (timeout%1000)*1000;
   
   FD_ZERO(&readfds);
   FD_ZERO(&exceptfds);
   FD_SET(fd, &readfds);
   FD_SET(fd, &exceptfds);
   
   n =  select( fd+1, &readfds, NULL, &exceptfds, &tv_timeout );
   if (n == 0 || n == SOCKET_ERROR) {
     sockets_err("select listen");
     return 0;
   } 
 }
        if((fd=accept(fd, SOCKADDR_CAST &frominet, &fromlen) ) < 0) 
                sockets_err("accept");

        if (getpeername(fd, SOCKADDR_CAST &peer, &peerlen) < 0)
          sockets_err("getpeername");
         
        fprintf(stderr,"ttcp-r: accept from %s\n",inet_ntoa(peer.sin_addr));

       } /* otherwise we are ... */
   }

 prep_timer();
 errno = 0;
 nbytes = 0.0;
 numCalls = 0;

 {
   register int cnt,multi;
   char *pb;
   int nb = 0;
   unsigned long rbuflen = 0;
   multi = nbuf;

   if (trans) {
#if !defined(BSD43)
     if(udp) {
       int err;
       int nochecksum_opt = 0;
       err = setsockopt(fd, IPPROTO_UDP, UDP_NOCHECKSUM,
                        (char *)&nochecksum_opt, sizeof(nochecksum_opt));
       if (err < 0)
         sockets_err("setsockopt");
     }
#endif
     
     if(udp && !no_start) {
       int start_count = 2;
       char start_pkt[START_PKT_LEN];
       memset(start_pkt, 0, START_PKT_LEN);
       memcpy(start_pkt, start_pattern, START_PATTERN_LEN);

       while (start_count-- > 0) {
         (void)Nwrite( fd, start_pkt, START_PKT_LEN ); /* rcvr start */
         if (udpsleep) {
           usleep(udpsleep);
           /* clear errno (see man page for errno(3), definition of
              EINTR.  usleep() uses SIGCONT? ) */
           if (errno == EINTR) errno = 0;
         } /* udpsleep */
       }
     }

     /* initial seqence num for UDP */
     if (udp) buf[0] = 0;
     k = burstn;

     if (sinkmode) {
       while (multi-- > 0 && !no_data) {

         if (bufmin == -1) {
           rbuflen = buflen;
         } else {
           /* set rbuflen to a random value evenly distributed in 
              [bufmin, buflen].  As long as buflen is < 2^16, we can 
              fit the calculation in 32 bits */
#if defined(__NTVIS__)
           rbuflen = (( (unsigned long)rand() *
                        ((unsigned long)(buflen-bufmin+1)) ) >> 15) + bufmin;
#else /* ! __NTVIS__ */
           rbuflen = (( ((unsigned long)random() >> 15) *
                        ((unsigned long)(buflen-bufmin+1)) ) >> 16) + bufmin;
#endif /* __NTVIS__ */
         }

         if (rndm) {
           unsigned int i, j, l;
           unsigned long data;
           
           if (udp)
             l = 1;
           else
             l = 0;

           for (i = l; i < rbuflen; /* null */) {
#if defined(__NTVIS__)
             data = (unsigned long)rand();
#else /* ! __NTVIS__ */
             data = (unsigned long)random();
#endif /* __NTVIS__ */
             /*fprintf(stderr, "%08x\n",data);*/ /* DEBUG */
             /* The NT rand() function returns only 16 useful bits! */
             for (j = 0; (j < 2/*4*/) && (i < rbuflen) ; j++) {
               buf[i++] = (unsigned char)(data & 0xff);
               data >>= 8;
             }
           }
         }
         
         if ((cnt=Nwrite(fd,buf,rbuflen)) != (int)rbuflen)
         sockets_err("error from the socket write");
         /* increment sequence num if UDP */
         if (udp) buf[0] = (char)(((unsigned char)(buf[0])+1) & 0xFF);
         
         if (debug)
           fprintf(stdout,"ttcp-t: %5d | %d Bytes Written in %d write commands \n",
                   ++nb, cnt, nbuf );

         nbytes += rbuflen;

         /* hashes */
         if (hash) {
           fprintf(stderr,"#");
         }

         /* Check for the end of a burst */
         if (--k <= 0) {
           k = burstn;
           
           /* sleep to avoid overrunning slower receivers - henry */
           if (udpsleep) {
             usleep(udpsleep);
             /* clear errno (see man page for errno(3), definition of
              EINTR.  usleep() uses SIGCONT? ) */
             if (errno == EINTR) errno = 0;
           } /* udpsleep */
         }
       } /* while */

     } /* if (sinkmode) */
     else 
     {

       nbuf = 0;

       if (bufmin == -1) {
         rbuflen = buflen;
       } else {
         /* set rbuflen to a random value evenly distributed in 
            [bufmin, buflen].  As long as buflen is < 2^16, we can 
            fit the calculation in 32 bits */
#if defined(__NTVIS__)
         rbuflen = (( (unsigned long)rand() *
                      ((unsigned long)(buflen-bufmin+1)) ) >> 15) + bufmin;
#else /* ! __NTVIS__ */
         rbuflen = (( ((unsigned long)random() >> 15) *
                      ((unsigned long)(buflen-bufmin+1)) ) >> 16) + bufmin;
#endif /* __NTVIS__ */
       }

       if (udp)
         pb = &(buf[1]);
       else
         pb = &(buf[0]);

#if !defined(__NTVIS__)     /* else case specific to WINNT */
       while((cnt=read(0,pb,rbuflen)) > 0)
#else /* __NTVIS__ */
       while((cnt=(int)fread(pb,1,rbuflen,stdin)) > 0)
#endif /* __NTVIS__ */
        {

         if (udp)
           multi = cnt+1; /* don't include seq. num. in count of data */
         else
           multi = cnt;
         if ((cnt=Nwrite(fd,buf,multi)) != (int)multi)
           sockets_err("error from the socket write");
         /* increment seqence num if UDP */
         if (udp) buf[0] = (char)(((unsigned char)(buf[0])+1) & 0xFF);
         nbuf++;

         if (debug)
           fprintf(stdout,"ttcp-t: %5d | %d Bytes Written in %d write commands \n",
                   ++nb, cnt, nbuf );

         nbytes += multi;

         /* hashes */
         if (hash) {
           fprintf(stderr,"#");
         }

         /* Check for the end of a burst */
         if (--k <= 0) {
           k = burstn;
           
           /* sleep to avoid overrunning slower receivers - henry */
           if (udpsleep) {
             usleep(udpsleep);
             /* clear errno (see man page for errno(3), definition of
              EINTR.  usleep() uses SIGCONT? ) */
             if (errno == EINTR) errno = 0;
           } /* udpsleep */
         }

         if (bufmin == -1) {
           rbuflen = buflen;
         } else {
           /* set rbuflen to a random value evenly distributed in 
              [bufmin, buflen].  As long as buflen is < 2^16, we can 
              fit the calculation in 32 bits */
#if defined(__NTVIS__)
           rbuflen = (( (unsigned long)rand() *
                        ((unsigned long)(buflen-bufmin+1)) ) >> 15) + bufmin;
#else /* ! __NTVIS__ */
           rbuflen = (( ((unsigned long)random() >> 15) *
                        ((unsigned long)(buflen-bufmin+1)) ) >> 16) + bufmin;
#endif /* __NTVIS__ */
         }

       } /* while */

     } /* if (sinkmode) */
     
     if (hash) {
       fprintf(stderr,"\n");
       fflush(stderr);
     }
     
#if defined(UNDER_CE)
	 errno = WSAGetLastError();
#endif
     if (!errno) {
       read_timer();
       if(udp && !no_end) {
         int end_count = 4;
         int data_count = nbuf;
         char end_pkt[END_PKT_LEN];
         unsigned char* net_byte = &end_pkt[END_PATTERN_LEN];
         memset(end_pkt, 0, END_PKT_LEN);
         memcpy(end_pkt, end_pattern, END_PATTERN_LEN);
         net_byte[3] = data_count & 0xFF; data_count >>= 8;
         net_byte[2] = data_count & 0xFF; data_count >>= 8;
         net_byte[1] = data_count & 0xFF; data_count >>= 8;
         net_byte[0] = data_count & 0xFF;
         
         while (end_count-- > 0) {
           Nwrite( fd, end_pkt, END_PKT_LEN ); /* rcvr end */
           if (udpsleep && end_count>0) usleep(udpsleep);
         }
       }
       result_summary();
     }
   } /* trans */ 
   else 
   {
       if (udp) {
         udp_rcv_test();
       }
       else 
       {/* not udp received transfer */
         while ((cnt=Nread(fd,buf,buflen)) > 0)
           {if(debug)fprintf(stdout,
                            "ttcp%s: %5d | %d Bytes Read\n",
                            trans?"-t":"-r", ++nb, cnt );
           nbytes += cnt;
           if (!sinkmode) {
#if !defined(__NTVIS__)     /* else case specific to WINNT */
             if (write(1,buf,cnt) != cnt) err("write");
#else /* __NTVIS__ */
             if ((int)fwrite(buf,1,cnt,stdout) != cnt) err("fwrite");
#endif /* __NTVIS__ */
           }
         }
#if defined(UNDER_CE)
		 errno = WSAGetLastError();
#endif
         if (!errno) {
           read_timer();
           result_summary();
         }
       }
     }
   }

 if(errno) err("IO");

#if defined(__NTVIS__) 
  closesocket ( fd );
#else 
  close ( fd );
#endif /* __NTVIS__ */

  if (verbose) fprintf(stdout,"ttcp%s: File-Descriptor  fd 0x%x Closed\n" ,
                       trans?"-t":"-r", fd );

  if (log_cnt) close_log();

  fprintf(stderr,"ttcp done.\n");
  fflush(stdout);

  exit(0);
}
/*--------------------------------------------------------------------------*/
void
udp_rcv_test(void)
{
  enum {START_WAIT, DATA_WAIT, DATA_RCV, END_TEST} rcv_state;
  enum {START_PKT_TYPE, END_PKT_TYPE, DATA_PKT_TYPE} pkt_type;
  int cnt;
  int nbuffers = 0;
  unsigned int start_pkt_cnt = 0;
  unsigned int end_pkt_cnt = 0;
  unsigned int data_pkt_cnt = 0;
  unsigned int expected_pkt_cnt = 0;
  unsigned char seq = 0;
  unsigned char buf_seq;
  
  rcv_state = START_WAIT;
  while (rcv_state != END_TEST && (cnt=Nread(fd,buf,buflen)) > 0) {
    /* Determine the type of packet received */
    if (!memcmp(buf, start_pattern, START_PATTERN_LEN)) {
      pkt_type = START_PKT_TYPE;
      start_pkt_cnt++;
    } else if (!memcmp(buf, end_pattern, END_PATTERN_LEN)) {
      /* read the expected data packet count from the end packet */
      unsigned char* net_byte = buf;
      net_byte += END_PATTERN_LEN;
      expected_pkt_cnt = ((net_byte[0] << 24) + (net_byte[1] << 16) +
                          (net_byte[2] << 8) + net_byte[3]);
      pkt_type = END_PKT_TYPE;
      end_pkt_cnt++;
    } else {
      data_pkt_cnt++;
      pkt_type = DATA_PKT_TYPE;
    }
           
    if (rcv_state == START_WAIT) {
      /* Wait until we see a vaild start packet */
      if (pkt_type == START_PKT_TYPE) {
        prep_timer();
        rcv_state = DATA_WAIT;
      } else if (pkt_type == DATA_PKT_TYPE) {
        fprintf(stderr, "ERROR: Missed UDP start packet.\n");
        prep_timer();
        rcv_state = DATA_RCV;
      } else if (pkt_type == END_PKT_TYPE) {
        fprintf(stderr, "ERROR: Found UDP end packet before start packet or data.\n");
        rcv_state = END_TEST;
      }
    } else if (rcv_state == DATA_WAIT) {
      /* Skip any extra start packets until we see data */
      if (pkt_type == START_PKT_TYPE) {
        prep_timer();
      } else if (pkt_type == DATA_PKT_TYPE) {
        rcv_state = DATA_RCV;
      } else if (pkt_type == END_PKT_TYPE) {
        fprintf(stderr, "ERROR: Found UDP end packet without receiving "
                "any data packets.\n");
        rcv_state = END_TEST;
      }
    } else { /* DATA_RCV */
      /* Collect data packets until we see a vaild end packet */
      if (pkt_type == START_PKT_TYPE) {
        /* We missed the end packets and now a new test is
         * starting. Report the results of this test then start
         * another. */
        read_timer();
        result_summary();
        fprintf(stderr, "ERROR: Found UDP start packet while receiving data.\n"
                "ERROR: Expected more data packets or an end packet.\n");
        nbytes = 0.0;
        nbuffers = 0;
        numCalls = 0;
        prep_timer();
        rcv_state = DATA_WAIT;
      } else if (pkt_type == DATA_PKT_TYPE) {
        /* loop in this state */
      } else if (pkt_type == END_PKT_TYPE) {
        /* we used to print results here but now we do when the loop ends */
        rcv_state = END_TEST;
      }
    }
    
    /* tally data packets the same way from whatever state we are in */
    if (pkt_type == DATA_PKT_TYPE) {
      if (debug)
        fprintf(stderr, "ttcp-r: %5d | %d Bytes Read\n", ++nbuffers, cnt);
      nbytes += cnt;
      if (seq_info) {
        /* check seqence num */
        buf_seq = (unsigned char)(buf[0]);
        if (buf_seq != seq) {
          fprintf(stderr, "ERROR: Out of sequence. "
                  "Buffer %u, seq %u, expected %u\n",
                  data_pkt_cnt, buf_seq, seq);
          seq = buf_seq;
        }
        seq++;
      }
      if (!sinkmode) {
#if !defined(__NTVIS__)     /* else case specific to WINNT */
        if (write(1,&(buf[1]),cnt-1) != cnt-1) err("write");
#else /* __NTVIS__ */
        if ((int)fwrite(&(buf[1]),1,cnt-1,stdout) != cnt-1) err("fwrite");
#endif /* __NTVIS__ */
      }
    }
  } /* end-while */

  /* normal end of test */
  read_timer();
  result_summary();
  fprintf(stderr, "ttcp-r: packets %u data, %u start, %u end\n",
          data_pkt_cnt, start_pkt_cnt, end_pkt_cnt);
  if (expected_pkt_cnt != 0) {
    int lost_cnt = expected_pkt_cnt - data_pkt_cnt;

    fprintf(stderr, "ttcp-r: %g%% (%u/%u) packet loss\n",
            100.0 * (double)lost_cnt/(double)expected_pkt_cnt,
            lost_cnt, expected_pkt_cnt);
  }
}

/*--------------------------------------------------------------------------*/
/* calc_cpu_time(): Time difference, in usecs, of the combined user and
 * sys times of the given start and end usage info */
double
calc_cpu_time(
  struct rusage *r0,
  struct rusage *r1
)
{
  double time;
  time_t ms;
  struct timeval tdiff, tend, tstart;
    
  /* total user delta time + total system delta time */
  tvadd( &tend, &r1->ru_utime, &r1->ru_stime );   /* user + sys time  @ end */
  tvadd( &tstart, &r0->ru_utime, &r0->ru_stime ); /* user + sys time  @ start*/
  tvsub( &tdiff, &tend, &tstart );

  /* useconds */
  time = ((double)tdiff.tv_sec)*1e6 + (double)tdiff.tv_usec;

  /* debug mseconds */
  ms = tdiff.tv_sec*(time_t)1000 + tdiff.tv_usec/(time_t)1000;
  if (ms != (time_t)(time/1000.0)) {
    fprintf(stderr, "ERROR: time calc mismatch of msec (%d) to usec (%f)\n",
            (int)ms, time/1000.0);
  }

  return time;
}

/*--------------------------------------------------------------------------*/
/* calc_real_time(): Time difference in usecs of the given times */
double
calc_real_time(
  struct timeval *t0,
  struct timeval *t1
)
{
  double time;
  time_t ms;
  struct timeval tdiff;
    
  tvsub(&tdiff, t1 ,t0);

  /* useconds */
  time = ((double)tdiff.tv_sec)*1e6 + (double)tdiff.tv_usec;
  
  /* debug mseconds */
  ms = tdiff.tv_sec*(time_t)1000 + tdiff.tv_usec/(time_t)1000;
  if (ms != (time_t)(time/1000.0)) {
    fprintf(stderr, "ERROR: time calc mismatch of msec (%d) to usec (%f)\n",
            (int)ms, time/1000.0);
  }

  return time;
}
/*--------------------------------------------------------------------------*/
void
result_summary(void)
{
  char* label = trans ? "ttcp-t" : "ttcp-r";
  double cput = calc_cpu_time(&ru0, &ru1);
  double realt = calc_real_time(&time0, &time1);
  double t_min;
  double bytes_per_sec;

  /* lower end boundary conditions */
  t_min = 10.0;                 /* 10 usec */
#if  defined(__NTVIS__)
  t_min = 1000.0;               /* 1 msec */
#endif
  if (cput <= t_min) {          /* value in usec */
    cput = t_min;
    fprintf(stderr,"%s: cpu time too short set at %.0f usec, NOT accurate result.\n",
            label,t_min);
  }
  if (realt <= t_min) {         /* value in usec */
    realt = t_min;
    fprintf(stderr,"%s: real time too short, set at %.0f usec, NOT accurate result.\n",
            label,t_min);
  }

  bytes_per_sec = (nbytes/realt)*1e6;

  fprintf(stderr,"%s: %.0f bytes in %.06f real seconds = %s/sec +++\n",
          label, nbytes, realt/1e6, outfmt(fmt, bytes_per_sec));

  if (verbose)
    fprintf(stderr,"%s: %.0f bytes in %.06f cpu  seconds = %s/cpu sec\n",
            label, nbytes,cput/1e6, outfmt(fmt, (nbytes/cput)*1e6));

  fprintf(stderr,"%s: %ld I/O calls, %.3f msec(real)/call, %.3f msec(cpu)/call\n",
          label, numCalls,
          (numCalls>0)?(realt/(double)numCalls)/1000.0:0.0,
          (numCalls>0)?(cput /(double)numCalls)/1000.0:0.0);

  fprintf(stderr,"%s: ", label);
  prusage(&ru0, &ru1, &time0, &time1);
  fprintf(stderr,"\n");

  if (verbose)
    printf("%s: buffer address %#x\n", label, (unsigned int)buf);

  if (fplog) {
    struct timeval tdiff;
    /* User time */
    tvsub(&tdiff, &ru1.ru_utime, &ru0.ru_utime);
    fprintf(fplog," %ld.%06ld", (long)tdiff.tv_sec, (long)tdiff.tv_usec);
    /* System time */
    tvsub(&tdiff, &ru1.ru_stime, &ru0.ru_stime);
    fprintf(fplog," %ld.%06ld", (long)tdiff.tv_sec, (long)tdiff.tv_usec);
    /* Percent of cput usage */
    fprintf(fplog," %.1f", 100.0 * (cput/realt));
    /* validity of real time mesurment */
    fprintf(fplog, (realt == t_min)?" 0":" 1");
    /* bytes, seconds, MBytes/sec, IO calls */
    fprintf(fplog," %10.0f %4.06f %4.3f %6ld",
            nbytes, realt/1e6, bytes_per_sec/(1024.0*1024.0), numCalls);
  }
}
/*--------------------------------------------------------------------------*/
void
sockets_err(char* s)
{
#if defined(__NTVIS__)
  int err = WSAGetLastError();
  char* prefix = trans?"ttcp-t":"ttcp-r";
  fprintf(stderr,"%s: %s\n", prefix, s);
  fprintf(stderr,"%s: errno=%d\n", prefix, err);
  exit(1);
#else
  err(s);
#endif /* __NTVIS__ */
}

/*--------------------------------------------------------------------------*/
void
err(char *s)
{
  char* prefix = trans?"ttcp-t":"ttcp-r";
  fprintf(stderr,"%s: ", prefix);
  perror(s);
  fprintf(stderr,"%s: errno=%d\n", prefix, errno);
  exit(1);
}
/*--------------------------------------------------------------------------*/
void
mes(char *s)
{
        fprintf(stderr,"ttcp%s: %s\n", trans?"-t":"-r", s);
}
/*--------------------------------------------------------------------------*/
void
pattern( char *cp, int cnt )
{
        register char c;
        register int cnt1;
        cnt1 = cnt;
        c = 0;
        while( cnt1-- > 0 )  {
                while( !isprint((c&0x7F)) )  c++;
                *cp++ = (c++&0x7F);
        }
}
/*--------------------------------------------------------------------------*/
char *
outfmt(char format, double b)
{
  static char obuf[50];
  double giga = 1024.0 * 1024.0 * 1024.0;
  double mega = 1024.0 * 1024.0;
  double kilo = 1024.0;
  
  if (format == 'A') {
    if (b >= giga)
      format = 'G';
    else if (b >= mega)
      format = 'M';
    else if (b >= kilo)
      format = 'K';
    else
      format = 'B';
  }

  switch (format) {
  case 'G':
    sprintf(obuf, "%.3f GB", b / giga);
    break;
  case 'M':
    sprintf(obuf, "%.3f MB", b / mega);
    break;
  case 'K':
    sprintf(obuf, "%.3f KB", b / kilo);
    break;
  case 'B':
    sprintf(obuf, "%4f B", b);
    break;
  case 'g':
    sprintf(obuf, "%.3f Gbit", b * 8.0 / giga);
    break;
  case 'm':
    sprintf(obuf, "%.3f Mbit", b * 8.0 / mega);
    break;
  case 'k':
    sprintf(obuf, "%.3f Kbit", b * 8.0 / kilo);
    break;
  case 'b':
    sprintf(obuf, "%4f b", b * 8.0);
    break;
  default:
    sprintf(obuf, "default.........."); 
  }
  return obuf;
}
/*--------------------------------------------------------------------------*/
#if defined(SYSV)

/*ARGSUSED*/

#if defined(__osf__)
/* getrusage defined in the system lib */
#else 
#if defined(__lynx__)
/* getrusage defined in the system lib */
#else 
#if defined(__sun__) 
/* getrusage defined in the system lib */
#else 

int
getrusage(ignored, ru)
     int ignored;
     register struct rusage *ru;
{


#if defined(__NTVIS__)
  HANDLE phd;
  FILETIME CreateTime, ExitTime, KernelTime, UserTime;
  SYSTEMTIME SysTime;
#if defined(UNDER_CE)
  phd = GetCurrentThread();
  if( GetThreadTimes(phd, &CreateTime, &ExitTime, &KernelTime, &UserTime) 
      != TRUE) 
#else
  phd = GetCurrentProcess();
  if( GetProcessTimes(phd, &CreateTime, &ExitTime, &KernelTime, &UserTime) 
      != TRUE) 
#endif
    {ru->ru_stime.tv_sec  = 0;
    ru->ru_stime.tv_usec = 0;
    ru->ru_utime.tv_sec  = 0;
    ru->ru_utime.tv_usec = 0;
    } 
  else 
   {
     (void) FileTimeToSystemTime(&KernelTime, &SysTime);
     /*
      * fprintf(stdout,
      * "System sec=%d, msec=%d\n", SysTime.wSecond, SysTime.wMilliseconds);
      */
     ru->ru_stime.tv_sec  = SysTime.wSecond; 
     ru->ru_stime.tv_usec = SysTime.wMilliseconds * 1000;
     (void) FileTimeToSystemTime(&UserTime, &SysTime);
     /*
      *  fprintf(stdout,
      *  "   User sec=%d, msec=%d\n", SysTime.wSecond, SysTime.wMilliseconds);
      */
     ru->ru_utime.tv_sec  = SysTime.wSecond;
     ru->ru_utime.tv_usec = SysTime.wMilliseconds * 1000;
    }

#else   /* __NTVIS__ */

  struct tms buftime;
  times(&buftime);
  /* Assumption: HZ <= 2147 (LONG_MAX/1000000) */
  /* info : in lynxOS HZ is called TICKSPERSEC (<conf.h>) */

    ru->ru_stime.tv_sec  = buftime.tms_stime / HZ;
    ru->ru_stime.tv_usec = ((buftime.tms_stime % HZ) * 1000000) / HZ;
    ru->ru_utime.tv_sec  = buftime.tms_utime / HZ;
    ru->ru_utime.tv_usec = ((buftime.tms_utime % HZ) * 1000000) / HZ;
#endif /* __NTVIS__ */
return(0);

} /* static getrusage */

#endif /* __sun__ */
#endif /* __lynx__ */
#endif /* __osf__ */
#endif /* SYSV */
/*--------------------------------------------------------------------------*/
#if defined(SYSV) 
#if defined(__hpux__) || defined(_AIX) || defined(__sun__)
/* gettimeofday defined in the system lib */
#else
# if defined (__osf__) ||defined (__lynx__)
/* gettimeofday defined in the system lib */
#else
/*ARGSUSED*/
static 
gettimeofday(tp, zp)
struct timeval *tp;
struct timezone *zp;
{
#if defined(__NTVIS__)
#if defined(UNDER_CE)
	SYSTEMTIME SystemTime;

	GetLocalTime(&SystemTime); 
	tp->tv_sec = SystemTime.wSecond;
	tp->tv_usec = SystemTime.wMilliseconds * 1000;
#else
  struct _timeb timeptr;

  _ftime(&timeptr);
  tp->tv_sec = timeptr.time;
  tp->tv_usec = timeptr.millitm * 1000;
#endif
#else /* all cases */
  tp->tv_sec = time(0);
  tp->tv_usec = 0;
#endif /* __NTVIS__ */
return(1);
} /* static gettimeofday */

#endif /*__osf__ || __lynx__ */
#endif /* __hpux__ || _AIX || __sun__ || __osf__*/
#endif /* SYSV */
/*--------------------------------------------------------------------------*/
/*
 *                        P R E P _ T I M E R
 */
void
prep_timer(void)
{
  gettimeofday(&time0, (struct timezone *)0);
  getrusage(RUSAGE_SELF, &ru0);
}
/*--------------------------------------------------------------------------*/
/*
 *                        R E A D _ T I M E R
 * 
 */
void
read_timer(void)
{
  getrusage(RUSAGE_SELF, &ru1);
  gettimeofday(&time1, (struct timezone *)0);
}
/*--------------------------------------------------------------------------*/
/*         Print the process usage calculated from timers values extracted 
 *         before and after the transfer execution.
 */
void
prusage(
  struct rusage *r0, struct rusage *r1,
  struct timeval *t0, struct timeval *t1
)
{
  struct timeval tdiff;
  int t, ms;
  register char *cp;
  double cput = calc_cpu_time(r0, r1);
  double realt = calc_real_time(t0, t1);

  /* t == total user delta time + total system delta time */
  if (debug)   
  { 
    printf("timers     : end          startup\n");
    printf("user (sec) : %9ld  %9ld\n",r1->ru_utime.tv_sec, 
           r0->ru_utime.tv_sec);
    printf("user (usec): %9ld  %9ld\n",r1->ru_utime.tv_usec, 
           r0->ru_utime.tv_usec);
    printf("sys  (sec) : %9ld  %9ld\n",r1->ru_stime.tv_sec, 
           r0->ru_stime.tv_sec);
    printf("sys  (usec): %9ld  %9ld\n",r1->ru_stime.tv_usec, 
           r0->ru_stime.tv_usec);
    printf("time (sec) : %9ld  %9ld\n",t1->tv_sec,t0->tv_sec);
    printf("time (usec): %9ld  %9ld\n",t1->tv_usec,t0->tv_usec);
  }
  /* for the AIX debug, most counters are outside a good range  
    printf("                               r0                   r1\n");
    printf("ru_ixrss    %20ld %20ld \n", r0->ru_ixrss  ,r1->ru_ixrss  );    
    printf("ru_idrss    %20ld %20ld \n", r0->ru_idrss  ,r1->ru_idrss  );    
    printf("ru_isrss    %20ld %20ld \n", r0->ru_isrss ,r1->ru_isrss );  
    printf("ru_minflt   %20ld %20ld \n", r0->ru_minflt  ,r1->ru_minflt  );   
    printf("ru_majflt   %20ld %20ld \n", r0->ru_majflt  ,r1->ru_majflt  );   
    printf("ru_nswap    %20ld %20ld \n", r0->ru_nswap  ,r1->ru_nswap  );    
    printf("ru_inblock  %20ld %20ld \n", r0->ru_inblock  ,r1->ru_inblock );  
    printf("ru_oublock  %20ld %20ld \n", r0->ru_oublock  ,r1->ru_oublock  );  
    printf("ru_msgsnd   %20ld %20ld \n", r0->ru_msgsnd  ,r1->ru_msgsnd  );   
    printf("ru_msgrcv   %20ld %20ld \n", r0->ru_msgrcv  ,r1->ru_msgrcv   );   
    printf("ru_nsignals %20ld %20ld \n", r0->ru_nsignals  ,r1->ru_nsignals); 
    printf("ru_nvcsw    %20ld %20ld \n", r0->ru_nvcsw   ,r1->ru_nvcsw   );    
    printf("ru_nivcsw   %20ld %20ld \n", r0->ru_nivcsw  ,r1->ru_nivcsw  );   
  */

  /* cpu time in mseconds */
  t = (int)(cput / 1000.0);

  /* ms == value of the internal clock at the end of the xfer   */      
  /*                             also called real time.         */
  /* real time in mseconds */
  ms = (int)(realt / 1000.0);

  /* The display is based on variables provided by the function getrusage 
     Info located in : /usr/include/sys/resource.h  
  */                       
#if defined(SYSV)

#if defined(_AIX)
  /* with AIX cernsp most counters are wrong  
   * cp = "%Uuser %Ssys %Ereal %P %Xi+%Dd %Mmaxrss %F+%Rpf %Ccsw\0";
   */
  cp = "%Uuser %Ssys %Ereal %P\0";

#else 
#if defined(__osf__)
  cp = "%Uuser %Ssys %Ereal %P %Xi+%Dd %Mmaxrss %F+%Rpf %Ccsw\0";
#else
#if defined(sgi)                        /* IRIX 3.3 will show 0 for %M,%F,%R,%C */
  cp = "%Uuser %Ssys %Ereal %P %Mmaxrss %F+%Rpf %Ccsw\0";
#else 
#if defined(__Lynx__)
  cp = "%Uuser %Ssys %Ereal %P\0";
#else 
  cp = "%Uuser %Ssys %Ereal %P\0";    /* all SYSV except those mentionned */
#endif /*__lynx__ */
#endif /* sgi     */
#endif /*__osf__  */
#endif /* _AIX    */

#else  /* BSD system */
  cp = "%Uuser %Ssys %Ereal %P %Xi+%Dd %Mmaxrss %F+%Rpf %Ccsw\0"; 

#endif /* SYSV    */

  for (; *cp; cp++) {
    if (*cp != '%') {
      putc(*cp, stderr);
      /*  *outp++ = *cp; */
    } else if (cp[1]) 
      switch(*++cp) {
      case 'U':
        tvsub(&tdiff, &r1->ru_utime, &r0->ru_utime);
        fprintf(stderr,"%ld.%06ld", (long)tdiff.tv_sec, (long)tdiff.tv_usec);
        break;

      case 'S':
        tvsub(&tdiff, &r1->ru_stime, &r0->ru_stime);
        fprintf(stderr,"%ld.%06ld", (long)tdiff.tv_sec, (long)tdiff.tv_usec);
        break;

      case 'E':
        psecs( ms / 1000);
        break;

      case 'P':
        fprintf(stderr,"%.1f%%", (cput*100.0 / (realt ? realt : 1.0)) );
        break;

#if !defined(SYSV) || defined(__osf__) || defined(_AIX)
      case 'W':
        {
          int i = r1->ru_nswap - r0->ru_nswap;
          fprintf(stderr,"%d", i);
          break;
        }
            
      case 'X':
        fprintf(stderr,"%ld", t == 0 ? 0 : (r1->ru_ixrss-r0->ru_ixrss)/t);
        break;

      case 'D':
        fprintf(stderr,"%ld", t == 0 ? 0 :
                (r1->ru_idrss+r1->ru_isrss - (r0->ru_idrss+r0->ru_isrss))/t);
        break;

      case 'K':
        fprintf(stderr,"%ld", t == 0 ? 0 :
                ((r1->ru_ixrss+r1->ru_isrss+r1->ru_idrss) -
                 (r0->ru_ixrss+r0->ru_idrss+r0->ru_isrss))/t);
        break;

      case 'M':
        fprintf(stderr,"%ld", r1->ru_maxrss/2);
        break;
  
      case 'F':
        fprintf(stderr,"%ld", r1->ru_majflt-r0->ru_majflt);
        break;

      case 'R':
        fprintf(stderr,"%ld", r1->ru_minflt-r0->ru_minflt);
        break;

      case 'I':
        fprintf(stderr,"%ld", r1->ru_inblock-r0->ru_inblock);
        break;

      case 'O':
        fprintf(stderr,"%ld", r1->ru_oublock-r0->ru_oublock);
        break;

      case 'C':
        fprintf(stderr,"%ld+%ld",
                r1->ru_nvcsw-r0->ru_nvcsw, r1->ru_nivcsw-r0->ru_nivcsw);
        break;
#endif /* !SYSV || __osf__ */
      default:
        putc(*cp, stderr);
        break;
      } /* switch */
  } /* for */
}
/*--------------------------------------------------------------------------*/
/* add 2 times structure and move usec bigger than 1000000 to sec */
void              
tvadd(tsum, t0, t1)
     struct timeval *tsum, *t0, *t1;
{
  tsum->tv_sec = t0->tv_sec + t1->tv_sec;
  tsum->tv_usec = t0->tv_usec + t1->tv_usec;
  if (tsum->tv_usec > 1000000) tsum->tv_sec++, tsum->tv_usec -= 1000000;
}
/*--------------------------------------------------------------------------*/
/* substract 2 time structure (t1 > t0) */
void             
tvsub(tdiff, t1, t0)
     struct timeval *tdiff, *t1, *t0;
{ 
  tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
  tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
  if (tdiff->tv_usec < 0)  tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}

/*--------------------------------------------------------------------------*/
void          
psecs(int l)
{
  int i = (int)l/3600;

  if (i) {
    /* hours:min:sec */
    fprintf(stderr,"%d:", i);
    i = (int)l % 3600;
    fprintf(stderr,"%.2d:%.2d", i/60, i%60);
  } else {
    /* min:sec */
    i = (int)l;
    fprintf(stderr,"%d:%.2d", i/60, i%60);
  }
}
/*--------------------------------------------------------------------------*/
/*                        N R E A D                                           */

int                          
Nread(SOCKET s, void* bufp, int count )
{
#if defined(__lynx__) || defined(__svr4__) || defined(_AIX) || defined(__NTVIS__) || defined(__FreeBSD__)
  struct sockaddr from;
#else
  struct sockaddr_in from;
#endif
  int len = sizeof(from);
  register int cnt;

  if (timeout > 0) {
    fd_set readfds, exceptfds;
    struct timeval tv_timeout;
    int n;

    tv_timeout.tv_sec = timeout/1000;
    tv_timeout.tv_usec = (timeout%1000)*1000;

    FD_ZERO(&readfds);
    FD_ZERO(&exceptfds);
    FD_SET(s, &readfds);
    FD_SET(s, &exceptfds);

    n =  select( s+1, &readfds, NULL, &exceptfds, &tv_timeout );
    if (n == SOCKET_ERROR) {
      sockets_err("select read");
    } else if (n == 0) {
      return (0);
    }

  }

  if( udp )  
    {cnt = recvfrom( s, bufp, count, 0, SOCKADDR_CAST &from, &len );
    numCalls++;
    } 
  else 
    {if( b_flag )  cnt = mread( s, bufp, count );        /* fill bufp */
    else 
      {
#if defined(__NTVIS__)
        cnt = recv( s, bufp, count, 0 );
#else
        cnt = read( s, bufp, count );
#endif /* __NTVIS__ */
        numCalls++;
      }
    if (touchdata && cnt > 0) 
      {register int c = cnt, sum = 0;
       register char *b = bufp;
       while (c--) sum += *b++;
      }
    }
  /* rchrch   printf (" numcall %d read buffer %d bytes \n",numCalls,cnt); */
  return(cnt);
}
/*--------------------------------------------------------------------------*/
/*                        N W R I T E                                         */

int                                    
Nwrite( int s, void* bufp, int count )
{
  register int cnt;
  if( udp )  
    {
    again:
      cnt = sendto( s, bufp, count, 0, SOCKADDR_CAST &sinhim,
                                                sizeof(sinhim) );

      numCalls++;

#if defined(__NTVIS__)
      if( cnt<0 && WSAENOBUFS == WSAGetLastError()) 
#else
      if( cnt<0 && errno == ENOBUFS )  
#endif /* __NTVIS__ */

        { delay(18000); errno = 0; goto again; }
    }  else /* if (udp) */
    {

#if defined(__NTVIS__)
      cnt = send( s, bufp, count, 0 );
      numCalls++;
#else
      cnt = write( s, bufp, count );
      numCalls++;
#endif /* __NTVIS__ */
    }        
  return(cnt);
}
/*--------------------------------------------------------------------------*/
void
delay(us)
int us;        
{
  struct timeval tv;

  tv.tv_sec = 0;
  tv.tv_usec = (time_t)us;

#if defined(__hpux__)
  select(1, 0, 0, 0, &tv); 
#else
  select(1, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tv); 
#endif
}
/*--------------------------------------------------------------------------*/
/*                        M R E A D        
 *
 * This function performs the function of a read(II) but will
 * call read(II) multiple times in order to get the requested
 * number of characters.  This can be necessary because
 * network connections don't deliver data with the same
 * grouping as it is written with.  Written by Robert S. Miles, BRL.
 */
int
mread(int s, char* bufp, unsigned n)
{
  register unsigned        count = 0;
  register int                nread;
  
  do
    {
#if defined(__NTVIS__)
     nread = recv(s, bufp, n-count, 0);
#else
     nread = read(s, bufp, n-count);
#endif /* __NTVIS__ */
     numCalls++;
     if(nread < 0)  {perror("ttcp_mread"); return(-1); }
     if(nread == 0)  return((int)count);
     count += (unsigned)nread;
     bufp += nread;
    }while(count < n);
  return((int)count);
}

/*--------------------------------------------------------------------------*/
void
open_log()
{static long sysTicks;
#if defined(UNDER_CE)
	SYSTEMTIME SystemTime;
#endif
 sprintf(logfile,"%s_%s",logfile_head,trans?"t":"r");

 fprintf(stderr,"open the log file >%s<\n",logfile);
 if ((fplog = fopen(logfile,"r")) == NULL)    
       
   {if ((fplog = fopen(logfile,"a+")) == NULL)
     {fprintf(stderr,"Failure : creation of the file >%s< \n",logfile );
      exit(1);
     }
    else 
     {fprintf(fplog," creation date : ");
      /* get date */
#if defined(UNDER_CE)
	GetLocalTime(&SystemTime);
	sprintf(fplog,"%02d-%02d-%02d %02d:%02d\n", SystemTime.wDay, month[SystemTime.wMonth - 1],
		SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute);
#else
      time(&sysTicks);
      tms = localtime(&sysTicks);
      fprintf(fplog,"%02d-%02d-%02d %02d:%02d\n",
              tms->tm_mday, tms->tm_mon, tms->tm_year,tms->tm_hour, tms->tm_min);
#endif

      /* An other version will produce : Mon Aug  4 16:32:16 1997
       * long  lDxcomsTicks;  char *pDateTime; 
       * time(&lDxcomsTicks);
       * pDateTime = ctime(&lDxcomsTicks);  *(pDateTime+24) = '\0';
       * fprintf(fplog," ttcp called : %s", pDateTime);
       */
      fprintf(fplog,"format\n");
      fprintf(fplog,",buflen, nbuf(byte), bufalign(byte), bufoffset(byte)");
      fprintf(fplog,",  port, sockbufsize(byte), UserTime(sec), SysTime(sec)\n");
      fprintf(fplog,",  CPUusage(%%), Validity, nbytes(byte), realt(sec)");
      fprintf(fplog,",  rate(MB/sec), I/O call, hours*3600+min*60+sec\n\n");
      /* day-month-year, hour:minute\n\n");  */
    }
  } /* file already exist */
   else 
   {fclose (fplog);
     if ((fplog = fopen(logfile,"a+")) == NULL)     
       {fprintf(stderr,"Failure : access of the file >%s< \n",logfile );
        exit(1);
           }
   }
} 
/*--------------------------------------------------------------------------*/
void
close_log()
{
#if defined(UNDER_CE)
	SYSTEMTIME SystemTime;

	GetLocalTime(&SystemTime);
	fprintf(fplog," %d\n", SystemTime.wHour * 3600 + SystemTime.wMinute * 60 + SystemTime.wSecond);
#else
 static long sysTicks;
 time(&sysTicks);
 tms = localtime(&sysTicks);
 fprintf(fplog," %d\n",((tms->tm_hour)*3600 + (tms->tm_min)*60 + tms->tm_sec));
#endif
 fclose(fplog);
 fflush(fplog);
}
/*--------------------------------------------------------------------------*/
/* routine emulating UNIX function under NT */
#if defined(__NTVIS__)

/*---------------------------------------------------------------------------*/
static void
error(char *pch)
{
        if (!opterr) {
                return;                // without printing
        }
        fprintf(stderr, "%s: %s: %c\n",
                (NULL != progname) ? progname : "getopt", pch, optopt);
}
/*---------------------------------------------------------------------------*/
int
getopt(int argc, char **argv, char *ostr)
{
  static char *place = EMSG;    /* option letter processing */
  register char *oli;           /* option letter list index */

  if (!*place) {
    // update scanning pointer
    if (optind >= argc || *(place = argv[optind]) != '-' || !*++place) {
      place = EMSG;
      return -1; 
    }
    // place now points to the first char past the initial '-'
    if (place[0] == '-') {
      // found "--"
      // Was the word just a '--'?
      if (place[1] == '\0')
        ++optind; // yes, so consume the word
      // otherwise, the '--' was the start of a longer word,
      // so do not consume it.
      place = EMSG;
      return -1;
    }
  }

  /* option letter okay? */
  if ((optopt = (int)*place++) == (int)':'
      || !(oli = strchr(ostr, optopt))) {
    if (!*place) {
      ++optind;
    }
    error("illegal option");
    return BADCH;
  }
  if (*++oli != ':') {        
    /* don't need argument */
    optarg = NULL;
    if (!*place)
      ++optind;
  } else {
    /* need an argument */
    if (*place) {
      optarg = place;                /* no white space */
    } else  if (argc <= ++optind) {
      /* no arg */
      place = EMSG;
      error("option requires an argument");
      return BADCH;
    } else {
      optarg = argv[optind];                /* white space */
    }
    place = EMSG;
    ++optind;
  }
  return optopt;                        // return option letter
}

void
usleep(unsigned int microseconds)
{
  Sleep(microseconds/1000);
}
#endif /* __NTVIS__ */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void
do_help()
{
char More_help[] =
" Details about the reply: \n"
"   Example: \n"
"   ttcp-t: buflen=8192, nbuf=100, align=16384/0, port=5010\n"
"   ttcp-t: File-Descriptor 0x4 Opened\n"
"   # tcp sender -> <host> #\n"
"   ttcp-t: 819200 bytes in 1.152557 real seconds = 694.109 KB/sec +++\n"
"   ttcp-t: 100 I/O calls, 11.526 msec(real)/call, 0.213 msec(cpu)/call\n"
"   ttcp-t: 0.001914user 0.019388sys 0:01real 1% 9i+58d 190maxrss 1+2pf 177+180csw\n"
"   ttcp-t: buffer address 0x28000\n"
"   ttcp-t: File-Descriptor  fd 0x4 Closed\n"
"   ttcp done.\n\n"
"cpu seconds  ==  (sec) elapse ru_utime + elapse ru_stime.\n"
"                 ru_utime == The total amount of time running in user mode.\n"
"                 ru_stime == The total amount of time spent in the system.\n"
"                             executing on behalf of the process.\n"
"real seconds ==  elapse time calculated by the system timer (date).\n"
"I/O calls    ==  I/O call to the driver.\n"
"msec/call    ==  average elapse time (Real seconds) between each I/O.\n"
"calls/sec    ==  invert of msec/call.\n"
"user         ==  (sec.msec) elaspe ru_utime.\n"
"sys          ==  (sec.msec) elapse ru_stime.\n"
"real         ==  (min:sec)  CPU seconds.\n"
"%%           ==  Real seconds / CPU seconds.\n"
"(ru_ixrss)i+(ru_idrss)d\n"
"             ru_ixrss  == An integral value indicating the amount of memory \n"
"                          used by the text segment that was also shared among\n"
"                          other processes. This value is expressed in units of\n"
"                          kilobytes * seconds-of-execution and is calculated \n"
"                          by adding the number of shared memory pages in use \n"
"                          each time the internal system clock ticks, and then\n"
"                          averaging over one-second intervals.\n"
"             ru_idrss  == An integral value of the amount of unshared memory \n"
"                          in the data segment of a process (expressed in \n"
"                          units of kilobytes * seconds-of-execution).\n";
 
char More_help1[] =
"  (ru_maxrss/2)maxrss.\n"
"             ru_maxrss == The maximum size, in kilobytes, of the used\n"
"                          resident set size. \n"
"  (ru_majflt)+(ru_minflt)pf : Page fault\n"
"             ru_majflt == The number of page faults serviced that required\n"
"                          I/O activity.\n"
"             ru_minflt == The number of page faults serviced without any\n"
"                          I/O activity. In this case, I/O activity is \n"
"                          avoided by reclaiming a page frame from the list \n"
"                          of pages awaiting reallocation. \n"
"(ru_nvcsw)+(ru_nivcsw)csw : context switch\n"
"             ru_nvcsw  == The number of times a context switch resulted \n"
"                          because a process voluntarily gave up the \n"
"                          processor before its time slice was completed. \n"
"                          This usually occurs while the process waits \n"
"                          for availability of a resource.\n"
"             ru_nivcsw == The number of times a context switch resulted \n"
"                          because a higher priority process ran or because\n"
"                          the current process exceeded its time slice.\n\n";

char More_help2[] =
"log file format :\n"
"         buflen, nbuf(byte), bufalign(byte), bufoffset(byte)\n"
"         port, sockbufsize(byte), UserTime(sec), SysTime(sec), CPUusage(%)\n"
"         nbytes(byte), realt(sec), rate(MB/sec), I/O call,\n"
"                 hours*3600+min*60+sec\n\n";

  fprintf(stderr,More_help);
  fprintf(stderr,More_help1);
  fprintf(stderr,More_help2);
}
/*---------------------------------------------------------------------------*/
void
do_Usage()
{
char Usage[] =
"  Usage: ttcp -t [-options] host [ < in ]    ttcp -r [-options > out]\n"
"Example: ttcp -t -s -v -n100 host            ttcp -r -s -v -n100\n"
"Common options:\n"
"    -V      prints version number and date of last modification\n"
"    -L      create and append all results to a file named ttcp_log\n"
"    -h      more help\n"
"    -l ##   length of bufs read from or written to network (default 8192,\n"
"            max 65535)\n"
"    -u      use UDP instead of TCP\n"
"    -p ##   port number to send to or listen at (default 5010)\n"
#if defined(__linux__)
"    -P ##   link-layer priority (default 0)\n"
#endif
"    -s      (ttcp -t) : source a pattern to network\n"
"            (ttcp -r) : sink (discard) all data from network\n"
"    -A ##   align the start of buffers to this modulus (default 16384)\n"
"    -O ##   start buffers at this offset from the modulus (default 0)\n"
"    -v      verbose: print more statistics\n"
"    -d      set SO_DEBUG socket option\n"
"    -b ##   set socket buffer size (if supported)\n"
"    -f X    format for rate: b,B = bits, bytes k,K = kilo{bits,bytes};\n"
"                             m,M = mega{bits,bytes}; g,G = giga{bits,bytes}\n"
"    -w ##   set timeout value (in milliseconds) to exit if no receive data or tcp connect\n"
"Options specific to (ttcp -t) :\n"
"    -n ##   number of source bufs written to network (default 2048)\n"
"    -x      use random data in tcp/udp frames (-I provides seed)\n"
"    -D      don't buffer TCP writes (sets TCP_NODELAY socket option)\n"
"    -H      print hash marks to indicate progress, one per buffer\n"
"    -I      init/seed value for RNG when sending random size bufs (default 1)\n"
"    -N ##   number of source bufs per burst, i.e between sleeps (default 1)\n"
"    -R ##   send random size buffers with minimum size specified, max size\n"
"            is value of -l option\n"
"    -S ##   millisecs between bursts (only used for udp, 10ms resolution)\n"
"Options specific to (ttcp -r) :\n"
"    -B      for -s, only output full blocks as specified by -l (for TAR)\n"
"    -T      \"touch\": access each byte as it's read\n"
"    -i      report information on out of order sequence numbers\n"
#if defined(DEBUG)
"Options for debug:\n"
"    --nostart do not send UDP start packets\n"
"    --noend   do not send UDP end packets\n"
"    --nodata  do not send UDP data packets\n"
"    --debug   print extra debug outputs\n"
#endif /* DEBUG */
;
 
  fprintf(stderr,Usage);
}

/* Define automatic Emacs variables for consistent code formatting */
/* Local Variables:     */
/* c-basic-offset:2     */
/* indent-tabs-mode:nil */
/* End:                 */
