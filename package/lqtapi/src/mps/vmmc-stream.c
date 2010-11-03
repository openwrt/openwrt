#include <linux/kernel.h>
#include <linux/tapi/tapi.h>
#include <linux/skbuff.h>

#include "vmmc.h"
#include "vmmc-coder.h"

struct vmmc_tapi_stream {
	struct vmmc_coder *coder;
	struct tapi_stream stream;
};

struct vmmc_tapi_stream *tapi_to_vmmc_stream(struct tapi_stream * stream)
{
	return container_of(stream, struct vmmc_tapi_stream, stream);
}

struct tapi_stream *vmmc_stream_alloc(struct tapi_device *tdev)
{
	struct vmmc *vmmc = tdev_to_vmmc(tdev);
	struct vmmc_tapi_stream *stream;
	struct vmmc_coder *coder;

	coder = vmmc_coder_get(vmmc);
	if (!coder)
		return ERR_PTR(-ENODEV);

	stream = kzalloc(sizeof(*stream), GFP_KERNEL);
	if (!stream)
		return ERR_PTR(-ENOMEM);

	stream->coder = coder;
	coder->stream = &stream->stream;
	tapi_endpoint_set_data(&stream->stream.ep, &coder->module);

	return &stream->stream;
}

void vmmc_stream_free(struct tapi_device *tdev, struct tapi_stream *tstream)
{
	struct vmmc *vmmc = tdev_to_vmmc(tdev);
	struct vmmc_tapi_stream *stream = tapi_to_vmmc_stream(tstream);

	stream->coder->stream = NULL;

	vmmc_coder_put(vmmc, stream->coder);
	kfree(stream);
}

int vmmc_stream_start(struct tapi_device *tdev, struct tapi_stream *stream)
{
	return 0;
}

int vmmc_stream_stop(struct tapi_device *tdev, struct tapi_stream *stream)
{
	return 0;
}

int vmmc_stream_send(struct tapi_device *tdev, struct tapi_stream *stream,
	struct sk_buff *skb)
{
	struct vmmc *vmmc = tdev_to_vmmc(tdev);
	struct vmmc_coder *coder = tapi_to_vmmc_stream(stream)->coder;

	vmmc_send_paket(vmmc, coder->id, skb);
	return 0;
}
/*
int vmmc_stream_recv(struct vmmc_stream *stream)
{
	tapi_stream_recv(&stream->coder->vmmc->tdev stream->stream, skb);
}*/
