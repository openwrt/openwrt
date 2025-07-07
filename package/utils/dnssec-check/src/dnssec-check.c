/*
 * dnssec-check - Simple DNSSEC validation tool
 * Copyright (C) 2025 Liu Yu <f78fk@live.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY...
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <uci.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>

#define DEFAULT_SECURE_DOMAIN "nic.cz"
#define DEFAULT_BROKEN_DOMAIN "dnssec-failed.org"
#define DEFAULT_DIG_PATH "/usr/bin/dig"
#define DEFAULT_TIME "3"
#define DEFAULT_TRIES "2"

#define BUF_SIZE 8192
#define EXIT_SUCCESS           0
#define EXIT_GENERAL_ERROR     1
#define EXIT_DIG_NOT_FOUND    127

static int g_debug_enabled = 0;
#define DEBUG_PRINT(fmt, ...) \
    do { if (g_debug_enabled) printf("[DEBUG] " fmt, ##__VA_ARGS__); } while (0)

static char g_dig_path[512] = DEFAULT_DIG_PATH;

static void handle_signal(int sig)
{
	printf("\n[INFO] Received signal %d, shutting down gracefully...\n",
	       sig);
	exit(EXIT_SUCCESS);
}

static void setup_signal_handlers()
{
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
}

static int run_dig(const char *domain, const char *args[], char *output_buf,
		   size_t buf_size, char *err_buf)
{
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		return 0;

	pid_t pid = fork();
	if (pid < 0)
		return 0;

	if (pid == 0) {
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		dup2(pipe_fd[1], STDERR_FILENO);
		close(pipe_fd[1]);

		char *argv[16];
		int i = 0;
		argv[i++] = (char *)g_dig_path;
		while (args[i - 1] != NULL && i < 14) {
			argv[i] = (char *)args[i - 1];
			i++;
		}
		argv[i++] = (char *)domain;
		argv[i++] = "A";
		argv[i] = NULL;

		execvp(g_dig_path, argv);
		perror("execvp failed");
		exit(1);
	} else {
		close(pipe_fd[1]);
		size_t total_read = 0;
		ssize_t bytes_read;
		while ((bytes_read =
			read(pipe_fd[0], output_buf + total_read,
			     buf_size - total_read - 1)) > 0) {
			total_read += bytes_read;
			if (total_read >= buf_size - 1)
				break;
		}
		output_buf[total_read] = '\0';
		close(pipe_fd[0]);

		int status;
		waitpid(pid, &status, 0);
		return WIFEXITED(status) && WEXITSTATUS(status) == 0;
	}
}

static char *strcasestr(const char *haystack, const char *needle)
{
	if (!*needle)
		return (char *)haystack;

	for (; *haystack; haystack++) {
		const char *h = haystack;
		const char *n = needle;

		while (*h && *n
		       && tolower((unsigned char)*h) ==
		       tolower((unsigned char)*n)) {
			h++;
			n++;
		}

		if (!*n)
			return (char *)haystack;
	}

	return NULL;
}

static void parse_dig_output(const char *output, int *ad_flag, char *rcode_buf)
{
	*ad_flag = 0;
	strcpy(rcode_buf, "UNKNOWN");

	char *copy = strdup(output);
	if (!copy) {
		fprintf(stderr, "[ERROR] strdup() failed\n");
		return;
	}

	char *line = NULL;
	char *rest = copy;

	DEBUG_PRINT("=== Begin parsing dig output ===\n");

	while ((line = strsep(&rest, "\n")) != NULL) {
		DEBUG_PRINT("LINE: %s\n", line);

		if (strstr(line, "HEADER") && strstr(line, "status:")) {
			DEBUG_PRINT("Found HEADER line with status:\n");

			char *pos = strstr(line, "status:");
			if (pos) {
				pos += strlen("status:");
				while (*pos == ' ')
					pos++;

				sscanf(pos, "%15[^, \n]", rcode_buf);
				DEBUG_PRINT("Parsed rcode_buf: %s\n",
					    rcode_buf);
			} else {
				printf
				    ("[WARN] 'status:' not found in HEADER line\n");
			}

		} else if (strstr(line, "flags:")) {
			DEBUG_PRINT("Found flags line\n");

			char *flag_start = strstr(line, "flags:");
			if (!flag_start)
				continue;

			flag_start += strlen("flags:");
			while (*flag_start == ' ')
				flag_start++;

			char *semi = strchr(flag_start, ';');
			if (semi)
				*semi = '\0';

			DEBUG_PRINT("Extracted flags segment: %s\n",
				    flag_start);

			char *token = strtok(flag_start, " \t");
			while (token) {
				DEBUG_PRINT("Found flag token: %s\n", token);
				if (strcmp(token, "ad") == 0) {
					*ad_flag = 1;
					DEBUG_PRINT
					    ("Found AD flag, set ad_flag = 1\n");
					break;
				}
				token = strtok(NULL, " \t");
			}
		}
	}

	DEBUG_PRINT("=== Finished parsing ===\n");
	free(copy);
}

static int query_domain(const char *domain, const char *args[], int *ad_flag,
			char *rcode, char *error_out)
{
	char output[BUF_SIZE] = { 0 };
	char error[256] = { 0 };

	if (!run_dig(domain, args, output, sizeof(output), error)) {
		snprintf(error_out, 256, "%s",
			 error[0] ? error : "dig execution failed");
		return 0;
	}

	parse_dig_output(output, ad_flag, rcode);
	return 1;
}

static void determine_result(const char *secure_domain, int secure_ad,
			     const char *rcode_secure,
			     const char *broken_domain, int broken_ad,
			     const char *rcode_broken)
{
	printf("Parsed %s: AD=%s, Status=%s\n", secure_domain,
	       secure_ad ? "true" : "false", rcode_secure);
	printf("Parsed %s: AD=%s, Status=%s\n", broken_domain,
	       broken_ad ? "true" : "false", rcode_broken);

	if (strcmp(rcode_broken, "NOERROR") == 0) {
		printf(" -> Severity level: insecure\n");
		return;
	}

	if (strcmp(rcode_secure, "NOERROR") != 0) {
		printf
		    (" -> Severity level: unknown (%s could not be resolved)\n",
		     secure_domain);
		return;
	}

	if (secure_ad && !broken_ad && strcmp(rcode_broken, "SERVFAIL") == 0) {
		printf(" -> Severity level: secure\n");
	} else if (!secure_ad && strcmp(rcode_broken, "SERVFAIL") == 0) {
		printf(" -> Severity level: medium\n");
	} else if (strcmp(rcode_broken, "SERVFAIL") != 0) {
		printf(" -> Severity level: insecure\n");
	} else {
		printf(" -> Severity level: unknown\n");
	}
}

static int find_dig_path(char *buf, size_t buflen)
{
	char *path_env = getenv("PATH");
	if (!path_env)
		return 0;

	char *paths = strdup(path_env);
	char *dir = strtok(paths, ":");
	while (dir) {
		char fullpath[512];
		snprintf(fullpath, sizeof(fullpath), "%s/dig", dir);
		if (access(fullpath, X_OK) == 0) {
			strncpy(buf, fullpath, buflen - 1);
			buf[buflen - 1] = '\0';
			free(paths);
			return 1;
		}
		dir = strtok(NULL, ":");
	}
	free(paths);
	return 0;
}

static int load_uci_config(char *secure_dom, size_t len1, char *broken_dom,
			   size_t len2, char *t, size_t tlen, char *tr,
			   size_t trlen)
{
	struct uci_context *ctx = uci_alloc_context();
	struct uci_package *pkg = NULL;
	struct uci_section *section;
	struct uci_element *e;

	if (!ctx || uci_load(ctx, "dnssec-check", &pkg) != UCI_OK) {
		uci_free_context(ctx);
		return 0;
	}

	uci_foreach_element(&pkg->sections, e) {
		section = uci_to_section(e);
		if (strcmp(section->type, "settings") == 0) {
			uci_foreach_element(&section->options, e) {
				struct uci_option *o = uci_to_option(e);
				if (strcmp(e->name, "secure-domain") == 0)
					strncpy(secure_dom, o->v.string,
						len1 - 1);
				else if (strcmp(e->name, "broken-domain") == 0)
					strncpy(broken_dom, o->v.string,
						len2 - 1);
				else if (strcmp(e->name, "dig-time") == 0)
					strncpy(t, o->v.string, tlen - 1);
				else if (strcmp(e->name, "dig-tries") == 0)
					strncpy(tr, o->v.string, trlen - 1);
				else if (strcmp(e->name, "debug") == 0) {
					if (strcmp(o->v.string, "1") == 0)
						g_debug_enabled = 1;
				}
			}
			break;
		}
	}

	uci_unload(ctx, pkg);
	uci_free_context(ctx);
	return 1;
}

int main(void)
{
	setup_signal_handlers();

	char secure_domain[128] = DEFAULT_SECURE_DOMAIN;
	char broken_domain[128] = DEFAULT_BROKEN_DOMAIN;
	char dig_time[16] = DEFAULT_TIME;
	char dig_tries[16] = DEFAULT_TRIES;

	if (!load_uci_config
	    (secure_domain, sizeof(secure_domain), broken_domain,
	     sizeof(broken_domain), dig_time, sizeof(dig_time), dig_tries,
	     sizeof(dig_tries))) {
		printf("[INFO] Using default configuration.\n");
	} else {
		printf("[INFO] Loaded configuration from UCI.\n");
	}

	char time_arg[32], tries_arg[32];
	snprintf(time_arg, sizeof(time_arg), "+time=%s", dig_time);
	snprintf(tries_arg, sizeof(tries_arg), "+tries=%s", dig_tries);
	const char *dig_args[] =
	    { "+dnssec", time_arg, tries_arg, "+multi", NULL };

	if (!find_dig_path(g_dig_path, sizeof(g_dig_path))) {
		fprintf(stderr, "dig not found in PATH\n");
		exit(EXIT_DIG_NOT_FOUND);
	}

	printf("[*] Querying %s ...\n", secure_domain);
	int secure_ad = 0;
	char rcode_secure[32] = { 0 }, error[256] = { 0 };
	int secure_query_result =
	    query_domain(secure_domain, dig_args, &secure_ad, rcode_secure,
			 error);
	if (!secure_query_result)
		fprintf(stderr, "[ERROR] Failed to query %s: %s\n",
			secure_domain, error);

	printf("[*] Querying %s ...\n", broken_domain);
	int broken_ad = 0;
	char rcode_broken[32] = { 0 };
	int broken_query_result =
	    query_domain(broken_domain, dig_args, &broken_ad, rcode_broken,
			 error);
	if (!broken_query_result)
		fprintf(stderr, "[ERROR] Failed to query %s: %s\n",
			broken_domain, error);

	if (!secure_query_result || !broken_query_result) {
		exit(EXIT_GENERAL_ERROR);
	}

	determine_result(secure_domain, secure_ad, rcode_secure,
			 broken_domain, broken_ad, rcode_broken);
	return 0;
}
