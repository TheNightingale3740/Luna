#include <Luna.h>

#include <iostream> // TODO: Need a logging library soon!

class LunaEditor : public Luna::Application
{
public:
    LunaEditor()
    {
        std::cout << "Created Application!" << std::endl;
    }

    ~LunaEditor()
    {
        std::cout << "Destroyed Application!" << std::endl;
    }
};

Luna::Application* Luna::CreateApplication()
{
    return new LunaEditor();
}