/*!***************************************************************************
*!
*! FILE NAME  : common.c
*!
*! DESCRIPTION: Common functions for e100boot.
*!
*! ---------------------------------------------------------------------------
*! HISTORY
*!
*! DATE         NAME               CHANGES
*! ----         ----               -------
*! ???  ? ????  Ronny Ranerup      Initial version
*! ---------------------------------------------------------------------------
*! (C) Copyright 1999-2003 Axis Communications AB, LUND, SWEDEN
*!***************************************************************************/
/* $Id: common.c,v 1.1 2004/01/26 10:52:40 mikaelp Exp $ */

/**************************  Version  **************************************/

char version[] = "Time-stamp: $Id: common.c,v 1.1 2004/01/26 10:52:40 mikaelp Exp $";

/**************************  Include files  ********************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <winsock2.h>

#include <e100boot.h>
#include <pcap.h>
#include "boot_images.h"
#include <reg_des.h>
#include <sv_addr_ag.h>
#include <common.h>

#include <serial.h>
#include <network.h>

/**************************  Constants and macros  *************************/

#define BOOT_CMDS_FILE "BOOT_CMDS"

/**************************  Type definitions  *****************************/

struct label_t {
  struct label_t *prev;
  udword addr;
  char *name;
};

/**************************  Global variables  *****************************/

char needle[MAX_STRING_LEN] = "END";    /* search needle for --find */
int needle_len = 3;

int exitonfind = FALSE;

int doing_flash = FALSE;        /* Just a flag to see if we should
                                   warn that it might take some
                                   time. */

int toFiles             = FALSE;        /* Dump all packets to files. */
int cmdsOnly            = FALSE;        /* Just dump boot cmds to file. */
int netBoot             = TRUE;         /* Do network boot? */
int serBoot             = FALSE;        /* Do serial boot? */

struct label_t *loop_label = NULL;
struct label_t *label      = NULL;

struct packet_buf *first_packet = NULL;
struct packet_buf *last_packet  = NULL;

struct packet_buf *boot_cmds_packet;

int create_boot_loader = TRUE;

/* We use the INTERNAL boot image as default */
char boot_loader_file[MAX_STRING_LEN] = INTERNAL_NW;

int noleds = FALSE;                     /* Use boot images that doesn't toggle leds? */

struct boot_files_T *first_boot_file = NULL;
struct boot_files_T *last_boot_file = NULL;

unsigned int boot_cmds[SIZE_OF_BOOT_CMDS / sizeof(unsigned int)];
int boot_cmds_cnt = 0;

int svboot = FALSE;

int quiet = 0;

struct packet_header_T send_packet;
struct packet_header_T *receive_packet;

int seq_nr = 0;

/* debug flags */
int db1 = FALSE;
int db2 = FALSE;
int db3 = FALSE;
int db4 = FALSE;

char device[MAX_STRING_LEN] = "";


/**************************  Function prototypes  **************************/

FILE                   *Fopen                   (const char *name, const char *mode);
int                     main                    (int argc, const char *argv[]);
int                     GetNumberOption         (int *argNr, int argCount, const char *argVect[], unsigned int *ret, const char *option, int base);
int                     GetStringOption         (int *argNr, int argCount, const char *argVect[], char *ret, const char *option);
int                     GetRegisterOption       (int *argNr, int argCount, const char *argVect[], unsigned int *ret, const char *option, int base);
struct boot_files_T*    allocate_boot_file      (struct boot_files_T *bf);
struct packet_buf*      CreateNewBootPacket     (void);
struct packet_buf*      allocate_packet         (struct packet_buf *p);
void                    SigHandler              (int sig);
void                    CreateBootLoader        (void);
void                    FinishBootCmds          (void);
void                    ParseArgs               (int argc, const char *argv[]);
void                    PrintHelp               (void);
void                    CreateBootCmds          (void);
void                    handle_labels           (void);
void                    new_label               (struct label_t **label, udword addr, const char *name);

/****************************************************************************
*#
*#  FUNCTION NAME: main
*#
*#  PARAMETERS: Command line arguments.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

int
main(int argc, const char *argv[])
{
  ParseArgs(argc, argv);

  if (cmdsOnly) {
    /* We make this a special case to avoid messing up other code. */
    CreateBootCmds();
    FinishBootCmds();
    SendToDevice(&boot_cmds_packet->data[SIZE_OF_HEADER],SIZE_OF_BOOT_CMDS);
    exit(EXIT_SUCCESS);
  }

  if (netBoot && !toFiles) {
    /* Do not bother with this if we just want to write the packets to
       files. Means you do not have to be root to run --tofiles. */
    net_init();
  }
  else if (serBoot) {
    /*if (!SetupSerial())*/ {
      exit(0);
    }
  }

  CreateBootLoader();
  CreateBootCmds();
  FinishBootCmds();

  printf("Starting boot...\n");
  if (doing_flash) {
    printf("We're doing a flash write, this may take up to a few minutes...\n");
  }

  if (toFiles) {
    udword seq = 0;
    struct packet_buf *p;

    while((p = create_packet(seq++))) {
      SendToDevice(p->data, p->size);
    }

    exit(EXIT_SUCCESS);
  }

  if (netBoot) {
    NetBoot();
  }
  else if (serBoot) {
    /*SerBoot();*/
  }

  printf("Done.\n");
  return EXIT_SUCCESS;
} /* main  */

/****************************************************************************
*#
*#  FUNCTION NAME: free_packet
*#
*#  PARAMETERS: struct to free
*#
*#  DESCRIPTION: Frees struct and data in struct.
*#
*#  RETURNS: Pointer to next struct.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2000 02 07   ronny    Initial version
*#
*#***************************************************************************/

struct packet_buf*
free_packet(struct packet_buf *p)
{
  struct packet_buf *next_p;

  next_p = p->next;
  free(p->data);
  free(p);
  return(next_p);
}

/****************************************************************************
*#
*#  FUNCTION NAME: create_packet
*#
*#  PARAMETERS: Sequence number of desired packet.
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE         NAME     CHANGES
*#  ----         ----     -------
*#  2000 06 28   ronny    Initial version
*#
*#***************************************************************************/

struct packet_buf*
create_packet(udword seq)
{
  struct packet_buf *p = first_packet;
  /* Should check last first? */

  if (db4) printf("> create_packet seq %d\n", seq);

  while (p) {
    if (p->seq == seq) {
      return(p);
    }
    p = p->next;
  }

  return(CreateNewBootPacket());
}

/****************************************************************************
*#
*#  FUNCTION NAME: find_string
*#
*#  PARAMETERS: New string to search.
*#
*#  DESCRIPTION: Searches a number of strings for needle[], including strings
*#  overlapping between different calls.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  020502   ronny    Initial version
*#
*#***************************************************************************/

void
find_string(char *str)
{
  static int matched = 0;
  int hs[MAX_STRING_LEN];
  static int cur_hs = 0;
  static int hs_len = 0;
  static int last_hs = 0;

  static int cur_needle = 0;

  if (!needle[0]) {
    return;
  }
  //    printf("# adding >%s<\n", str);

  {
    int c = 0;
    int s = 0;

    while((c = str[s])) {
      //    printf("\n# cur_hs %d, hs_len %d\n", cur_hs, hs_len);
      {
        int i;

        for(i = 0; i != hs_len; i++) {
          //      printf("hs[%d] = %d(%c)\n", i, (int)hs[i], hs[i] < 32 ? 'X' : hs[i]);
        }
      }

      if (cur_hs == hs_len) {
        //      printf("adding char %d(%c) at hs[%d]\n", (int)c, c < 32 ? 'X' : c, hs_len);
        hs[hs_len] = c;
        hs_len++;
        s++;
      }

      //      printf("testing %d at cur_hs %d against %d at cur_needle  %d\n",
      //             (int)hs[cur_hs], cur_hs, (int)needle[cur_needle], cur_needle);

      if (hs[cur_hs] == needle[cur_needle]) {
        if (cur_needle+1 == needle_len) {
          int exitcode = EXIT_SUCCESS;

          //      printf("\nFound needle from --find option.\n");
          if (exitonfind) {
            int ret;
            //      printf("scanf (s=%d)'%s'\n", s, &str[s+1]);
            ret = sscanf(&str[s+1], "%i", &exitcode);
            //      printf("ret %d, '%s'\n", ret, &str[s+1]);
          }
          printf("Exiting with code %d\n", exitcode);
          exit(exitcode);
        }
        cur_needle++;
        cur_hs++;
      }
      else {
        //      printf("no match, shifting hs.\n");
        {
          int i;
          for(i=0; i!= hs_len-1; i++) {
            hs[i] = hs[i+1];
          }
        }
        hs_len--;
        cur_needle = 0;
        cur_hs = 0;
      }
    }
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: Fopen
*#
*#  PARAMETERS: Name and mode, both strings.
*#
*#  DESCRIPTION: Opens a file and returns its fd, or NULL.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  961022   ronny    Initial version
*#
*#***************************************************************************/

FILE *
Fopen(const char *name, const char *mode)
{
  FILE *fd;

  if (db2) printf(">>> Fopen '%s', mode '%s'\n", name, mode);

  fd = fopen(name, mode);

  if (fd == NULL) {
    printf("<<< Fopen failed on '%s', mode '%s'\n", name, mode);
    return ((FILE*) NULL);
  }

  if (strncmp(mode, "a", 1) == 0) {
    if (db3) printf("* Append mode, seeking to end.\n");
    fseek(fd, 0L, SEEK_SET);
  }

  if (db2) printf("<<< Fopen: '%s'\n", name);

  return(fd);
}

/****************************************************************************
*#
*#  FUNCTION NAME: ParseArgs
*#
*#  PARAMETERS: Standard command line args.
*#
*#  DESCRIPTION: Parses command line arguments.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

void
ParseArgs (int argc, const char *argv[])
{
  int  argi;
  int  i;
  int  printHelp = FALSE;
  int  exitCode = EXIT_SUCCESS;
  char dbStr[MAX_STRING_LEN];              /* Debug option string. */
  int  number;
  int  argCount;
  const char **argVect;
  struct stat st;

  if (db4) printf(">>> ParseArgs\n");
  argCount = argc;
  argVect = argv;

  for (argi = 1; argi < argCount; argi++) {
    if (db4) printf("argv[%d] = '%s'\n", argi, argVect[argi]);
    if (strncmp(argVect[argi], "--from", 6) == 0) {
      if (GetStringOption(&argi, argCount, argVect, host1, "--from") == 0) {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
      else {
        printf("Host: %s %s\n", host1, host2);
        if (sscanf(host1, "%x-%x-%x-%x-%x-%x", &i, &i, &i, &i, &i, &i) == 6) {
          printf("Ethernet address\n");
        }
      }
    }

    else if (strncmp(argVect[argi], "--device", 8) == 0) {
      if (GetStringOption(&argi, argCount, argVect, device, "--device") == 0) {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
    }

    else if (strncmp(argVect[argi], "--network", 9) == 0) {
      netBoot = TRUE;
      serBoot = FALSE;
      strcpy(boot_loader_file, INTERNAL_NW);
    }

    else if (strncmp(argVect[argi], "--serial", 8) == 0) {
      serBoot = TRUE;
      netBoot = FALSE;
      strcpy(device, "/dev/ttyS0");
      strcpy(boot_loader_file, INTERNAL_SER);
    }

    else if (strncmp(argVect[argi], "--noleds", 8) == 0) {
      noleds = TRUE;
    }

    else if (strncmp(argVect[argi], "--images", 8) == 0) {
      int i = 0;

      printf("Internal images:\n");

      while(boot_image_info[i].name) {
        printf("'%s', %s, size %lu bytes.\n",
               boot_image_info[i].name,
               boot_image_info[i].info,
               boot_image_info[i].len
               );
        i++;
      }
      exit(EXIT_SUCCESS);
    }

    else if (strncmp(argv[argi], "--baudrate", 10) == 0) {
      /*      if (GetNumberOption (&argi, argCount, argVect, &set_baudrate, "--baudrate", 10) == 0)*/ {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
    }

    else if (strncmp(argVect[argi], "--tofiles", 9) == 0) {
      toFiles = TRUE;
    }

    else if (strncmp(argVect[argi], "--cmdsonly", 10) == 0) {
      cmdsOnly = TRUE;
    }

    else if (strncmp(argVect[argi], "--to", 4) == 0) {
      if ((GetStringOption(&argi, argCount, argVect, host2, "--to") == 0)) {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
      else {
        printf("Host: %s %s\n", host1, host2);
        both_addresses = TRUE;
        if (sscanf(host2, "%x-%x-%x-%x-%x-%x", &i, &i, &i, &i, &i, &i) == 6) {
          printf("Ethernet address\n");
        }
      }
    }

    else if (strncmp(argVect[argi], "--printp", 8) == 0) {
      pPacket = 1;
    }

    else if (strncmp(argVect[argi], "--printascii", 11) == 0) {
      pPacket = 1;
      printPacketType = ASCII;
    }

    else if (strncmp(argVect[argi], "--printudec", 11) == 0) {
      pPacket = 1;
      printPacketType = UDEC;
    }

    else if (strncmp(argVect[argi], "--printhex", 10) == 0) {
      pPacket = 1;
      printPacketType = HEX;
    }

    else if (strncmp(argVect[argi], "--bpl", 5) == 0) {
      if (GetNumberOption(&argi, argCount, argVect, &p_packet_bpl, "--bpl", 10) == 0) {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
    }

    else if (strncmp(argVect[argi], "--promisc", 11) == 0) {
      promisc = 1;
    }

    else if (strncmp(argVect[argi], "--5400", 6) == 0) {
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb0000000;
      boot_cmds[boot_cmds_cnt++] = 0x000095b6;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb0000008;
      boot_cmds[boot_cmds_cnt++] = 0x0000e751;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb000000c;
      boot_cmds[boot_cmds_cnt++] = 0x12604040;
    }

    else if (strncmp(argVect[argi], "--5600", 6) == 0) {
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb0000000;
      boot_cmds[boot_cmds_cnt++] = 0x000095b6;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb0000008;
      boot_cmds[boot_cmds_cnt++] = 0x00006751;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = 0xb000000c;
      boot_cmds[boot_cmds_cnt++] = 0x12204040;
    }

    else if (strncmp(argVect[argi], "--testcardlx", 12) == 0) {
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_CONFIG;
      boot_cmds[boot_cmds_cnt++] = 0x12604040;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_TIMING;
      boot_cmds[boot_cmds_cnt++] = 0x0000e751;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_WAITSTATES;
      boot_cmds[boot_cmds_cnt++] = 0x00b395b5;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_BUS_CONFIG;
      boot_cmds[boot_cmds_cnt++] = 0x00000207;
    }

    else if (strncmp(argVect[argi], "--testcard", 10) == 0) {
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_WAITSTATES;
      boot_cmds[boot_cmds_cnt++] = 0x000010b3;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_TIMING;
      boot_cmds[boot_cmds_cnt++] = 0x00006543;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_CONFIG;
      boot_cmds[boot_cmds_cnt++] = 0x12966060;
    }

    else if (strncmp(argVect[argi], "--devboard", 10) == 0) {
      /* Printing on serial port will not work until PORT_PB is set... */
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_PORT_PB_SET;
      boot_cmds[boot_cmds_cnt++] = 0x01001ef3;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_PORT_PA_SET;
      boot_cmds[boot_cmds_cnt++] = 0x00001df0;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_WAITSTATES;
      boot_cmds[boot_cmds_cnt++] = 0x000095a6;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_CONFIG;
      boot_cmds[boot_cmds_cnt++] = 0x1a200040;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_DRAM_TIMING;
      boot_cmds[boot_cmds_cnt++] = 0x00005611;
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      boot_cmds[boot_cmds_cnt++] = (unsigned int)R_BUS_CONFIG;
      boot_cmds[boot_cmds_cnt++] = 0x00000104;
    }

    else if (strncmp(argVect[argi], "--verify", 8) == 0) {
      boot_cmds[boot_cmds_cnt++] = MEM_VERIFY;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--verify", 16);
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--verify", 16);
    }

    else if (strncmp(argVect[argi], "--setreg", 8) == 0) {
      boot_cmds[boot_cmds_cnt++] = SET_REGISTER;
      if ((argVect[argi+1][0] >= 'A') && (argVect[argi+1][0] <= 'Z')) {
        GetRegisterOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--setreg", 16);
      }
      else {
        GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--setreg", 16);
      }
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--setreg", 16);
    }

    else if (strncmp(argVect[argi], "--getreg", 8) == 0) {
      boot_cmds[boot_cmds_cnt++] = GET_REGISTER;
      if ((argVect[argi+1][0] >= 'A') && (argVect[argi+1][0] <= 'Z')) {
        GetRegisterOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--getreg", 16);
      }
      else {
        GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--getreg", 16);
      }
    }

    else if (strncmp(argVect[argi], "--pause", 7) == 0) {
      boot_cmds[boot_cmds_cnt++] = PAUSE_LOOP;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--pause", 16);
    }

    else if (strncmp(argVect[argi], "--memtest", 9) == 0) {
      boot_cmds[boot_cmds_cnt++] = MEM_TEST;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memtest", 16);
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memtest", 16);
    }

    else if (strncmp(argVect[argi], "--loop", 6) == 0) {
      char str[MAX_STRING_LEN];
      boot_cmds[boot_cmds_cnt++] = LOOP;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--loop", 16);
      GetStringOption(&argi, argCount, argVect, str, argVect[argi]);
      new_label(&loop_label, boot_cmds_cnt+1, str);
      boot_cmds_cnt++;
    }

    else if (strncmp(argVect[argi], "--label", 7) == 0) {
      char str[MAX_STRING_LEN];
      GetStringOption(&argi, argCount, argVect, str, "--label");
      new_label(&label, boot_cmds_cnt, str);
    }

    else if (strncmp(argVect[argi], "--memdump", 9) == 0) {
      boot_cmds[boot_cmds_cnt++] = MEM_DUMP;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memdump", 16);
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memdump", 16);
    }

    else if (strncmp(argVect[argi], "--memclear", 10) == 0) {
      boot_cmds[boot_cmds_cnt++] = MEM_CLEAR;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memclear", 16);
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--memclear", 16);
    }

    else if (strncmp(argVect[argi], "--flash", 7) == 0) {
            boot_cmds[boot_cmds_cnt++] = FLASH;
            GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--flash", 16);
            GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--flash", 16);
            GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--flash", 16);
            doing_flash = TRUE;
    }

    else if (strncmp(argVect[argi], "--jump", 6) == 0) {
      boot_cmds[boot_cmds_cnt++] = JUMP;
      GetNumberOption(&argi, argCount, argVect, &boot_cmds[boot_cmds_cnt++], "--jump", 16);
    }

    else if (strncmp(argVect[argi], "--file", 6) == 0) {
      char str[MAX_STRING_LEN];
      boot_cmds[boot_cmds_cnt++] = PACKET_INFO;
      GetStringOption(&argi, argCount, argVect,
                      str, "--file");   /* file name */
      GetNumberOption(&argi, argCount, argVect,
                      &boot_cmds[boot_cmds_cnt++], str, 16); /* address */
      last_boot_file = allocate_boot_file(last_boot_file);
      first_boot_file = first_boot_file ? first_boot_file : last_boot_file;
      last_boot_file->fileName = malloc(strlen(str) + 1);
      strcpy(last_boot_file->fileName, str);
      last_boot_file->size_p = &boot_cmds[boot_cmds_cnt];
      last_boot_file->fd = NULL;
      last_boot_file->size_sent = 0;
      if (strncmp(str, "-", 1) != 0) {
        if (stat(last_boot_file->fileName, &st) == -1) {
          printf("Cannot get size of file '%s'. %s.\n",
                 last_boot_file->fileName, strerror(errno));
          exit(EXIT_FAILURE);
        }
        if (db2) printf("* size is %d 0x%8.8x\n",
                        (int)st.st_size, (unsigned int)st.st_size);
        last_boot_file->size = st.st_size;
        boot_cmds[boot_cmds_cnt++] = st.st_size;
      }
      else {
        GetNumberOption(&argi, argCount, argVect,
                        &last_boot_file->size , str, 16); /* size */

        boot_cmds[boot_cmds_cnt++] = last_boot_file->size;
        printf("* size is %d 0x%8.8x\n", last_boot_file->size,
               last_boot_file->size);
      }
    }

    else if (strncmp(argVect[argi], "--bootfile", 10) == 0) {
      GetStringOption(&argi, argCount, argVect,
                      boot_loader_file, "--bootfile");
    }

    else if (strncmp(argVect[argi], "-d", 2) == 0) {
      if (GetNumberOption (&argi, argCount, argVect, &number, "-d", 10) == 0) {
        printHelp = TRUE;
        exitCode = EXIT_FAILURE;
      }
      else {
        sprintf(dbStr, "%d", number);
        for (i = 0; i != (int) strlen(dbStr); i++) {
          switch (dbStr[i] - '0') {
          case 1:
            db1 = TRUE;
            break;
          case 2:
            db2 = TRUE;
            break;
          case 3:
            db3 = TRUE;
            break;
          case 4:
            db4 = TRUE;
            break;
          default:
            printf("ERROR! Debug level %d is not defined.\n", dbStr[i] - '0');
            printHelp = TRUE;
            exitCode = EXIT_FAILURE;
            break;
          }
        }
      }
    }

    else if (strncmp(argVect[argi], "--find", 6) == 0) {
      GetStringOption(&argi, argCount, argVect, needle, "--find");
      /* convert control characters like /n to the real ascii valure */
      {
        int i;
        int j = 0;
        char c;
        int esc = 0;

        for (i = 0; (c = needle[i]); i++,j++) {
//        printf("c = %d, i %d, j %d\n", (int)c, i, j);
          if (c == '\\') {
//          printf("esc\n");
            esc = 1;
            continue;
          }
          else if (esc) {
            esc = 0;
            switch(c) {
            case 'r':
              c = '\r';
              break;

            case 'n':
              c = '\n';
              break;

            case '\\':
              c = '\\';
              break;

            default:
              printf("Uknown escape sequence '\\%c'\n", c);
              exit(EXIT_FAILURE);
            }
            j--;
          }
          //      printf("setting j %d to %d\n", j, (int)c);
          needle[j] = c;
        }
        needle_len = j;
      }

      for (i = 0; needle[i]; i++) {
        //printf("i = %d, c %d\n", i,(int)needle[i]);
      }
    }

    else if (strncmp(argVect[argi], "--exitonfind", 12) == 0) {
      exitonfind = TRUE;
    }

    else if (strncmp(argVect[argi], "--help", 6) == 0) {
      printHelp = TRUE;
    }

    else {
      printf("ERROR! Don't understand option '%s'\n", argVect[argi]);
      printHelp = TRUE;
      exitCode = EXIT_FAILURE;
    }
  }

  if (printHelp == TRUE) {
    PrintHelp();
    exit(exitCode);
  }

  if (noleds) {
    strcpy(&boot_loader_file[strlen(boot_loader_file)], "_NOLEDS");
  }

  handle_labels();
}

/****************************************************************************
*#
*#  FUNCTION NAME: handle_labels
*#
*#  PARAMETERS: global loop_label, label
*#
*#  DESCRIPTION: Checks and matches labels from --loop and --label commands
*#  and inserts the resolved addresses into boot commands.
*#
*#---------------------------------------------------------------------------
*#  HISTORY
*#
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  20020204 ronny    Initial version
*#
*#***************************************************************************/

void
handle_labels(void)
{
  struct label_t *ll = loop_label;
  struct label_t *l  = label;
  struct label_t *last_l  = l;

  while(ll) {
    int match = 0;

    l = last_l;
    while(l) {
      match = 0;

      if (l->name && ll->name && (strcmp(l->name, ll->name) == 0)) {
        match = 1;
        boot_cmds[ll->addr] = IO_BUF_START+(l->addr*4);
        break;
      }
      l = l->prev;
    }
    if (!match) {
      printf("Error. No label '%s' defined as needed by --loop command.\n", ll->name);
      exit(EXIT_FAILURE);
    }
    ll = ll->prev;
  }
}

/****************************************************************************
*#
*#  FUNCTION NAME: new_label
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
*#  20020201 ronny    Initial version
*#
*#***************************************************************************/

void
new_label(struct label_t **label, udword addr, const char *name)
{
  struct label_t *new_p;

  new_p = malloc(sizeof(struct label_t));

  if (*label == NULL) {         /* first one? */
    *label = new_p;
    new_p->prev = NULL;
  }
  else {
    new_p->prev = *label;
  }
  *label = new_p;
  new_p->addr = boot_cmds_cnt;
  new_p->name = malloc(strlen(name));
  strcpy(new_p->name, name);
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
GetStringOption(int *argNr, int argCount, const char *argVect[], char *ret,
                const char *option)
{
  int startChar = strlen(option);

  *ret = '\0';

  /* Are there any more chars after option? If not skip to next argv. */
  if (strlen(argVect[*argNr]) <= (unsigned int)startChar) {
    (*argNr)++;
    startChar = 0;
  }

  /* Any args left? */
  if (*argNr >= argCount) {
    printf("ERROR! The option '%s' needs a string argument.\n", option);
    PrintHelp();
    exit(EXIT_FAILURE);
  }

  /* avoid stack overflow hacks */
  if (strlen(&argVect[*argNr][startChar]) > MAX_STRING_LEN) {
    printf("Argument '%s' longer than maximum allowable %d characters.\n",
           &argVect[*argNr][startChar], MAX_STRING_LEN);
    exit(EXIT_FAILURE);
  }

  strcpy(ret, &argVect[*argNr][startChar]);
  if (db4) printf("<<< GetStringOption '%s'\n", ret);

  return TRUE;
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
GetNumberOption(int *argNr, int argCount, const char *argVect[],
                unsigned int *ret, const char *option, int base)
{
  int startChar = 0;
  int add_io_base = 0;

  (*argNr)++;

  if (*argNr >= argCount) {
    printf("ERROR! The option '%s' needs a number argument.\n", option);
    PrintHelp();
    exit(EXIT_FAILURE);
  }

  if (argVect[*argNr][startChar] == '+') {
    add_io_base = 1;
    startChar++;
  }

  *ret = strtoul(&argVect[*argNr][startChar], NULL, base);

  if (add_io_base) {
    *ret += IO_BUF_START;
    if (*ret < IO_BUF_START || *ret >= IO_BUF_END) {
      printf("ERROR! '%s' is outside the IO buffer (option '%s').\n",
             argVect[*argNr], option);
      exit(EXIT_FAILURE);
    }
  }

  if (db4) printf("<<< GetNumberOption %08x\r\n", *ret);

  return TRUE;
}

/****************************************************************************
*#
*#  FUNCTION NAME: GetRegisterOption
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
GetRegisterOption(int *argNr, int argCount, const char *argVect[],
                  unsigned int *ret, const char *option, int base)
{
  int startChar = 0;

  (*argNr)++;

  if (*argNr >= argCount) {
    printf("Error! The option '%s' needs a register name.\n", option);
    PrintHelp();
    exit(EXIT_FAILURE);
  }

  {
    int r = 0;

    while (reg_des[r].name) {
      if (strcmp(reg_des[r].name, argVect[*argNr]) == 0) {
        *ret = reg_des[r].addr;
        return TRUE;
        break;
      }
      r++;
    }
  }

  printf("Error! Didn't find a register name matching '%s'.\n",
         argVect[*argNr]);

  exit(EXIT_FAILURE);

  return FALSE;
}

/****************************************************************************
*#
*#  FUNCTION NAME: PrintHelp
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: Prints help info.
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#
*#***************************************************************************/

void
PrintHelp(void)
{
  printf("e100boot version %s.\n", version);

  printf("\ne100boot [--device devicename] [--file filename|- addr [size]]\n"
         "         [--flash ram-source flash-offset size]\n"
         "         [--pause iter]\n"
         "         [--memtest addr addr]\n"
         "         [--memclear addr addr] [--memdump addr addr]\n"
         "         [--setreg addr|regname val] [--getreg addr|regname]\n"
         "         [--verify addr val] [--label label] [--loop addr label]\n"
         "         [--5400] [--5600] [--testcard] [--devboard]\n"
         "         [--testcardlx] [--network] [--serial]\n"
         "         [--baudrate baudrate] [--bootfile file] [--jump addr]\n"
         "         [--tofiles] [--cmdsonly] [--images] [--noleds]\n"
         "         [--help]\n\n");

  printf("         The commands sent to the cbl, and which are parsed by the cbl,\n"
         "         are stored at 0x%8.8x-0x%8.8x.\n\n", IO_BUF_START, IO_BUF_END);

  printf("         See the man page for more details about e100boot.\n\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: CreateBootLoader
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: Creates boot packets from boot file or internal loader.
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

void
CreateBootLoader(void)
{
  struct stat st;
  char *buf = NULL;
  //  int size_pos = 0x18;
  //  int addr_pos = 0x28;
  struct packet_header_T *ph;
  int packet_size;
  int header_size;
  int buf_cnt = 0;
  int i;
  udword sum = 0;

  if (create_boot_loader) {
    int image_nbr = 0;
    int found = 0;
    const struct boot_image_info_type *info;

    if (db4) printf("> CreateBootLoader\n");

    info = &boot_image_info[image_nbr];

    /* Use internal boot loader? */
    while (!found && info->name != NULL) {
      if (strcmp(boot_loader_file, info->name) == 0) {
        st.st_size = info->len;
        buf = (char*) malloc(st.st_size);
        memcpy(buf, info->ptr, st.st_size); /* unnecessary? */
        found = TRUE;
        printf("Using internal boot loader: %s - %s.\n",
               info->name, info->info);
      }
      else {
        image_nbr++;
        info = &boot_image_info[image_nbr];
      }
    }

    /* No internal? Load it from file instead. */
    if (!found) {
      FILE *fd;

      /* We didn't find an internal match, load the boot file from disk. */
      if ((fd = Fopen(boot_loader_file, "r")) == NULL) {
        printf("Cannot open bootloader '%s'. %s.\n",
               boot_loader_file, strerror(errno));
        exit(EXIT_FAILURE);
      }

      if (fstat(fileno(fd), &st) == -1) {
        printf("Cannot get filestatus of bootloader '%s'. %s.\n",
               boot_loader_file, strerror(errno));
        exit(EXIT_FAILURE);
      }

      buf = (char*) malloc(st.st_size);
      //      printf("CreateBootLoader: buf = (char*) malloc(st.st_size); 2\n");
      if (read(fileno(fd), buf, st.st_size) != st.st_size) {
        printf("Read fewer bytes than there should be in %s.\n",
               boot_loader_file);
        exit(EXIT_FAILURE);
      }

      fclose(fd);
    }

    /* Alright, got loader in buf[] and size in st. */
    if (netBoot) {
      /* The etrax code for all boot methods are linked to adress
         380000f0 but since network boot starts execution at 380000f4
         we have two nops in the beginning of the code which we do not
         transmit to etrax in the network case. The link adress
         doesn't change though. */
      buf += 4;
      st.st_size -= 4;
      packet_size = DATA_SIZE;
    }
    else {
      packet_size = st.st_size;
    }

    /* Hack binary, insert size and address. */

#define SIZE_PATTERN 0x12345678
#define ADDR_PATTERN 0x87654321
#define SIZE_POS (netBoot ? 0x0c : 0x10)
#define ADDR_POS (netBoot ? 0x10 : 0x14)

    if (*(udword*)&buf[SIZE_POS] != SIZE_PATTERN) {
      printf("Bootloader corrupt. Should contain ret/nop (0x%8.8x) at 0x%x, but contains %x\n",
             SIZE_PATTERN, SIZE_POS, *(udword*)&buf[SIZE_POS]);
      exit(EXIT_FAILURE);
    }

    /* How much data to load except data in first packet. */

    if (netBoot) {
      *(udword*)(&buf[SIZE_POS]) = st.st_size - DATA_SIZE;
    }
    else {
      *(udword*)(&buf[SIZE_POS]) = st.st_size - 784;
    }

    if (db3) printf("Inserting boot size 0x%x at 0x%x.\n",
                    (unsigned int) *(udword*)(&buf[SIZE_POS]),
                    (unsigned int)&buf[SIZE_POS]);

    if (*(udword*)&buf[ADDR_POS] != ADDR_PATTERN) {
      printf("Bootloader corrupt. Should contain ret/nop (0x%8.8x) at 0x%x, but contains %x\n",
             ADDR_PATTERN, ADDR_POS, *(udword*)&buf[ADDR_POS]);
      exit(EXIT_FAILURE);
    }

    if (netBoot) {
      *(udword*)(&buf[ADDR_POS]) = BOOT_ADDRESS + DATA_SIZE;
    }
    else {
      *(udword*)(&buf[ADDR_POS]) = BOOT_ADDRESS-4 + 784;
    }

    if (db3) printf("Inserting boot address 0x%x at 0x%x.\n",
                    (unsigned int)*(udword*)(&buf[ADDR_POS]),
                    (unsigned int)&buf[ADDR_POS]);


    for (i = 0; i != st.st_size; i++) {
      sum += ((byte*)buf)[i];
    }
    if (db1) printf("Checksum 0x%x, bytes %d\n", sum, i);

    if (db4) {
      int i;

      for(i=0; i<st.st_size; i+=8) {
        int j;

        printf("0x%8.8x[%4.4x]: ", BOOT_ADDRESS+i, i);
        for(j=0; i+j<st.st_size && j<8; j++) {
          printf("%2.2x ", (unsigned char) buf[i+j]);
        }
        printf("\n");
      }
    }
    /* Now create list of packets. */
    while (buf_cnt <= st.st_size) {

      header_size = seq_nr == 0 ?
        SIZE_OF_FIRST_HEADER : sizeof(struct packet_header_T);

      if (netBoot) {
        packet_size = ((st.st_size - buf_cnt) < DATA_SIZE ?
                       st.st_size - buf_cnt : DATA_SIZE) + header_size;
      }
      else {
        packet_size = st.st_size;
        header_size = 0;
      }

      if (db4) printf("seq_nr %d, header_size %d, packet_size %d\n",
                      seq_nr, header_size, packet_size);

      last_packet = allocate_packet(last_packet);

      first_packet = first_packet ? first_packet : last_packet;

      last_packet->size = packet_size;
      last_packet->data = (char*)malloc(packet_size);
      last_packet->seq = seq_nr;
      last_packet->baud_rate = 9600;

      last_packet->boot_file = allocate_boot_file(NULL);
      last_packet->boot_file->fileName = boot_loader_file;

      //      printf("last_packet->size %8.8x\n", last_packet->size);
      //      printf("last_packet->data %8.8x\n",last_packet->data);

      if (netBoot) {
        ph = (struct packet_header_T*) last_packet->data;
        memcpy(ph->dest, dst_addr_of_device, 6);
        memcpy(ph->src,  eth_addr_local,     6);
        ph->length = htons(packet_size);
        ph->snap1  = htonl(SNAP1);
        ph->snap2  = htonl(SNAP2);
        ph->tag    = htonl(SERVER_TAG);
        ph->seq    = htonl(seq_nr);
        if (seq_nr != 0) {
          ph->type   = htonl(BOOT_PACKET);
          ph->id     = htonl(0);
        }
      }

      memcpy(&last_packet->data[header_size], &buf[buf_cnt],
             packet_size - header_size);
      if (db3) DecodeSvintoBoot(last_packet->data);

      if (netBoot) {
        buf_cnt += DATA_SIZE;
      }
      else {
        buf_cnt += packet_size +1;
      }

      seq_nr++;
    }
  }

  if (db4) printf("< CreateBootLoader\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: allocate_packet
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

struct packet_buf*
allocate_packet(struct packet_buf *p)
{
  if (db4) printf("> allocate_packet\n");

  if (p) {
    p->next = (struct packet_buf*) malloc(sizeof(struct packet_buf));
    p       = p->next;
  }
  else {
    p = (struct packet_buf*) malloc(sizeof(struct packet_buf));
  }
  p->next = NULL;

  return(p);
}

/****************************************************************************
*#
*#  FUNCTION NAME: allocate_boot_file
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION:
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

struct boot_files_T*
allocate_boot_file(struct boot_files_T *bf)
{
  if (bf) {
    bf->next = (struct boot_files_T*) malloc(sizeof(struct boot_files_T));
    bf       = bf->next;
  }
  else {
    bf = (struct boot_files_T*) malloc(sizeof(struct boot_files_T));
  }
  bf->next = NULL;

  return(bf);
}

/****************************************************************************
*#
*#  FUNCTION NAME: CreateBootCmds
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: Creates a boot packet from the boot commands. The data is
*#  filled in later by FinishBootCmds().
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  980818   ronny    Initial version
*#***************************************************************************/

void
CreateBootCmds(void)
{
  struct packet_header_T *ph;

  if (db4) printf("***> CreateBootCmds\n");

  last_packet = allocate_packet(last_packet);

  boot_cmds_packet = last_packet;

  last_packet->boot_file = allocate_boot_file(NULL);
  last_packet->boot_file->fileName =  BOOT_CMDS_FILE;
  last_packet->baud_rate =  9600;

  last_packet->size = netBoot ? SIZE_OF_BOOT_CMDS + sizeof(struct packet_header_T)
    : SIZE_OF_BOOT_CMDS;

  last_packet->data = (char *) malloc(last_packet->size);
  last_packet->seq = seq_nr;

  if (netBoot) {
    /* Create packet header. */
    ph = (struct packet_header_T *) last_packet->data;
    memcpy(ph->dest, dst_addr_of_device, 6);
    memcpy(ph->src, eth_addr_local, 6);
    ph->length = htons(last_packet->size);
    ph->snap1 = htonl(SNAP1);
    ph->snap2 = htonl(SNAP2);
    ph->tag = htonl(SERVER_TAG);
    ph->seq = htonl(seq_nr);
    seq_nr++;
    ph->type = htonl(BOOT_CMDS);
    ph->id = htonl(0);
  }

  if (db3) DecodeSvintoBoot(last_packet->data);
  if (db4) printf("<*** CreateBootCmds\n");
}

/****************************************************************************
*#
*#  FUNCTION NAME: FinishBootCmds
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: Copies the boot commands into the correct packet and changes
*#  the dwords to network order.
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

void
FinishBootCmds(void)
{
  int i;
  unsigned int offset = 0;

  for (i = 0; i != boot_cmds_cnt; i++) {
    boot_cmds[i] = htonl(boot_cmds[i]);
    if (db3) printf("%8.8x\n", boot_cmds[i]);
  }

  /* Copy boot commands into packet. */
  if (netBoot) {
    offset = sizeof(struct packet_header_T);
  }

  memcpy(&boot_cmds_packet->data[offset], boot_cmds,
         boot_cmds_cnt * sizeof(udword));
}

/****************************************************************************
*#
*#  FUNCTION NAME: CreateNewBootPacket
*#
*#  PARAMETERS: None.
*#
*#  DESCRIPTION: Creates next packet for the files specified by '--file'.
*#
*#  RETURNS: Next packet, or NULL.
*#
*#---------------------------------------------------------------------------
*#  DATE     NAME     CHANGES
*#  ----     ----     -------
*#  960909   ronny    Initial version
*#***************************************************************************/

struct packet_buf*
CreateNewBootPacket(void)
{
  static char buf[DATA_SIZE];
  struct packet_header_T *ph;
  int packet_size;
  int header_size;
  int i;
  udword sum;
  int size = 0;
  int padding = 0;

  static struct boot_files_T *bf = NULL;

  if (db3) printf("> CreateNewBootPacket\n");

  bf = bf ? bf : first_boot_file;

  while (bf) {
    if (!bf->fd) {
      if (strcmp(bf->fileName, "-") == 0) {
        bf->fd = stdin;
      }
      else {
        bf->fd = fopen(bf->fileName, "rb");
      }

      if (bf->fd == NULL) {
        printf("Cannot open boot file %s. Exiting\n", bf->fileName);
        exit(EXIT_FAILURE);
      }
      if (db3) printf("Opening boot file %s\n", bf->fileName);
    }

    if (!padding) {
      size = fread(buf, 1, DATA_SIZE, bf->fd);
      if (size == 0) {
        if (db3) printf("Nothing more to read. Read: %d/%d\n",
                        bf->size_sent, bf->size);
        padding = 1;
      }
    }

    if (padding) {
      if (bf->size_sent < bf->size) {
        if (db3) printf("padding...\n");
        size = (bf->size - bf->size_sent > DATA_SIZE) ?
          DATA_SIZE : bf->size - bf->size_sent;
        memset(buf, 0, size);
      }
      else {
        if (db3) printf("All written\n");
        padding = 0;
        size = 0;
      }
    }

    if (size != 0) {
      if (db3) printf("size: %d %d/%d\n", size, bf->size_sent, bf->size);
      bf->size_sent += size;
      last_packet = allocate_packet(last_packet);

      /* Calculate checksum. */
      sum = 0;
      for (i = 0; i != size; i++) {
        sum += ((byte*)buf)[i];
      }
      if (db2) printf("Checksum 0x%x, bytes %d\n", sum, i);

      /* Figure out size of packet. */
      if (netBoot) {
        header_size = seq_nr == 0 ?
          SIZE_OF_FIRST_HEADER : sizeof(struct packet_header_T);

        packet_size = ((size) < DATA_SIZE ? size : DATA_SIZE) + header_size;
      }
      else {
        header_size = 0;
        packet_size = size;
      }

      if (packet_size < 60) { /* CRC adds 4 bytes to 64 */
        printf(
          "Last packet from file '%s', is smaller than 64 bytes. \n"
          "This is not allowed in the Ethernet standard. Will pad with %d "
          "bytes.\n", bf->fileName, 60-packet_size);

        *(bf->size_p) += 60-packet_size;
        packet_size = 60;
      }

      last_packet->size = packet_size;
      last_packet->data = (char*)malloc(packet_size);
      last_packet->boot_file = bf;
      last_packet->baud_rate = 0;/*set_baudrate;*/

      /*      printf("size %8.8x\n", last_packet->size);*/
      /*      printf("data %8.8x\n",last_packet->data);*/

      if (netBoot) {
        /* Initialize ethernet header. */
        ph = (struct packet_header_T*) last_packet->data;
        memcpy(ph->dest, dst_addr_of_device, 6);
        memcpy(ph->src,  eth_addr_local,     6);
        /*      printf("packet_size %d\n", packet_size);*/
        ph->length = htons(packet_size);
        ph->snap1  = htonl(SNAP1);
        ph->snap2  = htonl(SNAP2);
        ph->tag    = htonl(SERVER_TAG);
        ph->seq    = htonl(seq_nr);
        last_packet->seq = seq_nr;
        if (seq_nr != 0) {
          ph->type = htonl(BOOT_PACKET);
          ph->id   = htonl(0); /* id doesn't matter, we send to a unicast address */
        }
      }

      /* Copy data in place. */
      memcpy(&last_packet->data[header_size], buf, packet_size - header_size);
      if (db2) DecodeSvintoBoot(last_packet->data);
      /*      PrintPacket(last_packet->data, last_packet->size, HEX);*/
      seq_nr++;

      if (db3) printf("< CreateNewBootPacket\n");
      return(last_packet);
    }
    else {                   /* Nothing read from fd. */
      fclose(bf->fd);
      bf = bf->next;
    }
  }

  if (db3) printf("< CreateNewBootPacket\n");
  return(NULL);
}

/****************** END OF FILE common.c ************************************/
