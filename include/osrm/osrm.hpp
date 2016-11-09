/*

Copyright (c) 2016, Project OSRM contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef OSRM_HPP
#define OSRM_HPP

#include "osrm/osrm_fwd.hpp"
#include "osrm/status.hpp"

#include <memory>
#include <string>

namespace osrm
{
namespace json = util::json;
using engine::EngineConfig;
using engine::api::IsochroneParameters;
using engine::api::IsodistanceParameters;

/**
 * Represents a Open Source Routing Machine with access to its services.
 *
 * This represents an Open Source Routing Machine (OSRM) instance, with the services:
 *
 *  - Route: shortest path queries for coordinates
 *  - Table: distance tables for coordinates
 *  - Nearest: nearest street segment for coordinate
 *  - Trip: shortest round trip between coordinates
 *  - Match: snaps noisy coordinate traces to the road network
 *  - Tile: vector tiles with internal graph representation
 *
 *  All services take service-specific parameters, fill a JSON object, and return a status code.
 */
class OSRM final
{
  public:
    /**
     * Constructs an OSRM instance with user-configurable settings.
     *
     * \param config The user-provided OSRM configuration.
     * \see EngineConfig
     */
    explicit OSRM(EngineConfig &config);

    ~OSRM();

    // Moveable but not copyable
    OSRM(OSRM &&) noexcept;
    OSRM &operator=(OSRM &&) noexcept;

    /**
     * Isochrone for given duration
     * \param parameters nearest query specific parameters
     * \return Status indicating success for the query or failure
     * \see Status, NearestParameters and json::Object
     */
    Status Isochrone(const IsochroneParameters &parameters, json::Object &result) const;
    /**
     * Isodistance for given distance
     * \param parameters nearest query specific parameters
     * \return Status indicating success for the query or failure
     * \see Status, NearestParameters and json::Object
     */
    Status Isodistance(const IsodistanceParameters &parameters, json::Object &result) const;

  private:
    std::unique_ptr<engine::Engine> engine_;
};
}

#endif // OSRM_HPP
