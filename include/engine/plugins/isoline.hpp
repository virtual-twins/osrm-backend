//
// Created by robin on 11/9/16.
//

#ifndef OSRM_ISOLINE_HPP
#define OSRM_ISOLINE_HPP

#include "engine/plugins/plugin_base.hpp"

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

struct IsochroneNode
{
    IsochroneNode(){};
    IsochroneNode(osrm::extractor::QueryNode node,
                  osrm::extractor::QueryNode predecessor,
                  double distance,
                  int duration)
        : node(node), predecessor(predecessor), distance(distance), duration(duration)
    {
    }

    osrm::extractor::QueryNode node;
    osrm::extractor::QueryNode predecessor;
    double distance;
    int duration;

    bool operator==(const IsochroneNode &n) const
    {
        if (n.node.node_id == node.node_id)
            return true;
        else
            return false;
    }
};
class IsolinePlugin : public BasePlugin
{
};
}
}
}
#endif // OSRM_ISOLINE_HPP
