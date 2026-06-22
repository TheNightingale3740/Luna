#pragma once

#include "Radiance/Application.h"

namespace Radiance
{
    extern std::unique_ptr<Radiance::Application> CreateApplication();
}

int main()
{
    std::unique_ptr<Radiance::Application> app = Radiance::CreateApplication();
    app->Run();
}