#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tapi-device.h"
#include "tapi-stream.h"
#include "tapi-ioctl.h"

struct tapi_stream *tapi_stream_alloc(struct tapi_device *dev)
{
	struct tapi_stream *stream;

	stream = malloc(sizeof(*stream));
	if (!stream)
		return NULL;

	stream->fd = open(dev->stream_path, O_RDWR);

	if (stream->fd < 0) {
		free(stream);
		return NULL;
	}

	stream->ep = ioctl(stream->fd, TAPI_STREAM_IOCTL_GET_ENDPOINT, 0);

	if (stream->ep < 0) {
		close(stream->fd);
		free(stream);
		return NULL;
	}

	return stream;
}

void tapi_stream_free(struct tapi_stream *stream)
{
	close(stream->fd);
	free(stream);
}
