
// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "timer.h"

#include "Client.h"

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

void test_correctness_set_then_get(){
	std::cout << "Running test [test_correctness_set_then_get]..." << std::endl;
	const int NTEST = 100;
	char key[129];
 	char value[2049];
  	char value2[2049];
	for(int i=0;i<NTEST;i++){
	  	gen_random(key, 128);
	  	gen_random(value, 2018);
	  	kv739_put(key, value, value2);
	  	kv739_get(key, value2);
	  	assert(strcmp(value, value2) == 0);
  	}
  	std::cout << "Finished test [test_correctness_set_then_get]!" << std::endl;
}

void test_throughput_set(){
  std::cout << "Running test [test_throughput_set]..." << std::endl;
  const int NTEST = 100;
  char * keys = new char[129*NTEST];
  char value[2049];
  gen_random(value, 2018);
  char value2[2049];

  for(int i=0;i<NTEST;i++){
      gen_random(&keys[i*129], 128);
  }

  Timer t;
  for(int i=0;i<NTEST;i++){
      gen_random(&keys[i*129], 128);
      gen_random(value, 2018);
      kv739_put(&keys[i*129], value, value2);
  }
  double elapsed = t.elapsed();
  std::cout << "Finished test [test_throughput_set]!" << std::endl;
  std::cout << "   Throughput of put: " << (1.0*NTEST/elapsed) << " puts/secs..." << std::endl;
}

void test_throughput_get(){
  std::cout << "Running test [test_throughput_get]..." << std::endl;
  const int NTEST = 100;
  char * keys = new char[129*NTEST];
  char value[2049];
  char value2[2049];

  for(int i=0;i<NTEST;i++){
      gen_random(&keys[i*129], 128);
      gen_random(value, 2018);
      kv739_put(&keys[i*129], value, value2);
  }

  Timer t;
  for(int i=0;i<NTEST;i++){
      kv739_get(&keys[i*129], value2);
  }
  double elapsed = t.elapsed();
  std::cout << "Finished test [test_throughput_get]!" << std::endl;
  std::cout << "   Throughput of get: " << (1.0*NTEST/elapsed) << " puts/secs..." << std::endl;
}


int main() {
  char* server_addr = "127.0.0.1:8889";
  kv739_init(server_addr);

  test_correctness_set_then_get();
  test_throughput_set();
  test_throughput_get();

  //char* key = "hello";
  //char value[4096];
  //int ret = -1;
  ////std::cout << kv739_put("random", "world", value) << "\n";
  ////std::cout << kv739_put(key, "lala", value) << " # " << value << "\n";
  ////std::cout << kv739_put(key, "lala", value) << " # " << value << "\n";
  ////std::cout << kv739_get(key, value) << " # " << value << "\n";
  ////std::cout << kv739_get("haha", value) << "\n";
  return 0;
}
