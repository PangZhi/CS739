// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include "Message.h"

#include "string.h"

#include "Constant.h"

namespace kvstore {
   
Message::Message() {
  data_ = new char[kMsgMaxLen];
  size_ = kMsgMaxLen;
  l_ = 0;
  r_ = 0;
}

Message::Message(const char* src, const int len) {
  int new_len = kMsgMaxLen > len ? kMsgMaxLen : len;
  data_ = new char[new_len];
  size_ = new_len;
  
  memcpy(data_, src, len);
  l_ = 0;
  r_ = len;
}



Message::~Message() {
  size_ = 0;
  l_ = 0;
  r_ = 0;
  delete data_;
}
 
void Message::Clear() {
  l_ = 0;
  r_ = 0;
}

void Message::Append(const void* buf, const int len) {
  if (r_ + len > size_) {
    char *tmp = data_;
    int new_len = (size_ * 2) > (r_ + len) ? (size_ * 2) : (r_ + len);
    data_ = new char[new_len];
    memcpy(data_, tmp, r_);
    delete tmp;
  }
  memcpy(&data_[r_], buf, len);
  r_ = r_ + len;
}

void Message::Append(const void* buf) {
  int len = strlen((char *)buf);
  Append(&len, sizeof(len));
  Append(buf, len);
}

void Message::Get(const int len, void* buf) {
  memcpy(buf, &data_[l_], len);
  l_ += len;
}

void Message::GetStr(void *buf, int* len) {
  memcpy(len, &data_[l_], sizeof(int));
  l_ += sizeof(int);
  memcpy(buf, &data_[l_], *len);
  l_ += *len;
  ((char*)buf)[*len] = '\0';
}

void Message::Reset(const void* src, const int len) {
  if (len > size_){
    delete data_;
    int new_len = (size_ * 2) > len ? (size_ * 2) : len;
    data_ = new char[new_len];
    size_ = new_len;
  }
  memcpy(data_, src, len);
  l_ = 0;
  r_ = len;
}
} // namespace kvstore

