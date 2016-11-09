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

    if (params.duration <= 0)
    {
        return Error("InvalidOptions", "Duration should be set and greater than 0", json_result);
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

    auto source =
        (*facade)
            .GetUncompressedReverseGeometry(phantom.front().phantom_node.packed_geometry_id)
            .front();

    IsolineNodeVector isochroneVector;
    IsolineNodeVector convexhull;
    IsolineNodeVector concavehull;

    dijkstra(facade, isochroneVector, source, params.duration);

    std::sort(
        isochroneVector.begin(),
        isochroneVector.end(),
        [&](const IsolineNode n1, const IsolineNode n2) { return n1.weight < n2.weight; });

    // Optional param for calculating Convex Hull
    if (params.convexhull)
    {
        convexhull = util::monotoneChain(isochroneVector);
    }
    if (params.concavehull && params.convexhull)
    {
        concavehull = util::concavehull(convexhull, params.threshold, isochroneVector);
    }

    api::IsochroneAPI isochroneAPI{*facade, params};
    isochroneAPI.MakeResponse(isochroneVector, convexhull, concavehull, json_result);

    isochroneVector.clear();
    isochroneVector.shrink_to_fit();
    convexhull.clear();
    convexhull.shrink_to_fit();
    concavehull.clear();
    concavehull.shrink_to_fit();
    return Status::Ok;
}

void IsochronePlugin::dijkstra(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                               IsolineNodeVector &isochroneVector,
                               NodeID &source,
                               double duration)
{

    QueryHeap heap(facade->GetNumberOfNodes());
    heap.Insert(source, 0, source);

    isochroneVector.emplace_back(IsolineNode(
        facade->GetCoordinateOfNode2(source), facade->GetCoordinateOfNode2(source), 0));

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
                            isochroneVector.emplace_back(
                                IsolineNode(facade->GetCoordinateOfNode2(target),
                                            facade->GetCoordinateOfNode2(source),
                                            to_duration));
                        }
                        else if (to_duration < heap.GetKey(target))
                        {
                            heap.GetData(target).parent = source;
                            heap.DecreaseKey(target, to_duration);
                            update(isochroneVector,
                                   IsolineNode(facade->GetCoordinateOfNode2(target),
                                               facade->GetCoordinateOfNode2(source),
                                               to_duration));
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
