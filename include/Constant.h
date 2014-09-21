// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#ifndef CS739_CONSTANT_H_
#define CS739_CONSTANT_H_

namespace kvstore {
  const int kKeyMaxLen = 128;
  const int kValueMaxLen = 2048;
  const int kMsgMaxLen = kKeyMaxLen + kValueMaxLen + sizeof(int) * 3;
} // namespace kvstore

#endif // CS739_CONSTANT_H_
