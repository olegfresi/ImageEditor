#include "../../include/Core/Window.hpp"
#include <iostream>
#include <giomm/menu.h>
#include <gtkmm/box.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/alertdialog.h>

namespace Editor
{
    Window::Window(int width, int height, std::string title) :
        m_width{width}, m_height{height}, m_title{std::move(title)}, m_image{"", width, height, 4}
    {
        set_default_size(m_width, m_height);
        set_title(m_title);

        set_resizable(false);
        set_deletable(true);

        m_actionGroup = Gio::SimpleActionGroup::create();

        m_actionGroup->add_action("import", sigc::mem_fun(*this, &Window::OnImport));
        m_actionGroup->add_action("save",   sigc::mem_fun(*this, &Window::OnSave));
        m_actionGroup->add_action("rotate", [] {
            // TODO
        });

        m_actionGroup->add_action("flip", [] {
            // TODO
        });

        m_actionGroup->add_action("grayscale", [this] {
            ApplyFilter(m_image, FilterType::GrayScale);
            UpdateFromImage();
        });

        m_actionGroup->add_action("blur",[this] {
            ApplyFilter(m_image, FilterType::Blur);
            UpdateFromImage();
        });

        m_actionGroup->add_action("sharpen",[this] {
            ApplyFilter(m_image, FilterType::Sharpen);
            UpdateFromImage();
        });

        m_actionGroup->add_action("emboss",[this] {
            ApplyFilter(m_image, FilterType::Emboss);
            UpdateFromImage();
        });

        m_actionGroup->add_action("edge_detect",[this] {
            ApplyFilter(m_image, FilterType::EdgeDetect);
            UpdateFromImage();
        });

        m_actionGroup->add_action("about", [this] {
            auto about_dialog = Gtk::AlertDialog::create("Image Editor v1.0");
            about_dialog->set_detail("Created with gtkmm4 and C++\n2026 Edition");
            about_dialog->show(*this);
        });

        insert_action_group("win", m_actionGroup);

        auto menu = Gio::Menu::create();

        auto submenuFile = Gio::Menu::create();
        submenuFile->append("Import", "win.import");
        submenuFile->append("Save", "win.save");
        menu->append_submenu("File", submenuFile);

        auto submenuEdit = Gio::Menu::create();
        submenuEdit->append("Rotate", "win.rotate");
        submenuEdit->append("Flip", "win.flip");
        menu->append_submenu("Edit", submenuEdit);

        auto submenuFilter = Gio::Menu::create();
        submenuFilter->append("GrayScale", "win.grayscale");
        submenuFilter->append("Blur", "win.blur");
        submenuFilter->append("Sharpen", "win.sharpen");
        submenuFilter->append("Emboss", "win.emboss");
        submenuFilter->append("Edge Detect", "win.edge_detect");
        menu->append_submenu("Filter", submenuFilter);

        auto submenuAbout = Gio::Menu::create();
        submenuAbout->append("Info App", "win.about");
        menu->append_submenu("About", submenuAbout);

        auto menubar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu);

        m_picture.set_hexpand(true);
        m_picture.set_vexpand(true);
        m_picture.set_can_shrink(true);
        m_picture.set_keep_aspect_ratio(true);

        m_infoLabel.set_halign(Gtk::Align::CENTER);
        m_infoLabel.set_margin_top(10);
        m_infoLabel.set_margin_bottom(10);
        m_infoLabel.set_markup("<span color='gray'> Select an image to start editing </span>");

        m_imageContainer.set_orientation(Gtk::Orientation::VERTICAL);
        m_imageContainer.set_expand(true);
        m_imageContainer.set_valign(Gtk::Align::CENTER);

        m_imageContainer.append(m_picture);
        m_imageContainer.append(m_infoLabel);

        m_image_frame.set_child(m_imageContainer);
        m_image_frame.set_margin_top(20);
        m_image_frame.set_margin_bottom(20);
        m_image_frame.set_margin_start(20);
        m_image_frame.set_margin_end(20);

        m_scrolled.set_child(m_image_frame);
        m_scrolled.set_expand(true);
        m_scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
        m_scrolled.signal_map().connect(sigc::mem_fun(*this, &Window::UpdateImageSize));

        m_vbox.set_orientation(Gtk::Orientation::VERTICAL);
        m_vbox.append(*menubar);
        m_vbox.append(m_scrolled);

        set_child(m_vbox);
    }

    void Window::OnImport()
    {
        auto dialog = Gtk::FileDialog::create();

        dialog->open(*this, [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result)
        {
            try
            {
                auto file = dialog->open_finish(result);
                m_originalPixbuf = Gdk::Pixbuf::create_from_file(file->get_path());
                m_picture.set_pixbuf(m_originalPixbuf);

                m_infoLabel.set_markup(
                    "<b>File:</b> " + file->get_basename() +
                    " <span color='gray'>(" +
                    std::to_string(m_originalPixbuf->get_width()) + "x" +
                    std::to_string(m_originalPixbuf->get_height()) +
                    ")</span>"
                );

                UpdateImageSize();
                m_image.LoadImage(file->get_path());
            }
            catch (const Glib::Error& ex)
            {
                std::cerr << "Error: " << ex.what() << std::endl;
            }
        });
    }

    void Window::OnSave()
    {

    }

    void Window::ApplyFilter(Image& img, FilterType filterType)
    {
        img.ApplyFilter(filterType);
    }

    void Window::UpdateImageSize()
    {
        if (!m_originalPixbuf)
            return;

        int target_w = m_scrolled.get_allocated_width();
        int target_h = m_scrolled.get_allocated_height();

        if (target_w <= 0 || target_h <= 0)
            return;

        double ratio_w = static_cast<double>(target_w) / m_originalPixbuf->get_width();
        double ratio_h = static_cast<double>(target_h) / m_originalPixbuf->get_height();
        double ratio = std::min(ratio_w, ratio_h);

        if (ratio > 1.0)
            ratio = 1.0;

        int new_w = static_cast<int>(m_originalPixbuf->get_width() * ratio);
        int new_h = static_cast<int>(m_originalPixbuf->get_height() * ratio);

        auto scaled = m_originalPixbuf->scale_simple(new_w, new_h, Gdk::InterpType::BILINEAR);
    }

    void Window::SetupMenu()
    {
        m_actionGroup->add_action("import", sigc::mem_fun(*this, &Window::OnImport));

        insert_action_group("win", m_actionGroup);

        auto menu_model = Gio::Menu::create();
        auto submenu_file = Gio::Menu::create();
        submenu_file->append("Import", "win.import");
        menu_model->append_submenu("File", submenu_file);

        auto menubar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu_model);
        m_vbox.append(*menubar);
    }

    void Window::UpdateFromImage()
    {
        auto pixbuf = PixbufFromImage(m_image);
        m_picture.set_pixbuf(pixbuf);
    }

    Glib::RefPtr<Gdk::Pixbuf> Window::PixbufFromImage(const Image& img)
    {
        std::vector<uint8_t> buffer;
        buffer.reserve(img.GetWidth() * img.GetHeight() * img.GetChannels());

        for (const auto& px : img.GetPixelData())
        {
            buffer.push_back(px.GetR());
            buffer.push_back(px.GetG());
            buffer.push_back(px.GetB());
            if (img.GetChannels() == 4)
                buffer.push_back(px.GetA());
        }

        auto pixbuf = Gdk::Pixbuf::create_from_data(
            buffer.data(),
            Gdk::Colorspace::RGB,
            img.GetChannels() == 4,
            8,
            img.GetWidth(),
            img.GetHeight(),
            img.GetWidth() * img.GetChannels()
        );

        return pixbuf;
    }
}