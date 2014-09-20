// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include "KVServer.h" 

int main() {
  kvstore::KVServer* server = new kvstore::KVServer();
  server->Init(8888);
  server->Run();
}
