#ifndef ENGINE_GUIDANCE_ASSEMBLE_OVERVIEW_HPP
#define ENGINE_GUIDANCE_ASSEMBLE_OVERVIEW_HPP

#include "engine/guidance/leg_geometry.hpp"

#include "util/coordinate.hpp"

#include <vector>

namespace osrm::engine::guidance
{

std::vector<util::Coordinate> assembleOverview(const std::vector<LegGeometry> &leg_geometries,
                                               const bool use_simplification);

std::vector<NodeID> assembleVtOverview(const std::vector<LegGeometry> &leg_geometries);

} // namespace osrm::engine::guidance

#endif
