#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <curl/curl.h>

#include <libubox/ulog.h>

static const char *file_cert = "/etc/ucentral/cert.pem";
static const char *file_key  = "/etc/ucentral/key.pem";
static const char *file_json = "/etc/ucentral/redirector.json";
static const char *file_dbg  = "/tmp/firstcontact.hdr";

int main(int argc, char **argv)
{
	FILE *fp_json;
	FILE *fp_dbg;
	CURLcode res;
	CURL *curl;
	char *devid = NULL;
	char *url;

	alarm(15);

	while (1) {
		int option = getopt(argc, argv, "k:c:o:hi:");

		if (option == -1)
			break;

		switch (option) {
		case 'k':
			file_key = optarg;
			break;
		case 'c':
			file_cert = optarg;
			break;
		case 'o':
			file_json = optarg;
			break;
		case 'i':
			devid = optarg;
			break;
		default:
		case 'h':
			printf("Usage: firstcontact OPTIONS\n"
			       "  -k <keyfile>\n"
			       "  -c <certfile>\n"
			       "  -o <outfile>\n"
			       "  -i <devid>\n");
			return -1;
		}
	}

	if (!devid) {
		fprintf(stderr, "missing devid\n");
		return -1;
	}

	ulog_open(ULOG_SYSLOG | ULOG_STDIO, LOG_DAEMON, "firstcontact");
	ULOG_INFO("attempting first contact\n");

	fp_dbg = fopen(file_dbg, "wb");
	fp_json = fopen(file_json, "wb");
	if (!fp_json) {
		ULOG_ERR("failed to create %s\n", file_json);
		return -1;
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (!curl) {
		ULOG_ERR("curl_easy_init failed\n");
		return -1;
	}

	if (asprintf(&url, "https://clientauth.one.digicert.com/iot/api/v2/device/%s", devid) < 0) {
		ULOG_ERR("failed to assemble url\n");
		return -1;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp_json);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, fp_dbg);
	curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
	curl_easy_setopt(curl, CURLOPT_SSLCERT, file_cert);
	curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
	curl_easy_setopt(curl, CURLOPT_SSLKEY, file_key);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		ULOG_ERR("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	else
		ULOG_INFO("downloaded first contact data\n");
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	ulog_close();

	return (res != CURLE_OK);
}
