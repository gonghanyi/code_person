#include "substitute.h"
#include <iostream>

using internal::SubstituteArg;

const SubstituteArg SubstituteArg::kNoArg;

static int CountSubstituteArgs(const SubstituteArg* const* args_array) {
  int count = 0;
  while (args_array[count] != &SubstituteArg::kNoArg) {
    ++count;
  }
  return count;
}

namespace internal{ 
int SubstitutedSize(string& format, const SubstituteArg* const* args_array) {
  int size = 0;
  for (int i = 0; i < format.length(); i++) {
    if ('$' == format[i]) {
      if (i+1 >= format.length()) {
        std::cout << "Invalid strings::Substitute() format string: " << format << std::endl;
        return -1;
      } else if (format[i+1] >= '0' && format[i+1] <= '9') {
        int index = format[i+1] - '0';
        if (-1 == args_array[index]->size()) {
          std::cout << "strings::Substitute format string invalid: asked for \"$";
          std::cout << index << "\", but only " << CountSubstituteArgs(args_array);
          std::cout << " args were given. Full format string was: " << format << std::endl;
          return -1;
        }
        size += args_array[index]->size();
        ++i;
      } else if ('$' == format[i+1]) {
        ++size;
        ++i;
      } else {
        std::cout << "Invalid strings::Substitute() format string: \"" << format << "\"\n";
      }
    } else {
      ++size;
    }
  }
  return size;
}

char* SubstituteToBuffer(string& format, const SubstituteArg* const* args_array, char* target) {
  for (int i = 0; i < format.length(); i++) {
    if ('$' == format[i]) {
      if (format[i+1] >= '0' && format[i+1] <= '9') {
        const SubstituteArg* src = args_array[format[i+1] - '0'];
        memcpy(target, src->data(), src->size());
        target += src->size();
        ++i;
      } else if ('$' == format[i+1]) {
        *target++ = '$';
        ++i;
      }
    } else {
      *target++ = format[i];
    }
  }
  return target;
}

char* SubstituteArg::Uint32ToBufferLeft(uint32_t value, char* scratch) {
  if (NULL == scratch) return NULL;
  int len = CalcValueLen(value);
  int loop = len;
  for (; loop > 0; loop--) {
    char ch = (value % 10) + '0';
    *(scratch + loop - 1) = ch;
    value /= 10;
  }
  scratch += len;
  return scratch;
}

char* SubstituteArg::Int32ToBufferLeft(int32_t value, char* scratch) {
  if (NULL == scratch) return NULL;
  uint32_t u_value = value;
  if (value < 0) {
    *scratch++ = '-';
    u_value = 0 - value;
  }
  return Uint32ToBufferLeft(u_value, scratch);
}

} //namespace internal 

void SubstituteAndAppend(string* output, string& format, 
    const SubstituteArg& arg0, const SubstituteArg& arg1, 
    const SubstituteArg& arg2, const SubstituteArg& arg3, 
    const SubstituteArg& arg4, const SubstituteArg& arg5,
    const SubstituteArg& arg6, const SubstituteArg& arg7, 
    const SubstituteArg& arg8, const SubstituteArg& arg9) {
  const SubstituteArg* const args_array[] = {
    &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, nullptr
  };

  int size = SubstitutedSize(format, args_array);
  if (0 == size) return;

  int original_size = output->size();
  output->resize(original_size + size);
  char* target = &*output->begin() + original_size;
  target = SubstituteToBuffer(format, args_array, target);
}

SubstituteArg::SubstituteArg(const void* value) {
  if (nullptr == value) {
    text_ = "NULL";
    size_ = strlen(text_);
  } else {
    char* ptr = scratch_ + sizeof(scratch_);
    uintptr_t num = reinterpret_cast<uintptr_t>(value);
    static const char kHexDigits[] = "0123456789abcdef";
    do {
      *--ptr = kHexDigits[num & 0xf];
      num >>= 4;
    } while (0 != num);
    *--ptr = 'x';
    *--ptr = '0';
    text_ = ptr;
    size_ = scratch_ + sizeof(scratch_) - ptr;
  }
}
