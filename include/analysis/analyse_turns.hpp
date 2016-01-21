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

struct IntersectionType
{
    IntersectionType();

    std::uint8_t degree;
    std::uint8_t in_count;
    std::uint8_t out_count;

    bool roundabout_exit;
    bool roundabout_entry;
    bool barrier;
    bool traffic_light;

    std::int32_t example_lat;
    std::int32_t example_lon;

    bool operator<(const IntersectionType &other) const
    {
        if (degree < other.degree)
            return true;
        else if (degree > other.degree)
            return false;

        if (in_count < other.in_count)
            return true;
        else if (in_count > other.in_count)
            return false;

        if (out_count < other.out_count)
            return true;
        else if (out_count > other.out_count)
            return false;

        if (!roundabout_exit && other.roundabout_exit)
            return true;
        else if (roundabout_exit && !other.roundabout_exit)
            return false;

        if (!roundabout_entry && other.roundabout_entry)
            return true;
        else if (roundabout_entry && !other.roundabout_entry)
            return false;

        if (!barrier && other.barrier)
            return true;
        else if (barrier && !other.barrier)
            return false;

        if (!traffic_light && other.traffic_light)
            return true;
        else if (traffic_light && !other.traffic_light)
            return false;

        // turn types are equal
        return false;
    }

    std::string toString() const;
};

void analyseGraph(const graph::NodeBasedDynamicGraph &node_based_graph,
                  const std::unordered_set<NodeID> &barrier_nodes,
                  const std::unordered_set<NodeID> &traffic_lights,
                  const graph::RestrictionMap &restriction_map,
                  const std::vector<graph::QueryNode> &internal_to_external_node_map);
} // namespace analysis
} // namespace osrm

#endif // ANALYSIS_ANALYSE_TURNS_HPP_
