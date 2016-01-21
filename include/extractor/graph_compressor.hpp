#ifndef GEOMETRY_COMPRESSOR_HPP
#define GEOMETRY_COMPRESSOR_HPP

#include "util/typedefs.hpp"

#include "extractor/speed_profile.hpp"
#include "graph/node_based_graph.hpp"
#include "graph/restriction_map.hpp"

#include <memory>
#include <unordered_set>

namespace osrm
{
namespace extractor
{

class CompressedEdgeContainer;

class GraphCompressor
{
    using EdgeData = graph::NodeBasedDynamicGraph::EdgeData;

  public:
    GraphCompressor(SpeedProfileProperties speed_profile);

    void Compress(const std::unordered_set<NodeID> &barrier_nodes,
                  const std::unordered_set<NodeID> &traffic_lights,
                  graph::RestrictionMap &restriction_map,
                  graph::NodeBasedDynamicGraph &graph,
                  CompressedEdgeContainer &geometry_compressor);

  private:
    void PrintStatistics(unsigned original_number_of_nodes,
                         unsigned original_number_of_edges,
                         const graph::NodeBasedDynamicGraph &graph) const;

    SpeedProfileProperties speed_profile;
};
}
}

#endif
