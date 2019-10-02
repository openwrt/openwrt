
/*
	author: destan19@126.com
	微信公众号: wifi开发者
	date:2019/1/10
*/
#include <linux/init.h>
#include <linux/module.h>
#include <net/tcp.h>
#include <linux/netfilter.h>
#include <net/netfilter/nf_conntrack.h>
#include <linux/skbuff.h>
#include <net/ip.h>
#include <linux/types.h>
#include <net/sock.h>
#include <linux/etherdevice.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include "app_filter.h"
#include "af_utils.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("destan19@126.com");
MODULE_DESCRIPTION("app filter module");
MODULE_VERSION("1.0.2");
struct list_head af_feature_head = LIST_HEAD_INIT(af_feature_head);
#define AF_FEATURE_CONFIG_FILE "/etc/appfilter/feature.cfg"
#define AF_DEV_NAME "appfilter"

DEFINE_RWLOCK(af_feature_lock);

#define feature_list_read_lock() 		read_lock_bh(&af_feature_lock);
#define feature_list_read_unlock() 		read_unlock_bh(&af_feature_lock);
#define feature_list_write_lock() 		write_lock_bh(&af_feature_lock);
#define feature_list_write_unlock()		write_unlock_bh(&af_feature_lock);

#define MIN_HTTP_DATA_LEN 16
#define MAX_APP_NAME_LEN 64
#define MAX_FEATURE_NUM_PER_APP 16 
#define MAX_FEATURE_STR_LEN 128
#define MAX_HOST_URL_LEN 128
#define MAX_REQUEST_URL_LEN 128
#define MAX_MATH_DATA_LEN 1600 
#define MAX_FEATURE_BITS 16
#define MAX_POS_INFO_PER_FEATURE 16
#define MAX_FEATURE_LINE_LEN 256
#define MIN_FEATURE_LINE_LEN 16

typedef struct af_pos_info{
	int pos;
	unsigned char value;
}af_pos_info_t;

typedef struct af_feature_node{
	struct list_head  		head;
	int app_id;
	char app_name[MAX_APP_NAME_LEN];
	char feature_str[MAX_FEATURE_NUM_PER_APP][MAX_FEATURE_STR_LEN];
	int proto;
	int dport;
	char host_url[MAX_HOST_URL_LEN];
	char request_url[MAX_REQUEST_URL_LEN];
	int pos_num;
	af_pos_info_t pos_info[MAX_POS_INFO_PER_FEATURE];
}af_feature_node_t;

struct af_config_dev {
    dev_t id;
    struct cdev char_dev;
    struct class *c;
};
struct af_config_dev g_af_dev;

static void show_feature_list(void)
{
	af_feature_node_t *n,*node;
	unsigned int count = 0;
	feature_list_read_lock();
	if(!list_empty(&af_feature_head)) { // handle qos
		list_for_each_entry_safe(node, n, &af_feature_head, head) {
			count ++;
			printk("[%d] id=%d appname:%s, dport:%d, host:%s, request:%s\n", 
				count,
				node->app_id, node->app_name,
				node->dport,node->host_url, node->request_url);
			int i;
			for (i = 0;i < node->pos_num;i++){
				printk("(%d:%x)-->",
					node->pos_info[i].pos,
					node->pos_info[i].value);
				
			}
			printk("\n----------------------------------------\n\n\n");
		}
	}
	feature_list_read_unlock();
}

static af_feature_node_t* af_find_feature(char *app_id) 
{
	af_feature_node_t *node;
	feature_list_read_lock();

	if (!list_empty(&af_feature_head)) {
		list_for_each_entry(node, &af_feature_head, head) {
			if (node->app_id == app_id){
				feature_list_read_unlock();
				return node;
			}
		}
	}
	feature_list_read_unlock();
	return NULL;
}
enum AF_FEATURE_PARAM_INDEX{
	AF_PROTO_PARAM_INDEX,
	AF_DST_PORT_PARAM_INDEX,
	AF_HOST_URL_PARAM_INDEX,
	AF_REQUEST_URL_PARAM_INDEX,
	AF_DICT_PARAM_INDEX,
};

int __add_app_feature(int appid,
					char *name,
					int proto,
					int dst_port,
					char *host_url,
					char *request_url,
					char *dict)
{
	af_feature_node_t *node = NULL;
	node = kzalloc(sizeof(af_feature_node_t), GFP_KERNEL);
	if (node == NULL) {
		printk("malloc feature memory error\n");
		return -1;
	}
	else {
		node->app_id = appid;
		strcpy(node->app_name, name);
		node->proto = proto;
		node->dport = dst_port;
		strcpy(node->host_url, host_url);
		strcpy(node->request_url, request_url);
		// 00:0a-01:11
		char *p = dict;
		char *begin = dict;
		char pos[32] = {0};
		int index = 0;
		int value = 0;
		
		while (*p++) {
			if (*p == '-'){
				memset(pos, 0x0, sizeof(pos));
				strncpy(pos, begin, p - begin);
				k_sscanf(pos, "%d:%x",&index, &value);
				begin = p + 1;
				node->pos_info[node->pos_num].pos = index;
				node->pos_info[node->pos_num].value = value;
				node->pos_num++;
			}
		}
		
		if (begin != dict) {
			strncpy(pos, begin, p - begin);
			k_sscanf(pos, "%d:%x",&index, &value);
			node->pos_info[node->pos_num].pos = index;
			node->pos_info[node->pos_num].value = value;
			node->pos_num++;
		}
		feature_list_write_lock();
		list_add(&(node->head), &af_feature_head);
		feature_list_write_unlock();
	}
}

int add_app_feature(int appid, char *name, char *feature)
{
	char proto_str[16] = {0};
	int port_str[16] = {0};
	char host_url[32] = {0};
	char request_url[128] = {0};
	char dict[128] = {0};
	int proto = IPPROTO_TCP;
	if (!name || !feature) {
		printk("error, name or feature is null\n");
		return -1;
	}
	// tcp;8000;www.sina.com;0:get_name;00:0a-01:11
	
	char *p = feature;
	char *begin = feature;
	int param_num = 0;
	while(*p++) {
		if (*p != ';')
			continue;
		
		switch(param_num){
		case AF_PROTO_PARAM_INDEX:
			strncpy(proto_str, begin, p - begin);
			break;
			
		case AF_DST_PORT_PARAM_INDEX:
			strncpy(port_str, begin, p - begin);
			break;
			
		case AF_HOST_URL_PARAM_INDEX:
			strncpy(host_url, begin, p - begin);
			break;
		
		case AF_REQUEST_URL_PARAM_INDEX:
			strncpy(request_url, begin, p - begin);
			break;
		}
		param_num ++;
		begin = p + 1;
	}
	if (AF_DICT_PARAM_INDEX != param_num) {
		printk("invalid feature:%s\n", feature);
		return -1;
	}
	strncpy(dict, begin, p - begin);
	
	//sscanf(feature, "%[^;];%d;%[^;];%[^;];%s", proto, &dst_port, host, url, dict);
	//printk("proto = %s, port = %s, host = %s, url = %s, dict = %s\n",
	//				proto_str, port_str, host_url, request_url, dict);
	if (0 == strcmp(proto_str, "tcp"))
		proto = IPPROTO_TCP;
	else if (0 == strcmp(proto_str, "udp"))
		proto = IPPROTO_UDP;
	else {
		printk("proto %s is not support\n", proto_str);
		return -1;
	}
	int dst_port = 0;
	sscanf(port_str, "%d", &dst_port);
	
	__add_app_feature(appid,
					name,
					proto,
					dst_port,
					host_url,
					request_url,
					dict);

	return 0;
}




void af_init_feature(char *feature_str)
{
//	char * qq_config_str = "1001    qq     :[tcp;8000;r:www.baidu.com;4:get_status;00:0a-01:11]";
//	char * qq_config_str = "1001    qq:[tcp;443;baidu.com;get_status;00:0a-01:11,tcp;8000;www.sina.com;0:22222get;00:0a-01:11,tcp;8000;www.sina.com;hao123;00:0a-01:11-02:ff]";
//	char * qq_config_str = "1001    qq:[tcp;443;r:www.baidu.com;4:get_status;00:0a-01:11,tcp;8000;www.sina.com;0:22222get;00:0a-01:11,tcp;8000;www.sina.com;hao123;00:0a-01:11-02:ff]";
	int app_id;
	char app_name[128] = {0};
	char feature_buf[MAX_FEATURE_LINE_LEN] = {0};
	
	printk("feature_str=%s\n",feature_str);
	k_sscanf(feature_str, "%d%[^:]", &app_id, app_name);
	printk("id = %d, name = %s\n",app_id, app_name);

	char *p = feature_str;
	char *pos = NULL;	
	int len = 0;
	while(*p++) {
		if (*p == '['){
			pos = p + 1;
			continue;
		}
		if (*p == ']' && pos != NULL) {
			len = p - pos;
		}
	}

	if (pos && len )
		strncpy(feature_buf, pos, len);
	char feature[MAX_FEATURE_STR_LEN];;
	int i;
	memset(feature, 0x0, sizeof(feature));
	p = feature_buf;
	char *begin = feature_buf;

	while(*p++){
		if (*p == ',') {
			memset(feature, 0x0, sizeof(feature));
			strncpy((char *)feature, begin, p - begin);	
			
			add_app_feature(app_id, app_name, feature);
			begin = p + 1;
		}	
	}
	if (p != begin){
		memset(feature, 0x0, sizeof(feature));
		strncpy((char *)feature, begin, p - begin);	
		add_app_feature(app_id, app_name, feature);
	}
}

void load_feature_buf_from_file(char **config_buf)
{
	struct inode *inode;
	struct file *fp;
	mm_segment_t fs;
	off_t size;
	
	fp = filp_open(AF_FEATURE_CONFIG_FILE, O_RDONLY, 0);
	if(IS_ERR(fp)) {
          printk("open feature file failed\n");
          return -1;
	}
	
	inode = fp->f_dentry->d_inode;
	size = inode->i_size;
	printk("file size: %d\n", size);
	*config_buf = (char *) kzalloc( sizeof(char) * size, GFP_KERNEL);
	if(NULL == *config_buf ) {
		printk("alloc buf fail\n");
		filp_close(fp, NULL);
		return -1;
	}
	fs = get_fs();
	set_fs(KERNEL_DS);
	fp->f_op->read(fp, *config_buf, size, &(fp->f_pos));
	set_fs(fs);
	filp_close(fp, NULL);
	return size;
}

void load_feature_config(void)
{
	printk("begin load feature config.....\n");
	char *feature_buf = NULL;
	load_feature_buf_from_file(&feature_buf);
	if (!feature_buf) {
		printk("error, feature buf is null\n");
		return;
	}
	
	printk("feature_buf = %s\n", feature_buf);
	char *p;
	char *begin;
	p = begin = feature_buf;
	char line[MAX_FEATURE_LINE_LEN] = {0};
	while(*p++) {
		if (*p == '\n'){
			if (p - begin < MIN_FEATURE_LINE_LEN || p - begin > MAX_FEATURE_LINE_LEN ) {
				begin = p + 1;
				continue;
			}
			memset(line, 0x0, sizeof(line));
			strncpy(line, begin, p - begin);
			af_init_feature(line);
			begin = p + 1;
		}
	}
	if (p != begin) {
		if (p - begin < MIN_FEATURE_LINE_LEN || p - begin > MAX_FEATURE_LINE_LEN ) 
			return;
		memset(line, 0x0, sizeof(line));
		strncpy(line, begin, p - begin);
		af_init_feature(line);
		begin = p + 1;
	}
	if (feature_buf)
		kfree(feature_buf);
}

static void af_clean_feature_list()
{
	af_feature_node_t *n,*node;
	feature_list_write_lock();
	while(!list_empty(&af_feature_head)) {
		node = list_first_entry(&af_feature_head, af_feature_node_t, head);
		list_del(&(node->head));
		kfree(node);
	}
	feature_list_write_unlock();
}


int parse_flow_base(struct sk_buff *skb, flow_info_t *flow) 
{
	struct tcphdr * tcph = NULL;
	struct udphdr * udph = NULL;
	struct nf_conn *ct = NULL;
	struct iphdr *iph = NULL;
	if (!skb) {
		return -1;
	}
	ct = (struct nf_conn *)skb->nfct;
	if (!ct) {
		return -1;
	}
	iph = ip_hdr(skb);
	if ( !iph ) {
		return -1;
	}
	flow->ct = ct;
	flow->src = iph->saddr;
	flow->dst = iph->daddr;
	flow->l4_protocol = iph->protocol;
	switch (iph->protocol) {
		case IPPROTO_TCP:
			tcph = (struct tcphdr *)(iph + 1);
			flow->l4_data = skb->data + iph->ihl * 4 + tcph->doff * 4;
			flow->l4_len =  ntohs(iph->tot_len) - iph->ihl * 4 - tcph->doff * 4;
			flow->dport = htons(tcph->dest);
			flow->sport = htons(tcph->source);
			break;
		case IPPROTO_UDP:
			udph = (struct udphdr *)(iph + 1);
			flow->l4_data = skb->data + iph->ihl * 4 + 8;
			flow->l4_len = ntohs(udph->len) - 8;
			flow->dport = htons(udph->dest);
			flow->sport = htons(udph->source);
			break;
		case IPPROTO_ICMP:
			break;
		default:
			return -1;
	}
	return -1;
}


/*
	desc: 解析https url信息，保存到flow中
	return:
		-1: error
		0: match
	author: Derry
	Date:2018/12/19
*/
int parse_https_proto(flow_info_t *flow) {
	int i ;
	short url_len = 0 ;
	char * p = flow->l4_data;
	int data_len = flow->l4_len;
	
	if (NULL == flow) {
		AF_ERROR("flow is NULL\n");
		return -1;
	}
	if (NULL == p || data_len == 0) {
		return -1;
	}
	if (!(p[0] == 0x16 && p[1] == 0x03 && p[2] == 0x01))
		return -1;
	
	for(i = 0; i < data_len; i++) {
		if(i + HTTPS_URL_OFFSET >= data_len) {
			return -1;
		}
		
		if(p[i] == 0x0 && p[i + 1] == 0x0 && p[i + 2] == 0x0 && p[i + 3] != 0x0) {
			// 2 bytes
			memcpy(&url_len , p + i + HTTPS_LEN_OFFSET, 2);
			if(ntohs(url_len) <= 0 || ntohs(url_len) > data_len) {
				continue ;
			}
			
			if(i + HTTPS_URL_OFFSET + ntohs(url_len) < data_len) {
				//dump_hex("https hex", p, data_len);
				flow->https.match = AF_TRUE;
				flow->https.url_pos = p + i + HTTPS_URL_OFFSET;
				//dump_str("https url", flow->https.url_pos, 5);
				flow->https.url_len = ntohs(url_len);
				return 0;              
			}
		}
	}
	return -1;
}


void parse_http_proto(flow_info_t *flow) 
{
	if (!flow) {
		AF_ERROR("flow is null\n");
		return;
	}
	if (flow->l4_protocol != IPPROTO_TCP) {
		return;
	}

	int i = 0;
	int start = 0;
	char *data = flow->l4_data;
	int data_len = flow->l4_len;
	if (data_len < MIN_HTTP_DATA_LEN) {
		return;
	}
	if (flow->sport != 80 && flow->dport != 80)
		return;
	for (i = 0; i < data_len - 4; i++) {
		if (data[i] == 0x0d && data[i + 1] == 0x0a){
			if (0 == memcmp(&data[start], "POST ", 5)) {
				flow->http.match = AF_TRUE;
				flow->http.method = HTTP_METHOD_POST;
				flow->http.url_pos = data + start + 5;
				flow->http.url_len = i - start - 5;
				//dump_str("get request", flow->http.url_pos, flow->http.url_len);
			}
			else if(0 == memcmp(&data[start], "GET ", 4)) {
				flow->http.match = AF_TRUE;
				flow->http.method = HTTP_METHOD_GET;
				flow->http.url_pos = data + start + 4;
				flow->http.url_len = i - start - 4;
				//dump_str("post request", flow->http.url_pos, flow->http.url_len);
			}
			else if (0 == memcmp(&data[start], "Host: ", 6) ){
				flow->http.host_pos = data + start + 6;
				flow->http.host_len = i - start - 6;
				//dump_str("host ", flow->http.host_pos, flow->http.host_len);
			}
			// 判断http头部结束
			if (data[i + 2] == 0x0d && data[i + 3] == 0x0a){
				flow->http.data_pos = data + i + 4;
				flow->http.data_len = data_len - i - 4;
				break;
			}
			// 0x0d 0x0a
			start = i + 2; 
		}
	}
}

static void dump_http_flow_info(http_proto_t *http) {
	if (!http) {
		AF_ERROR("http ptr is NULL\n");
		return ;
	}
	if (!http->match)
		return;	
	if (http->method == HTTP_METHOD_GET){
		printk("Http method: "HTTP_GET_METHOD_STR"\n");
	}
	else if (http->method == HTTP_METHOD_POST) {
		printk("Http method: "HTTP_POST_METHOD_STR"\n");
	}
	if (http->url_len > 0 && http->url_pos){
		dump_str("Request url", http->url_pos, http->url_len);
	}

	if (http->host_len > 0 && http->host_pos){
		dump_str("Host", http->host_pos, http->host_len);
	}

	printk("--------------------------------------------------------\n\n\n");
}

static void dump_https_flow_info(https_proto_t *https) {
	if (!https) {
		AF_ERROR("https ptr is NULL\n");
		return ;
	}
	if (!https->match)
		return;	
	

	if (https->url_len > 0 && https->url_pos){
		printk("url len = %d\n",https->url_len);
		dump_str("https server name", https->url_pos, https->url_len);
	}

	printk("--------------------------------------------------------\n\n\n");
}

static void dump_flow_info(flow_info_t *flow)
{
	if (!flow) {
		AF_ERROR("flow is null\n");
		return;
	}
	#if 0
	if (check_local_network_ip(ntohl(flow->src))) {
		printk("src ip(inner net):"NIPQUAD_FMT", dst ip = "NIPQUAD_FMT"\n", NIPQUAD(flow->src), NIPQUAD(flow->dst));
	}
	else {
		printk("src ip(outer net):"NIPQUAD_FMT", dst ip = "NIPQUAD_FMT"\n", NIPQUAD(flow->src), NIPQUAD(flow->dst));
	}
	#endif
	if (flow->l4_protocol == IPPROTO_TCP) {
		if (AF_TRUE == flow->http.match) {
			printk("-------------------http protocol-------------------------\n");
			printk("protocol:TCP , sport: %-8d, dport: %-8d, data_len: %-8d\n",
					flow->sport, flow->dport, flow->l4_len);
			dump_http_flow_info(&flow->http);
		}
		if (AF_TRUE == flow->https.match) {
			dump_https_flow_info(&flow->https);
		}
	}
	else if (flow->l4_protocol == IPPROTO_UDP) {
		//	printk("protocol:UDP ,sport: %-8d, dport: %-8d, data_len: %-8d\n",
		//					flow->sport, flow->dport, flow->l4_len);
	}
	else {
		return;
	}
}

int app_filter_match(flow_info_t *flow)
{
	af_feature_node_t *n,*node;
	feature_list_read_lock();
	if(!list_empty(&af_feature_head)) { 
		list_for_each_entry_safe(node, n, &af_feature_head, head) {

			if (flow->https.match == AF_TRUE) {
				if (flow->https.url_pos && 
					strnstr(flow->https.url_pos, node->host_url, flow->https.url_len)){
					
					dump_str("Drop https url ",flow->https.url_pos, flow->https.url_len);
					
					feature_list_read_unlock();
					return 1;
				}
			} 
		}
	}
	feature_list_read_unlock();
	return 0;
}


/* 在netfilter框架注册的钩子 */
static u_int32_t app_filter_hook(unsigned int hook,
						    struct sk_buff *pskb,
					           const struct net_device *in,
					           const struct net_device *out,
					           int (*okfn)(struct sk_buff *))
{
	struct nf_conn *ct = (struct nf_conn *)pskb->nfct;
	
	if (ct == NULL) {
		AF_ERROR("ct is null\n");
        return NF_ACCEPT;
    }
	flow_info_t flow;
	memset((char *)&flow, 0x0, sizeof(flow_info_t));
	parse_flow_base(pskb, &flow);
	parse_http_proto(&flow);
	parse_https_proto(&flow);
	dump_flow_info(&flow);
	// todo: match url rules
	// this is example
	#if 0
	if (flow.http.match == AF_TRUE) {
		if (flow.http.host_pos && 
			strnstr(flow.http.host_pos, "sohu", flow.http.host_len)){
			
			dump_str("Drop http url ",flow.http.host_pos, flow.http.host_len);
			return NF_DROP;
		}
	}
	if (flow.https.match == AF_TRUE) {
		if (flow.https.url_pos && 
			strnstr(flow.https.url_pos, "hao123", flow.https.url_len)){
			
			dump_str("Drop https url ",flow.https.url_pos, flow.https.url_len);
			return NF_DROP;
		}
	}
	#endif
	if (app_filter_match(&flow))
		return NF_DROP;
	return NF_ACCEPT;
}


static struct nf_hook_ops app_filter_ops[] __read_mostly = {
	{
		.hook		= app_filter_hook,
		.owner		= THIS_MODULE,
		.pf			= PF_INET,
		.hooknum	= NF_INET_FORWARD,
		.priority	= NF_IP_PRI_MANGLE + 10,
	},
};

#include "cJSON.h"
void TEST_cJSON(void)
{
	cJSON * root = NULL;
	root = cJSON_CreateObject();
	if (!root) {
		AF_ERROR("create obj failed\n");
		return;
	}
	cJSON_AddNumberToObject(root, "id", 123);
	cJSON_AddStringToObject(root, "name", "derry");
	char * out = cJSON_Print(root);
	printk("out = %s\n", out);
	cJSON_Delete(root);
	kfree(out);
}

static struct mutex af_cdev_mutex;

struct af_cdev_file {
    size_t size;
    char buf[256 << 10];
};

static int af_cdev_open(struct inode *inode, struct file *filp)
{
    struct af_cdev_file *file;
	printk("cdev open\n");

    file = vzalloc(sizeof(*file));
    if (!file)
        return -EINVAL;

    mutex_lock(&af_cdev_mutex);
    filp->private_data = file;
    return 0;
}

static ssize_t af_cdev_read(struct file *filp, char *buf, size_t count, loff_t *off)
{
    return 0;
}

static int af_cdev_release(struct inode *inode, struct file *filp)
{
    struct af_cdev_file *file = filp->private_data;
    int ret;
    printk("config size: %d,data = %s\n", (int)file->size, file->buf);
    filp->private_data = NULL;
    mutex_unlock(&af_cdev_mutex);
    vfree(file);
    return 0;
}

static ssize_t af_cdev_write(struct file *filp, const char *buffer, size_t count, loff_t *off)
{
    struct af_cdev_file *file = filp->private_data;
    int ret;
	printk("cdev write\n");
    if (file->size + count > sizeof(file->buf)) {
        printk("config overflow, cur_size: %d, block_size: %d, max_size: %d",
            (int)file->size, (int)count, (int)sizeof(file->buf));
        return -EINVAL;
    }

    ret = copy_from_user(file->buf + file->size, buffer, count);
    if (ret != 0)
        return -EINVAL;

    file->size += count;
    return count;
}

static struct file_operations af_cdev_ops = {
	owner:   THIS_MODULE,
	release: af_cdev_release,
	open:    af_cdev_open,
	write:   af_cdev_write,
	read:    af_cdev_read,
};

void af_register_dev(void)
{
    struct device *dev;
    int res;
    mutex_init(&af_cdev_mutex);

    res = alloc_chrdev_region(&g_af_dev.id, 0, 1, AF_DEV_NAME);
    if (res != 0) {
        return -EINVAL;
    }

    cdev_init(&g_af_dev.char_dev, &af_cdev_ops);
    res = cdev_add(&g_af_dev.char_dev, g_af_dev.id, 1);
    if (res < 0) {
        goto REGION_OUT;
    }

    g_af_dev.c= class_create(THIS_MODULE, AF_DEV_NAME);
    if (IS_ERR_OR_NULL(g_af_dev.c)) {
        goto CDEV_OUT;
    }

    dev = device_create(g_af_dev.c, NULL, g_af_dev.id, NULL, AF_DEV_NAME);
    if (IS_ERR_OR_NULL(dev)) {
        goto CLASS_OUT;
    }
	printk("register char dev....ok\n");

    return 0;

CLASS_OUT:
    class_destroy(g_af_dev.c);
CDEV_OUT:
    cdev_del(&g_af_dev.char_dev);
REGION_OUT:
    unregister_chrdev_region(g_af_dev.id, 1);
	
	printk("register char dev....fail\n");
    return -EINVAL;
}


void af_unregister_dev(void)
{
	device_destroy(g_af_dev.c, g_af_dev.id);
    class_destroy(g_af_dev.c);
    cdev_del(&g_af_dev.char_dev);
    unregister_chrdev_region(g_af_dev.id, 1);
	printk("unregister char dev....ok\n");
}

extern void TEST_regexp();
/*
	模块初始化
*/
static int __init app_filter_init(void)
{
	AF_INFO("appfilter version:"AF_VERSION"\n");
	AF_DEBUG("app filter module init\n");
	//TEST_regexp();
	af_register_dev();
	load_feature_config();
	show_feature_list();
	TEST_cJSON();
	nf_register_hooks(app_filter_ops, ARRAY_SIZE(app_filter_ops));
	printk("init app filter ........ok\n");
	return 0;
}
/*
	模块退出
*/
static void app_filter_fini(void)
{
	AF_DEBUG("app filter module exit\n");
	nf_unregister_hooks(app_filter_ops, ARRAY_SIZE(app_filter_ops));
	af_clean_feature_list();
	af_unregister_dev();
	return ;
}


module_init(app_filter_init);
module_exit(app_filter_fini);

