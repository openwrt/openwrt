/*!***************************************************************************
*!
*! FILE NAME  : common.h
*!
*! DESCRIPTION: Common stuff for e100boot.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! 2002 05 02   Ronny Ranerup      Initial version
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999, 2000, 2001, 2002 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/**************************  Include files  ********************************/
/**************************  Constants and macros  *************************/
/* max length of argument strings */
#define MAX_STRING_LEN 256
/**************************  Type definitions  *****************************/
struct packet_buf {
  unsigned int size;
  struct packet_buf *next;
  unsigned int seq;
  char  *data;
  struct boot_files_T *boot_file;
  unsigned int baud_rate;
};

struct boot_files_T {
  char  *fileName;
  unsigned int  *size_p;		/* Pointer to size in boot_cmds. */
  unsigned int  size;			/* Size of file. */
  unsigned int  size_sent;		/* Number of bytes sent so far. */
  struct boot_files_T *next;
  FILE  *fd;
};

/**************************  Global variables  *****************************/
extern int db1;
extern int db2;
extern int db3;
extern int db4;

extern char device[MAX_STRING_LEN];
extern struct packet_buf *first_packet;
extern int cmdsOnly;	/* Just dump boot cmds to file. */
extern int toFiles;	/* Dump all packets to files. */

/**************************  Function prototypes  **************************/

struct packet_buf* create_packet (unsigned int seq);
struct packet_buf* free_packet	 (struct packet_buf *p);
void               find_string   (char *str);



