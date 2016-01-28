#ifndef ANALYSIS_ANALYSE_TURNS_HPP_
#define ANALYSIS_ANALYSE_TURNS_HPP_

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>

#include "util/typedefs.hpp"
#include "graph/query_node.hpp"
#include "graph/node_based_graph.hpp"
#include "graph/restriction_map.hpp"

namespace osrm
{
namespace analysis
{

enum LengthClass
{
    SHORT,
    NORMAL,
    NUM_LENGTH_CLASSES
};

#define SMALLER_OR_LARGER(x, y)                                                                    \
    if ((x) < (y))                                                                                 \
        return true;                                                                               \
    else if ((y) < (x))                                                                            \
    return false

#define SMALLER_OR_LARGER_BOOL(x, y)                                                               \
    if (!(x) && (y))                                                                               \
        return true;                                                                               \
    else if (!(y) && x)                                                                            \
    return false

struct IntersectionType
{
    IntersectionType();

    std::uint8_t neighbours;
    std::uint8_t in_count[static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES)];
    std::uint8_t out_count[static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES)];

    bool is_roundabout;
    bool barrier;
    bool traffic_light;

    std::uint16_t possible_turns;

    bool operator<(const IntersectionType &other) const
    {
        SMALLER_OR_LARGER(neighbours, other.neighbours);
        for (std::size_t i = 0; i < static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES); ++i)
        {
            SMALLER_OR_LARGER(in_count[i], other.in_count[i]);
            SMALLER_OR_LARGER(out_count[i], other.out_count[i]);
        }
        SMALLER_OR_LARGER(possible_turns,other.possible_turns);

        SMALLER_OR_LARGER_BOOL(is_roundabout, other.is_roundabout);
        SMALLER_OR_LARGER_BOOL(barrier, other.barrier);
        SMALLER_OR_LARGER_BOOL(traffic_light, other.traffic_light);

        // turn types are equal
        return false;
    }

    std::string toString() const;
};

#undef SMALLER_OR_LARGER
#undef SMALLER_OR_LARGER_BOOL

void analyseGraph(const graph::NodeBasedDynamicGraph &node_based_graph,
                  const std::unordered_set<NodeID> &barrier_nodes,
                  const std::unordered_set<NodeID> &traffic_lights,
                  const graph::RestrictionMap &restriction_map,
                  const std::vector<graph::QueryNode> &internal_to_external_node_map);
} // namespace analysis
} // namespace osrm

#endif // ANALYSIS_ANALYSE_TURNS_HPP_
