#include "analysis/analyse_turns_options.hpp"
#include "analysis/analyse_turns.hpp"
#include "util/simple_logger.hpp"
#include "util/typedefs.hpp"
#include "io/node_based_graph.hpp"
#include "graph/query_node.hpp"
#include "extractor/graph_compressor.hpp"
#include "extractor/compressed_edge_container.hpp"

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

    util::SimpleLogger().Write(logINFO) << "Loading restriction map (" << analyse_turn_options.restriction_file << ")";
    auto restriction_map = io::loadRestrictionMap(analyse_turn_options.restriction_file);
    util::SimpleLogger().Write(logINFO) << "Finished loading restriction map.";

    std::unordered_set<NodeID> barrier_nodes, traffic_lights;
    std::vector<graph::QueryNode> internal_to_external_node_map;
    util::SimpleLogger().Write(logINFO) << "Loading node based graph (" << analyse_turn_options.graph_file << ")";
    auto node_based_graph = io::loadNodeBasedGraph(analyse_turn_options.graph_file, barrier_nodes,
                                                   traffic_lights, internal_to_external_node_map);
    util::SimpleLogger().Write(logINFO) << "Finished loading graph";

    extractor::GraphCompressor compressor;
    extractor::CompressedEdgeContainer compressed_edges;
    util::SimpleLogger().Write(logINFO) << "Compressing graph geometry";
    compressor.Compress( barrier_nodes, 0, traffic_lights, *restriction_map, *node_based_graph, compressed_edges );
    util::SimpleLogger().Write(logINFO) << "Done with graph compression.";

    analysis::analyseGraph(*node_based_graph,barrier_nodes,traffic_lights,*restriction_map,internal_to_external_node_map);

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
