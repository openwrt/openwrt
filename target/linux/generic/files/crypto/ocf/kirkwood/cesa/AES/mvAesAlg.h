/* rijndael-alg-ref.h   v2.0   August '99
 * Reference ANSI C code
 * authors: Paulo Barreto
 *          Vincent Rijmen, K.U.Leuven
 */
#ifndef __RIJNDAEL_ALG_H
#define __RIJNDAEL_ALG_H

#define MAXBC				(128/32)
#define MAXKC				(256/32)
#define MAXROUNDS			14


int rijndaelKeySched (MV_U8 k[4][MAXKC], int keyBits, int blockBits, MV_U8 rk[MAXROUNDS+1][4][MAXBC]);

int rijndaelEncrypt128(MV_U8 a[4][MAXBC], MV_U8 rk[MAXROUNDS+1][4][MAXBC], int rounds);
int rijndaelDecrypt128(MV_U8 a[4][MAXBC], MV_U8 rk[MAXROUNDS+1][4][MAXBC], int rounds);

#endif /* __RIJNDAEL_ALG_H */
