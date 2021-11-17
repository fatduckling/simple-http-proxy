#include "curl/data.h"

/**
 * Set default size to 200,000 characters
 */
size_t CurlData::s_defaultMemorySize = 100000;

CurlData::CurlData() {
  m_memory = (char *) std::malloc(s_defaultMemorySize);
  if (m_memory == nullptr) {
    throw std::runtime_error("Failed to allocate " + std::to_string(s_defaultMemorySize) + " bytes");
  }
  m_size = s_defaultMemorySize;
  m_length = 0;
}

CurlData::CurlData(CurlData &&other) noexcept {
  move(other);
}

CurlData &CurlData::operator=(CurlData &&other) noexcept {
  move(other);
  return *this;
}

CurlData::~CurlData() {
  std::free(m_memory);
}

std::string_view CurlData::to_string_view() const {
  return {m_memory, m_length};
}

void CurlData::replace_data(char *string, size_t length) {
  std::free(m_memory);
  m_memory = string;
  m_length = length;
}

char *CurlData::get_memory() {
  return m_memory;
}

size_t CurlData::get_size() const {
  return m_size;
}

size_t CurlData::get_length() const {
  return m_length;
}

void CurlData::set_memory(char *memory) {
  m_memory = memory;
}

void CurlData::set_size(size_t size) {
  m_size = size;
}

void CurlData::set_length(size_t length) {
  m_length = length;
}

void CurlData::move(CurlData &other) {
  std::free(m_memory);
  m_memory = other.m_memory;
  m_size = other.get_size();
  m_length = other.get_length();
  other.set_length(0);
  other.set_size(0);
  other.m_memory = nullptr;
  std::free(other.m_memory);
}

void CurlData::set_max_memory_size(unsigned long defaultMemorySize) {
  if (defaultMemorySize > s_defaultMemorySize) {
    s_defaultMemorySize = defaultMemorySize;
  }
}

