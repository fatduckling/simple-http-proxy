#include "program_arguments.h"

ProgramArguments::ProgramArguments() = default;

ProgramArguments::~ProgramArguments() = default;

bool ProgramArguments::parse_arguments(int argc, char **argv) {

  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("port", po::value<int>(), "the HTTP port to launch the server on");

  po::variables_map vm;

  try {

    // parse the program arguments
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.empty() or vm.count("help")) {
      std::cout << desc << std::endl;
      return false;
    }

    if (vm.count("port")) {
      m_port = vm["port"].as<int>();
    } else {
      SPDLOG_CRITICAL("HTTP port was not set");
      std::cout << desc << std::endl;
    }

    return true;
  }
  catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cout << desc << std::endl;
  }
  catch (...) {
    SPDLOG_CRITICAL("Failed to parse program arguments");
  }

  return false;
}

int ProgramArguments::get_port() const {
  return m_port;
}
