#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "engine/status.hpp"
#include "util/json_container.hpp"

#include <memory>
#include <string>
#include <unordered_map>

namespace osrm
{

namespace util
{
namespace json
{
struct Object;
}
}

namespace storage
{
struct SharedBarriers;
}

// Fwd decls
namespace engine
{
struct EngineConfig;
namespace api
{
struct IsochroneParameters;
struct IsodistanceParameters;
}
namespace plugins
{
class IsochronePlugin;
class IsodistancePlugin;
}
// End fwd decls

namespace datafacade
{
class BaseDataFacade;
}

class Engine final
{
  public:
    explicit Engine(const EngineConfig &config);

    Engine(Engine &&) noexcept;
    Engine &operator=(Engine &&) noexcept;

    // Impl. in cpp since for unique_ptr of incomplete types
    ~Engine();

    Status Isochrone(const api::IsochroneParameters &parameters, util::json::Object &result) const;

  private:
    std::unique_ptr<storage::SharedBarriers> lock;

    std::unique_ptr<plugins::IsochronePlugin> isochrone_plugin;
    std::unique_ptr<plugins::IsodistancePlugin> isochrone_plugin;

    std::shared_ptr<datafacade::BaseDataFacade> query_data_facade;
};
}
}

#endif // OSRM_IMPL_HPP
