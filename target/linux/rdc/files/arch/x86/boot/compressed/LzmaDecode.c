/*
  LzmaDecode.c
  LZMA Decoder (optimized for Speed version)
  
  LZMA SDK 4.17 Copyright (c) 1999-2005 Igor Pavlov (2005-04-05)
  http://www.7-zip.org/

  LZMA SDK is licensed under two licenses:
  1) GNU Lesser General Public License (GNU LGPL)
  2) Common Public License (CPL)
  It means that you can select one of these two licenses and 
  follow rules of that license.

  SPECIAL EXCEPTION:
  Igor Pavlov, as the author of this Code, expressly permits you to 
  statically or dynamically link your Code (or bind by name) to the 
  interfaces of this file without subjecting your linked Code to the 
  terms of the CPL or GNU LGPL. Any modifications or additions 
  to this file, however, are subject to the LGPL or CPL terms.
*/

#include "LzmaDecode.h"

#ifndef Byte
#define Byte unsigned char
#endif

#define kNumTopBits 24
#define kTopValue ((UInt32)1 << kNumTopBits)

#define kNumBitModelTotalBits 11
#define kBitModelTotal (1 << kNumBitModelTotalBits)
#define kNumMoveBits 5

#define RC_READ_BYTE (*Buffer++)

#define RC_INIT2 Code = 0; Range = 0xFFFFFFFF; \
  { int i; for(i = 0; i < 5; i++) { RC_TEST; Code = (Code << 8) | RC_READ_BYTE; }}

#ifdef _LZMA_IN_CB

#define RC_TEST { if (Buffer == BufferLim) \
  { UInt32 size; int result = InCallback->Read(InCallback, &Buffer, &size); if (result != LZMA_RESULT_OK) return result; \
  BufferLim = Buffer + size; if (size == 0) return LZMA_RESULT_DATA_ERROR; }}

#define RC_INIT Buffer = BufferLim = 0; RC_INIT2

#else

#define RC_TEST { if (Buffer == BufferLim) return LZMA_RESULT_DATA_ERROR; }

#define RC_INIT(buffer, bufferSize) Buffer = buffer; BufferLim = buffer + bufferSize; RC_INIT2
 
#endif

#define RC_NORMALIZE if (Range < kTopValue) { RC_TEST; Range <<= 8; Code = (Code << 8) | RC_READ_BYTE; }

#define IfBit0(p) RC_NORMALIZE; bound = (Range >> kNumBitModelTotalBits) * *(p); if (Code < bound)
#define UpdateBit0(p) Range = bound; *(p) += (kBitModelTotal - *(p)) >> kNumMoveBits;
#define UpdateBit1(p) Range -= bound; Code -= bound; *(p) -= (*(p)) >> kNumMoveBits;

#define RC_GET_BIT2(p, mi, A0, A1) IfBit0(p) \
  { UpdateBit0(p); mi <<= 1; A0; } else \
  { UpdateBit1(p); mi = (mi + mi) + 1; A1; } 
  
#define RC_GET_BIT(p, mi) RC_GET_BIT2(p, mi, ; , ;)               

#define RangeDecoderBitTreeDecode(probs, numLevels, res) \
  { int i = numLevels; res = 1; \
  do { CProb *p = probs + res; RC_GET_BIT(p, res) } while(--i != 0); \
  res -= (1 << numLevels); }


#define kNumPosBitsMax 4
#define kNumPosStatesMax (1 << kNumPosBitsMax)

#define kLenNumLowBits 3
#define kLenNumLowSymbols (1 << kLenNumLowBits)
#define kLenNumMidBits 3
#define kLenNumMidSymbols (1 << kLenNumMidBits)
#define kLenNumHighBits 8
#define kLenNumHighSymbols (1 << kLenNumHighBits)

#define LenChoice 0
#define LenChoice2 (LenChoice + 1)
#define LenLow (LenChoice2 + 1)
#define LenMid (LenLow + (kNumPosStatesMax << kLenNumLowBits))
#define LenHigh (LenMid + (kNumPosStatesMax << kLenNumMidBits))
#define kNumLenProbs (LenHigh + kLenNumHighSymbols) 


#define kNumStates 12
#define kNumLitStates 7

#define kStartPosModelIndex 4
#define kEndPosModelIndex 14
#define kNumFullDistances (1 << (kEndPosModelIndex >> 1))

#define kNumPosSlotBits 6
#define kNumLenToPosStates 4

#define kNumAlignBits 4
#define kAlignTableSize (1 << kNumAlignBits)

#define kMatchMinLen 2

#define IsMatch 0
#define IsRep (IsMatch + (kNumStates << kNumPosBitsMax))
#define IsRepG0 (IsRep + kNumStates)
#define IsRepG1 (IsRepG0 + kNumStates)
#define IsRepG2 (IsRepG1 + kNumStates)
#define IsRep0Long (IsRepG2 + kNumStates)
#define PosSlot (IsRep0Long + (kNumStates << kNumPosBitsMax))
#define SpecPos (PosSlot + (kNumLenToPosStates << kNumPosSlotBits))
#define Align (SpecPos + kNumFullDistances - kEndPosModelIndex)
#define LenCoder (Align + kAlignTableSize)
#define RepLenCoder (LenCoder + kNumLenProbs)
#define Literal (RepLenCoder + kNumLenProbs)

#if Literal != LZMA_BASE_SIZE
StopCompilingDueBUG
#endif

#ifdef _LZMA_OUT_READ

typedef struct _LzmaVarState
{
  Byte *Buffer;
  Byte *BufferLim;
  UInt32 Range;
  UInt32 Code;
  #ifdef _LZMA_IN_CB
  ILzmaInCallback *InCallback;
  #endif
  Byte *Dictionary;
  UInt32 DictionarySize;
  UInt32 DictionaryPos;
  UInt32 GlobalPos;
  UInt32 Reps[4];
  int lc;
  int lp;
  int pb;
  int State;
  int RemainLen;
  Byte TempDictionary[4];
} LzmaVarState;

int LzmaDecoderInit(
    unsigned char *buffer, UInt32 bufferSize,
    int lc, int lp, int pb,
    unsigned char *dictionary, UInt32 dictionarySize,
    #ifdef _LZMA_IN_CB
    ILzmaInCallback *InCallback
    #else
    unsigned char *inStream, UInt32 inSize
    #endif
    )
{
  Byte *Buffer;
  Byte *BufferLim;
  UInt32 Range;
  UInt32 Code;
  LzmaVarState *vs = (LzmaVarState *)buffer;
  CProb *p = (CProb *)(buffer + sizeof(LzmaVarState));
  UInt32 numProbs = Literal + ((UInt32)LZMA_LIT_SIZE << (lc + lp));
  UInt32 i;
  if (bufferSize < numProbs * sizeof(CProb) + sizeof(LzmaVarState))
    return LZMA_RESULT_NOT_ENOUGH_MEM;
  vs->Dictionary = dictionary;
  vs->DictionarySize = dictionarySize;
  vs->DictionaryPos = 0;
  vs->GlobalPos = 0;
  vs->Reps[0] = vs->Reps[1] = vs->Reps[2] = vs->Reps[3] = 1;
  vs->lc = lc;
  vs->lp = lp;
  vs->pb = pb;
  vs->State = 0;
  vs->RemainLen = 0;
  dictionary[dictionarySize - 1] = 0;
  for (i = 0; i < numProbs; i++)
    p[i] = kBitModelTotal >> 1; 

  #ifdef _LZMA_IN_CB
  RC_INIT;
  #else
  RC_INIT(inStream, inSize);
  #endif
  vs->Buffer = Buffer;
  vs->BufferLim = BufferLim;
  vs->Range = Range;
  vs->Code = Code;
  #ifdef _LZMA_IN_CB
  vs->InCallback = InCallback;
  #endif

  return LZMA_RESULT_OK;
}

int LzmaDecode(unsigned char *buffer, 
    unsigned char *outStream, UInt32 outSize,
    UInt32 *outSizeProcessed)
{
  LzmaVarState *vs = (LzmaVarState *)buffer;
  Byte *Buffer = vs->Buffer;
  Byte *BufferLim = vs->BufferLim;
  UInt32 Range = vs->Range;
  UInt32 Code = vs->Code;
  #ifdef _LZMA_IN_CB
  ILzmaInCallback *InCallback = vs->InCallback;
  #endif
  CProb *p = (CProb *)(buffer + sizeof(LzmaVarState));
  int state = vs->State;
  Byte previousByte;
  UInt32 rep0 = vs->Reps[0], rep1 = vs->Reps[1], rep2 = vs->Reps[2], rep3 = vs->Reps[3];
  UInt32 nowPos = 0;
  UInt32 posStateMask = (1 << (vs->pb)) - 1;
  UInt32 literalPosMask = (1 << (vs->lp)) - 1;
  int lc = vs->lc;
  int len = vs->RemainLen;
  UInt32 globalPos = vs->GlobalPos;

  Byte *dictionary = vs->Dictionary;
  UInt32 dictionarySize = vs->DictionarySize;
  UInt32 dictionaryPos = vs->DictionaryPos;

  Byte tempDictionary[4];
  if (dictionarySize == 0)
  {
    dictionary = tempDictionary;
    dictionarySize = 1;
    tempDictionary[0] = vs->TempDictionary[0];
  }

  if (len == -1)
  {
    *outSizeProcessed = 0;
    return LZMA_RESULT_OK;
  }

  while(len != 0 && nowPos < outSize)
  {
    UInt32 pos = dictionaryPos - rep0;
    if (pos >= dictionarySize)
      pos += dictionarySize;
    outStream[nowPos++] = dictionary[dictionaryPos] = dictionary[pos];
    if (++dictionaryPos == dictionarySize)
      dictionaryPos = 0;
    len--;
  }
  if (dictionaryPos == 0)
    previousByte = dictionary[dictionarySize - 1];
  else
    previousByte = dictionary[dictionaryPos - 1];
#else

int LzmaDecode(
    Byte *buffer, UInt32 bufferSize,
    int lc, int lp, int pb,
    #ifdef _LZMA_IN_CB
    ILzmaInCallback *InCallback,
    #else
    unsigned char *inStream, UInt32 inSize,
    #endif
    unsigned char *outStream, UInt32 outSize,
    UInt32 *outSizeProcessed)
{
  UInt32 numProbs = Literal + ((UInt32)LZMA_LIT_SIZE << (lc + lp));
  CProb *p = (CProb *)buffer;

  UInt32 i;
  int state = 0;
  Byte previousByte = 0;
  UInt32 rep0 = 1, rep1 = 1, rep2 = 1, rep3 = 1;
  UInt32 nowPos = 0;
  UInt32 posStateMask = (1 << pb) - 1;
  UInt32 literalPosMask = (1 << lp) - 1;
  int len = 0;
  
  Byte *Buffer;
  Byte *BufferLim;
  UInt32 Range;
  UInt32 Code;
  
  if (bufferSize < numProbs * sizeof(CProb))
    return LZMA_RESULT_NOT_ENOUGH_MEM;
  for (i = 0; i < numProbs; i++)
    p[i] = kBitModelTotal >> 1;
  

  #ifdef _LZMA_IN_CB
  RC_INIT;
  #else
  RC_INIT(inStream, inSize);
  #endif
#endif

  *outSizeProcessed = 0;
  while(nowPos < outSize)
  {
    CProb *prob;
    UInt32 bound;
    int posState = (int)(
        (nowPos 
        #ifdef _LZMA_OUT_READ
        + globalPos
        #endif
        )
        & posStateMask);

    prob = p + IsMatch + (state << kNumPosBitsMax) + posState;
    IfBit0(prob)
    {
      int symbol = 1;
      UpdateBit0(prob)
      prob = p + Literal + (LZMA_LIT_SIZE * 
        (((
        (nowPos 
        #ifdef _LZMA_OUT_READ
        + globalPos
        #endif
        )
        & literalPosMask) << lc) + (previousByte >> (8 - lc))));

      if (state >= kNumLitStates)
      {
        int matchByte;
        #ifdef _LZMA_OUT_READ
        UInt32 pos = dictionaryPos - rep0;
        if (pos >= dictionarySize)
          pos += dictionarySize;
        matchByte = dictionary[pos];
        #else
        matchByte = outStream[nowPos - rep0];
        #endif
        do
        {
          int bit;
          CProb *probLit;
          matchByte <<= 1;
          bit = (matchByte & 0x100);
          probLit = prob + 0x100 + bit + symbol;
          RC_GET_BIT2(probLit, symbol, if (bit != 0) break, if (bit == 0) break)
        }
        while (symbol < 0x100);
      }
      while (symbol < 0x100)
      {
        CProb *probLit = prob + symbol;
        RC_GET_BIT(probLit, symbol)
      }
      previousByte = (Byte)symbol;

      outStream[nowPos++] = previousByte;
      #ifdef _LZMA_OUT_READ
      dictionary[dictionaryPos] = previousByte;
      if (++dictionaryPos == dictionarySize)
        dictionaryPos = 0;
      #endif
      if (state < 4) state = 0;
      else if (state < 10) state -= 3;
      else state -= 6;
    }
    else             
    {
      UpdateBit1(prob);
      prob = p + IsRep + state;
      IfBit0(prob)
      {
        UpdateBit0(prob);
        rep3 = rep2;
        rep2 = rep1;
        rep1 = rep0;
        state = state < kNumLitStates ? 0 : 3;
        prob = p + LenCoder;
      }
      else
      {
        UpdateBit1(prob);
        prob = p + IsRepG0 + state;
        IfBit0(prob)
        {
          UpdateBit0(prob);
          prob = p + IsRep0Long + (state << kNumPosBitsMax) + posState;
          IfBit0(prob)
          {
            #ifdef _LZMA_OUT_READ
            UInt32 pos;
            #endif
            UpdateBit0(prob);
            if (nowPos 
                #ifdef _LZMA_OUT_READ
                + globalPos
                #endif
                == 0)
              return LZMA_RESULT_DATA_ERROR;
            state = state < kNumLitStates ? 9 : 11;
            #ifdef _LZMA_OUT_READ
            pos = dictionaryPos - rep0;
            if (pos >= dictionarySize)
              pos += dictionarySize;
            previousByte = dictionary[pos];
            dictionary[dictionaryPos] = previousByte;
            if (++dictionaryPos == dictionarySize)
              dictionaryPos = 0;
            #else
            previousByte = outStream[nowPos - rep0];
            #endif
            outStream[nowPos++] = previousByte;
            continue;
          }
          else
          {
            UpdateBit1(prob);
          }
        }
        else
        {
          UInt32 distance;
          UpdateBit1(prob);
          prob = p + IsRepG1 + state;
          IfBit0(prob)
          {
            UpdateBit0(prob);
            distance = rep1;
          }
          else 
          {
            UpdateBit1(prob);
            prob = p + IsRepG2 + state;
            IfBit0(prob)
            {
              UpdateBit0(prob);
              distance = rep2;
            }
            else
            {
              UpdateBit1(prob);
              distance = rep3;
              rep3 = rep2;
            }
            rep2 = rep1;
          }
          rep1 = rep0;
          rep0 = distance;
        }
        state = state < kNumLitStates ? 8 : 11;
        prob = p + RepLenCoder;
      }
      {
        int numBits, offset;
        CProb *probLen = prob + LenChoice;
        IfBit0(probLen)
        {
          UpdateBit0(probLen);
          probLen = prob + LenLow + (posState << kLenNumLowBits);
          offset = 0;
          numBits = kLenNumLowBits;
        }
        else
        {
          UpdateBit1(probLen);
          probLen = prob + LenChoice2;
          IfBit0(probLen)
          {
            UpdateBit0(probLen);
            probLen = prob + LenMid + (posState << kLenNumMidBits);
            offset = kLenNumLowSymbols;
            numBits = kLenNumMidBits;
          }
          else
          {
            UpdateBit1(probLen);
            probLen = prob + LenHigh;
            offset = kLenNumLowSymbols + kLenNumMidSymbols;
            numBits = kLenNumHighBits;
          }
        }
        RangeDecoderBitTreeDecode(probLen, numBits, len);
        len += offset;
      }

      if (state < 4)
      {
        int posSlot;
        state += kNumLitStates;
        prob = p + PosSlot +
            ((len < kNumLenToPosStates ? len : kNumLenToPosStates - 1) << 
            kNumPosSlotBits);
        RangeDecoderBitTreeDecode(prob, kNumPosSlotBits, posSlot);
        if (posSlot >= kStartPosModelIndex)
        {
          int numDirectBits = ((posSlot >> 1) - 1);
          rep0 = (2 | ((UInt32)posSlot & 1));
          if (posSlot < kEndPosModelIndex)
          {
            rep0 <<= numDirectBits;
            prob = p + SpecPos + rep0 - posSlot - 1;
          }
          else
          {
            numDirectBits -= kNumAlignBits;
            do
            {
              RC_NORMALIZE
              Range >>= 1;
              rep0 <<= 1;
              if (Code >= Range)
              {
                Code -= Range;
                rep0 |= 1;
              }
            }
            while (--numDirectBits != 0);
            prob = p + Align;
            rep0 <<= kNumAlignBits;
            numDirectBits = kNumAlignBits;
          }
          {
            int i = 1;
            int mi = 1;
            do
            {
              CProb *prob3 = prob + mi;
              RC_GET_BIT2(prob3, mi, ; , rep0 |= i);
              i <<= 1;
            }
            while(--numDirectBits != 0);
          }
        }
        else
          rep0 = posSlot;
        if (++rep0 == (UInt32)(0))
        {
          /* it's for stream version */
          len = -1;
          break;
        }
      }

      len += kMatchMinLen;
      if (rep0 > nowPos 
        #ifdef _LZMA_OUT_READ
        + globalPos || rep0 > dictionarySize
        #endif
        ) 
        return LZMA_RESULT_DATA_ERROR;
      do
      {
        #ifdef _LZMA_OUT_READ
        UInt32 pos = dictionaryPos - rep0;
        if (pos >= dictionarySize)
          pos += dictionarySize;
        previousByte = dictionary[pos];
        dictionary[dictionaryPos] = previousByte;
        if (++dictionaryPos == dictionarySize)
          dictionaryPos = 0;
        #else
        previousByte = outStream[nowPos - rep0];
        #endif
        len--;
        outStream[nowPos++] = previousByte;
      }
      while(len != 0 && nowPos < outSize);
    }
  }
  RC_NORMALIZE;

  #ifdef _LZMA_OUT_READ
  vs->Buffer = Buffer;
  vs->BufferLim = BufferLim;
  vs->Range = Range;
  vs->Code = Code;
  vs->DictionaryPos = dictionaryPos;
  vs->GlobalPos = globalPos + nowPos;
  vs->Reps[0] = rep0;
  vs->Reps[1] = rep1;
  vs->Reps[2] = rep2;
  vs->Reps[3] = rep3;
  vs->State = state;
  vs->RemainLen = len;
  vs->TempDictionary[0] = tempDictionary[0];
  #endif

  *outSizeProcessed = nowPos;
  return LZMA_RESULT_OK;
}
