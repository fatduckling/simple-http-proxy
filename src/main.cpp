#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <cstdlib>

#include "app.h"

/**
 * Signal handler: used to gracefully stop the application
 * @param error exceptions that have occurred during signal handling
 * @param signalNumber eg 10 for SIGUSR1 etc
 */
void signalHandler(const boost::system::error_code &error, int signalNumber) {
  SPDLOG_WARN("Received signal: {} {}", signalNumber, error.message());
}

int main(int argc, char **argv) {

  // set the logging pattern
  spdlog::set_pattern("%Y-%m-%d %H:%M:%S | %^%l | %t | %s:%# | %v%$");

  // construct a signal set registered for process termination.
  boost::asio::io_service signalIoService;
  boost::asio::signal_set signals(signalIoService, SIGUSR1);

  // start_in_background an asynchronous wait for one of the signals to occur.
  signals.async_wait(signalHandler);

  // run the app
  App app;
  if (app.run(argc, argv)) {

    // wait for a signal to gracefully terminate the app
    signalIoService.run();
    SPDLOG_INFO("Signal received to stop the application");

    app.stop();

    return EXIT_SUCCESS;

  } else {

    SPDLOG_CRITICAL("Failed to start the application");
    return EXIT_FAILURE;
  }
}
