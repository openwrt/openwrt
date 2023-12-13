#include "../type.h"
#define OTP_KEY_INFO_NUM 2

#define EFUSE_EXTERNALPN_DEFAULT 0xFF
#define EFUSE_EXTERNALPN_ADDR  0x5EC
#define EFUSE_EXTERNALPN_SH 0
#define EFUSE_EXTERNALPN_MSK 0xff

#define EFUSE_CUSTOMER_DEFAULT 0xF
#define EFUSE_CUSTOMER_ADDR  0x5ED
#define EFUSE_CUSTOMER_SH 0
#define EFUSE_CUSTOMER_MSK 0xf

#define EFUSE_SERIALNUM_DEFAULT 0x7
#define EFUSE_SERIALNUM_ADDR  0x5ED
#define EFUSE_SERIALNUM_SH 4
#define EFUSE_SERIALNUM_MSK 0x7

extern u8 otp_key_info_externalPN[OTP_KEY_INFO_NUM];
extern u8 otp_key_info_customer[OTP_KEY_INFO_NUM];
extern u8 otp_key_info_serialNum[OTP_KEY_INFO_NUM];
extern u32 otp_key_info_keyID[OTP_KEY_INFO_NUM];
