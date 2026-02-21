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

namespace Editor
{
    class Window : public Gtk::Window
    {
    public:
        Window(int width, int height, std::string title);

        void ApplyFilter(Image& image, FilterType filterType);

    protected:
        void OnImport();
        void OnSave();
        void SetupMenu();
        void UpdateImageSize();
        void UpdateFromImage();
        Glib::RefPtr<Gdk::Pixbuf> PixbufFromImage(const Image& img);

    private:
        int m_width;
        int m_height;
        std::string m_title;
        Image m_image;

        Gtk::Box m_vbox{Gtk::Orientation::VERTICAL};
        Gtk::ScrolledWindow m_scrolled;
        Gtk::Picture m_picture;
        Gtk::Frame m_image_frame;
        Glib::RefPtr<Gio::SimpleActionGroup> m_actionGroup;
        Gtk::Box m_imageContainer{Gtk::Orientation::VERTICAL};
        Glib::RefPtr<Gdk::Pixbuf> m_originalPixbuf;
        Gtk::Label m_infoLabel{"Nessuna immagine caricata"};
    };
}
