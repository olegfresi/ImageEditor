#include "../include/Core/Application.hpp"
#include <giomm.h>


int main(int argc, char *argv[])
{
    Gio::init();
    Editor::AppConfiguration config{"Image Editor", 600, 800};
    Editor::Application app{config};


    return app.Run(argc, argv);
}