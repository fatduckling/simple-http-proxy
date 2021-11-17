#include "curl/response.h"

CURL *CurlResponse::s_curlHandle = nullptr;

CurlResponse::CurlResponse(const std::string &url) {
  curl_easy_setopt(s_curlHandle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(s_curlHandle, CURLOPT_WRITEFUNCTION, curl_write_callback);
  curl_easy_setopt(s_curlHandle, CURLOPT_WRITEDATA, &m_httpResponse);
  m_curlCode = curl_easy_perform(s_curlHandle);
  double timeTaken; // in seconds
  curl_easy_getinfo(s_curlHandle, CURLINFO_RESPONSE_CODE, &m_statusCode);
  curl_easy_getinfo(s_curlHandle, CURLINFO_TOTAL_TIME, &timeTaken);
  if (m_curlCode == CURLE_OK) {
    m_timeTaken = (long) (timeTaken * 1000.0);
    CurlData::set_max_memory_size(m_httpResponse.get_length());
    SPDLOG_INFO("Successfully made a GET request with {} status code to {} in {} ms",
                m_statusCode, url, m_timeTaken);
  } else {
    SPDLOG_WARN("Failed to make HTTP request to: {}. CURL status code: {}. HTTP status code: {}",
                url, m_curlCode, m_statusCode);
  }
}

CurlResponse::CurlResponse(CurlResponse &&other) noexcept {
  m_statusCode = other.m_statusCode;
  m_httpResponse = std::move(other.m_httpResponse);
  m_timeTaken = other.m_timeTaken;
  m_curlCode = other.m_curlCode;
}

CurlResponse &CurlResponse::operator=(CurlResponse &&other) noexcept {
  m_statusCode = other.m_statusCode;
  m_httpResponse = std::move(other.m_httpResponse);
  m_timeTaken = other.m_timeTaken;
  m_curlCode = other.m_curlCode;
  return *this;
}

CurlResponse::operator bool() const {
  return m_curlCode == CURLE_OK;
}

bool CurlResponse::initialise_curl_handle() {
  if (s_curlHandle == nullptr) {
    if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
      SPDLOG_CRITICAL("Failed to initialise CURL globally");
      return false;
    } else {
      s_curlHandle = curl_easy_init();
      if (s_curlHandle == nullptr) {
        SPDLOG_CRITICAL("Failed to initialise CURL handle");
        return false;
      } else {
        return true;
      }
    }
  }
  return s_curlHandle != nullptr;
}

void CurlResponse::destroy_curl_handle() {
  if (s_curlHandle != nullptr) {
    curl_easy_cleanup(s_curlHandle);
    s_curlHandle = nullptr;
    curl_global_cleanup();
  }
}

size_t CurlResponse::curl_write_callback(char *contents, size_t size, size_t nmemb, CurlData *data) {
  size_t realSize = size * nmemb;
  char *ptr;
  if (auto memoryRequired = data->get_length() + realSize + 1;
      memoryRequired > data->get_size()) {
    data->set_size(memoryRequired * 2);
    ptr = (char *) realloc(data->get_memory(), memoryRequired * 2);
    data->set_memory(ptr);
  } else {
    ptr = data->get_memory();
  }
  auto length = data->get_length();
  std::memcpy(&(ptr[length]), contents, realSize);
  length += realSize;
  data->set_length(length);
  ptr[length] = 0;
  return realSize;
}

long CurlResponse::get_status_code() const {
  return m_statusCode;
}

CurlData &CurlResponse::get_http_response() {
  return m_httpResponse;
}

long CurlResponse::get_time_taken() const {
  return m_timeTaken;
}
