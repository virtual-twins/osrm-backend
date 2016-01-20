#include "analysis/analyse_turns_options.hpp"
#include "util/simple_logger.hpp"

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <exception>
#include <new>

using namespace osrm;

int main(int argc, char *argv[]) try
{
    util::LogPolicy::GetInstance().Unmute();
    analysis::AnalyseTurnsOptions analyse_turn_options;

    if( !analyse_turn_options.ParseArguments(argc, argv) )
      return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
catch (const std::bad_alloc &e)
{
    util::SimpleLogger().Write(logWARNING) << "[exception] " << e.what();
    util::SimpleLogger().Write(logWARNING)
        << "Please provide more memory or consider using a larger swapfile";
    return EXIT_FAILURE;
}
catch (const std::exception &e)
{
    util::SimpleLogger().Write(logWARNING) << "[exception] " << e.what();
    return EXIT_FAILURE;
}
