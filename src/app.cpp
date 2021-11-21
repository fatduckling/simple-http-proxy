#include "app.h"

App::App() = default;

App::~App() { CurlResponse::destroy_curl_handle(); };

bool App::run(int argc, char **argv) {

  // parse the command line arguments and curl handle
  if (m_programArguments.parse_arguments(argc, argv) and CurlResponse::initialise_curl_handle()) {

    // print the library version and the welcome message
    print_welcome_message();

    // start the http server in the background
    m_httpServerThread = std::make_unique<std::thread>(&App::run_web_server, this);

    return true;
  }
  return false;
}

void App::stop() {
  if (m_httpServerThread) {
    SPDLOG_WARN("Stopping the HTTP server...");
    if (m_httpSocket) { us_listen_socket_close(false, m_httpSocket); }
    m_httpServerThread->join();
    m_httpServerThread = nullptr;
    SPDLOG_INFO("Successfully joined the HTTP server thread");
  }
}

void App::print_welcome_message() {
  SPDLOG_INFO("Welcome to the Simple HTTP Proxy Server!");
  SPDLOG_INFO("Using Boost Version {}.{}.{}", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000,
              BOOST_VERSION % 100);
  SPDLOG_INFO("C++ compiler version: {}", __cplusplus);
  SPDLOG_INFO("CURL library version: {}", curl_version());
  SPDLOG_INFO("SPDLOG library version: {}.{}.{}", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
}

void App::run_web_server() {
  int port = m_programArguments.get_port();
  uWS::App().get("/*", std::bind(&App::on_http_request, this, std::placeholders::_1, std::placeholders::_2))
      .listen(port, std::bind(&App::on_http_server_open, this, std::placeholders::_1)).run();
}

void App::on_http_server_open(App::HttpSocket *listenSocket) {
  const static auto port = m_programArguments.get_port();
  m_httpSocket = listenSocket;
  if (listenSocket) {
    SPDLOG_INFO("HTTP server is listening on port {}", port);
  } else {
    SPDLOG_CRITICAL("Failed to launch HTTP server on port {}", port);
  }
}

void App::on_http_request(App::HttpResponse *res, App::HttpRequest *req) {
  auto proxyHeader = req->getHeader("proxy");
  std::string url(proxyHeader.data(), proxyHeader.length());
  if (url.empty()) {
    return res->writeStatus("400")
        ->writeHeader("Content-Type", "application/json; charset=utf-8")
        ->end("\"Proxy header is empty!\"");
  } else {
    CurlResponse response(url);
    if (response) {

      // write the http status first
      char str[20];
      std::sprintf(str, "%ld", response.get_status_code());
      res->writeStatus(str);

      CurlData &proxyResponse = response.get_http_response();
      auto httpBody = proxyResponse.to_string_view();
      const static bool enableCompression = m_programArguments.enable_compression();
      if (enableCompression) {
        unsigned long originalHttpBodyLength = httpBody.length();
        if (compress_http_response(proxyResponse)) {
          httpBody = proxyResponse.to_string_view();
          res->writeHeader("Proxy-Compressed-Length", originalHttpBodyLength);
        } else {
          SPDLOG_CRITICAL("Failed to compress HTTP response");
          return res->writeStatus("500")
              ->writeHeader("Content-Type", "application/json; charset=utf-8")
              ->end("\"Failed to compress HTTP response!\"");
        }
      }
      return res->end(httpBody);
    }
  }
  return res->writeStatus("400")
      ->writeHeader("Content-Type", "application/json; charset=utf-8")
      ->end("\"Failed to make GET request!\"");
}

bool App::compress_http_response(CurlData &httpResponse) {
  auto httpBody = httpResponse.to_string_view();
  const char *const src = httpBody.data();
  const int srcSize = (int) (httpBody.length() + 1);
  const int maxDestSize = LZ4_compressBound(srcSize);
  auto *compressedData = (char *) malloc((size_t) maxDestSize);
  long initialTime = get_timestamp();
  const size_t compressedDataSize = LZ4_compress_HC(src, compressedData, srcSize, maxDestSize, 1);
  long timeTaken = get_timestamp() - initialTime;
  if (compressedDataSize <= 0) {
    SPDLOG_ERROR("A 0 or negative result from LZ4_compress_default() indicates a failure trying to compress the data.");
  } else if (compressedDataSize > 0) {
    float compressRatio = (float) compressedDataSize / (float) srcSize;
    SPDLOG_INFO(
        "Successfully compressed the HTTP response! Ratio: {:0.2f}% Original size: {}, New size: {}. Time taken: {} ms",
        compressRatio, srcSize, compressedDataSize, timeTaken);
    httpResponse.replace_data(compressedData, compressedDataSize);
    return true;
  } else {
    SPDLOG_ERROR("Compressed data size is zero or negative");
  }
  return false;
}

long App::get_timestamp() {
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  );
  return ms.count();
}
