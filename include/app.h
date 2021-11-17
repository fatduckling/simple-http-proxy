#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <functional>
#include <boost/version.hpp>
#include "lz4/lz4hc.h"
#include "uWebSockets/App.h"
#include "spdlog/spdlog.h"
#include "program_arguments.h"
#include "curl/response.h"

class App {

 private:

  typedef uWS::HttpRequest HttpRequest;
  typedef uWS::HttpResponse<false> HttpResponse;
  typedef us_listen_socket_t HttpSocket;

  /**
   * Program argument handler
   */
  ProgramArguments m_programArguments;

  /**
   * Http socket
   */
  HttpSocket *m_httpSocket = nullptr;

  /**
   * Thread that is responsible for running the Http server
   */
  std::unique_ptr<std::thread> m_httpServerThread = nullptr;

 public:

  /**
   * Empty constructor
   */
  App();

  /**
   * Empty destructor
   */
  ~App();

  /**
   * Run the application with the program arguments
   * @param argc number of program arguments
   * @param argv array of program arguments
   * @return the exit code
   */
  [[nodiscard]] bool run(int argc, char **argv);

  /**
   * Stop the application
   */
  void stop();

 private:

  /**
   * Print the welcome message which involves writing the current library versions used
   */
  void static print_welcome_message();

  /**
   * Start the web server
   */
  void run_web_server();

  /**
   * Callback when the http server is opened (successful or not)
   * @param socket http socket that handles the http requests. This is null if the http server fails to start.
   */
  void on_http_server_open(HttpSocket *socket);

  /**
   * Serve a HTTP request
   * @param response http response
   * @param request http request
   */
  void on_http_request(HttpResponse *response, HttpRequest *request);
};
