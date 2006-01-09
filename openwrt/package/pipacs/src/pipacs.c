// http://www.phj.hu/freesoft.asp
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

#ifndef LINUX
#include <mstcpip.h>
#include <ws2tcpip.h>
#else
#include <termios.h>
struct promisc_device
{
    char name[16]; /* name (e.g. eth0) */

    int reset; /* do we have to reset it on exit ? */
    struct ifreq oldifr; /* old settings */

    struct promisc_device *next;
};

#endif

DWORD  dwIoControlCode=SIO_RCVALL;
DWORD   dwProtocol=IPPROTO_IP,  dwInterface=0;

#define MAXVER 2
#define MINVER 6
SOCKET        s;

//
// Filters (Globals)
//
unsigned int   uiSourceAddr=0,  uiDestAddr=0, uiXAddr=0;
unsigned short usSourcePort = 0, usDestPort = 0, usXPort = 0;
unsigned short usSourceNet = 32, usDestNet = 32, usXNet = 32;
unsigned long  ulDestNet=0xffffffff, ulSourceNet=0xffffffff, ulXNet=0xffffffff;
BOOL           bFilter=FALSE;
int            iline=25;
char myipname[64];
char pattern[1024];
int justheader=0;
int gre=0;
int sortbysize,fromip,toip;
int skipvlan=0;

extern char filename[128];

extern char intlist[128];

#ifndef LINUX
void PrintInterfaceList( void );
int  GetInterface(SOCKET , SOCKADDR_IN *, int );
#endif
extern int InitIPAcc( void );
extern int CloseIPAcc( void );
extern int iCycle;
extern int iScreen;
extern int iFile;
extern int iDetail;
extern int iRun;
extern long lNum;
extern FILE *f;
extern int iProto;
extern int iSum;
extern char execname[];
extern int mostird;
extern  int iLnxplus;

int set_raw_mode(void)
{
    int fd = STDIN_FILENO;
    struct termios t;
	
    if (tcgetattr(fd, &t) < 0) { perror("tcgetattr");	return -1; }
    t.c_lflag &= ~ICANON;
    if (tcsetattr(fd, TCSANOW, &t) < 0)	{ perror("tcsetattr"); return -1; }
    setbuf(stdin, NULL);
    return 0;
}//
// Function: usage
//
// Description:
//    Prints usage information.
//
char *author = "phj";

void usage(char *progname)
{
    printf("  usage: %s options\n    where options:\n", progname);
        printf("       [-c:sec]             Dump cycle in sec (10)\n");
        printf("       [-f:file[-e:program]] Results into a file [and exec program](-)\n");
        printf("       [-n:db]              Execute just db cycle (0)\n");
        printf("       [-l:lineno]          Print lineno lines of hosts(25)\n");
        printf("       [-k]                 Sort result by packet count (size)\n");
        printf("       [-1]                 Ignore source IP (don't ignore)\n");
        printf("       [-2]                 Ignore destination IP (don't ignore)\n");
        printf("       [-h]                 Print just the header(use -a!)\n");
        printf("       [-a]                 Print packet info&data (-)\n");
        printf("       [-p]                 Print just summary info (-)\n");
        printf("            Otherwise print sum&ip pairs\n");
//#ifndef LINUX
        printf("       [-t:[tcp|udp|icmp|....|number]] Filter on IP protocoll (ALL)\n");
//#endif
        printf("       [-g]                 Make GRE encapsulation trasparent (-)\n");
        printf("       [-v]                 Skip VLAN headers (-)\n");
	printf("       [-sa:IP[/Net]]       Filter on source address (-)/net\n");
        printf("       [-sp:Port]           Filter on source port (-)\n");
        printf("       [-da:IP[/Net]]       Filter on dest address/net (-)\n");
        printf("       [-dp:Port]           Filter on dest port(-)\n");
        printf("       [-xa:IP[/Net]]       Filter on src|dest address/net (-)\n");
        printf("       [-xp:Port]           Filter on src|dest port (-)\n");
        printf("       [-pa:pattern]        String match (0), last param!!!\n");
#ifndef LINUX
        printf("       [-i:int]             Capture on this interface (0)\n");
        printf("             Available interfaces:\n");
        PrintInterfaceList();
#else
        printf("       [-i:int[,int]]       Capture on this interface (eth0)\n");
#endif
        printf("             Filtering rules:  t && (sa|da|xa) && (sp|dp|xp)");
        printf("\nVer. %d.%d  (c):2000-2006, P l¢czi-Horv th J nos\n",MAXVER,MINVER);
#ifndef LINUX
        WSACleanup();
        ExitProcess(-1);
#else
        exit(5);
#endif
}

//
// Function: ValidateArgs
//
// Description:
//    This function parses the command line arguments and
//    sets global variables to indicate how the app should act.
//
void ValidateArgs(int argc, char **argv)
{
    int   i,j;
    char *ptr;
    
    sortbysize=1; fromip=1; toip=1;

        if (argc <2) { usage(argv[0]); return; }
        if (*(author+2) != 'j') { usage(argv[0]); return; }
    for(i=1; i < argc ;i++) {
        if ((argv[i][0] == '-') || (argv[i][0] == '/')) {
            switch (tolower(argv[i][1])) {
                case 't':        // traffic type
                    ptr = &argv[i][2];
                    while (*++ptr)  *ptr = toupper(*ptr);
                    ptr = &argv[i][3];
                    for ( j=0;j<134;j++) {
                    if (!strcmp(ptr, szProto[j])) {
//                        dwIoControlCode = SIO_RCVALL;
#ifdef LINUX
                        dwProtocol = j;
#endif
                        iProto=j;
                        break;
                    }
                    }
                    if ((j>133) && atoi(&argv[i][3])) {
//                        dwIoControlCode = SIO_RCVALL;
#ifdef LINUX
                        dwProtocol = atoi(&argv[i][3]);
#endif
                        iProto=atoi(&argv[i][3]);
                    } else  if (j>133)    usage(argv[0]);
                    break;
                case 'i':        // interface number
#ifndef LINUX
                    dwInterface = atoi(&argv[i][3]);
#else
                                    strcpy(intlist,&argv[i][3]);
                                        ptr=strchr(intlist,' ');
                                        if (ptr) *ptr=0;
#endif
                    break;
                case 'g':        // gre
                    gre=1;
                    break;
                case 'c':        // cycle time
                    iCycle = atoi(&argv[i][3]);
                    break;
                case 'a':        // cycle time
                    iDetail = 1;
                    break;
                case 'h':        // cycle time
                    iDetail = justheader = 1;
                    break;
                case 'n':        // just n cycle
                    lNum = atol(&argv[i][3]);
                    break;
                case 'l':        // lineno lines
                    iline = atoi(&argv[i][3]);
                    break;
                case 'p':        // just summary
                    if ((tolower(argv[i][2]) == 'a')) {
                       strcpy(pattern,&argv[i][4]); printf("\n Pattern: \'%s",&argv[i][4]);
                       while (++i<argc) { strcat(pattern," "); strcat(pattern,&argv[i][0]); printf(" %s",argv[i]); }
                       printf("\'\n");
                    } else  iSum=1;
                    break;
                case 'f':        // filename to write
                    strcpy(filename,&argv[i][3]);
                    iFile=1; //iScreen=0;
                    break;
                case 'e':        // execname
                    strcpy(execname,&argv[i][3]);
                    break;
                case 'k':        // sor by count
                    sortbysize = 0;
                    break;
                case '1':        // ignore src
                    fromip = 0;
                    break;
                case '2':        // ignore dst
                    toip = 0;
                    break;
                case 'v':        // sor by count 
                    skipvlan = 4; 
                    if ((tolower(argv[i][2]) == ':')) { 
                         skipvlan=atoi(&argv[i][3]); 
                     } 
                     break; 
                case 's':        // Filter on source ip or port
                    if (tolower(argv[i][2]) == 'a') {
                       ptr=strchr(&argv[i][4],'/');
                       if (ptr) { usSourceNet=atoi(ptr+1); *ptr=0;}
                       uiSourceAddr = ntohl(inet_addr(&argv[i][4]));
                    } else if (tolower(argv[i][2]) == 'p')
                        usSourcePort = (unsigned short)atoi(&argv[i][4]);
                    else
                    usage(argv[0]);
                    bFilter = TRUE;
                    break;
                case 'd':       // Filter on dest ip or port
                    if (tolower(argv[i][2]) == 'a') {
                        ptr=strchr(&argv[i][4],'/');
                        if (ptr) { usDestNet=atoi(ptr+1); *ptr=0; }
                        uiDestAddr = ntohl(inet_addr(&argv[i][4]));
                    } else if (tolower(argv[i][2]) == 'p')
                        usDestPort = (unsigned short)atoi(&argv[i][4]);
                    else
                        usage(argv[0]);
                    bFilter = TRUE;
                    break;
                case 'x':        // Filter on source or dest ip or port
                    if (tolower(argv[i][2]) == 'a') {
                       ptr=strchr(&argv[i][4],'/');
                       if (ptr) { usXNet=atoi(ptr+1); *ptr=0; }
                       uiXAddr = ntohl(inet_addr(&argv[i][4]));
                    } else if (tolower(argv[i][2]) == 'p')
                        usXPort = (unsigned short)atoi(&argv[i][4]);
                    else
                        usage(argv[0]);
                    bFilter = TRUE;
                    break;
                default:
                    usage(argv[0]);
            }
        } else usage(argv[0]);
    }
    iLnxplus+=skipvlan;
    return;
}

#ifndef LINUX
//
// Function: PrintInterfaceList
//
// Description:
//    This function prints all local IP interfaces.
//
void PrintInterfaceList()
{
    SOCKET_ADDRESS_LIST *slist=NULL;
    SOCKET               s;
    char                 buf[2048];
    DWORD                dwBytesRet;
    int                  ret,
                         i;

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (s == SOCKET_ERROR) {
        printf("socket() failed: %d\n", WSAGetLastError());
        return;
    }
    ret = WSAIoctl(s, SIO_ADDRESS_LIST_QUERY, NULL, 0, buf, 2048,&dwBytesRet, NULL, NULL);
    if (ret == SOCKET_ERROR){
        printf("WSAIoctl(SIO_ADDRESS_LIST_QUERY) failed: %d\n",WSAGetLastError());
        return;
    }
    slist = (SOCKET_ADDRESS_LIST *)buf;
    for(i=0; i < slist->iAddressCount ;i++) {
        printf("               %-2d ........ [%s]\n", i,
            inet_ntoa(((SOCKADDR_IN *)slist->Address[i].lpSockaddr)->sin_addr));
    }
    closesocket(s);
    return;
}

//
// Function: GetInterface
//
// Description:
//    This function retrieves a zero based index and returns
//    the IP interface corresponding to that.
//
int GetInterface(SOCKET s, SOCKADDR_IN *ifx, int num)
{
    SOCKET_ADDRESS_LIST *slist=NULL;
    char                 buf[2048];
    DWORD                dwBytesRet;
    int                  ret;

    ret = WSAIoctl(s, SIO_ADDRESS_LIST_QUERY, NULL, 0, buf, 2048,&dwBytesRet, NULL, NULL);
    if (ret == SOCKET_ERROR) {
        printf("WSAIoctl(SIO_ADDRESS_LIST_QUERY) failed: %d\n",WSAGetLastError());
        return -1;
    }
    slist = (SOCKET_ADDRESS_LIST *)buf;
    if (num >= slist->iAddressCount)  return -1;
    ifx->sin_addr.s_addr = ((SOCKADDR_IN *)slist->Address[num].lpSockaddr)->sin_addr.s_addr;
        if (*author != 'p') return -1;
    return 0;
}
#endif
#ifdef LINUX
struct promisc_device *prom;

void init_capture( void )
/*
 * 1) Open our capture socket
 * 2) Set all the promisc devices to promiscous mode
 */
{
    struct ifreq ifr;
    struct promisc_device *p,*pp;
    struct protoent *pr;
    char *p1,*p2;

    if ((s = socket (AF_INET, SOCK_PACKET, htons (ETH_P_ALL))) < 0)
        {
            printf(" can't get socket: \n");
            exit(1);
        }
    strcpy(myipname,intlist);
    p1=intlist; p=NULL;
    while (p1) {
                pp=p;
                p = malloc(sizeof(struct promisc_device));
                if (pp) pp->next=p; else prom=p;
                if ( (p2=strchr(p1,',')))       *p2++=0;
                strcpy(&p->name,p1); p->next=NULL;
                printf(" %s",p->name); fflush(stdout);
                p1=p2;
//    while(p!=NULL) {
            strcpy (p -> oldifr.ifr_name, p -> name);

            if (ioctl (s, SIOCGIFFLAGS, &(p -> oldifr)) < 0) {
                    printf(" can't get flags: \n");
                    exit(2);
                }
            p -> reset = 1;
            ifr = p -> oldifr;
            if (ifr.ifr_flags & IFF_PROMISC) printf(" already promisc! \n");
            ifr.ifr_flags |= IFF_PROMISC;
            if (ioctl (s, SIOCSIFFLAGS, &ifr) < 0) {
                    printf(" can't set flags: \n");
                    exit(3);
                }
//          p = p -> next;
        }
}

void exit_capture(void)
{
    struct promisc_device *p;

    /* do we have to check (capture_sd >= 0) ? */

    p = prom;

    while(p != NULL) {
            if (ioctl (s, SIOCSIFFLAGS, &(p -> oldifr)) < 0) {
                    printf("can't reset flags: \n");
                }

            p = p -> next;
        }

    close (s);
}
#endif
//
// Function: main
//
int main(int argc, char **argv) {
    WSADATA       wsd;
    SOCKADDR_IN   if0;
    int           ret,count;
    unsigned int  optval;
    DWORD         dwBytesRet,
                  dwFlags,
                  nproc;
    char          rcvbuf[MAX_IP_SIZE];
    WSABUF        wbuf;
    unsigned long i;
#ifndef LINUX
    // Load Winsock
    //
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0) {
        printf(" WSAStartup() failed: %d\n", GetLastError());
        return -1;
    }
#else
    SOCKADDR    ssaddr;
    struct promisc_device *p;
    fd_set	ready;
    struct timeval tv;
#endif
    char Key;
    int status;
    FILE *input;
// Parse the command line
//
    strcpy(intlist,"eth0");
    for(i=100;i<255;i++) szProto[i]="?!?";
    szProto[103]="PIM";
    szProto[108]="IPCOMP";
    szProto[112]="VRRP";
    szProto[115]="L2TP";
    szProto[124]="ISIS";
    szProto[132]="SCTP";
    szProto[133]="FC";
    *execname=0;
    ValidateArgs(argc, argv);
    if (bFilter) {
                i=uiSourceAddr;
                if ( i || usSourcePort)
        printf(" Source: %03d.%03d.%03d.%03d/%d:%d\n",(i&0xff000000)>>24,(i&0x00ff0000)>>16,(i&0x0000ff00)>>8,i&0xff,uiSourceAddr?usSourceNet:0, usSourcePort);
                i=uiDestAddr;
                if ( i || usDestPort)
                        printf(" Dest. : %03d.%03d.%03d.%03d/%d:%d\n",(i&0xff000000)>>24,(i&0x00ff0000)>>16,(i&0x0000ff00)>>8,i&0xff,uiDestAddr?usDestNet:0, usDestPort);
                i=uiXAddr;
                if ( i || usXPort)
                        printf("  IP.   : %03d.%03d.%03d.%03d/%d:%d\n",(i&0xff000000)>>24,(i&0x00ff0000)>>16,(i&0x0000ff00)>>8,i&0xff,uiXAddr?usXNet:0, usXPort);
    }
        if (iFile) printf(" To file : %s\n",filename);
        if (iProto) printf(" Protocol: %s (%d)\n",szProto[iProto],iProto);
    // Create a raw socket for receiving IP datagrams
    //
#ifndef LINUX
    s = WSASocket(AF_INET, SOCK_RAW, dwProtocol, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (s == INVALID_SOCKET)
    {
        printf("WSASocket() failed: %d\n", WSAGetLastError());
        return -1;
    }
    // Get an interface to read IP packets on
    //
        memset(&if0,0,sizeof(if0));
        if0.sin_family = AF_INET;
    if0.sin_port = htons(0);
    if (GetInterface(s, &if0, dwInterface) != 0)
    {
        printf("Unable to obtain an interface\n");
        return -1;
    }
    sprintf(myipname,"%-16s",inet_ntoa(if0.sin_addr));
#else
        printf("starting capture ...."); fflush(stdout);
        init_capture();
        printf(" capture started ....\n"); fflush(stdout);
#endif
    printf(" Binding to IF: %s\n", myipname);
#ifndef LINUX
//
// This socket MUST be bound before calling the ioctl
//

    if (bind(s, (SOCKADDR *)&if0, sizeof(if0)) == SOCKET_ERROR) {
        printf("bind() failed: %d\n", WSAGetLastError());
        return -1;
    }
//
// Set the SIO_RCVALLxxx ioctl
//
    optval = 1;
    if (WSAIoctl(s, dwIoControlCode, &optval, sizeof(optval),
            NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
        printf("WSAIotcl() set raw socket failed; %d\n", WSAGetLastError());
//        return -1;
        optval = 2;
	if (WSAIoctl(s, dwIoControlCode, &optval, sizeof(optval),
            NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
        printf("WSAIotcl() set raw socket only failed; %d\n", WSAGetLastError());
        return -1;
        }
    }
    system("cls");
#else
    tv.tv_sec=0; tv.tv_usec=0;
    set_raw_mode();
    FD_ZERO(&ready);
    FD_SET(STDIN_FILENO,&ready);
#endif
    input = fopen("/dev/tty", "r");      //open the terminal keyboard 
    if (uiSourceAddr==0) ulSourceNet=0;
    else for ( i=0; i<32-usSourceNet; i++) ulSourceNet <<= 1;
    if (uiDestAddr==0) ulDestNet=0;
    else for ( i=0; i<32-usDestNet; i++) ulDestNet <<= 1;
    if (uiXAddr==0) ulXNet=0;
    else for ( i=0; i<32-usXNet; i++) ulXNet <<= 1;
    if (uiXAddr) uiSourceAddr=uiDestAddr=uiXAddr;
    if (usXPort) usSourcePort=usDestPort=usXPort;
    if (ulXNet) ulSourceNet=ulDestNet=ulXNet;
    InitIPAcc();
// Start receiving IP datagrams until interrupted
//
    count = 0;
    if (iFile && iDetail)   f=fopen(filename,"w+");
    if (iProto) bFilter=1;
    if (*(author+1) != 'h') iRun=0;
    while (iRun)    {
	rcvbuf[MAX_IP_SIZE]=0;
        wbuf.len = MAX_IP_SIZE;
        wbuf.buf = rcvbuf;
#ifndef LINUX
        dwFlags  = 0;
        ret = WSARecv(s, &wbuf, 1, &dwBytesRet, &dwFlags, NULL, NULL);
        if (ret == SOCKET_ERROR) {
            printf("WSARecv() failed: %d\n", WSAGetLastError());
            return -1;
        }
	if (kbhit()) {
#else
        dwFlags  = sizeof(ssaddr);

                ret = recvfrom (s, wbuf.buf, MAX_IP_SIZE, 0, &ssaddr, (int *) &dwFlags);
                if (ret == -1) continue;
                dwBytesRet=wbuf.len=ret;
                p=prom;
                while(p!=NULL)  {
                        if (!strcmp(p -> name, ssaddr.sa_data)) break;
                        p=p->next;
                }
                if (!p) {
//                      printf("\n%s: ignored",ssaddr.sa_data); fflush(stdout);
                        continue;
                }
        FD_ZERO(&ready);
	FD_SET(STDIN_FILENO,&ready);
	if (select(STDIN_FILENO+1,&ready,NULL,NULL,&tv)>0) {
//	if (FD_ISSET(STDIN_FILENO,&ready)) {
#endif
	   switch (getchar())  { /* branch to appropiate key handler */
		case 0x1b: /* Esc */
                    iRun=0;
		break;
		default:
                    mostird=1;
        	break;
	   }  //end of switch key
	}

// Deccode the IP header
//
        if (!(nproc = DecodeIPHeader(&wbuf, uiSourceAddr, usSourcePort, ulSourceNet,
                uiDestAddr, usDestPort, ulDestNet, dwBytesRet,usXPort,uiXAddr,ulXNet)))
        {
//            printf("Error decoding IP header!\n");
//            break;
        }
    }
    // Cleanup
    //
    if (iRun && !iDetail) CloseIPAcc();
    if (f) fclose(f);
#ifndef LINUX
    closesocket(s);
    WSACleanup();
#else
    exit_capture();
#endif
    return 0;
}
