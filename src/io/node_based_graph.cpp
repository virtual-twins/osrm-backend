#include "graph/turn_restriction.hpp"
#include "io/node_based_graph.hpp"
#include "extractor/node_based_edge.hpp"
#include "util/fingerprint.hpp"
#include "util/graph_loader.hpp"
#include "util/simple_logger.hpp"

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <fstream>
#include <string>
#include <vector>

namespace osrm
{
namespace io
{

/**
\brief Build load restrictions from .restriction file
*/
std::shared_ptr<graph::RestrictionMap> loadRestrictionMap(const std::string &filename)
{
    boost::filesystem::ifstream input_stream(filename,
                                             std::ios::in | std::ios::binary);

    std::vector<graph::TurnRestriction> restriction_list;
    util::loadRestrictionsFromFile(input_stream, restriction_list);

    util::SimpleLogger().Write() << " - " << restriction_list.size() << " restrictions.";

    return std::make_shared<graph::RestrictionMap>(restriction_list);
}

/**
\brief Load a node based graph from .osrm file
*/
std::shared_ptr<graph::NodeBasedDynamicGraph>
loadNodeBasedGraph(const std::string &filename,
                   std::unordered_set<NodeID> &barrier_nodes,
                   std::unordered_set<NodeID> &traffic_lights,
                   std::vector<graph::QueryNode> &internal_to_external_node_map)
{
    std::vector<extractor::NodeBasedEdge> edge_list;

    boost::filesystem::ifstream input_stream(filename,
                                             std::ios::in | std::ios::binary);

    std::vector<NodeID> barrier_list;
    std::vector<NodeID> traffic_light_list;
    NodeID number_of_node_based_nodes = util::loadNodesFromFile(
        input_stream, barrier_list, traffic_light_list, internal_to_external_node_map);

    util::SimpleLogger().Write() << " - " << barrier_list.size() << " bollard nodes, "
                                 << traffic_light_list.size() << " traffic lights";

    // insert into unordered sets for fast lookup
    barrier_nodes.insert(barrier_list.begin(), barrier_list.end());
    traffic_lights.insert(traffic_light_list.begin(), traffic_light_list.end());

    barrier_list.clear();
    barrier_list.shrink_to_fit();
    traffic_light_list.clear();
    traffic_light_list.shrink_to_fit();

    util::loadEdgesFromFile(input_stream, edge_list);

    if (edge_list.empty())
    {
        util::SimpleLogger().Write(logWARNING) << "The input data is empty, exiting.";
        return std::shared_ptr<graph::NodeBasedDynamicGraph>();
    }

    return graph::NodeBasedDynamicGraphFromEdges(number_of_node_based_nodes, edge_list);
}

} // namespace io
} // namespace osrm
