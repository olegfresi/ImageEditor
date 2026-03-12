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
#include <string>
#include <giomm/simpleactiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/picture.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include "Image.hpp"
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

        /**
        * Apply a filter effect to the current image.
        *
        * Applies the specified filter type to modify the image appearance.
        * Updates the display and histogram after filter application.
        *
        * @param filterType The type of filter to apply (blur, edge detection, etc.)
        */
        void ApplyFilter(Filter::FilterType filterType);

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

    private:
        int m_width;
        int m_height;
        std::string m_title;
        Image m_image;

        std::vector<uint8_t> m_displayBuffer;
        std::vector<Pixel> m_originalPixelsBackup;
        std::string m_currentFilePath;

        Gtk::Box m_vbox{Gtk::Orientation::VERTICAL};
        Gtk::Box m_imageContainer{Gtk::Orientation::VERTICAL};
        Gtk::Label m_infoLabel{"No image loaded"};
        Gtk::Picture m_picture;
        Gtk::Frame m_imageFrame;
        Gtk::ScrolledWindow m_scrolled;
        Glib::RefPtr<Gdk::Pixbuf> m_pixbuf;
        Glib::RefPtr<Gdk::Pixbuf> m_originalPixbuf;
        Glib::RefPtr<Gio::SimpleActionGroup> m_actionGroup;

        Editor::Widget::ToneCurveWidget m_toneCurve;
        Editor::Widget::HistogramWidget m_histogram;

        Gtk::Frame m_histogramFrame;
        Gtk::Frame m_curveFrame;
        Gtk::Box m_controlsBox;

        /**
        * Initialize and setup the application menu.
        *
        * Creates menu items, actions, and signal handlers for user interactions.
        * Sets up keyboard shortcuts and action groups.
        */
        void SetupMenu();

        /**
        * Update window display from current image.
        *
        * Converts image data to pixbuf format and updates the display.
        * Optionally updates the histogram visualization.
        *
        * @param updateHistogram Whether to recalculate the histogram (can be slow)
        */
        void UpdateFromImage(bool updateHistogram);

        /**
        * Load an image from a file path.
        *
        * Reads image data from disk and updates all display elements.
        * Stores the file path for future save operations.
        *
        * @param path Path to the image file to load
        */
        void LoadImageFromPath(const std::filesystem::path& path);

        /**
        * Convert Image object to GTK Pixbuf.
        *
        * Converts the internal Image representation to a Gdk::Pixbuf
        * suitable for GTK display.
        *
        * @param img The Image to convert
        * @return Pixbuf representation of the image
        */
        Glib::RefPtr<Gdk::Pixbuf> PixbufFromImage(const Image& img);

        /**
        * Update display without reloading image data.
        *
        * Refreshes only the visual display without reprocessing the image.
        * Faster than full update when only display needs refresh.
        */
        void UpdateDisplayOnly();

        /**
        * Tone curve drag finished handler.
        *
        * Called when the user finishes adjusting the tone curve.
        * Applies the tone curve LUT to the image.
        */
        void OnCurveDragFinished();

        /**
        * Apply a tone curve LUT to the image quickly.
        *
        * Uses a precomputed lookup table for fast pixel intensity remapping.
        * Much faster than computing the curve for each pixel individually.
        *
        * @param lut 256-entry lookup table mapping input intensities to output
        */
        void FastLut(const std::vector<uint8_t>& lut);


        /**
        * Apply a lookup table transformation to temporary pixel data.
        *
        * Modifies the temporary pixel buffer using the provided LUT for
        * real-time preview of tone curve adjustments without affecting
        * the original image data.
        *
        * @param lut 256-entry lookup table mapping input intensities to output
        */
        void ApplyLUTToTempPixels(const std::vector<uint8_t>& lut);

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
        template <typename Callable, typename... Args>
        void AddAction(const std::string& actionName, Callable&& callable, Args&&... args)
        {
            m_actionGroup->add_action(actionName,
                [this,
                 func = std::forward<Callable>(callable),
                 ...capturedArgs = std::forward<Args>(args)]() mutable
                {
                    std::invoke(func, this, capturedArgs...);
                });
        }
    };
}