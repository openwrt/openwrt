/******************************************************************************
  @file    qfirehose.c
  @brief   entry point.

  DESCRIPTION
  QFirehoe Tool for USB and PCIE of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include <getopt.h>
#include <grp.h>
#include <sys/types.h>
#include <pwd.h>
#ifdef USE_IPC_MSG
#include <sys/msg.h>
#include <sys/ipc.h>
#endif

#include "usb_linux.h"
#include "md5.h"

/*
[PATCH 3.10 27/54] usb: xhci: Add support for URB_ZERO_PACKET to bulk/sg transfers
https://www.spinics.net/lists/kernel/msg2100618.html

commit 4758dcd19a7d9ba9610b38fecb93f65f56f86346
Author: Reyad Attiyat <reyad.attiyat@gmail.com>
Date:   Thu Aug 6 19:23:58 2015 +0300

    usb: xhci: Add support for URB_ZERO_PACKET to bulk/sg transfers

    This commit checks for the URB_ZERO_PACKET flag and creates an extra
    zero-length td if the urb transfer length is a multiple of the endpoint's
    max packet length.
*/
unsigned qusb_zlp_mode = 1; //MT7621 donot support USB ZERO PACKET
unsigned q_erase_all_before_download = 0;
const char *q_device_type = "nand"; //nand/emmc/ufs
int sahara_main(const char *firehose_dir, const char *firehose_mbn, void *usb_handle, int edl_mode_05c69008);
int firehose_main (const char *firehose_dir, void *usb_handle, unsigned qusb_zlp_mode);
int stream_download(const char *firehose_dir, void *usb_handle, unsigned qusb_zlp_mode);
int retrieve_soft_revision(void *usb_handle, uint8_t *mobile_software_revision, unsigned length);
int usb2tcp_main(const void *usb_handle, int tcp_port, unsigned qusb_zlp_mode);
int ql_capture_usbmon_log(const char* usbmon_logfile);
void ql_stop_usbmon_log();

//process vals
static long long all_bytes_to_transfer = 0;    //need transfered
static long long transfer_bytes = 0;        //transfered bytes;

int switch_to_edl_mode(void *usb_handle) {
    //DIAG commands used to switch the Qualcomm devices to EDL (Emergency download mode)
    unsigned char edl_cmd[] = {0x4b, 0x65, 0x01, 0x00, 0x54, 0x0f, 0x7e};
    //unsigned char edl_cmd[] = {0x3a, 0xa1, 0x6e, 0x7e}; //DL (download mode)
    unsigned char *pbuf = malloc(512);
    int rx_len;
    int rx_count = 0;

     do {
        rx_len = qusb_noblock_read(usb_handle, pbuf , 512, 0, 1000);
        if (rx_count++ > 100)
            break;
    } while (rx_len > 0);

    dbg_time("switch to 'Emergency download mode'\n");
    rx_len = qusb_noblock_write(usb_handle, edl_cmd, sizeof(edl_cmd), sizeof(edl_cmd), 3000, 0);
    if (rx_len < 0)
        return 0;

    rx_count = 0;
    
    do {
        rx_len = qusb_noblock_read(usb_handle, pbuf , 512, 0, 3000);
        if (rx_len == sizeof(edl_cmd) && memcmp(pbuf, edl_cmd, sizeof(edl_cmd)) == 0) {
            dbg_time("successful, wait module reboot\n");
            free(pbuf);
            return 1;
        }
        
        if (rx_count++ > 50)
            break;
        
    } while (rx_len > 0);

    free(pbuf);
    return 0;
}

static void usage(int status, const char *program_name)
{
    if(status != EXIT_SUCCESS)
    {
        printf("Try '%s --help' for more information.\n", program_name);
    }
    else
    {
        dbg_time("Upgrade Quectel's modules with Qualcomm's firehose protocol.\n");
        dbg_time("Usage: %s [options...]\n", program_name);
        dbg_time("    -f [package_dir]               Upgrade package directory path\n");
        dbg_time("    -p [/dev/ttyUSBx]              Diagnose port, will auto-detect if not specified\n");
        dbg_time("    -s [/sys/bus/usb/devices/xx]   When multiple modules exist on the board, use -s specify which module you want to upgrade\n");
        dbg_time("    -e                             Erase All Before Download (will Erase calibration data, careful to USE)\n");
        dbg_time("    -l [dir_name]                  Sync log into a file(will create qfirehose_timestamp.log)\n");
        dbg_time("    -u [usbmon_log]                Catch usbmon log and save to file (need debugfs and usbmon driver)\n");
        dbg_time("    -n                             Skip MD5 check\n");
        dbg_time("    -d                             Device Type, default nand, support emmc/ufs\n");
    }
    exit(status);
}

/*
1. enum dir, fix up dirhose_dir
2. md5 examine
3. furture
*/
static char * find_firehose_mbn(char** firehose_dir, size_t size)
{
    char *firehose_mbn = NULL;

    if (strstr(*firehose_dir, "/update/firehose") == NULL) {
        size_t len = strlen(*firehose_dir);

        strncat(*firehose_dir, "/update/firehose", size);
        if (access(*firehose_dir, R_OK)) {
            (*firehose_dir)[len] = '\0'; // for smart module
        }
    }

    if (access(*firehose_dir, R_OK)) {
        dbg_time("%s access(%s fail), errno: %d (%s)\n", __func__, *firehose_dir, errno, strerror(errno));
        return NULL;
    }

    if (!qfile_find_file(*firehose_dir, "prog_nand_firehose_", ".mbn", &firehose_mbn)
        && !qfile_find_file(*firehose_dir, "prog_emmc_firehose_", ".mbn", &firehose_mbn)
        && !qfile_find_file(*firehose_dir, "prog_firehose_", ".mbn", &firehose_mbn)
        && !qfile_find_file(*firehose_dir, "prog_firehose_", ".elf", &firehose_mbn)
        && !qfile_find_file(*firehose_dir, "firehose-prog", ".mbn", &firehose_mbn)
        && !qfile_find_file(*firehose_dir, "prog_", ".mbn", &firehose_mbn)
      ) {
        dbg_time("%s fail to find firehose mbn file in %s\n", __func__, *firehose_dir);
        return NULL;
    }

    dbg_time("%s %s\n", __func__, firehose_mbn);
    return firehose_mbn;
}

static int detect_and_judge_module_version(void *usb_handle) {
    static uint8_t version[64] = {'\0'};

    if (usb_handle && version[0] == '\0') {
        retrieve_soft_revision(usb_handle, version, sizeof(version));
        if (version[0]) {
            size_t i = 0;
            size_t length = strlen((const char *)version) - strlen("R00A00");
            dbg_time("old software version: %s\n", version);
            for (i = 0; i < length; i++) {
                if (version[i] == 'R' && isdigit(version[i+1]) &&  isdigit(version[i+2])
                    && version[i+3] == 'A'  && isdigit(version[i+4]) &&  isdigit(version[i+5]))
                {
                    version[i] = '\0';
                    //dbg_time("old hardware version: %s\n", mobile_software_revision);
                    break;
                }
            }
        }
    }
        
    if (version[0])
        return 0;
    
    error_return();
}

FILE* loghandler = NULL;
#ifdef FIREHOSE_ENABLE
int firehose_main_entry(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    int opt;
    int check_hash = 1;
    int retval;
    void *usb_handle = NULL;
    int idVendor = 0, idProduct = 0, interfaceNum = 0;
    int edl_retry = 30; //SDX55 require long time by now 20190412
    double start;
    char *firehose_dir = malloc(MAX_PATH);
    char *firehose_mbn = NULL;
    char *module_port_name = malloc(MAX_PATH);
    char *module_sys_path = malloc(MAX_PATH);
    int xhci_usb3_to_usb2_cause_syspatch_chage = 1;
    int usb2tcp_port = 0;
    char filename[128] = {'\0'};
    const char *usbmon_logfile = NULL;

    firehose_dir[0] = module_port_name[0] = module_sys_path[0] = '\0';

    /* set file priviledge mask 0 */
    umask(0);
    /*build V1.0.8*/
    dbg_time("Version: QFirehose_Linux_Android_V1.4.11\n"); //when release, rename to V1.X
#ifndef __clang__
    dbg_time("Builded: %s %s\n", __DATE__,__TIME__);
#endif

#ifdef ANDROID
    struct passwd* passwd;
    passwd = getpwuid(getuid());
    dbg_time("------------------\n");
    dbg_time("User:\t %s\n",passwd->pw_name);
    struct group* group;
    group = getgrgid(passwd->pw_gid);
    dbg_time("Group:\t %s\n", group->gr_name);
    dbg_time("------------------\n");
#if 0 //not all customers need this function
    loghandler = fopen("/data/upgrade.log", "w+");
#endif
    if (loghandler) dbg_time("upgrade log will be sync to /data/upgrade.log\n");
#endif

    optind = 1;
    while ( -1 != (opt = getopt(argc, argv, "f:p:z:s:l:u:d:neh"))) {
        switch (opt) {
            case 'n':
                check_hash = 0;
                break;
            case 'l':
                if (loghandler) fclose(loghandler);
                snprintf(filename, sizeof(filename), "%.80s/qfirehose_%lu.log", optarg, time(NULL));
                loghandler = fopen(filename, "w+");
                if (loghandler) dbg_time("upgrade log will be sync to %s\n", filename);
                break;
            case 'f':
                strncpy(firehose_dir, optarg, MAX_PATH);
            break;
            case 'p':
                strncpy(module_port_name, optarg, MAX_PATH);
                if (!strcmp(module_port_name, "9008")) {
                    usb2tcp_port = atoi(module_port_name);
                    module_port_name[0] = '\0';
                }
            break;
            case 's':
                xhci_usb3_to_usb2_cause_syspatch_chage = 0;
                strncpy(module_sys_path, optarg, MAX_PATH);
                if (module_sys_path[strlen(optarg)-1] == '/')
                    module_sys_path[strlen(optarg)-1] = '\0';
                break;
            case 'z':
                qusb_zlp_mode = !!atoi(optarg);
                break;
            case 'e':    
                q_erase_all_before_download = 1;
                break;
            case 'u':
                usbmon_logfile = strdup(optarg);
            break;
            case 'd':
                q_device_type = strdup(optarg);
            break;
            case 'h':
                usage(EXIT_SUCCESS, argv[0]);
                break;
            default:
            break;
        }
    }

    if (usbmon_logfile)
        ql_capture_usbmon_log(usbmon_logfile);

    update_transfer_bytes(0);
    if (usb2tcp_port)
        goto _usb2tcp_start;

    if (firehose_dir[0] == '\0') {
        usage(EXIT_SUCCESS, argv[0]);
        update_transfer_bytes(-1);
        error_return();
    }
        
    if (access(firehose_dir, R_OK)) {
        dbg_time("fail to access %s, errno: %d (%s)\n", firehose_dir, errno, strerror(errno));
        update_transfer_bytes(-1);
        error_return();
    }

    opt = strlen(firehose_dir);
    if (firehose_dir[opt-1] == '/') {
        firehose_dir[opt-1] = '\0';
    }
        
    // check the md5 value of the upgrade file
    if (check_hash && md5_check(firehose_dir)) {
        update_transfer_bytes(-1);
        error_return();
    }

    //hunter.lv add check dir 2018-07-28
    firehose_mbn = find_firehose_mbn(&firehose_dir, MAX_PATH);
    if (!firehose_mbn) {
        update_transfer_bytes(-1);
        error_return();
    }
    //hunter.lv add check dir 2018-07-28

    if (module_port_name[0] && !strncmp(module_port_name, "/dev/mhi", strlen("/dev/mhi"))) {
        if (qpcie_open(firehose_dir, firehose_mbn)) {
            update_transfer_bytes(-1);
            error_return();
        }      
		
        usb_handle = &edl_pcie_mhifd;
        start = get_now();
        goto __firehose_main;
    }
    else if (module_port_name[0] && strstr(module_port_name, ":9008")) {
        strcpy(module_sys_path, module_port_name);
        goto __edl_retry;
    }

_usb2tcp_start:
    if (module_sys_path[0] && access(module_sys_path, R_OK)) {
        dbg_time("fail to access %s, errno: %d (%s)\n", module_sys_path, errno, strerror(errno));
        update_transfer_bytes(-1);
        error_return();
    }

    if (module_port_name[0] && access(module_port_name, R_OK | W_OK)) {
        dbg_time("fail to access %s, errno: %d (%s)\n", module_port_name, errno, strerror(errno));
        update_transfer_bytes(-1);
        error_return();
    }

    if (module_sys_path[0] == '\0' && module_port_name[0] != '\0') {
        //get sys path by port name
        quectel_get_syspath_name_by_ttyport(module_port_name, module_sys_path, MAX_PATH);
    }

    if (module_sys_path[0] == '\0') {
        int module_count = auto_find_quectel_modules(module_sys_path, MAX_PATH);
        if (module_count <= 0) {
            dbg_time("Quectel module not found\n");
            update_transfer_bytes(-1);
            error_return();
        }
        else if (module_count == 1) {

        } else {
            dbg_time("There are multiple quectel modules in system, Please use <-s /sys/bus/usb/devices/xx> specify which module you want to upgrade!\n");
            dbg_time("The module's </sys/bus/usb/devices/xx> path was printed in the previous log!\n");
            update_transfer_bytes(-1);
            error_return();
        }
    }

__edl_retry:
    while (edl_retry-- > 0) {
        usb_handle = qusb_noblock_open(module_sys_path, &idVendor, &idProduct, &interfaceNum);
        if (usb_handle == NULL && module_sys_path[0] == '/') {
            sleep(1); //in reset sate, wait connect
            if (xhci_usb3_to_usb2_cause_syspatch_chage && access(module_sys_path, R_OK) && errno_nodev()) {
                auto_find_quectel_modules(module_sys_path, MAX_PATH);
            }
            else if (access(module_sys_path, R_OK) && errno_nodev()) {
                int busidx = strlen("/sys/bus/usb/devices/");
                char busnum = module_sys_path[busidx];

                module_sys_path[busidx] = busnum-1;
                if (access(module_sys_path, R_OK) && errno_nodev())
                    module_sys_path[busidx] = busnum+1;

                if (!access(module_sys_path, R_OK)) {
                    usb_handle = qusb_noblock_open(module_sys_path, &idVendor, &idProduct, &interfaceNum);
                    if (usb_handle && (idVendor != 0x05c6 || idProduct != 0x9008)) {
                        qusb_noblock_close(usb_handle);
                        usb_handle = NULL;  
                    }
                }
                module_sys_path[busidx] = busnum;
            }

            if (usb_handle == NULL)
                continue;
        }

        if (idVendor == 0x2c7c && interfaceNum > 1) {
            if (detect_and_judge_module_version(usb_handle)) {
                // update_transfer_bytes(-1);
                /* do not return here, this command will fail when modem is not ready */
                // error_return();
            }
        }

        if (interfaceNum == 1) {
            if ((idVendor == 0x2C7C) && (idProduct == 0x0800)) {
                // although 5G module stay in dump mode, after send edl command, it also can enter edl mode
                dbg_time("5G module stay in dump mode!\n");				
            } else {
                break;					
            }
            dbg_time("something went wrong???, why only one interface left\n");
        }

        switch_to_edl_mode(usb_handle);
        qusb_noblock_close(usb_handle);
        usb_handle = NULL;
        sleep(1); //wait usb disconnect and re-connect
    }

    if (usb_handle == NULL) {
        update_transfer_bytes(-1);
        error_return();
    }

    if (usb2tcp_port) {
        retval = usb2tcp_main(usb_handle, usb2tcp_port, qusb_zlp_mode);
        qusb_noblock_close(usb_handle);
        return retval;
    }

    start = get_now();
    retval = sahara_main(firehose_dir, firehose_mbn, usb_handle, idVendor == 0x05c6);

    if (!retval) {
        if (idVendor != 0x05C6) {
            sleep(1);
            stream_download(firehose_dir, usb_handle, qusb_zlp_mode);
            qusb_noblock_close(usb_handle);
            sleep(10);      //EM05-G switching to download mode is slow and increases the waiting time to 10 seconds
            goto __edl_retry;
        }

__firehose_main:
        retval = firehose_main(firehose_dir, usb_handle, qusb_zlp_mode);
        if(retval == 0)
        {
            get_duration(start);
        }
    }

    qusb_noblock_close(usb_handle);

    if (firehose_dir) free(firehose_dir);
    if (module_port_name) free(module_port_name);
    if (module_sys_path) free(module_sys_path);

    dbg_time("Upgrade module %s.\n", retval == 0 ? "successfully" : "failed");
    if (loghandler) fclose(loghandler);
    if (retval) update_transfer_bytes(-1);
	if (usbmon_logfile) ql_stop_usbmon_log();
	
    return retval;
}

double get_now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
}

void get_duration(double start)
{
    dbg_time("THE TOTAL DOWNLOAD TIME IS %.3f s\n",(get_now() - start));
}

void set_transfer_allbytes(long long bytes)
{
    transfer_bytes = 0;
    all_bytes_to_transfer = bytes;
}

int update_progress_msg(int percent);
int update_progress_file(int percent);
/*
return percent
*/
int update_transfer_bytes(long long bytes_cur)
{
    static int last_percent = -1;
    int percent = 0;

    if (bytes_cur == -1 || bytes_cur == 0)
    {
        percent = bytes_cur;
    }
    else
    {
        transfer_bytes += bytes_cur;
        percent = (transfer_bytes * 100) / all_bytes_to_transfer;
    }

    if (percent != last_percent)
    {
        last_percent = percent;
#ifdef USE_IPC_FILE
        update_progress_file(percent);
#endif
#ifdef USE_IPC_MSG
        update_progress_msg(percent);
#endif
    }

    return percent;
}

void show_progress()
{
    static int percent = 0;

    if (all_bytes_to_transfer)
        percent = (transfer_bytes * 100) / all_bytes_to_transfer;
    dbg_time("upgrade progress %d%% %lld/%lld\n", percent, transfer_bytes, all_bytes_to_transfer);
}

#ifdef USE_IPC_FILE
#define IPC_FILE_ANDROID "/data/update.conf"
#define IPC_FILE_LINUX "/tmp/update.conf"
int update_progress_file(int percent)
{
    static int ipcfd = -1;
    char buff[16];

    if (ipcfd < 0)
    {
#ifdef ANDROID
        const char *ipc_file = IPC_FILE_ANDROID;
#else
        const char *ipc_file = IPC_FILE_LINUX;
#endif
        /* Have set umask previous, no need to call fchmod */
        ipcfd = open(ipc_file, O_TRUNC | O_CREAT | O_WRONLY | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
        if (ipcfd < 0)
        {
            dbg_time("Fail to open(O_WRONLY) %s: %s\n", ipc_file, strerror(errno));
            return -1;
        }
    }

    lseek(ipcfd, 0, SEEK_SET);
    snprintf(buff, sizeof(buff), "%d", percent);
    if (write(ipcfd, buff, strlen(buff)) < 0)
        dbg_time("fail to write upgrade progress into %s: %s\n", ipc_file, strerror(errno));

    if (percent == 100 || percent < 0)
        close(ipcfd);
    return 0;
}
#endif

#ifdef USE_IPC_MSG
#define MSGBUFFSZ 16
struct message
{
    long mtype;
    char mtext[MSGBUFFSZ];
};

#define MSG_FILE "/etc/passwd"
#define MSG_TYPE_IPC 1
static int msg_get()
{
    key_t key = ftok(MSG_FILE, 'a');
    int msgid = msgget(key, IPC_CREAT | 0644);

    if (msgid < 0)
    {
        dbg_time("msgget fail: key %d, %s\n", key, strerror(errno));
        return -1;
    }
    return msgid;
}

static int msg_rm(int msgid)
{
    return msgctl(msgid, IPC_RMID, 0);
}

static int msg_send(int msgid, long type, const char *msg)
{
    struct message info;
    info.mtype = type;
    snprintf(info.mtext, MSGBUFFSZ, "%s", msg);
    if (msgsnd(msgid, (void *)&info, MSGBUFFSZ, IPC_NOWAIT) < 0)
    {
        dbg_time("msgsnd faild: msg %s, %s\n", msg, strerror(errno));
        return -1;
    }
    return 0;
}

static int msg_recv(int msgid, struct message *info)
{
    if (msgrcv(msgid, (void *)info, MSGBUFFSZ, info->mtype, IPC_NOWAIT) < 0)
    {
        dbg_time("msgrcv faild: type %ld, %s\n", info->mtype, strerror(errno));
        return -1;
    }
    return 0;
}

/**
 * this function will not delete the msg queue
 */
int update_progress_msg(int percent)
{
    char buff[MSGBUFFSZ];
    int msgid = msg_get();
    if (msgid < 0)
        return -1;
    snprintf(buff, sizeof(buff), "%d", percent);

#ifndef IPC_TEST
    return msg_send(msgid, MSG_TYPE_IPC, buff);
#else
    msg_send(msgid, MSG_TYPE_IPC, buff);
    struct message info;
    info.mtype = MSG_TYPE_IPC;
    msg_recv(msgid, &info);
    printf("msg queue read: %s\n", info.mtext);
#endif
}
#endif
