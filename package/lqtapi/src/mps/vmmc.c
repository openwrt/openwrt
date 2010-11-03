static uint32_t magic_init[] = {
/* ALI INIT */
/*0x0601210c, 0x88002000, 0x20000000, 0x00000000, 0xffffffff,*/
/* COD_CHAN_SPEECH_ECMD */
/*0x06016110, 0x2462f700, 0x20002000, 0x00000000, 0x00000000, 0xffffffff,*/
/* COD_DEC_STAT_ECMD */
/*0x06017504, 0x00c00000, 0xffffffff,*/
/* COD_JB_CONF_ECMD */
/*0x06017208, 0x16410050, 0x005005a0, 0xffffffff,*/
/* SIG_RTP_SUP */
/*0x0601500c, 0x00000000, 0x00000060, 0x0712007f, 0xffffffff,*/
/* SIG_CHAN */
/*0x06014104, 0xc0190000, 0xffffffff,*/
/* SIG_CIDS_CTRL_ECMD */
/*0x06014204, 0x3811e000, 0xffffffff, */
/* SIG_DTMFATG_DATA */
/*0x06014b04, 0x00010000, 0xffffffff,*/
/* SIG_DTMFATG_CTRL */
/*0x06014304, 0x6a110000, 0xffffffff,
0x0601cc04, 0xe21c2000, 0xffffffff,
0x06014404, 0xb0110000, 0xffffffff,
0x06014a04, 0x04510000, 0xffffffff,
0x06014604, 0x04560000, 0xffffffff,
0x06014104, 0xc0190000, 0xffffffff,*/
/* COD_CHAN_RTP_SUP_CFG_USD */
/*0x06017124, 0x00000000, 0x00008880, 0xe3e4e5e6, 0x72727272, 0x72727272,
0x0f7f1261, 0x7374097f, 0xf1f06767, 0x04047675, 0xffffffff,*/
/* COD_CHAN_RTP_SUP_CFG_DS */
/* 0x06017920, 0x08006364, 0x65667272, 0x72727272, 0x72720f7f, 0x12617374,
0x097f7170, 0x67670404, 0x76750000, 0xffffffff, */
/* OPMODE_CMD */
0x01010004, 0x00010000, 0xffffffff,
0x01000004, 0x00030000, 0xffffffff,
/*0x01010004, 0x00010000, 0xffffffff,*/
/* COD_CHAN_RTP_SUP_CFG_US */
/* 0x06017124, 0x00000000, 0x00008880, 0xe3e4e5e6, 0x72727272, 0x72727272,
0x0f7f1261, 0x7374097f, 0xf1f06767, 0x04047675, 0xffffffff, */
/* COD_CHAN_RTP_SUP_CFG_DS */
/* 0x06017920, 0x08006364, 0x65667272, 0x72727272, 0x72720f7f, 0x12617374,
0x097f7170, 0x67670404, 0x76750000, 0xffffffff, */
/* COD_JB_CONF */
/* 0x06017208, 0x16410050, 0x005005a0, 0xffffffff, */
/* COD_CHAN_RTP_SUP_CFG_US */
/*0x06017108, 0x00000000, 0x00008880, 0xffffffff,*/
/* COD_CHAN_RTP_TIMESTAMP */
/*0x06017004, 0x00000000, 0xffffffff,*/
/* SIG_RTP_SUP */
/* 0x0601500c, 0x00000000, 0x00000062, 0x0712007f, 0xffffffff,*/
/* SIG_DTMFR_CTRL */
/*0x06014404, 0xb0010000, 0xffffffff,*/
/* COD_CHAN_SPEECH */
/* 0x06016110, 0x0462d600, 0x20002000, 0x00000000, 0x00000000, 0xffffffff, */
/* ALI_CHAN */
0x0601210c, 0x88232000, 0x20000000, 0x00000000, 0xffffffff,
/* SIG_CHAN */
/*0x06014104, 0xc5190000, 0xffffffff,*/
/* SIG_DTMFR_CTRL_ECMD */
/*0x06014404, 0x30010000, 0xffffffff,*/
/* SIG_CHAN_ECMD */
/*0x06014104, 0x45190000, 0xffffffff,*/
};

static void setup_alm(struct mps *mps, int chan)
{
	uint32_t *data = magic_init;
	int size, i;

/*	data[0] = ALI_CHAN_DATA1(1, 0, 0, 1, 1, 0, 0);
	data[1] = ALI_CHAN_DATA2(0, 0, 0);
	data[2] = ALI_CHAN_DATA3(0, 0);*/

	size = 1;
	for (i = 1; i < ARRAY_SIZE(magic_init); ++i) {
		if (magic_init[i] == 0xffffffff) {

			printk("cmd: %x\n", *data);
			VMMC_FIFO_fifo_in(&mps->mbox_cmd.downstream, data, size);
			size = 0;
			data = &magic_init[i+1];
			mdelay(500);
		} else {
			size += 1;
		}
	}


/*	VMMC_FIFO_mailbox_command_write(&mps->mbox_cmd, MPS_CMD_ALI(chan), data);*/
}


