#include "../include/Core/Application.hpp"


int main(int argc, char *argv[])
{

    Editor::AppConfiguration config{"Image Editor", 600, 800};
    Editor::Application app{config};


    return app.Run(argc, argv);
}