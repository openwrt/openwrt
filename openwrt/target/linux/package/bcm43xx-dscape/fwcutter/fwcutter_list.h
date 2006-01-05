
static const struct file {
	const char *name;
	const char *version;
	const char *md5;
	const uint8_t flags;
	const uint32_t iv_pos;
	const uint32_t uc2_pos;
	const uint32_t uc2_length;
	const uint32_t uc4_pos;
	const uint32_t uc4_length;
	const uint32_t uc5_pos;
	const uint32_t uc5_length;
	const uint32_t uc11_pos;
	const uint32_t uc11_length;
	const uint32_t pcm4_pos;
	const uint32_t pcm4_length;
	const uint32_t pcm5_pos;
	const uint32_t pcm5_length;
} files[] = 
{
	{
		.name        = "AppleAirPort2",
		.version     = "3.30.15.p3 (3.1.1b2)",
		.md5         = "a8275cc50107a13b5be15d067b2245a0",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x42550,
		.uc2_pos     = 0x45630,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x493bc,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x4d860,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x52724,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x52ba0,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.30.15.p3 (3.1.2b3)",
		.md5         = "55134c1298abaa85f190331f016d4d36",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x42c24,
		.uc2_pos     = 0x45d04,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x49a90,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x4df34,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x52df8,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x53274,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.30.15.p7 (3.2)",
		.md5         = "e74e8d2df2e4eb97e28602f3b2dd4647",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x40efc,
		.uc2_pos     = 0x43fdc,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x47d68,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x4c20c,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x510d0,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x5154c,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.30.15.p8 (3.3b1)",			/* 01/19/2004 */
		.md5         = "87c74c55d2501d2e968f8c132e160b6e",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x41c8c,
		.uc2_pos     = 0x43fe4,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x47d70,  .uc4_length  = 0x44b2,
		.uc5_pos     = 0x4c214,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x510d8,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x51554,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p4 (3.4.2b1)",
		.md5         = "1739c357ade1d04c9be47e8604afb1c2",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x4f2b8,
		.uc2_pos     = 0x52da0,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x56ad4,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b0a0,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x605a4,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x60a20,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p4 (3.4.3f1)",			/* 09/29/2004 */
		.md5         = "c672b8c218c5dc4a55060bdfa9f58a69",
		.flags       = BYTE_ORDER_BIG_ENDIAN | 
		               MISSING_INITVAL_08,
		.iv_pos      = 0x4f378,
		.uc2_pos     = 0x52e60,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x56b94,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b160,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x60664,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x60ae0,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p4 (3.4.4f5)",
		.md5         = "2c388e3e8ea9310a58cf76a3757e8ccc",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x4f5f0,
		.uc2_pos     = 0x530d8,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x56e0c,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b3d8,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x608dc,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x60d58,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p4 (3.4.4f5)",
		.md5         = "3860545266b554d2955664db55452f5a",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x4f5f0,
		.uc2_pos     = 0x530d8,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x56e0c,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b3d8,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x608dc,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x60d58,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p6 (3.5f1)",
		.md5         = "a62e35ee9956b286c46b145d35bd6e0c",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x4f9b8,
		.uc2_pos     = 0x534a0,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x571d4,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b7a0,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x60ca4,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x61120,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.50.37.p6 (3.5f1)",
		.md5         = "b6f3d2437c40277c197f0afcf12208e9",
		.flags       = BYTE_ORDER_BIG_ENDIAN,
		.iv_pos      = 0x4f9b8,
		.uc2_pos     = 0x534a0,  .uc2_length  = 0x3d30,
		.uc4_pos     = 0x571d4,  .uc4_length  = 0x45c8,
		.uc5_pos     = 0x5b7a0,  .uc5_length  = 0x5500,
		.pcm4_pos    = 0x60ca4,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x61120,  .pcm5_length = 0x478,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.90.34.0.p11 (400.17)",	       /* 09/13/2005 (??) */
		.md5         = "ca0f34df2f0bfb8b5cfd83b5848d2bf5",
		.flags       = BYTE_ORDER_BIG_ENDIAN | 
		               MISSING_INITVAL_80211_A,
		.iv_pos      = 0x4ff2c,                                /* A-PHY init vals empty */
		.uc2_pos     = 0x5181c,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x55764,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5a554,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x5fd34,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x60254,  .pcm5_length = 0x520,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.90.34.0.p11 (400.17)",
		.md5         = "dc3a69aac95c68fe8edc760e39bbb2c9",
		.flags       = BYTE_ORDER_BIG_ENDIAN | 
		               MISSING_INITVAL_80211_A,
		.iv_pos      = 0x50efc,                                /* A-PHY init vals empty */
		.uc2_pos     = 0x527ec,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x56734,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5b524,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x60d04,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x61224,  .pcm5_length = 0x520,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.90.34.0.p13 (401.2)",                /* 07/10/2005 */
		.md5         = "6ecf38e5ab6997c7ec483c0d637f5c68",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_80211_A,
		.iv_pos      = 0x50fcc,                                /* A-PHY init vals empty */
		.uc2_pos     = 0x528bc,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x56804,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5b5f4,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x60dd4,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x612f4,  .pcm5_length = 0x520,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.90.34.0.p16 (404.2)",
		.md5         = "7200d1aef5f413ebc811046d068b40dc",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_80211_A,
		.iv_pos      = 0x511ec,                                /* A-PHY init vals empty */
		.uc2_pos     = 0x52adc,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x56a24,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5b814,  .uc5_length  = 0x57f0,
		.pcm4_pos    = 0x61004,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x61524,  .pcm5_length = 0x520,
	},
	{
		.name        = "AppleAirPort2",
		.version     = "3.90.34.0.p16 (404.2)",
		.md5         = "86cc708e8df3b035a1dbea41ac4eb7d2",
		.flags       = BYTE_ORDER_BIG_ENDIAN |
		               MISSING_INITVAL_80211_A,
		.iv_pos      = 0x5021c,                                /* A-PHY init vals empty */
		.uc2_pos     = 0x51b0c,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x55a54,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5a844,  .uc5_length  = 0x57f0,
		.pcm4_pos    = 0x60034,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x60554,  .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.10.8.0",                             /* 10/04/2002 */ 
		.md5         = "288923b401e87ef76b7ae2652601ee47",
		.flags       = DRIVER_UNSUPPORTED,                     /* file differs from later ones */
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.10.53.6",                            /* 04/28/2003 */ 
		.md5         = "b43c593fd7c2a47cdc40580fe341f674",
		.flags       = DRIVER_UNSUPPORTED,                     /* file differs from later ones */
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.20.23.0",                            /* 06/13/2003 */ 
		.md5         = "1b1cf5e962c15abca83d1ef2b3906e2f",     /* pcm5 not available, driver is too old */
		.flags       = BYTE_ORDER_LITTLE_ENDIAN | 
		               MISSING_INITVAL_08,
		.iv_pos      = 0x2a1d0,
		.uc2_pos     = 0x2d228,  .uc2_length  = 0x3da8,
		.uc4_pos     = 0x30fd8,  .uc4_length  = 0x4470,
		.uc5_pos     = 0x35450,  .uc5_length  = 0x4ba0,
		.pcm4_pos    = 0x39ff8,  .pcm4_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.30.15.0",                            /* 07/17/2003 */ 
		.md5         = "ebf36d658d0da5b1ea667fa403919c26", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN | 
		               MISSING_INITVAL_08,
		.iv_pos      = 0x2c658,
		.uc2_pos     = 0x2f738,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x334c8,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x37970,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x3c838,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x3ccb8,  .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.40.20.0",                            /* 09/24/2003 */ 
		.md5         = "0c3fc803184f6f85e665dd012611225b", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x302f0,
		.uc2_pos     = 0x33d88,  .uc2_length  = 0x3db8,
		.uc4_pos     = 0x37b48,  .uc4_length  = 0x45d8,
		.uc5_pos     = 0x3c128,  .uc5_length  = 0x5050,
		.pcm4_pos    = 0x41180,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x41600,  .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.40.25.3",                            /* 10/28/2003 */ 
		.md5         = "5e58a3148b98c9f356cde6049435cb21", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x30970,
		.uc2_pos     = 0x34408,  .uc2_length  = 0x3db0,
		.uc4_pos     = 0x381c0,  .uc4_length  = 0x45d0,
		.uc5_pos     = 0x3c798,  .uc5_length  = 0x5050,
		.pcm4_pos    = 0x417f0,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x41c70,  .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.40.73.0",                            /* 06/25/2004 */ 
		.md5         = "52d67c5465c01913b03b7daca0cc4077", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x37398,
		.uc2_pos     = 0x3ae30,  .uc2_length  = 0x3ff0,
		.uc4_pos     = 0x3ee28,  .uc4_length  = 0x47f0,
		.uc5_pos     = 0x43620,  .uc5_length  = 0x5260,
		.pcm4_pos    = 0x48888,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x48d08,  .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.40.100.0",                           /* 02/07/2004 */ 
		.md5         = "431195b941dff794f23c2077fcbf8377", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x34d18,
		.uc2_pos     = 0x387b0,  .uc2_length  = 0x3ff0,
		.uc4_pos     = 0x3c7a8,  .uc4_length  = 0x47f0,
		.uc5_pos     = 0x40fa0,  .uc5_length  = 0x5260,
		.pcm4_pos    = 0x46208,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x46688,  .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.50.21.11",                           /* 02/19/2004 */
		.md5         = "ae96075a3aed5c40f1ead477ea94acd7", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x33370,
		.uc2_pos     = 0x36e58,	 .uc2_length  = 0x3e00,
		.uc4_pos     = 0x3ac60,	 .uc4_length  = 0x4628,
		.uc5_pos     = 0x3f290,	 .uc5_length  = 0x5548,
		.pcm4_pos    = 0x447e0,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x44c60,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.60.7.0",                             /* 03/22/2004 */
		.md5         = "c5616736df4e83930780dca5795387ca", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3b988,
		.uc2_pos     = 0x3f580,	 .uc2_length  = 0x3e08,
		.uc4_pos     = 0x43390,	 .uc4_length  = 0x4e58,
		.uc5_pos     = 0x481f0,	 .uc5_length  = 0x5608,
		.pcm4_pos    = 0x4d800,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x4dc80,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.60.7.5",                             /* 06/07/2004 */
		.md5         = "d2ae116c741c215ef3ef68603db9917f", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3b988,
		.uc2_pos     = 0x3f580,	 .uc2_length  = 0x3e08,
		.uc4_pos     = 0x43390,	 .uc4_length  = 0x4e58,
		.uc5_pos     = 0x481f0,	 .uc5_length  = 0x5608,
		.pcm4_pos    = 0x4d800,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x4dc80,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.70.12.0",                            /* 06/02/2004 */
		.md5         = "d409b089370486521d5408baed9bffde", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x39480,
		.uc2_pos     = 0x3d0c8,	 .uc2_length  = 0x3e80,
		.uc4_pos     = 0x40f50,	 .uc4_length  = 0x4ed0,
		.uc5_pos     = 0x45e28,	 .uc5_length  = 0x5680,
		.pcm4_pos    = 0x4b4b0,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x4b930,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.70.17.0",                            /* 06/25/2004 */
		.md5         = "d87b4e14e890091d8e64fb5c570cf192", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x39600,
		.uc2_pos     = 0x3d248,	 .uc2_length  = 0x3e80,
		.uc4_pos     = 0x410d0,	 .uc4_length  = 0x4ed0,
		.uc5_pos     = 0x45fa8,	 .uc5_length  = 0x5680,
		.pcm4_pos    = 0x4b630,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x4bab0,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.70.22.0",                            /* 10/20/2004 */
		.md5         = "185a6dc6d655dc31c0b228cc94fb99ac", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x39a88,
		.uc2_pos     = 0x3d6d0,	 .uc2_length  = 0x3e80,
		.uc4_pos     = 0x41558,	 .uc4_length  = 0x4ed0,
		.uc5_pos     = 0x46430,	 .uc5_length  = 0x5680,
		.pcm4_pos    = 0x4bab8,	 .pcm4_length = 0x478,
		.pcm5_pos    = 0x4bf38,	 .pcm5_length = 0x478,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.90.41.1",                            /* 07/04/2005 */
		.md5         = "0a87541dd24c6f046a4bf8f671f74de2", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3bec8,
		.uc2_pos     = 0x3fb38,	 .uc2_length  = 0x3f48,
		.uc4_pos     = 0x43a88,	 .uc4_length  = 0x4df0,
		.uc5_pos     = 0x48880,	 .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x4e068,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x4e590,	 .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.100.35.0",                           /* 11/27/2004 */
		.md5         = "c3ab2d6954c7b5103770832a3a6a591b", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3de80,
		.uc2_pos     = 0x41af0,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x45a50,	 .uc4_length  = 0x4df8,
		.uc5_pos     = 0x4a850,	 .uc5_length  = 0x57f8,
		.pcm4_pos    = 0x50050,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x50578,	 .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.100.35.1",                           /* 02/10/2005 */
		.md5         = "da7ca369a1a3593ceac85dec2d267a08", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3df00,
		.uc2_pos     = 0x41b70,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x45ad0,	 .uc4_length  = 0x4df8,
		.uc5_pos     = 0x4a8d0,	 .uc5_length  = 0x57f8,
		.pcm4_pos    = 0x500d0,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x505f8,	 .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.100.46.0",                           /* 12/22/2004 */
		.md5         = "38ca1443660d0f5f06887c6a2e692aeb", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3de80,
		.uc2_pos     = 0x41af0,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x45a50,	 .uc4_length  = 0x4df8,
		.uc5_pos     = 0x4a850,	 .uc5_length  = 0x57f8,
		.pcm4_pos    = 0x50050,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x50578,	 .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.100.64.0",                           /* 02/11/2005 */
		.md5         = "e7debb46b9ef1f28932e533be4a3d1a9", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3e980,
		.uc2_pos     = 0x425f0,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x46550,	 .uc4_length  = 0x4e00,
		.uc5_pos     = 0x4b358,	 .uc5_length  = 0x5800,
		.pcm4_pos    = 0x50b60,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x51088,	 .pcm5_length = 0x520,
	},
	{ 
		.name        = "bcmwl5.sys",
		.version     = "3.100.64.50",                          /* 04/05/2005 */
		.md5         = "4b3e367b829b9b2c0c088909a617e04e", 
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3e900,
		.uc2_pos     = 0x42570,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x464d0,	 .uc4_length  = 0x4e00,
		.uc5_pos     = 0x4b2d8,	 .uc5_length  = 0x5800,
		.pcm4_pos    = 0x50ae0,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x51008,	 .pcm5_length = 0x520,
	},
	{
		.name        = "bcmwl5.sys",
		.version     = "3.100.65.1",                           /* 04/21/2005 */
		.md5         = "d5f1ab1aab8b81bca6f19da9554a267a",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3e980,
		.uc2_pos     = 0x425f0,	 .uc2_length  = 0x3f58,
		.uc4_pos     = 0x46550,	 .uc4_length  = 0x4e00,
		.uc5_pos     = 0x4b358,	 .uc5_length  = 0x5800,
		.pcm4_pos    = 0x50b60,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x51088,	 .pcm5_length = 0x520,
	},
	{
		.name        = "bcmwl5.sys",
		.version     = "3.120.27.0",                           /* 05/19/2005 */
		.md5         = "8d49f11238815a320880fee9f98b2c92",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3f700,
		.uc2_pos     = 0x43370,	 .uc2_length  = 0x3f68,
		.uc4_pos     = 0x472e0,	 .uc4_length  = 0x4e10,
		.uc5_pos     = 0x4c0f8,	 .uc5_length  = 0x5800,
		.pcm4_pos    = 0x51900,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x51e28,	 .pcm5_length = 0x520,
	},
	{
		.name        = "bcmwl5.sys",
		.version     = "3.140.16.0",                           /* 07/21/2005 */
		.md5         = "fa4a4a50b4b2647afedc676cc68c69cc",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3f700,
		.uc2_pos     = 0x43370,	 .uc2_length  = 0x3f68,
		.uc4_pos     = 0x472e0,	 .uc4_length  = 0x4e10,
		.uc5_pos     = 0x4c0f8,	 .uc5_length  = 0x5800,
		.pcm4_pos    = 0x51900,	 .pcm4_length = 0x520,
		.pcm5_pos    = 0x51e28,	 .pcm5_length = 0x520,
	},
	{
		.name        = "bcmwl564.sys",
		.version     = "3.70.17.5",                            /* 09/21/2004 */
		.md5         = "f5590c8784b91dfd9ee092d3040b6e40",     /* for 64bit machines   */
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x566f0,
		.uc2_pos     = 0x5a360,  .uc2_length  = 0x3e80,
		.uc4_pos     = 0x5e1f0,  .uc4_length  = 0x4ed0,
		.uc5_pos     = 0x630d0,  .uc5_length  = 0x5680,
		.pcm4_pos    = 0x68760,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x68be0,  .pcm5_length = 0x478,
	},
	{
		.name        = "bcmwl564.sys",
		.version     = "3.100.64.10",                          /* 05/12/2005 */
		.md5         = "b8d76da338ecf2c650f5f7ca226ccf89",     /* for 64bit machines   */
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x50bf0,
		.uc2_pos     = 0x54860,  .uc2_length  = 0x3f58,
		.uc4_pos     = 0x587c0,  .uc4_length  = 0x4e00,
		.uc5_pos     = 0x5d5d0,  .uc5_length  = 0x5800,
		.pcm4_pos    = 0x62de0,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x63310,  .pcm5_length = 0x520,
	},
	{
		.name        = "bcmwl5a.sys",
		.version     = "3.90.16.0",                            /* 12/06/2004 */
		.md5         = "e6d927deea6c75bddf84080e6c3837b7",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3b4c8,
		.uc2_pos     = 0x3f138,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x43088,  .uc4_length  = 0x4de8,
		.uc5_pos     = 0x47e78,  .uc5_length  = 0x57d8,
		.pcm4_pos    = 0x4d658,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x4db80,  .pcm5_length = 0x520,
	},
	{
		.name        = "d11ucode.o",
		.version     = "3.31.16.0 ?", /*FIXME: version correct? */
		.md5         = "31e6cac8a8129bf8f91291293e017329",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
			       MISSING_INITVAL_08,
		.iv_pos      = 0x40,
		.uc2_pos     = 0x3120,   .uc2_length  = 0x3d88,
		.uc4_pos     = 0x6eac,   .uc4_length  = 0x44a0,
		.uc5_pos     = 0xb350,   .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x10214,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x10690,  .pcm5_length = 0x478,
	},
	{
		.name        = "d11ucode.o",
		.version     = "3.60.7.0 ? (1.3.2)",                   /* no version number found */
		.md5         = "7774e5dda1daa2b3f83d279552ca1cc4",     /* but same fw files are also */
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,               /* in bcmwl5.sys 3.60.7.0 */
		.iv_pos      = 0x40,
		.uc2_pos     = 0x3c38,   .uc2_length  = 0x3e08,
		.uc4_pos     = 0x7a44,   .uc4_length  = 0x4e58,
		.uc5_pos     = 0xc8a0,   .uc5_length  = 0x5608,
		.pcm4_pos    = 0x11eac,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x12328,  .pcm5_length = 0x478,
	},
	{
		.name        = "d11ucode.o",
		.version     = "3.90.7.0 ? (3.90.RC7)",                /* 10/14/2004 */
		.md5         = "b2580361620881b06fa810422ec8b7ce",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x60,
		.uc2_pos     = 0x3cd0,   .uc2_length  = 0x3ec0,
		.uc4_pos     = 0x7b94,   .uc4_length  = 0x4d60,
		.uc5_pos     = 0xc8f8,   .uc5_length  = 0x5750,
		.pcm4_pos    = 0x1204c,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x12570,  .pcm5_length = 0x520,
	},
	{
		.name        = "d11ucode.o",
		.version     = "3.90.37.0",
		.md5         = "2543935259739a8a879ccb8386647ac7",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x60,
		.uc2_pos     = 0x3cd0,   .uc2_length  = 0x3f48,
		.uc4_pos     = 0x7c1c,   .uc4_length  = 0x4df0,
		.uc5_pos     = 0xca10,   .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x121f4,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x12718,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl.o",
		.version     = "3.31.15.0",                            /* 07/28/2003 */
		.md5         = "a85af65b5ae1d64ee11eab714faab843",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x32270,
		.uc2_pos     = 0x35350,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x390dc,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x3d580,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x42444,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x428c0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.31.15.0",                            /* 07/28/2003 */
		.md5         = "98dd50a95b02c8bcb3725c770df81dfc",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x32270,
		.uc2_pos     = 0x35350,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x390dc,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x3d580,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x42444,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x428c0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.50.21.0",                            /* 05/11/2003 */
		.md5         = "f71be0e1d14f68c98d916465a300d835",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x38990,
		.uc2_pos     = 0x3c428,  .uc2_length  = 0x3db8,
		.uc4_pos     = 0x401e4,  .uc4_length  = 0x45d8,
		.uc5_pos     = 0x447c0,  .uc5_length  = 0x5050,
		.pcm4_pos    = 0x49814,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x49c90,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.50.21.10",                           /* 01/21/2004 */
		.md5         = "191029d5e7097ed7db92cbd6e6131f85",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3a5d0,
		.uc2_pos     = 0x3e0b8,  .uc2_length  = 0x3e00,
		.uc4_pos     = 0x41ebc,  .uc4_length  = 0x4628,
		.uc5_pos     = 0x464e8,  .uc5_length  = 0x5548,
		.pcm4_pos    = 0x4ba34,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x4beb0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.60.7.0 ? (1.3.2.0)",                   /* 02/04/2004 */
		.md5         = "7009220d84c445797443aa0221b7d8a4",     /* no bcm version found */
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,               /* but same fw files are also */
		.iv_pos      = 0x545e0,                                /* in bcmwl5.sys 3.60.7.0, */
		.uc2_pos     = 0x581d8,  .uc2_length  = 0x3e08,
		.uc4_pos     = 0x5bfe4,  .uc4_length  = 0x4e58,
		.uc5_pos     = 0x60e40,  .uc5_length  = 0x5608,
		.pcm4_pos    = 0x6644c,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x668c8,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.60.13.0",                            /* 05/05/2004 */
		.md5         = "275dd4958eb94058d4650f58d534c635",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3b740,
		.uc2_pos     = 0x3f388,  .uc2_length  = 0x3e30,
		.uc4_pos     = 0x431bc,  .uc4_length  = 0x4e80,
		.uc5_pos     = 0x48040,  .uc5_length  = 0x5630,
		.pcm4_pos    = 0x4d674,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x4daf0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.60.13.0",                            /* 08/02/2004 */
		.md5         = "4f20653a0b7c0bb3bd1ee0dd79f77785",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3b730,
		.uc2_pos     = 0x3f378,  .uc2_length  = 0x3e30,
		.uc4_pos     = 0x431ac,  .uc4_length  = 0x4e80,
		.uc5_pos     = 0x48030,  .uc5_length  = 0x5630,
		.pcm4_pos    = 0x4d664,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x4dae0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl.o",
		.version     = "3.90.7.0",                             /* 14/10/2004 */
		.md5         = "f15b59ad4d6f6ad2c7f45193d033aff8",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x4d890,
		.uc2_pos     = 0x51500,  .uc2_length  = 0x3ec0,
		.uc4_pos     = 0x553c4,  .uc4_length  = 0x4d60,
		.uc5_pos     = 0x5a128,  .uc5_length  = 0x5750,
		.pcm4_pos    = 0x5f87c,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x5fda0,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "984c42947552652d5ab61b78e7d12227",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x4cf50,
		.uc2_pos     = 0x50bc0,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x54b0c,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x59900,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x5f0e4,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x5f608,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_ap.o",
		.version     = "3.31.16.0",                            /* 08/06/2003 */
		.md5         = "463633e7bf0efc6c0f8eac2514a71024",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x32270,
		.uc2_pos     = 0x35350,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x390dc,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x3d580,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x42444,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x428c0,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl_ap.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "0538296d46a854d2facc9a0db2088180",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3eac0,
		.uc2_pos     = 0x42730,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x4667c,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x4b470,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x50c54,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x51178,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_ap.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "6c9073531a528dd455b716b5e821e696",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x3eac0,
		.uc2_pos     = 0x42730,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x4667c,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x4b470,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x50c54,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x51178,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_apsta.o",
		.version     = "3.31.16.0",                            /* 06/08/2003 */
		.md5         = "22b90e4cbeee45ad7f78ff536c65712a",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x38020,
		.uc2_pos     = 0x3b100,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x3ee8c,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x43330,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x481f4,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x48670,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl_apsta.o",
		.version     = "3.31.16.0",                            /* 06/08/2003 */
		.md5         = "dfce35a8b5cb6e53e1ab75342c7e7194",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x38020,
		.uc2_pos     = 0x3b100,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x3ee8c,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x43330,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x481f4,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x48670,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl_apsta.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "2922e6c48917eacd6c8c094347d2ec4b",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x4ebc0,
		.uc2_pos     = 0x52830,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x5677c,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5b570,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x60d54,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x61278,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_apsta.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "f9fa565a766e8befeb495a8a8c0c134b",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x4ebc0,
		.uc2_pos     = 0x52830,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x5677c,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x5b570,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x60d54,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x61278,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_apsta.o",
		.version     = "3.130.20.0",                           /* 19/05/2005 */
		.md5         = "e08665c5c5b66beb9c3b2dd54aa80cb3",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x56020,
		.uc2_pos     = 0x59ca0,  .uc2_length  = 0x3fe0,
		.uc4_pos     = 0x5dc84,  .uc4_length  = 0x4e78,
		.uc5_pos     = 0x62b00,  .uc5_length  = 0x5700,
		.uc11_pos    = 0x68204,  .uc11_length = 0x54a8,
		.pcm4_pos    = 0x6d6b0,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x6dbd4,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_sta.o",
		.version     = "3.31.16.0",                            /* 08/06/2003 */
		.md5         = "c3e663cb78b2fc299088de69fc11a9a9",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN |
		               MISSING_INITVAL_08,
		.iv_pos      = 0x317b0,
		.uc2_pos     = 0x34890,  .uc2_length  = 0x3d88,
		.uc4_pos     = 0x3861c,  .uc4_length  = 0x44a0,
		.uc5_pos     = 0x3cac0,  .uc5_length  = 0x4ec0,
		.pcm4_pos    = 0x41984,  .pcm4_length = 0x478,
		.pcm5_pos    = 0x41e00,  .pcm5_length = 0x478,
	},
	{
		.name        = "wl_sta.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "4631d4d4c3ab943462e1ea24f4dba7bd",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x47040,
		.uc2_pos     = 0x4acb0,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x4ebfc,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x539f0,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x591d4,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x596f8,  .pcm5_length = 0x520,
	},
	{
		.name        = "wl_sta.o",
		.version     = "3.90.37.0",                            /* 15/02/2005 */
		.md5         = "bf824b38b3993e0a8b5a9bf717c428ed",
		.flags       = BYTE_ORDER_LITTLE_ENDIAN,
		.iv_pos      = 0x47040,
		.uc2_pos     = 0x4acb0,  .uc2_length  = 0x3f48,
		.uc4_pos     = 0x4ebfc,  .uc4_length  = 0x4df0,
		.uc5_pos     = 0x539f0,  .uc5_length  = 0x57e0,
		.pcm4_pos    = 0x591d4,  .pcm4_length = 0x520,
		.pcm5_pos    = 0x596f8,  .pcm5_length = 0x520,
	},
};

#define FILES (sizeof(files) / sizeof(files[0]))
