#ifndef _FLOW_H_
#define _FLOW_H_

class Flow
{
 public:
  int bytesUsed(void);
  void reset(unsigned char *d);
  
 protected:
  unsigned char *_data;
  int _bytes;
  unsigned char _mask;
};

class OFlow:public Flow
{
 public:
  bool writeBit(bool b);
  int writeBits(int,int);  
};

class IFlow:public Flow
{
 public:
  bool readBit(void);
  int readBits(int);  
};

#endif
