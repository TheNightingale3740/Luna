#include <Luna.h>

Luna::Application* Luna::CreateApplication()
{
    Luna::ApplicationSpecification spec;
    spec.Name = "Luna Editor";
    spec.WindowSpec.Title = "Luna Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    Luna::Application *app = new Luna::Application(spec);
    return app;
}