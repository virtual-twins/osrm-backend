#include "analysis/analyse_turns.hpp"
#include "util/simple_logger.hpp"
#include "util/coordinate.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <utility>
#include <map>
#include <set>

namespace osrm
{
namespace analysis
{

namespace detail
{
LengthClass getClass(EdgeWeight weight)
{
    if (weight < 5)
        return LengthClass::SHORT;
    return LengthClass::NORMAL;
}
}

IntersectionType::IntersectionType()
    : is_roundabout(false), barrier(false), traffic_light(false), example_lat(0), example_lon(0),
      possible_turns(0)
{
    for (std::size_t i = 0; i < static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES); ++i)
    {
        in_count[i] = out_count[i] = 0;
    }
}

std::string IntersectionType::toString() const
{
    std::ostringstream oss;
    oss << "Neighbours: " << static_cast<std::uint32_t>(neighbours) << " In:";
    for (std::size_t i = 0; i < static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES); ++i)
        oss << " " << static_cast<std::uint32_t>(in_count[i]);
    oss << " Out:";
    for (std::size_t i = 0; i < static_cast<std::size_t>(LengthClass::NUM_LENGTH_CLASSES); ++i)
        oss << " " << static_cast<std::uint32_t>(out_count[i]);
    oss << " Turns: " << possible_turns << " Rnd: " << is_roundabout << " B: " << barrier
        << " TL: " << traffic_light << " [i.e. "
        << std::setprecision(ceil(log(COORDINATE_PRECISION))) << example_lat / COORDINATE_PRECISION
        << ", " << example_lon / COORDINATE_PRECISION << "]";
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

    const auto isRoundabout = [&node_based_graph](NodeID nid)
    {
        for (auto eid : node_based_graph.GetAdjacentEdgeRange(nid))
        {
            const auto &data = node_based_graph.GetEdgeData(eid);
            if (data.roundabout)
                return true;
        }
        return false;
    };

    for (std::size_t nid = 0, nid_count = node_based_graph.GetNumberOfNodes(); nid < nid_count;
         ++nid)
    {
        if (0 == node_based_graph.GetOutDegree(nid)) // filter compressed nodes
            continue;
        IntersectionType intersection_type;
        intersection_type.is_roundabout = isRoundabout(nid);
        std::set<NodeID> neighbours;
        for (auto eid : node_based_graph.GetAdjacentEdgeRange(nid))
        {
            const auto &data = node_based_graph.GetEdgeData(eid);
            NodeID target = node_based_graph.GetTarget(eid);
            neighbours.insert( target );
            bool is_inbound = false;
            if (!data.reversed)
            {
                intersection_type.out_count[detail::getClass(data.distance)]++;
                for (auto eid2 : node_based_graph.GetAdjacentEdgeRange(target))
                {
                    if (node_based_graph.GetTarget(eid2) == nid)
                    {
                        const auto &in_data = node_based_graph.GetEdgeData(eid2);
                        if (!in_data.reversed)
                        {
                            intersection_type.in_count[detail::getClass(in_data.distance)]++;
                            is_inbound = true;
                        }
                    }
                }
            }
            else
            {
                intersection_type.in_count[detail::getClass(data.distance)]++;
                is_inbound = true;
            }

            if (is_inbound)
            {
                const auto allow_turns_only_to =
                    restriction_map.CheckForEmanatingIsOnlyTurn(target, nid);

                for (auto eid2 : node_based_graph.GetAdjacentEdgeRange(nid))
                {

                    const auto &out_data = node_based_graph.GetEdgeData(eid2);
                    if (!out_data.reversed)
                    {
                        NodeID out_node = node_based_graph.GetTarget(eid2);
                        if (SPECIAL_NODEID != allow_turns_only_to &&
                            out_node != allow_turns_only_to)
                            continue;
                        if (SPECIAL_NODEID == allow_turns_only_to &&
                            restriction_map.CheckIfTurnIsRestricted(target, nid, out_node))
                            continue;

                        if(barrier_nodes.count(nid))
                        {
                            if (target != out_node)
                            {
                                continue;
                            }
                        }
                        else
                        {
                            if (target == out_node && node_based_graph.GetOutDegree(nid) > 1)
                            {
                                auto number_of_emmiting_bidirectional_edges = 0;
                                for (auto edge : node_based_graph.GetAdjacentEdgeRange(nid))
                                {
                                    auto target = node_based_graph.GetTarget(edge);
                                    auto reverse_edge =
                                        node_based_graph.FindEdge(target, nid);
                                    if (!node_based_graph.GetEdgeData(reverse_edge).reversed)
                                    {
                                        ++number_of_emmiting_bidirectional_edges;
                                    }
                                }
                                if (number_of_emmiting_bidirectional_edges > 1)
                                {
                                    continue;
                                }
                            }
                        }
                        intersection_type.possible_turns++;
                    }
                }
            }
        }
        intersection_type.neighbours = neighbours.size();
        intersection_type.barrier = (barrier_nodes.count(nid) > 0);
        intersection_type.traffic_light = (traffic_lights.count(nid) > 0);

        intersection_type.example_lat = internal_to_external_node_map[nid].lat;
        intersection_type.example_lon = internal_to_external_node_map[nid].lon;

        intersection_map[intersection_type]++;
    }

    util::SimpleLogger().Write(logINFO) << "Found " << intersection_map.size()
                                        << " intersection types.";

    using std::begin;
    using std::end;
    typedef std::pair<std::uint32_t, IntersectionType> pUI;
    std::vector<pUI> intersections(intersection_map.size());
    std::transform(begin(intersection_map), end(intersection_map), begin(intersections),
                   [](const std::pair<IntersectionType, std::uint32_t> &pair)
                   {
                       return std::make_pair(pair.second, pair.first);
                   });
    std::sort(begin(intersections), end(intersections), [](const pUI &left, const pUI &right)
              {
                  return left.first > right.first;
              });

    for (const auto &intersection : intersections)
        util::SimpleLogger().Write(logINFO) << intersection.first << " : "
                                            << intersection.second.toString();
}

} // namespace graph
} // namespace osrm
