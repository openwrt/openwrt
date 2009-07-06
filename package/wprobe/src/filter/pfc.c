#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <pcap.h>
#include <pcap-bpf.h>

struct wprobe_filter_hdr {
	char name[32];
	uint32_t len;
} hdr;

int main (int argc, char ** argv)
{
    struct  bpf_program filter;
    pcap_t  *pc;
	int i;

    if (argc != 3)
    {
		fprintf(stderr, "Usage: %s <name> <expression>\n", argv[0]);
		return 1;
    }

    pc = pcap_open_dead(DLT_IEEE802_11_RADIO, 256);
    if (pcap_compile(pc, &filter, argv[2], 1, 0) != 0)
	{
		pcap_perror(pc, argv[0]);
		exit(1);
	}

	/* fix up for linux */
	for (i = 0; i < filter.bf_len; i++) {
		struct bpf_insn *bi = &filter.bf_insns[i];
		switch(BPF_CLASS(bi->code)) {
		case BPF_RET:
			if (BPF_MODE(bi->code) == BPF_K) {
				if (bi->k != 0)
					bi->k = 65535;
			}
			break;
		}
		bi->code = ntohs(bi->code);
		bi->k = ntohl(bi->k);
	}

	memset(&hdr, 0, sizeof(hdr));
	strncpy(hdr.name, argv[1], sizeof(hdr.name));
	hdr.len = htonl(filter.bf_len);
	fwrite(&hdr, sizeof(hdr), 1, stdout);
	fwrite(filter.bf_insns, 8, filter.bf_len, stdout);
	fflush(stdout);

    return 0;
}
