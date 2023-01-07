#include "qlog.h"

struct CSDLFileHeader
{
	uint32_t    dwHeaderVersion;//0x0
	uint32_t    dwDataFormat;//0x1
	uint32_t    dwAPVersion;
	uint32_t    dwCPVersion;
	uint32_t    dwSequenceNum;//文件序号，从0开始递增
	uint32_t    dwTime;//Total seconds from 1970.1.1 0:0:0
	uint32_t    dwCheckSum;//0x0
};

int g_is_asr_chip = 0;

ssize_t asr_send_cmd(int ttyfd, const unsigned char *buf, size_t size) {
    ssize_t wc = 0;

    while (wc < size) {
        uint32_t *cmd_data = (uint32_t *)(buf+wc);
        unsigned cmd_len =  qlog_le32(cmd_data[0]);
        //unsigned i;

        if (cmd_len > (size - wc))
            break;

        //qlog_dbg("Send CMD to UE: ");
        //for(i=0; i<cmd_len; i++)
        //    printf("0x%02X ", buf[wc+i]);
        //printf("\n");        

        if (write(ttyfd, buf+wc, cmd_len) != cmd_len)
            break;
        wc += cmd_len; 
    }

    return wc ;
}

static int asr_init_filter(int ttyfd, const char *cfg) {
    unsigned char ACATReady[16] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// ACAT Ready command
    unsigned char GetAPDBVer[16] = {0x10, 0x00, 0x00, 0x00, 0x80, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// Get AP DB Ver command
    unsigned char GetCPDBVer[16] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};	// Get CP DB Ver command
 
    //Send the ACAT Ready to UE
    usleep(200);
    asr_send_cmd(ttyfd, ACATReady, 16);
    usleep(200);
    asr_send_cmd(ttyfd, GetAPDBVer, 16);
    usleep(300);
    asr_send_cmd(ttyfd, GetCPDBVer, 16);

    return 0;
}

static int asr_logfile_init(int logfd, unsigned logfile_seq) {
    struct CSDLFileHeader SDLHeader;
    size_t size = sizeof(struct CSDLFileHeader);
    ssize_t nbytes;
    time_t ltime;

    // init the SDL file header
    SDLHeader.dwHeaderVersion = 0x0;
    SDLHeader.dwDataFormat = qlog_le32(0x1);//0x1
    SDLHeader.dwAPVersion = 0x0;
    SDLHeader.dwCPVersion = 0x0;
    SDLHeader.dwTime = 0x0;//Total seconds from 1970.1.1 0:0:0
    SDLHeader.dwCheckSum = 0x0;//0x0

    time(&ltime);

    SDLHeader.dwTime = qlog_le32(ltime); //Total seconds from 1970.1.1 0:0:0
    SDLHeader.dwSequenceNum = qlog_le32(logfile_seq); //文件序号，从0开始递增

    // Write the file header.
    nbytes = write(logfd, &SDLHeader, size);
    if (nbytes != size) {
        qlog_dbg("write %zd/%zd, errno: %d (%s)\n", nbytes, size, errno, strerror(errno));
    }
	
    return 0;
}

qlog_ops_t asr_qlog_ops = {
    .init_filter = asr_init_filter,
    .logfile_init = asr_logfile_init,
};
