#ifndef __TAPI_STREAM_H__
#define __TAPI_STREAM_H__

struct tapi_device;

struct tapi_stream {
	int fd;
	int ep;
};

struct tapi_stream *tapi_stream_alloc(struct tapi_device *);
void tapi_stream_free(struct tapi_stream *);

static inline int tapi_stream_get_endpoint(struct tapi_stream *stream)
{
	return stream->ep;
}

#endif
