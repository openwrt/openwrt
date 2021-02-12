/**********************************************************************
 *                          gostsum12.c                               *
 *             Copyright (c) 2005-2014 Cryptocom LTD                  *
 *         This file is distributed under same license as OpenSSL     *
 *                                                                    *
 *    Implementation of GOST R 34.11-2012 hash function as            *
 *    command line utility more or less interface                     *
 *    compatible with md5sum and sha1sum                              *
 *    Doesn't need OpenSSL                                            *
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#ifdef _WIN32
# include <io.h>
#endif
#include <string.h>
#include "gosthash2012.h"

#define BUF_SIZE 262144
#define gost_hash_ctx gost2012_hash_ctx
#define GOST34112012Init init_gost2012_hash_ctx
#define GOST34112012Update gost2012_hash_block
#define GOST34112012Final gost2012_finish_hash

#define MAX_HASH_SIZE 128

typedef unsigned char byte;

int hashsize = 256;
int hash_file(gost_hash_ctx * ctx, char *filename, char *sum, int mode);
int hash_stream(gost_hash_ctx * ctx, int fd, char *sum);
int get_line(FILE *f, char *hash, char *filename, int verbose);

void help()
{
    fprintf(stderr, "Calculates GOST R 34.11-2012 hash function\n\n");
    fprintf(stderr, "gostsum12 [-bvl] [-c [file]]| [files]|-x\n"
            "\t-c check message digests (default is generate)\n"
            "\t-v verbose, print file names when checking\n"
            "\t-b read files in binary mode\n"
            "\t-l use 512 bit hash (default 256 bit)\n"
            "\t-x read filenames from stdin rather than from arguments \n"
            "The input for -c should be the list of message digests and file names\n"
            "that is printed on stdout by this program when it generates digests.\n");
    exit(3);
}

#ifndef O_BINARY
# define O_BINARY 0
#endif

int start_hash12(gost_hash_ctx * ctx)
{
    GOST34112012Init(ctx, hashsize);
    return 1;
}

int hash12_block(gost_hash_ctx * ctx, const byte * block, size_t length)
{
    GOST34112012Update(ctx, block, length);
    return 1;
}

int finish_hash12(gost_hash_ctx * ctx, byte * hashval)
{
    GOST34112012Final(ctx, hashval);
    return 1;
}

int main(int argc, char **argv)
{
    int c, i;
    int verbose = 0;
    int errors = 0;
    int open_mode = O_RDONLY;
    FILE *check_file = NULL;
    int filenames_from_stdin = 0;
    gost_hash_ctx ctx;

    while ((c = getopt(argc, argv, "bxlvc::")) != -1) {
        switch (c) {
        case 'b':
            open_mode = open_mode | O_BINARY;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'l':
            hashsize = 512;
            break;
        case 'x':
            filenames_from_stdin = 1;
            break;
        case 'c':
            if (optarg) {
                check_file = fopen(optarg, "r");
                if (!check_file) {
                    perror(optarg);
                    exit(2);
                }
            } else {
                check_file = stdin;
            }
            break;
        default:
            fprintf(stderr, "invalid option %c", optopt);
            help();
        }
    }
    if (check_file) {
        char inhash[MAX_HASH_SIZE + 1], calcsum[MAX_HASH_SIZE + 1],
            filename[PATH_MAX];
        int failcount = 0, count = 0;;
        if (check_file == stdin && optind < argc) {
            check_file = fopen(argv[optind], "r");
            if (!check_file) {
                perror(argv[optind]);
                exit(2);
            }
        }
        while (get_line(check_file, inhash, filename, verbose)) {
            count++;
            if (!hash_file(&ctx, filename, calcsum, open_mode)) {
                errors++;
                continue;
            }
            if (!strncmp(calcsum, inhash, hashsize / 4 + 1)) {
                if (verbose) {
                    fprintf(stderr, "%s\tOK\n", filename);
                }
            } else {
                if (verbose) {
                    fprintf(stderr, "%s\tFAILED\n", filename);
                } else {
                    fprintf(stderr,
                            "%s: GOST hash sum check failed for '%s'\n",
                            argv[0], filename);
                }
                failcount++;
            }
        }
        if (errors) {
            fprintf(stderr,
                    "%s: WARNING %d of %d file(s) cannot be processed\n",
                    argv[0], errors, count);

        }
        if (failcount) {
            fprintf(stderr,
                    "%s: WARNING %d of %d file(s) failed GOST hash sum check\n",
                    argv[0], failcount, count - errors);
        }
        exit((failcount || errors) ? 1 : 0);
    } else if (filenames_from_stdin) {
        char sum[65];
        char filename[PATH_MAX + 1], *end;
        while (!feof(stdin)) {
            if (!fgets(filename, PATH_MAX, stdin))
                break;
            for (end = filename; *end; end++) ;
            end--;
            for (; *end == '\n' || *end == '\r'; end--)
                *end = 0;
            if (!hash_file(&ctx, filename, sum, open_mode)) {
                errors++;
            } else {
                printf("%s %s\n", sum, filename);
            }
        }
    } else if (optind == argc) {
        char sum[65];
#ifdef _WIN32
        if (open_mode & O_BINARY) {
            _setmode(fileno(stdin), O_BINARY);
        }
#endif
        if (!hash_stream(&ctx, fileno(stdin), sum)) {
            perror("stdin");
            exit(1);
        }
        printf("%s -\n", sum);
        exit(0);
    } else {
        for (i = optind; i < argc; i++) {
            char sum[65];
            if (!hash_file(&ctx, argv[i], sum, open_mode)) {
                errors++;
            } else {
                printf("%s %s\n", sum, argv[i]);
            }
        }
    }
    exit(errors ? 1 : 0);
}

int hash_file(gost_hash_ctx * ctx, char *filename, char *sum, int mode)
{
    int fd;
    if ((fd = open(filename, mode)) < 0) {
        perror(filename);
        return 0;
    }
    if (!hash_stream(ctx, fd, sum)) {
        perror(filename);
        return 0;
    }
    close(fd);
    return 1;
}

int hash_stream(gost_hash_ctx * ctx, int fd, char *sum)
{
    unsigned char buffer[BUF_SIZE];
    ssize_t bytes;
		size_t i;

    start_hash12(ctx);
    while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
        hash12_block(ctx, buffer, bytes);
    }
    if (bytes < 0) {
        return 0;
    }
    finish_hash12(ctx, buffer);
    for (i = 0; i < (hashsize / 8); i++) {
        sprintf(sum + 2 * i, "%02x", buffer[i]);
    }
    return 1;
}

int get_line(FILE *f, char *hash, char *filename, int verbose)
{
    int i, len;
    int hashstrlen = hashsize / 4;
    while (!feof(f)) {
        if (!fgets(filename, PATH_MAX, f))
            return 0;
        len = strlen(filename);
        if (len < hashstrlen + 2) {
            goto nextline;
        }
        if (filename[hashstrlen] != ' ') {
            goto nextline;
        }
        for (i = 0; i < hashstrlen; i++) {
            if (filename[i] < '0' || (filename[i] > '9' && filename[i] < 'A')
                || (filename[i] > 'F' && filename[i] < 'a')
                || filename[i] > 'f') {
                goto nextline;
            }
        }
        memcpy(hash, filename, hashstrlen);
        hash[hashstrlen] = 0;
        while (filename[--len] == '\n' || filename[len] == '\r')
            filename[len] = 0;
        memmove(filename, filename + hashstrlen + 1, len - hashstrlen + 1);
        return 1;
 nextline:
        if (verbose)
	    printf("%s\n", filename);
    }
    return 0;
}
