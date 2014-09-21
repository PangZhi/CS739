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
#include "Message.h"
#include "string_util.h"


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

// Send Message: 4 bytes int for the kind of operation | 4 bytes int for the length of
// key | key | 4 bytes int for the length of value (if any) | value 
// operation type: get->0, put->1
// Receive Message: 4 bytes int for return value | 4 bytes int for the length
// of old_value (if any) | old_value
int kv739_get(char* key, char* value) {
  // Generate message.
  int pos = 0;
  int flag = 0;
 
  Message msg;
  msg.Append(&flag, sizeof(flag));
  msg.Append(key);

  if (send(sock , msg.data(), msg.length() , 0) < 0) {
    puts("Send failed");
    return -1;
  }

  //Receive a reply from the server
  char buf[kMsgMaxLen];
  int size = -1;
  if ((size = recv(sock , buf, kMsgMaxLen, 0)) < 0) {
    puts("recv failed");
    return -1;
  }

  puts("recv success");
  std::cout << "read size is: " << size << std::endl;
  
  int ret = 0;
  msg.Reset(buf, size);
  msg.Get(sizeof(ret), &ret);

  if (0 == ret) {
    int len = -1;
    msg.GetStr(value, &len);
    std::cout << "ret :" << ret << " len :" << len << " value :" << value <<"\n";
  }
  return ret;
}

int kv739_put(char* key, char* value, char* old_value) {
  // Generate message.
  Message msg;
  int flag = 1;
  msg.Append(&flag, sizeof(flag));
  msg.Append(key);
  msg.Append(value);

  if (send(sock , msg.data(), msg.length(), 0) < 0) {
    puts("Send failed");
    return -1;
  }
  puts("Send succeed");
  
  char buf[kMsgMaxLen];
  int size = -1;
  //Receive a reply from the server
  if ((size = recv(sock , buf, kMsgMaxLen, 0) < 0)) {
    puts("recv failed");
    return -1;
  }
  
  int ret = 0;
  std::cout << "Read size is: " << size << std::endl;
  msg.Reset(buf, size);
  msg.Get(sizeof(ret), &ret);

  if (0 == ret) {
    int len = -1;
    msg.GetStr(old_value, &len);
  }
  std::cout << "Client receive ret: " << ret << " value: " << value << std::endl;
  return ret;
}
