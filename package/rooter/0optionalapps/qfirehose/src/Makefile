all: qfirehose

qfirehose:
	$(CC) $(CFLAGS) -Wall firehose_protocol.c  qfirehose.c  sahara_protocol.c usb_linux.c stream_download_protocol.c md5.c usb2tcp.c -o $@ $^

clean:
	rm -rf firehose obj libs
