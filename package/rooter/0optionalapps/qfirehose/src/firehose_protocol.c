/******************************************************************************
  @file    firehose_protocol.c
  @brief   firehose protocol.

  DESCRIPTION
  QFirehoe Tool for USB and PCIE of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#include "usb_linux.h"
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>

#define error_return()  do {dbg_time("%s %s %d fail\n", __FILE__, __func__, __LINE__); return __LINE__; } while(0)
int recv_sc600y_configure_num = 1;
extern const char *q_device_type;
static int fh_recv_cmd_sk[2];

extern unsigned q_erase_all_before_download;
extern int update_transfer_bytes(long long bytes_cur);
extern int show_progress();

struct fh_configure_cmd {
    const char *type;
    const char *MemoryName;
    uint32_t Verbose;
    uint32_t AlwaysValidate;
    uint32_t MaxDigestTableSizeInBytes;
    uint32_t MaxPayloadSizeToTargetInBytes;
    uint32_t MaxPayloadSizeFromTargetInBytes ; 			//2048
    uint32_t MaxPayloadSizeToTargetInByteSupported;		//16k
    uint32_t ZlpAwareHost;
    uint32_t SkipStorageInit;
};

struct fh_erase_cmd {
    const char *type;
    //uint32_t PAGES_PER_BLOCK;
    //uint32_t SECTOR_SIZE_IN_BYTES;
    //char label[32];
    uint32_t last_sector;
    uint32_t num_partition_sectors;
    //uint32_t physical_partition_number;
    uint32_t start_sector;
};

struct fh_program_cmd {
    const char *type;
    char *filename;
    uint32_t filesz;
    //uint32_t PAGES_PER_BLOCK;
    uint32_t SECTOR_SIZE_IN_BYTES;
    //char label[32];
    //uint32_t last_sector;
    uint32_t num_partition_sectors;
    //uint32_t physical_partition_number;
    uint32_t start_sector;
};

struct fh_response_cmd {
    const char *type;
    const char *value;
    uint32_t rawmode;
    uint32_t MaxPayloadSizeToTargetInBytes;
};

struct fh_log_cmd {
    const char *type;
};

struct fh_patch_cmd {
    const char *type;
    char *filename;
    uint32_t filesz;
    uint32_t SECTOR_SIZE_IN_BYTES;
    uint32_t num_partition_sectors;
};

struct fh_cmd_header {
    const char *type;
};

struct fh_vendor_defines {
    const char *type; // "vendor"
};

struct fh_cmd {
    union {
        struct fh_cmd_header cmd;
        struct fh_configure_cmd cfg;
        struct fh_erase_cmd erase;
        struct fh_program_cmd program;
        struct fh_response_cmd response;
        struct fh_log_cmd log;
        struct fh_patch_cmd patch;
        struct fh_vendor_defines vdef;
    };
    int part_upgrade; 
    char xml_original_data[512];
};

struct fh_data {
    const char *firehose_dir;
    const void *usb_handle;
    unsigned MaxPayloadSizeToTargetInBytes;
    unsigned fh_cmd_count;
    unsigned fh_patch_count;
    unsigned ZlpAwareHost;
    struct fh_cmd fh_cmd_table[256]; //AG525 have more than 64 partition
    
    unsigned xml_tx_size;
    unsigned xml_rx_size;
    char xml_tx_buf[1024];
    char xml_rx_buf[1024];
};

static const char * fh_xml_find_value(const char *xml_line, const char *key, char **ppend) {
    char *pchar = strstr(xml_line, key);
    char *pend;

    if (!pchar) {
        dbg_time("%s: no key %s in %s\n", __func__, key, xml_line);
        return NULL;
    }

    pchar += strlen(key);
    if (pchar[0] != '=' && pchar[1] != '"') {
        dbg_time("%s: no start %s in %s\n", __func__, "=\"", xml_line);
        return NULL;
    }

    pchar += strlen("=\"");
    pend = strstr(pchar, "\"");
    if (!pend) {
        dbg_time("%s: no end %s in %s\n", __func__, "\"", xml_line);
        return NULL;
    }

    *ppend = pend;
    return pchar;
}

static const char * fh_xml_get_value(const char *xml_line, const char *key) {
    static char value[64];
    char *pend;
    const char *pchar = fh_xml_find_value(xml_line, key, &pend);

    if (!pchar) {
        return NULL;
    }
    
    strncpy(value, pchar, pend - pchar);
    value[pend - pchar] = '\0';

    return value;
}

static void fh_xml_set_value(char *xml_line, const char *key, unsigned value) {
    char *pend;
    const char *pchar = fh_xml_find_value(xml_line, key, &pend);
    char *tmp_line = strdup(xml_line);
    char value_str[32];

    if (!pchar || !tmp_line) {
        return;
    }

    snprintf(value_str, sizeof(value_str), "%u", value);
    tmp_line[pchar - xml_line] = '\0';
    strcat(tmp_line, value_str);
    strcat(tmp_line, pend);

    strcpy(xml_line, tmp_line);
    free(tmp_line);
}

static int fh_parse_xml_line(const char *xml_line, struct fh_cmd *fh_cmd) {
    const char *pchar = NULL;
    size_t len = strlen(xml_line);
    
    memset(fh_cmd, 0, sizeof( struct fh_cmd));
    strcpy(fh_cmd->xml_original_data, xml_line);
    if (fh_cmd->xml_original_data[len - 1] == '\n')
        fh_cmd->xml_original_data[len - 1] = '\0';

    if (strstr(xml_line, "vendor=\"quectel\"")) {
        fh_cmd->vdef.type = "vendor";
        return 0;
    }
    else if (!strncmp(xml_line, "<erase ", strlen("<erase "))) {
        fh_cmd->erase.type = "erase";
        if (strstr(xml_line, "last_sector")) {
            if ((pchar = fh_xml_get_value(xml_line, "last_sector")))
                fh_cmd->erase.last_sector = atoi(pchar);		
        }
        if ((pchar = fh_xml_get_value(xml_line, "start_sector")))
            fh_cmd->erase.start_sector = atoi(pchar);
        if ((pchar = fh_xml_get_value(xml_line, "num_partition_sectors")))
            fh_cmd->erase.num_partition_sectors = atoi(pchar);

        return 0;
    }
    else if (!strncmp(xml_line, "<program ", strlen("<program "))) {
        fh_cmd->program.type = "program";
        if ((pchar = fh_xml_get_value(xml_line, "filename")))
        {
            fh_cmd->program.filename = strdup(pchar);                          
            if(fh_cmd->program.filename[0] == '\0')
            {//some fw version have blank program line, ignore it.
                return -1;
            }
        }        
        if ((pchar = fh_xml_get_value(xml_line, "start_sector")))
            fh_cmd->program.start_sector = atoi(pchar);
        if ((pchar = fh_xml_get_value(xml_line, "num_partition_sectors")))
            fh_cmd->program.num_partition_sectors = atoi(pchar);
        if ((pchar = fh_xml_get_value(xml_line, "SECTOR_SIZE_IN_BYTES")))
            fh_cmd->program.SECTOR_SIZE_IN_BYTES = atoi(pchar);

        return 0;
    }
    else if (!strncmp(xml_line, "<patch ", strlen("<patch "))) {
        fh_cmd->patch.type = "patch";
        pchar = fh_xml_get_value(xml_line, "filename");
        if (pchar && strcmp(pchar, "DISK"))
            return -1;
        return 0;
    }
    else if (!strncmp(xml_line, "<response ", strlen("<response "))) {
        fh_cmd->response.type = "response";
        pchar = fh_xml_get_value(xml_line, "value");
        if (pchar) {
            if (!strcmp(pchar, "ACK"))
                fh_cmd->response.value =  "ACK";
            else if(!strcmp(pchar, "NAK"))
                fh_cmd->response.value =  "NAK";
            else
                 fh_cmd->response.value =  "OTHER";               
        }
        if (strstr(xml_line, "rawmode")) {
            pchar = fh_xml_get_value(xml_line, "rawmode");
            if (pchar) {
                fh_cmd->response.rawmode = !strcmp(pchar, "true");
            }
        }
        else if (strstr(xml_line, "MaxPayloadSizeToTargetInBytes")) {
            pchar = fh_xml_get_value(xml_line, "MaxPayloadSizeToTargetInBytes");
            if (pchar) {
                fh_cmd->response.MaxPayloadSizeToTargetInBytes = atoi(pchar);
            }
        }
        return 0;
    }
    else if (!strncmp(xml_line, "<log ", strlen("<log "))) {
        fh_cmd->program.type = "log";
        return 0;
    }

    error_return();
}

static int fh_parse_xml_file(struct fh_data *fh_data, const char *xml_file) {
    FILE *fp = fopen(xml_file, "rb");
    
    if (fp == NULL) {
        dbg_time("%s fail to fopen(%s), errno: %d (%s)\n", __func__, xml_file, errno, strerror(errno));
        error_return();
    }

    while (fgets(fh_data->xml_tx_buf, fh_data->xml_tx_size, fp)) {
        char *xml_line = strstr(fh_data->xml_tx_buf, "<");
        char *c_start = NULL;

        if (!xml_line)
            continue;

        c_start = strstr(xml_line, "<!--");
        if (c_start) {
            char *c_end = strstr(c_start, "-->");

            if (c_end) {
                /* 
                <erase case 1 /> <!-- xxx -->
                <!-- xxx --> <erase case 2 /> 
                <!-- <erase case 3 /> -->
                */
                char *tmp = strstr(xml_line, "/>");
                if (tmp && (tmp < c_start || tmp > c_end)) {
                    memset(c_start, ' ', c_end - c_start + strlen("-->"));
                    goto __fh_parse_xml_line;
                }

                continue;
            }
            else {
                /* 
                     <!-- line1
                             <! -- line2 -->
                      -->
                */
                do {
                    if (fgets(fh_data->xml_tx_buf, fh_data->xml_tx_size, fp) == NULL) { break; };
                    xml_line = fh_data->xml_tx_buf;
                } while (!strstr(xml_line, "-->") && strstr(xml_line, "<!--"));

                continue;
            }
        }

__fh_parse_xml_line:		
        if (xml_line) {
            char *tag = NULL;

            tag = strstr(xml_line, "<erase ");
            if (!tag) {
                tag = strstr(xml_line, "<program ");
                if (!tag) {
                    tag = strstr(xml_line, "<patch ");
                }
            }

            if (tag) {
                if (!fh_parse_xml_line(tag, &fh_data->fh_cmd_table[fh_data->fh_cmd_count])) {
                    fh_data->fh_cmd_count++;
                    if (strstr(tag, "<patch "))
                        fh_data->fh_patch_count++;
                }
            }
            else if (!strstr(xml_line, "<?xml") && !strcmp(xml_line, "<data>") && !strcmp(xml_line, "</data>")
                && !strcmp(xml_line, "<patches>") && !strcmp(xml_line, "<patches>")) {
                dbg_time("unspport xml_line '%s'\n", xml_line);
                exit(-1);
            }
        }
    }

    fclose(fp);

    return 0;
}

static int fh_fixup_program_cmd(struct fh_data *fh_data, struct fh_cmd *fh_cmd, long* filesize_out) {
    char full_path[512];
    char *unix_filename = strdup(fh_cmd->program.filename);
    char *ptmp;
    FILE *fp;
    long filesize = 0;
    uint32_t num_partition_sectors = fh_cmd->program.num_partition_sectors;

    while((ptmp = strchr(unix_filename, '\\'))) {
        *ptmp = '/';
    }    

    snprintf(full_path, sizeof(full_path), "%.255s/%.240s", fh_data->firehose_dir, unix_filename);
    if (access(full_path, R_OK)) {
        fh_cmd->program.num_partition_sectors = 0;
        dbg_time("fail to access %s, errno: %d (%s)\n", full_path, errno, strerror(errno));
        error_return();
    }

    fp = fopen(full_path, "rb");
    if (!fp) {
        fh_cmd->program.num_partition_sectors = 0;
        dbg_time("fail to fopen %s, errno: %d (%s)\n", full_path, errno, strerror(errno));
        error_return();
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    *filesize_out = filesize;
    fclose(fp);

    if (filesize <= 0) {
        dbg_time("fail to ftell %s, errno: %d (%s)\n", full_path, errno, strerror(errno));
        fh_cmd->program.num_partition_sectors = 0;
        fh_cmd->program.filesz = 0;
        error_return();
    }
    fh_cmd->program.filesz = filesize;

    fh_cmd->program.num_partition_sectors = filesize/fh_cmd->program.SECTOR_SIZE_IN_BYTES;
    if (filesize%fh_cmd->program.SECTOR_SIZE_IN_BYTES)
        fh_cmd->program.num_partition_sectors += 1;

    if (num_partition_sectors != fh_cmd->program.num_partition_sectors) {
        fh_xml_set_value(fh_cmd->xml_original_data, "num_partition_sectors",
            fh_cmd->program.num_partition_sectors);
    }
    
    free(unix_filename);

    return 0;
}

static int _fh_recv_cmd(struct fh_data *fh_data, struct fh_cmd *fh_cmd, unsigned timeout) {  
    int ret;
    char *xml_line;
    char *pend;

    memset(fh_cmd, 0, sizeof(struct fh_cmd));    

    ret = qusb_noblock_read(fh_data->usb_handle, fh_data->xml_rx_buf, fh_data->xml_rx_size, 1, timeout);
    if (ret <= 0) {
        return -1;
    }
    fh_data->xml_rx_buf[ret] = '\0';

   xml_line = fh_data->xml_rx_buf;
    while (*xml_line) {
        xml_line = strstr(xml_line, "<?xml version=");
        if (xml_line == NULL) {
            if (fh_cmd->cmd.type == 0) {
                dbg_time("{{{%s}}}", fh_data->xml_rx_buf);
                error_return();
            } else {
                break;
            }
        }
        xml_line += strlen("<?xml version=");

        xml_line = strstr(xml_line, "<data>");
        if (xml_line == NULL) {
            dbg_time("{{{%s}}}", fh_data->xml_rx_buf);
            error_return();
        }
        xml_line += strlen("<data>");
        if (xml_line[0] == '\n')
            xml_line++;
		
        if (!strncmp(xml_line, "<response ", strlen("<response "))) {
            fh_parse_xml_line(xml_line, fh_cmd);
            pend = strstr(xml_line, "/>");
            pend += 2;
            dbg_time("%.*s\n", (int)(pend -xml_line),  xml_line);
            xml_line = pend + 1;
        }
        else if (!strncmp(xml_line, "<log ", strlen("<log "))) {
            if (fh_cmd->cmd.type && strcmp(fh_cmd->cmd.type, "log")) {
                dbg_time("{{{%s}}}", fh_data->xml_rx_buf);
                break;
            }
            fh_parse_xml_line(xml_line, fh_cmd);
            pend = strstr(xml_line, "/>");
            pend += 2;
            {
                char *prn = xml_line;
                while (prn < pend) {
                    if (*prn == '\r' || *prn == '\n')
                        *prn = '.';
                    prn++;
                }
            }
            dbg_time("%.*s\n", (int)(pend -xml_line),  xml_line);
            xml_line = pend + 1;
        } else {
            dbg_time("unkonw %s", xml_line);
            error_return();
        }
    }

    if (fh_cmd->cmd.type)
        return 0;

    error_return();
}

static void * fh_recv_cmd_thread(void *arg) {
    struct fh_data *fh_data = (struct fh_data *)arg;
    struct fh_cmd fh_rx_cmd;

    while (_fh_recv_cmd(fh_data, &fh_rx_cmd, -1) ==  0) {
        if (write(fh_recv_cmd_sk[1], &fh_rx_cmd, sizeof(fh_rx_cmd)) == -1) { };
    }

    return NULL;
}

static int fh_recv_cmd(struct fh_data *fh_data, struct fh_cmd *fh_cmd, unsigned timeout, int ignore_timeout) {
    struct pollfd pollfds[] = {{fh_recv_cmd_sk[0], POLLIN, 0}};
    int ret = poll(pollfds, 1, timeout);

    (void)fh_data;
    if (ret == 1 && (pollfds[0].revents & POLLIN)) {
        ret = read(fh_recv_cmd_sk[0], fh_cmd, sizeof(struct fh_cmd));
        if (ret == sizeof(struct fh_cmd))
            return 0;
    }
    else if (ret == 0 && ignore_timeout) {
        return __LINE__;
    }

    error_return();
}

static int fh_wait_response_cmd(struct fh_data *fh_data, struct fh_cmd *fh_cmd, unsigned timeout) { 
    while (1) {
        int ret = fh_recv_cmd(fh_data, fh_cmd, timeout, 0);

        if (ret !=0)
            error_return();

        if (strstr(fh_cmd->cmd.type, "log"))
            continue;

        return 0;
    }

    error_return();
}

static int fh_send_cmd(struct fh_data *fh_data, const struct fh_cmd *fh_cmd) {  
    int tx_len = 0;
    char *pstart, *pend;
    char *xml_buf = fh_data->xml_tx_buf;
    unsigned xml_size = fh_data->xml_tx_size;
    xml_buf[0] = '\0';

    snprintf(xml_buf + strlen(xml_buf), xml_size, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
    snprintf(xml_buf + strlen(xml_buf), xml_size, "<data>\n");

    pstart = xml_buf + strlen(xml_buf);
    if (!strcmp(fh_cmd->cmd.type, "vendor")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size, "%s", fh_cmd->xml_original_data);
    }
    else if (!strcmp(fh_cmd->cmd.type, "erase")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size, "%s", fh_cmd->xml_original_data);
    }
    else if (!strcmp(fh_cmd->cmd.type, "program")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size, "%s", fh_cmd->xml_original_data);
    }
    else if (!strcmp(fh_cmd->cmd.type, "patch")) {	
        snprintf(xml_buf + strlen(xml_buf), xml_size, "%s", fh_cmd->xml_original_data);
    }
    else if (!strcmp(fh_cmd->cmd.type, "configure")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size,  
            "<configure MemoryName=\"%.8s\" Verbose=\"%d\" AlwaysValidate=\"%d\" MaxDigestTableSizeInBytes=\"%d\" MaxPayloadSizeToTargetInBytes=\"%d\"  ZlpAwareHost=\"%d\" SkipStorageInit=\"%d\" />",
            fh_cmd->cfg.MemoryName, fh_cmd->cfg.Verbose, fh_cmd->cfg.AlwaysValidate,
            fh_cmd->cfg.MaxDigestTableSizeInBytes, 
            fh_cmd->cfg.MaxPayloadSizeToTargetInBytes,
            fh_cmd->cfg.ZlpAwareHost, fh_cmd->cfg.SkipStorageInit);
    }
    else if (!strcmp(fh_cmd->cmd.type, "setbootablestoragedrive")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size, "<setbootablestoragedrive value=\"%d\" />",
            !strcmp(q_device_type, "ufs") ? 1 : 0);
    }
    else if (!strcmp(fh_cmd->cmd.type, "reset")) {
        snprintf(xml_buf + strlen(xml_buf), xml_size, "<power DelayInSeconds=\"%u\" value=\"reset\" />", 10);
    }
    else {
        dbg_time("%s unkonw fh_cmd->cmd.type=%s\n", __func__, fh_cmd->cmd.type);
        error_return();
    }
    
    pend = xml_buf + strlen(xml_buf);
    dbg_time("%.*s\n", (int)(pend - pstart),  pstart);
    snprintf(xml_buf + strlen(xml_buf), xml_size, "\n</data>");

    tx_len = qusb_noblock_write(fh_data->usb_handle, xml_buf, strlen(xml_buf), strlen(xml_buf), 3000, fh_data->ZlpAwareHost);

    if ((size_t)tx_len == strlen(xml_buf))
        return 0;

    error_return();
}

static int fh_send_cfg_cmd(struct fh_data *fh_data, const char *device_type) {
    struct fh_cmd fh_cfg_cmd;
    struct fh_cmd fh_rx_cmd;

    memset(&fh_cfg_cmd, 0x00, sizeof(fh_cfg_cmd));
    fh_cfg_cmd.cfg.type = "configure";
    fh_cfg_cmd.cfg.MemoryName = device_type;
    fh_cfg_cmd.cfg.Verbose = 0;
    fh_cfg_cmd.cfg.AlwaysValidate = 0;
    fh_cfg_cmd.cfg.SkipStorageInit = 0;
    fh_cfg_cmd.cfg.ZlpAwareHost = fh_data->ZlpAwareHost; // only sdx20 support zlp set to 0 by 20180822
    if (!strcmp(device_type, "emmc") || !strcmp(device_type, "ufs"))
    {
        fh_cfg_cmd.cfg.MaxDigestTableSizeInBytes = 8192;
        fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInBytes = 1048576;
        fh_cfg_cmd.cfg.MaxPayloadSizeFromTargetInBytes = 8192;
        fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInByteSupported = 1048576;
    }
    else
    {
        fh_cfg_cmd.cfg.MaxDigestTableSizeInBytes = 2048;
        fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInBytes = 8192;
        fh_cfg_cmd.cfg.MaxPayloadSizeFromTargetInBytes = 2048;
        fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInByteSupported = 8192;
    }

    fh_send_cmd(fh_data, &fh_cfg_cmd);
    if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 3000) != 0)
        error_return();
	
    if (!strcmp(fh_rx_cmd.response.value, "NAK") && fh_rx_cmd.response.MaxPayloadSizeToTargetInBytes) {
         fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInBytes = fh_rx_cmd.response.MaxPayloadSizeToTargetInBytes;
         fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInByteSupported = fh_rx_cmd.response.MaxPayloadSizeToTargetInBytes;

        fh_send_cmd(fh_data, &fh_cfg_cmd);
        if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 3000) != 0)
            error_return();
    }

    if (strcmp(fh_rx_cmd.response.value, "ACK") != 0)
        error_return();

    fh_data->MaxPayloadSizeToTargetInBytes = fh_cfg_cmd.cfg.MaxPayloadSizeToTargetInBytes;

    return 0;
}

static int fh_send_setbootablestoragedrive_cmd(struct fh_data *fh_data) {
    struct fh_cmd fh_0_cmd;
    fh_0_cmd.cmd.type = "setbootablestoragedrive";

    return fh_send_cmd(fh_data, &fh_0_cmd);
}

static int fh_send_reset_cmd(struct fh_data *fh_data) {
    struct fh_cmd fh_reset_cmd;
    fh_reset_cmd.cmd.type = "reset";

    return fh_send_cmd(fh_data, &fh_reset_cmd);
}

static int fh_send_rawmode_image(struct fh_data *fh_data, const struct fh_cmd *fh_cmd, unsigned timeout) {
    char full_path[512];
    char *unix_filename = strdup(fh_cmd->program.filename);
    char *ptmp;
    FILE *fp;
    size_t filesize, filesend;
    void *pbuf = malloc(fh_data->MaxPayloadSizeToTargetInBytes);

    if (pbuf == NULL)
        error_return();
    
    while((ptmp = strchr(unix_filename, '\\'))) {
        *ptmp = '/';
    }

    snprintf(full_path, sizeof(full_path), "%.255s/%.240s", fh_data->firehose_dir, unix_filename);
    fp = fopen(full_path, "rb");
    if (!fp) {
        dbg_time("fail to fopen %s, errno: %d (%s)\n", full_path, errno, strerror(errno));
        error_return();
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    filesend = 0;
    fseek(fp, 0, SEEK_SET);        

    dbg_time("send %s, filesize=%zd\n", unix_filename, filesize);
    int idx = -1;
    while (filesend < filesize) {
        size_t reads = fread(pbuf, 1, fh_data->MaxPayloadSizeToTargetInBytes, fp);
        update_transfer_bytes(reads);
        if (!((++idx) % 0x80)) {
            printf(".");
            fflush(stdout);
        }

        if (reads > 0) {
            if (reads % fh_cmd->program.SECTOR_SIZE_IN_BYTES) {
                memset((uint8_t *)pbuf + reads, 0, fh_cmd->program.SECTOR_SIZE_IN_BYTES - (reads % fh_cmd->program.SECTOR_SIZE_IN_BYTES));
                reads +=  fh_cmd->program.SECTOR_SIZE_IN_BYTES - (reads % fh_cmd->program.SECTOR_SIZE_IN_BYTES);
            }
            size_t writes = qusb_noblock_write(fh_data->usb_handle, pbuf, reads, reads, timeout, fh_data->ZlpAwareHost);                  
            if (reads != writes) {
                dbg_time("%s send fail reads=%zd, writes=%zd\n", __func__, reads, writes);
                dbg_time("%s send fail filesend=%zd, filesize=%zd\n", __func__, filesend, filesize);
                break;
            }
            filesend += reads;
            //dbg_time("filesend=%zd, filesize=%zd\n", filesend, filesize);
        } else {
            break;
        }
    }
    printf("\n");
    show_progress();
    dbg_time("send finished\n");

    fclose(fp);   
    free(unix_filename);
    free(pbuf);

    if (filesend >= filesize)
        return 0;

    error_return();
}

static int fh_process_erase(struct fh_data *fh_data, const struct fh_cmd *fh_cmd)
{
    struct fh_cmd fh_rx_cmd;
    unsigned timeout = 15000; //8+8 MCP need more time

    fh_send_cmd(fh_data, fh_cmd);
    if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, timeout) != 0) //SDX55 need 4 seconds
        error_return();
    if (strcmp(fh_rx_cmd.response.value, "ACK"))
        error_return();

    return 0;
}

static int fh_process_patch(struct fh_data *fh_data, const struct fh_cmd *fh_cmd)
{
    struct fh_cmd fh_rx_cmd;
    unsigned timeout = 15000; //8+8 MCP need more time

    fh_send_cmd(fh_data, fh_cmd);
    if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, timeout) != 0) //SDX55 need 4 seconds
    {
        dbg_time("fh_process_patch : fh_wait_response_cmd fail\n");
        error_return();
    }
    if (strcmp(fh_rx_cmd.response.value, "ACK"))
    {
        dbg_time("fh_process_patch : response should be ACK\n");
        error_return();
    }

    return 0;
}

static int fh_process_program(struct fh_data *fh_data, const struct fh_cmd *fh_cmd)
{
    struct fh_cmd fh_rx_cmd;

    fh_send_cmd(fh_data, fh_cmd);
    if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 3000) != 0) {
        dbg_time("fh_wait_response_cmd fail\n");
        error_return();
     }
     if (strcmp(fh_rx_cmd.response.value, "ACK")) {
        dbg_time("response should be ACK\n");
        error_return();
     }
     if (fh_rx_cmd.response.rawmode != 1) {
        dbg_time("response should be rawmode true\n");
        error_return();
     }
     if (fh_send_rawmode_image(fh_data, fh_cmd, 15000)) {
        dbg_time("fh_send_rawmode_image fail\n");
        error_return();
     }
     if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 6000) != 0) {
        dbg_time("fh_wait_response_cmd fail\n");
        error_return();
     }
     if (strcmp(fh_rx_cmd.response.value, "ACK")) {
        dbg_time("response should be ACK\n");
        error_return();
     }
     if (fh_rx_cmd.response.rawmode != 0) {
        dbg_time("response should be rawmode false\n");
        error_return();
     }

    free(fh_cmd->program.filename);

    return 0;
}

int firehose_main(const char *firehose_dir, void *usb_handle, unsigned qusb_zlp_mode) {
    unsigned x;
    char rawprogram_full_path[512];
    char *xmlfile_list[32];
    char xmlfile_tmp[32];
    unsigned xmlfile_cnt = 0;
    struct fh_cmd fh_rx_cmd;
    struct fh_data *fh_data;
    long long filesizes = 0;
    long filesize = 0;
    unsigned max_num_partition_sectors = 0;
    static pthread_t recv_cmd_tid;
    int first_earse_and_last_programm_SBL = 0;

    fh_data = (struct fh_data *)malloc(sizeof(struct fh_data));
    if (!fh_data)
        error_return();
    
    memset(fh_data, 0x00, sizeof(struct fh_data));
    fh_data->firehose_dir = firehose_dir;
    fh_data->usb_handle = usb_handle;
    fh_data->xml_tx_size = sizeof(fh_data->xml_tx_buf);
    fh_data->xml_rx_size = sizeof(fh_data->xml_rx_buf);
    fh_data->ZlpAwareHost = qusb_zlp_mode;
	
    if (!qfile_find_file(firehose_dir, "rawprogram_", ".xml", &xmlfile_list[xmlfile_cnt])
        && !qfile_find_file(firehose_dir, "firehose-rawprogram", ".xml", &xmlfile_list[xmlfile_cnt])
       ) {
        dbg_time("retrieve rawprogram namd file failed.\n");
        //error_return();
    }
    else
        xmlfile_cnt++;

    for (x = 0; x < 10; x++) {
        snprintf(xmlfile_tmp, sizeof(xmlfile_tmp), "rawprogram%u.xml", x);
        if (!qfile_find_file(firehose_dir, xmlfile_tmp, ".xml", &xmlfile_list[xmlfile_cnt])) {
            continue;
        }
        xmlfile_cnt++;
    }

    for (x = 0; x < 10; x++) {
        snprintf(xmlfile_tmp, sizeof(xmlfile_tmp), "patch%u.xml", x);
        if (!qfile_find_file(firehose_dir, xmlfile_tmp, ".xml", &xmlfile_list[xmlfile_cnt])) {
            continue;
        }
        xmlfile_cnt++;
    }

    for (x = 0; x < xmlfile_cnt; x++) {
        snprintf(rawprogram_full_path, sizeof(rawprogram_full_path), "%.255s/%.255s", firehose_dir, xmlfile_list[x]);
        free(xmlfile_list[xmlfile_cnt]);
        fh_parse_xml_file(fh_data, rawprogram_full_path);
    }
    
    if (fh_data->fh_cmd_count == 0)
        error_return();

    for (x = 0; x < fh_data->fh_cmd_count; x++) {
        struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];
        
        if (strstr(fh_cmd->cmd.type, "program")) {
            fh_fixup_program_cmd(fh_data, fh_cmd, &filesize);
            if (fh_cmd->program.num_partition_sectors == 0)
                error_return();
                
            //calc files size
            filesizes += filesize;
        }
        else if (strstr(fh_cmd->cmd.type, "erase")) {
            if ((fh_cmd->erase.num_partition_sectors + fh_cmd->erase.start_sector) > max_num_partition_sectors)
                max_num_partition_sectors = (fh_cmd->erase.num_partition_sectors + fh_cmd->erase.start_sector);   
        }
    }

    if (socketpair( AF_LOCAL, SOCK_STREAM, 0, fh_recv_cmd_sk))
        error_return();
    fcntl(fh_recv_cmd_sk[0], F_SETFL, O_NONBLOCK);
    if (pthread_create(&recv_cmd_tid, NULL, fh_recv_cmd_thread, (void *)fh_data))	
        error_return();
    set_transfer_allbytes(filesizes);
    //must first read <log from mdm9x07, then send <configure, and 1 second is not enough
    fh_recv_cmd(fh_data, &fh_rx_cmd, 3000, 1);
    while (fh_recv_cmd(fh_data, &fh_rx_cmd, 1000, 1) == 0);

    if (fh_send_cfg_cmd(fh_data, q_device_type))
        error_return();

    if (!strcmp(q_device_type, "nand")) 
        first_earse_and_last_programm_SBL = 1;

    if (first_earse_and_last_programm_SBL) {
        for (x = 0; x < fh_data->fh_cmd_count; x++) {
            struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];
            
            if (!strstr(fh_cmd->cmd.type, "erase"))
                continue;
                
             if (fh_cmd->erase.start_sector != 0)
                continue;

            if (q_erase_all_before_download) {
                fh_xml_set_value(fh_cmd->xml_original_data, "num_partition_sectors", max_num_partition_sectors);
                if (fh_cmd->erase.last_sector) {
                    fh_xml_set_value(fh_cmd->xml_original_data, "last_sector", max_num_partition_sectors - 1);
                }
            }

             if (fh_process_erase(fh_data, fh_cmd))
                error_return();
        }
    }

    for (x = 0; x < fh_data->fh_cmd_count; x++) {
        const struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];

        if (strstr(fh_cmd->cmd.type, "vendor")) {
            fh_send_cmd(fh_data, fh_cmd);
            if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 6000) != 0)
                error_return();
            if (strcmp(fh_rx_cmd.response.value, "ACK"))
                error_return();
        }
    }

    if (!q_erase_all_before_download) {
        for (x = 0; x < fh_data->fh_cmd_count; x++) {
            struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];
            
            if (!strstr(fh_cmd->cmd.type, "erase"))
                continue;

             if (fh_process_erase(fh_data, fh_cmd))
                error_return();
        }
    }
    
    for (x = 0; x < fh_data->fh_cmd_count; x++) {
        const struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];

        if (!strstr(fh_cmd->cmd.type, "program"))
            continue;

        if (first_earse_and_last_programm_SBL && fh_cmd->program.start_sector == 0)
            continue;

        if (fh_process_program(fh_data, fh_cmd))
            error_return();
    }

    if (first_earse_and_last_programm_SBL) {
        for (x = 0; x < fh_data->fh_cmd_count; x++) {
            const struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];

            if (!strstr(fh_cmd->cmd.type, "program"))
                continue;

            if (fh_cmd->program.start_sector != 0)
                continue;

            if (fh_process_program(fh_data, fh_cmd))
                error_return();
        }
    }
    
    if (fh_data->fh_patch_count) {
        for (x = 0; x < fh_data->fh_cmd_count; x++) {
            const struct fh_cmd *fh_cmd = &fh_data->fh_cmd_table[x];

            if (!strstr(fh_cmd->cmd.type, "patch"))
                continue;

            if (fh_process_patch(fh_data, fh_cmd))
                error_return();
        }
    }

    if (strcmp(q_device_type, "nand")) {
        fh_send_setbootablestoragedrive_cmd(fh_data);
        if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 3000) != 0)
            error_return();
    }
	
    fh_send_reset_cmd(fh_data);
    if (fh_wait_response_cmd(fh_data, &fh_rx_cmd, 3000) != 0)
        error_return();
    while (fh_recv_cmd(fh_data, &fh_rx_cmd, 1000, 1) == 0); //required by sdx20

    free(fh_data);

    //pthread_join(recv_cmd_tid, NULL);
    close(fh_recv_cmd_sk[0]);
    close(fh_recv_cmd_sk[1]);
    return 0;
}
