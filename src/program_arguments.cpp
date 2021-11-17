#include "program_arguments.h"

ProgramArguments::ProgramArguments() = default;

ProgramArguments::~ProgramArguments() = default;

bool ProgramArguments::parse_arguments(int argc, char **argv) {

  CLI::App app{"Allowed options"};

  app.add_option("-p,--port", m_port, "the HTTP port to launch the server on")->required(true);
  app.add_flag("-e,--enable-compression",
               m_compressBody, "enable compression on the HTTP body with LZ4 compression method to save bandwidth");

  try {
    app.parse(argc, argv);
    return true;
  } catch (const CLI::ParseError &e) {
    std::cerr << e.what() << std::endl;
    std::cout << app.help() << std::endl;
  }
  return false;
}

int ProgramArguments::get_port() const {
  return m_port;
}

bool ProgramArguments::enable_compression() const {
  return m_compressBody;
}
