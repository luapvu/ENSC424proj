/*********************************************************************************
 *
 * Binary arithmetic codec
 *
 * The arithmetic codec was originally written by Dr. Chengjie Tu 
 * at the Johns Hopkins University (now with Microsoft Windows Media Division).
 *
 * Modified by Jie Liang
 * School of Engineering Science
 * Simon Fraser University
 *
 * Jan. 2005
 *
 *********************************************************************************/

#include "arith.h"
#include "bits.h"

//--------------------------
// Probability models for a context
//--------------------------

//initialization for binary probability model
binModel::binModel(int f0,int f1,int m) {
    freq[1] = f1;       //counter for 1
    freq[0] = f0 + f1;  //total counters for 1 and 0
    max = m;
}

// Scaling of probability models for a context
// to prevent overflow of the counter. 
// It also increases thw weighting of recent history
void binModel::scale(void) {
    int gap = freq[0] - freq[1];
    freq[1] = ((freq[1] + 1) >> 1);
    freq[0] = freq[1] + ((gap + 1) >> 1);
}

//--------------------------
// ACEncoder class
//--------------------------
void ACEncoder::start(void){
    _high = TOP;
    _low = 0;
    _follow = 0;
}

void ACEncoder::stop(OFlow *o)
{
  bool s = (_low < FIRST);
  
  o->writeBit(!s);
  while((_follow--) >= 0) {
    o->writeBit(s);
  }
}

//--------------------------
// Encode a bit by binary arithmetic coder
//--------------------------
bool ACEncoder::codeSymbol(
    bool s,         //bit to be coded
    binModel *c,    //probability model of a context
    OFlow *o)       //output stream
{ 
  if (s) {
      //freq[1] is prob of symbol 1, and the lower part of the interval is for symbol 1
      _high = _low + ((_high - _low + 1) *c->freq[1]) / c->freq[0] - 1;
      c->freq[1]++;
  } else {
      _low += ((_high - _low + 1) * c->freq[1]) / c->freq[0];
  }

  //scale the prob model to prevent overflow
  if(++c->freq[0] >= c->max)
      c->scale();
  
  while(true)
  {
    if(_high < HALF)  //E1
    {
      o->writeBit(false);

      //compensate for previous E3. _follow is the counter of E3
      for( ; _follow>0; _follow--)
		o->writeBit(true);
    } 
    else if(_low >= HALF) //E2
    {
      o->writeBit(true);
      
      for(;_follow>0;_follow--)
    	o->writeBit(false);
      
      _high-=HALF,_low-=HALF;
    }
    else if(_low >= FIRST && _high < THIRD) {
        //E3
      _follow++;
      _high -= FIRST;
      _low -= FIRST;
    } else {
      break;  //range too large, need next symbol to further narrow down.
    }

    //scale
    _low <<= 1;
    _high += _high + 1; //add 1 to HIGH!
  };
  
  return s;
}

//--------------------------
// ACDecoder class
//--------------------------

//initialize and read 16 bits to _value
void ACDecoder::start(IFlow *in)
{
  _high = TOP;
  _low = 0;
  _value = 0;
  
  for(int i = 0; i < 16; i++)
  {
    _value <<= 1;
    if(in->readBit())
      _value++;
  }
}

//--------------------------
// Decode a bit by binary arithmetic coder
//--------------------------
bool ACDecoder::decodeSymbol(
    binModel *c,    //probability model of the selected context
    IFlow *i)       //input stream
{
  //decode one bit by comparing _value (read from bitstream) and HIGH
  int range = (_high - _low + 1) * c->freq[1] / c->freq[0]; //range of the lower part (for symbol 1)

  //lower part of the interval is for symbol 1
  bool s = (_value <= _low + range - 1);    //_low + range - 1 is the HIGH of the current interval

  //update interval
  if(s) {
    _high = _low + range - 1;
    c->freq[1]++;
  } else {
    _low += range;
  }

  //update probability and scale if necessary
  if(++c->freq[0] >= c->max) {
    c->scale();
  }

  //scale as many times as possible before decoding next symbol
  while(true)
  {
    if(_high >= HALF) {
        if(_low < HALF) {
            if(_low >= FIRST && _high < THIRD) {
              //E3
	          _high -= FIRST;
              _low -= FIRST;   
              _value -= FIRST;
            } else {
              //no more scalign is needed
	          break;
            }
        } else {
            //E2
	        _high -= HALF;
            _low -= HALF;
            _value -= HALF;
        }
    }

    //if _high < HALF ==> E1, double directly
    //E2 and E3 also reach here
    _low <<= 1;
    _high += _high + 1;
    
    //read one more bit after scaling
    _value <<= 1;
    if(i->readBit()) {  
      _value++;
    }
  };

  //return decoded bit
  return s;
}
