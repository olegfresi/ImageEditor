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


/**
 * Class for representing the main window of the application with all user interface elements
 * It plays the view role in the MVC pattern
 */

namespace Editor
{
    class Window : public Gtk::Window
    {
    public:
        /*
         * Constructor
         * @param width: width of the window
         * @param height: height of the window
         * @param title: title of the window
         */
        Window(int width, int height, std::string title);

        void ApplyFilter(Filter::FilterType filterType);

    protected:
        void OnImport();
        void OnSave();
        void OnRotate();
        void OnFlip();
        void OnAbout();

    private:
        int m_width;
        int m_height;
        std::string m_title;
        Image m_image;

        std::vector<uint8_t> m_displayBuffer;
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

        void SetupMenu();
        void UpdateFromImage();
        void LoadImageFromPath(const std::filesystem::path& path);
        Glib::RefPtr<Gdk::Pixbuf> PixbufFromImage(const Image& img);

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