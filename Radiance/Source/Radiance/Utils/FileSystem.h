#pragma once

#include <mach-o/dyld.h>
#include <filesystem>

namespace Radiance
{

    std::filesystem::path GetExecutableDir();

}