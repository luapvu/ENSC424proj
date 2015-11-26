#ifndef _AC_H_
#define _AC_H_

#define FIRST 0x4000
#define HALF  0x8000
#define THIRD 0xC000
#define TOP   0xFFFF

class binModel
{
public:
    binModel(int f0,int f1,int m);   
    void scale(void);
  
    int freq[2],max;
};

class ACEncoder
{
 public:
  void start(void);
  void stop(class OFlow *);
  bool codeSymbol(bool,binModel *,class OFlow *);
  
 private:
  int _high,_low,_follow;
};

class ACDecoder
{
 public:
  void start(class IFlow*);
  bool decodeSymbol(binModel *,class IFlow *);
  
 private:
  int _high,_low,_value;
};

#endif
