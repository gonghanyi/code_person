#include "number.h"
#include <iostream>

const char two_ASCII_digits[100][2] = {
  {'0', '0'}, {'0', '1'}, {'0', '2'}, {'0', '3'}, {'0', '4'},
  {'0', '5'}, {'0', '6'}, {'0', '7'}, {'0', '8'}, {'0', '9'},
  {'1', '0'}, {'1', '1'}, {'1', '2'}, {'1', '3'}, {'1', '4'},
  {'1', '5'}, {'1', '6'}, {'1', '7'}, {'1', '8'}, {'1', '9'},
  {'2', '0'}, {'2', '1'}, {'2', '2'}, {'2', '3'}, {'2', '4'},
  {'2', '5'}, {'2', '6'}, {'2', '7'}, {'2', '8'}, {'2', '9'},
  {'3', '0'}, {'3', '1'}, {'3', '2'}, {'3', '3'}, {'3', '4'},
  {'3', '5'}, {'3', '6'}, {'3', '7'}, {'3', '8'}, {'3', '9'},
  {'4', '0'}, {'4', '1'}, {'4', '2'}, {'4', '3'}, {'4', '4'},
  {'4', '5'}, {'4', '6'}, {'4', '7'}, {'4', '8'}, {'4', '9'},
  {'5', '0'}, {'5', '1'}, {'5', '2'}, {'5', '3'}, {'5', '4'},
  {'5', '5'}, {'5', '6'}, {'5', '7'}, {'5', '8'}, {'5', '9'},
  {'6', '0'}, {'6', '1'}, {'6', '2'}, {'6', '3'}, {'6', '4'},
  {'6', '5'}, {'6', '6'}, {'6', '7'}, {'6', '8'}, {'6', '9'},
  {'7', '0'}, {'7', '1'}, {'7', '2'}, {'7', '3'}, {'7', '4'},
  {'7', '5'}, {'7', '6'}, {'7', '7'}, {'7', '8'}, {'7', '9'},
  {'8', '0'}, {'8', '1'}, {'8', '2'}, {'8', '3'}, {'8', '4'},
  {'8', '5'}, {'8', '6'}, {'8', '7'}, {'8', '8'}, {'8', '9'},
  {'9', '0'}, {'9', '1'}, {'9', '2'}, {'9', '3'}, {'9', '4'},
  {'9', '5'}, {'9', '6'}, {'9', '7'}, {'9', '8'}, {'9', '9'}
};

char* UtilNumber::FastUInt32ToBufferLeft(uint32_t u, char* buffer) {
  uint32_t digits;
  const char* ASCII_digits = nullptr;

  if (u >= 1000000000) { // >= 1,000,000,000
    digits = u / 100000000;   //100,000,000
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
sublt100_000_000:
    std::cout << "use sublt100_000_000, digits = " << digits << ", u = " << u << std::endl;
    u -= digits * 100000000;  // 100,000,000
lt100_000_000:
    std::cout << "use lt100_000_000, digits = " << digits << ", u = " << u << std::endl;
    digits = u / 1000000; // 1,000,000
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
sublt1_000_000:
    std::cout << "use sublt1_000_000, digits = " << digits << ", u = " << u << std::endl;
    u -= digits * 1000000;  // 1,000,000
lt1_000_000:
    std::cout << "use lt1_000_000, digits = " << digits << ", u = " << u << std::endl;
    digits = u / 10000;   //10,000
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
sublt10_000:
    std::cout << "use sublt10_000, digits = " << digits << ", u = " << u << std::endl;
    u -= digits * 10000;  //10,000
lt10_000:
    std::cout << "use lt10_000, digits = " << digits << ", u = " << u << std::endl;
    digits = u / 100;
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
sublt100:
    std::cout << "use sublt100_000_000, digits = " << digits << ", u = " << u << std::endl;
    u -= digits * 100;
lt100:
    digits = u;
    ASCII_digits = two_ASCII_digits[digits];
    buffer[0] = ASCII_digits[0];
    buffer[1] = ASCII_digits[1];
    buffer += 2;
done:
    *buffer = 0;
    return buffer;
  }

  if (u < 100) {
    digits = u;
    if (u >= 10) goto lt100;
    *buffer++ = '0' + digits;
    goto done;
  }
  if (u < 10000) {  //10,000
    if (u >= 1000) goto lt10_000;
    digits = u / 100;
    *buffer++ = '0' + digits;
    goto sublt100;
  }
  if (u < 1000000) {  // 1,000,000
    if (u >= 100000) goto lt1_000_000;
    digits = u / 10000; // 10,000
    *buffer++ = '0' + digits;
    goto sublt10_000;
  }
  if (u < 100000000) {  // 100,000,000
    if (u >= 10000000) goto lt100_000_000;
    digits = u / 1000000; //1,000,000
    *buffer++ = '0' + digits;
    goto sublt1_000_000;
  }
  // we already know that u < 1,000,000,000
  digits = u / 100000000; // 100,000,000
  *buffer++ = '0' + digits;
  goto sublt100_000_000;
}

char* UtilNumber::FastInt32ToBufferLeft(int32_t i, char* buffer) {
  uint32_t u = i;
  if (i < 0) {
    *buffer++ = '-';
    u = 0 - i;
  }
  return FastUInt32ToBufferLeft(u, buffer);
}

char* UtilNumber::FastUInt32ToBuffer(uint32_t u, char* buffer) {
  FastUInt32ToBufferLeft(u, buffer);
  return buffer;
}

char* UtilNumber::FastInt32ToBuffer(int32_t i, char* buffer) {
  FastInt32ToBufferLeft(i, buffer);
  return buffer;
}
