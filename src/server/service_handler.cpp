#include "server/service_handler.hpp"
#include <engine/engine_config.hpp>
#include "server/service/isochrone_service.hpp"
#include "server/service/isodistance_service.hpp"

#include "server/api/parsed_url.hpp"
#include "util/json_util.hpp"
#include "util/make_unique.hpp"

namespace osrm
{
namespace server
{
ServiceHandler::ServiceHandler(osrm::EngineConfig &config) : routing_machine(config)
{
    service_map["isochrone"] = util::make_unique<service::IsochroneService>(routing_machine);
    service_map["isodistance"] = util::make_unique<service::IsodistanceService>(routing_machine);

}

engine::Status ServiceHandler::RunQuery(api::ParsedURL parsed_url,
                                        service::BaseService::ResultT &result)
{
    const auto &service_iter = service_map.find(parsed_url.service);
    if (service_iter == service_map.end())
    {
        result = util::json::Object();
        auto &json_result = result.get<util::json::Object>();
        json_result.values["code"] = "InvalidService";
        json_result.values["message"] = "Service " + parsed_url.service + " not found!";
        return engine::Status::Error;
    }
    auto &service = service_iter->second;

    if (service->GetVersion() != parsed_url.version)
    {
        result = util::json::Object();
        auto &json_result = result.get<util::json::Object>();
        json_result.values["code"] = "InvalidVersion";
        json_result.values["message"] = "Service " + parsed_url.service + " not found!";
        return engine::Status::Error;
    }

    return service->RunQuery(parsed_url.prefix_length, parsed_url.query, result);
}
}
}
