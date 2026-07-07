#include "FileSystem.h"

namespace Radiance
{

    std::filesystem::path GetExecutableDir()
    {
        char path[1024];
        uint32_t size = sizeof(path);

        if (_NSGetExecutablePath(path, &size) == 0) {
            return std::filesystem::path(path).parent_path();
        }
        return std::filesystem::current_path(); // fallback
    }

}