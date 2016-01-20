#ifndef ANALYSIS_ANALYSE_TURNS_OPTIONS_HPP_
#define ANALYSIS_ANALYSE_TURNS_OPTIONS_HPP_

#include "util/typedefs.hpp"

#include <cstdint>
#include <cstddef>

#include <iostream>

#include <string>

namespace osrm
{
namespace analysis
{

enum class SegmentClassificationType : std::uint32_t
{
    LINEAR,
    EXPONENTIAL
};

struct SegmentLengthClassificator
{
    SegmentLengthClassificator(const SegmentClassificationType type,
                               const float length_grouping_base);

    std::uint32_t operator()(const EdgeWeight) const;

    const SegmentClassificationType type;
    const float length_grouping_base;
};

struct AnalyseTurnsOptions
{
    bool ParseArguments(int argc, char *argv[]);

    std::string graph_file;
    std::string restriction_file;

    std::uint8_t neighbourhood_size;
    float length_grouping_base;
    std::uint8_t classification_type_uint8;

    SegmentLengthClassificator makeClassificator() const;
};

} // namespace analysis
} // namespace osrm

#endif // ANALYSIS_ANALYSE_TURNS_OPTIONS_HPP_
