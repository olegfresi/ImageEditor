#include "../include/Core/Application.hpp"

int main()
{

    AppConfiguration config{"Image Editor", 600, 800};

    Application app{config};
    app.Run();

    return 0;
}