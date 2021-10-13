/*
 ***************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************************************************
 */

/*
 * nss_match_log.c
 *	NSS match logger file.
 */

#include "nss_core.h"

/*
 * nss_match_log_message_types_str
 *	Match message strings.
 */
static char *nss_match_log_message_types_str[NSS_MATCH_MSG_MAX] __maybe_unused = {
	"Match no message",
	"Match profile configure",
	"Match add VoW rule",
	"Match add L2 rule",
	"Match delete VoW rule",
	"Match delete L2 rule",
	"Match sync stats"
};

/*
 * nss_match_log_error_types_str
 *	Strings for error types for match messages
 */
static char *nss_match_log_error_types_str[NSS_MATCH_ERROR_MAX] __maybe_unused = {
	"Match success",
	"Match unknown message",
	"Match DSCP is not in the range",
	"Match 802.1p outer is not in the range",
	"Match 802.1p inner is not in the range",
	"Match rule ID is not in the range",
	"Match action type is not in the range",
	"Match rule ID already exists",
	"Match rule ID doesn't exists",
	"Match instance already configured",
	"Match profile configuration message is invalid",
	"Match database initialzation failed",
	"Match table ID is not in the range",
	"Match error in adding rule",
	"Match error in deleting rule",
	"Match error in adding table",
	"Match error in deleting table",
	"Match error mask ID is not in the range",
	"Match error next node interface number is invalid",
};

/*
 * nss_match_log_profile_configure_msg()
 * 	Log NSS match profile configuration message.
 */
static void nss_match_log_profile_configure_msg(struct nss_match_msg *nmm)
{
	struct nss_match_profile_configure_msg *nmcm __maybe_unused = &nmm->msg.configure_msg;
	int mask_num, mask_word;

	nss_trace("%px: NSS match configuration message \n"
			"Match profile type: %u \n"
			"Match mask flag: %u \n",
			nmcm,
			nmcm->profile_type,
			nmcm->valid_mask_flag);

	for (mask_num = 0; mask_num < NSS_MATCH_MASK_MAX; mask_num++) {
		nss_trace("Match mask number %d\n", mask_num + 1);
		for (mask_word = 0; mask_word < NSS_MATCH_MASK_WORDS_MAX; mask_word++) {
			nss_trace("%x ", nmcm->maskset[mask_num][mask_word]);
		}
	}
}

/*
 * nss_match_log_vow_rule_msg()
 * 	Log NSS match VoW rule message.
 */
static void nss_match_log_vow_rule_msg(struct nss_match_msg *nmm)
{
	struct nss_match_rule_vow_msg *nmvrm __maybe_unused = &nmm->msg.vow_rule;
	nss_trace("%px: NSS match VoW rule message \n"
			"Match rule id: %hu \n"
			"Match mask id: %hu \n"
			"Match action: action flag = %u, next node = %u, priority = %hu \n"
			"Match interface number: %u \n"
			"Match DSCP: %hu \n"
			"Match outer_8021p: %hu \n"
			"Match inner_8021p: %hu \n",
			nmvrm,
			nmvrm->rule_id,
			nmvrm->mask_id,
			nmvrm->action.action_flag, nmvrm->action.forward_ifnum, nmvrm->action.setprio,
			nmvrm->if_num,
			nmvrm->dscp,
			nmvrm->outer_8021p,
			nmvrm->inner_8021p);
}

/*
 * nss_match_log_l2_rule_msg()
 * 	Log NSS match L2 rule message.
 */
static void nss_match_log_l2_rule_msg(struct nss_match_msg *nmm)
{
	struct nss_match_rule_l2_msg *nmlrm __maybe_unused = &nmm->msg.l2_rule;
	nss_trace("%px: NSS match L2 rule message \n"
			"Match rule id: %hu \n"
			"Match mask id: %hu \n"
			"Match action: action flag = %u, next node = %u, priority = %hu \n"
			"Match interface number: %u \n"
			"Match destination mac address: %x :%x :%x \n"
			"Match source mac address: %x :%x :%x \n"
			"Match ether type: %x \n",
			nmlrm,
			nmlrm->rule_id,
			nmlrm->mask_id,
			nmlrm->action.action_flag, nmlrm->action.forward_ifnum, nmlrm->action.setprio,
			nmlrm->if_num,
			nmlrm->dmac[0], nmlrm->dmac[1], nmlrm->dmac[2],
			nmlrm->smac[0], nmlrm->smac[1], nmlrm->smac[2],
			nmlrm->ethertype);

}

/*
 * nss_clmap_log_verbose()
 *	Log message contents.
 */
static void nss_match_log_verbose(struct nss_match_msg *nmm)
{
	switch (nmm->cm.type) {
	case NSS_MATCH_TABLE_CONFIGURE_MSG:
		nss_match_log_profile_configure_msg(nmm);
		break;

	case NSS_MATCH_ADD_VOW_RULE_MSG:
		nss_match_log_vow_rule_msg(nmm);
		break;

	case NSS_MATCH_ADD_L2_RULE_MSG:
		nss_match_log_l2_rule_msg(nmm);
		break;

	case NSS_MATCH_DELETE_VOW_RULE_MSG:
		nss_match_log_vow_rule_msg(nmm);
		break;

	case NSS_MATCH_DELETE_L2_RULE_MSG:
		nss_match_log_l2_rule_msg(nmm);
		break;

	case NSS_MATCH_STATS_SYNC:
		break;

	default:
		nss_trace("%px: Invalid message type\n", nmm);
		break;
	}
}

/*
 * nss_match_log_tx_msg()
 *	Log messages transmitted to FW.
 */
void nss_match_log_tx_msg(struct nss_match_msg *nmm)
{
	if (nmm->cm.type >= NSS_MATCH_MSG_MAX) {
		nss_warning("%px: Invalid message type\n", nmm);
		return;
	}

	nss_info("%px: type[%d]:%s\n", nmm, nmm->cm.type, nss_match_log_message_types_str[nmm->cm.type]);
	nss_match_log_verbose(nmm);
}

/*
 * nss_match_log_rx_msg()
 *	Log messages received from FW.
 */
void nss_match_log_rx_msg(struct nss_match_msg *nmm)
{
	if (nmm->cm.response >= NSS_CMN_RESPONSE_LAST) {
		nss_warning("%px: Invalid response\n", nmm);
		return;
	}

	if (nmm->cm.response == NSS_CMN_RESPONSE_NOTIFY || (nmm->cm.response == NSS_CMN_RESPONSE_ACK)) {
		nss_info("%px: type[%d]:%s, response[%d]:%s\n", nmm, nmm->cm.type,
			nss_match_log_message_types_str[nmm->cm.type],
			nmm->cm.response, nss_cmn_response_str[nmm->cm.response]);
		goto verbose;
	}

	if (nmm->cm.error >= NSS_MATCH_ERROR_MAX) {
		nss_warning("%px: msg failure - type[%d]:%s, response[%d]:%s, error[%d]:Invalid error\n",
			nmm, nmm->cm.type, nss_match_log_message_types_str[nmm->cm.type],
			nmm->cm.response, nss_cmn_response_str[nmm->cm.response],
			nmm->cm.error);
		goto verbose;
	}

	nss_info("%px: msg nack - type[%d]:%s, response[%d]:%s, error[%d]:%s\n",
		nmm, nmm->cm.type, nss_match_log_message_types_str[nmm->cm.type],
		nmm->cm.response, nss_cmn_response_str[nmm->cm.response],
		nmm->cm.error, nss_match_log_error_types_str[nmm->cm.error]);

verbose:
	nss_match_log_verbose(nmm);
}
