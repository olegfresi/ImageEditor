/*
 * Project: ImageEditor
 * File: Window.hpp
 * Author: olegfresi
 * Created: 20/02/26 17:51
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
#include <filesystem>
#include <string>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/picture.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include "Document.hpp"
#include "../Filters/Filter.hpp"
#include "../Widget/Histogram.hpp"
#include "../Widget/ToneCurve.hpp"


namespace Editor
{
    /**
    * Main application window for the image editor.
    *
    * Displays the image editing interface with tools, menus, and visualization widgets.
    * Manages the current image, display buffer, and interaction with filters and adjustment tools.
    * Built on top of GTK4 for cross-platform UI support.
    */
    class Window : public Gtk::Window
    {
    public:
        /**
        * Constructor for the main window.
        *
        * Initializes the application window with specified dimensions and title.
        * Sets up the GTK UI, menus, widgets, and signal handlers.
        *
        * @param width Window width in pixels
        * @param height Window height in pixels
        * @param title Window title string
        */
        Window(int width, int height, std::string title);

        ~Window() override;

        /**
        * Apply a filter effect to the current image.
        *
        * Applies the specified filter type to modify the image appearance.
        * Updates the display and histogram after filter application.
        *
        * @param filterType The type of filter to apply (blur, edge detection, etc.)
        */
        void ApplyFilter(Filter::FilterType filterType);

        /**
        * Set the document associated with this window.
        *
        * Associates a document with the window for editing and display.
        * Updates the window's image and display accordingly.
        *
        * @param document Pointer to the document to set.
        */
        void SetDocument(Document* document);

        /**
        * Set the callback for opening documents.
        *
        * Sets a callback function that will be called when the user wants to open a document.
        * This allows the window to communicate with the application for file operations.
        *
        * @param cb The callback function to set, taking a file path as parameter.
        */
        void SetOpenDocumentCallback(std::function<void(const std::filesystem::path&)> cb);

        /**
        * Load a document into the window.
        *
        * Loads the document's image data and updates the window's display and widgets.
        * Sets up the image for editing and initializes related components.
        *
        * @param doc Pointer to the document to load.
        */
        void LoadDocument(Document* doc);


        sigc::signal<void()> signal_new_document;
        sigc::signal<void()> signal_save_document;
        std::function<void(const std::filesystem::path&)> m_openDocumentCallback;

    protected:
        /**
        * File import handler.
        *
        * Opens a file dialog to allow the user to load an image file.
        * Loads the selected image and updates the display.
        */
        void OnImport();

        /**
        * File save handler.
        *
        * Saves the current image to disk at the current file path.
        * If no file path is set, prompts user for save location.
        */
        void OnSave();

        /**
        * Rotate image handler.
        *
        * Rotates the current image 90 degrees clockwise.
        * Updates display and histogram after rotation.
        */
        void OnRotate();

        /**
        * Flip image handler.
        *
        * Flips the current image horizontally (mirror across vertical axis).
        * Updates display and histogram after flip.
        */
        void OnFlip();

        /**
        * About dialog handler.
        *
        * Displays information about the application and credits.
        */
        void OnAbout();

        /**
        * Grayscale filter handler.
        *
        * Converts the current image to grayscale by applying a luminance-based
        * transformation. Updates the display and histogram after conversion.
        */
        void OnGrayScale();

    private:

        /**
        * Initialize and setup the application menu.
        *
        * Creates menu items, actions, and signal handlers for user interactions.
        * Sets up keyboard shortcuts and action groups.
        */
        void SetupMenu();

        /**
        * Tone curve drag finished handler.
        *
        * Called when the user finishes adjusting the tone curve.
        * Applies the tone curve LUT to the image.
        */
        void OnCurveDragFinished();

        /**
        * Update the display from the current document's image data.
        *
        * Retrieves the image from the associated document and creates a texture
        * to display it in the picture widget. This method refreshes the visual
        * representation of the document's current state.
        */
        void UpdateDisplayFromDocument();

        /**
        * Notify that the image has changed and update the display.
        *
        * Called when the image data has been modified to refresh the visual display
        * and optionally update the histogram visualization. Ensures the UI stays
        * synchronized with the current image state.
        *
        * @param updateHistogram Whether to recalculate the histogram (can be slow)
        */
        void NotifyImageChanged(bool updateHistogram);

        /**
        * Initialize drag and drop functionality for the window.
        *
        * Sets up GTK drag and drop handlers to allow users to drag files
        * into the window for opening. Configures the drop target and signal connections.
        */
        void SetupDragAndDrop();

        /**
        * Handle drag and drop file drop events.
        *
        * Called when a file is dropped onto the window. Processes the dropped
        * data and attempts to open the file as an image document.
        *
        * @param value The dropped value containing file information
        * @param x X-coordinate of the drop location
        * @param y Y-coordinate of the drop location
        * @return True if the drop was handled successfully, false otherwise
        */
        bool OnDrop(const Glib::ValueBase& value, double x, double y) const;

        /**
        * Process a file path string for opening.
        *
        * Validates and handles a file path string, typically from drag and drop
        * or other external sources. Attempts to open the file as a document.
        *
        * @param path_str The file path string to process
        * @return True if the file was processed successfully, false otherwise
        */
        bool HandleFilePath(const std::string& path_str) const;

        /**
        * Add an action to the action group with optional parameters.
        *
        * Template function for registering menu actions with lambdas.
        * Supports capture of parameters for action handlers.
        *
        * @param actionName Name of the action to register
        * @param callable The callback function or lambda
        * @param args Optional arguments to capture in the callback
        */
        template<typename Callable, typename... Args>
        void AddAction(const std::string& actionName, Callable&& callable, Args&&... args)
        {
            m_actionGroup->add_action(actionName,
                                      [this,
                                          func = std::forward<Callable>(callable),
                                          ...capturedArgs = std::forward<Args>(args)]() mutable {
                                          std::invoke(func, this, capturedArgs...);
                                      });
        }

        int m_width;
        int m_height;

        std::string m_title;
        std::string m_currentFilePath;

        Document* m_document = nullptr;

        std::vector<uint8_t> m_displayBuffer;
        std::vector<Pixel> m_originalPixelsBackup;

        Gtk::Box m_vbox{Gtk::Orientation::VERTICAL};
        Gtk::Box m_imageContainer{Gtk::Orientation::VERTICAL};
        Gtk::Box m_controlsBox;

        Gtk::Picture m_picture;
        Gtk::ScrolledWindow m_scrolled;
        Gtk::Label m_infoLabel{"No image loaded"};

        Gtk::Frame m_imageFrame;
        Gtk::Frame m_histogramFrame;
        Gtk::Frame m_curveFrame;

        Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
        Glib::RefPtr<Gdk::Pixbuf> m_originalPixbuf;
        Glib::RefPtr<Gio::SimpleActionGroup> m_actionGroup;

        Editor::Widget::ToneCurveWidget m_toneCurve;
        Editor::Widget::HistogramWidget m_histogram;
    };
}

