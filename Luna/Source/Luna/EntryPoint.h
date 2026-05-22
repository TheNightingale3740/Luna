#pragma once

#include "Luna/Application.h"

namespace Luna
{
    extern Luna::Application* CreateApplication();
}

int main()
{
    Luna::Application* app = Luna::CreateApplication();
    app->Run();
    delete app;
}