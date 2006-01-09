#define LINUX 1
#ifdef LINUX
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#define BYTE unsigned char
#define DWORD unsigned long
#define BOOL char
#define TRUE 1
#define FALSE 0
#define WSADATA int
#define SOCKET int
#define SOCKADDR struct sockaddr
#define SOCKADDR_IN struct sockaddr_in
typedef struct _WSABUF {
	unsigned long len;
	unsigned char *buf;
} WSABUF;
#define SIO_RCVALL 0
#define SIO_RCVALL_IGMPMCAST 0
#define SIO_RCVALL_MCAST 0
#define ETH_P_ALL 0x0003
#else	// WINDOWS

#include <winsock2.h>
#include <windows.h>

#endif

#ifndef _RCVALL_H_
#define _RCVALL_H_


#define MAX_IP_SIZE        65535
#define MIN_IP_HDR_SIZE       20

#define HI_WORD(byte)    (((byte) >> 4) & 0x0F)
#define LO_WORD(byte)    ((byte) & 0x0F)

extern char *szProto[];



void PrintRawBytes   (BYTE *ptr, DWORD len);
int  DecodeIGMPHeader(WSABUF *wsabuf, DWORD iphdrlen);
int  DecodeUDPHeader (WSABUF *wsabuf, DWORD iphdrlen);
int  DecodeTCPHeader (WSABUF *wsabuf, DWORD iphdrlenz);
int  DecodeIPHeader  (WSABUF *wasbuf, unsigned int srcaddr, unsigned short srcport, unsigned long srcnet,
	  unsigned int destaddr, unsigned short destport, unsigned long destnet, DWORD bytesret,
	  unsigned short xport, unsigned int xip, unsigned long xnet);

#endif
