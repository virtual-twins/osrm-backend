#include "engine/api/json_factory.hpp"

#include "engine/hint.hpp"
#include "engine/polyline_compressor.hpp"
#include "util/integer_range.hpp"
#include "util/typedefs.hpp"

#include <boost/assert.hpp>
#include <boost/optional.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace osrm
{
namespace engine
{
namespace api
{
namespace json
{
namespace detail
{

const constexpr char *modifier_names[] = {"uturn",
                                          "sharp right",
                                          "right",
                                          "slight right",
                                          "straight",
                                          "slight left",
                                          "left",
                                          "sharp left"};

// translations of TurnTypes. Not all types are exposed to the outside world.
// invalid types should never be returned as part of the API
const constexpr char *turn_type_names[] = {
    "invalid",         "new name",   "continue", "turn",        "merge",
    "on ramp",         "off ramp",   "fork",     "end of road", "notification",
    "roundabout",      "roundabout", "rotary",   "rotary",      "roundabout turn",
    "roundabout turn", "use lane",   "invalid",  "invalid",     "invalid",
    "invalid",         "invalid",    "invalid",  "invalid",     "invalid",
    "invalid",         "invalid"};

const constexpr char *waypoint_type_names[] = {"invalid", "arrive", "depart"};

util::json::Array coordinateToLonLat(const util::Coordinate coordinate)
{
    util::json::Array array;
    array.values.push_back(static_cast<double>(toFloating(coordinate.lon)));
    array.values.push_back(static_cast<double>(toFloating(coordinate.lat)));
    return array;
}

// FIXME this actually needs to be configurable from the profiles
std::string modeToString(const extractor::TravelMode mode)
{
    std::string token;
    switch (mode)
    {
    case TRAVEL_MODE_INACCESSIBLE:
        token = "inaccessible";
        break;
    case TRAVEL_MODE_DRIVING:
        token = "driving";
        break;
    case TRAVEL_MODE_CYCLING:
        token = "cycling";
        break;
    case TRAVEL_MODE_WALKING:
        token = "walking";
        break;
    case TRAVEL_MODE_FERRY:
        token = "ferry";
        break;
    case TRAVEL_MODE_TRAIN:
        token = "train";
        break;
    case TRAVEL_MODE_PUSHING_BIKE:
        token = "pushing bike";
        break;
    case TRAVEL_MODE_STEPS_UP:
        token = "steps up";
        break;
    case TRAVEL_MODE_STEPS_DOWN:
        token = "steps down";
        break;
    case TRAVEL_MODE_RIVER_UP:
        token = "river upstream";
        break;
    case TRAVEL_MODE_RIVER_DOWN:
        token = "river downstream";
        break;
    case TRAVEL_MODE_ROUTE:
        token = "route";
        break;
    default:
        token = "other";
        break;
    }
    return token;
}

} // namespace detail

util::json::Object makeWaypoint(const util::Coordinate location, std::string name, const Hint &hint)
{
    util::json::Object waypoint;
    waypoint.values["location"] = detail::coordinateToLonLat(location);
    waypoint.values["name"] = std::move(name);
    waypoint.values["hint"] = hint.ToBase64();
    return waypoint;
}

} // namespace json
} // namespace api
} // namespace engine
} // namespace osrm
