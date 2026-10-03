#include "ResourceManager.h"
#include "Adachi/Diagnostics/Debug.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Adachi {

std::string ResourceManager::ReadFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error(
            "Failed to open resource: " + path.string()
        );
    ADACHI_CORE_ASSERT(file.is_open(), "Failed to open resource: {}", path.string());

    std::ostringstream contents;
    contents << file.rdbuf();

    ADACHI_CORE_ASSERT(!file.bad(), "Failed to open resource: {}", path.string());

    return contents.str();
}

} // namespace Adachi