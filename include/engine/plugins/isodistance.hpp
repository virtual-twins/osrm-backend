#ifndef ISODISTANCE_HPP
#define ISODISTANCE_HPP

#include "engine/api/isodistance_parameters.hpp"
#include "engine/plugins/isoline.hpp"
#include "osrm/json_container.hpp"
#include "util/binary_heap.hpp"

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/function.hpp>
#include <tbb/parallel_sort.h>

#include <algorithm>
#include <cstdlib>
#include <utility>
#include <vector>

namespace osrm
{
namespace engine
{
namespace plugins
{


using QueryHeap = osrm::util::
    BinaryHeap<NodeID, NodeID, int, HeapData, osrm::util::UnorderedMapStorage<NodeID, int>>;
typedef std::vector<IsochroneNode> IsochroneVector;

class IsodistancePlugin final : public IsolinePlugin
{
  private:
    void dijkstraByDistance(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                            IsochroneVector &isochroneSet,
                            NodeID &source,
                            double distance);
    void update(IsochroneVector &s, IsochroneNode node);

  public:
    explicit IsodistancePlugin();

    Status HandleRequest(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                         const api::IsodistanceParameters &params,
                         util::json::Object &json_result);
};
}
}
}

#endif // ISODISTANCE_HPP
