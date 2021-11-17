#pragma once

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
#include "spdlog/spdlog.h"

/**
 * Used to parse the command line arguments
 */
 class ProgramArguments : private boost::noncopyable {

 private:

  /**
   * Http port to serve the application in
   */
  int m_port = 0;

 public:

   /**
    * Empty constructor
    */
   ProgramArguments();

   /**
    * Empty destructor
    */
   ~ProgramArguments();

   /**
    * Parse the command line arguments
    * @param argc number of command line arguments
    * @param argv array of arguments
    * @return true if all the arguments are parsed correctly, false otherwise
    */
   [[nodiscard]] bool parse_arguments(int argc, char** argv);

   /**
    * @return the http server port
    */
   [[nodiscard]] int get_port() const;
 };
