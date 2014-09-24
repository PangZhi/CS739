// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#ifndef CS739_SERVER_H_
#define CS739_SERVER_H_

#include <sqlite3.h>
#define CALL_SQLITE(f) \
{\
  int i;           \
  i = sqlite3_##f;\
  if (i != SQLITE_OK) {\
    std::cout << #f << " failed with status " << i << "\n";\
    if (db_!= nullptr) sqlite3_close(db_);\
    exit(-1);\
  }\
}

/**
 * @brief This is the server interface file.
 */

namespace kvstore {
class KVServer {
 public:
  /**
   * @brief Constructor.
   */
  KVServer();

  /**
   * @brief Distructor.
   */
  virtual ~KVServer();

  /**
   * @brief Init the server. Handle with failure, listen to the port.
   * @param port The port to listen.
   */
  void Init(int port);

  /**
   * @brief Run the server.
   */
  void Run();

 private:
  /**
   * @brief Init connection with database. If there is no table kvstore, create
   * the table kvstore.
   */
  void InitDB();
  
  /**
   * @brief Get data with key from db and store in value.
   * @param key The key.
   * @param value The value get from db.
   * @return 0 if success, 1 no such key, -1 if not.
   */
  int GetFromDB(const char* key, char* value); 

  /**
   * @brief Insert <key, value> pairs into database.
   * @param key The key.
   * @param value The value. 
   * @return 0 if success, -1 if not.
   */
  int InsertIntoDB(const char* key, const char* value);
  
  /**
   * @brief Update the value of the key.
   * @param key The key.
   * @param value The new value.
   * @return 0 if success, -1 if not.
   */
  int UpdateDB(const char* key, const char* value);

  // Port number.
  int port_;
  // Socket number.
  int socket_desc_;
  // Pointer to sqlite3.
  sqlite3 *db_;
  // For get.
  sqlite3_stmt *get_stmt_;
  // For insert.
  sqlite3_stmt *insert_stmt_;
  // For update.
  sqlite3_stmt *update_stmt_;
}; // KVServer
} // namespace kvstore



#endif // CS739_SERVER_H_
