#include "engine/plugins/isochrone.hpp"
#include "engine/api/isochrone_api.hpp"
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

IsochronePlugin::IsochronePlugin() {}

Status IsochronePlugin::HandleRequest(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                                      const api::IsochroneParameters &params,
                                      util::json::Object &json_result)
{
    BOOST_ASSERT(params.IsValid());

    if (!CheckAllCoordinates(params.coordinates))
        return Error("InvalidOptions", "Coordinates are invalid", json_result);

    if (params.coordinates.size() != 1)
    {
        return Error("InvalidOptions", "Only one input coordinate is supported", json_result);
    }

    if (params.distance != 0 && params.duration != 0)
    {
        return Error("InvalidOptions", "Only distance or duration should be set", json_result);
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

    util::SimpleLogger().Write() << "asdasd";
    auto phantom = phantomnodes.front();
    std::vector<NodeID> forward_id_vector;

    auto source =
        (*facade)
            .GetUncompressedReverseGeometry(phantom.front().phantom_node.packed_geometry_id)
            .front();

    //    (*facade).GetUncompressedGeometry(phantom.front().phantom_node.reverse_packed_geometry_id,
    //                                   forward_id_vector);
    //    auto source = phantom.;

    util::SimpleLogger().Write() << "asdasd";
    IsochroneVector isochroneVector;
    IsochroneVector convexhull;
    IsochroneVector concavehull;

    if (params.duration != 0)
    {
        TIMER_START(DIJKSTRA);
        dijkstraByDuration(facade, isochroneVector, source, params.duration);
        TIMER_STOP(DIJKSTRA);
        util::SimpleLogger().Write() << "DijkstraByDuration took: " << TIMER_MSEC(DIJKSTRA) << "ms";
        TIMER_START(SORTING);
        std::sort(isochroneVector.begin(),
                  isochroneVector.end(),
                  [&](const IsochroneNode n1, const IsochroneNode n2) {
                      return n1.duration < n2.duration;
                  });
        TIMER_STOP(SORTING);
        util::SimpleLogger().Write() << "SORTING took: " << TIMER_MSEC(SORTING) << "ms";
    }
    if (params.distance != 0)
    {
        TIMER_START(DIJKSTRA);
        dijkstraByDistance(facade, isochroneVector, source, params.distance);
        TIMER_STOP(DIJKSTRA);
        util::SimpleLogger().Write() << "DijkstraByDistance took: " << TIMER_MSEC(DIJKSTRA) << "ms";
        TIMER_START(SORTING);
        std::sort(isochroneVector.begin(),
                  isochroneVector.end(),
                  [&](const IsochroneNode n1, const IsochroneNode n2) {
                      return n1.distance < n2.distance;
                  });
        TIMER_STOP(SORTING);
        util::SimpleLogger().Write() << "SORTING took: " << TIMER_MSEC(SORTING) << "ms";
    }

    util::SimpleLogger().Write() << "Nodes Found: " << isochroneVector.size();
    // Optional param for calculating Convex Hull
    if (params.convexhull)
    {

        TIMER_START(CONVEXHULL);
        convexhull = util::monotoneChain(isochroneVector);
        TIMER_STOP(CONVEXHULL);
        util::SimpleLogger().Write() << "CONVEXHULL took: " << TIMER_MSEC(CONVEXHULL) << "ms";
    }
    if (params.concavehull && params.convexhull)
    {
        TIMER_START(CONCAVEHULL);
        concavehull = util::concavehull(convexhull, params.threshold, isochroneVector);
        TIMER_STOP(CONCAVEHULL);
        util::SimpleLogger().Write() << "CONCAVEHULL took: " << TIMER_MSEC(CONCAVEHULL) << "ms";
    }

    TIMER_START(RESPONSE);
    api::IsochroneAPI isochroneAPI{*facade, params};
    isochroneAPI.MakeResponse(isochroneVector, convexhull, concavehull, json_result);
    TIMER_STOP(RESPONSE);
    util::SimpleLogger().Write() << "RESPONSE took: " << TIMER_MSEC(RESPONSE) << "ms";
    isochroneVector.clear();
    isochroneVector.shrink_to_fit();
    convexhull.clear();
    convexhull.shrink_to_fit();
    concavehull.clear();
    concavehull.shrink_to_fit();
    return Status::Ok;
}

void IsochronePlugin::dijkstraByDuration(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                                         IsochroneVector &isochroneSet,
                                         NodeID &source,
                                         int duration)
{

    QueryHeap heap(facade->GetNumberOfNodes());
    heap.Insert(source, 0, source);

    isochroneSet.emplace_back(IsochroneNode(
        facade->GetCoordinateOfNode2(source), facade->GetCoordinateOfNode2(source), 0, 0));

    int steps = 0;
    int MAX_DURATION = duration * 60 * 10;
    {
        // Standard Dijkstra search, terminating when path length > MAX
        while (!heap.Empty())
        {
            steps++;
            const NodeID source = heap.DeleteMin();
            const std::int32_t weight = heap.GetKey(source);

            for (const auto current_edge : facade->GetAdjacentEdgeRange(source))
            {
                const auto target = facade->GetTarget(current_edge);
                if (target != SPECIAL_NODEID)
                {
                    const auto data = facade->GetEdgeData(current_edge);
                    if (data.real)
                    {
                        int to_duration = weight + data.weight;
                        if (to_duration > MAX_DURATION)
                        {
                            continue;
                        }
                        else if (!heap.WasInserted(target))
                        {
                            heap.Insert(target, to_duration, source);
                            isochroneSet.emplace_back(
                                IsochroneNode(facade->GetCoordinateOfNode2(target),
                                              facade->GetCoordinateOfNode2(source),
                                              0,
                                              to_duration));
                        }
                        else if (to_duration < heap.GetKey(target))
                        {
                            heap.GetData(target).parent = source;
                            heap.DecreaseKey(target, to_duration);
                            update(isochroneSet,
                                   IsochroneNode(facade->GetCoordinateOfNode2(target),
                                                 facade->GetCoordinateOfNode2(source),
                                                 0,
                                                 to_duration));
                        }
                    }
                }
            }
        }
    }
    util::SimpleLogger().Write() << "Steps took: " << steps;
}
void IsochronePlugin::dijkstraByDistance(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                                         IsochroneVector &isochroneSet,
                                         NodeID &source,
                                         double distance)
{
    QueryHeap heap(facade->GetNumberOfNodes());
    heap.Insert(source, 0, source);

    isochroneSet.emplace_back(IsochroneNode(
        facade->GetCoordinateOfNode2(source), facade->GetCoordinateOfNode2(source), 0, 0));
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
                            isochroneSet.emplace_back(
                                IsochroneNode(facade->GetCoordinateOfNode2(target),
                                              facade->GetCoordinateOfNode2(source),
                                              to_distance,
                                              0));
                        }
                        else if (to_distance < heap.GetKey(target))
                        {
                            heap.GetData(target).parent = source;
                            heap.DecreaseKey(target, to_distance);
                            update(isochroneSet,
                                   IsochroneNode(facade->GetCoordinateOfNode2(target),
                                                 facade->GetCoordinateOfNode2(source),
                                                 to_distance,
                                                 0));
                        }
                    }
                }
            }
        }
    }
    util::SimpleLogger().Write() << "Steps took: " << steps;
}

void IsochronePlugin::update(IsochroneVector &v, IsochroneNode n)
{
    for (auto node : v)
    {
        if (node.node.node_id == n.node.node_id)
        {
            node = n;
        }
    }
}
}
}
}
