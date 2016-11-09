#include "engine/engine.hpp"
#include "engine/engine_config.hpp"
#include "engine/status.hpp"

#include "engine/plugins/isochrone.hpp"

#include "engine/datafacade/datafacade_base.hpp"
#include "engine/datafacade/internal_datafacade.hpp"
//#include "engine/datafacade/shared_datafacade.hpp"

#include "storage/shared_barriers.hpp"
#include "util/simple_logger.hpp"

#include <boost/assert.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/thread/lock_types.hpp>

#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>

namespace
{
// Abstracted away the query locking into a template function
// Works the same for every plugin.
template <typename ParameterT, typename PluginT, typename ResultT>
osrm::engine::Status
RunQuery(const std::unique_ptr<osrm::storage::SharedBarriers> &lock,
         const std::shared_ptr<osrm::engine::datafacade::BaseDataFacade> &facade,
         const ParameterT &parameters,
         PluginT &plugin,
         ResultT &result)
{
    if (!lock)
    {
        return plugin.HandleRequest(facade, parameters, result);
    }

    BOOST_ASSERT(lock);
    // this locks aquires shared ownership of the query mutex: other requets are allowed
    // to run, but data updates need to wait for all queries to finish until they can aquire an
    // exclusive lock
    boost::interprocess::sharable_lock<boost::interprocess::named_sharable_mutex> query_lock(
        lock->query_mutex);

//    auto &shared_facade = static_cast<osrm::engine::datafacade::SharedDataFacade &>(*facade);
//    shared_facade.CheckAndReloadFacade();
    // Get a shared data lock so that other threads won't update
    // things while the query is running
//    boost::shared_lock<boost::shared_mutex> data_lock{shared_facade.data_mutex};

    osrm::engine::Status status = plugin.HandleRequest(facade, parameters, result);

    return status;
}

} // anon. ns

namespace osrm
{
namespace engine
{

Engine::Engine(const EngineConfig &config)
    : lock(config.use_shared_memory ? std::make_unique<storage::SharedBarriers>()
                                    : std::unique_ptr<storage::SharedBarriers>())
{
    if (config.use_shared_memory)
    {
//        query_data_facade = std::make_shared<datafacade::SharedDataFacade>();
    }
    else
    {
        if (!config.storage_config.IsValid())
        {
            throw util::exception("Invalid file paths given!");
        }
        query_data_facade = std::make_shared<datafacade::InternalDataFacade>(config.storage_config);
    }

    // Register plugins
    using namespace plugins;

    isochrone_plugin = std::make_unique<IsochronePlugin>();
    isodistance_plugin = std::make_unique<IsodistancePlugin>();

}

// make sure we deallocate the unique ptr at a position where we know the size of the plugins
Engine::~Engine() = default;
Engine::Engine(Engine &&) noexcept = default;
Engine &Engine::operator=(Engine &&) noexcept = default;

Status Engine::Isochrone(const api::IsochroneParameters &params, util::json::Object &result) const
{
    return RunQuery(lock, query_data_facade, params, *isochrone_plugin, result);
}

} // engine ns
} // osrm ns
