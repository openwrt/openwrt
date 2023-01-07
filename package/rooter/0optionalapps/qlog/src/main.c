#include "qlog.h"
#include "getopt.h"

#define LOGFILE_SIZE_MIN (2*1024*1024)
#define LOGFILE_SIZE_MAX (512*1024*1024)
#define LOGFILE_SIZE_DEFAULT (128*1024*1024)
#define LOGFILE_NUM 512
static char s_logfile_List[LOGFILE_NUM][32];
static unsigned s_logfile_num = 0;

uint32_t qlog_le32 (uint32_t v32) {
    uint32_t tmp = v32;
    const int is_bigendian = 1;

    if ( (*(char*)&is_bigendian) == 0 ) {
        unsigned char *s = (unsigned char *)(&v32);
        unsigned char *d = (unsigned char *)(&tmp);
        d[0] = s[3];
        d[1] = s[2];
        d[2] = s[1];
        d[3] = s[0];
    }
    return tmp;
}

uint64_t qlog_le64(uint64_t v64) {
    const uint64_t is_bigendian = 1;
    uint64_t tmp = v64;
	
    if ((*(char*)&is_bigendian) == 0) {
        unsigned char *s = (unsigned char *)(&v64);
        unsigned char *d = (unsigned char *)(&tmp);
        d[0] = s[7];
        d[1] = s[6];
        d[2] = s[5];
        d[3] = s[4];
        d[4] = s[3];
        d[5] = s[2];
        d[6] = s[1];
        d[7] = s[0];
    }
    return tmp;
}

unsigned qlog_msecs(void) {
    static unsigned start = 0;
    unsigned now;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    now =  (unsigned)tv.tv_sec*1000 + (unsigned)tv.tv_usec / 1000;
    if (start == 0)
        start = now;
    return now - start;
}

void qlog_get_vidpid_by_ttyport(const char *ttyport, char idVendor[5], char idProduct[5], char bNumInterfaces[5]) {
    char syspath[255];
    char sysport[64];
    int count;
    char *pchar = NULL;
    int fd = 0;

    memset(idVendor, 0x00, 5);
    memset(idProduct, 0x00, 5);
    memset(bNumInterfaces, 0x00, 5);
    
    snprintf(sysport, sizeof(sysport), "/sys/class/tty/%s", &ttyport[strlen("/dev/")]);
    count = readlink(sysport, syspath, sizeof(syspath) - 1);
    if (count < strlen(":1.0/tty"))
        return;

//ttyUSB0 -> ../../devices/soc0/soc/2100000.aips-bus/2184200.usb/ci_hdrc.1/usb1/1-1/1-1:1.0/ttyUSB0/tty/ttyUSB0
    pchar = strstr(syspath, ":1.0/tty"); //MDM
    if (pchar == NULL) {
        pchar = strstr(syspath, ":1.2/tty"); //ASR
        g_is_asr_chip = (pchar != NULL);
    }
    if (pchar == NULL) {
        qlog_dbg("%s is not a usb-to-serial device?\n", ttyport);
        return;
     }

    *pchar = '\0';
    while (*pchar != '/')
        pchar--;

    strcpy(sysport, pchar + 1);
    
    snprintf(syspath, sizeof(syspath), "/sys/bus/usb/devices/%s/idVendor", sysport);
    fd = open(syspath, O_RDONLY);
    if (fd <= 0) {
        qlog_dbg("Fail to open %s,  errno: %d (%s)\n", syspath, errno, strerror(errno));
        return;
    }
    read(fd, idVendor, 4);
    close(fd);

    snprintf(syspath, sizeof(syspath), "/sys/bus/usb/devices/%s/idProduct", sysport);
    fd = open(syspath, O_RDONLY);
    if (fd <= 0) {
        qlog_dbg("Fail to open %s,  errno: %d (%s)\n", syspath, errno, strerror(errno));
        return;
    }
    read(fd, idProduct, 4);
    close(fd);

    snprintf(syspath, sizeof(syspath), "/sys/bus/usb/devices/%s/bNumInterfaces", sysport);
    fd = open(syspath, O_RDONLY);
    if (fd <= 0) {
        qlog_dbg("Fail to open %s,  errno: %d (%s)\n", syspath, errno, strerror(errno));
        return;
    }
    read(fd, bNumInterfaces, 4);
    close(fd);

    qlog_dbg("%s idVendor=%s, idProduct=%s, bNumInterfaces=%d\n", __func__, idVendor, idProduct, atoi(bNumInterfaces));
}

static unsigned long get_now_msec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000 + tv.tv_usec / 1000;
}

static size_t ql_tty_read(int fd, void *buf, size_t size)
{
    size_t rc;
    
    rc = read(fd,buf,size);

    if (rc > 0) {
    	static size_t total_read = 0;
    	static unsigned long now_msec = 0;
    	unsigned long n = get_now_msec();

    	if (now_msec == 0)
    		now_msec = get_now_msec();
    	total_read += rc;
    	
    	if ((total_read >= (4*1024*1024)) || (n >= (now_msec + 5000))) {
    		qlog_dbg("recv: %zdM %zdK %zdB  in %ld msec\n", total_read/(1024*1024),
    			total_read/1024%1024,total_read%1024, n-now_msec);
    		now_msec = n;
    		total_read = 0;
    	}
    }
		
    return rc;
}

ssize_t qlog_poll_write(int fd, const void *buf, size_t size, unsigned timeout_msec) {
    ssize_t wc = 0;
    ssize_t nbytes;

    nbytes = write(fd, buf+wc, size-wc);

    if (nbytes <= 0) {
        if (errno != EAGAIN) {
            qlog_dbg("Fail to write fd = %d, errno : %d (%s)\n", fd, errno, strerror(errno));
            goto out;
        }
        else {
            nbytes = 0;
        }
    }

    wc += nbytes;

    while (wc < size) {
        int ret;
        struct pollfd pollfds[] = {{fd, POLLOUT, 0}};

        ret = poll(pollfds, 1, timeout_msec);

        if (ret <= 0) {
            qlog_dbg("Fail to poll fd = %d, errno : %d (%s)\n", fd, errno, strerror(errno));
            break;
        }

        if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            qlog_dbg("Fail to poll fd = %d, revents = %04x\n", fd, pollfds[0].revents);
            break;
        }

        if (pollfds[0].revents & (POLLOUT)) {
            nbytes = write(fd, buf+wc, size-wc);
            
            if (nbytes <= 0) {
                qlog_dbg("Fail to write fd = %d, errno : %d (%s)\n", fd, errno, strerror(errno));
                break;
            }
            wc += nbytes;
        }
    }

out:
    if (wc != size) {
        qlog_dbg("%s fd=%d, size=%zd, timeout=%d, wc=%zd\n", __func__, fd, size, timeout_msec, wc);
    }
    
    return (wc);
}

static int qlog_logfile_create(const char *logfile_dir, const char *logfile_suffix, unsigned logfile_seq) {
    int logfd;
    time_t ltime;
    char shortname[32];
    char filename[255+1];
    struct tm *currtime;

    //delete old logfile
    if (s_logfile_num && s_logfile_List[logfile_seq%s_logfile_num][0]) {
        sprintf(filename, "%s/%s.%s", logfile_dir, s_logfile_List[logfile_seq%s_logfile_num], logfile_suffix);
        if (access(filename, R_OK) == 0) {
            remove(filename);
        }
    }

    time(&ltime);
    currtime = localtime(&ltime);
    snprintf(shortname, sizeof(shortname), "%04d%02d%02d_%02d%02d%02d_%04d",
    	(currtime->tm_year+1900), (currtime->tm_mon+1), currtime->tm_mday,
    	currtime->tm_hour, currtime->tm_min, currtime->tm_sec, logfile_seq);
    sprintf(filename, "%s/%s.%s", logfile_dir, shortname, logfile_suffix);

    logfd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0444);
    if (logfd <= 0) {
        qlog_dbg("Fail to create new logfile! errno : %d (%s)\n", errno, strerror(errno));
    }

    qlog_dbg("%s %s logfd=%d\n", __func__, filename, logfd);

    if (s_logfile_num) {
        strcpy(s_logfile_List[logfile_seq%s_logfile_num], shortname);
    }

    return logfd;
}

static size_t qlog_logfile_save(int logfd, const void *buf, size_t size) {
    return qlog_poll_write(logfd, buf, size, 1000);
}

static int qlog_logfile_close(int logfd) {
    return close(logfd);
}

static void* qlog_logfile_init_filter_thread(void* arg) {
    void **thread_args = (void **)arg;
    qlog_ops_t *qlog_ops = (qlog_ops_t *)thread_args[0];
    int *ttyfd = (int *)thread_args[1];
    const char *filter_cfg =  ( const char *)thread_args[2];

    if (qlog_ops->init_filter)
        qlog_ops->init_filter(*ttyfd, filter_cfg);
        
    return NULL;
}

static int qlog_handle(int ttyfd, const char *logfile_dir, size_t logfile_size, unsigned logfile_num, const char *filter_cfg) {
    ssize_t savelog_size = 0;
    void *rbuf;
    const size_t rbuf_size = (16*1024);
    static int logfd = -1;
    unsigned logfile_seq = 0;
    const char *logfile_suffix = g_is_asr_chip ? "sdl" : "qmdl";
    static qlog_ops_t qlog_ops;
    pthread_t thread_id;
    pthread_attr_t thread_attr;
    const void *thread_args[3];

    if (logfile_dir[0] == '9' && atoi(logfile_dir) >= 9000) {
        filter_cfg = logfile_dir;
        qlog_ops = tty2tcp_qlog_ops;
    }
    else {
        qlog_ops = g_is_asr_chip ? asr_qlog_ops : mdm_qlog_ops;
        if (access(logfile_dir, F_OK) && errno == ENOENT)
            mkdir(logfile_dir, 0755);
    }

    if (!qlog_ops.logfile_create)
        qlog_ops.logfile_create = qlog_logfile_create;
    if (!qlog_ops.logfile_save)
        qlog_ops.logfile_save = qlog_logfile_save;
    if (!qlog_ops.logfile_close)
        qlog_ops.logfile_close = qlog_logfile_close;

    rbuf = malloc(rbuf_size);
    if (rbuf == NULL) {
          qlog_dbg("Fail to malloc rbuf_size=%zd, errno: %d (%s)\n", rbuf_size, errno, strerror(errno));
          return -1;
    }

    thread_args[0] = &qlog_ops;
    thread_args[1] = &ttyfd;
    thread_args[2] = filter_cfg;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_id, &thread_attr, qlog_logfile_init_filter_thread, (void*)thread_args);

    while(1) {
        ssize_t rc, wc;
        int ret;
        struct pollfd pollfds[] = {{ttyfd, POLLIN, 0}};

        ret = poll(pollfds, 1, -1);

        if (ret <= 0) {
            qlog_dbg("poll(ttyfd) =%d, errno: %d (%s)\n", ret, errno, strerror(errno));
            break;
        }

        if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            qlog_dbg("ttyfd revents = %04x\n", pollfds[0].revents);
            break;
        }

        if (pollfds[0].revents & (POLLIN)) {
            rc = ql_tty_read(ttyfd, rbuf, rbuf_size);
            
            if(rc > 0) {
                if (logfd == -1) {
                    logfd = qlog_ops.logfile_create(logfile_dir, logfile_suffix, logfile_seq);
                    if (logfd <= 0) {
                        break;
                    }
                    if (qlog_ops.logfile_init)
                        qlog_ops.logfile_init(logfd, logfile_seq);
                    logfile_seq++;
                }
                        
                wc = qlog_ops.logfile_save(logfd, rbuf, rc);
 
                if (wc != rc) {
                    qlog_dbg("savelog fail %zd/%zd, break\n", wc, rc);
                    break;
                }

                savelog_size += wc;

                if (savelog_size >= logfile_size) {
                    savelog_size = 0;
                    qlog_ops.logfile_close(logfd);
                    logfd = -1;
                }
            }
            else
            {
                qlog_dbg("ttyfd recv %zd Bytes. break\n", rc);
                break;
            }
        }
    }   

    if (logfd > 0)
        qlog_ops.logfile_close(logfd);
    free(rbuf);

    return 0;
}

static void ql_sigaction(int signal_num) {
    qlog_dbg("recv signal %d\n", signal_num);
}

static void qlog_usage(const char *self, const char *dev) {
    qlog_dbg("Usage: %s -p <log port> -s <log save dir> -f filter_cfg -n <log file max num> -b <log file size MBytes>\n", self);
    qlog_dbg("Default: %s -p %s -s %s -n %d -b %d to save log to local disk\n",
        self, dev, ".", LOGFILE_NUM, LOGFILE_SIZE_DEFAULT/1024/1024);
    qlog_dbg("    -p    ttyport to catch log, default is '/dev/ttyUSB0'\n");
    qlog_dbg("    -s    where to save log, default is '.' \n");
    qlog_dbg("          if set as '9000', QLog will run as TCP Server Mode, and let 'QPST/QWinLog/CATStudio' to connect!\n");
    qlog_dbg("    -f    filter cfg for catch log, can be found in directory 'conf'. if not set this arg, will use default filter conf\n");
    qlog_dbg("          and UC200T&EC200T do not need filter cfg.\n");
    qlog_dbg("    -n    max num of log file to save, range is '0~512'. default is 0. 0 means no limit.\n");
    qlog_dbg("          or QLog will auto delete oldtest log file if exceed max num\n");
    qlog_dbg("    -m    max size of single log file, unit is MBytes, range is '2~512', default is 128\n");

    qlog_dbg("\nFor example: %s -p /dev/ttyUSB0 -w .\n", self);
}

int main(int argc, char **argv)
{   
    int opt;
    char ttyname[32] = "/dev/ttyUSB0";
    int ttyfd = -1;
    const char *logfile_dir = "./";
    const char *filter_cfg = NULL;
    size_t logfile_size = LOGFILE_SIZE_DEFAULT;
    unsigned logfile_num = LOGFILE_NUM;
    char idVendor[5] = "0000";
    char idProduct[5] = "0000";
    char bNumInterfaces[5] = "0";

    qlog_dbg("QLog Version: Quectel_QLog_Linux&Android_V1.3\n"); //when release, rename to V1.X

    optind = 1; //call by popen(), optind mayby is not 1
    while ( -1 != (opt = getopt(argc, argv, "d:p:s:w:n:m:b:f:c:h"))) {
        switch (opt) {
            case 'p':
                if (optarg[0] == 't') //ttyUSB0
                    snprintf(ttyname, sizeof(ttyname), "/dev/%s", optarg);
                else if (optarg[0] == 'U') //USB0
                    snprintf(ttyname, sizeof(ttyname), "/dev/tty%s", optarg);
                else if (optarg[0] == '/')
                    snprintf(ttyname, sizeof(ttyname), "%s", optarg);
                else {
                    qlog_dbg("unknow dev %s\n", optarg);
                }
            break;
            case 's':
                logfile_dir = optarg;
            break;
            case 'n':
                logfile_num = atoi(optarg);
                if (logfile_num < 0)
                    logfile_num = 0;
                else if (logfile_num > LOGFILE_NUM)
                    logfile_num = LOGFILE_NUM;
                s_logfile_num = logfile_num;
            break;
            case 'm':
                logfile_size = atoi(optarg)*1024*1024;
                if (logfile_size < LOGFILE_SIZE_MIN)
                    logfile_size = LOGFILE_SIZE_MIN;
                else if (logfile_size > LOGFILE_SIZE_MAX)
                    logfile_size = LOGFILE_SIZE_MAX;
            break;
            case 'f':
                filter_cfg = optarg;
            break;
            case 'b':
            case 'c': //unused
            break;
            default:
                qlog_usage(argv[0], ttyname);
            return 0;
            break;
        }
    }     

    signal(SIGTERM, ql_sigaction);
    signal(SIGHUP, ql_sigaction);
    signal(SIGINT, ql_sigaction);

    ttyfd = open (ttyname, O_RDWR | O_NDELAY | O_NOCTTY);

    if (ttyfd <= 0) {
        qlog_dbg("Fail to open %s, errno : %d (%s)\n", ttyname, errno, strerror(errno));
        return -1;
    }

    qlog_dbg("open %s ttyfd = %d\n", ttyname, ttyfd);
    
    if ( ttyfd > 0 ) {
        struct termios  ios;
        
        memset(&ios, 0, sizeof(ios));
        tcgetattr( ttyfd, &ios );
        cfmakeraw(&ios);
        cfsetispeed(&ios, B115200);
        cfsetospeed(&ios, B115200);
        tcsetattr( ttyfd, TCSANOW, &ios );
    }

    if (strncmp(ttyname, "/dev/mhi", strlen("/dev/mhi"))) {
        qlog_get_vidpid_by_ttyport(ttyname, idVendor, idProduct, bNumInterfaces);
    }                         
    else {
        if (!strcmp(ttyname, "/dev/mhi_DIAG")) {
            strcpy(idVendor, "2c7c");
            strcpy(bNumInterfaces, "5"); //log mode
        }
        else if (!strcmp(ttyname, "/dev/mhi_SAHARA")) {
            strcpy(idVendor, "2c7c");
            strcpy(bNumInterfaces, "1"); //dump mode
        }
    }   
 
    qlog_dbg("Press CTRL+C to stop catch log.\n");

    if (g_is_asr_chip == 0 && atoi(bNumInterfaces) == 1) {
        if (access(logfile_dir, F_OK) && errno == ENOENT)
            mkdir(logfile_dir, 0755);
        sahara_catch_dump(ttyfd, logfile_dir, 1);
    }
    else if (atoi(bNumInterfaces) > 1) {
        qlog_handle(ttyfd, logfile_dir, logfile_size, logfile_num, filter_cfg);
    } 
    else {
        qlog_dbg("unknow state! quit!\n");
    }
    
    close(ttyfd);

    return 0;
}
