#ifndef _QUANT_H_
#define _QUANT_H_

class Quant
{
 public:
  static void QuantMidtread(float *[], int iBlkSize, float fQStep);
  static void DequantMidtread(float *[], int iBlkSize, float fQStep);
};

#endif
