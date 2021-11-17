#pragma once

#include <string>
#include <utility>
#include <boost/noncopyable.hpp>
#include <curl/curl.h>
#include "spdlog/spdlog.h"
#include "data.h"

/**
* Class used to contain the HTTP responses of CURL requests
*/
class CurlResponse : private boost::noncopyable {

 private:

  /**
   * HTTP status code
   */
  long m_statusCode = 0;

  /**
   * Holds the HTTP response
   */
  CurlData m_httpResponse;

  /**
   * Time taken (in milliseconds) to make the HTTP request
   */
  long m_timeTaken = 0;

  /**
   * CURL code
   */
  unsigned m_curlCode = 0;

  /**
   * CURL handle
   */
  static CURL *s_curlHandle;

 public:

  /**
   * Make a CURL request to an URL
   * @param url to make the curl request
   */
  CurlResponse(const std::string &url);

  /*
   * Standard operations
   */

  CurlResponse(CurlResponse &&other) noexcept; // move constructor
  CurlResponse &operator=(CurlResponse &&other) noexcept; // move assignment
  explicit operator bool() const; // bool operator

  /**
   * Initialise the curl handle
   * @return true if the handle is initialised correctly, or false otherwise
   */
  [[nodiscard]] static bool initialise_curl_handle();

  /**
   * Destroy the CURL handle
   */
  static void destroy_curl_handle();

  /*
   * Standard getters
   */

  [[nodiscard]] long get_status_code() const;
  [[nodiscard]] const CurlData &get_http_response() const;
  [[nodiscard]] long get_time_taken() const;

 private:

  /**
   * Used to write the HTTP response output into a MemoryStruct
   * @param contents string contents
   * @param size always 1
   * @param nmemb size of that string
   * @param data holds the HTTP response string and its size
   * @return the amount of bytes read
   */
  static size_t curl_write_callback(char *contents, size_t size, size_t nmemb, CurlData *data);
};
