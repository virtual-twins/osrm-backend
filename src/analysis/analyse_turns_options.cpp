#include "analysis/analyse_turns_options.hpp"

#include "util/version.hpp"
#include "util/simple_logger.hpp"

#include <boost/program_options.hpp>

#include <cmath>

namespace osrm
{
namespace analysis
{

SegmentLengthClassificator::SegmentLengthClassificator(const SegmentClassificationType type,
                                                       const float length_grouping_base)
    : type(type), length_grouping_base(length_grouping_base)
{
}

std::uint32_t SegmentLengthClassificator::operator()(const EdgeWeight weight) const
{
    switch (type)
    {
    case SegmentClassificationType::LINEAR:
        return static_cast<std::uint32_t>(std::floor(weight / length_grouping_base));
    case SegmentClassificationType::EXPONENTIAL:
        return static_cast<std::uint32_t>(
            std::floor(std::log(weight) / std::log(length_grouping_base)));
    default:
        return 0; // default classification classifies all weights the same
    }
}

SegmentLengthClassificator AnalyseTurnsOptions::makeClassificator() const
{
    return SegmentLengthClassificator(static_cast<SegmentClassificationType>(classification_type_uint8), length_grouping_base);
}

bool AnalyseTurnsOptions::ParseArguments(int argc, char *argv[])
{
    // declare a group of options that will be allowed only on command line
    boost::program_options::options_description generic_options("Options");
    generic_options.add_options()("help,h", "Show this help message")(
        "group,g", boost::program_options::value<float>(&length_grouping_base)->default_value(10.f),
        "Grouping Length in Units of EdgeWeight")(
        "type,t", boost::program_options::value<std::uint8_t>(&classification_type_uint8)
                      ->default_value(static_cast<std::uint8_t>(SegmentClassificationType::LINEAR)),
        "Grouping Type: LINEAR(0) / Exponential(1)")(
        "neighbourhood,n",
        boost::program_options::value<std::uint8_t>(&neighbourhood_size)->default_value(1),
        "Size of the classification Neighbourhood");

    // hidden options, will be allowed both on command line and in config file, but will not be
    // shown to the user
    boost::program_options::options_description hidden_options("Hidden Options");
    hidden_options.add_options()("input,i", boost::program_options::value<std::string>(&input_file),
                                 "Path to input file in .osrm format");

    // positional option
    boost::program_options::positional_options_description positional_options;
    positional_options.add("input", 1);

    // combine above options for parsing
    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic_options).add(hidden_options);

    // parse command line options
    try
    {
        boost::program_options::variables_map option_variables;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                          .options(cmdline_options)
                                          .positional(positional_options)
                                          .run(),
                                      option_variables);

        if (option_variables.count("help"))
        {
            util::SimpleLogger().Write() << generic_options;
            return false;
        }

        boost::program_options::notify(option_variables);

        if (!option_variables.count("input"))
        {
            util::SimpleLogger().Write() << "Missing Input File (positional option)";
            return false;
        }
    }
    catch (std::exception &e)
    {
        util::SimpleLogger().Write(logWARNING) << e.what();
        return false;
    }

    return true;
}

} // namespace analysis
} // namespace osrm

std::ostream &operator<<(std::ostream &os, const osrm::analysis::SegmentClassificationType type)
{
    os << static_cast<std::uint32_t>(type);
    return os;
}

std::istream &operator>>(std::istream &is, osrm::analysis::SegmentClassificationType &type)
{
    std::uint32_t tmp;
    is >> tmp;
    type = static_cast<osrm::analysis::SegmentClassificationType>(tmp);
    return is;
}
