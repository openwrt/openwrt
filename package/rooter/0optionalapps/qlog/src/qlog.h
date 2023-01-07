#include <stdio.h>  
#include <ctype.h>
#include <stdlib.h> 
#ifndef __QUECTEL_QLOG_H
#define __QUECTEL_QLOG_H
#include <string.h>   
#include <unistd.h>   
#include <fcntl.h>   
#include <errno.h>   
#include <termios.h> 
#include <time.h>  
#include <signal.h>
#include <sys/time.h> 
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <pthread.h>

typedef unsigned int uint32_t;

typedef struct {
    int (*init_filter)(int ttyfd, const char *conf);
    int (*logfile_create)(const char *logfile_dir, const char *logfile_suffix, unsigned logfile_seq);
    int (*logfile_init)(int logfd, unsigned logfile_seq);
    size_t (*logfile_save)(int logfd, const void *buf, size_t size);
    int (*logfile_close)(int logfd);
} qlog_ops_t;

extern qlog_ops_t mdm_qlog_ops;
extern qlog_ops_t asr_qlog_ops;
extern qlog_ops_t tty2tcp_qlog_ops;
extern int g_is_asr_chip;
extern ssize_t asr_send_cmd(int ttyfd, const unsigned char *buf, size_t size);
extern ssize_t mdm_send_cmd(int ttyfd, const unsigned char *buf, size_t size);

extern uint32_t qlog_le32 (uint32_t v32);
extern uint64_t qlog_le64 (uint64_t v64);
extern ssize_t qlog_poll_write(int fd, const void *buf, size_t size, unsigned timeout_mesc);

extern unsigned qlog_msecs(void);
#define qlog_dbg(fmt, arg... ) do { unsigned msec = qlog_msecs(); printf("[%03d.%03d]" fmt,  msec/1000, msec%1000, ## arg); } while (0)
 int sahara_catch_dump(int port_fd, const char *path_to_save_files, int do_reset);
#endif
