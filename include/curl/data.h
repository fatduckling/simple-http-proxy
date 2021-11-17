#pragma once

#include <cstdlib>
#include <stdexcept>
#include <string_view>
#include <cstring>
#include <boost/noncopyable.hpp>

/**
 * Simple class to hold the HTTP response and headers in a C type string
 */
struct CurlData : private boost::noncopyable {

 private:

  /**
   * Default memory size
   */
  static size_t s_defaultMemorySize;

  /**
   * HTTP response string
   */
  char *m_memory = nullptr;

  /**
   * Total size of the response string
   */
  size_t m_size = 0;

  /**
   * Length of the HTTP response string
   */
  size_t m_length = 0;

 public:

  /**
   * Create some default amount of memory
   */
  CurlData();

  CurlData(CurlData &&other) noexcept; // move constructor
  CurlData &operator=(CurlData &&other) noexcept; // move assignment

  /**
   * Delete the created memory
   */
  ~CurlData();

  /**
   * @return the string representing the memory
   */
  [[nodiscard]] std::string_view to_string_view() const;

  /**
   * Replace the current string with a new one
   * @param string new string to replace
   * @param length length of the new string
   */
  void replace_data(char *string, size_t length);

  /*
   * Standard getters and setters
   */

  char *get_memory();
  [[nodiscard]] size_t get_size() const;
  [[nodiscard]] size_t get_length() const;
  void set_memory(char *memory);
  void set_size(size_t size);
  void set_length(size_t length);

  /**
   * Move constructor
   * @param other to move
   */
  void move(CurlData &other);

  /**
   * Set the default memory size
   * @param defaultMemorySize default memory size
   */
  static void set_max_memory_size(unsigned long defaultMemorySize);
};
