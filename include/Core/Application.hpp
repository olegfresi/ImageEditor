/*
 * Project: ImageEditor
 * File: Application.hpp
 * Author: olegfresi
 * Created: 20/02/26 16:54
 * 
 * Copyright © 2026 olegfresi
 * 
 * Licensed under the MIT License. You may obtain a copy of the License at:
 * 
 *     https://opensource.org/licenses/MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once
#include <string>
#include "Document.hpp"
#include "Window.hpp"
#include "File.hpp"


namespace Editor
{
    struct AppConfiguration
    {
        std::string name;
        uint32_t winWidth;
        uint32_t winHeight;
    };

    class Application
    {
    public:
        explicit Application(AppConfiguration config);

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Init();
        int Run(int argc, char** argv);
        void ShutDown();

        std::unique_ptr<Document> NewDocument();
        std::unique_ptr<Document> NewDocument(std::filesystem::path path);
        std::unique_ptr<Document> NewDocumentFromFile(const File& file);

        void OpenDocument(std::unique_ptr<Document> document);
        void CloseDocument(std::unique_ptr<Document> document);

    private:
        AppConfiguration m_config;
        Glib::RefPtr<Gtk::Application> m_app;

        std::list<File> m_recentFiles;
        std::vector<std::unique_ptr<Document>> m_Documents;
    };
}
