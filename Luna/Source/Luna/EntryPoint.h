#pragma once

#include "Luna/Application.h"

namespace Luna
{
    extern std::unique_ptr<Luna::Application> CreateApplication();
}

int main()
{
    std::unique_ptr<Luna::Application> app = Luna::CreateApplication();
    app->Run();
}