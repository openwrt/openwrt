/******************************************************************************

                               Copyright (c) 2010
                            Lantiq Deutschland GmbH
                     Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include <pjmedia-audiodev/audiodev_imp.h>
#include <pjmedia/errno.h>
#include <pj/assert.h>
#include <pj/pool.h>
#include <pj/log.h>
#include <pj/os.h>

/* Linux includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <poll.h>

#if PJMEDIA_AUDIO_DEV_HAS_TAPI_DEVICE
/* TAPI includes */
#include "drv_tapi_io.h"
#include "vmmc_io.h"

/* Maximum 2 devices */
#define TAPI_AUDIO_PORT_NUM          (2)
#define TAPI_BASE_NAME              "TAPI"
#define TAPI_LL_DEV_BASE_PATH       "/dev/vmmc"
#define TAPI_LL_DEV_FIRMWARE_NAME   "/lib/firmware/danube_firmware.bin"
#define TAPI_LL_BBD_NAME   "/lib/firmware/danube_bbd_fxs.bin"

#define TAPI_LL_DEV_SELECT_TIMEOUT_MS      (2000)
#define TAPI_LL_DEV_MAX_PACKET_SIZE        (800)
#define TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE   (12)
#define TAPI_LL_DEV_ENC_FRAME_LEN_MS       (20)
#define TAPI_LL_DEV_ENC_SMPL_PER_SEC       (8000)
#define TAPI_LL_DEV_ENC_BITS_PER_SMPLS     (16)
#define TAPI_LL_DEV_ENC_SMPL_PER_FRAME     (160)
#define TAPI_LL_DEV_ENC_BYTES_PER_FRAME    (TAPI_LL_DEV_ENC_SMPL_PER_FRAME * (TAPI_LL_DEV_ENC_BITS_PER_SMPLS / 8))

#define THIS_FILE     "tapi_dev.c"

#if 1
#   define TRACE_(x)    PJ_LOG(1,x)
#else
#   define TRACE_(x)
#endif

pj_int32_t ch_fd[TAPI_AUDIO_PORT_NUM];

typedef struct
{
	pj_int32_t dev_fd;
	pj_int32_t ch_fd[TAPI_AUDIO_PORT_NUM];
	pj_int8_t  data2phone_map[TAPI_AUDIO_PORT_NUM];
} tapi_ctx;

struct tapi_aud_factory
{
	pjmedia_aud_dev_factory	base;
	pj_pool_t		*pool;
	pj_pool_factory		*pf;
	pj_uint32_t		dev_count;
	pjmedia_aud_dev_info	*dev_info;
	tapi_ctx		dev_ctx;
};

typedef struct tapi_aud_factory tapi_aud_factory_t;

struct tapi_aud_stream
{
	pjmedia_aud_stream	base;
	pj_pool_t		*pool;
	pjmedia_aud_param	param;
	pjmedia_aud_rec_cb	rec_cb;
	pjmedia_aud_play_cb	play_cb;
	void			*user_data;

	pj_thread_desc		thread_desc;
	pj_thread_t		*thread;
	tapi_ctx		*dev_ctx;
	pj_uint8_t		run_flag;
	pj_timestamp		timestamp;
};

typedef struct tapi_aud_stream tapi_aud_stream_t;

/* Factory prototypes */
static pj_status_t factory_init(pjmedia_aud_dev_factory *f);
static pj_status_t factory_destroy(pjmedia_aud_dev_factory *f);
static unsigned    factory_get_dev_count(pjmedia_aud_dev_factory *f);
static pj_status_t factory_get_dev_info(pjmedia_aud_dev_factory *f,
          unsigned index,
          pjmedia_aud_dev_info *info);
static pj_status_t factory_default_param(pjmedia_aud_dev_factory *f,
           unsigned index,
           pjmedia_aud_param *param);
static pj_status_t factory_create_stream(pjmedia_aud_dev_factory *f,
           const pjmedia_aud_param *param,
           pjmedia_aud_rec_cb rec_cb,
           pjmedia_aud_play_cb play_cb,
           void *user_data,
           pjmedia_aud_stream **p_aud_strm);

/* Stream prototypes */
static pj_status_t stream_get_param(pjmedia_aud_stream *strm,
          pjmedia_aud_param *param);
static pj_status_t stream_get_cap(pjmedia_aud_stream *strm,
              pjmedia_aud_dev_cap cap,
              void *value);
static pj_status_t stream_set_cap(pjmedia_aud_stream *strm,
              pjmedia_aud_dev_cap cap,
              const void *value);
static pj_status_t stream_start(pjmedia_aud_stream *strm);
static pj_status_t stream_stop(pjmedia_aud_stream *strm);
static pj_status_t stream_destroy(pjmedia_aud_stream *strm);

static pjmedia_aud_dev_factory_op tapi_fact_op =
{
	&factory_init,
	&factory_destroy,
	&factory_get_dev_count,
	&factory_get_dev_info,
	&factory_default_param,
	&factory_create_stream
};

static pjmedia_aud_stream_op tapi_strm_op =
{
	&stream_get_param,
	&stream_get_cap,
	&stream_set_cap,
	&stream_start,
	&stream_stop,
	&stream_destroy
};

void (*tapi_digit_callback)(unsigned int port, unsigned char digit) = NULL;
void (*tapi_hook_callback)(unsigned int port, unsigned char event) = NULL;

static pj_int32_t
tapi_dev_open(char* dev_path, const pj_int32_t ch_num)
{
	char devname[128] = {0};
	pj_ansi_sprintf(devname,"%s%u%u", dev_path, 1, ch_num);
	return open((const char*)devname, O_RDWR, 0644);
}

static pj_status_t
tapi_dev_binary_buffer_create(const char *pPath, pj_uint8_t **ppBuf, pj_uint32_t *pBufSz)
{
	pj_status_t status = PJ_SUCCESS;
	FILE *fd;
	struct stat file_stat;

	fd = fopen(pPath, "rb");
	if (fd == NULL) {
		TRACE_((THIS_FILE, "ERROR -  binary file %s open failed!\n", pPath));
		return PJ_EUNKNOWN;
	}

	if (stat(pPath, &file_stat) != 0) {
		TRACE_((THIS_FILE, "ERROR -  file %s statistics get failed!\n", pPath));
		return PJ_EUNKNOWN;
	}

	*ppBuf = malloc(file_stat.st_size);
	if (*ppBuf == NULL) {
		TRACE_((THIS_FILE, "ERROR -  binary file %s memory allocation failed!\n", pPath));
		status = PJ_EUNKNOWN;
		goto on_exit;
	}

	if (fread (*ppBuf, sizeof(pj_uint8_t), file_stat.st_size, fd) <= 0) {
		TRACE_((THIS_FILE, "ERROR - file %s read failed!\n", pPath));
		status = PJ_EUNKNOWN;
		goto on_exit;
	}

	*pBufSz = file_stat.st_size;

on_exit:
	if (fd != NULL)
		fclose(fd);

	if (*ppBuf != NULL && status != PJ_SUCCESS)
		free(*ppBuf);

	return status;
}

static void
tapi_dev_binary_buffer_delete(pj_uint8_t *pBuf)
{
	if (pBuf != NULL)
		free(pBuf);
}

static pj_status_t
tapi_dev_firmware_download(pj_int32_t fd, const char *pPath)
{
	pj_status_t status = PJ_SUCCESS;
	pj_uint8_t *pFirmware = NULL;
	pj_uint32_t binSz = 0;
	VMMC_IO_INIT vmmc_io_init;

	status = tapi_dev_binary_buffer_create(pPath, &pFirmware, &binSz);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - binary buffer create failed!\n"));
		return PJ_EUNKNOWN;
	}

	memset(&vmmc_io_init, 0, sizeof(VMMC_IO_INIT));
	vmmc_io_init.pPRAMfw   = pFirmware;
	vmmc_io_init.pram_size = binSz;

	status = ioctl(fd, FIO_FW_DOWNLOAD, &vmmc_io_init);
	if (status != PJ_SUCCESS)
		TRACE_((THIS_FILE, "ERROR -  FIO_FW_DOWNLOAD ioctl failed!"));

	tapi_dev_binary_buffer_delete(pFirmware);

	return status;
}

static int
tapi_dev_bbd_download(int fd, const char *pPath)
{
	int status = PJ_SUCCESS;
	unsigned char *pFirmware = NULL;
	unsigned int binSz = 0;
	VMMC_DWLD_t bbd_data;


	/* Create binary buffer */
	status = tapi_dev_binary_buffer_create(pPath, &pFirmware, &binSz);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - binary buffer create failed!\n"));
		return status;
	}

	/* Download Voice Firmware */
	memset(&bbd_data, 0, sizeof(VMMC_DWLD_t));
	bbd_data.buf = pFirmware;
	bbd_data.size = binSz;

	status = ioctl(fd, FIO_BBD_DOWNLOAD, &bbd_data);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - FIO_BBD_DOWNLOAD failed!\n"));
	}

	/* Delete binary buffer */
	tapi_dev_binary_buffer_delete(pFirmware);

	return status;
}

static pj_status_t tapi_dev_start(tapi_aud_factory_t *f)
{
	pj_uint8_t c, hook_status;
	pj_status_t status = PJ_SUCCESS;
	IFX_TAPI_DEV_START_CFG_t tapistart;
	IFX_TAPI_MAP_DATA_t datamap;
	IFX_TAPI_ENC_CFG_t enc_cfg;
	IFX_TAPI_LINE_VOLUME_t line_vol;
	IFX_TAPI_CID_CFG_t cid_cnf;
	
	/* Open device */
	f->dev_ctx.dev_fd = tapi_dev_open(TAPI_LL_DEV_BASE_PATH, 0);
	
	if (f->dev_ctx.dev_fd < 0) {
		TRACE_((THIS_FILE, "ERROR - TAPI device open failed!"));
		return PJ_EUNKNOWN;
	}

	for (c = 0; c < TAPI_AUDIO_PORT_NUM; c++) {
		ch_fd[c] = f->dev_ctx.ch_fd[c] = tapi_dev_open(TAPI_LL_DEV_BASE_PATH, TAPI_AUDIO_PORT_NUM - c);

		if (f->dev_ctx.dev_fd < 0) {
			TRACE_((THIS_FILE, "ERROR - TAPI channel%d open failed!", c));
			return PJ_EUNKNOWN;
		}
		f->dev_ctx.data2phone_map[c] = c & 0x1 ? 0 : 1;
	}

	status = tapi_dev_firmware_download(f->dev_ctx.dev_fd, TAPI_LL_DEV_FIRMWARE_NAME);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - Voice Firmware Download failed!"));
		return PJ_EUNKNOWN;
	}

	/* Download coefficients */
	/*
	status = tapi_dev_bbd_download(f->dev_ctx.dev_fd, TAPI_LL_BBD_NAME);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - Voice Coefficients Download failed!"));
		return PJ_EUNKNOWN;
	}
	*/

	memset(&tapistart, 0x0, sizeof(IFX_TAPI_DEV_START_CFG_t));
	tapistart.nMode = IFX_TAPI_INIT_MODE_VOICE_CODER;
	
	/* Start TAPI */
	status = ioctl(f->dev_ctx.dev_fd, IFX_TAPI_DEV_START, &tapistart);
	if (status != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_DEV_START ioctl failed"));
		return PJ_EUNKNOWN;
	}


	for (c = 0; c < TAPI_AUDIO_PORT_NUM; c++) {
		/* Perform mapping */
		memset(&datamap, 0x0, sizeof(IFX_TAPI_MAP_DATA_t));
		datamap.nDstCh  = f->dev_ctx.data2phone_map[c];
		datamap.nChType = IFX_TAPI_MAP_TYPE_PHONE;
		
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_MAP_DATA_ADD, &datamap);
		
		if (status != PJ_SUCCESS) {
			TRACE_((THIS_FILE, "ERROR - IFX_TAPI_MAP_DATA_ADD ioctl failed"));
			return PJ_EUNKNOWN;
		}
		
		/* Set Line feed */
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_LINE_FEED_SET, IFX_TAPI_LINE_FEED_STANDBY);
		
		if (status != PJ_SUCCESS) {
			TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_FEED_SET ioctl failed"));
			return PJ_EUNKNOWN;
		}
		
		/* Configure encoder for linear stream */
		memset(&enc_cfg, 0x0, sizeof(IFX_TAPI_ENC_CFG_t));
		
		enc_cfg.nFrameLen = IFX_TAPI_COD_LENGTH_20;
		enc_cfg.nEncType  = IFX_TAPI_COD_TYPE_LIN16_8;
		
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_ENC_CFG_SET, &enc_cfg);
		if (status != PJ_SUCCESS) {
			TRACE_((THIS_FILE, "ERROR - IFX_TAPI_ENC_CFG_SET ioctl failed"));
			return PJ_EUNKNOWN;
		}
		
		/* Suppress TAPI volume, otherwise PJSIP starts autogeneration!!! */
		line_vol.nGainRx = -8;
		line_vol.nGainTx = -8;
		
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_PHONE_VOLUME_SET, &line_vol);
		if (status != PJ_SUCCESS) {
			TRACE_((THIS_FILE, "ERROR - IFX_TAPI_PHONE_VOLUME_SET ioctl failed"));
			return PJ_EUNKNOWN;
		}
		
		/* Configure Caller ID type */
		/* One can choose from following (for now at compile time):
			IFX_TAPI_CID_STD_TELCORDIA
			IFX_TAPI_CID_STD_ETSI_FSK
			IFX_TAPI_CID_STD_ETSI_DTMF
			IFX_TAPI_CID_STD_SIN
			IFX_TAPI_CID_STD_NTT
			IFX_TAPI_CID_STD_KPN_DTMF
			IFX_TAPI_CID_STD_KPN_DTMF_FSK
		*/
		memset(&cid_cnf, 0, sizeof(cid_cnf));
		cid_cnf.nStandard = IFX_TAPI_CID_STD_ETSI_FSK;
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_CID_CFG_SET, &cid_cnf);
		if (status != PJ_SUCCESS) {
			TRACE_((THIS_FILE, "ERROR - IFX_TAPI_CID_CFG_SET ioctl failed"));
			return PJ_EUNKNOWN;
		}
		
		/* check hook status */
		hook_status = 0;
		status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_LINE_HOOK_STATUS_GET, &hook_status);
		if (status != PJ_SUCCESS) {
		   TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_HOOK_STATUS_GET ioctl failed!"));
		   return PJ_EUNKNOWN;
		}
		
		/* if off hook do initialization */
		if (hook_status) {
			status = ioctl(f->dev_ctx.ch_fd[c], IFX_TAPI_LINE_FEED_SET, IFX_TAPI_LINE_FEED_ACTIVE);
			if (status != PJ_SUCCESS) {
				TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_FEED_SET ioctl failed!"));
				return PJ_EUNKNOWN;
			}
			status = ioctl(c, IFX_TAPI_ENC_START, 0);
			if (status != PJ_SUCCESS) {
				TRACE_((THIS_FILE, "ERROR - IFX_TAPI_ENC_START ioctl failed!"));
				return PJ_EUNKNOWN;
			}

			status = ioctl(c, IFX_TAPI_DEC_START, 0);
			if (status != PJ_SUCCESS) {
				TRACE_((THIS_FILE, "ERROR - IFX_TAPI_DEC_START ioctl failed!"));
				return PJ_EUNKNOWN;
			}
		}
	}

	return status;
}

static pj_status_t
tapi_dev_stop(tapi_aud_factory_t *f)
{
	pj_status_t status = PJ_SUCCESS;
	pj_uint8_t c;

	if (ioctl(f->dev_ctx.dev_fd, IFX_TAPI_DEV_STOP, 0) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_DEV_STOP ioctl failed"));
		status = PJ_EUNKNOWN;
	}

	close(f->dev_ctx.dev_fd);
	for (c = TAPI_AUDIO_PORT_NUM; c > 0; c--)
		close(f->dev_ctx.ch_fd[TAPI_AUDIO_PORT_NUM-c]);

	return status;
}

static pj_status_t
tapi_dev_codec_control(pj_int32_t fd, pj_uint8_t start)
{
	if (ioctl(fd, start ? IFX_TAPI_ENC_START : IFX_TAPI_ENC_STOP, 0) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_ENC_%s ioctl failed!",
			start ? "START" : "STOP"));
		return PJ_EUNKNOWN;
	}

	if (ioctl(fd, start ? IFX_TAPI_DEC_START : IFX_TAPI_DEC_STOP, 0) != IFX_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_DEC_%s ioctl failed!",
			start ? "START" : "STOP"));
		return PJ_EUNKNOWN;
	}

	return PJ_SUCCESS;
}

static pj_status_t tapi_dev_event_on_hook(tapi_ctx *dev_ctx, pj_uint32_t dev_idx)
{
   PJ_LOG(1,(THIS_FILE, "TAPI: ONHOOK"));

   if (ioctl(dev_ctx->ch_fd[dev_idx], IFX_TAPI_LINE_FEED_SET,
               IFX_TAPI_LINE_FEED_STANDBY) != PJ_SUCCESS) {
      TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_FEED_SET ioctl failed!"));

      return PJ_EUNKNOWN;
   }

   /* enc/dec stop */
   if (tapi_dev_codec_control(dev_ctx->ch_fd[dev_idx], 0) != PJ_SUCCESS) {
      TRACE_((THIS_FILE, "ERROR - codec start failed!"));

      return PJ_EUNKNOWN;
   }

   return PJ_SUCCESS;
}

static pj_status_t tapi_dev_event_off_hook(tapi_ctx *dev_ctx, pj_uint32_t dev_idx)
{
   PJ_LOG(1,(THIS_FILE, "TAPI: OFFHOOK"));

   if (ioctl(dev_ctx->ch_fd[dev_idx], IFX_TAPI_LINE_FEED_SET,
               IFX_TAPI_LINE_FEED_ACTIVE) != PJ_SUCCESS) {
      TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_FEED_SET ioctl failed!"));

      return PJ_EUNKNOWN;
   }

   /* enc/dec stop */
   if (tapi_dev_codec_control(dev_ctx->ch_fd[dev_idx], 1) != PJ_SUCCESS) {
      TRACE_((THIS_FILE, "ERROR - codec start failed!"));

      return PJ_EUNKNOWN;
   }

   return PJ_SUCCESS;
}

static pj_status_t
tapi_dev_event_digit(tapi_ctx *dev_ctx, pj_uint32_t dev_idx)
{
	PJ_LOG(1,(THIS_FILE, "TAPI: OFFHOOK"));

	if (ioctl(dev_ctx->ch_fd[dev_idx], IFX_TAPI_LINE_FEED_SET,
			IFX_TAPI_LINE_FEED_ACTIVE) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_FEED_SET ioctl failed!"));
		return PJ_EUNKNOWN;
	}

	/* enc/dec stop */
	if (tapi_dev_codec_control(dev_ctx->ch_fd[dev_idx], 1) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - codec start failed!"));
		return PJ_EUNKNOWN;
	}

	return PJ_SUCCESS;
}

static pj_status_t
tapi_dev_event_handler(tapi_aud_stream_t *stream)
{
	IFX_TAPI_EVENT_t tapiEvent;
	tapi_ctx *dev_ctx = stream->dev_ctx;
	pj_status_t status = PJ_SUCCESS;
	unsigned int i;

	for (i = 0; i < TAPI_AUDIO_PORT_NUM; i++) {
		memset (&tapiEvent, 0, sizeof(tapiEvent));
		tapiEvent.ch = dev_ctx->data2phone_map[i];
		status = ioctl(dev_ctx->dev_fd, IFX_TAPI_EVENT_GET, &tapiEvent);

		if ((status == PJ_SUCCESS) && (tapiEvent.id != IFX_TAPI_EVENT_NONE)) {
			switch(tapiEvent.id) {
			case IFX_TAPI_EVENT_FXS_ONHOOK:
				status = tapi_dev_event_on_hook(dev_ctx, i);
				if(tapi_hook_callback)
					tapi_hook_callback(i, 0);
				break;
			case IFX_TAPI_EVENT_FXS_OFFHOOK:
				status = tapi_dev_event_off_hook(dev_ctx, i);
				if(tapi_hook_callback)
					tapi_hook_callback(i, 1);
				break;
			case IFX_TAPI_EVENT_DTMF_DIGIT:
				if(tapi_digit_callback)
					tapi_digit_callback(i, tapiEvent.data.dtmf.ascii);
				break;
			case IFX_TAPI_EVENT_COD_DEC_CHG:
			case IFX_TAPI_EVENT_TONE_GEN_END:
			case IFX_TAPI_EVENT_CID_TX_SEQ_END:
				break;
			default:
				PJ_LOG(1,(THIS_FILE, "unknown tapi event %08X", tapiEvent.id));
				break;
			}
		}
	}

	return status;
}

static pj_status_t
tapi_dev_data_handler(tapi_aud_stream_t *stream) {
   pj_status_t status = PJ_SUCCESS;
   tapi_ctx *dev_ctx = stream->dev_ctx;
   pj_uint32_t dev_idx = stream->param.rec_id;
   pj_uint8_t buf_rec[TAPI_LL_DEV_ENC_BYTES_PER_FRAME + TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE]={0};
   pj_uint8_t buf_play[TAPI_LL_DEV_ENC_BYTES_PER_FRAME + TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE]={0};
   pjmedia_frame frame_rec, frame_play;
   pj_int32_t ret;

   /* Get data from driver */
   ret = read(dev_ctx->ch_fd[dev_idx], buf_rec, sizeof(buf_rec));
   if (ret < 0) {
      TRACE_((THIS_FILE, "ERROR - no data available from device!"));

      return PJ_EUNKNOWN;
   }

   if (ret > 0) {
      frame_rec.type = PJMEDIA_FRAME_TYPE_AUDIO;
      frame_rec.buf  = buf_rec + TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE;
      frame_rec.size = ret - TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE;
      frame_rec.timestamp.u64 = stream->timestamp.u64;

      status = stream->rec_cb(stream->user_data, &frame_rec);
      if (status != PJ_SUCCESS)
      {
        PJ_LOG(1, (THIS_FILE, "rec_cb() failed %d", status));
      }

      frame_play.type = PJMEDIA_FRAME_TYPE_AUDIO;
      frame_play.buf  = buf_play + TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE;
      frame_play.size = TAPI_LL_DEV_ENC_BYTES_PER_FRAME;
      frame_play.timestamp.u64 = stream->timestamp.u64;

      status = (*stream->play_cb)(stream->user_data, &frame_play);
      if (status != PJ_SUCCESS)
      {
         PJ_LOG(1, (THIS_FILE, "play_cb() failed %d", status));
      }
      else
      {
         memcpy(buf_play, buf_rec, TAPI_LL_DEV_RTP_HEADER_SIZE_BYTE);

         ret = write(dev_ctx->ch_fd[dev_idx], buf_play, sizeof(buf_play));

         if (ret < 0) {
            PJ_LOG(1, (THIS_FILE, "ERROR - device data writing failed!"));
            return PJ_EUNKNOWN;
         }

         if (ret == 0) {
            PJ_LOG(1, (THIS_FILE, "ERROR - no data written to device!"));
            return PJ_EUNKNOWN;
         }
      }

      stream->timestamp.u64 += TAPI_LL_DEV_ENC_SMPL_PER_FRAME;
   }

   return PJ_SUCCESS;
}

static int
PJ_THREAD_FUNC tapi_dev_thread(void *arg) {
	tapi_aud_stream_t *strm = (struct tapi_aud_stream*)arg;
	tapi_ctx *dev_ctx = strm->dev_ctx;
	pj_uint32_t sretval;
	pj_uint32_t dev_idx;
	struct pollfd fds[3];

	PJ_LOG(1,(THIS_FILE, "TAPI: thread starting..."));

	if (strm->param.rec_id != strm->param.play_id) {
		PJ_LOG(1,(THIS_FILE, "TAPI: thread exit - incorrect play/rec IDs"));
		return 0;
	}

	dev_idx = strm->param.rec_id;
	strm->run_flag = 1;

	fds[0].fd = dev_ctx->dev_fd;
	fds[0].events = POLLIN;
	fds[1].fd = dev_ctx->ch_fd[0];
	fds[1].events = POLLIN;
	fds[2].fd = dev_ctx->ch_fd[1];
	fds[2].events = POLLIN;

	while(1)
	{
		sretval = poll(fds, TAPI_AUDIO_PORT_NUM + 1, TAPI_LL_DEV_SELECT_TIMEOUT_MS);

		if (!strm->run_flag)
			break;
		if (sretval <= 0)
			continue;

		if (fds[0].revents == POLLIN) {
			if (tapi_dev_event_handler(strm) != PJ_SUCCESS) {
				PJ_LOG(1,(THIS_FILE, "TAPI: event hanldler failed!"));
				break;
			}
		}

		if (fds[1].revents == POLLIN) {
			if (tapi_dev_data_handler(strm) != PJ_SUCCESS) {
				PJ_LOG(1,(THIS_FILE, "TAPI: data hanldler failed!"));
				break;
			}
		}

		if (fds[2].revents == POLLIN) {
			if (tapi_dev_data_handler(strm) != PJ_SUCCESS) {
				PJ_LOG(1,(THIS_FILE, "TAPI: data hanldler failed!"));
				break;
			}
		}
	}
	PJ_LOG(1,(THIS_FILE, "TAPI: thread stopping..."));

	return 0;
}

/****************************************************************************
 Factory operations
 ****************************************************************************/

pjmedia_aud_dev_factory*
pjmedia_tapi_factory(pj_pool_factory *pf) {
	struct tapi_aud_factory *f;
	pj_pool_t *pool;

	TRACE_((THIS_FILE, "pjmedia_tapi_factory()"));

	pool = pj_pool_create(pf, "tapi", 512, 512, NULL);
	f = PJ_POOL_ZALLOC_T(pool, struct tapi_aud_factory);
	f->pf = pf;
	f->pool = pool;
	f->base.op = &tapi_fact_op;

	return &f->base;
}

static pj_status_t
factory_init(pjmedia_aud_dev_factory *f)
{
	struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;
	pj_uint8_t c;

	TRACE_((THIS_FILE, "factory_init()"));

	af->dev_count = 1;
	af->dev_info = (pjmedia_aud_dev_info*)
	pj_pool_calloc(af->pool, af->dev_count, sizeof(pjmedia_aud_dev_info));
	pj_ansi_sprintf(af->dev_info[0].name,"%s_%02d", TAPI_BASE_NAME, c);
	af->dev_info[0].input_count = af->dev_info[0].output_count = TAPI_AUDIO_PORT_NUM;
	af->dev_info[0].default_samples_per_sec = TAPI_LL_DEV_ENC_SMPL_PER_SEC;
	pj_ansi_strcpy(af->dev_info[0].driver, "/dev/vmmc");
	af->dev_info[0].caps = PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING |
		PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY |
		PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY;
	af->dev_info[0].routes = PJMEDIA_AUD_DEV_ROUTE_DEFAULT ;
	if (tapi_dev_start(af) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - TAPI device init failed!"));
		return PJ_EUNKNOWN;
	}

	return PJ_SUCCESS;
}

static pj_status_t
factory_destroy(pjmedia_aud_dev_factory *f)
{
	struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;
	pj_pool_t *pool;
	pj_status_t status = PJ_SUCCESS;

	TRACE_((THIS_FILE, "factory_destroy()"));

	if (tapi_dev_stop(f) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - TAPI device stop failed!"));
		status = PJ_EUNKNOWN;
	}
	pool = af->pool;
	af->pool = NULL;
	pj_pool_release(pool);

	return status;
}

static unsigned
factory_get_dev_count(pjmedia_aud_dev_factory *f)
{
	struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;
	TRACE_((THIS_FILE, "factory_get_dev_count()"));

	return af->dev_count;
}

static pj_status_t
factory_get_dev_info(pjmedia_aud_dev_factory *f, unsigned index, pjmedia_aud_dev_info *info)
{
	struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;

	TRACE_((THIS_FILE, "factory_get_dev_info()"));
	PJ_ASSERT_RETURN(index < af->dev_count, PJMEDIA_EAUD_INVDEV);

	pj_memcpy(info, &af->dev_info[index], sizeof(*info));

	return PJ_SUCCESS;
}

static pj_status_t
factory_default_param(pjmedia_aud_dev_factory *f, unsigned index, pjmedia_aud_param *param)
{
	struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;
	struct pjmedia_aud_dev_info *di = &af->dev_info[index];

	TRACE_((THIS_FILE, "factory_default_param."));
	PJ_ASSERT_RETURN(index < af->dev_count, PJMEDIA_EAUD_INVDEV);

	pj_bzero(param, sizeof(*param));
	if (di->input_count && di->output_count) {
		param->dir = PJMEDIA_DIR_CAPTURE_PLAYBACK;
		param->rec_id = index;
		param->play_id = index;
	} else if (di->input_count) {
		param->dir = PJMEDIA_DIR_CAPTURE;
		param->rec_id = index;
		param->play_id = PJMEDIA_AUD_INVALID_DEV;
	} else if (di->output_count) {
		param->dir = PJMEDIA_DIR_PLAYBACK;
		param->play_id = index;
		param->rec_id = PJMEDIA_AUD_INVALID_DEV;
	} else {
		return PJMEDIA_EAUD_INVDEV;
	}

	param->clock_rate = TAPI_LL_DEV_ENC_SMPL_PER_SEC; //di->default_samples_per_sec;
	param->channel_count = 1;
	param->samples_per_frame = TAPI_LL_DEV_ENC_SMPL_PER_FRAME;
	param->bits_per_sample = TAPI_LL_DEV_ENC_BITS_PER_SMPLS;
	param->flags = PJMEDIA_AUD_DEV_CAP_OUTPUT_ROUTE | di->caps;
	param->output_route = PJMEDIA_AUD_DEV_ROUTE_DEFAULT;

	return PJ_SUCCESS;
}

static pj_status_t
factory_create_stream(pjmedia_aud_dev_factory *f, const pjmedia_aud_param *param,
	pjmedia_aud_rec_cb rec_cb, pjmedia_aud_play_cb play_cb,
	void *user_data, pjmedia_aud_stream **p_aud_strm)
{
   struct tapi_aud_factory *af = (struct tapi_aud_factory*)f;
   pj_pool_t *pool;
   struct tapi_aud_stream *strm;
   pj_status_t status;

   TRACE_((THIS_FILE, "factory_create_stream()"));

   /* Can only support 16bits per sample */
   PJ_ASSERT_RETURN(param->bits_per_sample == TAPI_LL_DEV_ENC_BITS_PER_SMPLS, PJ_EINVAL);
   printf("param->clock_rate = %d, samples_per_frame = %d\n", param->clock_rate, param->samples_per_frame);
   PJ_ASSERT_RETURN(param->clock_rate == TAPI_LL_DEV_ENC_SMPL_PER_SEC, PJ_EINVAL);

   PJ_ASSERT_RETURN(param->samples_per_frame == TAPI_LL_DEV_ENC_SMPL_PER_FRAME, PJ_EINVAL);

   /* Can only support bidirectional stream */
   PJ_ASSERT_RETURN(param->dir & PJMEDIA_DIR_CAPTURE_PLAYBACK, PJ_EINVAL);

   /* Initialize our stream data */
   pool = pj_pool_create(af->pf, "tapi-dev", 1000, 1000, NULL);
   PJ_ASSERT_RETURN(pool != NULL, PJ_ENOMEM);

   strm = PJ_POOL_ZALLOC_T(pool, struct tapi_aud_stream);
   strm->pool      = pool;
   strm->rec_cb    = rec_cb;
   strm->play_cb   = play_cb;
   strm->user_data = user_data;
   pj_memcpy(&strm->param, param, sizeof(*param));

   if ((strm->param.flags & PJMEDIA_AUD_DEV_CAP_EXT_FORMAT) == 0) {
      strm->param.ext_fmt.id = PJMEDIA_FORMAT_L16;
   }

   strm->timestamp.u64 = 0;
   strm->dev_ctx = &(af->dev_ctx);

   /* Create and start the thread */
   status = pj_thread_create(pool, "tapi", &tapi_dev_thread, strm, 0, 0, 
			      &strm->thread);
   if (status != PJ_SUCCESS) {
      stream_destroy(&strm->base);
      return status;
   }

   /* Done */
   strm->base.op = &tapi_strm_op;
   *p_aud_strm = &strm->base;

   return PJ_SUCCESS;
}

static pj_status_t
stream_get_param(pjmedia_aud_stream *s, pjmedia_aud_param *pi)
{
	struct tapi_aud_stream *strm = (struct tapi_aud_stream*)s;

	PJ_ASSERT_RETURN(strm && pi, PJ_EINVAL);
	pj_memcpy(pi, &strm->param, sizeof(*pi));

	if (stream_get_cap(s, PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING,
			&pi->output_vol) == PJ_SUCCESS)
		pi->flags |= PJMEDIA_AUD_DEV_CAP_OUTPUT_VOLUME_SETTING;

	if (stream_get_cap(s, PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY,
			&pi->output_latency_ms) == PJ_SUCCESS)
		pi->flags |= PJMEDIA_AUD_DEV_CAP_OUTPUT_LATENCY;

	if (stream_get_cap(s, PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY,
			&pi->input_latency_ms) == PJ_SUCCESS)
		pi->flags |= PJMEDIA_AUD_DEV_CAP_INPUT_LATENCY;

	return PJ_SUCCESS;
}

static pj_status_t
stream_get_cap(pjmedia_aud_stream *s, pjmedia_aud_dev_cap cap, void *pval)
{
	// struct tapi_aud_stream *strm = (struct tapi_aud_stream*)s;
	return PJ_SUCCESS;
}

static pj_status_t
stream_set_cap(pjmedia_aud_stream *s, pjmedia_aud_dev_cap cap, const void *pval)
{
	// struct tapi_aud_stream *strm = (struct tapi_aud_stream*)s;
	return PJ_SUCCESS;
}

static pj_status_t
stream_start(pjmedia_aud_stream *s)
{
	struct tapi_aud_stream *strm = (struct tapi_aud_stream*)s;
	tapi_ctx *dev_ctx = strm->dev_ctx;
	pj_uint32_t dev_idx;

	TRACE_((THIS_FILE, "stream_start()"));

	dev_idx = strm->param.rec_id;

	return PJ_SUCCESS;
}

static pj_status_t
stream_stop(pjmedia_aud_stream *s)
{
	struct tapi_aud_stream *strm = (struct tapi_aud_stream*)s;
	tapi_ctx *dev_ctx = strm->dev_ctx;
	pj_uint32_t dev_idx;

	TRACE_((THIS_FILE, "stream_stop()"));
	dev_idx = strm->param.rec_id;

	if (tapi_dev_codec_control(dev_ctx->ch_fd[dev_idx], 0) != PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - codec start failed!"));
		return PJ_EUNKNOWN;
	}

	return PJ_SUCCESS;
}

static pj_status_t
stream_destroy(pjmedia_aud_stream *s)
{
	pj_status_t state = PJ_SUCCESS;
	struct tapi_aud_stream *stream = (struct tapi_aud_stream*)s;
	pj_pool_t *pool;

	PJ_ASSERT_RETURN(stream != NULL, PJ_EINVAL);
	TRACE_((THIS_FILE, "stream_destroy()"));

	stream_stop(stream);
	stream->run_flag = 0;

	if (stream->thread)
	{
		pj_thread_join(stream->thread);
		pj_thread_destroy(stream->thread);
		stream->thread = NULL;
	}

	pool = stream->pool;
	pj_bzero(stream, sizeof(stream));
	pj_pool_release(pool);

	return state;
}

pj_status_t
tapi_hook_status(pj_uint32_t port, pj_uint32_t *status)
{
	if (ioctl(ch_fd[port], IFX_TAPI_LINE_HOOK_STATUS_GET, status)
			!= PJ_SUCCESS) {
		TRACE_((THIS_FILE, "ERROR - IFX_TAPI_LINE_HOOK_STATUS_GET ioctl failed!"));
		return PJ_EUNKNOWN;
	}
		
	return PJ_SUCCESS;
}

pj_status_t
tapi_ring(pj_uint32_t port, pj_uint32_t state, char *caller_number) {
	PJ_ASSERT_RETURN(port < TAPI_AUDIO_PORT_NUM, PJ_EINVAL);

	if (state) {
		if (caller_number) {
			IFX_TAPI_CID_MSG_t cid_msg;
			IFX_TAPI_CID_MSG_ELEMENT_t cid_msg_el[1];
			memset(&cid_msg, 0, sizeof(cid_msg));
			memset(&cid_msg_el, 0, sizeof(cid_msg_el));

			cid_msg_el[0].string.elementType = IFX_TAPI_CID_ST_CLI;
			cid_msg_el[0].string.len = strlen(caller_number);
			strncpy(cid_msg_el[0].string.element, caller_number, sizeof(cid_msg_el[0].string.element));

			cid_msg.txMode = IFX_TAPI_CID_HM_ONHOOK;
			cid_msg.messageType = IFX_TAPI_CID_MT_CSUP;
			cid_msg.nMsgElements = 1;
			cid_msg.message = cid_msg_el;
			ioctl(ch_fd[port], IFX_TAPI_CID_TX_SEQ_START, &cid_msg);
		} else {
			ioctl(ch_fd[port], IFX_TAPI_RING_START, 0);
		}
	} else {
		ioctl(ch_fd[port], IFX_TAPI_RING_STOP, 0);
	}

	return PJ_SUCCESS;
}

pj_status_t
tapi_dial_tone(pj_uint32_t port) {
	PJ_ASSERT_RETURN(port < TAPI_AUDIO_PORT_NUM, PJ_EINVAL);

	ioctl(ch_fd[port], IFX_TAPI_TONE_DIALTONE_PLAY, 0);

	return PJ_SUCCESS;
}

pj_status_t
tapi_no_tone(pj_uint32_t port) {
	PJ_ASSERT_RETURN(port < TAPI_AUDIO_PORT_NUM, PJ_EINVAL);

	ioctl(ch_fd[port], IFX_TAPI_TONE_LOCAL_PLAY, 0);

	return PJ_SUCCESS;
}

#endif
