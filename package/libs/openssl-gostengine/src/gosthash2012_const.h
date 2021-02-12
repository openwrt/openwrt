/*
 * GOST R 34.11-2012 constants.
 *
 * Copyright (c) 2013 Cryptocom LTD.
 * This file is distributed under the same license as OpenSSL.
 *
 * Author: Alexey Degtyarev <alexey@renatasystems.org>
 *
 */

ALIGN(16)
static const union uint512_u buffer0 = {
    {0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL}
};

#ifndef __GOST3411_BIG_ENDIAN__
ALIGN(16)
static const union uint512_u buffer512 = {
    {0x0000000000000200ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
     0x0ULL}
};
#else
ALIGN(16)
static const union uint512_u buffer512 = {
    {0x0002000000000000ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL,
     0x0ULL}
};
#endif

#ifndef __GOST3411_BIG_ENDIAN__
ALIGN(16)
static const union uint512_u C[12] = {
    {{
      0xdd806559f2a64507ULL,
      0x05767436cc744d23ULL,
      0xa2422a08a460d315ULL,
      0x4b7ce09192676901ULL,
      0x714eb88d7585c4fcULL,
      0x2f6a76432e45d016ULL,
      0xebcb2f81c0657c1fULL,
      0xb1085bda1ecadae9ULL}},
    {{
      0xe679047021b19bb7ULL,
      0x55dda21bd7cbcd56ULL,
      0x5cb561c2db0aa7caULL,
      0x9ab5176b12d69958ULL,
      0x61d55e0f16b50131ULL,
      0xf3feea720a232b98ULL,
      0x4fe39d460f70b5d7ULL,
      0x6fa3b58aa99d2f1aULL}},
    {{
      0x991e96f50aba0ab2ULL,
      0xc2b6f443867adb31ULL,
      0xc1c93a376062db09ULL,
      0xd3e20fe490359eb1ULL,
      0xf2ea7514b1297b7bULL,
      0x06f15e5f529c1f8bULL,
      0x0a39fc286a3d8435ULL,
      0xf574dcac2bce2fc7ULL}},
    {{
      0x220cbebc84e3d12eULL,
      0x3453eaa193e837f1ULL,
      0xd8b71333935203beULL,
      0xa9d72c82ed03d675ULL,
      0x9d721cad685e353fULL,
      0x488e857e335c3c7dULL,
      0xf948e1a05d71e4ddULL,
      0xef1fdfb3e81566d2ULL}},
    {{
      0x601758fd7c6cfe57ULL,
      0x7a56a27ea9ea63f5ULL,
      0xdfff00b723271a16ULL,
      0xbfcd1747253af5a3ULL,
      0x359e35d7800fffbdULL,
      0x7f151c1f1686104aULL,
      0x9a3f410c6ca92363ULL,
      0x4bea6bacad474799ULL}},
    {{
      0xfa68407a46647d6eULL,
      0xbf71c57236904f35ULL,
      0x0af21f66c2bec6b6ULL,
      0xcffaa6b71c9ab7b4ULL,
      0x187f9ab49af08ec6ULL,
      0x2d66c4f95142a46cULL,
      0x6fa4c33b7a3039c0ULL,
      0xae4faeae1d3ad3d9ULL}},
    {{
      0x8886564d3a14d493ULL,
      0x3517454ca23c4af3ULL,
      0x06476983284a0504ULL,
      0x0992abc52d822c37ULL,
      0xd3473e33197a93c9ULL,
      0x399ec6c7e6bf87c9ULL,
      0x51ac86febf240954ULL,
      0xf4c70e16eeaac5ecULL}},
    {{
      0xa47f0dd4bf02e71eULL,
      0x36acc2355951a8d9ULL,
      0x69d18d2bd1a5c42fULL,
      0xf4892bcb929b0690ULL,
      0x89b4443b4ddbc49aULL,
      0x4eb7f8719c36de1eULL,
      0x03e7aa020c6e4141ULL,
      0x9b1f5b424d93c9a7ULL}},
    {{
      0x7261445183235adbULL,
      0x0e38dc92cb1f2a60ULL,
      0x7b2b8a9aa6079c54ULL,
      0x800a440bdbb2ceb1ULL,
      0x3cd955b7e00d0984ULL,
      0x3a7d3a1b25894224ULL,
      0x944c9ad8ec165fdeULL,
      0x378f5a541631229bULL}},
    {{
      0x74b4c7fb98459cedULL,
      0x3698fad1153bb6c3ULL,
      0x7a1e6c303b7652f4ULL,
      0x9fe76702af69334bULL,
      0x1fffe18a1b336103ULL,
      0x8941e71cff8a78dbULL,
      0x382ae548b2e4f3f3ULL,
      0xabbedea680056f52ULL}},
    {{
      0x6bcaa4cd81f32d1bULL,
      0xdea2594ac06fd85dULL,
      0xefbacd1d7d476e98ULL,
      0x8a1d71efea48b9caULL,
      0x2001802114846679ULL,
      0xd8fa6bbbebab0761ULL,
      0x3002c6cd635afe94ULL,
      0x7bcd9ed0efc889fbULL}},
    {{
      0x48bc924af11bd720ULL,
      0xfaf417d5d9b21b99ULL,
      0xe71da4aa88e12852ULL,
      0x5d80ef9d1891cc86ULL,
      0xf82012d430219f9bULL,
      0xcda43c32bcdf1d77ULL,
      0xd21380b00449b17aULL,
      0x378ee767f11631baULL}}
};
#else
ALIGN(16)
static const union uint512_u C[12] = {
    {{
      0x0745a6f2596580ddULL,
      0x234d74cc36747605ULL,
      0x15d360a4082a42a2ULL,
      0x0169679291e07c4bULL,
      0xfcc485758db84e71ULL,
      0x16d0452e43766a2fULL,
      0x1f7c65c0812fcbebULL,
      0xe9daca1eda5b08b1ULL}},
    {{
      0xb79bb121700479e6ULL,
      0x56cdcbd71ba2dd55ULL,
      0xcaa70adbc261b55cULL,
      0x5899d6126b17b59aULL,
      0x3101b5160f5ed561ULL,
      0x982b230a72eafef3ULL,
      0xd7b5700f469de34fULL,
      0x1a2f9da98ab5a36fULL}},
    {{
      0xb20aba0af5961e99ULL,
      0x31db7a8643f4b6c2ULL,
      0x09db6260373ac9c1ULL,
      0xb19e3590e40fe2d3ULL,
      0x7b7b29b11475eaf2ULL,
      0x8b1f9c525f5ef106ULL,
      0x35843d6a28fc390aULL,
      0xc72fce2bacdc74f5ULL}},
    {{
      0x2ed1e384bcbe0c22ULL,
      0xf137e893a1ea5334ULL,
      0xbe0352933313b7d8ULL,
      0x75d603ed822cd7a9ULL,
      0x3f355e68ad1c729dULL,
      0x7d3c5c337e858e48ULL,
      0xdde4715da0e148f9ULL,
      0xd26615e8b3df1fefULL}},
    {{
      0x57fe6c7cfd581760ULL,
      0xf563eaa97ea2567aULL,
      0x161a2723b700ffdfULL,
      0xa3f53a254717cdbfULL,
      0xbdff0f80d7359e35ULL,
      0x4a1086161f1c157fULL,
      0x6323a96c0c413f9aULL,
      0x994747adac6bea4bULL}},
    {{
      0x6e7d64467a4068faULL,
      0x354f903672c571bfULL,
      0xb6c6bec2661ff20aULL,
      0xb4b79a1cb7a6facfULL,
      0xc68ef09ab49a7f18ULL,
      0x6ca44251f9c4662dULL,
      0xc039307a3bc3a46fULL,
      0xd9d33a1daeae4faeULL}},
    {{
      0x93d4143a4d568688ULL,
      0xf34a3ca24c451735ULL,
      0x04054a2883694706ULL,
      0x372c822dc5ab9209ULL,
      0xc9937a19333e47d3ULL,
      0xc987bfe6c7c69e39ULL,
      0x540924bffe86ac51ULL,
      0xecc5aaee160ec7f4ULL}},
    {{
      0x1ee702bfd40d7fa4ULL,
      0xd9a8515935c2ac36ULL,
      0x2fc4a5d12b8dd169ULL,
      0x90069b92cb2b89f4ULL,
      0x9ac4db4d3b44b489ULL,
      0x1ede369c71f8b74eULL,
      0x41416e0c02aae703ULL,
      0xa7c9934d425b1f9bULL}},
    {{
      0xdb5a238351446172ULL,
      0x602a1fcb92dc380eULL,
      0x549c07a69a8a2b7bULL,
      0xb1ceb2db0b440a80ULL,
      0x84090de0b755d93cULL,
      0x244289251b3a7d3aULL,
      0xde5f16ecd89a4c94ULL,
      0x9b223116545a8f37ULL}},
    {{
      0xed9c4598fbc7b474ULL,
      0xc3b63b15d1fa9836ULL,
      0xf452763b306c1e7aULL,
      0x4b3369af0267e79fULL,
      0x0361331b8ae1ff1fULL,
      0xdb788aff1ce74189ULL,
      0xf3f3e4b248e52a38ULL,
      0x526f0580a6debeabULL}},
    {{
      0x1b2df381cda4ca6bULL,
      0x5dd86fc04a59a2deULL,
      0x986e477d1dcdbaefULL,
      0xcab948eaef711d8aULL,
      0x7966841421800120ULL,
      0x6107abebbb6bfad8ULL,
      0x94fe5a63cdc60230ULL,
      0xfb89c8efd09ecd7bULL}},
    {{
      0x20d71bf14a92bc48ULL,
      0x991bb2d9d517f4faULL,
      0x5228e188aaa41de7ULL,
      0x86cc91189def805dULL,
      0x9b9f2130d41220f8ULL,
      0x771ddfbc323ca4cdULL,
      0x7ab14904b08013d2ULL,
      0xba3116f167e78e37ULL}}
};
#endif

#ifndef __GOST3411_BIG_ENDIAN__
static const unsigned long long A[64] = {
    0x8e20faa72ba0b470ULL, 0x47107ddd9b505a38ULL, 0xad08b0e0c3282d1cULL,
    0xd8045870ef14980eULL, 0x6c022c38f90a4c07ULL, 0x3601161cf205268dULL,
    0x1b8e0b0e798c13c8ULL, 0x83478b07b2468764ULL, 0xa011d380818e8f40ULL,
    0x5086e740ce47c920ULL, 0x2843fd2067adea10ULL, 0x14aff010bdd87508ULL,
    0x0ad97808d06cb404ULL, 0x05e23c0468365a02ULL, 0x8c711e02341b2d01ULL,
    0x46b60f011a83988eULL, 0x90dab52a387ae76fULL, 0x486dd4151c3dfdb9ULL,
    0x24b86a840e90f0d2ULL, 0x125c354207487869ULL, 0x092e94218d243cbaULL,
    0x8a174a9ec8121e5dULL, 0x4585254f64090fa0ULL, 0xaccc9ca9328a8950ULL,
    0x9d4df05d5f661451ULL, 0xc0a878a0a1330aa6ULL, 0x60543c50de970553ULL,
    0x302a1e286fc58ca7ULL, 0x18150f14b9ec46ddULL, 0x0c84890ad27623e0ULL,
    0x0642ca05693b9f70ULL, 0x0321658cba93c138ULL, 0x86275df09ce8aaa8ULL,
    0x439da0784e745554ULL, 0xafc0503c273aa42aULL, 0xd960281e9d1d5215ULL,
    0xe230140fc0802984ULL, 0x71180a8960409a42ULL, 0xb60c05ca30204d21ULL,
    0x5b068c651810a89eULL, 0x456c34887a3805b9ULL, 0xac361a443d1c8cd2ULL,
    0x561b0d22900e4669ULL, 0x2b838811480723baULL, 0x9bcf4486248d9f5dULL,
    0xc3e9224312c8c1a0ULL, 0xeffa11af0964ee50ULL, 0xf97d86d98a327728ULL,
    0xe4fa2054a80b329cULL, 0x727d102a548b194eULL, 0x39b008152acb8227ULL,
    0x9258048415eb419dULL, 0x492c024284fbaec0ULL, 0xaa16012142f35760ULL,
    0x550b8e9e21f7a530ULL, 0xa48b474f9ef5dc18ULL, 0x70a6a56e2440598eULL,
    0x3853dc371220a247ULL, 0x1ca76e95091051adULL, 0x0edd37c48a08a6d8ULL,
    0x07e095624504536cULL, 0x8d70c431ac02a736ULL, 0xc83862965601dd1bULL,
    0x641c314b2b8ee083ULL
};
#else
static const unsigned long long A[64] = {
    0x70b4a02ba7fa208eULL, 0x385a509bdd7d1047ULL, 0x1c2d28c3e0b008adULL,
    0x0e9814ef705804d8ULL, 0x074c0af9382c026cULL, 0x8d2605f21c160136ULL,
    0xc8138c790e0b8e1bULL, 0x648746b2078b4783ULL, 0x408f8e8180d311a0ULL,
    0x20c947ce40e78650ULL, 0x10eaad6720fd4328ULL, 0x0875d8bd10f0af14ULL,
    0x04b46cd00878d90aULL, 0x025a3668043ce205ULL, 0x012d1b34021e718cULL,
    0x8e98831a010fb646ULL, 0x6fe77a382ab5da90ULL, 0xb9fd3d1c15d46d48ULL,
    0xd2f0900e846ab824ULL, 0x6978480742355c12ULL, 0xba3c248d21942e09ULL,
    0x5d1e12c89e4a178aULL, 0xa00f09644f258545ULL, 0x50898a32a99cccacULL,
    0x5114665f5df04d9dULL, 0xa60a33a1a078a8c0ULL, 0x530597de503c5460ULL,
    0xa78cc56f281e2a30ULL, 0xdd46ecb9140f1518ULL, 0xe02376d20a89840cULL,
    0x709f3b6905ca4206ULL, 0x38c193ba8c652103ULL, 0xa8aae89cf05d2786ULL,
    0x5455744e78a09d43ULL, 0x2aa43a273c50c0afULL, 0x15521d9d1e2860d9ULL,
    0x842980c00f1430e2ULL, 0x429a4060890a1871ULL, 0x214d2030ca050cb6ULL,
    0x9ea81018658c065bULL, 0xb905387a88346c45ULL, 0xd28c1c3d441a36acULL,
    0x69460e90220d1b56ULL, 0xba2307481188832bULL, 0x5d9f8d248644cf9bULL,
    0xa0c1c8124322e9c3ULL, 0x50ee6409af11faefULL, 0x2877328ad9867df9ULL,
    0x9c320ba85420fae4ULL, 0x4e198b542a107d72ULL, 0x2782cb2a1508b039ULL,
    0x9d41eb1584045892ULL, 0xc0aefb8442022c49ULL, 0x6057f342210116aaULL,
    0x30a5f7219e8e0b55ULL, 0x18dcf59e4f478ba4ULL, 0x8e5940246ea5a670ULL,
    0x47a2201237dc5338ULL, 0xad511009956ea71cULL, 0xd8a6088ac437dd0eULL,
    0x6c5304456295e007ULL, 0x36a702ac31c4708dULL, 0x1bdd0156966238c8ULL,
    0x83e08e2b4b311c64ULL
};
#endif

static const unsigned char Tau[64] = {
    0, 8, 16, 24, 32, 40, 48, 56,
    1, 9, 17, 25, 33, 41, 49, 57,
    2, 10, 18, 26, 34, 42, 50, 58,
    3, 11, 19, 27, 35, 43, 51, 59,
    4, 12, 20, 28, 36, 44, 52, 60,
    5, 13, 21, 29, 37, 45, 53, 61,
    6, 14, 22, 30, 38, 46, 54, 62,
    7, 15, 23, 31, 39, 47, 55, 63
};

static const unsigned char Pi[256] = {
    252, 238, 221, 17, 207, 110, 49, 22,
    251, 196, 250, 218, 35, 197, 4, 77,
    233, 119, 240, 219, 147, 46, 153, 186,
    23, 54, 241, 187, 20, 205, 95, 193,
    249, 24, 101, 90, 226, 92, 239, 33,
    129, 28, 60, 66, 139, 1, 142, 79,
    5, 132, 2, 174, 227, 106, 143, 160,
    6, 11, 237, 152, 127, 212, 211, 31,
    235, 52, 44, 81, 234, 200, 72, 171,
    242, 42, 104, 162, 253, 58, 206, 204,
    181, 112, 14, 86, 8, 12, 118, 18,
    191, 114, 19, 71, 156, 183, 93, 135,
    21, 161, 150, 41, 16, 123, 154, 199,
    243, 145, 120, 111, 157, 158, 178, 177,
    50, 117, 25, 61, 255, 53, 138, 126,
    109, 84, 198, 128, 195, 189, 13, 87,
    223, 245, 36, 169, 62, 168, 67, 201,
    215, 121, 214, 246, 124, 34, 185, 3,
    224, 15, 236, 222, 122, 148, 176, 188,
    220, 232, 40, 80, 78, 51, 10, 74,
    167, 151, 96, 115, 30, 0, 98, 68,
    26, 184, 56, 130, 100, 159, 38, 65,
    173, 69, 70, 146, 39, 94, 85, 47,
    140, 163, 165, 125, 105, 213, 149, 59,
    7, 88, 179, 64, 134, 172, 29, 247,
    48, 55, 107, 228, 136, 217, 231, 137,
    225, 27, 131, 73, 76, 63, 248, 254,
    141, 83, 170, 144, 202, 216, 133, 97,
    32, 113, 103, 164, 45, 43, 9, 91,
    203, 155, 37, 208, 190, 229, 108, 82,
    89, 166, 116, 210, 230, 244, 180, 192,
    209, 102, 175, 194, 57, 75, 99, 182
};
