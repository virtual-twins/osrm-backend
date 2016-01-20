#ifndef IO_NODE_BASED_GRAPH_HPP_
#define IO_NODE_BASED_GRAPH_HPP_

#include <iostream>
#include <unordered_set>
#include <memory>
#include <string>
#include <vector>

#include "graph/query_node.hpp"
#include "graph/restriction_map.hpp"

#include "util/typedefs.hpp"
#include "util/node_based_graph.hpp"

namespace osrm
{
namespace io
{

// file reading for the restriction map;
std::shared_ptr<graph::RestrictionMap> loadRestrictionMap( const std::string &filename );

// load a node based graph from file
std::shared_ptr<util::NodeBasedDynamicGraph>
loadNodeBasedGraph(const std::string &filename,
                   std::unordered_set<NodeID> &barrier_nodes,
                   std::unordered_set<NodeID> &traffic_lights,
                   std::vector<graph::QueryNode> &internal_to_external_node_map);

} // namespace io
} // namespace osrm

#endif // IO_NODE_BASED_GRAPH_HPP_
