// CS739 key-value store project by Shu Wang, Ce Zhang and Jing Fan.

#ifndef CS739_MESSAGE_H_
#define CS739_MESSAGE_H_

/**
 * @brief This is the Mesage class.
 */

namespace kvstore {
class Message {
 public:
  /**
   * @brief Constructor.
   */
   Message();

  /**
   * @brief Constructor.
   * @param src Source buffer to copy.
   * @param len Length of the buffer.
   */
  Message(const char* src, const int len);

  /**
   * @brief Destructor.
   */
  ~Message();
 
  /**
   * @brief Getter for data.
   */
  inline char* data() const { return data_; }

  /**
   * @brief Getter for length.
   */
  inline int length() const { return r_ - l_; }

  /**
   * @brief Clear the message.
   */
  void Clear();

  /**
   * @brief Reset to a new buffer.
   * @param src Source buffer to copy.
   * @param len Length of content.
   */
  void Reset(const void* src, const int len);
  
  /**
   * @brief Append str to the end of this message.
   * @param buf Str.
   * @param len Length of the content.
   */
  void Append(const void* buf, const int len);

  /**
   * @brief Append str to the end of this message, should first append the
   * length of buffer, then the buffer.
   * @param buf str.
   */
  void Append(const void* buf);

  /**
   * @brief Get the first bytes of the message and copy to buf.
   * @param len The length of content.
   * @param buf The place to copy result.
   */
  void Get(const int len, void* buf);

  /**
   * @brief Get str from the message.
   * @param buf The place to copy the str.
   * @param len The length of the str.
   */
  void GetStr(void* buf, int* len);

 private:
  // Data.
  char *data_;
  // Left pointer.
  int l_;
  // Write pointer.
  int r_;
  // Size of buffer.
  int size_;
}; // Message  

} // namespace kvstore



 #endif // CS739_MESSAGE_H_
