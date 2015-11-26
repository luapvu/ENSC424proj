#include "quant.h"

//mid-tread quantization
void Quant::QuantMidtread(float *fpBlk[], int iBlkSize, float fQStep)
{
    int iIdx, iSign;
    float fAbs;

    for (int i = 0; i < iBlkSize; i++) {
        for (int j = 0; j < iBlkSize; j++) {
            if (fpBlk[i][j] > 0) {
                iSign = 0;
                fAbs = fpBlk[i][j];
            } else {
                iSign = 1;
                fAbs = -fpBlk[i][j];
            }

            iIdx = (int) (fAbs / fQStep + 0.5f);
            
            if (!iSign) {
                fpBlk[i][j] = (float) iIdx;
            } else {
                fpBlk[i][j] = (float) -iIdx;
            }
        }
    }
}

//mid-tread inverse quantization
void Quant::DequantMidtread(float *fpBlk[], int iBlkSize, float fQStep)
{
    for (int i = 0; i < iBlkSize; i++) {
        for (int j = 0; j < iBlkSize; j++) {
            fpBlk[i][j] =  fpBlk[i][j] * fQStep;
        }
    }
}
