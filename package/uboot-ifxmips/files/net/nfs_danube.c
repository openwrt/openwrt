/*
 * NFS support driver - based on etherboot and U-BOOT's tftp.c
 *
 * Masami Komiya <mkomiya@sonare.it> 2004
 *
 */

/* NOTE: the NFS code is heavily inspired by the NetBSD netboot code (read:
 * large portions are copied verbatim) as distributed in OSKit 0.97.  A few
 * changes were necessary to adapt the code to Etherboot and to fix several
 * inconsistencies.  Also the RPC message preparation is done "by hand" to
 * avoid adding netsprintf() which I find hard to understand and use.  */

/* NOTE 2: Etherboot does not care about things beyond the kernel image, so
 * it loads the kernel image off the boot server (ARP_SERVER) and does not
 * access the client root disk (root-path in dhcpd.conf), which would use
 * ARP_ROOTSERVER.  The root disk is something the operating system we are
 * about to load needs to use.	This is different from the OSKit 0.97 logic.  */

/* NOTE 3: Symlink handling introduced by Anselm M Hoffmeister, 2003-July-14
 * If a symlink is encountered, it is followed as far as possible (recursion
 * possible, maximum 16 steps). There is no clearing of ".."'s inside the
 * path, so please DON'T DO THAT. thx. */

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>
#include "nfs.h"
#include "bootp.h"

/*#define NFS_DEBUG*/

#if ((CONFIG_COMMANDS & CFG_CMD_NET) && (CONFIG_COMMANDS & CFG_CMD_NFS))

#define HASHES_PER_LINE 65	/* Number of "loading" hashes per line	*/
#define NFS_TIMEOUT 60

static int fs_mounted = 0;
static unsigned long rpc_id = 0;
static int nfs_offset = -1;
static int nfs_len;

static char dirfh[NFS_FHSIZE];	/* file handle of directory */
static char filefh[NFS_FHSIZE]; /* file handle of kernel image */

static int	NfsDownloadState;
static IPaddr_t NfsServerIP;
static int	NfsSrvMountPort;
static int	NfsSrvNfsPort;
static int	NfsOurPort;
static int	NfsTimeoutCount;
static int	NfsState;
#define STATE_PRCLOOKUP_PROG_MOUNT_REQ	1
#define STATE_PRCLOOKUP_PROG_NFS_REQ	2
#define STATE_MOUNT_REQ			3
#define STATE_UMOUNT_REQ		4
#define STATE_LOOKUP_REQ		5
#define STATE_READ_REQ			6
#define STATE_READLINK_REQ		7

static char default_filename[64];
static char *nfs_filename;
static char *nfs_path;
static char nfs_path_buff[2048];

static __inline__ int
store_block (uchar * src, unsigned offset, unsigned len)
{
	ulong newsize = offset + len;
#ifdef CFG_DIRECT_FLASH_NFS
	int i, rc = 0;

	for (i=0; i<CFG_MAX_FLASH_BANKS; i++) {
		/* start address in flash? */
		if (load_addr + offset >= flash_info[i].start[0]) {
			rc = 1;
			break;
		}
	}

	if (rc) { /* Flash is destination for this packet */
		rc = flash_write ((uchar *)src, (ulong)(load_addr+offset), len);
		if (rc) {
			flash_perror (rc);
			return -1;
		}
	} else
#endif /* CFG_DIRECT_FLASH_NFS */
	{
		(void)memcpy ((void *)(load_addr + offset), src, len);
	}

	if (NetBootFileXferSize < (offset+len))
		NetBootFileXferSize = newsize;
	return 0;
}

static char*
basename (char *path)
{
	char *fname;

	fname = path + strlen(path) - 1;
	while (fname >= path) {
		if (*fname == '/') {
			fname++;
			break;
		}
		fname--;
	}
	return fname;
}

static char*
dirname (char *path)
{
	char *fname;

	fname = basename (path);
	--fname;
	*fname = '\0';
	return path;
}

/**************************************************************************
RPC_ADD_CREDENTIALS - Add RPC authentication/verifier entries
**************************************************************************/
static long *rpc_add_credentials (long *p)
{
	int hl;
	int hostnamelen;
	char hostname[256];

	strcpy (hostname, "");
	hostnamelen=strlen (hostname);

	/* Here's the executive summary on authentication requirements of the
	 * various NFS server implementations:	Linux accepts both AUTH_NONE
	 * and AUTH_UNIX authentication (also accepts an empty hostname field
	 * in the AUTH_UNIX scheme).  *BSD refuses AUTH_NONE, but accepts
	 * AUTH_UNIX (also accepts an empty hostname field in the AUTH_UNIX
	 * scheme).  To be safe, use AUTH_UNIX and pass the hostname if we have
	 * it (if the BOOTP/DHCP reply didn't give one, just use an empty
	 * hostname).  */

	hl = (hostnamelen + 3) & ~3;

	/* Provide an AUTH_UNIX credential.  */
	*p++ = htonl(1);		/* AUTH_UNIX */
	*p++ = htonl(hl+20);		/* auth length */
	*p++ = htonl(0);		/* stamp */
	*p++ = htonl(hostnamelen);	/* hostname string */
	if (hostnamelen & 3) {
		*(p + hostnamelen / 4) = 0; /* add zero padding */
	}
	memcpy (p, hostname, hostnamelen);
	p += hl / 4;
	*p++ = 0;			/* uid */
	*p++ = 0;			/* gid */
	*p++ = 0;			/* auxiliary gid list */

	/* Provide an AUTH_NONE verifier.  */
	*p++ = 0;			/* AUTH_NONE */
	*p++ = 0;			/* auth length */

	return p;
}

/**************************************************************************
RPC_LOOKUP - Lookup RPC Port numbers
**************************************************************************/
static void
rpc_req (int rpc_prog, int rpc_proc, uint32_t *data, int datalen)
{
	struct rpc_t pkt;
	unsigned long id;
	uint32_t *p;
	int pktlen;
	int sport;

	id = ++rpc_id;
	pkt.u.call.id = htonl(id);
	pkt.u.call.type = htonl(MSG_CALL);
	pkt.u.call.rpcvers = htonl(2);	/* use RPC version 2 */
	pkt.u.call.prog = htonl(rpc_prog);
	pkt.u.call.vers = htonl(2);	/* portmapper is version 2 */
	pkt.u.call.proc = htonl(rpc_proc);
	p = (uint32_t *)&(pkt.u.call.data);

	if (datalen)
		memcpy ((char *)p, (char *)data, datalen*sizeof(uint32_t));

	pktlen = (char *)p + datalen*sizeof(uint32_t) - (char *)&pkt;

	memcpy ((char *)NetTxPacket + NetEthHdrSize() + IP_HDR_SIZE, (char *)&pkt, pktlen);

	if (rpc_prog == PROG_PORTMAP)
		sport = SUNRPC_PORT;
	else if (rpc_prog == PROG_MOUNT)
		sport = NfsSrvMountPort;
	else
		sport = NfsSrvNfsPort;

	NetSendUDPPacket (NetServerEther, NfsServerIP, sport, NfsOurPort, pktlen);
}

/**************************************************************************
RPC_LOOKUP - Lookup RPC Port numbers
**************************************************************************/
static void
rpc_lookup_req (int prog, int ver)
{
	uint32_t data[16];

	data[0] = 0; data[1] = 0;	/* auth credential */
	data[2] = 0; data[3] = 0;	/* auth verifier */
	data[4] = htonl(prog);
	data[5] = htonl(ver);
	data[6] = htonl(17);	/* IP_UDP */
	data[7] = 0;

	rpc_req (PROG_PORTMAP, PORTMAP_GETPORT, data, 8);
}

/**************************************************************************
NFS_MOUNT - Mount an NFS Filesystem
**************************************************************************/
static void
nfs_mount_req (char *path)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	int pathlen;

	pathlen = strlen (path);

	p = &(data[0]);
	p = (uint32_t *)rpc_add_credentials((long *)p);

	*p++ = htonl(pathlen);
	if (pathlen & 3) *(p + pathlen / 4) = 0;
	memcpy (p, path, pathlen);
	p += (pathlen + 3) / 4;

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req (PROG_MOUNT, MOUNT_ADDENTRY, data, len);
}

/**************************************************************************
NFS_UMOUNTALL - Unmount all our NFS Filesystems on the Server
**************************************************************************/
static void
nfs_umountall_req (void)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	if ((NfsSrvMountPort == -1) || (!fs_mounted)) {
		/* Nothing mounted, nothing to umount */
		return;
	}

	p = &(data[0]);
	p = (uint32_t *)rpc_add_credentials ((long *)p);

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req (PROG_MOUNT, MOUNT_UMOUNTALL, data, len);
}

/***************************************************************************
 * NFS_READLINK (AH 2003-07-14)
 * This procedure is called when read of the first block fails -
 * this probably happens when it's a directory or a symlink
 * In case of successful readlink(), the dirname is manipulated,
 * so that inside the nfs() function a recursion can be done.
 **************************************************************************/
static void
nfs_readlink_req (void)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	p = &(data[0]);
	p = (uint32_t *)rpc_add_credentials ((long *)p);

	memcpy (p, filefh, NFS_FHSIZE);
	p += (NFS_FHSIZE / 4);

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req (PROG_NFS, NFS_READLINK, data, len);
}

/**************************************************************************
NFS_LOOKUP - Lookup Pathname
**************************************************************************/
static void
nfs_lookup_req (char *fname)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	int fnamelen;

	fnamelen = strlen (fname);

	p = &(data[0]);
	p = (uint32_t *)rpc_add_credentials ((long *)p);

	memcpy (p, dirfh, NFS_FHSIZE);
	p += (NFS_FHSIZE / 4);
	*p++ = htonl(fnamelen);
	if (fnamelen & 3) *(p + fnamelen / 4) = 0;
	memcpy (p, fname, fnamelen);
	p += (fnamelen + 3) / 4;

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req (PROG_NFS, NFS_LOOKUP, data, len);
}

/**************************************************************************
NFS_READ - Read File on NFS Server
**************************************************************************/
static void
nfs_read_req (int offset, int readlen)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	p = &(data[0]);
	p = (uint32_t *)rpc_add_credentials ((long *)p);

	memcpy (p, filefh, NFS_FHSIZE);
	p += (NFS_FHSIZE / 4);
	*p++ = htonl(offset);
	*p++ = htonl(readlen);
	*p++ = 0;

	len = (uint32_t *)p - (uint32_t *)&(data[0]);

	rpc_req (PROG_NFS, NFS_READ, data, len);
}

/**************************************************************************
RPC request dispatcher
**************************************************************************/

static void
NfsSend (void)
{
#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	switch (NfsState) {
	case STATE_PRCLOOKUP_PROG_MOUNT_REQ:
		rpc_lookup_req (PROG_MOUNT, 1);
		break;
	case STATE_PRCLOOKUP_PROG_NFS_REQ:
		rpc_lookup_req (PROG_NFS, 2);
		break;
	case STATE_MOUNT_REQ:
		nfs_mount_req (nfs_path);
		break;
	case STATE_UMOUNT_REQ:
		nfs_umountall_req ();
		break;
	case STATE_LOOKUP_REQ:
		nfs_lookup_req (nfs_filename);
		break;
	case STATE_READ_REQ:
		nfs_read_req (nfs_offset, nfs_len);
		break;
	case STATE_READLINK_REQ:
		nfs_readlink_req ();
		break;
	}
}

/**************************************************************************
Handlers for the reply from server
**************************************************************************/

static int
rpc_lookup_reply (int prog, uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

	memcpy ((unsigned char *)&rpc_pkt, pkt, len);

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.astatus) {
		return -1;
	}

	switch (prog) {
	case PROG_MOUNT:
		NfsSrvMountPort = ntohl(rpc_pkt.u.reply.data[0]);
		break;
	case PROG_NFS:
		NfsSrvNfsPort = ntohl(rpc_pkt.u.reply.data[0]);
		break;
	}

	return 0;
}

static int
nfs_mount_reply (uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	memcpy ((unsigned char *)&rpc_pkt, pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0]) {
		return -1;
	}

	fs_mounted = 1;
	memcpy (dirfh, rpc_pkt.u.reply.data + 1, NFS_FHSIZE);

	return 0;
}

static int
nfs_umountall_reply (uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	memcpy ((unsigned char *)&rpc_pkt, pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus) {
		return -1;
	}

	fs_mounted = 0;
	memset (dirfh, 0, sizeof(dirfh));

	return 0;
}

static int
nfs_lookup_reply (uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	memcpy ((unsigned char *)&rpc_pkt, pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0]) {
		return -1;
	}

	memcpy (filefh, rpc_pkt.u.reply.data + 1, NFS_FHSIZE);

	return 0;
}

static int
nfs_readlink_reply (uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;
	int rlen;

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	memcpy ((unsigned char *)&rpc_pkt, pkt, len);

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0]) {
		return -1;
	}

	rlen = ntohl (rpc_pkt.u.reply.data[1]); /* new path length */

	if (*((char *)&(rpc_pkt.u.reply.data[2])) != '/') {
		int pathlen;
		strcat (nfs_path, "/");
		pathlen = strlen(nfs_path);
		memcpy (nfs_path+pathlen, (uchar *)&(rpc_pkt.u.reply.data[2]), rlen);
		nfs_path[pathlen+rlen+1] = 0;
	} else {
		memcpy (nfs_path, (uchar *)&(rpc_pkt.u.reply.data[2]), rlen);
		nfs_path[rlen] = 0;
	}
	return 0;
}

static int
nfs_read_reply (uchar *pkt, unsigned len)
{
	struct rpc_t rpc_pkt;
	int rlen;

#ifdef NFS_DEBUG_nop
	printf ("%s\n", __FUNCTION__);
#endif

	memcpy ((uchar *)&rpc_pkt, pkt, sizeof(rpc_pkt.u.reply));

	if (ntohl(rpc_pkt.u.reply.id) != rpc_id)
		return -1;

	if (rpc_pkt.u.reply.rstatus  ||
	    rpc_pkt.u.reply.verifier ||
	    rpc_pkt.u.reply.astatus  ||
	    rpc_pkt.u.reply.data[0]) {
		if (rpc_pkt.u.reply.rstatus) {
			return -9999;
		}
		if (rpc_pkt.u.reply.astatus) {
			return -9999;
		}
		return -ntohl(rpc_pkt.u.reply.data[0]);;
	}

	if ((nfs_offset!=0) && !((nfs_offset) % (NFS_READ_SIZE/2*10*HASHES_PER_LINE))) {
		puts ("\n\t ");
	}
	if (!(nfs_offset % ((NFS_READ_SIZE/2)*10))) {
		putc ('#');
	}

	rlen = ntohl(rpc_pkt.u.reply.data[18]);
	if ( store_block ((uchar *)pkt+sizeof(rpc_pkt.u.reply), nfs_offset, rlen) )
		return -9999;

	return rlen;
}

/**************************************************************************
Interfaces of U-BOOT
**************************************************************************/

static void
NfsTimeout (void)
{
	puts ("Timeout\n");
	NetState = NETLOOP_FAIL;
	return;
}

static void
NfsHandler (uchar *pkt, unsigned dest, unsigned src, unsigned len)
{
	int rlen;

#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif

	if (dest != NfsOurPort) return;

	switch (NfsState) {
	case STATE_PRCLOOKUP_PROG_MOUNT_REQ:
		rpc_lookup_reply (PROG_MOUNT, pkt, len);
		NfsState = STATE_PRCLOOKUP_PROG_NFS_REQ;
		NfsSend ();
		break;

	case STATE_PRCLOOKUP_PROG_NFS_REQ:
		rpc_lookup_reply (PROG_NFS, pkt, len);
		NfsState = STATE_MOUNT_REQ;
		NfsSend ();
		break;

	case STATE_MOUNT_REQ:
		if (nfs_mount_reply(pkt, len)) {
			puts ("*** ERROR: Cannot mount\n");
			/* just to be sure... */
			NfsState = STATE_UMOUNT_REQ;
			NfsSend ();
		} else {
			NfsState = STATE_LOOKUP_REQ;
			NfsSend ();
		}
		break;

	case STATE_UMOUNT_REQ:
		if (nfs_umountall_reply(pkt, len)) {
			puts ("*** ERROR: Cannot umount\n");
			NetState = NETLOOP_FAIL;
		} else {
			puts ("\ndone\n");
			NetState = NfsDownloadState;
		}
		break;

	case STATE_LOOKUP_REQ:
		if (nfs_lookup_reply(pkt, len)) {
			puts ("*** ERROR: File lookup fail\n");
			NfsState = STATE_UMOUNT_REQ;
			NfsSend ();
		} else {
			NfsState = STATE_READ_REQ;
			nfs_offset = 0;
			nfs_len = NFS_READ_SIZE;
			NfsSend ();
		}
		break;

	case STATE_READLINK_REQ:
		if (nfs_readlink_reply(pkt, len)) {
			puts ("*** ERROR: Symlink fail\n");
			NfsState = STATE_UMOUNT_REQ;
			NfsSend ();
		} else {
#ifdef NFS_DEBUG
			printf ("Symlink --> %s\n", nfs_path);
#endif
			nfs_filename = basename (nfs_path);
			nfs_path     = dirname (nfs_path);

			NfsState = STATE_MOUNT_REQ;
			NfsSend ();
		}
		break;

	case STATE_READ_REQ:
		rlen = nfs_read_reply (pkt, len);
		NetSetTimeout (NFS_TIMEOUT * CFG_HZ, NfsTimeout);
		if (rlen > 0) {
			nfs_offset += rlen;
			NfsSend ();
		}
		else if ((rlen == -NFSERR_ISDIR)||(rlen == -NFSERR_INVAL)) {
			/* symbolic link */
			NfsState = STATE_READLINK_REQ;
			NfsSend ();
		} else {
			if ( ! rlen ) NfsDownloadState = NETLOOP_SUCCESS;
			NfsState = STATE_UMOUNT_REQ;
			NfsSend ();
		}
		break;
	}
}


void
NfsStart (void)
{
#ifdef NFS_DEBUG
	printf ("%s\n", __FUNCTION__);
#endif
	NfsDownloadState = NETLOOP_FAIL;

	NfsServerIP = NetServerIP;
	nfs_path = (char *)nfs_path_buff;

	if (nfs_path == NULL) {
		NetState = NETLOOP_FAIL;
		puts ("*** ERROR: Fail allocate memory\n");
		return;
	}

	if (BootFile[0] == '\0') {
		sprintf (default_filename, "/nfsroot/%02lX%02lX%02lX%02lX.img",
			NetOurIP & 0xFF,
			(NetOurIP >>  8) & 0xFF,
			(NetOurIP >> 16) & 0xFF,
			(NetOurIP >> 24) & 0xFF	);
		strcpy (nfs_path, default_filename);

		printf ("*** Warning: no boot file name; using '%s'\n",
			nfs_path);
	} else {
		char *p=BootFile;

		p = strchr (p, ':');

		if (p != NULL) {
			NfsServerIP = string_to_ip (BootFile);
			++p;
			strcpy (nfs_path, p);
		} else {
			strcpy (nfs_path, BootFile);
		}
	}

	nfs_filename = basename (nfs_path);
	nfs_path     = dirname (nfs_path);

#if defined(CONFIG_NET_MULTI)
	printf ("Using %s device\n", eth_get_name());
#endif

	puts ("File transfer via NFS from server "); print_IPaddr (NfsServerIP);
	puts ("; our IP address is ");		    print_IPaddr (NetOurIP);

	/* Check if we need to send across this subnet */
	if (NetOurGatewayIP && NetOurSubnetMask) {
		IPaddr_t OurNet	    = NetOurIP	  & NetOurSubnetMask;
		IPaddr_t ServerNet  = NetServerIP & NetOurSubnetMask;

		if (OurNet != ServerNet) {
			puts ("; sending through gateway ");
			print_IPaddr (NetOurGatewayIP) ;
		}
	}
	printf ("\nFilename '%s/%s'.", nfs_path, nfs_filename);

	if (NetBootFileSize) {
		printf (" Size is 0x%x Bytes = ", NetBootFileSize<<9);
		print_size (NetBootFileSize<<9, "");
	}
	printf ("\nLoad address: 0x%lx\n"
		"Loading: *\b", load_addr);

	NetSetTimeout (NFS_TIMEOUT * CFG_HZ, NfsTimeout);
	NetSetHandler (NfsHandler);

	NfsTimeoutCount = 0;
	NfsState = STATE_PRCLOOKUP_PROG_MOUNT_REQ;

	/*NfsOurPort = 4096 + (get_ticks() % 3072);*/
	/*FIX ME !!!*/
	NfsOurPort = 1000;

	/* zero out server ether in case the server ip has changed */
	memset (NetServerEther, 0, 6);

	NfsSend ();
}

#endif /* CONFIG_COMMANDS & CFG_CMD_NFS */
