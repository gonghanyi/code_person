#ifndef UTIL_NUMBER_H_
#define UTIL_NUMBER_H_ 

#include <stdlib.h>
#include <stdint.h>

//static const int kFastToBufferSize = 32;

class UtilNumber {
public:
  static char* FastInt32ToBuffer(int32_t i, char* buffer);
  static char* FastUInt32ToBuffer(uint32_t i, char* buffer);

  static char* FastInt32ToBufferLeft(int32_t i, char* buffer);
  static char* FastUInt32ToBufferLeft(uint32_t i, char* buffer); 

private:
  static const int kFastToBufferSize = 32;
};

#endif
