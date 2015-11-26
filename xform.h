#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

class Transform
{
 public:
  static void FDCT4(float *[4]);
  static void IDCT4(float *[4]);
  static void FDCT8(float *[8]);
  static void IDCT8(float *[8]);
  
 private:
  static float _Rot4[3];
  static float _Rot8[7];
};

#endif
