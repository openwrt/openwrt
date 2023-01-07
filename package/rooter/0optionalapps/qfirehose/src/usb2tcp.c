/******************************************************************************
  @file    tty2tcp.c
  @brief   switch data between tcp socket and ttyUSB port.

  DESCRIPTION
  QLog Tool for USB and PCIE of Quectel wireless cellular modules.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2016 - 2020 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <netinet/in.h>
#include <linux/un.h>
#include <linux/usbdevice_fs.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 20)
#include <linux/usb/ch9.h>
#else
#include <linux/usb_ch9.h>
#endif
//#include <asm/byteorder.h>
#include "usb_linux.h"
#include <endian.h> //for __BYTE_ORDER
char *inet_ntoa(struct in_addr in);

#define dprintf dbg_time

#define MIN(a,b)	 ((a) < (b)? (a) : (b))

#define MAX_USBFS_BULK_IN_SIZE (4 * 1024)
#define MAX_USBFS_BULK_OUT_SIZE (16 * 1024)

static uint32_t cpu_to_le32 (uint32_t v32) {
    uint32_t tmp = v32;
#if __BYTE_ORDER == __LITTLE_ENDIAN
#else
    unsigned char *s = (unsigned char *)(&v32);
    unsigned char *d = (unsigned char *)(&tmp);
    d[0] = s[3];
    d[1] = s[2];
    d[2] = s[1];
    d[3] = s[0];
#endif
    return tmp;
}
#define le32_to_cpu(_v32) cpu_to_le32(_v32)

static int qusb_control[2];

static int noblock_full_read(int fd, void *pbuf, ssize_t size) {
    ssize_t cur = 0;
        
    while (cur < size) {
        ssize_t ret = read(fd, (char *)pbuf+cur, size-cur);

        if (ret > 0)
            cur += ret;
        else if (ret < 0 && errno == EAGAIN) {
            struct pollfd pollfds[] = {{fd, POLLIN, 0}};
            poll(pollfds, 1, -1);
            if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
                break;
        } else {
            dprintf("fd=%d read=%zd, errno: %d (%s)\n", fd, ret, errno, strerror(errno));
            break;
        }
    }

    if (cur != size) {
        dprintf("%s fd=%d cur=%zd, size=%zd\n", __func__, fd, cur, size);
    }
    
    return cur;
}

static ssize_t noblock_full_write(int fd, const void *pbuf, ssize_t size) {
    ssize_t cur = 0;
        
    while (cur < size) {
        ssize_t ret = write(fd, (char *)pbuf+cur, size-cur);
        if (ret > 0)
            cur += ret;
        else if (ret <= 0 && errno == EAGAIN) {
            struct pollfd pollfds[] = {{fd, POLLOUT, 0}};
            poll(pollfds, 1, -1);
            if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
                break;
        } else {
            dprintf("fd=%d write=%zd, errno: %d (%s)\n", fd, ret, errno, strerror(errno));
            break;
        }
    }

    if (cur != size) {
        dprintf("%s fd=%d cur=%zd, size=%zd\n", __func__, fd, cur, size);
    }
    
    return cur;
}

static void* usb_bulk_read_thread(void* arg) {
    const void *usb_handle = arg;
    void *buf = malloc(MAX_USBFS_BULK_IN_SIZE);
    int fd = qusb_control[1];

    if (buf == NULL)
        return NULL;

    while(usb_handle) {
        int count = qusb_noblock_read(usb_handle,  buf, MAX_USBFS_BULK_IN_SIZE, 1, 30000);
                
        if (count > 0) {
            count = write(fd, buf, count);
            count = read(fd, buf, 32); //wait usb2tcp_main read
            if (count <= 0) {
                dprintf("read=%d\n", count);
                break;
            }
        } else if (count <= 0) {
            break;
        }
    }

    close(fd);
    free(buf);
    return NULL;
}

static int qusb_open(const void *usb_handle) {
    int fd = -1;
    pthread_t thread_id;

    pthread_attr_t usb_thread_attr;
    pthread_attr_init(&usb_thread_attr);
    pthread_attr_setdetachstate(&usb_thread_attr, PTHREAD_CREATE_DETACHED);

    socketpair(AF_LOCAL, SOCK_STREAM, 0, qusb_control);
    pthread_create(&thread_id, &usb_thread_attr, usb_bulk_read_thread, (void*)usb_handle);

    fd = qusb_control[0];

    return fd;
}

static ssize_t qusb_read(int fd, void* pbuf, size_t size) {
    return read(fd, pbuf, size);
}

static int create_tcp_server(int socket_port) {
    int sockfd = -1;
    int reuse_addr = 1;
    struct sockaddr_in sockaddr;

    dprintf("%s tcp_port=%d\n", __func__, socket_port);
    /*Create server socket*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <= 0)
        return sockfd;

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(socket_port);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr));
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        close(sockfd);
        dprintf("%s bind %d errno: %d (%s)\n", __func__, socket_port, errno, strerror(errno));
        return -1;
    }

    return sockfd;
}

static int wait_client_connect(int server_fd) {
    int client_fd = -1;
    unsigned char addr[128];
    socklen_t alen = sizeof(addr);

    dprintf("%s\n", __func__);
    listen(server_fd, 1);
    client_fd = accept(server_fd, (struct sockaddr *)addr, &alen);
    if (client_fd <= 0)
        return client_fd;

    if (client_fd > 0) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
        dprintf("clientfd = %d %s:%d connect\n", client_fd, inet_ntoa(addr_in->sin_addr), addr_in->sin_port);
    }
    
    return client_fd;
}

int usb2tcp_main(const void *usb_handle, int tcp_port, unsigned qusb_zlp_mode) {
    void *pbuf = malloc(MAX_USBFS_BULK_OUT_SIZE);
    int server_fd = -1, client_fd = -1, usb_fd = -1, size = -1;
    TLV_USB tlv_usb;

    if (pbuf == NULL)
        return -1;

    server_fd  = create_tcp_server(tcp_port);
    dprintf("server_fd=%d\n", server_fd);
    if (server_fd <= 0) {
        dprintf("Fail create_tcp_server\n");
        goto _out;
    }
    
    if (client_fd <= 0) {
        client_fd = wait_client_connect(server_fd);
        if (client_fd < 0) {
            dprintf("Fail wait_client_connect\n");
            goto _out;
        }
    }

    usb_fd = qusb_open(usb_handle);
    dprintf("usb_fd = %d\n", usb_fd);

    tlv_usb.tag = cpu_to_le32(Q_USB2TCP_VERSION);
    tlv_usb.length = cpu_to_le32(12);
    tlv_usb.idVendor = cpu_to_le32(0x05c6);
    tlv_usb.idProduct = cpu_to_le32(0x9008);
    tlv_usb.interfaceNum = cpu_to_le32(1);
    if (write(client_fd, &tlv_usb, sizeof(tlv_usb)) == -1) {};

    fcntl(usb_fd, F_SETFL, fcntl(usb_fd,F_GETFL) | O_NONBLOCK);
    fcntl(client_fd, F_SETFL, fcntl(client_fd,F_GETFL) | O_NONBLOCK);

    while (usb_fd > 0 && client_fd > 0) {
        struct pollfd pollfds[] = {{usb_fd, POLLIN, 0}, {client_fd, POLLIN, 0}};
        int ne, ret, nevents = sizeof(pollfds)/sizeof(pollfds[0]);

        do {
            ret = poll(pollfds, nevents,  -1);
         } while (ret < 0 && errno == EINTR);


        if (ret <= 0) {
            dprintf("%s poll=%d, errno: %d (%s)\n", __func__, ret, errno, strerror(errno));
            goto _hangup;
        }


        if (pollfds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            dprintf("%s poll usb_fd = %d, revents = %04x\n", __func__, usb_fd, pollfds[0].revents);
            goto _hangup;
        }


        if (pollfds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            dprintf("%s poll client_fd = %d, revents = %04x\n", __func__, client_fd, pollfds[1].revents);
            goto _hangup;
        }
        
        for (ne = 0; ne < nevents; ne++) {
            int fd = pollfds[ne].fd;
            TLV tlv = {Q_USB2TCP_VERSION, 0};
            
            if ((pollfds[ne].revents & POLLIN) == 0)
                continue;

            if (fd == usb_fd) {
                size = qusb_read(usb_fd, pbuf, MAX_USBFS_BULK_IN_SIZE);
                if (size <= 0) {
                    dprintf("usb_fd=%d read=%d, errno: %d (%s)\n", fd, size, errno, strerror(errno));
                    goto _hangup;;
                }
                if (write(usb_fd, pbuf, 1) == -1) {}; //wakeup usb_bulk_read_thread

                tlv.tag = cpu_to_le32(Q_USB2TCP_VERSION);
                tlv.length = cpu_to_le32(size);
                if (sizeof(tlv) != noblock_full_write(client_fd, &tlv, sizeof(tlv))) {
                    goto _hangup;
                    break;
                }
                
                if (size != noblock_full_write(client_fd, pbuf, size)) {
                    goto _hangup;
                    break;
                }
            }
            else if (fd == client_fd) {
                size = noblock_full_read(client_fd, &tlv, sizeof(tlv));
                if (size !=  sizeof(tlv)) {
                    dprintf("client_fd=%d read=%d, errno: %d (%s)\n", fd, size, errno, strerror(errno));
                    goto _hangup;
                }

                if (le32_to_cpu(tlv.tag) != Q_USB2TCP_VERSION) {
                    break;
                }

                size = le32_to_cpu(tlv.length);
                if (size != noblock_full_read(client_fd, pbuf, size)) {
                    goto _hangup;
                    break;
                }
                qusb_noblock_write(usb_handle, pbuf, size, size, 3000, qusb_zlp_mode);
            }
        }
    }

_hangup:
    if (usb_fd > 0) {
        close(usb_fd);
        usb_fd = -1;
    }
    if (client_fd > 0) {
        close(client_fd);
        client_fd = -1;
    }

_out:
    free(pbuf);
    return 0;
}
