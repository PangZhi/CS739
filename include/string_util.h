#ifndef STRING_UTIL_H_
#define STRING_UTIL_H_

#include <sstream>
#include <string>
#include <vector>

namespace util {

std::vector<std::string> Split(const std::string& s, const char delim) {
  std::vector<std::string> elems;
  std::string item;
  std::stringstream ss(s);
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

} // namespace util

#endif

