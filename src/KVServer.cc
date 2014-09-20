// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#include "KVServer.h"

#include<stdio.h>
#include <stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <sqlite3.h>
#include <iostream>
#include <string>

#include "Constant.h"
// #include "msg.pb.h"

namespace kvstore {
 /**
  static int sqliteCallback(void *notUsed, int argc, char** argv, char **colNames) {
    int i = 0;
    for (i = 0; i < argc; ++i) {
      std::cout << colNames[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
    }
    return 0;
  }
 */

  KVServer::KVServer() : 
    socket_desc_(-1),
    db_(nullptr),
    get_stmt_(nullptr),
    put_stmt_(nullptr) {}
  
  KVServer::~KVServer() {
    if (db_ != nullptr) {
      sqlite3_close(db_);
    }
  }

  void KVServer::Init(int port) {
    this->port_ = port;
    struct sockaddr_in server;
     
    //Create socket
    socket_desc_ = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc_ == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    //Bind
    if( bind(socket_desc_,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc_ , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
 
    InitDB();
  }

  void KVServer::InitDB() {
     // Init the database.
    CALL_SQLITE(open("739.db", &db_));
    std::cout << "Open database successfully\n";

    char *sql = nullptr, *errMsg = nullptr;
    sqlite3_stmt *stmt = nullptr;
    
    // Create table.
    sql = "SELECT COUNT(*) "\
          "FROM sqlite_master "\
          "WHERE tbl_name = 'kvstore'";

    // ret = sqlite3_exec(db, sql, sqliteCallback, (void *)buf, &errMsg);
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &stmt, nullptr));
    
    const char* text = nullptr;
    int s;
    s = sqlite3_step(stmt);
    if (s == SQLITE_ROW) {
      text = (char *)sqlite3_column_text(stmt, 0);
      // std::cout << text << "\n";
      if (atoi(text) == 0) {
        sql = "CREATE TABLE kvstore ("\
              "key char(129),"\
              "value char(2049));";
        CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &stmt, nullptr));
        s = sqlite3_step(stmt);
        if (s != SQLITE_DONE) {
          std::cout << "ERROR: Fail to create table.\n";
          exit(-1);
        } else {
          std::cout << "Create table successfully\n";
        }
      } else {
        std::cout << "Table kvstore already exist\n";
      } 
    } else {
      std::cout << "ERROR: Create table failed.\n";
      exit(-1);
    }

    sql = "SELECT value FROM kvstore WHERE key=?";
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &get_stmt_, nullptr));
    sql = "UPDATE kvstore set value=? WHERE key=?";
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &put_stmt_, nullptr));
  } 

  void KVServer::Run() {
    struct sockaddr client;
    int c, read_size, client_sock;
    char msg[kMsgMaxLen];
    c = sizeof(struct sockaddr_in);
  
    while (1) {
      //accept connection from an incoming client
      client_sock = accept(socket_desc_, (struct sockaddr *)&client, (socklen_t*)&c);
      if (client_sock < 0) {
        perror("accept failed");
      }
      puts("Connection accepted");
       
      //Receive a message from client
      while( (read_size = recv(client_sock , msg, kMsgMaxLen, 0)) > 0 ) {
        int flag;
        char* cur = msg;
        memcpy(&flag, cur, sizeof(flag));
        cur += sizeof(flag);
        switch (flag) {
          case 0: {
            int len;
            memcpy(&len, cur, sizeof(len));
            cur += sizeof(len);
            char key[len + 1];
            memcpy(key, cur, len);
            key[len] = '\0';
            char old_value[kValueMaxLen + 1];
            // Query the db.
            int ret = GetFromDB(key, old_value);
            std::cout << old_value << std::endl;
            // std::cout << "Get Value: ret # " << ret << " key # " << key << " value # " << old_value << std::endl; 
            cur = msg;
            memcpy(cur, &ret, sizeof(ret));
            cur += sizeof(ret);
            if (0 == ret) {
              std::cout << "Get: ret # " << ret << " key # " << key << " value # " << old_value << std::endl; 
              len = strlen(old_value);
              memcpy(cur, &len, sizeof(len));
              cur += sizeof(len);
              memcpy(cur, old_value, len);
              cur += len;
              *cur = '\0';
              // write(client_sock , client_message , strlen(msg));
            } else {
              *cur = '\0';
            }
            std::cout << "send msg" << std::endl;
            write(client_sock, msg, len + 8 );
            std::cout << "send msg" << std::endl;
            break;
          }
          case 1: {
            int len;
            memcpy(&len, cur, sizeof(len));
            cur += sizeof(len);
            char key[len + 1];
            memcpy(key, cur, len);
            key[len] = '\0';
            cur += len;
            memcpy(&len, cur, sizeof(len));
            cur += sizeof(len);
            char value[len + 1];
            memcpy(value, cur, len);
            value[len] = '\0';
            char old_value[kValueMaxLen + 1];
            // Query the db.
            int ret = GetFromDB(key, old_value);
            // TODO:-1 == ret, add atomic operation here to not split the get
            // and put?
            int prev_ret = ret;
            ret = PutIntoDB(key, value);
            if (-1 == prev_ret || -1 == ret) {
              cur = msg;
              int tmp = -1;
              memcpy(msg, &tmp , sizeof(tmp));
              cur += sizeof(tmp);
              *cur = '\0';
            } else {
              cur = msg;
              memcpy(msg, &prev_ret, sizeof(prev_ret));
              cur += sizeof(prev_ret);
              if (0 == prev_ret) {
                len = strlen(old_value);
                memcpy(cur, &len, sizeof(len));
                cur += sizeof(len);
                memcpy(cur, old_value, len);
                cur += len;
                // write(client_sock , client_message , strlen(msg));
              }
              *cur = '\0';
            }
            write(client_sock, msg, strlen(msg));
            break;
          }
        }
        //Send the message back to client
        // write(client_sock , client_message , strlen(msg));
      }
     
      if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
      } else if(read_size == -1) {
        perror("recv failed");
      }
    }
  }

int KVServer::GetFromDB(const char* key, char* value) {
  int ret = -1;
  CALL_SQLITE(bind_text(get_stmt_, 1, key, strlen(key), nullptr)); 
  int s = sqlite3_step(get_stmt_);
  if (s == SQLITE_ROW) {
    strcpy(value, (char*)sqlite3_column_text(get_stmt_, 0));
    ret = 0;
  } else if (s == SQLITE_DONE) {
    ret = 1;
  } else {
    ret = -1;
  }
  CALL_SQLITE(reset(get_stmt_));
  CALL_SQLITE(clear_bindings(get_stmt_));
  return ret;
}

int KVServer::PutIntoDB(const char* key,const char* value) {
  int ret = -1;
  CALL_SQLITE(bind_text(put_stmt_, 1, key, strlen(key), nullptr)); 
  CALL_SQLITE(bind_text(put_stmt_, 2, value, strlen(value), nullptr));
  int s = sqlite3_step(put_stmt_);
  if (s == SQLITE_DONE) {
    ret = 1;
  } else {
    ret = -1;
  }
  CALL_SQLITE(reset(put_stmt_));
  CALL_SQLITE(clear_bindings(put_stmt_));
  return ret;
}
} // namespace kvstore
