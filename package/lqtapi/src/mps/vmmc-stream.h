#ifndef __VMMC_STREAM_H__
#define __VMMC_STREAM_H__

struct tapi_stream *vmmc_stream_alloc(struct tapi_device *tdev);
void vmmc_stream_free(struct tapi_device *tdev, struct tapi_stream *stream);
int vmmc_stream_start(struct tapi_device *tdev, struct tapi_stream *stream);
int vmmc_stream_send(struct tapi_device *tdev, struct tapi_stream *stream,
	struct sk_buff *skb);

#endif
