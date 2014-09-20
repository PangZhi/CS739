// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include "Client.h"

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "Constant.h"
#include "string_util.h"
 // #include "msg.pb.h"

using namespace kvstore;

// socket connection
int sock;

int kv739_init(char *server_str) {
  struct sockaddr_in server;
  
  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
    std::cout << "ERROR: Could not create socket" << std::endl;
  }

  std::vector<std::string> strs = util::Split(std::string(server_str), ':');
  if (strs.size() != 2) {
    std::cout << "ERROR: Could not format the server addr" << std::endl;
  }

  server.sin_addr.s_addr = inet_addr(strs[0].c_str());
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(strs[1].c_str()));
  
  std::cout << "Connect to " << server_str << "\n";

  if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
    std::cout << "ERROR: Fail to connect to server.\n";
    exit(-1);
  }
  
}

// 4 bytes int for the kind of operation | 4 bytes int for the length of
// key | key | 4 bytes int for the length of value (if any) | value 
// operation type: get->0, put->1
int kv739_get(char* key, char* value) {
  std::cout << "Get message.\n"; 
  // Generate message.
  char msg[kMsgMaxLen];
  int pos = 0;
  int flag = 0;
  memcpy(msg, &flag, sizeof(flag));
  pos += sizeof(flag);
  int len = strlen(key);
  memcpy(msg + pos, &len, sizeof(len));
  pos += sizeof(len);
  memcpy(msg + pos, key, len);
  pos += len;
  msg[pos] = '\0'; 

  if (send(sock , msg, pos , 0) < 0) {
    puts("Send failed");
    return -1;
  }

  // memset(msg, 0, kMsgMaxLen);
  //Receive a reply from the server
  if (recv(sock , msg, kMsgMaxLen, 0) < 0) {
    puts("recv failed");
    return -1;
  }
 
  puts("recv success");
  int ret = 0;
  pos = 0;
  memcpy(&ret, msg, sizeof(ret));
  pos += sizeof(ret);
 
  puts("recv success");
  if (0 == ret) {
    memcpy(&len, msg + pos, sizeof(len));
    std::cout << len;
  puts("recv success");
    pos += sizeof(len);
    memcpy(value, msg + pos, len);
  puts("recv success");
    pos += len;
    std::cout << "ret :" << ret << "len :" << len << "\n";
    value[pos] = '\0';
  }
  return ret;
}

int kv739_put(char* key, char* value, char* old_value) {
  // Generate message.
  std::cout << "Put Message.\n";
  char msg[kMsgMaxLen];
  int pos = 0;
  int flag = 1;
  memcpy(msg, &flag, sizeof(flag));
  pos += sizeof(flag);
  int len = strlen(key);
  memcpy(msg + pos, &len, sizeof(len));
  pos += sizeof(len);
  memcpy(msg + pos, key, len);
  pos += len;
  len = strlen(value);
  memcpy(msg + pos, &len, sizeof(len));
  pos += sizeof(len);
  memcpy(msg + pos, value, len);
  pos += len;
  msg[pos] = '\0'; 

  std::cout << "Send Message: " << &msg[8] << " Pos: " << pos << std::endl;
  std::cout << strlen(msg) << std::endl;
  if (send(sock , msg, pos , 0) < 0) {
    
    std::cout << "send hah" << std::endl;
    puts("Send failed");
    return -1;
  }
    std::cout << "send hah" << std::endl;
  puts("Send succeed");
  //Receive a reply from the server
  if (recv(sock , msg, kMsgMaxLen, 0) < 0) {
    puts("recv failed");
    return -1;
  }
  
  int ret = 0;
  pos = 0;
  memcpy(&ret, msg, sizeof(ret));
  pos += sizeof(ret);
 
  if (0 == ret) {
    memcpy(&len, msg + pos, sizeof(len));
    pos += sizeof(len);
    memcpy(old_value, msg + pos, len);
    pos += len;
    old_value[pos] = '\0';
  }
  return ret;
}
