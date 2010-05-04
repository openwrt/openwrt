/* rijndael-alg-ref.c   v2.0   August '99
 * Reference ANSI C code
 * authors: Paulo Barreto
 *          Vincent Rijmen, K.U.Leuven
 *
 * This code is placed in the public domain.
 */

#include "mvOs.h"

#include "mvAesAlg.h"

#include "mvAesBoxes.dat"


MV_U8 mul1(MV_U8 aa, MV_U8 bb);
void KeyAddition(MV_U8 a[4][MAXBC], MV_U8 rk[4][MAXBC], MV_U8 BC);
void ShiftRow128Enc(MV_U8 a[4][MAXBC]);
void ShiftRow128Dec(MV_U8 a[4][MAXBC]);
void Substitution(MV_U8 a[4][MAXBC], MV_U8 box[256]);
void MixColumn(MV_U8 a[4][MAXBC], MV_U8 rk[4][MAXBC]);
void InvMixColumn(MV_U8 a[4][MAXBC]);


#define mul(aa, bb) (mask[bb] & Alogtable[aa + Logtable[bb]])
                         
MV_U8 mul1(MV_U8 aa, MV_U8 bb)
{
    return mask[bb] & Alogtable[aa + Logtable[bb]];
}


void KeyAddition(MV_U8 a[4][MAXBC], MV_U8 rk[4][MAXBC], MV_U8 BC) 
{
	/* Exor corresponding text input and round key input bytes
	 */
    ((MV_U32*)(&(a[0][0])))[0] ^= ((MV_U32*)(&(rk[0][0])))[0];
    ((MV_U32*)(&(a[1][0])))[0] ^= ((MV_U32*)(&(rk[1][0])))[0];
    ((MV_U32*)(&(a[2][0])))[0] ^= ((MV_U32*)(&(rk[2][0])))[0];
    ((MV_U32*)(&(a[3][0])))[0] ^= ((MV_U32*)(&(rk[3][0])))[0];

}

void ShiftRow128Enc(MV_U8 a[4][MAXBC]) {
	/* Row 0 remains unchanged
	 * The other three rows are shifted a variable amount
	 */
	MV_U8 tmp[MAXBC];
	
    tmp[0] = a[1][1];
    tmp[1] = a[1][2];
    tmp[2] = a[1][3];
    tmp[3] = a[1][0];

    ((MV_U32*)(&(a[1][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
     /*
    a[1][0] = tmp[0];
    a[1][1] = tmp[1];
    a[1][2] = tmp[2];
    a[1][3] = tmp[3];
       */
    tmp[0] = a[2][2];
    tmp[1] = a[2][3];
    tmp[2] = a[2][0];
    tmp[3] = a[2][1];

    ((MV_U32*)(&(a[2][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
      /*
    a[2][0] = tmp[0];
    a[2][1] = tmp[1];
    a[2][2] = tmp[2];
    a[2][3] = tmp[3];
    */
    tmp[0] = a[3][3];
    tmp[1] = a[3][0];
    tmp[2] = a[3][1];
    tmp[3] = a[3][2];

    ((MV_U32*)(&(a[3][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
    /*
    a[3][0] = tmp[0];
    a[3][1] = tmp[1];
    a[3][2] = tmp[2];
    a[3][3] = tmp[3];
    */
}  

void ShiftRow128Dec(MV_U8 a[4][MAXBC]) {
	/* Row 0 remains unchanged
	 * The other three rows are shifted a variable amount
	 */
   	MV_U8 tmp[MAXBC];
	
    tmp[0] = a[1][3];
    tmp[1] = a[1][0];
    tmp[2] = a[1][1];
    tmp[3] = a[1][2];

    ((MV_U32*)(&(a[1][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
    /*
    a[1][0] = tmp[0];
    a[1][1] = tmp[1];
    a[1][2] = tmp[2];
    a[1][3] = tmp[3];
    */

    tmp[0] = a[2][2];
    tmp[1] = a[2][3];
    tmp[2] = a[2][0];
    tmp[3] = a[2][1];

    ((MV_U32*)(&(a[2][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
    /*
    a[2][0] = tmp[0];
    a[2][1] = tmp[1];
    a[2][2] = tmp[2];
    a[2][3] = tmp[3];
    */

    tmp[0] = a[3][1];
    tmp[1] = a[3][2];
    tmp[2] = a[3][3];
    tmp[3] = a[3][0];

    ((MV_U32*)(&(a[3][0])))[0] = ((MV_U32*)(&(tmp[0])))[0];
    /*
    a[3][0] = tmp[0];
    a[3][1] = tmp[1];
    a[3][2] = tmp[2];
    a[3][3] = tmp[3];
    */
}  

void Substitution(MV_U8 a[4][MAXBC], MV_U8 box[256]) {
	/* Replace every byte of the input by the byte at that place
	 * in the nonlinear S-box
	 */
	int i, j;
	
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++) a[i][j] = box[a[i][j]] ;
}
   
void MixColumn(MV_U8 a[4][MAXBC], MV_U8 rk[4][MAXBC]) {
        /* Mix the four bytes of every column in a linear way
	 */
	MV_U8 b[4][MAXBC];
	int i, j;
		
	for(j = 0; j < 4; j++){
        b[0][j] = mul(25,a[0][j]) ^ mul(1,a[1][j]) ^ a[2][j] ^ a[3][j];
        b[1][j] = mul(25,a[1][j]) ^ mul(1,a[2][j]) ^ a[3][j] ^ a[0][j];
        b[2][j] = mul(25,a[2][j]) ^ mul(1,a[3][j]) ^ a[0][j] ^ a[1][j];
        b[3][j] = mul(25,a[3][j]) ^ mul(1,a[0][j]) ^ a[1][j] ^ a[2][j];
    }
	for(i = 0; i < 4; i++)
		/*for(j = 0; j < BC; j++) a[i][j] = b[i][j];*/
        ((MV_U32*)(&(a[i][0])))[0] = ((MV_U32*)(&(b[i][0])))[0] ^ ((MV_U32*)(&(rk[i][0])))[0];;
}

void InvMixColumn(MV_U8 a[4][MAXBC]) {
        /* Mix the four bytes of every column in a linear way
	 * This is the opposite operation of Mixcolumn
	 */
	MV_U8 b[4][MAXBC];
	int i, j;
	
	for(j = 0; j < 4; j++){
        b[0][j] = mul(223,a[0][j]) ^ mul(104,a[1][j]) ^ mul(238,a[2][j]) ^ mul(199,a[3][j]);
        b[1][j] = mul(223,a[1][j]) ^ mul(104,a[2][j]) ^ mul(238,a[3][j]) ^ mul(199,a[0][j]);
        b[2][j] = mul(223,a[2][j]) ^ mul(104,a[3][j]) ^ mul(238,a[0][j]) ^ mul(199,a[1][j]);
        b[3][j] = mul(223,a[3][j]) ^ mul(104,a[0][j]) ^ mul(238,a[1][j]) ^ mul(199,a[2][j]);
    }
	for(i = 0; i < 4; i++)
		/*for(j = 0; j < BC; j++) a[i][j] = b[i][j];*/
        ((MV_U32*)(&(a[i][0])))[0] = ((MV_U32*)(&(b[i][0])))[0];
}

int rijndaelKeySched (MV_U8 k[4][MAXKC], int keyBits, int blockBits, MV_U8 W[MAXROUNDS+1][4][MAXBC]) 
{
	/* Calculate the necessary round keys
	 * The number of calculations depends on keyBits and blockBits
	 */
	int KC, BC, ROUNDS;
	int i, j, t, rconpointer = 0;
	MV_U8 tk[4][MAXKC];   

	switch (keyBits) {
	case 128: KC = 4; break;
	case 192: KC = 6; break;
	case 256: KC = 8; break;
	default : return (-1);
	}

	switch (blockBits) {
	case 128: BC = 4; break;
	case 192: BC = 6; break;
	case 256: BC = 8; break;
	default : return (-2);
	}

	switch (keyBits >= blockBits ? keyBits : blockBits) {
	case 128: ROUNDS = 10; break;
	case 192: ROUNDS = 12; break;
	case 256: ROUNDS = 14; break;
	default : return (-3); /* this cannot happen */
	}

	
	for(j = 0; j < KC; j++)
		for(i = 0; i < 4; i++)
			tk[i][j] = k[i][j];
	t = 0;
	/* copy values into round key array */
	for(j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		for(i = 0; i < 4; i++) W[t / BC][i][t % BC] = tk[i][j];
		
	while (t < (ROUNDS+1)*BC) { /* while not enough round key material calculated */
		/* calculate new values */
		for(i = 0; i < 4; i++)
			tk[i][0] ^= S[tk[(i+1)%4][KC-1]];
		tk[0][0] ^= rcon[rconpointer++];

		if (KC != 8)
			for(j = 1; j < KC; j++)
				for(i = 0; i < 4; i++) tk[i][j] ^= tk[i][j-1];
		else {
			for(j = 1; j < KC/2; j++)
				for(i = 0; i < 4; i++) tk[i][j] ^= tk[i][j-1];
			for(i = 0; i < 4; i++) tk[i][KC/2] ^= S[tk[i][KC/2 - 1]];
			for(j = KC/2 + 1; j < KC; j++)
				for(i = 0; i < 4; i++) tk[i][j] ^= tk[i][j-1];
	}
	/* copy values into round key array */
	for(j = 0; (j < KC) && (t < (ROUNDS+1)*BC); j++, t++)
		for(i = 0; i < 4; i++) W[t / BC][i][t % BC] = tk[i][j];
	}		

	return 0;
}
      
        

int rijndaelEncrypt128(MV_U8 a[4][MAXBC], MV_U8 rk[MAXROUNDS+1][4][MAXBC], int rounds)
{
	/* Encryption of one block. 
	 */
	int r, BC, ROUNDS;

    BC = 4;
    ROUNDS = rounds;

	/* begin with a key addition
	 */

	KeyAddition(a,rk[0],BC); 

    /* ROUNDS-1 ordinary rounds
	 */
	for(r = 1; r < ROUNDS; r++) {
		Substitution(a,S);
		ShiftRow128Enc(a);
		MixColumn(a, rk[r]);
		/*KeyAddition(a,rk[r],BC);*/
	}
	
	/* Last round is special: there is no MixColumn
	 */
	Substitution(a,S);
	ShiftRow128Enc(a);
	KeyAddition(a,rk[ROUNDS],BC);

	return 0;
}   


int rijndaelDecrypt128(MV_U8 a[4][MAXBC], MV_U8 rk[MAXROUNDS+1][4][MAXBC], int rounds)
{
	int r, BC, ROUNDS;
	
    BC = 4;
    ROUNDS = rounds;

	/* To decrypt: apply the inverse operations of the encrypt routine,
	 *             in opposite order
	 * 
	 * (KeyAddition is an involution: it 's equal to its inverse)
	 * (the inverse of Substitution with table S is Substitution with the inverse table of S)
	 * (the inverse of Shiftrow is Shiftrow over a suitable distance)
	 */

        /* First the special round:
	 *   without InvMixColumn
	 *   with extra KeyAddition
	 */
	KeyAddition(a,rk[ROUNDS],BC);
    ShiftRow128Dec(a);               
	Substitution(a,Si);
	
	/* ROUNDS-1 ordinary rounds
	 */
	for(r = ROUNDS-1; r > 0; r--) {
		KeyAddition(a,rk[r],BC);
		InvMixColumn(a);      
		ShiftRow128Dec(a);               
		Substitution(a,Si);

	}
	
	/* End with the extra key addition
	 */
	
	KeyAddition(a,rk[0],BC);    

	return 0;
}

