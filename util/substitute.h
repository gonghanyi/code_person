#include <string>
#include "number.h"
#include <cstring>

using std::string;

#ifndef STRINGS_SUBSTITUTE_H_
#define STRINGS_SUBSTITUTE_H_

namespace internal {

static const int kFastToBufferSize = 32;

class SubstituteArg {
public:
  inline SubstituteArg(const char* value) : text_(value), size_(NULL==value ? 0 : strlen(text_)) {}

  inline SubstituteArg(const string& value) : text_(value.data()), size_(value.size()) {}

  inline SubstituteArg(char value) : text_(scratch_), size_(1) { scratch_[0] = value; }

  inline SubstituteArg(short value) : text_(scratch_), size_(UtilNumber::FastInt32ToBufferLeft(value, scratch_)-scratch_) {}

  inline SubstituteArg(int value) : text_(scratch_), size_(UtilNumber::FastInt32ToBufferLeft(value, scratch_)-scratch_) {}

  inline SubstituteArg(uint32_t value) : text_(scratch_), size_(UtilNumber::FastUInt32ToBufferLeft(value, scratch_)-scratch_) {}

  SubstituteArg(const void* value);

  inline const char* data() const { return text_; }
  inline int size() const { return size_; }

  static const SubstituteArg kNoArg;  //Indicates that no argument was given

private:
  int CalcValueLen(int32_t value) {
    int len = 0;
    while(value) {
      len++;
      value /= 10;
    }
    return len;
  }

  char* Uint32ToBufferLeft(uint32_t value, char* scratch);

  char* Int32ToBufferLeft(int32_t value, char* scratch);

private:
  inline SubstituteArg() : text_(NULL), size_(-1) {}

  const char* text_;
  int size_;
  char scratch_[kFastToBufferSize];
};

int SubstitutedSize(string& format, const SubstituteArg* const* args_array);

char* SubstituteToBuffer(string& format, const SubstituteArg* const* args_array, char* target);

} //namespace internal

void SubstituteAndAppend(
  string* output, string& format, 
  const internal::SubstituteArg& arg0 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg1 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg2 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg3 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg4 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg5 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg6 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg7 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg8 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg9 = internal::SubstituteArg::kNoArg);

inline string Substitute(
  string& format, 
  const internal::SubstituteArg& arg0 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg1 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg2 = internal::SubstituteArg::kNoArg, 
  const internal::SubstituteArg& arg3 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg4 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg5 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg6 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg7 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg8 = internal::SubstituteArg::kNoArg,
  const internal::SubstituteArg& arg9 = internal::SubstituteArg::kNoArg) {
  string result;
  SubstituteAndAppend(&result, format, arg0, arg1, arg2, arg3);
  return result;
}



#endif




