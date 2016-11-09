#ifndef ENGINE_RESPONSE_OBJECTS_HPP_
#define ENGINE_RESPONSE_OBJECTS_HPP_

#include "extractor/travel_mode.hpp"
#include "engine/polyline_compressor.hpp"
#include "util/coordinate.hpp"
#include "util/json_container.hpp"

#include <boost/optional.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace osrm
{
namespace engine
{

struct Hint;

namespace api
{
namespace json
{
namespace detail
{

util::json::Array coordinateToLonLat(const util::Coordinate coordinate);

std::string modeToString(const extractor::TravelMode mode);

} // namespace detail

template <typename ForwardIter> util::json::String makePolyline(ForwardIter begin, ForwardIter end)
{
    return {encodePolyline(begin, end)};
}

template <typename ForwardIter>
util::json::Object makeGeoJSONGeometry(ForwardIter begin, ForwardIter end)
{
    auto num_coordinates = std::distance(begin, end);
    BOOST_ASSERT(num_coordinates != 0);
    util::json::Object geojson;
    if (num_coordinates > 1)
    {
        geojson.values["type"] = "LineString";
        util::json::Array coordinates;
        coordinates.values.reserve(num_coordinates);
        std::transform(
            begin, end, std::back_inserter(coordinates.values), &detail::coordinateToLonLat);
        geojson.values["coordinates"] = std::move(coordinates);
    }
    else if (num_coordinates > 0)
    {
        geojson.values["type"] = "Point";
        util::json::Array coordinates;
        coordinates.values.push_back(detail::coordinateToLonLat(*begin));
        geojson.values["coordinates"] = std::move(coordinates);
    }
    return geojson;
}

util::json::Object
makeWaypoint(const util::Coordinate location, std::string name, const Hint &hint);

}
}
} // namespace engine
} // namespace osrm

#endif // ENGINE_GUIDANCE_API_RESPONSE_GENERATOR_HPP_
