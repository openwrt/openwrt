/* rijndael-api-ref.c   v2.1   April 2000
 * Reference ANSI C code
 * authors: v2.0 Paulo Barreto
 *               Vincent Rijmen, K.U.Leuven
 *          v2.1 Vincent Rijmen, K.U.Leuven
 *
 * This code is placed in the public domain.
 */
#include "mvOs.h"

#include "mvAes.h"
#include "mvAesAlg.h"


/*  Defines:
	Add any additional defines you need
*/

#define     MODE_ECB        1    /*  Are we ciphering in ECB mode?   */
#define     MODE_CBC        2    /*  Are we ciphering in CBC mode?   */
#define     MODE_CFB1       3    /*  Are we ciphering in 1-bit CFB mode? */


int     aesMakeKey(MV_U8 *expandedKey, MV_U8 *keyMaterial, int keyLen, int blockLen)
{
    MV_U8   W[MAXROUNDS+1][4][MAXBC];
	MV_U8   k[4][MAXKC];
    MV_U8   j;
	int     i, rounds, KC;
	
	if (expandedKey == NULL) 
    {
		return AES_BAD_KEY_INSTANCE;
	}

	if (!((keyLen == 128) || (keyLen == 192) || (keyLen == 256))) 
    { 
		return AES_BAD_KEY_MAT;
	}

	if (keyMaterial == NULL) 
    {
		return AES_BAD_KEY_MAT;
	}

	/* initialize key schedule: */ 
 	for(i=0; i<keyLen/8; i++) 
    {
		j = keyMaterial[i];
		k[i % 4][i / 4] = j; 
	}	
	
	rijndaelKeySched (k, keyLen, blockLen, W);
#ifdef MV_AES_DEBUG
    {
        MV_U8*  pW = &W[0][0][0];
        int     x;

        mvOsPrintf("Expended Key: size = %d\n", sizeof(W));
        for(i=0; i<sizeof(W); i++)
        {
            mvOsPrintf("%02x ", pW[i]);
        }
        for(i=0; i<MAXROUNDS+1; i++)
        {
            mvOsPrintf("\n Round #%02d: ", i);
            for(x=0; x<MAXBC; x++)
            {
                mvOsPrintf("%02x%02x%02x%02x ", 
                    W[i][0][x], W[i][1][x], W[i][2][x], W[i][3][x]);
            }
            mvOsPrintf("\n");
        }
    }
#endif /* MV_AES_DEBUG */
  	switch (keyLen) 
    {
	    case 128: 
            rounds = 10;
            KC = 4; 
            break;
	    case 192: 
            rounds = 12;
            KC = 6; 
            break;
	    case 256: 
            rounds = 14;
            KC = 8; 
            break;
	    default : 
            return (-1);
	}

    for(i=0; i<MAXBC; i++)
    {
        for(j=0; j<4; j++)
        {
            expandedKey[i*4+j] = W[rounds][j][i];
        }
    }
    for(; i<KC; i++)
    {
        for(j=0; j<4; j++)
        {
            expandedKey[i*4+j] = W[rounds-1][j][i+MAXBC-KC];
        }
    }

	
	return 0;
}

int     aesBlockEncrypt128(MV_U8 mode, MV_U8 *IV, MV_U8 *expandedKey, int  keyLen, 
                        MV_U32 *plain, int numBlocks, MV_U32 *cipher)
{
	int     i, j, t;
	MV_U8   block[4][MAXBC];
    int     rounds;
    char    *input, *outBuffer;

    input = (char*)plain;
    outBuffer = (char*)cipher;

        /* check parameter consistency: */
    if( (expandedKey == NULL) || ((keyLen != 128) && (keyLen != 192) && (keyLen != 256))) 
    {
        return AES_BAD_KEY_MAT;
    }
    if ((mode != MODE_ECB && mode != MODE_CBC))
    {
        return AES_BAD_CIPHER_STATE;
    }

	switch (keyLen) 
    {
	    case 128: rounds = 10; break;
	    case 192: rounds = 12; break;
	    case 256: rounds = 14; break;
	    default : return (-3); /* this cannot happen */
	}

	
	switch (mode) 
    {
	    case MODE_ECB: 
		    for (i = 0; i < numBlocks; i++) 
            {
			    for (j = 0; j < 4; j++) 
                {
				    for(t = 0; t < 4; t++)
				        /* parse input stream into rectangular array */
					    block[t][j] = input[16*i+4*j+t] & 0xFF;
			    }                           
			    rijndaelEncrypt128(block, (MV_U8 (*)[4][MAXBC])expandedKey, rounds);
			    for (j = 0; j < 4; j++) 
                {           
				    /* parse rectangular array into output ciphertext bytes */
				    for(t = 0; t < 4; t++)
                        outBuffer[16*i+4*j+t] = (MV_U8) block[t][j];

			    }
		    }
		    break;
		
	    case MODE_CBC:
		    for (j = 0; j < 4; j++) 
            {
			    for(t = 0; t < 4; t++)
			    /* parse initial value into rectangular array */
					block[t][j] = IV[t+4*j] & 0xFF;
			}
		    for (i = 0; i < numBlocks; i++) 
            {
			    for (j = 0; j < 4; j++) 
                {
				    for(t = 0; t < 4; t++)
				        /* parse input stream into rectangular array and exor with 
				        IV or the previous ciphertext */
					    block[t][j] ^= input[16*i+4*j+t] & 0xFF;
			    }
			    rijndaelEncrypt128(block, (MV_U8 (*)[4][MAXBC])expandedKey, rounds);
			    for (j = 0; j < 4; j++) 
                {
				    /* parse rectangular array into output ciphertext bytes */
				    for(t = 0; t < 4; t++)
					    outBuffer[16*i+4*j+t] = (MV_U8) block[t][j];
			    }
		    }
		    break;
	
	    default: return AES_BAD_CIPHER_STATE;
	}
	
	return 0;
}

int     aesBlockDecrypt128(MV_U8 mode, MV_U8 *IV, MV_U8 *expandedKey, int  keyLen, 
                            MV_U32 *srcData, int numBlocks, MV_U32 *dstData)
{
	int     i, j, t;
	MV_U8   block[4][MAXBC];
    MV_U8   iv[4][MAXBC];    
    int     rounds;
    char    *input, *outBuffer;

    input = (char*)srcData;
    outBuffer = (char*)dstData;

    if (expandedKey == NULL) 
    {
		return AES_BAD_KEY_MAT;
	}

    /* check parameter consistency: */
    if (keyLen != 128 && keyLen != 192 && keyLen != 256)
    {
        return AES_BAD_KEY_MAT;
    }
    if ((mode != MODE_ECB && mode != MODE_CBC)) 
    {
        return AES_BAD_CIPHER_STATE;
    }

	switch (keyLen) 
    {
	    case 128: rounds = 10; break;
	    case 192: rounds = 12; break;
	    case 256: rounds = 14; break;
	    default : return (-3); /* this cannot happen */
	}

	
	switch (mode) 
    {
	    case MODE_ECB: 
		    for (i = 0; i < numBlocks; i++) 
            {
			    for (j = 0; j < 4; j++) 
                {
				    for(t = 0; t < 4; t++)
                    {
				        /* parse input stream into rectangular array */
					    block[t][j] = input[16*i+4*j+t] & 0xFF;
                    }
			    }
			    rijndaelDecrypt128(block, (MV_U8 (*)[4][MAXBC])expandedKey, rounds);
			    for (j = 0; j < 4; j++) 
                {
				    /* parse rectangular array into output ciphertext bytes */
				    for(t = 0; t < 4; t++)
					    outBuffer[16*i+4*j+t] = (MV_U8) block[t][j];
			    }
		    }
		    break;
		
	    case MODE_CBC:
		    /* first block */
		    for (j = 0; j < 4; j++) 
            {
			    for(t = 0; t < 4; t++)
                {
			        /* parse input stream into rectangular array */
				    block[t][j] = input[4*j+t] & 0xFF;
                    iv[t][j] = block[t][j];
                }
		    }
		    rijndaelDecrypt128(block, (MV_U8 (*)[4][MAXBC])expandedKey, rounds);
		
		    for (j = 0; j < 4; j++) 
            {
			    /* exor the IV and parse rectangular array into output ciphertext bytes */
			    for(t = 0; t < 4; t++)
                {
				    outBuffer[4*j+t] = (MV_U8) (block[t][j] ^ IV[t+4*j]);
                    IV[t+4*j] = iv[t][j];
                }
		    }
		
		    /* next blocks */
		    for (i = 1; i < numBlocks; i++) 
            {
			    for (j = 0; j < 4; j++) 
                {
				    for(t = 0; t < 4; t++)
                    {
				        /* parse input stream into rectangular array */
                        iv[t][j] = input[16*i+4*j+t] & 0xFF;
					    block[t][j] = iv[t][j];
                    }
			    }
			    rijndaelDecrypt128(block, (MV_U8 (*)[4][MAXBC])expandedKey, rounds);
			
			    for (j = 0; j < 4; j++) 
                {
				    /* exor previous ciphertext block and parse rectangular array 
				       into output ciphertext bytes */
				    for(t = 0; t < 4; t++)
                    {
					    outBuffer[16*i+4*j+t] = (MV_U8) (block[t][j] ^ IV[t+4*j]);
                        IV[t+4*j] = iv[t][j];
                    }
			    }
		    }
		    break;
	
	    default: return AES_BAD_CIPHER_STATE;
	}
	
	return 0;
}


