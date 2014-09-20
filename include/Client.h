// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#ifndef CS739_CLIENT_H_
#define CS739_CLIENT_H_
/**
 * @brief This is the client interface file.
 */

/**
 * @brief Init the client and connect to the server.
 * @param "host:port".
 * @return Status code. 0 for success and -1 for failure.
 */
 int kv739_init(char *server);

/**
 * @brief Retrieve the corresponding key value. If the key is present, store
 * the value in the string.
 * @param key Key.
 * @param value Value.
 * @return 0 for success with old value, 1 for success with no old value, -1
 * for failure.
 */
 int kv739_get(char* key, char* value);

/**
 * @brief Perform a get operation on old_value and store value.
 * @param key Key.
 * @param value The value to store.
 * @param old_value The old value.
 * @return 0 for success with old value, 1 for success with no old value, -1
 * for failure.
 */
 int kv739_put(char* key, char* value, char* old_value);

 #endif // CS739_CLIENT_H_
