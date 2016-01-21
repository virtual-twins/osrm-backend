#include "analysis/analyse_turns.hpp"
#include "util/simple_logger.hpp"
#include "util/coordinate.hpp"

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <map>

namespace osrm
{
namespace analysis
{

IntersectionType::IntersectionType()
    : degree(0), in_count(0), out_count(0), roundabout_exit(false), roundabout_entry(false), barrier(false),
      traffic_light(false)
{
}

std::string IntersectionType::toString() const
{
    std::ostringstream oss;
    oss << "Neighbours: " << static_cast<std::uint32_t>(degree) << " " << static_cast<std::uint32_t>(in_count) << " : "
        << static_cast<uint32_t>(out_count) << " Roundabout: " << roundabout_exit << ":"
        << roundabout_entry << " Barrier: " << barrier << " Traffic Light: " << traffic_light;
    oss << " [i.e. " << example_lat / COORDINATE_PRECISION << ", "
        << example_lon / COORDINATE_PRECISION << "]";
    return oss.str();
}

void analyseGraph(const graph::NodeBasedDynamicGraph &node_based_graph,
                  const std::unordered_set<NodeID> &barrier_nodes,
                  const std::unordered_set<NodeID> &traffic_lights,
                  const graph::RestrictionMap &restriction_map,
                  const std::vector<graph::QueryNode> &internal_to_external_node_map)
{
    util::SimpleLogger().Write(logINFO) << "Analysing " << node_based_graph.GetNumberOfNodes()
                                        << " nodes.";
    std::map<IntersectionType, std::uint32_t> intersection_map;

    for (std::size_t nid = 0, nid_count = node_based_graph.GetNumberOfNodes(); nid < nid_count;
         ++nid)
    {
        IntersectionType intersection_type;
        intersection_type.degree = node_based_graph.GetOutDegree(nid);
        for (auto eid : node_based_graph.GetAdjacentEdgeRange(nid))
        {
            const NodeID target = node_based_graph.GetTarget(eid);
            const auto &data = node_based_graph.GetEdgeData(eid);
            if( !data.reversed ){
              intersection_type.out_count++;
              for( auto eid2 : node_based_graph.GetAdjacentEdgeRange(target) ){
                if( nid == node_based_graph.GetTarget(eid2) ){
                  const auto &in_data = node_based_graph.GetEdgeData(eid2);
                  if( !in_data.reversed ){
                    intersection_type.in_count++;
                    break;
                  }
                }
              }
            } else {
              intersection_type.in_count++;
              for( auto eid2 : node_based_graph.GetAdjacentEdgeRange(target) ){
                if( nid == node_based_graph.GetTarget(eid2) ){
                  const auto &in_data = node_based_graph.GetEdgeData(eid2);
                  if( in_data.reversed ){
                    intersection_type.out_count++;
                    break;
                  }
                }
              }
            }
        }
        intersection_type.barrier = (barrier_nodes.count(nid) > 0);
        intersection_type.traffic_light = (traffic_lights.count(nid) > 0);

        intersection_type.example_lat = internal_to_external_node_map[nid].lat;
        intersection_type.example_lon = internal_to_external_node_map[nid].lon;

        intersection_map[intersection_type]++;
    }

    util::SimpleLogger().Write(logINFO) << "Found " << intersection_map.size()
                                        << " intersection types.";
    for (const auto &intersection : intersection_map)
        util::SimpleLogger().Write(logINFO) << intersection.first.toString() << " ["
                                            << intersection.second << "]";
}

} // namespace graph
} // namespace osrm
