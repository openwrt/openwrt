/**
  * This file contains ioctl functions
  */

#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>

#include <net/iw_handler.h>
#include <net/ieee80211.h>

#include "host.h"
#include "radiotap.h"
#include "decl.h"
#include "defs.h"
#include "dev.h"
#include "wext.h"
#include "cmd.h"
#include "ioctl.h"

#define MAX_SCAN_CELL_SIZE      (IW_EV_ADDR_LEN + \
				IW_ESSID_MAX_SIZE + \
				IW_EV_UINT_LEN + IW_EV_FREQ_LEN + \
				IW_EV_QUAL_LEN + IW_ESSID_MAX_SIZE + \
				IW_EV_PARAM_LEN + 40)	/* 40 for WPAIE */

#define WAIT_FOR_SCAN_RRESULT_MAX_TIME (10 * HZ)

static int lbs_set_region(struct lbs_private * priv, u16 region_code)
{
	int i;
	int ret = 0;

	for (i = 0; i < MRVDRV_MAX_REGION_CODE; i++) {
		// use the region code to search for the index
		if (region_code == lbs_region_code_to_index[i]) {
			priv->regioncode = region_code;
			break;
		}
	}

	// if it's unidentified region code
	if (i >= MRVDRV_MAX_REGION_CODE) {
		lbs_deb_ioctl("region Code not identified\n");
		ret = -1;
		goto done;
	}

	if (lbs_set_regiontable(priv, priv->regioncode, 0)) {
		ret = -EINVAL;
	}

done:
	lbs_deb_leave_args(LBS_DEB_IOCTL, "ret %d", ret);
	return ret;
}

static inline int hex2int(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	return -1;
}

/* Convert a string representation of a MAC address ("xx:xx:xx:xx:xx:xx")
   into binary format (6 bytes).

   This function expects that each byte is represented with 2 characters
   (e.g., 11:2:11:11:11:11 is invalid)

 */
static char *eth_str2addr(char *ethstr, u8 * addr)
{
	int i, val, val2;
	char *pos = ethstr;

	/* get rid of initial blanks */
	while (*pos == ' ' || *pos == '\t')
		++pos;

	for (i = 0; i < 6; i++) {
		val = hex2int(*pos++);
		if (val < 0)
			return NULL;
		val2 = hex2int(*pos++);
		if (val2 < 0)
			return NULL;
		addr[i] = (val * 16 + val2) & 0xff;

		if (i < 5 && *pos++ != ':')
			return NULL;
	}
	return pos;
}

/* this writes xx:xx:xx:xx:xx:xx into ethstr
   (ethstr must have space for 18 chars) */
static int eth_addr2str(u8 * addr, char *ethstr)
{
	int i;
	char *pos = ethstr;

	for (i = 0; i < 6; i++) {
		sprintf(pos, "%02x", addr[i] & 0xff);
		pos += 2;
		if (i < 5)
			*pos++ = ':';
	}
	return 17;
}

/**
 *  @brief          Add an entry to the BT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_add_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char ethaddrs_str[18];
	char *pos;
	u8 ethaddr[ETH_ALEN];
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(ethaddrs_str, wrq->u.data.pointer,
			   sizeof(ethaddrs_str)))
		return -EFAULT;

	if ((pos = eth_str2addr(ethaddrs_str, ethaddr)) == NULL) {
		lbs_pr_info("BT_ADD: Invalid MAC address\n");
		return -EINVAL;
	}

	lbs_deb_ioctl("BT: adding %s\n", ethaddrs_str);
	ret = lbs_prepare_and_send_command(priv, CMD_BT_ACCESS,
				      CMD_ACT_BT_ACCESS_ADD,
				      CMD_OPTION_WAITFORRSP, 0, ethaddr);
	lbs_deb_leave_args(LBS_DEB_IOCTL, "ret %d", ret);
	return ret;
}

/**
 *  @brief          Delete an entry from the BT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_del_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char ethaddrs_str[18];
	u8 ethaddr[ETH_ALEN];
	char *pos;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(ethaddrs_str, wrq->u.data.pointer,
			   sizeof(ethaddrs_str)))
		return -EFAULT;

	if ((pos = eth_str2addr(ethaddrs_str, ethaddr)) == NULL) {
		lbs_pr_info("Invalid MAC address\n");
		return -EINVAL;
	}

	lbs_deb_ioctl("BT: deleting %s\n", ethaddrs_str);

	return (lbs_prepare_and_send_command(priv,
				      CMD_BT_ACCESS,
				      CMD_ACT_BT_ACCESS_DEL,
				      CMD_OPTION_WAITFORRSP, 0, ethaddr));

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Reset all entries from the BT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_reset_ioctl(struct lbs_private * priv)
{
	lbs_deb_enter(LBS_DEB_IOCTL);

	lbs_pr_alert( "BT: resetting\n");

	return (lbs_prepare_and_send_command(priv,
				      CMD_BT_ACCESS,
				      CMD_ACT_BT_ACCESS_RESET,
				      CMD_OPTION_WAITFORRSP, 0, NULL));

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          List an entry from the BT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_list_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	int pos;
	char *addr1;
	struct iwreq *wrq = (struct iwreq *)req;
	/* used to pass id and store the bt entry returned by the FW */
	union {
		u32 id;
		char addr1addr2[2 * ETH_ALEN];
	} param;
	static char outstr[64];
	char *pbuf = outstr;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(outstr, wrq->u.data.pointer, sizeof(outstr))) {
		lbs_deb_ioctl("Copy from user failed\n");
		return -1;
	}
	param.id = simple_strtoul(outstr, NULL, 10);
	pos = sprintf(pbuf, "%d: ", param.id);
	pbuf += pos;

	ret = lbs_prepare_and_send_command(priv, CMD_BT_ACCESS,
				    CMD_ACT_BT_ACCESS_LIST,
				    CMD_OPTION_WAITFORRSP, 0,
				    (char *)&param);

	if (ret == 0) {
		addr1 = param.addr1addr2;

		pos = sprintf(pbuf, "BT includes node ");
		pbuf += pos;
		pos = eth_addr2str(addr1, pbuf);
		pbuf += pos;
	} else {
		sprintf(pbuf, "(null)");
		pbuf += pos;
	}

	wrq->u.data.length = strlen(outstr);
	if (copy_to_user(wrq->u.data.pointer, (char *)outstr,
			 wrq->u.data.length)) {
		lbs_deb_ioctl("BT_LIST: Copy to user failed!\n");
		return -EFAULT;
	}

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0 ;
}

/**
 *  @brief          Sets inverted state of blacklist (non-zero if inverted)
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_set_invert_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	int ret;
	struct iwreq *wrq = (struct iwreq *)req;
	union {
		u32 id;
		char addr1addr2[2 * ETH_ALEN];
	} param;

	lbs_deb_enter(LBS_DEB_IOCTL);

	param.id = SUBCMD_DATA(wrq) ;
	ret = lbs_prepare_and_send_command(priv, CMD_BT_ACCESS,
				    CMD_ACT_BT_ACCESS_SET_INVERT,
				    CMD_OPTION_WAITFORRSP, 0,
				    (char *)&param);
	if (ret != 0)
		return -EFAULT;
	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Gets inverted state of blacklist (non-zero if inverted)
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_bt_get_invert_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	int ret;
	union {
		__le32 id;
		char addr1addr2[2 * ETH_ALEN];
	} param;

	lbs_deb_enter(LBS_DEB_IOCTL);

	ret = lbs_prepare_and_send_command(priv, CMD_BT_ACCESS,
				    CMD_ACT_BT_ACCESS_GET_INVERT,
				    CMD_OPTION_WAITFORRSP, 0,
				    (char *)&param);

	if (ret == 0)
		wrq->u.param.value = le32_to_cpu(param.id);
	else
		return -EFAULT;

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Find the next parameter in an input string
 *  @param ptr      A pointer to the input parameter string
 *  @return         A pointer to the next parameter, or 0 if no parameters left.
 */
static char * next_param(char * ptr)
{
	if (!ptr) return NULL;
	while (*ptr == ' ' || *ptr == '\t') ++ptr;
	return (*ptr == '\0') ? NULL : ptr;
}

/**
 *  @brief          Add an entry to the FWT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_add_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[128];
	static struct cmd_ds_fwt_access fwt_access;
	char *ptr;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str)))
		return -EFAULT;

	if ((ptr = eth_str2addr(in_str, fwt_access.da)) == NULL) {
		lbs_pr_alert( "FWT_ADD: Invalid MAC address 1\n");
		return -EINVAL;
	}

	if ((ptr = eth_str2addr(ptr, fwt_access.ra)) == NULL) {
		lbs_pr_alert( "FWT_ADD: Invalid MAC address 2\n");
		return -EINVAL;
	}

	if ((ptr = next_param(ptr)))
		fwt_access.metric =
			cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
	else
		fwt_access.metric = cpu_to_le32(FWT_DEFAULT_METRIC);

	if ((ptr = next_param(ptr)))
		fwt_access.dir = (u8)simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.dir = FWT_DEFAULT_DIR;

	if ((ptr = next_param(ptr)))
		fwt_access.rate = (u8) simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.rate = FWT_DEFAULT_RATE;

	if ((ptr = next_param(ptr)))
		fwt_access.ssn =
			cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
	else
		fwt_access.ssn = cpu_to_le32(FWT_DEFAULT_SSN);

	if ((ptr = next_param(ptr)))
		fwt_access.dsn =
			cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
	else
		fwt_access.dsn = cpu_to_le32(FWT_DEFAULT_DSN);

	if ((ptr = next_param(ptr)))
		fwt_access.hopcount = simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.hopcount = FWT_DEFAULT_HOPCOUNT;

	if ((ptr = next_param(ptr)))
		fwt_access.ttl = simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.ttl = FWT_DEFAULT_TTL;

	if ((ptr = next_param(ptr)))
		fwt_access.expiration =
			cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
	else
		fwt_access.expiration = cpu_to_le32(FWT_DEFAULT_EXPIRATION);

	if ((ptr = next_param(ptr)))
		fwt_access.sleepmode = (u8)simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.sleepmode = FWT_DEFAULT_SLEEPMODE;

	if ((ptr = next_param(ptr)))
		fwt_access.snr =
			cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
	else
		fwt_access.snr = cpu_to_le32(FWT_DEFAULT_SNR);

#ifdef DEBUG
	{
		char ethaddr1_str[18], ethaddr2_str[18];
		eth_addr2str(fwt_access.da, ethaddr1_str);
		eth_addr2str(fwt_access.ra, ethaddr2_str);
		lbs_deb_ioctl("FWT_ADD: adding (da:%s,%i,ra:%s)\n", ethaddr1_str,
		       fwt_access.dir, ethaddr2_str);
		lbs_deb_ioctl("FWT_ADD: ssn:%u dsn:%u met:%u hop:%u ttl:%u exp:%u slp:%u snr:%u\n",
		       fwt_access.ssn, fwt_access.dsn, fwt_access.metric,
		       fwt_access.hopcount, fwt_access.ttl, fwt_access.expiration,
		       fwt_access.sleepmode, fwt_access.snr);
	}
#endif

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
						CMD_ACT_FWT_ACCESS_ADD,
						CMD_OPTION_WAITFORRSP, 0,
						(void *)&fwt_access);

	lbs_deb_leave_args(LBS_DEB_IOCTL, "ret %d", ret);
	return ret;
}

/**
 *  @brief          Delete an entry from the FWT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_del_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[64];
	static struct cmd_ds_fwt_access fwt_access;
	char *ptr;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str)))
		return -EFAULT;

	if ((ptr = eth_str2addr(in_str, fwt_access.da)) == NULL) {
		lbs_pr_alert( "FWT_DEL: Invalid MAC address 1\n");
		return -EINVAL;
	}

	if ((ptr = eth_str2addr(ptr, fwt_access.ra)) == NULL) {
		lbs_pr_alert( "FWT_DEL: Invalid MAC address 2\n");
		return -EINVAL;
	}

	if ((ptr = next_param(ptr)))
		fwt_access.dir = (u8)simple_strtoul(ptr, &ptr, 10);
	else
		fwt_access.dir = FWT_DEFAULT_DIR;

#ifdef DEBUG
	{
		char ethaddr1_str[18], ethaddr2_str[18];
		lbs_deb_ioctl("FWT_DEL: line is %s\n", in_str);
		eth_addr2str(fwt_access.da, ethaddr1_str);
		eth_addr2str(fwt_access.ra, ethaddr2_str);
		lbs_deb_ioctl("FWT_DEL: removing (da:%s,ra:%s,dir:%d)\n", ethaddr1_str,
		       ethaddr2_str, fwt_access.dir);
	}
#endif

	ret = lbs_prepare_and_send_command(priv,
						CMD_FWT_ACCESS,
						CMD_ACT_FWT_ACCESS_DEL,
						CMD_OPTION_WAITFORRSP, 0,
						(void *)&fwt_access);
	lbs_deb_leave_args(LBS_DEB_IOCTL, "ret %d", ret);
	return ret;
}


/**
 *  @brief             Print route parameters
 *  @param fwt_access  struct cmd_ds_fwt_access with route info
 *  @param buf         destination buffer for route info
 */
static void print_route(struct cmd_ds_fwt_access fwt_access, char *buf)
{
	buf += sprintf(buf, " ");
	buf += eth_addr2str(fwt_access.da, buf);
	buf += sprintf(buf, " ");
	buf += eth_addr2str(fwt_access.ra, buf);
	buf += sprintf(buf, " %u", fwt_access.valid);
	buf += sprintf(buf, " %u", le32_to_cpu(fwt_access.metric));
	buf += sprintf(buf, " %u", fwt_access.dir);
	buf += sprintf(buf, " %u", fwt_access.rate);
	buf += sprintf(buf, " %u", le32_to_cpu(fwt_access.ssn));
	buf += sprintf(buf, " %u", le32_to_cpu(fwt_access.dsn));
	buf += sprintf(buf, " %u", fwt_access.hopcount);
	buf += sprintf(buf, " %u", fwt_access.ttl);
	buf += sprintf(buf, " %u", le32_to_cpu(fwt_access.expiration));
	buf += sprintf(buf, " %u", fwt_access.sleepmode);
	buf += sprintf(buf, " %u ", le32_to_cpu(fwt_access.snr));
	buf += eth_addr2str(fwt_access.prec, buf);
}

/**
 *  @brief          Lookup an entry in the FWT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_lookup_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[64];
	char *ptr;
	static struct cmd_ds_fwt_access fwt_access;
	static char out_str[128];
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str)))
		return -EFAULT;

	if ((ptr = eth_str2addr(in_str, fwt_access.da)) == NULL) {
		lbs_pr_alert( "FWT_LOOKUP: Invalid MAC address\n");
		return -EINVAL;
	}

#ifdef DEBUG
	{
		char ethaddr1_str[18];
		lbs_deb_ioctl("FWT_LOOKUP: line is %s\n", in_str);
		eth_addr2str(fwt_access.da, ethaddr1_str);
		lbs_deb_ioctl("FWT_LOOKUP: looking for (da:%s)\n", ethaddr1_str);
	}
#endif

	ret = lbs_prepare_and_send_command(priv,
						CMD_FWT_ACCESS,
						CMD_ACT_FWT_ACCESS_LOOKUP,
						CMD_OPTION_WAITFORRSP, 0,
						(void *)&fwt_access);

	if (ret == 0)
		print_route(fwt_access, out_str);
	else
		sprintf(out_str, "(null)");

	wrq->u.data.length = strlen(out_str);
	if (copy_to_user(wrq->u.data.pointer, (char *)out_str,
			 wrq->u.data.length)) {
		lbs_deb_ioctl("FWT_LOOKUP: Copy to user failed!\n");
		return -EFAULT;
	}

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Reset all entries from the FWT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_reset_ioctl(struct lbs_private * priv)
{
	lbs_deb_ioctl("FWT: resetting\n");

	return (lbs_prepare_and_send_command(priv,
				      CMD_FWT_ACCESS,
				      CMD_ACT_FWT_ACCESS_RESET,
				      CMD_OPTION_WAITFORRSP, 0, NULL));
}

/**
 *  @brief          List an entry from the FWT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_list_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[8];
	static struct cmd_ds_fwt_access fwt_access;
	char *ptr = in_str;
	static char out_str[128];
	char *pbuf = out_str;
	int ret = 0;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str))) {
		ret = -EFAULT;
		goto out;
	}

	fwt_access.id = cpu_to_le32(simple_strtoul(ptr, &ptr, 10));

#ifdef DEBUG
	{
		lbs_deb_ioctl("FWT_LIST: line is %s\n", in_str);
		lbs_deb_ioctl("FWT_LIST: listing id:%i\n", le32_to_cpu(fwt_access.id));
	}
#endif

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
				    CMD_ACT_FWT_ACCESS_LIST,
				    CMD_OPTION_WAITFORRSP, 0, (void *)&fwt_access);

	if (ret == 0)
		print_route(fwt_access, pbuf);
	else
		pbuf += sprintf(pbuf, " (null)");

	wrq->u.data.length = strlen(out_str);
	if (copy_to_user(wrq->u.data.pointer, (char *)out_str,
			 wrq->u.data.length)) {
		lbs_deb_ioctl("FWT_LIST: Copy to user failed!\n");
		ret = -EFAULT;
		goto out;
	}

	ret = 0;

out:
	lbs_deb_leave(LBS_DEB_IOCTL);
	return ret;
}

/**
 *  @brief          List an entry from the FRT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_list_route_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[64];
	static struct cmd_ds_fwt_access fwt_access;
	char *ptr = in_str;
	static char out_str[128];
	char *pbuf = out_str;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str)))
		return -EFAULT;

	fwt_access.id = cpu_to_le32(simple_strtoul(ptr, &ptr, 10));

#ifdef DEBUG
	{
		lbs_deb_ioctl("FWT_LIST_ROUTE: line is %s\n", in_str);
		lbs_deb_ioctl("FWT_LIST_ROUTE: listing id:%i\n", le32_to_cpu(fwt_access.id));
	}
#endif

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
				    CMD_ACT_FWT_ACCESS_LIST_ROUTE,
				    CMD_OPTION_WAITFORRSP, 0, (void *)&fwt_access);

	if (ret == 0) {
		print_route(fwt_access, pbuf);
	} else
		pbuf += sprintf(pbuf, " (null)");

	wrq->u.data.length = strlen(out_str);
	if (copy_to_user(wrq->u.data.pointer, (char *)out_str,
			 wrq->u.data.length)) {
		lbs_deb_ioctl("FWT_LIST_ROUTE: Copy to user failed!\n");
		return -EFAULT;
	}

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          List an entry from the FNT table
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_list_neighbor_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	char in_str[8];
	static struct cmd_ds_fwt_access fwt_access;
	char *ptr = in_str;
	static char out_str[128];
	char *pbuf = out_str;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	if (copy_from_user(in_str, wrq->u.data.pointer, sizeof(in_str)))
		return -EFAULT;

	memset(&fwt_access, 0, sizeof(fwt_access));
	fwt_access.id = cpu_to_le32(simple_strtoul(ptr, &ptr, 10));

#ifdef DEBUG
	{
		lbs_deb_ioctl("FWT_LIST_NEIGHBOR: line is %s\n", in_str);
		lbs_deb_ioctl("FWT_LIST_NEIGHBOR: listing id:%i\n", le32_to_cpu(fwt_access.id));
	}
#endif

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
				    CMD_ACT_FWT_ACCESS_LIST_NEIGHBOR,
				    CMD_OPTION_WAITFORRSP, 0,
				    (void *)&fwt_access);

	if (ret == 0) {
		pbuf += sprintf(pbuf, " ra ");
		pbuf += eth_addr2str(fwt_access.ra, pbuf);
		pbuf += sprintf(pbuf, "  slp %u", fwt_access.sleepmode);
		pbuf += sprintf(pbuf, "  snr %u", le32_to_cpu(fwt_access.snr));
		pbuf += sprintf(pbuf, "  ref %u", le32_to_cpu(fwt_access.references));
	} else
		pbuf += sprintf(pbuf, " (null)");

	wrq->u.data.length = strlen(out_str);
	if (copy_to_user(wrq->u.data.pointer, (char *)out_str,
			 wrq->u.data.length)) {
		lbs_deb_ioctl("FWT_LIST_NEIGHBOR: Copy to user failed!\n");
		return -EFAULT;
	}

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Cleans up the route (FRT) and neighbor (FNT) tables
 *                  (Garbage Collection)
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_cleanup_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	static struct cmd_ds_fwt_access fwt_access;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	lbs_deb_ioctl("FWT: cleaning up\n");

	memset(&fwt_access, 0, sizeof(fwt_access));

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
				    CMD_ACT_FWT_ACCESS_CLEANUP,
				    CMD_OPTION_WAITFORRSP, 0,
				    (void *)&fwt_access);

	if (ret == 0)
		wrq->u.param.value = le32_to_cpu(fwt_access.references);
	else
		return -EFAULT;

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}

/**
 *  @brief          Gets firmware internal time (debug purposes)
 *  @param priv     A pointer to struct lbs_private structure
 *  @param req      A pointer to ifreq structure
 *  @return         0 --success, otherwise fail
 */
static int lbs_fwt_time_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	static struct cmd_ds_fwt_access fwt_access;
	int ret;

	lbs_deb_enter(LBS_DEB_IOCTL);

	lbs_deb_ioctl("FWT: getting time\n");

	memset(&fwt_access, 0, sizeof(fwt_access));

	ret = lbs_prepare_and_send_command(priv, CMD_FWT_ACCESS,
				    CMD_ACT_FWT_ACCESS_TIME,
				    CMD_OPTION_WAITFORRSP, 0,
				    (void *)&fwt_access);

	if (ret == 0)
		wrq->u.param.value = le32_to_cpu(fwt_access.references);
	else
		return -EFAULT;

	lbs_deb_leave(LBS_DEB_IOCTL);
	return 0;
}


/**
 *  @brief              Manages all mesh related ioctls
 *  @param priv         A pointer to struct lbs_private structure
 *  @param req          A pointer to ifreq structure
 *  @param cmd		The command type
 *  @param host_subcmd  The device code for the subcommand
 *                          0: sets a value in the firmware
 *                          1: retrieves an int from the firmware
 *  @return             0 --success, otherwise fail
 */
static int lbs_mesh_ioctl(struct lbs_private * priv, struct iwreq * wrq, 
		int cmd, int subcmd)
{
	struct cmd_ds_mesh_access mesh_access;
	int parameter;
	char str[128];
	char *ptr = str;
	int ret, i;

	lbs_deb_enter(LBS_DEB_IOCTL);

	memset(&mesh_access, 0, sizeof(mesh_access));

	if (cmd == LBS_SETONEINT_GETNONE) {
		parameter = SUBCMD_DATA(wrq);

		/* Convert rate from Mbps -> firmware rate index */
		if (subcmd == CMD_ACT_MESH_SET_BCAST_RATE)
			parameter = lbs_data_rate_to_fw_index(parameter);

		if (parameter < 0)
			return -EINVAL;
		mesh_access.data[0] = cpu_to_le32(parameter);
	} else if (subcmd == CMD_ACT_MESH_SET_LINK_COSTS) {
		if (copy_from_user(str, wrq->u.data.pointer, sizeof(str)))
			return -EFAULT;

		for (i = 0; i < COSTS_LIST_SIZE; i++) {
			mesh_access.data[i] = cpu_to_le32(simple_strtoul(ptr, &ptr, 10));
			if (!(ptr = next_param(ptr)) && i!= (COSTS_LIST_SIZE - 1))
				return -EINVAL;
		}
	}

	ret = lbs_mesh_access(priv, subcmd, &mesh_access);

	if (ret != 0)
		return ret;

	if (cmd == LBS_SETNONE_GETONEINT) {
		u32 data = le32_to_cpu(mesh_access.data[0]);

		if (subcmd == CMD_ACT_MESH_GET_BCAST_RATE)
			wrq->u.param.value = lbs_fw_index_to_data_rate(data);
		else
			wrq->u.param.value = data;
	} else if (subcmd == CMD_ACT_MESH_GET_LINK_COSTS) {
		for (i = 0; i < COSTS_LIST_SIZE; i++)
			ptr += sprintf (ptr, " %u", le32_to_cpu(mesh_access.data[i]));
		wrq->u.data.length = strlen(str);

		if (copy_to_user(wrq->u.data.pointer, (char *)str,
				 wrq->u.data.length)) {
			lbs_deb_ioctl("MESH_IOCTL: Copy to user failed!\n");
			ret = -EFAULT;
		}
	}

	lbs_deb_leave(LBS_DEB_IOCTL);
	return ret;
}

/**
 *  @brief Control Beacon transmissions
 *  @param priv                 A pointer to struct lbs_private structure
 *  @param wrq			A pointer to iwreq structure
 *  @return 	   		0 --success, otherwise fail
 */
static int lbs_bcn_ioctl(struct lbs_private * priv, struct iwreq *wrq)
{
	int ret;
	int data[2];

	memset(data, 0, sizeof(data));
	if (!wrq->u.data.length) {
		lbs_deb_ioctl("Get Beacon control\n");
		ret = lbs_prepare_and_send_command(priv,
					    CMD_802_11_BEACON_CTRL,
					    CMD_ACT_GET,
					    CMD_OPTION_WAITFORRSP, 0, NULL);
		data[0] = priv->beacon_enable;
		data[1] = priv->beacon_period;
		if (copy_to_user(wrq->u.data.pointer, data, sizeof(int) * 2)) {
			lbs_deb_ioctl("Copy to user failed\n");
			return -EFAULT;
		}
#define GET_TWO_INT	2
		wrq->u.data.length = GET_TWO_INT;
	} else {
		lbs_deb_ioctl("Set beacon control\n");
		if (wrq->u.data.length > 2)
			return -EINVAL;
		if (copy_from_user (data, wrq->u.data.pointer,
		     sizeof(int) * wrq->u.data.length)) {
			lbs_deb_ioctl("Copy from user failed\n");
			return -EFAULT;
		}
		priv->beacon_enable = data[0];
		if (wrq->u.data.length > 1) {
		if ((data[1] > MRVDRV_MAX_BEACON_INTERVAL)
		    || (data[1] < MRVDRV_MIN_BEACON_INTERVAL))
			return -ENOTSUPP;
		priv->beacon_period= data[1];
		}
		ret = lbs_prepare_and_send_command(priv,
					    CMD_802_11_BEACON_CTRL,
					    CMD_ACT_SET,
					    CMD_OPTION_WAITFORRSP, 0, NULL);
	}
	return ret;
}

static int lbs_led_gpio_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	int i, ret = 0;
	int data[16];
	struct cmd_ds_802_11_led_ctrl ctrl;
	struct mrvlietypes_ledgpio *gpio = (struct mrvlietypes_ledgpio *) ctrl.data;
	int len = wrq->u.data.length;

	if ((len > MAX_LEDS * 2) || (len % 2 != 0))
		return -ENOTSUPP;

	memset(&ctrl, 0, sizeof(ctrl));
	if (len == 0) {
		ctrl.action = cpu_to_le16(CMD_ACT_GET);
	} else {
		if (copy_from_user(data, wrq->u.data.pointer, sizeof(int) * len)) {
			lbs_deb_ioctl("Copy from user failed\n");
			ret = -EFAULT;
			goto out;
		}

		ctrl.action = cpu_to_le16(CMD_ACT_SET);
		ctrl.numled = cpu_to_le16(0);
		gpio->header.type = cpu_to_le16(TLV_TYPE_LED_GPIO);
		gpio->header.len = cpu_to_le16(len);
		for (i = 0; i < len; i += 2) {
			gpio->ledpin[i / 2].led = data[i];
			gpio->ledpin[i / 2].pin = data[i + 1];
		}
	}

	ret = lbs_prepare_and_send_command(priv, CMD_802_11_LED_GPIO_CTRL,
			0, CMD_OPTION_WAITFORRSP, 0, (void *)&ctrl);
	if (ret) {
		lbs_deb_ioctl("Error doing LED GPIO control: %d\n", ret);
		goto out;
	}
	len = le16_to_cpu(gpio->header.len);
	for (i = 0; i < len; i += 2) {
		data[i] = gpio->ledpin[i / 2].led;
		data[i + 1] = gpio->ledpin[i / 2].pin;
	}

	if (copy_to_user(wrq->u.data.pointer, data, sizeof(int) * len)) {
		lbs_deb_ioctl("Copy to user failed\n");
		ret = -EFAULT;
		goto out;
	}

	wrq->u.data.length = len;

out:
	return ret;
}


static int lbs_led_bhv_ioctl(struct lbs_private * priv, struct ifreq *req)
{
	struct iwreq *wrq = (struct iwreq *)req;
	int i, ret = 0;
	int data[MAX_LEDS*4];
	int firmwarestate = 0;
	struct cmd_ds_802_11_led_ctrl ctrl;
	struct mrvlietypes_ledbhv *bhv = (struct mrvlietypes_ledbhv *) ctrl.data;
	int len = wrq->u.data.length;

	if ((len > MAX_LEDS * 4) ||(len == 0)  )
		return -ENOTSUPP;

	memset(&ctrl, 0, sizeof(ctrl));
	if (copy_from_user(data, wrq->u.data.pointer, sizeof(int) * len)) {
			lbs_deb_ioctl("Copy from user failed\n");
			ret = -EFAULT;
			goto out;
	}
	if (len == 1) {
		ctrl.action = cpu_to_le16(CMD_ACT_GET);
		firmwarestate = data[0];
	} else {
		
		if (len % 4 != 0 )
			return -ENOTSUPP;

		bhv->header.type = cpu_to_le16(TLV_TYPE_LEDBEHAVIOR);
		bhv->header.len = cpu_to_le16(len);
		ctrl.action = cpu_to_le16(CMD_ACT_SET);
		ctrl.numled = cpu_to_le16(0);
		for (i = 0; i < len; i += 4) {
			bhv->ledbhv[i / 4].firmwarestate = data[i];
			bhv->ledbhv[i / 4].led = data[i + 1];
			bhv->ledbhv[i / 4].ledstate = data[i + 2];
			bhv->ledbhv[i / 4].ledarg = data[i + 3];
		}
	}

	ret = lbs_prepare_and_send_command(priv, CMD_802_11_LED_GPIO_CTRL,
			0, CMD_OPTION_WAITFORRSP, 0, (void *)&ctrl);
	if (ret) {
		lbs_deb_ioctl("Error doing LED GPIO control: %d\n", ret);
		goto out;
	}

	/* Get LED behavior IE, we have received gpio control as well when len 
          is equal to 1. */
	if (len ==1 ) {
		bhv = (struct mrvlietypes_ledbhv *) 
			((unsigned char *)bhv->ledbhv + le16_to_cpu(bhv->header.len));
		i = 0;
		while ( i < (MAX_LEDS*4) &&
			(bhv->header.type != cpu_to_le16(MRVL_TERMINATE_TLV_ID)) ) {
			if (bhv->ledbhv[0].firmwarestate == firmwarestate) {
				data[i++] = bhv->ledbhv[0].firmwarestate;
				data[i++] = bhv->ledbhv[0].led;
				data[i++] = bhv->ledbhv[0].ledstate;
				data[i++] = bhv->ledbhv[0].ledarg;
			}
			bhv++;
		}
		len = i;
	} else {
		for (i = 0; i < le16_to_cpu(bhv->header.len); i += 4) {
			data[i] = bhv->ledbhv[i / 4].firmwarestate;
			data[i + 1] = bhv->ledbhv[i / 4].led;
			data[i + 2] = bhv->ledbhv[i / 4].ledstate;
			data[i + 3] = bhv->ledbhv[i / 4].ledarg;
		}
		len = le16_to_cpu(bhv->header.len);
	}

	if (copy_to_user(wrq->u.data.pointer, data,
			 sizeof(int) * len)) {
		lbs_deb_ioctl("Copy to user failed\n");
		ret = -EFAULT;
		goto out;
	}

	wrq->u.data.length = len;

out:
	return ret;
}

/**
 *  @brief ioctl function - entry point
 *
 *  @param dev		A pointer to net_device structure
 *  @param req	   	A pointer to ifreq structure
 *  @param cmd 		command
 *  @return 	   	0--success, otherwise fail
 */
int lbs_do_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	int *pdata;
	int ret = 0;
	struct lbs_private *priv = dev->priv;
	struct iwreq *wrq = (struct iwreq *)req;

	lbs_deb_enter(LBS_DEB_IOCTL);

	lbs_deb_ioctl("lbs_do_ioctl: ioctl cmd = 0x%x\n", cmd);
	switch (cmd) {
	case LBS_SETNONE_GETNONE:
		switch (wrq->u.data.flags) {
		case LBS_SUBCMD_BT_RESET:
			lbs_bt_reset_ioctl(priv);
			break;
		case LBS_SUBCMD_FWT_RESET:
			lbs_fwt_reset_ioctl(priv);
			break;
		}
		break;

	case LBS_SETONEINT_GETNONE:
		switch (wrq->u.mode) {
		case LBS_SUBCMD_SET_REGION:
			ret = lbs_set_region(priv, (u16) SUBCMD_DATA(wrq));
			break;
		case LBS_SUBCMD_MESH_SET_TTL:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_SET_TTL);
			break;
		case LBS_SUBCMD_MESH_SET_BCAST_RATE:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_SET_BCAST_RATE);
			break;
		case LBS_SUBCMD_MESH_SET_RREQ_DELAY:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_SET_RREQ_DELAY);
			break;
		case LBS_SUBCMD_MESH_SET_ROUTE_EXP:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_SET_ROUTE_EXP);
			break;
		case LBS_SUBCMD_BT_SET_INVERT:
			ret = lbs_bt_set_invert_ioctl(priv, req);
			break;
		default:
			ret = -EOPNOTSUPP;
			break;
		}
		break;

	case LBS_SET128CHAR_GET128CHAR:
		switch ((int)wrq->u.data.flags) {
		case LBS_SUBCMD_BT_ADD:
			ret = lbs_bt_add_ioctl(priv, req);
			break;
		case LBS_SUBCMD_BT_DEL:
			ret = lbs_bt_del_ioctl(priv, req);
			break;
		case LBS_SUBCMD_BT_LIST:
			ret = lbs_bt_list_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_ADD:
			ret = lbs_fwt_add_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_DEL:
			ret = lbs_fwt_del_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_LOOKUP:
			ret = lbs_fwt_lookup_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_LIST_NEIGHBOR:
			ret = lbs_fwt_list_neighbor_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_LIST:
			ret = lbs_fwt_list_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_LIST_ROUTE:
			ret = lbs_fwt_list_route_ioctl(priv, req);
			break;
		case LBS_SUBCMD_MESH_SET_LINK_COSTS:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_SET_LINK_COSTS);
			break ;
		case LBS_SUBCMD_MESH_GET_LINK_COSTS:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_GET_LINK_COSTS);
			break;
		}
		break;

	case LBS_SETNONE_GETONEINT:
		switch (wrq->u.mode) {
		case LBS_SUBCMD_GET_REGION:
			pdata = (int *)wrq->u.name;
			*pdata = (int)priv->regioncode;
			break;
		case LBS_SUBCMD_FWT_CLEANUP:
			ret = lbs_fwt_cleanup_ioctl(priv, req);
			break;
		case LBS_SUBCMD_FWT_TIME:
			ret = lbs_fwt_time_ioctl(priv, req);
			break;
		case LBS_SUBCMD_MESH_GET_TTL:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_GET_TTL);
			break;
		case LBS_SUBCMD_MESH_GET_BCAST_RATE:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_GET_BCAST_RATE);
			break;
		case LBS_SUBCMD_MESH_GET_RREQ_DELAY:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_GET_RREQ_DELAY);
			break;
		case LBS_SUBCMD_MESH_GET_ROUTE_EXP:
			ret = lbs_mesh_ioctl(priv, wrq, cmd,
					CMD_ACT_MESH_GET_ROUTE_EXP);
			break;
		case LBS_SUBCMD_BT_GET_INVERT:
			ret = lbs_bt_get_invert_ioctl(priv, req);
			break;
		default:
			ret = -EOPNOTSUPP;
		}
		break;

	case LBS_SET_GET_SIXTEEN_INT:
		switch ((int)wrq->u.data.flags) {
		case LBS_LED_GPIO_CTRL:
			ret = lbs_led_gpio_ioctl(priv, req);
			break;
		case LBS_BCN_CTRL:
			ret = lbs_bcn_ioctl(priv,wrq);
			break;
		case LBS_LED_BEHAVIOR_CTRL:
			ret = lbs_led_bhv_ioctl(priv, req);
			break;
		}
		break;

	default:
		ret = -EINVAL;
		break;
	}

	lbs_deb_leave_args(LBS_DEB_IOCTL, "ret %d", ret);
	return ret;
}
