#include "../../include/Core/Application.hpp"
#include <gtkmm/application.h>
#include <algorithm>


namespace Editor
{
    Application::Application(AppConfiguration config) : m_config{std::move(config)}
    {
        m_app = Gtk::Application::create("org.gtkmm.example");
        m_app->signal_activate().connect(sigc::mem_fun(*this, &Application::OnActivate));
    }

    Application::~Application() { ShutDown(); }

    int Application::Run(int argc, char** argv) const
    {
        return m_app->run(argc, argv);
    }

    void Application::OnActivate()
    {
        if(!m_window)
        {
            m_window = std::make_unique<Window>(m_config.winWidth, m_config.winHeight, m_config.name);
            m_app->add_window(*m_window);

            m_window->SetOpenDocumentCallback([this](const std::filesystem::path& path) {
                OnWindowOpenDocument(path, m_window.get());
            });
        }

        m_window->present();
    }

    void Application::ShutDown()
    {
        if(m_window)
        {
            m_window->hide();
            m_app->remove_window(*m_window);
            m_window.reset();
        }
        m_documents.clear();
    }

    Document* Application::NewDocument(const std::filesystem::path& path)
    {
        m_documents.push_back(FileManager::Load(path));
        return m_documents.back().get();
    }

    void Application::CloseDocument(Document* document)
    {
        auto it = std::ranges::find_if(m_documents.begin(), m_documents.end(),
                                       [document](const std::unique_ptr<Document>& ptr) {
                                           return ptr.get() == document;
                                       });

        if(it != m_documents.end())
            m_documents.erase(it);
    }

    void Application::OnWindowOpenDocument(const std::filesystem::path& path, Window* win)
    {
        Document* doc = NewDocument(path);
        if(!doc)
            return;

        win->LoadDocument(doc);
    }
}