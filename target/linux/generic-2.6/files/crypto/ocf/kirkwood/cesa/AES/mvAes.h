/* mvAes.h   v2.0   August '99
 * Reference ANSI C code
 */

/*  AES Cipher header file for ANSI C Submissions
      Lawrence E. Bassham III
      Computer Security Division
      National Institute of Standards and Technology

      April 15, 1998

    This sample is to assist implementers developing to the Cryptographic 
API Profile for AES Candidate Algorithm Submissions.  Please consult this 
document as a cross-reference.

    ANY CHANGES, WHERE APPROPRIATE, TO INFORMATION PROVIDED IN THIS FILE
MUST BE DOCUMENTED.  CHANGES ARE ONLY APPROPRIATE WHERE SPECIFIED WITH
THE STRING "CHANGE POSSIBLE".  FUNCTION CALLS AND THEIR PARAMETERS CANNOT 
BE CHANGED.  STRUCTURES CAN BE ALTERED TO ALLOW IMPLEMENTERS TO INCLUDE 
IMPLEMENTATION SPECIFIC INFORMATION.
*/

/*  Includes:
	Standard include files
*/

#include "mvOs.h"


/*  Error Codes - CHANGE POSSIBLE: inclusion of additional error codes  */

/*  Key direction is invalid, e.g., unknown value */
#define     AES_BAD_KEY_DIR        -1  

/*  Key material not of correct length */
#define     AES_BAD_KEY_MAT        -2  

/*  Key passed is not valid  */
#define     AES_BAD_KEY_INSTANCE   -3  

/*  Params struct passed to cipherInit invalid */
#define     AES_BAD_CIPHER_MODE    -4  

/*  Cipher in wrong state (e.g., not initialized) */
#define     AES_BAD_CIPHER_STATE   -5  

#define     AES_BAD_CIPHER_INSTANCE   -7 


/*  Function protoypes  */
/*  CHANGED: makeKey(): parameter blockLen added
                        this parameter is absolutely necessary if you want to
			setup the round keys in a variable block length setting 
	     cipherInit(): parameter blockLen added (for obvious reasons)		
 */
int     aesMakeKey(MV_U8 *expandedKey, MV_U8 *keyMaterial, int keyLen, int blockLen);
int     aesBlockEncrypt128(MV_U8 mode, MV_U8 *IV, MV_U8 *expandedKey, int  keyLen, 
                    MV_U32 *plain, int numBlocks, MV_U32 *cipher);
int     aesBlockDecrypt128(MV_U8 mode, MV_U8 *IV, MV_U8 *expandedKey, int  keyLen, 
                    MV_U32 *plain, int numBlocks, MV_U32 *cipher);


