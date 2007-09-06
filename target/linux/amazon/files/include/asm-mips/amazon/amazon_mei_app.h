//509221:tc.chen 2005/09/22 Reset DFE added when MEI_TO_ARC_CS_DONE not cleared by ARC and Added AMAZON_MEI_DEBUG_MODE ioctl 

#ifndef       	_AMAZON_MEI_APP_H
#define        	_AMAZON_MEI_APP_H

/////////////////////////////////////////////////////////////////////////////////////////////////////

                //  ioctl control
#define AMAZON_MEI_START 	                       	300
#define AMAZON_MEI_REPLY                           	301
#define AMAZON_MEI_NOREPLY                      	302

#define AMAZON_MEI_RESET				303
#define AMAZON_MEI_REBOOT				304
#define AMAZON_MEI_HALT					305
#define AMAZON_MEI_CMV_WINHOST				306
#define AMAZON_MEI_CMV_READ				307
#define AMAZON_MEI_CMV_WRITE				308
#define AMAZON_MEI_MIB_DAEMON				309
#define AMAZON_MEI_SHOWTIME				310
#define AMAZON_MEI_REMOTE				311
#define AMAZON_MEI_READDEBUG				312
#define AMAZON_MEI_WRITEDEBUG				313
#define AMAZON_MEI_LOP					314

#define AMAZON_MEI_PCM_SETUP				315
#define AMAZON_MEI_PCM_START_TIMER			316
#define AMAZON_MEI_PCM_STOP_TIMER			317
#define AMAZON_MEI_PCM_CHECK				318
#define AMAZON_MEI_GET_EOC_LEN				319
#define AMAZON_MEI_GET_EOC_DATA				320
#define AMAZON_MEI_PCM_GETDATA				321
#define AMAZON_MEI_PCM_GPIO				322
#define AMAZON_MEI_EOC_SEND				323
//MIB
#define AMAZON_MIB_LO_ATUC				324
#define AMAZON_MIB_LO_ATUR				325
#define AMAZON_MEI_DOWNLOAD				326

#define AMAZON_MEI_DEBUG_MODE				327 //509221:tc.chen
#define LOOP_DIAGNOSTIC_MODE_COMPLETE			328


/***	Enums    ***/
typedef enum mei_error
{
	MEI_SUCCESS = 0,
	MEI_FAILURE = -1,
	MEI_MAILBOX_FULL = -2,
	MEI_MAILBOX_EMPTY = -3,
        MEI_MAILBOX_TIMEOUT = -4,
}MEI_ERROR;

#endif
