#include "storage/storage_config.hpp"
#include "util/simple_logger.hpp"

#include <boost/filesystem/operations.hpp>

namespace osrm
{
namespace storage
{

StorageConfig::StorageConfig(const boost::filesystem::path &base)
    : base{base.string()}, ram_index_path{base.string() + ".ramIndex"},
      file_index_path{base.string() + ".fileIndex"},
      nodes_data_path{base.string() + ".nodes"}, edges_data_path{base.string() + ".edges"},
      geometries_path{base.string() + ".geometry"}
{
}

bool StorageConfig::IsValid() const
{
    const constexpr auto num_files = 6;
    const boost::filesystem::path paths[num_files] = {base,
                                                      ram_index_path,
                                                      file_index_path,
                                                      nodes_data_path,
                                                      edges_data_path,
                                                      geometries_path};

    bool success = true;
    for (auto path = paths; path != paths + num_files; ++path)
    {
        if (!boost::filesystem::is_regular_file(*path))
        {
            util::SimpleLogger().Write(logWARNING) << "Missing/Broken File: " << path->string();
            success = false;
        }
    }

    return success;
}
}
}
