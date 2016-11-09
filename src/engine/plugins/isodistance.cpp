//
// Created by robin on 11/9/16.
//
#include "engine/plugins/isodistance.hpp"
#include "engine/api/isodistance_api.hpp"
#include "engine/phantom_node.hpp"
#include "util/concave_hull.hpp"
#include "util/coordinate_calculation.hpp"
#include "util/graph_loader.hpp"
#include "util/monotone_chain.hpp"
#include "util/simple_logger.hpp"
#include "util/timing_util.hpp"

#include <algorithm>

namespace osrm
{
namespace engine
{
namespace plugins
{

IsodistancePlugin::IsodistancePlugin() {}

Status IsodistancePlugin::HandleRequest(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                                        const api::IsodistanceParameters &params,
                                        util::json::Object &json_result)
{
    BOOST_ASSERT(params.IsValid());

    if (!CheckAllCoordinates(params.coordinates))
        return Error("InvalidOptions", "Coordinates are invalid", json_result);

    if (params.coordinates.size() != 1)
    {
        return Error("InvalidOptions", "Only one input coordinate is supported", json_result);
    }

    if (params.distance <= 0)
    {
        return Error("InvalidOptions", "Distance should be set and greater than 0", json_result);
    }

    if (params.concavehull == true && params.convexhull == false)
    {
        return Error(
            "InvalidOptions", "If concavehull is set, convexhull must be set too", json_result);
    }

    auto phantomnodes = GetPhantomNodes(*facade, params, 1);

    if (phantomnodes.front().size() <= 0)
    {
        return Error("PhantomNode", "PhantomNode couldnt be found for coordinate", json_result);
    }

    auto phantom = phantomnodes.front();
    std::vector<NodeID> forward_id_vector;

    auto source =
        (*facade)
            .GetUncompressedReverseGeometry(phantom.front().phantom_node.packed_geometry_id)
            .front();

    IsolineNodeVector isodistanceVector;
    IsolineNodeVector convexhull;
    IsolineNodeVector concavehull;

    dijkstra(facade, isodistanceVector, source, params.distance);

    std::sort(
        isodistanceVector.begin(),
        isodistanceVector.end(),
        [&](const IsolineNode n1, const IsolineNode n2) { return n1.weight < n2.weight; });

    // Optional param for calculating Convex Hull
    if (params.convexhull)
    {

        convexhull = util::monotoneChain(isodistanceVector);
    }
    if (params.concavehull && params.convexhull)
    {
        concavehull = util::concavehull(convexhull, params.threshold, isodistanceVector);
    }

    api::IsodistanceAPI isodistanceAPI{*facade, params};
    isodistanceAPI.MakeResponse(isodistanceVector, convexhull, concavehull, json_result);

    isodistanceVector.clear();
    isodistanceVector.shrink_to_fit();
    convexhull.clear();
    convexhull.shrink_to_fit();
    concavehull.clear();
    concavehull.shrink_to_fit();
    return Status::Ok;
}

void IsodistancePlugin::dijkstra(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                                 IsolineNodeVector &isodistanceVector,
                                 NodeID &source,
                                 double distance)
{
    QueryHeap heap(facade->GetNumberOfNodes());
    heap.Insert(source, 0, source);

    isodistanceVector.emplace_back(IsolineNode(
        facade->GetCoordinateOfNode2(source), facade->GetCoordinateOfNode2(source), 0));
    int steps = 0;
    int MAX_DISTANCE = distance;
    {
        // Standard Dijkstra search, terminating when path length > MAX
        while (!heap.Empty())
        {
            steps++;
            NodeID source = heap.DeleteMin();
            std::int32_t weight = heap.GetKey(source);

            for (const auto current_edge : facade->GetAdjacentEdgeRange(source))
            {
                const auto target = facade->GetTarget(current_edge);
                if (target != SPECIAL_NODEID)
                {
                    const auto data = facade->GetEdgeData(current_edge);
                    if (data.real)
                    {
                        Coordinate s(facade->GetCoordinateOfNode2(source).lon,
                                     facade->GetCoordinateOfNode2(source).lat);
                        Coordinate t(facade->GetCoordinateOfNode2(target).lon,
                                     facade->GetCoordinateOfNode2(target).lat);
                        // FIXME this might not be accurate enough
                        int to_distance =
                            static_cast<int>(
                                util::coordinate_calculation::haversineDistance(s, t)) +
                            weight;

                        if (to_distance > MAX_DISTANCE)
                        {
                            continue;
                        }
                        else if (!heap.WasInserted(target))
                        {
                            heap.Insert(target, to_distance, source);
                            isodistanceVector.emplace_back(
                                IsolineNode(facade->GetCoordinateOfNode2(target),
                                            facade->GetCoordinateOfNode2(source),
                                            to_distance));
                        }
                        else if (to_distance < heap.GetKey(target))
                        {
                            heap.GetData(target).parent = source;
                            heap.DecreaseKey(target, to_distance);
                            update(isodistanceVector,
                                   IsolineNode(facade->GetCoordinateOfNode2(target),
                                               facade->GetCoordinateOfNode2(source),
                                               to_distance));
                        }
                    }
                }
            }
        }
    }
}
}
}
}
