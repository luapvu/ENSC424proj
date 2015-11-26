#include <iostream>
#include <fstream>
using namespace std;

#include "xform.h"

//constants used in 4-point DCT:
//sqrt(2)/2, cos(3pi/8), sin(3pi/8)
float Transform::_Rot4[]={0.70710678118655f, 0.38268343236509f, 0.92387953251129f};

//sqrt(2), cos(3pi/16), sin(3pi/16), cos(3pi/8), sin(3pi/8), cos(7pi/16), sin(7pi/16)
float Transform::_Rot8[]={
    0.70710678118655f, 
    0.83146961230255f, 0.55557023301960f, 
    0.38268343236509f, 0.92387953251129f,
    0.19509032201613f, 0.98078528040323f
};

//Forward 4-point DCT:
// There is a scaling of 1/sqrt(2) for 1D transform.
// After 2D transform, the scalign becomes 1/2. Apply if after the column transform.
void Transform::FDCT4(float *fpBlk[4])
{
  float f[4];

  //row transform
  for(int y = 0; y < 4; y++) {
    f[0] = fpBlk[y][0] + fpBlk[y][3];
    f[3] = fpBlk[y][0] - fpBlk[y][3];
    f[1] = fpBlk[y][1] + fpBlk[y][2];
    f[2] = fpBlk[y][1] - fpBlk[y][2];

    fpBlk[y][0] = _Rot4[0] * (f[0] + f[1]);
    fpBlk[y][2] = _Rot4[0] * (f[0] - f[1]);

    fpBlk[y][1] =  _Rot4[1] * f[2] + _Rot4[2] * f[3];
    fpBlk[y][3] = -_Rot4[2] * f[2] + _Rot4[1] * f[3];    
  }

  //column transform, including the final scaling of 1/2
  for(int x = 0; x < 4; x++) {
    f[0] = fpBlk[0][x] + fpBlk[3][x];
    f[3] = fpBlk[0][x] - fpBlk[3][x];
    f[1] = fpBlk[1][x] + fpBlk[2][x];
    f[2] = fpBlk[1][x] - fpBlk[2][x];

    fpBlk[0][x] = _Rot4[0] * (f[0] + f[1]) / 2;
    fpBlk[2][x] = _Rot4[0] * (f[0] - f[1]) / 2;

    fpBlk[1][x] =  (_Rot4[1] * f[2] + _Rot4[2] * f[3]) / 2;
    fpBlk[3][x] = (-_Rot4[2] * f[2] + _Rot4[1] * f[3]) / 2;
  }
}

//inverse 4-point transform
void Transform::IDCT4(float *fpBlk[4])
{
  int x, y;
  float f[4];

  //column transform
  for(x = 0; x < 4; x++)
  {
    f[0] = _Rot4[0] * (fpBlk[0][x] + fpBlk[2][x]);
    f[1] = _Rot4[0] * (fpBlk[0][x] - fpBlk[2][x]);

    f[2] = _Rot4[1] * fpBlk[1][x] - _Rot4[2] * fpBlk[3][x];
    f[3] = _Rot4[2] * fpBlk[1][x] + _Rot4[1] * fpBlk[3][x];

    fpBlk[0][x] = f[0] + f[3];
    fpBlk[3][x] = f[0] - f[3];
    fpBlk[1][x] = f[1] + f[2];
    fpBlk[2][x] = f[1] - f[2];
  }
  
  //row transform, including final scaling of 1/2 and clipping
  for(y = 0; y < 4; y++)
  {
    f[0] = _Rot4[0] * (fpBlk[y][0] + fpBlk[y][2]);
    f[1] = _Rot4[0] * (fpBlk[y][0] - fpBlk[y][2]);

    f[2] = _Rot4[1] * fpBlk[y][1] - _Rot4[2] * fpBlk[y][3];
    f[3] = _Rot4[2] * fpBlk[y][1] + _Rot4[1] * fpBlk[y][3];

    fpBlk[y][0] = (f[0] + f[3]) / 2;
    fpBlk[y][3] = (f[0] - f[3]) / 2;
    fpBlk[y][1] = (f[1] + f[2]) / 2;
    fpBlk[y][2] = (f[1] - f[2]) / 2;
  }

}

void Transform::FDCT8(float *fpBlk[8])
{
  int ctr;
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp10, tmp11, tmp12, tmp13;
  float z1, z2, z3, z4;

  //row transform
  for (ctr = 7; ctr >= 0; ctr--) {
    
    tmp0 = fpBlk[ctr][0] + fpBlk[ctr][7];
    tmp7 = fpBlk[ctr][0] - fpBlk[ctr][7];
    tmp1 = fpBlk[ctr][1] + fpBlk[ctr][6];
    tmp6 = fpBlk[ctr][1] - fpBlk[ctr][6];
    tmp2 = fpBlk[ctr][2] + fpBlk[ctr][5];
    tmp5 = fpBlk[ctr][2] - fpBlk[ctr][5];
    tmp3 = fpBlk[ctr][3] + fpBlk[ctr][4];
    tmp4 = fpBlk[ctr][3] - fpBlk[ctr][4];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    //pi/4
    fpBlk[ctr][0] = (tmp10 + tmp11) * _Rot8[0];
    fpBlk[ctr][4] = (tmp10 - tmp11) * _Rot8[0];

    //3pi/8
    fpBlk[ctr][2] = ( _Rot8[3] * tmp12 + _Rot8[4] * tmp13);
    fpBlk[ctr][6] = (-_Rot8[4] * tmp12 + _Rot8[3] * tmp13);
    
    /* Odd part */
    tmp11 = (-tmp5 + tmp6) * _Rot8[0];
    tmp12 = ( tmp5 + tmp6) * _Rot8[0];

    z1 =  tmp4  + tmp11;
    z2 =  tmp4  - tmp11;
    z3 = -tmp12 + tmp7;
    z4 =  tmp12 + tmp7;

    //7pi/16
    fpBlk[ctr][1] =  _Rot8[5] * z1 + _Rot8[6] * z4;
    fpBlk[ctr][7] = -_Rot8[6] * z1 + _Rot8[5] * z4;

    //3pi/16
    fpBlk[ctr][5] =  _Rot8[1] * z2 + _Rot8[2] * z3;
    fpBlk[ctr][3] = -_Rot8[2] * z2 + _Rot8[1] * z3;
  }

  /* Pass 2: process columns. */

  for (ctr = 7; ctr >= 0; ctr--) {
    tmp0 = fpBlk[0][ctr] + fpBlk[7][ctr];
    tmp7 = fpBlk[0][ctr] - fpBlk[7][ctr];
    tmp1 = fpBlk[1][ctr] + fpBlk[6][ctr];
    tmp6 = fpBlk[1][ctr] - fpBlk[6][ctr];
    tmp2 = fpBlk[2][ctr] + fpBlk[5][ctr];
    tmp5 = fpBlk[2][ctr] - fpBlk[5][ctr];
    tmp3 = fpBlk[3][ctr] + fpBlk[4][ctr];
    tmp4 = fpBlk[3][ctr] - fpBlk[4][ctr];
    
    /* Even part */
    
    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
 
    fpBlk[0][ctr] = (tmp10 + tmp11) * _Rot8[0] / 4;
    fpBlk[4][ctr] = (tmp10 - tmp11) * _Rot8[0] / 4;
    
    //3pi/8
    fpBlk[2][ctr] = ( _Rot8[3] * tmp12 + _Rot8[4] * tmp13) / 4;
    fpBlk[6][ctr] = (-_Rot8[4] * tmp12 + _Rot8[3] * tmp13) / 4;
    
    /* Odd part */

    tmp11 = (-tmp5 + tmp6) * _Rot8[0];
    tmp12 = ( tmp5 + tmp6) * _Rot8[0];

    z1 =  tmp4  + tmp11;
    z2 =  tmp4  - tmp11;
    z3 = -tmp12 + tmp7;
    z4 =  tmp12 + tmp7;

    //7pi/8
    fpBlk[1][ctr] = ( _Rot8[5] * z1 + _Rot8[6] * z4) / 4;
    fpBlk[7][ctr] = (-_Rot8[6] * z1 + _Rot8[5] * z4) / 4;

    //3pi/8
    fpBlk[5][ctr] = ( _Rot8[1] * z2 + _Rot8[2] * z3) / 4;
    fpBlk[3][ctr] = (-_Rot8[2] * z2 + _Rot8[1] * z3) / 4;   
  }


}

void Transform::IDCT8(float *fpBlk[8])
{
  int ctr;
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp11, tmp12;
  float z1, z2, z3, z4;

  //column transform
  for (ctr = 7; ctr >= 0; ctr--) {
   
    z1 = (fpBlk[0][ctr] + fpBlk[4][ctr]) * _Rot8[0];
    z2 = (fpBlk[0][ctr] - fpBlk[4][ctr]) * _Rot8[0];    
    
    z3 = (_Rot8[3] * fpBlk[2][ctr] - _Rot8[4] * fpBlk[6][ctr]);
    z4 = (_Rot8[4] * fpBlk[2][ctr] + _Rot8[3] * fpBlk[6][ctr]);

    tmp0 = z1 + z4;
    tmp3 = z1 - z4;
    tmp1 = z2 + z3;
    tmp2 = z2 - z3;
     
    z1 = (_Rot8[5] * fpBlk[1][ctr] - _Rot8[6] * fpBlk[7][ctr]);
    z4 = (_Rot8[6] * fpBlk[1][ctr] + _Rot8[5] * fpBlk[7][ctr]);
    z2 = (_Rot8[1] * fpBlk[5][ctr] - _Rot8[2] * fpBlk[3][ctr]);
    z3 = (_Rot8[2] * fpBlk[5][ctr] + _Rot8[1] * fpBlk[3][ctr]);   

    tmp4  =  z1 + z2;
    tmp11 =  z1 - z2;
    tmp12 = -z3 + z4;
    tmp7  =  z3 + z4;

    tmp5 = (-tmp11 + tmp12) * _Rot8[0];
    tmp6 = ( tmp11 + tmp12) * _Rot8[0];
      
    fpBlk[0][ctr] = tmp0 + tmp7;
    fpBlk[7][ctr] = tmp0 - tmp7;
    fpBlk[1][ctr] = tmp1 + tmp6;
    fpBlk[6][ctr] = tmp1 - tmp6;
    fpBlk[2][ctr] = tmp2 + tmp5;
    fpBlk[5][ctr] = tmp2 - tmp5;
    fpBlk[3][ctr] = tmp3 + tmp4;
    fpBlk[4][ctr] = tmp3 - tmp4;    
  }

  //row transform
  for (ctr = 7; ctr >= 0; ctr--) {
    
    z1 = (fpBlk[ctr][0] + fpBlk[ctr][4]) * _Rot8[0];
    z2 = (fpBlk[ctr][0] - fpBlk[ctr][4]) * _Rot8[0];    
    z3 = (_Rot8[3] * fpBlk[ctr][2] - _Rot8[4] * fpBlk[ctr][6]);
    z4 = (_Rot8[4] * fpBlk[ctr][2] + _Rot8[3] * fpBlk[ctr][6]);

    tmp0 = z1 + z4;
    tmp3 = z1 - z4;
    tmp1 = z2 + z3;
    tmp2 = z2 - z3;
     
    z1 = (_Rot8[5] * fpBlk[ctr][1] - _Rot8[6] * fpBlk[ctr][7]);
    z4 = (_Rot8[6] * fpBlk[ctr][1] + _Rot8[5] * fpBlk[ctr][7]);
    z2 = (_Rot8[1] * fpBlk[ctr][5] - _Rot8[2] * fpBlk[ctr][3]);
    z3 = (_Rot8[2] * fpBlk[ctr][5] + _Rot8[1] * fpBlk[ctr][3]);   

    tmp4  =  z1 + z2;
    tmp11 =  z1 - z2;
    tmp12 = -z3 + z4;
    tmp7  =  z3 + z4;

    tmp5 = (-tmp11 + tmp12) * _Rot8[0];
    tmp6 = ( tmp11 + tmp12) * _Rot8[0];
            
    fpBlk[ctr][0] = (tmp0 + tmp7) / 4;
    fpBlk[ctr][7] = (tmp0 - tmp7) / 4;
    fpBlk[ctr][1] = (tmp1 + tmp6) / 4;
    fpBlk[ctr][6] = (tmp1 - tmp6) / 4;
    fpBlk[ctr][2] = (tmp2 + tmp5) / 4;
    fpBlk[ctr][5] = (tmp2 - tmp5) / 4;
    fpBlk[ctr][3] = (tmp3 + tmp4) / 4;
    fpBlk[ctr][4] = (tmp3 - tmp4) / 4;
  }
}
