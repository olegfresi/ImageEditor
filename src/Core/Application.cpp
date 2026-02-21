#include "../../include/Core/Application.hpp"
#include <gtkmm/application.h>

namespace Editor
{

    Application::Application(AppConfiguration config) : m_config{std::move(config)}
    {
        m_app = Gtk::Application::create("org.gtkmm.example");
    }

    int Application::Run(int argc, char** argv)
    {
        return m_app->make_window_and_run<Window>(argc, argv, m_config.winWidth, m_config.winHeight, m_config.name);
    }

}