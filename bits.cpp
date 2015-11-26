/*
 * functions to take care of bitstream-level read/write operations
 */

#include "bits.h"

//member functions of Flow
void Flow::reset(unsigned char *pcBitstreamBuf){
    _bytes = 0;     //index to buffer
    _mask  = 0x80;  //bit to be used next time, write from MSB to LSB
    _data  = pcBitstreamBuf;
}

int Flow::bytesUsed(void){
    //if mask = 80, current byte is not used yet.
    return _mask == 0x80 ? _bytes : _bytes + 1;
}

//member functions of OFlow
bool OFlow::writeBit(bool b)
{
    if (_mask == 0x80) {
        //first bit of the current byte: set MSB to b, and reset all other bits.
      _data[_bytes] = (b ? 0x80 : 0x00);
    } else if (b) {
      _data[_bytes] |= _mask;
    }

    if (_mask == 0x01) {
        //current bit is LSB, move to next byte
      _mask = 0x80;
      _bytes++;
    } else {
      _mask >>= 1;
    }

    return b;
}

//write multiple bits: from higher bits to LSB
//return the last n bits of data
int OFlow::writeBits(int data,int bits)
{
  int r = 0;
  int m = ((0x01 << bits) >> 1);
  
  for(int b = 0; b < bits; b++, m >>= 1)
  {
    writeBit((data & m) == m);
    r |= m;
  }
  
  return (data & r);
}

//member functions of IFlow
bool IFlow::readBit(void)
{
    //read the current bit
    bool b = ((_data[_bytes] & _mask) == _mask);

    //adjust pointer
    if (_mask == 0x01) {
      _mask = 0x80;
      _bytes++;
    } else {
      _mask >>= 1;
    }

    return b;
}

//read multiple bits, fill from MSB to LSB
int IFlow::readBits(int bits)
{
  int data = 0;
  int m = ((0x01 << bits) >> 1);

  for(int b = 0; b < bits; b++, m >>= 1) {
      if(readBit()) {
        data |= m;
      }
  }
  
  return data;
}
