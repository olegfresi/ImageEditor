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
#include "FileManager.hpp"
#include "Window.hpp"
#include <experimental/memory>


namespace Editor
{
    /**
    * Configuration structure for the Image Editor application.
    *
    * Contains essential settings required to initialize and configure the main application window
    * and application behavior. These settings are used during the application initialization phase.
    */
    struct AppConfiguration
    {
        std::string name;
        uint32_t winWidth;
        uint32_t winHeight;
    };

    /**
    * Main application class managing the Image Editor lifecycle and windows.
    *
    * The Application class serves as the central hub of the image editor, handling:
    * - Application initialization and shutdown
    * - Document creation and management
    * - Main event loop and window management
    * - File I/O operations through FileManager
    *
    * This class follows the non-copyable pattern to prevent multiple instances and ensure
    * single application lifecycle management.
    */
    class Application
    {
    public:
        /**
        * Construct the Application with configuration settings.
        *
        * Initializes the Application object with the provided configuration parameters.
        * The application is not fully initialized until Init() is called.
        *
        * @param config AppConfiguration struct containing name, window width, and window height
        */
        explicit Application(AppConfiguration config);


        ~Application();

        /**
        * Deleted copy constructor to prevent application duplication.
        *
        * The Application manages global state and resources, so only one instance should exist.
        * Copying is explicitly deleted to enforce single-instance pattern.
        */
        Application(const Application&) = delete;

        /**
        * Deleted assignment operator to prevent application duplication.
        *
        * The Application manages global state and resources, so reassignment is not allowed.
        * This enforces single-instance pattern and prevents resource conflicts.
        */
        Application& operator=(const Application&) = delete;

        /**
        * Shut down the application and release all resources.
        *
        * Performs cleanup operations including closing all open windows and documents,
        * releasing GTK+ resources, and freeing allocated memory.
        * Should be called before the Application object is destroyed.
        */
        void ShutDown();

        /**
        * Enter the main application event loop.
        *
        * Starts the GTK+ main event loop, which processes user events, redraws windows,
        * and handles all interaction with the application until the user quits.
        * This function blocks until the application terminates.
        *
        * @param argc Command line argument count (typically passed from main())
        * @param argv Command line argument array (typically passed from main())
        * @return Exit code of the application (typically 0 for success)
        */
        int Run(int argc, char** argv) const;

        /**
        * Create a new document from a file path.
        *
        * Loads an image from the specified path using the FileManager and creates a new document.
        * Also creates a new window to display and edit the document.
        * The document is added to the application's document list.
        *
        * @param path The file path to load the image from.
        * @return Pointer to the newly created document, or nullptr if loading failed.
        */
        Document* NewDocument(const std::filesystem::path& path);

        /**
        * Close a document and remove its associated window.
        *
        * Closes the window containing the document and releases its resources.
        * Unsaved changes will be lost unless explicitly saved through the file manager.
        * The document will no longer be managed by the application after this call.
        *
        * @param document Pointer to the Document to close
        */
        void CloseDocument(Document* document);

        /**
        * Handle document opening request from a window.
        *
        * Creates a new document by loading an image from the specified file path.
        * If the document is successfully created, it is loaded into the provided window for editing.
        * This method is typically called as a callback when the user requests to open a document.
        *
        * @param path The file path of the image to open
        * @param win Pointer to the Window that will display the document
        */
        void OnWindowOpenDocument(const std::filesystem::path& path, Window* win);

    private:

        /**
        * Handle the GTK application activation signal.
        *
        * Called when the GTK application is activated (e.g., when the app starts or is brought to foreground).
        * Creates the main application window if it doesn't exist, sets up document opening callbacks,
        * and presents the window to the user.
        */
        void OnActivate();

        AppConfiguration m_config{};
        std::vector<std::unique_ptr<Document>> m_documents{};
        std::unique_ptr<Window> m_window{};
        Glib::RefPtr<Gtk::Application> m_app{};

        static inline FileManager m_fileManager{};
    };
}
