#include "../../include/Core/Application.hpp"
#include <gtkmm/application.h>

namespace Editor
{

    Application::Application(AppConfiguration config) : m_config{std::move(config)}
    {
        m_app = Gtk::Application::create("org.gtkmm.example");
    }

    void Application::Init()
    {

    }

    int Application::Run(int argc, char** argv)
    {
        return m_app->make_window_and_run<Window>(argc, argv, m_config.winWidth, m_config.winHeight, m_config.name);
    }

    void Application::ShutDown()
    {

    }

    std::unique_ptr<Document> Application::NewDocument()
    {
        return std::make_unique<Document>();
    }

    std::unique_ptr<Document> Application::NewDocument(std::filesystem::path path)
    {
        return std::make_unique<Document>(path);
    }

    std::unique_ptr<Document> Application::NewDocumentFromFile(const File& file)
    {
        return std::make_unique<Document>(file);
    }

    void Application::OpenDocument(std::unique_ptr<Document> document)
    {

    }

    void Application::CloseDocument(std::unique_ptr<Document> document)
    {

    }
}