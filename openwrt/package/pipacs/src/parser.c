#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <time.h>
//#include <process.h>
#include "parser.h"

#define printit

extern BOOL bFilter;
extern int iline;
extern char * author;
extern char myipname[];
extern int justheader;
extern int gre,sortbysize,fromip,toip;
int nomac=1;
int mostird=0;
char mypbuff[2048];
// accounting variables
#define MAXHASH 0xffff
#define MAXTCPPORT 4096
unsigned long *iph=NULL; //[MAXHASH];

typedef struct {
                unsigned short from;
                unsigned short to;
                unsigned long byte;
                unsigned short pkt;
                unsigned short sport;
                unsigned short dport;
} ta;

ta *acc = NULL;
unsigned long tcppb[MAXTCPPORT];
unsigned long tcppp[MAXTCPPORT];
unsigned long typp[255];
unsigned long typb[255];
unsigned long udpb,tcpb,udpp,tcpp;
time_t elapsed=0;
int iCycle=10;
int iScreen=1;
int iFile=0;
long lNum=0;
char filename[128];
char intlist[128];
int iRun=1;
int iDetail=0;
FILE *f=NULL;
int iProto=0;
int iSum=0;
char execname[255];
char pbuf[8196];
char str[255];
extern char pattern[];
#ifndef LINUX
int iLnxplus=0;                 // Windows buffer without the MAC frame !
#else
int iLnxplus=14;                        // Linux plus IP header len =14 !!!
#endif
//
// A list of protocol types in the IP protocol header
//
char *szProto[255] = {"IP",     //  0
                   "ICMP",         //  1
                   "IGMP",         //  2
                   "GGP",          //  3
                   "IP",           //  4
                   "ST",           //  5
                   "TCP",          //  6
                   "UCL",          //  7
                   "EGP",          //  8
                   "IGP",          //  9
                   "BBN-RCC-MON",  // 10
                   "NVP-II",       // 11
                   "PUP",          // 12
                   "ARGUS",        // 13
                   "EMCON",        // 14
                   "XNET",         // 15
                   "CHAOS",        // 16
                   "UDP",          // 17
                   "MUX",          // 18
                   "DCN-MEAS",     // 19
                   "HMP",          // 20
                   "PRM",          // 21
                   "XNS-IDP",      // 22
                   "TRUNK-1",      // 23
                   "TRUNK-2",      // 24
                   "LEAF-1",       // 25
                   "LEAF-2",       // 26
                   "RDP",          // 27
                   "IRTP",         // 28
                   "ISO-TP4",      // 29
                   "NETBLT",       // 30
                   "MFE-NSP",      // 31
                   "MERIT-INP",    // 32
                   "SEP",          // 33
                   "3PC",          // 34
                   "IDPR",         // 35
                   "XTP",          // 36
                   "DDP",          // 37
                   "IDPR-CMTP",    // 38
                   "TP++",         // 39
                   "IL",           // 40
                   "SIP",          // 41
                   "SDRP",         // 42
                   "SIP-SR",       // 43
                   "SIP-FRAG",     // 44
                   "IDRP",         // 45
                   "RSVP",         // 46
                   "GRE",          // 47
                   "MHRP",         // 48
                   "BNA",          // 49
                   "IPSEC-ESP",     // 50
                   "IPSEC-AH",      // 51
                   "I-NLSP",       // 52
                   "SWIPE",        // 53
                   "NHRP",         // 54
                   "?55?",   // 55
                   "?56?",   // 56
                   "SKIO",   // 57
                   "V6ICMP",   // 58
                   "V6NoNXT",   // 59
                   "V6OPT",   // 60
                   "int.host",  // 61
                   "CFTP",         // 62
                   "loc.net",           // 63
                   "SAT-EXPAK",    // 64
                   "KRYPTOLAN",    // 65
                   "RVD",          // 66
                   "IPPC",         // 67
                   "dist.fs", // 68
                   "SAT-MON",    // 69
                   "VISA",       // 70
                   "IPCV",       // 71
                   "CPNX",       // 72
                   "CPHB",       // 73
                   "WSN",        // 74
                   "PVP",        // 75
                   "BR-SAT-MON", // 76
                   "SUN-ND",     // 77
                   "WB-MON",     // 78
                   "WB-EXPAK",   // 79
                   "ISO-IP",     // 80
                   "VMTP",       // 81
                   "SECURE-VMTP",// 82
                   "VINES",      // 83
                   "TTP",        // 84
                   "NSFNET-IGP", // 85
                   "DGP",        // 86
                   "TCF",        // 87
                   "IGRP",       // 88
                   "OSPF",    // 89
                   "Sprite-RPC", // 90
                   "LARP",       // 91
                   "MTP",        // 92
                   "AX.25",      // 93
                   "IPIP",       // 94
                   "MICP",       // 95
                   "SCC-SP",     // 96
                   "ETHERIP",    // 97
                   "ENCAP",      // 98
                   "priv.enc",    // 99
                   "GMTP"        // 99
                  };
//
// The types of IGMP messages
//
char *szIgmpType[] = {"",
                      "Host Membership Query",
                      "HOst Membership Report",
                      "",
                      "",
                      "",
                      "Version 2 Membership Report",
                      "Leave Group",
                                          "",
                                          ""
                     };

//
// Function: PrintRawBytes
//
// Description:
//    This function simply prints out a series of bytes
//    as hexadecimal digits.
//
void PrintRawBytes(BYTE *ptr, DWORD len)
{
    int        i,j;
//      if (! iFile) {
*(ptr+len)=0;
if ((*pattern==0) || strstr(ptr,pattern) ) {
        fprintf(iFile?f:stdout,"%s",pbuf);
        fprintf(iFile?f:stdout,"   " );
                while (len > 0)    {
        for(i=0; i < 16; i++)   {
            fprintf(iFile?f:stdout,"%x%x ", HI_WORD(*ptr), LO_WORD(*ptr));
            len--;
            ptr++;
            if (len == 0) {j=i++; while(++j < 16) fprintf(iFile?f:stdout,"   ");  break; }
        }
          fprintf(iFile?f:stdout," ");
        for(j=0; j < i; j++)    fprintf(iFile?f:stdout,"%c",isprint(*(ptr-i+j))?*(ptr-i+j):'.');
        if (len) fprintf(iFile?f:stdout,"\n   ");
    }
//      } else {
//              fwrite(ptr,sizeof(BYTE),len,f);
//      }
}
}

static char *ICMPTypeTable[]={
    "Echo Reply", "ICMP 1", "ICMP 2", "Dest Unreachable","SrcQuench", "Redirect", "6", "7","Echo Request","9","10",
    "Time Exceed", "ParamPrblm", "Timestamp", "Timestamp reply","InfoRqst", "InfoRply"
};
static char *Dstunreach[]={
"net unreach.","host unreach.","protocol unreach.","port unreach.",
"frag needed","source route?","",""
};
int DecodeICMPHeader(WSABUF *wsabuf, DWORD iphdrlen) {
    BYTE          *hdr = (BYTE *)((BYTE *)wsabuf->buf + iphdrlen + iLnxplus );
    unsigned short type,code,chksum,
                   id,
                   seq;
 unsigned long resptime,r1,r2;
    BYTE *hhh;
    SOCKADDR_IN    addr;
    type=*hdr++; code=*hdr++;
    sprintf(str," Type:%-12s Code:%3d,",ICMPTypeTable[type],code);

    strcat(pbuf,str);
    memcpy(&chksum, hdr, 2);
    hdr += 2; hhh=hdr;
    memcpy(&id, hdr, 2);
    hdr += 2;
    memcpy(&seq, hdr, 2);
    hdr+=2;
//    memcpy(&resptime, hdr, 4);
//   hdr+=4;
    switch (type) {
    case 3:
        memcpy(&addr.sin_addr.s_addr, hdr+16, 4);
        if (code==4 ) sprintf(str,"frag needed-Max MTU:%u at %-15s\n",ntohs(seq), inet_ntoa(addr.sin_addr));
        else sprintf(str,"%s at %-15s\n",Dstunreach[code&7],inet_ntoa(addr.sin_addr));
        hdr+=iphdrlen;
        break;
    case 11:
        memcpy(&addr.sin_addr.s_addr, hdr+16, 4);
        sprintf(str,"%s  at %-15s\n",code?"frag reass. exceed":"ttl exceed",inet_ntoa(addr.sin_addr));
        hdr+=iphdrlen;
        break;
    case 12:
        memcpy(&addr.sin_addr.s_addr, hdr+16, 4);
        sprintf(str," err:%d  at %-15s\n",id,inet_ntoa(addr.sin_addr));
        hdr+=iphdrlen;
        break;
    case 4:
        memcpy(&addr.sin_addr.s_addr, hdr+16, 4);
        sprintf(str," wait for %-15s\n",ntohs(id),inet_ntoa(addr.sin_addr));
        hdr+=iphdrlen;
        break;
    case 5:
        memcpy(&addr.sin_addr.s_addr, hhh, 4);
        sprintf(str," from gw: %-15s\n",inet_ntoa(addr.sin_addr));
        hdr+=iphdrlen;
        break;
    case 0:
    case 8:
         sprintf(str," Id:%3u Seq:%3u\n",ntohs(id),ntohs(seq));
         break;
    case 13:
    case 14:
        memcpy(&resptime, hdr, 4);
        hdr+=4;
        memcpy(&r1, hdr, 4);
        hdr+=4;
        memcpy(&r2, hdr, 4);
        hdr+=4;
        sprintf(str," Id:%3u Seq:%3d Rec/Tr %ld/%ld ms\n",ntohs(id),ntohs(seq),ntohl(r1)-ntohl(resptime),ntohl(r2)-ntohl(resptime));
        break;
    case 15:
    case 16:
         sprintf(str," Id:%3u Seq:%3d\n",ntohs(id),ntohs(seq));
         break;
    }
    strcat(pbuf,str);
    return hdr-(BYTE *)(wsabuf->buf + iphdrlen + iLnxplus);
}

//
// Function: DecodeIGMPHeader
//
// Description:
//    This function takes a pointer to a buffer containing
//    an IGMP packet and prints it out in a readable form.
//

int DecodeIGMPHeader(WSABUF *wsabuf, DWORD iphdrlen) {
    BYTE          *hdr = (BYTE *)((BYTE *)wsabuf->buf + iphdrlen + iLnxplus);
    unsigned short chksum,
                   version,
                   type,
                   maxresptime;
    SOCKADDR_IN    addr;
    version = HI_WORD(*hdr);
    type    = LO_WORD(*hdr);

    hdr++;
    maxresptime = *hdr;
    hdr++;

    memcpy(&chksum, hdr, 2);
    chksum = ntohs(chksum);
    hdr += 2;

    memcpy(&(addr.sin_addr.s_addr), hdr, 4);
    sprintf(str,"   IGMP HEADER:\n");
    strcat(pbuf,str);
    if ((type == 1) || (type == 2))        version = 1;
    else        version = 2;
    sprintf(str,"   IGMP Version = %d\n   IGMP Type = %s\n",version, szIgmpType[type]);
    strcat(pbuf,str);
    if (version == 2) {
     sprintf(str,"   Max Resp Time = %d\n", maxresptime);
     strcat(pbuf,str);
    }
    sprintf(str,"   IGMP Grp Addr = %s\n", inet_ntoa(addr.sin_addr));
    strcat(pbuf,str);

    return 8;
}

//
// Function: DecodeUDPHeader
//
// Description:
//    This function takes a buffer which points to a UDP
//    header and prints it out in a readable form.
//
int DecodeUDPHeader(WSABUF *wsabuf, DWORD iphdrlen) {
    BYTE          *hdr = (BYTE *)((BYTE *)wsabuf->buf + iphdrlen + iLnxplus);
    unsigned short shortval,
                   udp_src_port,
                   udp_dest_port,
                   udp_len,
                   udp_chksum;
    memcpy(&shortval, hdr, 2);
    udp_src_port = ntohs(shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    udp_dest_port = ntohs(shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    udp_len = ntohs(shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    udp_chksum = ntohs(shortval);
    hdr += 2;

    sprintf(str," UDP:  SPort: %-05d  | DPort: %-05d",udp_src_port, udp_dest_port);
    strcat(pbuf,str);
    sprintf(str," | Len: %-05d | CSum: 0x%08x\n",udp_len, udp_chksum);
    strcat(pbuf,str);
    return hdr-(BYTE *)(wsabuf->buf + iphdrlen + iLnxplus);
}

//
// Function: DecodeTCPHeader
//
// Description:
//    This function takes a buffer pointing to a TCP header
//    and prints it out in a readable form.
//
int DecodeTCPHeader(WSABUF *wsabuf, DWORD iphdrlen) {
    BYTE           *hdr = (BYTE *)((BYTE *)wsabuf->buf + iphdrlen + iLnxplus);
    unsigned short shortval;
    unsigned long   longval;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
    sprintf(str," TCP: SPort: %u", shortval);
    strcat(pbuf,str);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
    sprintf(str,"  DPort: %u", shortval);
    strcat(pbuf,str);
    hdr += 2;

    memcpy(&longval, hdr, 4);
    longval = ntohl(longval);
    sprintf(str," Seq: %lX", longval);
    strcat(pbuf,str);
    hdr += 4;

    memcpy(&longval, hdr, 4);
    longval = ntohl(longval);
    sprintf(str," ACK: %lX", longval);
    strcat(pbuf,str);
    hdr += 4;
//    printf("   Header Len : %d (bytes %d)\n", HI_WORD(*hdr), (HI_WORD(*hdr) * 4));

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval) & 0x3F;
    sprintf(str," Flags: ");
    strcat(pbuf,str);
    if (shortval & 0x20)        strcat(pbuf,"URG ");
    if (shortval & 0x10)        strcat(pbuf,"ACK ");
    if (shortval & 0x08)        strcat(pbuf,"PSH ");
    if (shortval & 0x04)        strcat(pbuf,"RST ");
    if (shortval & 0x02)        strcat(pbuf,"SYN ");
    if (shortval & 0x01)        strcat(pbuf,"FIN ");
    strcat(pbuf,"\n");
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
//    printf("   Window size: %d\n", shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
//    printf("   TCP Chksum : %d\n", shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
    hdr += 2;
//    printf("   Urgent ptr : %d\n", shortval);

    return hdr-(BYTE *)(wsabuf->buf + iphdrlen + iLnxplus);
}

int DecodeGREHeader(WSABUF *wsabuf, DWORD iphdrlen,DWORD bytesret,
 unsigned int srcip, unsigned short srcport, unsigned long srcnet,unsigned int destip, unsigned short destport, unsigned long destnet,
 unsigned short xport,unsigned int xip, unsigned long xnet)
 {
    BYTE           *hdr = (BYTE *)((BYTE *)wsabuf->buf + iphdrlen + iLnxplus);
    unsigned short shortval;
    unsigned long   longval;
    int ipe;
    BYTE           *orihdr;
    char *sstr;
    SOCKADDR_IN         srcaddr;

    orihdr=hdr;
    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
    sprintf(str," GRE Flag: %u Prot:", shortval);
    strcat(mypbuff,str);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    shortval = ntohs(shortval);
    ipe=0;
    sstr=str;
//    sprintf(str,"  Prot: %u", shortval);
    switch ( shortval ) {
    case 4: sstr="SNA";
       break;
    case 0xfe: sstr="OSI";
       break;
    case 0x200: sstr="PUP";
       break;
    case 0x600: sstr="XNS";
       break;
    case 0x800: sstr="IP";
        ipe=1;
       break;
    case 0x804: sstr="Chaos";
       break;
    case 0x806: sstr="ARP";
       break;
    case 0x6558: sstr="Tr.bridge";
       break;
    default: sprintf(str,"%u", shortval);
       break;
    }
    hdr += 2;
    strcat(mypbuff,sstr);
    if (ipe && gre) {
        int plusment,jj,protoment;
        plusment=iLnxplus;
        protoment=iProto;
        if (iProto==47)  iProto=0;
        iLnxplus+=4;
        nomac=0;
        iLnxplus=plusment+24;
        DecodeIPHeader(wsabuf,srcip,srcport,srcnet,destip,destport,destnet,bytesret,xport,xip,xnet);
        nomac=1;
        iLnxplus=plusment;
        iProto=protoment;
    }
return -1;
//    return hdr-(BYTE *)(wsabuf->buf + iphdrlen + iLnxplus);
}


int ClearIPAcc() {
        unsigned long i;
        ta *tai;
        for(i=0;i<MAXHASH;i++) *(iph + i)=0;
        tai=acc;
        for(i=0;i<MAXHASH;i++) { tai->from=tai->to=0; tai++; }
        for (i=0;i<MAXTCPPORT; i++) tcppb[i]=tcppp[i]=0;
        udpb=udpp=tcpp=tcpb=0;
        for (i=0;i<255; i++) typp[i]=0;
        for (i=0;i<255; i++) typb[i]=0;
        return 0;
        };

int InitIPAcc() {
        acc=malloc(MAXHASH*sizeof(ta));
        iph=malloc(MAXHASH*sizeof(long));
        if (!acc || !iph ) return 0;
        ClearIPAcc();
        time(&elapsed);
        return 1;
}

int bytesort(const void *s1, const void *s2) { // sorting tale in byte order
        ta *d1;
        ta *d2;
        d1= (ta *)s1; d2=(ta *)s2;
        if (d1->byte > d2->byte) return -1;
        if (d1->byte < d2->byte) return 1;
        return 0;
}
int countsort(const void *s1, const void *s2) { // sorting tale in packet count order
        ta *d1;
        ta *d2;
        d1= (ta *)s1; d2=(ta *)s2;
        if (d1->pkt > d2->pkt) return -1;
        if (d1->pkt < d2->pkt) return 1;
        return 0;
}
int CloseIPAcc( long ti) {
        unsigned long i;
        ta *tai;
    SOCKADDR_IN    srcaddr;
    SOCKADDR_IN    dstaddr;
        float ff;
        char str[16];
        unsigned long j,k,l;
        int lin=0;
        int linn;

        time(&elapsed);
        if (iFile) f=fopen(filename,"w+");
        k=0;
        if (sortbysize) qsort(acc,MAXHASH,sizeof(ta),bytesort);
	else  qsort(acc,MAXHASH,sizeof(ta),countsort);
        ff=0.0;
        for (i=0;i<255;i++) ff+=typb[i];
        for (i=0; i<MAXHASH; i++) {
            tai=acc + i;
            if ((tai->from!=0) && (tai->to!=0)) ++k;
        }
        if (iScreen) {
#ifndef LINUX
                system("cls");
#else
                system("clear");
//              printf("\033[1~");
#endif
                printf("%-16s Speed: %5.2f Kbit/s , %ld IP pairs / %ld secs.    %s@%s.hu",myipname,ff/ti/1024*8,k,ti,author,author);
                printf("\nProt:"); j=0; ++lin;
                while (1) {
                        l=k=0;
                        for (i=0;i<100;i++) if ( typb[i]>k) { k=typb[i]; l=i; }
                        if (k==0) break;
                        if ((j>0) && ((j%3)==0)) { printf("\n     "); ++lin; }
                        if (k>1024*1024) printf(" %-8.8s:%5.1fk/%-6.1f M",szProto[l],(float)typp[l]/1024,(float)k/(1024*1024));
                        else if (k>1024) printf(" %-8.8s:%5ld/%-6.1f k",szProto[l],typp[l],(float)k/1024);
                        else printf(" %-8.8s:%5ld/%-8ld",szProto[l],typp[l],k);
                        typb[l]=0;
                        ++j;
                }
                printf("\nPort:"); j=0; ++lin;
                k=0; linn=lin;
                while (1) {
                        l=k=0;
                        for (i=0;i<MAXTCPPORT;i++) if (tcppb[i]>k) { k=tcppb[i]; l=i; }
                        if (k==0) break;
                        if (j && (j%4)==0) {
                                if (lin >= linn+1) break;
                                printf("\n     ");
                                ++lin;
                        }
                        if (k>1024*1024) printf(" %04d:%4.1fk/%-5.1f M",l,(float)tcppp[l]/1024,(float)k/(1024*1024));
                        else if (k>1024) printf(" %04d:%4ld/%-5.1f k",l,tcppp[l],(float)k/1024);
                        else printf(" %04d:%4ld/%-7ld",l,tcppp[l],k);
                        tcppb[l]=0;
                        ++j;
                }
        } else if (f) {
                fprintf(f,"%-16s Speed: %5.2f Kbit/s , %ld IP pairs / %ld secs.    %s@%s.hu",myipname,ff/ti/1024*8,k,ti,author,author);
                fprintf(f,"\nProt:"); j=0;
                while (1) {
                        l=k=0;
                        for (i=0;i<100;i++) if ( typb[i]>k) { k=typb[i]; l=i; }
                        if (k==0) break;
                        if (k>1024*1024) fprintf(f," %-8.8s:%5.1fk/%-6.1f M",szProto[l],(float)typp[l]/1024,(float)k/(1024*1024));
                        else if (k>1024) fprintf(f," %-8.8s:%5ld/%-6.1f k",szProto[l],typp[l],(float)k/1024);
                        else fprintf(f," %-8.8s:%5ld/%-8ld",szProto[l],typp[l],k);
                        typb[l]=0;
                        ++j;
                }
                printf("\nPort:"); j=0;
                k=0; linn=lin;
                while (1) {
                        l=k=0;
                        for (i=0;i<MAXTCPPORT;i++) if (tcppb[i]>k) { k=tcppb[i]; l=i; }
                        if (k==0) break;
                        if (k>1024*1024) fprintf(f," %04d:%4.1fk/%-5.1f M",l,(float)tcppp[l]/1024,(float)k/(1024*1024));
                        else if (k>1024) fprintf(f," %04d:%4ld/%-5.1f k",l,tcppp[l],(float)k/1024);
                        else fprintf(f," %04d:%4ld/%-7ld",l,tcppp[l],k);
                        tcppb[l]=0;
                        ++j;
                }
	}

        for (i=0; i<MAXHASH; i++) {
                tai=acc + i;
                if ((tai->from!=0) && (tai->to!=0)) { ++k;
                if (!iSum) {
                        dstaddr.sin_addr.s_addr = htonl(*(iph+tai->from));
                        srcaddr.sin_addr.s_addr = htonl(*(iph+(tai->to)));
                        strcpy(str,inet_ntoa(dstaddr.sin_addr));
                        if (iScreen && (++lin<iline) ) printf("\n%-15s\t%-15s\t%5d pkt, %10ld byte :%7.2f Kbps",str,inet_ntoa(srcaddr.sin_addr),tai->pkt,tai->byte,((float)tai->byte)/ti/1024*8);
                        if (f) fprintf(f,"%-15s\t%-15s\t%d\t%ld\n",str,inet_ntoa(srcaddr.sin_addr),tai->pkt,tai->byte);
                }
                }
        }
      if (iScreen) printf("\n");
#ifdef LINUX
        if (iScreen) fflush(stdout);
#endif
        ClearIPAcc();
        if (f) {
                char cmdline[255];
                fclose(f);
//              if (*execname) _spawnle(_P_NOWAIT,execname,execname,filename);
//              if (*execname) _execl(execname,execname);
                if (*execname) {
#ifndef LINUX
                sprintf(cmdline,"%s %s",execname,filename);
#else
                sprintf(cmdline,"%s %s",execname,filename);
#endif
                system(cmdline);
//              iRun=0;
                }
        }
        f=NULL;
        return 0;
}

unsigned short FindIPHash( unsigned long ip ) {
        unsigned short hashval;
        unsigned long *ipt;

        hashval = (unsigned short)(((ip&0xFFFF0000)>>16) ^ (ip&0x0000FFFF));
        ipt=iph + hashval;
        while (*ipt != 0 && (*ipt!=ip)) { ipt++; hashval++; }
        if (*ipt==0) *ipt=ip;
        return hashval;
}

unsigned short SetIPAcc( unsigned long src, unsigned long dst, unsigned long byte, unsigned short typ, unsigned short sport, unsigned short dport) {
        unsigned short from,to,hash;
        ta *tai;
        hash=0;
        if (src) {

                if (fromip) from=FindIPHash(src); else from=-1;
                if (toip) to=FindIPHash(dst); else to=-1;
                hash=from^to;
                tai=acc + hash;
                while ( ((tai->from!=from) && (tai->to!=to)) && ((tai->from!=0) && (tai->to!=0)) ) {tai++; hash++; }
                if ((tai->from==0)&&(tai->to==0)) {
                        tai->byte=byte; tai->from=from; tai->to=to; tai->pkt=1;
                } else { tai->byte+=byte; tai->pkt++; }

                typp[typ]++;
                typb[typ]+=byte;
                if ((sport>0) && (sport<MAXTCPPORT)) { tcppp[sport]++; tcppb[sport]+=byte; }
                if ((dport>0) && (dport<MAXTCPPORT)) { tcppp[dport]++; tcppb[dport]+=byte; }
        }
        return hash;
}

//
// Function: DecodeIPHeader
//
// Description:
//    This function takes a pointer to an IP header and prints
//    it out in a readable form.
//
int DecodeIPHeader(WSABUF *wsabuf, unsigned int srcip, unsigned short srcport, unsigned long srcnet,
        unsigned int destip, unsigned short destport, unsigned long destnet, DWORD bytesret,
        unsigned short xport,unsigned int xip, unsigned long xnet)
{
    BYTE                        *hdr = (BYTE *)wsabuf->buf,
                                        *nexthdr = NULL,
                                        *ohdr;
    unsigned short      shortval;
    SOCKADDR_IN         srcaddr,
                                        destaddr;

    unsigned short ip_version,
                   ip_hdr_len,
                   ip_tos,
                   ip_total_len,
                   ip_id,
                   ip_flags,
                   ip_ttl,
                   ip_frag_offset,
                   ip_proto,
                   ip_hdr_chksum,
                   ip_src_port,
                   ip_dest_port;
    unsigned int   ip_src,
                   ip_dest;
    BOOL           bPrint = FALSE;
    char       ip_prtype=0;
    int     j;
    time_t tt;
    struct tm *tmm;

    ohdr=hdr;
    if (iLnxplus) ip_prtype=*(hdr+iLnxplus-1);
    if (ip_prtype) return 0;
    hdr += iLnxplus;
    ip_version = HI_WORD(*hdr);
    ip_hdr_len = LO_WORD(*hdr) * 4;
    nexthdr = (BYTE *)((BYTE *)hdr + ip_hdr_len);
    hdr++;

    ip_tos = *hdr;
    hdr++;

    memcpy(&shortval, hdr, 2);
    ip_total_len = ntohs(shortval);
    hdr += 2;

    memcpy(&shortval, hdr, 2);
    ip_id = ntohs(shortval);
    hdr += 2;

    ip_flags = ((*hdr) >> 5);

    memcpy(&shortval, hdr, 2);
    ip_frag_offset = ((ntohs(shortval)) & 0x1FFF);
    hdr += 2;

    ip_ttl = *hdr;
    hdr++;

    ip_proto = *hdr;
    hdr++;

    memcpy(&shortval, hdr, 2);
    ip_hdr_chksum = ntohs(shortval);
    hdr += 2;

    memcpy(&srcaddr.sin_addr.s_addr, hdr, 4);
    ip_src = ntohl(srcaddr.sin_addr.s_addr);
    hdr += 4;

    memcpy(&destaddr.sin_addr.s_addr, hdr, 4);
    ip_dest = ntohl(destaddr.sin_addr.s_addr);
    hdr += 4;
    //
    // If packet is UDP, TCP, or IGMP read ahead and
    //  get the port values.
    //
        ip_src_port=ip_dest_port=0;
    if (((ip_proto == 2) ||
         (ip_proto == 6) ||
         (ip_proto == 17)) ) //&& bFilter)
    {
        memcpy(&ip_src_port, nexthdr, 2);
        ip_src_port = ntohs(ip_src_port);
        memcpy(&ip_dest_port, nexthdr+2, 2);
        ip_dest_port = ntohs(ip_dest_port);

    };
    bPrint = 0;
//      xaok=   (xip!=0) && (((xip&xnet)==(ip_src&xnet))||((xip&xnet)==(ip_dest&xnet)));
//      saok= ((srcip==0)||((srcip&srcnet)==(ip_src&srcnet)));
//      daok = ((destip==0)||((destip&destnet)==(ip_dest&destnet)));
//      xpok=(xport!=0) && ((xport==ip_src_port)||(xport==ip_dest_port));
//      spok=((srcport==0)||(srcport == ip_src_port));
//      dpok=((destport==0)||(destport == ip_dest_port));
//printf("\nf:%d xa:%d sa:%d da:%d xp:%d sp:%d dp:%d",bFilter,xaok,saok,daok,xpok,spok,dpok);
//      if (!bFilter || ( (xaok||(saok&&daok)) && (xpok||(spok&&dpok)))) {
if ((!bFilter) || ((ip_proto==47)&&gre) ||
                (
                ((iProto==0)||(ip_proto==iProto)) &&
                 (
                  ((xip!=0) && (((xip&xnet)==(ip_src&xnet))||((xip&xnet)==(ip_dest&xnet)))
                  ) || (
                   ((srcip==0) || ((srcip&srcnet)==(ip_src&srcnet))) && ((destip==0)||((destip&destnet)==(ip_dest&destnet)))
                  )
                 )
                 &&
                 (
                  ((xport!=0) && ((xport==ip_src_port)||(xport==ip_dest_port))
                  ) || (
                   ((srcport==0)||(srcport == ip_src_port))&&((destport==0)||(destport == ip_dest_port))
                  )
                 )
                )
                ) {
                        if (! iDetail) {
                            if ((ip_proto==47)&&gre) {
                            *mypbuff=0;
                             DecodeGREHeader(wsabuf, ip_hdr_len, bytesret,
                    srcip,srcport,srcnet,destip,destport,destnet,xport,xip,xnet);
//                            SetIPAcc(0,0,0,0,0,0);
                            return ip_hdr_len;
                            }
                            SetIPAcc(ip_src,ip_dest,ip_total_len,ip_proto,ip_src_port,ip_dest_port);
                        }
                        else bPrint=TRUE;
//                        printf("%d %ld %ld %ld %ld",ip_proto,xip,xip&xnet,ip_src&xnet,ip_dest&xnet);
        } else {
                        if (! iDetail)          SetIPAcc(0,0,0,0,0,0);
//                      else bPrint=TRUE;
        }
        time(&tt);
        if ((!iSum && ( tt-elapsed > iCycle)) || !iRun || mostird) {
	    mostird=0;
                if (! iDetail) CloseIPAcc(tt-elapsed-1);
                else {
                        time(&elapsed);
                        if (f) fclose(f);
                        if (iFile) f=fopen(filename,"a");
                }
        }
        if (lNum) { if (--lNum <= 0) iRun=0; }

    //
    *pbuf=0;
    if (bPrint)    {
                tmm=localtime(&tt);
if (! nomac ) {
    strcpy(pbuf,mypbuff);
} else {
        sprintf(str,"\n%4.4d.%2.2d.%2.2d %2.2d:%2.2d:%2.2d ",
            tmm->tm_year+1900,tmm->tm_mon+1,tmm->tm_mday,tmm->tm_hour,tmm->tm_min,tmm->tm_sec);
        strcat(pbuf,str);
#ifdef LINUX
        sprintf(str,"%x:%x:%x:%x:%x:%x > %x:%x:%x:%x:%x:%x",*ohdr,*(ohdr+1),*(ohdr+2),*(ohdr+3),*(ohdr+4),*(ohdr+5),
            *(ohdr+6),*(ohdr+7),*(ohdr+8),*(ohdr+9),*(ohdr+10),*(ohdr+11));
        strcat(pbuf,str);
#endif
}
        sprintf(str,"  %d bytes\n%-15s>", ip_total_len, inet_ntoa(srcaddr.sin_addr));
        strcat(pbuf,str);
        sprintf(str,"%-15s", inet_ntoa(destaddr.sin_addr));
        strcat(pbuf,str);
        sprintf(str," TTL:%-3d Proto:%-6s F:%d/%d TOS:%X%X\n",
            ip_ttl, szProto[ip_proto],ip_flags,ip_frag_offset,HI_WORD(ip_tos), LO_WORD(ip_tos));
        strcat(pbuf,str);
        if (iFile) strcat(pbuf,".");
        strcpy(mypbuff,pbuf);

    }
    else        return ip_hdr_len;

    if (justheader) { if (*pbuf) fprintf(iFile?f:stdout,"%s",pbuf); return ip_hdr_len; }
        if (iDetail) {
                switch (ip_proto)    {
                        case 1:        // ICMP
                                j=DecodeICMPHeader(wsabuf, ip_hdr_len);
                                break;
                        case 2:        // IGMP
                                j=DecodeIGMPHeader(wsabuf, ip_hdr_len);
                                break;
                        case 6:        // TCP
                                j=DecodeTCPHeader(wsabuf, ip_hdr_len);
                                break;
                        case 17:       // UDP
                                j=DecodeUDPHeader(wsabuf, ip_hdr_len);
                                break;
                        case 47:       // UDP
                                j=DecodeGREHeader(wsabuf, ip_hdr_len, bytesret,
                    srcip,srcport,srcnet,destip,destport,destnet,xport,xip,xnet);
                                break;
                        default:
                                j=0;  hdr=(BYTE *)wsabuf->buf;
                                sprintf(str,"   No decoder installed for protocol\n");
                                strcat(pbuf,str);
                                break;
                }
                if (j>=0) PrintRawBytes(hdr+j,bytesret-j-ip_hdr_len-12); //(hdr-(BYTE *)(wsabuf->buf + iLnxplus)));
        }
        else if (*pbuf) fprintf(iFile?f:stdout,"%s",pbuf);

    return ip_hdr_len;
}
