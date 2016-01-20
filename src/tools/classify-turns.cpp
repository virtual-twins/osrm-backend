#include "analysis/analyse_turns_options.hpp"
#include "util/simple_logger.hpp"
#include "util/typedefs.hpp"
#include "io/node_based_graph.hpp"
#include "graph/query_node.hpp"

#include <boost/filesystem.hpp>

#include <cstdlib>
#include <exception>
#include <new>

#include <vector>
#include <unordered_set>

using namespace osrm;

int main(int argc, char *argv[]) try
{
    util::LogPolicy::GetInstance().Unmute();
    analysis::AnalyseTurnsOptions analyse_turn_options;

    if (!analyse_turn_options.ParseArguments(argc, argv))
        return EXIT_FAILURE;

    auto restriction_map = io::loadRestrictionMap(analyse_turn_options.restriction_file);

    std::unordered_set<NodeID> barrier_nodes, traffic_lights;
    std::vector<graph::QueryNode> internal_to_external_node_map;
    auto node_based_graph = io::loadNodeBasedGraph(analyse_turn_options.graph_file, barrier_nodes,
                                                   traffic_lights, internal_to_external_node_map);

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
