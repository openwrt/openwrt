#ifndef __VMMC_CMDS_H__
#define __VMMC_CMDS_H__

#define _VMMC_CMD(_x) ((_x) << 24)
#define _VMMC_MOD(_x) ((_x) << 13)
#define _VMMC_ECMD(_x) ((_x) << 8)

#define _VMMC_MSG(_cmd, _mod, _ecmd) \
	(_VMMC_CMD(_cmd) | _VMMC_ECMD(_ecmd) | _VMMC_MOD(_mod))

#define _VMMC_CHAN(_chan) ((_chan) << 16)
#define _VMMC_LENGTH(_length) ((_length) << 2)

#define VMMC_CMD_OPMODE(_chan) (_VMMC_MSG(1, 0, 0) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(1))

#define VMMC_CMD_SIG(_chan) (_VMMC_MSG(6, 2, 1) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(1))
#define VMMC_CMD_SIG_DATA(_enable, _event, _rate, _i1, _i2, _mute1, _mute2) \
	(((_enable) << 31) | ((_event) << 30) | ((_i1) << 24) | ((_i2) << 16) | \
	((_rate) << 23) | ((_mute1) << 15) | ((_mute2) << 14))

#define VMMC_CMD_SIG_SET_ENABLE(_data, _enable) (((_data) & ~BIT(31)) | ((_enable) << 31))
#define VMMC_CMD_SIG_SET_INPUTS(_data, _i1, _i2) (((_data) & ~0x3f3f0000) | \
	((_i1) << 24) | ((_i2) << 16))

#define VMMC_CMD_DTMFR(_chan) (_VMMC_MSG(6, 2, 4) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(1))
#define VMMC_CMD_DTMFR_DATA(_enable, _event, _nr) \
	(((_enable) << 31) | ((_event) << 30) | (6 << 27) | ((_nr) << 16))

#define VMMC_CMD_CODER(_chan) (_VMMC_MSG(6, 3, 1) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(4))

#define VMMC_CMD_CODER_DATA1(_enable, _rate, _ns, _pte, _nr, _i1, _hp, _pf, \
	_cng, _bfi, _dec, _im, _pst, _sic, _em, _enc) \
	(((_enable) << 31) | ((_rate) << 30) | ((_ns) << 29) | ((_pte) << 26) | \
	((_nr) << 22) | ((_i1) << 16) | ((_hp) << 15) | ((_pf) << 14) | \
	((_cng) << 13) | ((_bfi) << 12) | ((_dec) << 11) | ((_im) << 10) | \
	((_pst) << 9) | ((_sic) << 8) | ((_em) << 7) | (_enc))

#define VMMC_CMD_CODER_DATA2(_gain1, _gain2) (((_gain1) << 16) | (_gain2))

#define VMMC_CMD_CODER_DATA3(_de, _ee, _i2, _red, _i3, _plc, _i4, _i5) \
	(((_de) << 31) | ((_ee) << 30) | ((_i2) << 24) | ((_red) << 22) | \
	((_i3) << 16) | ((_plc) << 15) | ((_i4) << 8) | (_i5))

#define VMMC_CMD_SERR_ACK(_chan) _VMMC_MSG(6, 7, 1) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(1)

#define VMMC_CMD_SERR_ACK_DATA1(_foo) ((_foo) << 22)

#define VMMC_CMD_CODER_DATA4(_tsf) ((_tsf) << 31)

#define VMMC_EVENT_ID_MASK (_VMMC_MSG(0x1f, 0x7, 0x1f) | 0xff)

#define VMMC_MSG_GET_CHAN(_msg) (((_msg) >> 16) & 0x1f)

#define VMMC_EVENT_HOOK_STATE(_data) ((_data) & 1)

#define VMMC_EVENT_HOOK_ID (_VMMC_MSG(9, 1, 1) | _VMMC_LENGTH(1))
#define VMMC_EVENT_DTMF_ID (_VMMC_MSG(9, 2, 0) | _VMMC_LENGTH(1))

#define VMMC_VOICE_DATA(_type, _chan, _len) (((_type) << 24) | ((_chan) << 16) \
	| (_len))


#define VMMC_CMD_ALI(_chan) (_VMMC_MSG(6, 1, 1) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(3))

#define VMMC_CMD_ALI_DATA1(_enable, _rate, _ud, _eh, _eo, _i1, _dg1) \
	(((_enable) << 31) | ((_rate) << 30) | ((_ud) << 29) | ((_eh) << 27) | \
	((_eo) << 26) | ((_i1) << 16) | (_dg1))

#define VMMC_CMD_ALI_DATA2(_dg2, _i2, _i3) \
	(((_dg2) << 16) | ((_i2) << 8) | (_i3))

#define VMMC_CMD_ALI_DATA3(_i4, _i5) \
	(((_i4) << 24) | ((_i5) << 16))

#define VMMC_CMD_ALM_COEF(_chan, _offset, _len) \
	(_VMMC_MSG(2, 0, _offset) | _VMMC_CHAN(_chan) | (_len))

#define CMD_VOICEREC_STATUS_PACKET  0x0
#define CMD_VOICEREC_DATA_PACKET    0x1
#define CMD_RTP_VOICE_DATA_PACKET   0x4
#define CMD_RTP_EVENT_PACKET        0x5
#define CMD_ADDRESS_PACKET          0x8
#define CMD_FAX_DATA_PACKET         0x10
#define CMD_FAX_STATUS_PACKET       0x11
#define CMD_P_PHONE_DATA_PACKET     0x12
#define CMD_P_PHONE_STATUS_PACKET   0x13

#define VMMC_CMD_RTP_CFG_US(_chan) \
	(_VMMC_MSG(6, 3, 17) | _VMMC_CHAN(_chan) | (36))
#define VMMC_CMD_RTP_CFG_DS(_chan) \
	(_VMMC_MSG(6, 3, 25) | _VMMC_CHAN(_chan) | (32))

#define VMMC_CMD_LEC(_chan) \
	(_VMMC_MSG(6, 2, 1) | _VMMC_CHAN(_chan) | _VMMC_LENGTH(1))
//	(_VMMC_MSG(CMD_EOP, ALI_LEC_ECMD, MOD_ALI) | _VMMC_CHAN(_chan) | (32))

#define VMMC_CMD_LEC_DATA() 
#endif
