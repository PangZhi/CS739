// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include <iostream>

#include "Client.h"

int main() {
  char* server_addr = "127.0.0.1:8889";
  kv739_init(server_addr);
  char* key = "hello";
  char value[4096];
  int ret = -1;
  //std::cout << kv739_put("random", "world", value) << "\n";
  //std::cout << kv739_put(key, "lala", value) << " # " << value << "\n";
  //std::cout << kv739_put(key, "lala", value) << " # " << value << "\n";
  //std::cout << kv739_get(key, value) << " # " << value << "\n";
  //std::cout << kv739_get("haha", value) << "\n";
  return 0;
}
