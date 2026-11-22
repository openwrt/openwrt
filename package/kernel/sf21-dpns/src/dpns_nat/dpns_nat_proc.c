
#include <linux/kernel.h>
#include <linux/inet.h>
#include <linux/proc_fs.h>
#include "dpns_common.h"
#include "dpns_nat_genl.h"
#include "nat.h"
static int parse_ip(void *buf, const char *str, bool is_ipv6)
{
	if (is_ipv6) {
		if (!in6_pton(str, strlen(str), buf, -1, NULL))
			goto invalid_ip;
	} else {
		if (!in4_pton(str, strlen(str), buf, -1, NULL))
			goto invalid_ip;
	}
	return 0;
invalid_ip:
	printk("invalid IP address: %s\n", str);
	return -EINVAL;
}
static void dpns_nat_proc_help(void)
{
	printk("usage: \n");
	printk("\n");
	printk("echo count > proc/dpns_nat\n");
	printk("echo dump_nat > proc/dpns_nat\n");
	printk("echo offload_en [0/1] > proc/dpns_nat\n");
	printk("echo search is_dnat [0/1] is_udp [0/1] is_v6 [0/1] pub_port [int] pri_port [int] \
rt_port [int] pub_ip [string] pri_ip [string] rt_ip [string] > proc/dpns_nat\n");
	printk("echo mode is_lf [0/1] is_udp [0/1] is_v6 [0/1] lf_mode [int 0~7] hnat_mode [int 0~4] > proc/dpns_nat\n");
	printk("echo subnet is_get [0/1] is_lan [0/1] index [int 0~7] ifname [string] > proc/dpns_nat\n");
	printk("echo spl_set is_dnat [0/1] spl_index [int 0~256] spl_value [int] pkt_length [int] mib_mode [int 0~15] \
is_zerolmt [0/1] spl_cntmode [int 0~3] spl_mode [0/1] spl_source [int 0~3] > proc/dpns_nat\n");
}
enum search_nat_blob_id {
	BLOB_NAT_DIR,
	BLOB_NAT_PROTO,
	BLOB_NAT_IS_V6,
	BLOB_NAT_PUB_PORT,
	BLOB_NAT_PRI_PORT,
	BLOB_NAT_RT_PORT,
	BLOB_NAT_PUB_IP,
	BLOB_NAT_PRI_IP,
	BLOB_NAT_RT_IP,
	NUM_SEARCH_NAT_BLOB_IDS,
};
static ssize_t nat_hwsearch_parse(struct dpns_nat_priv *priv, char *str, char *str_end)
{
	struct nat_genl_msg msg = {};
	const char *search_key[NUM_SEARCH_NAT_BLOB_IDS] = {"is_dnat", "is_udp", "is_v6", "pub_port", "pri_port",
							   "rt_port", "pub_ip", "pri_ip", "rt_ip"};
	char *key[NUM_SEARCH_NAT_BLOB_IDS], *value[NUM_SEARCH_NAT_BLOB_IDS], *search_value[NUM_SEARCH_NAT_BLOB_IDS];
	uint32_t ip_buf[4] = {0};
	int i = 0, j =0, ret = 0, num_params = 0, count = 0;

	for (i = 0; i < NUM_SEARCH_NAT_BLOB_IDS; i++) {
		key[i] = NULL;
		value[i] = NULL;
		search_value[i] = NULL;
	}
	while (str < str_end && num_params < NUM_SEARCH_NAT_BLOB_IDS) {
		key[num_params] = strsep(&str, "\t \n");
		if (!key[num_params])
			break;
		value[num_params] = strsep(&str, "\t \n");
		if (!value[num_params])
			return -EINVAL;
		count++;
		num_params++;
	}
	for (i = 0, j = 0; i < NUM_SEARCH_NAT_BLOB_IDS && j < count; i++) {
		if (strcmp(key[j], search_key[i]) == 0) {
			search_value[i] = value[j];
			j++;
		}
	}
	if (!search_value[BLOB_NAT_DIR])
		return -EINVAL;
	ret = kstrtobool(search_value[BLOB_NAT_DIR], &msg.is_dnat);
	if (ret)
		return -EINVAL;
	if (!search_value[BLOB_NAT_PROTO])
		return -EINVAL;
	ret = kstrtobool(search_value[BLOB_NAT_PROTO], &msg.is_udp);
	if (ret)
		return -EINVAL;
	if (!search_value[BLOB_NAT_IS_V6])
		return -EINVAL;
	ret = kstrtobool(search_value[BLOB_NAT_IS_V6], &msg.is_v6);
	if (ret)
		return -EINVAL;
	if (search_value[BLOB_NAT_PUB_PORT])
		ret = kstrtou16(search_value[BLOB_NAT_PUB_PORT], 10, &msg.public_port);
	if (search_value[BLOB_NAT_PRI_PORT])
		ret = kstrtou16(search_value[BLOB_NAT_PRI_PORT], 10, &msg.private_port);
	if (search_value[BLOB_NAT_RT_PORT])
		ret = kstrtou16(search_value[BLOB_NAT_RT_PORT], 10, &msg.router_port);
	if (search_value[BLOB_NAT_PUB_IP]) {
		ret = parse_ip(ip_buf, search_value[BLOB_NAT_PUB_IP], msg.is_v6);
		if (ret)
			return -EINVAL;
		for (i = 0; i < 4; i++) {
			msg.public_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}
	memset(ip_buf, 0, sizeof(ip_buf));
	if (search_value[BLOB_NAT_PRI_IP]) {
		ret = parse_ip(ip_buf, search_value[BLOB_NAT_PRI_IP], msg.is_v6);
		if (ret)
			return -EINVAL;
		for (i = 0; i < 4; i++) {
			msg.private_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}
	memset(ip_buf, 0, sizeof(ip_buf));
	if (search_value[BLOB_NAT_RT_IP]) {
		ret = parse_ip(ip_buf, search_value[BLOB_NAT_RT_IP], msg.is_v6);
		if (ret)
			return -EINVAL;
		for (i = 0; i < 4; i++) {
			msg.router_ip[i] = be32_to_cpu(ip_buf[3 - i]);
		}
	}

	if (msg.is_v6)
		dpns_nat_hw_search6(priv, &msg);
	else
		dpns_nat_hw_search4(priv, &msg);

	return 0;
}

enum mode_set_blob_id {
	BLOB_NAT_IS_LF,
	BLOB_NAT_IS_UDP,
	BLOB_NAT_IS_V6_MODE,
	BLOB_NAT_LF_MODE,
	BLOB_NAT_HNAT_MODE,
	NUM_MODE_SET_BLOB_IDS,
};

static ssize_t nat_modeset_parse(struct dpns_nat_priv *priv, char *str, char *str_end)
{
	struct nat_genl_msg msg = {};
	const char *search_key[NUM_MODE_SET_BLOB_IDS] = {"is_lf", "is_udp", "is_v6", "lf_mode", "hnat_mode"};
	char *key[NUM_MODE_SET_BLOB_IDS], *value[NUM_MODE_SET_BLOB_IDS], *modeset_value[NUM_MODE_SET_BLOB_IDS];
	int i = 0, j = 0, ret = 0, num_params = 0, count = 0;

	for (i = 0; i < NUM_MODE_SET_BLOB_IDS; i++) {
		key[i] = NULL;
		value[i] = NULL;
		modeset_value[i] = NULL;
	}
	while (str < str_end && num_params < NUM_MODE_SET_BLOB_IDS) {
		key[num_params] = strsep(&str, "\t \n");
		if (!key[num_params])
			break;
		value[num_params] = strsep(&str, "\t \n");
		if (!value[num_params])
			return -EINVAL;
		count++;
		num_params++;
	}
	for (i = 0, j = 0; i < NUM_MODE_SET_BLOB_IDS && j < count; i++) {
		if (strcmp(key[j], search_key[i]) == 0) {
			modeset_value[i] = value[j];
			j++;
		}
	}
	if (!modeset_value[BLOB_NAT_IS_LF])
		return -EINVAL;
	ret = kstrtobool(modeset_value[BLOB_NAT_IS_LF], &msg.is_lf);
	if (!modeset_value[BLOB_NAT_IS_V6_MODE])
		return -EINVAL;
	ret = kstrtobool(modeset_value[BLOB_NAT_IS_V6_MODE], &msg.is_v6_mode);
	if (msg.is_lf) {
		if (!modeset_value[BLOB_NAT_LF_MODE])
			return -EINVAL;
		ret = kstrtou16(modeset_value[BLOB_NAT_LF_MODE], 10, &msg.lf_mode);
		if (ret)
			return -EINVAL;
	} else {
		if (!modeset_value[BLOB_NAT_IS_UDP] || !modeset_value[BLOB_NAT_HNAT_MODE])
			return -EINVAL;
		ret = kstrtobool(modeset_value[BLOB_NAT_IS_UDP], &msg.is_udp);
		ret = kstrtou16(modeset_value[BLOB_NAT_HNAT_MODE], 10, &msg.hnat_mode);
	}
	ret = dpns_nat_mode_set(priv, &msg);
	return ret;
}
enum subnet_op_blob_id {
	BLOB_SUBNET_IS_GET,
	BLOB_SUBNET_IS_LAN,
	BLOB_SUBNET_INDEX,
	BLOB_SUBNET_IFNAME,
	NUM_SUBNET_OP_BLOB_IDS,
};
static ssize_t nat_subnet_parse(struct dpns_nat_priv *priv, char *str, char *str_end)
{
	struct nat_genl_msg msg = {};
	const char *search_key[NUM_SUBNET_OP_BLOB_IDS] = {"is_get", "is_lan", "index", "ifname"};
	char *key[NUM_SUBNET_OP_BLOB_IDS], *value[NUM_SUBNET_OP_BLOB_IDS], *subnet_value[NUM_SUBNET_OP_BLOB_IDS];
	int i = 0, j = 0, ret = 0, num_params = 0, count = 0;

	for (i = 0; i < NUM_SUBNET_OP_BLOB_IDS; i++) {
		key[i] = NULL;
		value[i] = NULL;
		subnet_value[i] = NULL;
	}
	while (str < str_end && num_params < NUM_SUBNET_OP_BLOB_IDS) {
		key[num_params] = strsep(&str, "\t \n");
		if (!key[num_params])
			break;
		value[num_params] = strsep(&str, "\t \n");
		if (!value[num_params])
			return -EINVAL;
		count++;
		num_params++;
	}
	for (i = 0, j = 0; i < NUM_SUBNET_OP_BLOB_IDS && j < count; i++) {
		if (strcmp(key[j], search_key[i]) == 0) {
			subnet_value[i] = value[j];
			j++;
		}
	}
	if (subnet_value[BLOB_SUBNET_IS_GET])
		ret = kstrtobool(subnet_value[BLOB_SUBNET_IS_GET], &msg.is_get);
	if (msg.is_get) {
		if (!subnet_value[BLOB_SUBNET_IS_LAN])
			return -EINVAL;
		ret = kstrtobool(subnet_value[BLOB_SUBNET_IS_LAN], &msg.is_lan);
		if (ret)
			return -EINVAL;
	} else {
		if (!subnet_value[BLOB_SUBNET_IS_LAN])
			return -EINVAL;
		ret = kstrtobool(subnet_value[BLOB_SUBNET_IS_LAN], &msg.is_lan);
		if (ret)
			return -EINVAL;
		if (!subnet_value[BLOB_SUBNET_INDEX])
			return -EINVAL;
		ret = kstrtou16(subnet_value[BLOB_SUBNET_INDEX], 10, &msg.index);
		if (ret)
			return -EINVAL;
		if (!subnet_value[BLOB_SUBNET_IFNAME])
			return -EINVAL;
		/* "-" clears the entry */
		if (strcmp(subnet_value[BLOB_SUBNET_IFNAME], "-"))
			strscpy(msg.ifname, subnet_value[BLOB_SUBNET_IFNAME], IFNAMSIZ);
	}
	dpns_nat_subnet_op(priv, &msg);
	return 0;
}
enum spl_set_blob_id {
	BLOB_SPL_IS_DNAT,
	BLOB_SPL_INDEX,
	BLOB_SPL_VALUE,
	BLOB_PKT_LENGTH,
	BLOB_NAT_MIB_MODE,
	BLOB_IS_ZERO_LMT,
	BLOB_SPL_CNT_MODE,
	BLOB_SPL_MODE,
	BLOB_SPL_SOURCE,
	NUM_SPL_SET_BLOB_IDS,
};
static ssize_t nat_splset_parse(struct dpns_nat_priv *priv, char *str, char *str_end)
{
	struct nat_genl_msg msg = {};
	const char *search_key[NUM_SPL_SET_BLOB_IDS] = {"is_dnat", "spl_index", "spl_value", "pkt_length",
							"mib_mode", "is_zerolmt", "spl_cntmode", "spl_mode",
							"spl_source"};
	char *key[NUM_SPL_SET_BLOB_IDS], *value[NUM_SPL_SET_BLOB_IDS], *splset_value[NUM_SPL_SET_BLOB_IDS];
	int i = 0, j = 0, num_params = 0, count = 0, ret;

	for (i = 0; i < NUM_SPL_SET_BLOB_IDS; i++) {
		key[i] = NULL;
		value[i] = NULL;
		splset_value[i] = NULL;
	}
	while (str < str_end && num_params < NUM_SPL_SET_BLOB_IDS) {
		key[num_params] = strsep(&str, "\t \n");
		if (!key[num_params])
			break;
		value[num_params] = strsep(&str, "\t \n");
		if (!value[num_params])
			return -EINVAL;
		count++;
		num_params++;
	}
	for (i = 0, j = 0; i < NUM_SPL_SET_BLOB_IDS && j < count; i++) {
		if (strcmp(key[j], search_key[i]) == 0) {
			splset_value[i] = value[j];
			j++;
		}
	}
	if (!splset_value[BLOB_SPL_IS_DNAT])
		return -EINVAL;
	ret = kstrtobool(splset_value[BLOB_SPL_IS_DNAT], &msg.is_dnat);
	if (!splset_value[BLOB_SPL_INDEX])
		return -EINVAL;
	ret = kstrtou16(splset_value[BLOB_SPL_INDEX], 10, &msg.spl_index);
	if (splset_value[BLOB_PKT_LENGTH])
		ret = kstrtou16(splset_value[BLOB_PKT_LENGTH], 10, &msg.pkt_length);
	if (splset_value[BLOB_NAT_MIB_MODE]) {
		ret = kstrtou8(splset_value[BLOB_NAT_MIB_MODE], 10, &msg.nat_mib_mode);
		if (msg.nat_mib_mode > 15 || msg.nat_mib_mode < 0) {
			printk("nat_mib_mode ranges from 0 to 15.\n");
			return -EINVAL;
		}
	}
	if (splset_value[BLOB_IS_ZERO_LMT])
		ret = kstrtobool(splset_value[BLOB_IS_ZERO_LMT], &msg.is_zero_lmt);
	if (!splset_value[BLOB_SPL_CNT_MODE])
		return -EINVAL;
	ret = kstrtou8(splset_value[BLOB_SPL_CNT_MODE], 10, &msg.spl_cnt_mode);
	if (msg.spl_cnt_mode > 3 || msg.spl_cnt_mode < 0) {
		printk("spl_cnt_mode ranges from 0 to 3.");
			return -EINVAL;
	}
	if (splset_value[BLOB_SPL_MODE])
		ret = kstrtobool(splset_value[BLOB_SPL_MODE], &msg.spl_mode);
	if (!splset_value[BLOB_SPL_SOURCE])
		return -EINVAL;
	ret = kstrtou8(splset_value[BLOB_SPL_SOURCE], 10, &msg.spl_source);
	if ((msg.spl_source > 3) || (msg.spl_source < 0)) {
		printk("spl_source ranges from 0 to 3.\n");
	}
	if (!splset_value[BLOB_SPL_VALUE])
		return -EINVAL;
	ret = kstrtou32(splset_value[BLOB_SPL_VALUE], 10, &msg.spl_value);
	dpns_nat_spl_set(priv, &msg);
	return 0;
}
static ssize_t nat_proc(struct file *filp, const char *buffer, size_t count, loff_t *offp)
{
	struct inode *ino = file_inode(filp);
	struct dpns_nat_priv *priv = pde_data(ino);
	char *str, *cmd, *str_end;
	char tmpbuf[256] = {0};
	if (count >= sizeof(tmpbuf))
		return -EFAULT;
	if (!buffer || copy_from_user(tmpbuf, buffer, count) != 0)
		return 0;
	if (count > 0) {
		str = tmpbuf;
		str_end = str + count;
		cmd = strsep(&str, "\t \n");
		if(!cmd)
			return -EFAULT;
		if (strcmp(cmd, "dump_nat") == 0) {
			dpns_nat_show(priv);
		} else if (strcmp(cmd, "count") == 0) {
			dpns_nat_count(priv);
		} else if (strcmp(cmd, "offload_en") == 0) {
			int offload_en = 0;
			if((sscanf(str, "%d", &offload_en)) != 1)
				return -EINVAL;
			if (offload_en)
				priv->nat_offload_en = true;
			else
				priv->nat_offload_en = false;
		} else if (strcmp(cmd, "search") == 0) {
			nat_hwsearch_parse(priv, str, str_end);
		} else if (strcmp(cmd, "mode") == 0) {
			nat_modeset_parse(priv, str, str_end);
		} else if (strcmp(cmd, "subnet") == 0) {
			nat_subnet_parse(priv, str, str_end);
		} else if (strcmp(cmd, "spl_set") == 0) {
			nat_splset_parse(priv, str, str_end);
		} else if (strcmp(cmd, "help") == 0) {
			dpns_nat_proc_help();
		} else {
			NAT_DBG(ERR_LV, "INVALID CMD\n");
			dpns_nat_proc_help();
			return -EFAULT;
		}
	}
	return count;
}
static const struct proc_ops nat_ctrl = {
	.proc_write = nat_proc,
};
void dpns_nat_proc_init(struct dpns_nat_priv *priv)
{
	proc_create_data("dpns_nat", 0600, NULL, &nat_ctrl, (void*)priv);
}
void dpns_nat_proc_exit(void)
{
	remove_proc_entry("dpns_nat", NULL);
}
