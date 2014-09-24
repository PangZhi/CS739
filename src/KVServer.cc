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
#include "Message.h"

namespace kvstore {
 /**
  * Another way of call sqlite.
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
    insert_stmt_(nullptr),
    update_stmt_(nullptr) {}
  
  KVServer::~KVServer() {
    if (db_ != nullptr) {
      sqlite3_finalize(get_stmt_);
      sqlite3_finalize(update_stmt_);
      sqlite3_finalize(insert_stmt_);
      sqlite3_close(db_);
    }
  }

  void KVServer::Init(int port) {
    this->port_ = port;
    struct sockaddr_in server;
     
    // Create socket.
    socket_desc_ = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc_ == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    // Prepare the sockaddr_in structure.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
     
    // Bind.
    if( bind(socket_desc_,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
    }
    puts("bind done");
     
    listen(socket_desc_ , 3);
    //Accept and incoming connection
    puts("Waiting for incoming connections...");

    // Init connection to database.
    InitDB();
  }

  void KVServer::InitDB() {
     // Init the database.
    CALL_SQLITE(open("739.db", &db_));
    std::cout << "Open database successfully\n";

    char *sql = nullptr, *errMsg = nullptr;
    sqlite3_stmt *stmt = nullptr;
    
    // We store the key-value pairs in table kvstore. If not exist, create table.
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
      if (atoi(text) == 0) {
        sql = "CREATE TABLE kvstore ("\
              "key char(129),"\
              "value char(2049));";
        CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &stmt, nullptr));
        s = sqlite3_step(stmt);
        if (s != SQLITE_DONE) {
          std::cout << "ERROR: Fail to create table.\n";
          CALL_SQLITE(close(db_));
          exit(-1);
        } else {
          std::cout << "Create table successfully\n";
        }
      } else {
        std::cout << "Table kvstore already exist\n";
      } 
    } else {
      std::cout << "ERROR: Create table failed.\n";
      CALL_SQLITE(close(db_));
      exit(-1);
    }

    // Prepare select and update statement for performance improvement.
    sql = "SELECT value FROM kvstore WHERE key=?";
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &get_stmt_, nullptr));
    sql = "INSERT INTO kvstore VALUES(?,?)";
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &insert_stmt_, nullptr));
    sql = "UPDATE kvstore SET value=? WHERE key=?";
    CALL_SQLITE(prepare_v2(db_, sql, strlen(sql) + 1, &update_stmt_, nullptr));
  } 

  void KVServer::Run() {
    struct sockaddr client;
    int c, read_size, client_sock;
    c = sizeof(struct sockaddr_in);
 
    Message msg;
    char buf[kMsgMaxLen];
    char key[kKeyMaxLen + 1];
    char value[kValueMaxLen + 1];
    char old_value[kValueMaxLen + 1];

    while (1) {
      // Accept connection from an incoming client.
      client_sock = accept(socket_desc_, (struct sockaddr *)&client, (socklen_t*)&c);
      if (client_sock < 0) {
        perror("accept failed");
      }
      puts("Connection accepted");
      
      // Receive a message from client.
      while( (read_size = recv(client_sock , buf, kMsgMaxLen, 0)) > 0 ) {
        int flag = -1;
        msg.Reset(buf, read_size);
        msg.Get(sizeof(flag), &flag);
        switch (flag) {
          case 0: {
            // Get request, message should be of the format: flag | length of
            // key | key.
            int len = -1;
            msg.GetStr(key, &len);
            // Query the db.
            int ret = GetFromDB(key, old_value);
            msg.Reset(&ret, sizeof(ret));
            // Return message: ret | length of value | value.
            if (0 == ret) {
              std::cout << "Get: ret # " << ret << " key # " << key << " value # " << old_value << std::endl; 
              msg.Append(old_value);
            } 
            break;
          }
          case 1: {
            // Put request, message should be of the format: flag | length of
            // key | key | length of value | value.
            int len = -1;
            msg.GetStr(key, &len);
            msg.GetStr(value, &len);
            // Query the db to get old value.
            int ret = GetFromDB(key, old_value);
            // TODO:-1 == ret, add atomic operation here to not split the get
            // and put?
            int prev_ret = ret;
            if (0 == prev_ret) {
              ret = UpdateDB(key, value);
            } else if (1 == prev_ret) {
              ret = InsertIntoDB(key, value);  
            }
            std::cout << "prev_ret: " << prev_ret << " ret: " << ret << std::endl;
            if (-1 == prev_ret || -1 == ret) {
              int tmp = -1;
              msg.Reset(&tmp, sizeof(tmp));
            } else {
              msg.Reset(&prev_ret, sizeof(prev_ret));
              if (0 == prev_ret) {
                std::cout << "old value: " << old_value << std::endl;
                msg.Append(old_value);
              }
            }
            break;
          }
        }
        int size = write(client_sock, msg.data(), msg.length());
        std::cout << "write size is: " << size <<" send msg size:" << msg.length() << "\n";
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

int KVServer::InsertIntoDB(const char* key,const char* value) {
  int ret = -1;
  CALL_SQLITE(bind_text(insert_stmt_, 1, key, strlen(key), nullptr)); 
  CALL_SQLITE(bind_text(insert_stmt_, 2, value, strlen(value), nullptr));
  int s = sqlite3_step(insert_stmt_);
  if (s == SQLITE_DONE) {
    ret = 1;
  } else {
    ret = -1;
  }
  CALL_SQLITE(reset(insert_stmt_));
  CALL_SQLITE(clear_bindings(insert_stmt_));
  return ret;
}

int KVServer::UpdateDB(const char* key, const char* value) {
  int ret = -1;
  CALL_SQLITE(bind_text(update_stmt_, 2, key, strlen(key), nullptr));
  CALL_SQLITE(bind_text(update_stmt_, 1, value, strlen(value), nullptr));
  int s = sqlite3_step(update_stmt_);
  if (s == SQLITE_DONE) {
    ret = 1;
  } else {
    ret = -1;
  }
  CALL_SQLITE(reset(update_stmt_));
  CALL_SQLITE(clear_bindings(update_stmt_));
}
} // namespace kvstore
