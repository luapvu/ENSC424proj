#ifndef _ICODEC_H_
#define _ICODEC_H_

#define DUMPMV
#define MAX_CONTEXT_NUM     256
#define BLOCKSIZE           8
#define MBSIZE              16
#define MBSIZEUV            8

//----------------
// class common to both encoder and decoder
//----------------
class ICodec
{
 public:
  ICodec(unsigned,unsigned);
  ~ICodec(void);

  void SetImage(unsigned char *);
  void GetImage(unsigned char *);
  unsigned ForwardGolombRiceIndexMapping(int iNumber);
  int InverseGolombRiceIndexMapping(unsigned ucIndex);

  void SetFrameBufPointer(float *pfBuf1, float *pfBuf2);

  void GetReconstructedFrame();
  void GetBlockRecon(
    float **pfCurrFrame,    
    float **pfRefFrame,     
    int y, 
    int x, 
    int iMVy,
    int iMVx);

 protected:

  unsigned _w, _h, _w2, _h2, m_iMBNumW, m_iMBNumH;

  float *m_pfFrameBuf[2];
  float **_ydata, **_udata, **_vdata;
  float **_ydataRef, **_udataRef, **_vdataRef;

  int **m_iMVy, **m_iMVx;

  float *m_fBlkBuf[BLOCKSIZE];

  const static int m_zz[BLOCKSIZE][BLOCKSIZE];
  
  class binModel *_context[MAX_CONTEXT_NUM]; 
};


//----------------
// encoder class
//----------------
class IEncoder:public ICodec
{
 public:
  IEncoder(unsigned,unsigned);
  ~IEncoder(void);

  int codeImage(bool bIsIFrame, unsigned char *b, float fQstep);

  void codeBlock(float **buf, unsigned y, unsigned x, float fQstep);

  void MotionEst();
  void MBMotionEst(float **pfCurrFrame, float **pfRefFrame, int y, int x);
  int GetSAD(
    float **pfCurrFrame,    
    float **pfRefFrame,
    int y,
    int x,
    int iMVy,
    int iMVx,
    int iMAD0);

  void EncodeMV();

  void DumpMV(ofstream& DumpFile);

  void GetPredError();
  void GetBlockPredError(
    float **pfCurrFrame,    
    float **pfRefFrame,     
    int y, 
    int x, 
    int iMVy,
    int iMVx);

  void EncodeUnary(unsigned uiIndex);
  void EncodeGolombRice(unsigned uiIndex, int iGRPara);
  void EncodeExpGolomb(unsigned uiIndex);

 private:
  class ACEncoder *_ace;
  class OFlow *_out;
};

//----------------
// decoder class
//----------------
class IDecoder:public ICodec
{
 public:
  IDecoder(unsigned,unsigned);
  ~IDecoder(void);

  int decodeImage(bool bIsIFrame, unsigned char *, float);
  void decodeBlock(float **buf, unsigned y, unsigned x, float fQstep);
  void DecodeMV();

  void GetPrediction();
  void GetBlockPred(
    float **pfCurrFrame,    
    float **pfRefFrame,     
    int y, 
    int x, 
    int iMVy,
    int iMVx);

  unsigned DecodeUnary();
  unsigned DecodeGolombRice(int iGRPara);
  unsigned DecodeExpGolomb();

 private:
  class ACDecoder *_acd;
  class IFlow *_in;
  float m_fTmpBlock[8][8];
};

#endif
