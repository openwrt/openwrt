/* SPDX-License-Identifier: GPL-2.0-only */
#define _POSIX_C_SOURCE 200809L

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define JRD_HEADER_LEN 44U
#define JRD_MAX_FRAME  (4U * 1024U * 1024U)

static int debug_enabled = 0;

static void put_le32(unsigned char *p, uint32_t v)
{
    p[0] = (unsigned char)(v & 0xffU);
    p[1] = (unsigned char)((v >> 8) & 0xffU);
    p[2] = (unsigned char)((v >> 16) & 0xffU);
    p[3] = (unsigned char)((v >> 24) & 0xffU);
}

static uint32_t get_le32(const unsigned char *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static size_t bounded_strlen(const unsigned char *p, size_t maxlen)
{
    size_t n;
    for (n = 0; n < maxlen; n++) {
        if (p[n] == '\0')
            break;
    }
    return n;
}

static int write_all(int fd, const void *buf, size_t len)
{
    const unsigned char *p = (const unsigned char *)buf;

    while (len > 0) {
        ssize_t n = send(fd, p, len, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (n == 0) {
            errno = EPIPE;
            return -1;
        }
        p += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

/*
 * Return values:
 *   1 = all requested bytes read
 *   0 = peer closed the connection
 *  -1 = error / timeout
 */
static int read_exact(int fd, void *buf, size_t len)
{
    unsigned char *p = (unsigned char *)buf;

    while (len > 0) {
        ssize_t n = recv(fd, p, len, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (n == 0)
            return 0;
        p += (size_t)n;
        len -= (size_t)n;
    }
    return 1;
}

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [options] COMMAND [PARAMS_JSON]\n"
        "       %s [options] rpc METHOD [PARAMS_JSON]\n"
        "\n"
        "Options:\n"
        "  -H IPv4      modem address (default 192.168.225.1)\n"
        "  -p PORT      TCP port (default 2016)\n"
        "  -t TX        transaction ID (default generated automatically)\n"
        "  -T SEC       socket timeout in seconds (default 5)\n"
        "  -d           print protocol diagnostics to stderr\n"
        "  -h           show this help\n"
        "\n"
        "Commands:\n"
        "  status             GetConnectionState\n"
        "  system             GetSystemStatus\n"
        "  connect            Connect\n"
        "  disconnect         DisConnect\n"
        "  network            GetNetworkInfo\n"
        "  sim                GetSimStatus\n"
        "  profiles           GetProfileList\n"
        "  current-profile    getCurrentProfile\n"
        "\n"
        "An unrecognized COMMAND is sent as the exact RPC method name.\n"
        "PARAMS_JSON defaults to {} and is inserted verbatim.\n",
        prog, prog);
}

static int parse_u32(const char *s, uint32_t *out)
{
    char *end = NULL;
    unsigned long v;

    errno = 0;
    v = strtoul(s, &end, 0);
    if (errno != 0 || end == s || *end != '\0' || v > 0xffffffffUL)
        return -1;

    *out = (uint32_t)v;
    return 0;
}

static int parse_u16(const char *s, uint16_t *out)
{
    uint32_t v;
    if (parse_u32(s, &v) < 0 || v == 0 || v > 65535U)
        return -1;
    *out = (uint16_t)v;
    return 0;
}

static int parse_timeout(const char *s, int *out)
{
    char *end = NULL;
    long v;

    errno = 0;
    v = strtol(s, &end, 10);
    if (errno != 0 || end == s || *end != '\0' || v < 1 || v > 300)
        return -1;

    *out = (int)v;
    return 0;
}

static int valid_method(const char *s)
{
    const unsigned char *p = (const unsigned char *)s;

    if (*p == '\0')
        return 0;

    while (*p) {
        if (!(isalnum(*p) || *p == '_'))
            return 0;
        p++;
    }
    return 1;
}

static uint32_t default_transaction(void)
{
    uint32_t v;

    v = ((uint32_t)time(NULL) ^ ((uint32_t)getpid() << 16)) & 0x7fffffffU;
    if (v == 0)
        v = 1;
    return v;
}

static int connect_modem(const char *host, uint16_t port, int timeout_sec)
{
    int fd;
    struct sockaddr_in sa;
    struct timeval tv;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    tv.tv_sec = timeout_sec;
    tv.tv_usec = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0 ||
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt");
        close(fd);
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &sa.sin_addr) != 1) {
        fprintf(stderr, "invalid IPv4 address: %s\n", host);
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

static unsigned char *build_request(const char *method,
                                    const char *params_json,
                                    uint32_t transaction,
                                    size_t *out_len)
{
    char *json;
    unsigned char *frame;
    size_t json_cap;
    size_t json_len;
    size_t total_len;
    int n;

    json_cap = strlen(method) + strlen(params_json) + 96U;
    json = (char *)malloc(json_cap);
    if (!json) {
        perror("malloc");
        return NULL;
    }

    n = snprintf(json, json_cap,
                 "{\"id\":\"12\",\"jsonrpc\":\"2.0\",\"method\":\"%s\",\"params\":%s}",
                 method, params_json);
    if (n < 0 || (size_t)n >= json_cap) {
        fprintf(stderr, "failed to build JSON request\n");
        free(json);
        return NULL;
    }

    json_len = (size_t)n;
    total_len = JRD_HEADER_LEN + json_len + 1U;
    if (total_len > 0xffffffffU) {
        fprintf(stderr, "request is too large\n");
        free(json);
        return NULL;
    }

    frame = (unsigned char *)calloc(1U, total_len);
    if (!frame) {
        perror("calloc");
        free(json);
        return NULL;
    }

    memcpy(frame + 0x00, "kcap", 4);
    put_le32(frame + 0x04, (uint32_t)total_len);
    put_le32(frame + 0x08, transaction);
    put_le32(frame + 0x0c, 1U);  /* client/channel: proven working */

    /* 0x10..0x2b remain zero for requests. */
    memcpy(frame + JRD_HEADER_LEN, json, json_len);
    frame[JRD_HEADER_LEN + json_len] = '\0';

    if (debug_enabled) {
        fprintf(stderr,
                "TX: total=%zu transaction=%" PRIu32 " client=1 json_len=%zu\n",
                total_len, transaction, json_len);
        fprintf(stderr, "TX JSON: %s\n", json);
    }

    free(json);
    *out_len = total_len;
    return frame;
}

static int receive_matching_response(int fd, uint32_t wanted_transaction)
{
    for (;;) {
        unsigned char first8[8];
        unsigned char *frame;
        uint32_t total;
        uint32_t rx_tx = 0;
        uint32_t client = 0;
        uint32_t response_flag = 0;
        uint32_t json_declared = 0;
        uint32_t tail_flags = 0;
        size_t available;
        size_t json_len;
        int r;

        r = read_exact(fd, first8, sizeof(first8));
        if (r == 0) {
            fprintf(stderr, "connection closed before matching response\n");
            return 1;
        }
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                fprintf(stderr, "timeout waiting for response\n");
            else
                perror("recv");
            return 1;
        }

        if (memcmp(first8, "kcap", 4) != 0) {
            fprintf(stderr, "invalid response magic\n");
            return 1;
        }

        total = get_le32(first8 + 4);
        if (total < 8U || total > JRD_MAX_FRAME) {
            fprintf(stderr, "invalid response length: %" PRIu32 "\n", total);
            return 1;
        }

        frame = (unsigned char *)malloc(total);
        if (!frame) {
            perror("malloc");
            return 1;
        }
        memcpy(frame, first8, 8);

        if (total > 8U) {
            r = read_exact(fd, frame + 8, (size_t)total - 8U);
            if (r == 0) {
                fprintf(stderr, "connection closed in middle of frame\n");
                free(frame);
                return 1;
            }
            if (r < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    fprintf(stderr, "timeout in middle of frame\n");
                else
                    perror("recv");
                free(frame);
                return 1;
            }
        }

        if (total >= 12U)
            rx_tx = get_le32(frame + 0x08);
        if (total >= 16U)
            client = get_le32(frame + 0x0c);
        if (total >= 24U)
            response_flag = get_le32(frame + 0x14);
        if (total >= 40U)
            json_declared = get_le32(frame + 0x24);
        if (total >= 44U)
            tail_flags = get_le32(frame + 0x28);

        if (debug_enabled) {
            fprintf(stderr,
                    "RX: total=%" PRIu32
                    " transaction=%" PRIu32
                    " client=%" PRIu32
                    " response=%" PRIu32
                    " json_len=%" PRIu32
                    " flags=0x%08" PRIx32 "\n",
                    total, rx_tx, client, response_flag,
                    json_declared, tail_flags);
        }

        /*
         * The modem emits short unsolicited frames with transaction 0.
         * Ignore those, and any unrelated transaction, until ours arrives.
         */
        if (rx_tx != wanted_transaction) {
            if (debug_enabled)
                fprintf(stderr, "RX: ignoring unrelated/notification frame\n");
            free(frame);
            continue;
        }

        if (total < JRD_HEADER_LEN) {
            fprintf(stderr,
                    "matching response has no 44-byte JSON header (length=%" PRIu32 ")\n",
                    total);
            free(frame);
            return 1;
        }

        available = (size_t)total - JRD_HEADER_LEN;

        /*
         * In observed replies, offset 0x24 is JSON length excluding the NUL.
         * Fall back to bounded NUL scanning if a firmware variant leaves it 0
         * or reports an impossible value.
         */
        if (json_declared > 0U && (size_t)json_declared <= available)
            json_len = (size_t)json_declared;
        else
            json_len = bounded_strlen(frame + JRD_HEADER_LEN, available);

        if (json_len == 0U) {
            fprintf(stderr, "matching response contains no JSON payload\n");
            free(frame);
            return 1;
        }

        if (fwrite(frame + JRD_HEADER_LEN, 1, json_len, stdout) != json_len) {
            perror("fwrite");
            free(frame);
            return 1;
        }
        fputc('\n', stdout);
        free(frame);
        return 0;
    }
}

static const char *map_command(const char *cmd)
{
    if (strcmp(cmd, "status") == 0 || strcmp(cmd, "connection") == 0)
        return "GetConnectionState";
    if (strcmp(cmd, "system") == 0)
        return "GetSystemStatus";
    if (strcmp(cmd, "connect") == 0)
        return "Connect";
    if (strcmp(cmd, "disconnect") == 0)
        return "DisConnect";
    if (strcmp(cmd, "network") == 0)
        return "GetNetworkInfo";
    if (strcmp(cmd, "sim") == 0)
        return "GetSimStatus";
    if (strcmp(cmd, "profiles") == 0)
        return "GetProfileList";
    if (strcmp(cmd, "current-profile") == 0)
        return "getCurrentProfile";
    return cmd;
}

int main(int argc, char **argv)
{
    const char *host = "192.168.225.1";
    uint16_t port = 2016;
    uint32_t transaction = 0;
    int transaction_set = 0;
    int timeout_sec = 5;
    const char *cmd;
    const char *method;
    const char *params_json = "{}";
    unsigned char *request;
    size_t request_len;
    int fd;
    int opt;
    int rc;

    while ((opt = getopt(argc, argv, "H:p:t:T:dh")) != -1) {
        switch (opt) {
        case 'H':
            host = optarg;
            break;
        case 'p':
            if (parse_u16(optarg, &port) < 0) {
                fprintf(stderr, "invalid port: %s\n", optarg);
                return 1;
            }
            break;
        case 't':
            if (parse_u32(optarg, &transaction) < 0 || transaction == 0U) {
                fprintf(stderr, "invalid transaction ID: %s\n", optarg);
                return 1;
            }
            transaction_set = 1;
            break;
        case 'T':
            if (parse_timeout(optarg, &timeout_sec) < 0) {
                fprintf(stderr, "invalid timeout: %s\n", optarg);
                return 1;
            }
            break;
        case 'd':
            debug_enabled = 1;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (optind >= argc) {
        usage(argv[0]);
        return 1;
    }

    cmd = argv[optind++];

    if (strcmp(cmd, "rpc") == 0) {
        if (optind >= argc) {
            fprintf(stderr, "rpc requires a method name\n");
            return 1;
        }
        method = argv[optind++];
    } else {
        method = map_command(cmd);
    }

    if (!valid_method(method)) {
        fprintf(stderr, "invalid RPC method name: %s\n", method);
        return 1;
    }

    if (optind < argc)
        params_json = argv[optind++];

    if (optind < argc) {
        fprintf(stderr, "too many arguments\n");
        return 1;
    }

    if (!transaction_set)
        transaction = default_transaction();

    request = build_request(method, params_json, transaction, &request_len);
    if (!request)
        return 1;

    fd = connect_modem(host, port, timeout_sec);
    if (fd < 0) {
        free(request);
        return 1;
    }

    if (write_all(fd, request, request_len) < 0) {
        perror("send");
        free(request);
        close(fd);
        return 1;
    }
    free(request);

    rc = receive_matching_response(fd, transaction);
    close(fd);
    return rc;
}
