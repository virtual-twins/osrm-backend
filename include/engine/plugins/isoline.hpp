//
// Created by robin on 11/9/16.
//

#ifndef OSRM_ISOLINE_HPP
#define OSRM_ISOLINE_HPP

#include "engine/plugins/plugin_base.hpp"
#include "util/binary_heap.hpp"

namespace osrm
{
namespace engine
{
namespace plugins
{
struct HeapData
{
    NodeID parent;
    HeapData(NodeID p) : parent(p) {}
};

struct IsolineNode
{
    IsolineNode(){};
    IsolineNode(osrm::extractor::QueryNode node,
                osrm::extractor::QueryNode predecessor,
                double weight)
        : node(node), predecessor(predecessor), weight(weight)
    {
    }

    osrm::extractor::QueryNode node;
    osrm::extractor::QueryNode predecessor;
    double weight;

    bool operator==(const IsolineNode &n) const
    {
        if (n.node.node_id == node.node_id)
            return true;
        else
            return false;
    }
};

using QueryHeap = osrm::util::
    BinaryHeap<NodeID, NodeID, int, HeapData, osrm::util::UnorderedMapStorage<NodeID, int>>;

typedef std::vector<IsolineNode> IsolineNodeVector;

class IsolinePlugin : public BasePlugin
{

  private:
    void dijkstra(const std::shared_ptr<datafacade::BaseDataFacade> facade,
                  IsolineNodeVector &isolineNodeVector,
                  NodeID &source,
                  double radius);

  public:
    void update(IsolineNodeVector &isolineNodeVector, IsolineNode n)
    {
        for (auto node : isolineNodeVector)
        {
            if (node.node.node_id == n.node.node_id)
            {
                node = n;
            }
        }
    }
};
}
}
}
#endif // OSRM_ISOLINE_HPP
