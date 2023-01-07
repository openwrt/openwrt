#include "qlog.h"

unsigned int inet_addr(const char *cp);
char *inet_ntoa(struct in_addr in);

static int wait_tcp_client_connect(int tcp_port) {
    int sockfd, n, connfd;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    int reuse_addr = 1;
    size_t sin_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        qlog_dbg("Create socket fail!\n");
        return 0;
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(tcp_port);

    qlog_dbg("Starting the TCP server(%d)...\n", tcp_port);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr));
        
    n = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (n == -1)
    {
        qlog_dbg("bind fail! errno: %d\n", errno);
        close(sockfd);
        return 0;
    }
    qlog_dbg("bind OK!\n");

    n = listen(sockfd, 1);
    if (n == -1)
    {
        qlog_dbg("listen fail! errno: %d\n", errno);
        close(sockfd);
        return 0;
    }
    qlog_dbg("listen OK!\nWaiting the TCP Client...\n");

    sin_size = sizeof(struct sockaddr_in);
    connfd = accept(sockfd, (struct sockaddr *)&clientaddr, (socklen_t *)&sin_size);
    close(sockfd);
    if (connfd == -1)
    {
        qlog_dbg("accept fail! errno: %d\n", errno);
        return -1;
    }

    qlog_dbg("TCP Client %s:%d connect\n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);

    return connfd;
}

int tty2tcp_sockfd = -1;
static int tty2tcp_ttyfd = -1;
static int tty2tcp_tcpport = 9000;

static void *tcp_sock_read_Loop(void *arg) {
    int sockfd = tty2tcp_sockfd;
    int ttyfd = tty2tcp_ttyfd;
    void *rbuf;
    const size_t rbuf_size = (4*1024);
    
    rbuf = malloc(rbuf_size);
    if (rbuf == NULL) {
          qlog_dbg("Fail to malloc rbuf_size=%zd, errno: %d (%s)\n", rbuf_size, errno, strerror(errno));
          return NULL;
    }

    while (sockfd > 0) {      
        ssize_t rc, wc;
        int ret;
        struct pollfd pollfds[] = {{sockfd, POLLIN, 0}};

        ret = poll(pollfds,  1, -1);

        if (ret <= 0) {
            qlog_dbg("poll(ttyfd) =%d, errno: %d (%s)\n", ret, errno, strerror(errno));
            break;
        }

        if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            tty2tcp_sockfd = -1;
            close(sockfd);
            qlog_dbg("ttyfd revents = %04x\n", pollfds[0].revents);
            sockfd = tty2tcp_sockfd = wait_tcp_client_connect(tty2tcp_tcpport);
        }

        if (pollfds[0].revents & (POLLIN)) {
            rc = read(sockfd, rbuf, rbuf_size);
            
            if(rc > 0) {
                if (g_is_asr_chip)
                    wc = asr_send_cmd(ttyfd, rbuf, rc);
                else
                    wc = mdm_send_cmd(ttyfd, rbuf, rc);
 
                if (wc != rc) {
                    //qlog_dbg("ttyfd write fail %zd/%zd, break\n", wc, rc);
                    //break;
                }               
            }
            else
            {
                tty2tcp_sockfd = -1;
                close(sockfd);
                qlog_dbg("sockfd recv %zd Bytes. maybe terminae by peer!\n", rc);
                sockfd = tty2tcp_sockfd = wait_tcp_client_connect(tty2tcp_tcpport);
            }
        }
    }

    free(rbuf);
    close(ttyfd);
    tty2tcp_sockfd = -1;
    qlog_dbg("%s exit\n", __func__);

    return NULL;
}

static int tty2tcp_init_filter(int ttyfd, const char *cfg) {
    int tcp_port = 9000;
    int sockfd = -1;
    pthread_t tid;
    pthread_attr_t attr;

    if (cfg)
        tcp_port = atoi(cfg);

    tty2tcp_tcpport = tcp_port;
    sockfd = wait_tcp_client_connect(tcp_port);

    if (sockfd <= 0)
        return -1;

    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);
    
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    tty2tcp_sockfd = sockfd;
    tty2tcp_ttyfd = ttyfd;
    pthread_create(&tid, &attr, tcp_sock_read_Loop, &attr);

    return 0;
}

static int tty2tcp_logfile_create(const char *logfile_dir, const char *logfile_suffix, unsigned logfile_seq) {
    return 1;
}

static size_t tty2tcp_logfile_save(int logfd, const void *buf, size_t size) {
    if (tty2tcp_sockfd > 0)
        return qlog_poll_write(tty2tcp_sockfd, buf, size, 200);
    return size;
}

static int tty2tcp_logfile_close(int logfd) {
    return 0;
}

qlog_ops_t tty2tcp_qlog_ops = {
    .init_filter = tty2tcp_init_filter,
    .logfile_create = tty2tcp_logfile_create,
    .logfile_save = tty2tcp_logfile_save,
    .logfile_close = tty2tcp_logfile_close,
};
