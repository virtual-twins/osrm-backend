#include "osrm/osrm.hpp"
#include "engine/api/isochrone_parameters.hpp"
#include "engine/engine.hpp"
#include "engine/engine_config.hpp"
#include "engine/status.hpp"
#include "util/make_unique.hpp"

namespace osrm
{

// Pimpl idiom

OSRM::OSRM(engine::EngineConfig &config) : engine_(util::make_unique<engine::Engine>(config)) {}
OSRM::~OSRM() = default;
OSRM::OSRM(OSRM &&) noexcept = default;
OSRM &OSRM::operator=(OSRM &&) noexcept = default;

// Forward to implementation

engine::Status OSRM::Isochrone(const engine::api::IsochroneParameters &params,
                               json::Object &result) const
{
    return engine_->Isochrone(params, result);
}

} // ns osrm
