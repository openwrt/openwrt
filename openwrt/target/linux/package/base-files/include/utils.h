#include <cy_conf.h>

extern int diag_led(int type, int act);
extern int C_led(int i);
extern int get_single_ip(char *ipaddr, int which);
extern char *get_mac_from_ip(char *ip);
extern struct dns_lists *get_dns_list(int no);
extern int dns_to_resolv(void);
extern char *get_wan_face(void);
extern int check_wan_link(int num);
extern char *get_complete_lan_ip(char *ip);
extern int get_int_len(int num);
extern int file_to_buf(char *path, char *buf, int len);
extern int buf_to_file(char *path, char *buf);
extern pid_t* find_pid_by_name( char* pidName);
extern int find_pid_by_ps(char* pidName);
extern int *find_all_pid_by_ps(char* pidName);
extern char *find_name_by_proc(int pid);
extern int get_ppp_pid(char *file);
extern long convert_ver(char *ver);
extern int check_flash(void);
extern int check_action(void);
extern int check_now_boot(void);
extern int check_hw_type(void);
extern int is_exist(char *filename);
extern void set_ip_forward(char c);
struct mtu_lists *get_mtu(char *proto);
extern void set_host_domain_name(void);

extern void encode(char *buf, int len);
extern void decode(char *buf, int len);

extern int sys_netdev_ioctl(int family, int socket, char *if_name, int cmd, struct ifreq *ifr);

int ct_openlog(const char *ident, int option, int facility, char *log_name);
void ct_syslog(int level, int enable, const char *fmt,...);
void ct_logger(int level, const char *fmt,...);
struct wl_assoc_mac * get_wl_assoc_mac(int *c);
	


enum { DMZ, SESSION, DIAG , WL};

enum { START_LED, STOP_LED };

typedef enum { ACT_IDLE, 
	       ACT_TFTP_UPGRADE, 
	       ACT_WEB_UPGRADE, 
	       ACT_WEBS_UPGRADE, 
	       ACT_SW_RESTORE, 
	       ACT_HW_RESTORE } ACTION;

enum { UNKNOWN_BOOT = -1, PMON_BOOT, CFE_BOOT };

enum { BCM4702_CHIP, BCM4712_CHIP, BCM5325E_CHIP };

enum { FIRST, SECOND };

enum { SYSLOG_LOG=1, SYSLOG_DEBUG, CONSOLE_ONLY, LOG_CONSOLE, DEBUG_CONSOLE };

#define ACTION(cmd)	buf_to_file(ACTION_FILE, cmd)

struct dns_lists {
        int num_servers;
        char dns_server[4][16];
};

#define NOT_USING	0
#define USING		1

struct wl_assoc_mac
{
	char mac[18];
};

struct mtu_lists {
        char	*proto;	/* protocol */
        char	*min;	/* min mtu */
        char	*max;	/* max mtu */
};


#define PPP_PSEUDO_IP	"10.64.64.64"
#define PPP_PSEUDO_NM	"255.255.255.255"
#define PPP_PSEUDO_GW	"10.112.112.112"

#define PING_TMP	"/tmp/ping.log"
#define TRACEROUTE_TMP	"/tmp/traceroute.log"
#define MAX_BUF_LEN	254

#define RESOLV_FILE	"/tmp/resolv.conf"
#define HOSTS_FILE	"/tmp/hosts"

#define LOG_FILE	"/var/log/mess"

#define ACTION_FILE	"/tmp/action"


#define split(word, wordlist, next, delim) \
	for (next = wordlist, \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL ; \
	     strlen(word); \
	     next = next ? : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[(next=strstr(next, delim)) ? strstr(word, delim) - word : sizeof(word) - 1] = '\0', \
	     next = next ? next + sizeof(delim) - 1 : NULL)

#define STRUCT_LEN(name)    sizeof(name)/sizeof(name[0])

#define printHEX(str,len) { \
	int i; \
	for (i=0 ; i<len ; i++) { \
		printf("%02X ", (unsigned char)*(str+i)); \
		if(((i+1)%16) == 0) printf("- "); \
		if(((i+1)%32) == 0) printf("\n"); \
	} \
	printf("\n\n"); \
}


#define printASC(str,len) { \
	int i; \
	for (i=0 ; i<len ; i++) { \
		printf("%c", (unsigned char)*(str+i)); \
		if(((i+1)%16) == 0) printf("- "); \
		if(((i+1)%32) == 0) printf("\n"); \
	} \
	printf("\n\n"); \
}
