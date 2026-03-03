#include <iostream>
#include <giomm/menu.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/droptarget.h>
#include "../../include/Core/Window.hpp"
#include "../../include/Filters/Blur.hpp"
#include "../../include/Tools/Profiler.hpp"

namespace Editor
{
    Window::Window(int width, int height, std::string title) : m_width{ width }, m_height{ height },
                                                               m_title{ std::move(title) },
                                                               m_image{ "", width, height, 4 }
    {
        set_default_size(m_width, m_height);
        set_title(m_title);

        set_resizable(true);
        set_deletable(true);

        // Create group of actions for user interface commands
        m_actionGroup = Gio::SimpleActionGroup::create();

        AddAction("import", &Window::OnImport);
        AddAction( "save", &Window::OnSave);
        AddAction("rotate", &Window::OnRotate);
        AddAction("flip", &Window::OnFlip);
        AddAction("about", &Window::OnAbout);
        AddAction("grayscale", &Window::ApplyFilter, Filter::FilterType::GrayScale);

        m_actionGroup->add_action("blur", [this] {
            ApplyFilter(Filter::FilterType::Blur);
            UpdateFromImage();
        });

        m_actionGroup->add_action("sharpen", [this] {
            ApplyFilter(Filter::FilterType::Sharpen);
            UpdateFromImage();
        });

        m_actionGroup->add_action("emboss", [this] {
            ApplyFilter(Filter::FilterType::Emboss);
            UpdateFromImage();
        });

        m_actionGroup->add_action("edge_detect", [this] {
            ApplyFilter(Filter::FilterType::EdgeDetect);
            UpdateFromImage();
        });

        // All previous actions are grouped in the win action group
        insert_action_group("win", m_actionGroup);

        // Create main menu bar
        auto menu = Gio::Menu::create();

        // Create submenu for file actions
        auto submenuFile = Gio::Menu::create();
        submenuFile->append("Import", "win.import");
        submenuFile->append("Save", "win.save");
        menu->append_submenu("File", submenuFile);

        // Create submenu for edit actions
        auto submenuEdit = Gio::Menu::create();
        submenuEdit->append("Rotate", "win.rotate");
        submenuEdit->append("Flip", "win.flip");
        menu->append_submenu("Edit", submenuEdit);

        // Create submenu for filter actions
        auto submenuFilter = Gio::Menu::create();
        submenuFilter->append("GrayScale", "win.grayscale");
        submenuFilter->append("Blur", "win.blur");
        submenuFilter->append("Sharpen", "win.sharpen");
        submenuFilter->append("Emboss", "win.emboss");
        submenuFilter->append("Edge Detect", "win.edge_detect");
        menu->append_submenu("Filter", submenuFilter);

        // Create submenu for about actions
        auto submenuAbout = Gio::Menu::create();
        submenuAbout->append("Info App", "win.about");
        menu->append_submenu("About", submenuAbout);

        // Create popover menu bar with all previous submenus
        auto menubar = Gtk::make_managed <Gtk::PopoverMenuBar>(menu);

        // Set picture properties
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

        m_imageFrame.set_child(m_imageContainer);
        m_imageFrame.set_margin_top(20);
        m_imageFrame.set_margin_bottom(20);
        m_imageFrame.set_margin_start(20);
        m_imageFrame.set_margin_end(20);

        m_scrolled.set_child(m_imageFrame);
        m_scrolled.set_expand(true);
        m_scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

        m_vbox.set_orientation(Gtk::Orientation::VERTICAL);
        m_vbox.append(*menubar);
        m_vbox.append(m_scrolled);

        // Set the image frame as receiver of a drag action, copying the dropped resource
        auto drop_target = Gtk::DropTarget::create(Gio::File::get_type(), Gdk::DragAction::COPY);

        drop_target->signal_drop().connect([this](const Glib::ValueBase& value, double, double) -> bool {
            // Get the dropped file as GValue object
            GValue const *g_value = value.gobj();

            if (G_VALUE_HOLDS(g_value, G_TYPE_FILE))
            {
                // If GValue holds a GFile object then get a pointer to it and wrap it in a Glib::RefPtr
                if (gpointer g_file_ptr = g_value_get_object(g_value))
                    if (auto file = Glib::wrap(static_cast <GFile*>(g_file_ptr), true))
                    {
                        LoadImageFromPath(file->get_path());
                        return true;
                    }
            }
            return false;
        }, false);

        m_imageFrame.add_controller(drop_target);
        set_child(m_vbox);
    }

    void Window::OnImport()
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->open(*this, [this, dialog](const Glib::RefPtr <Gio::AsyncResult> &result) {
            try
            {
                if (auto file = dialog->open_finish(result))
                    LoadImageFromPath(file->get_path());
            } catch (const Glib::Error &ex)
            {
                std::cerr << "Error loading image: " << ex.what() << std::endl;
            }
        });
    }

    void Window::LoadImageFromPath(const std::filesystem::path& path)
    {
        try
        {
            m_image.LoadImage(path);
            m_originalPixbuf = Gdk::Pixbuf::create_from_data(
                reinterpret_cast<const guint8*>(m_image.GetPixelData().data()),
                Gdk::Colorspace::RGB,
                true,
                8,
                m_image.GetWidth(),
                m_image.GetHeight(),
                m_image.GetWidth() * m_image.GetChannels()
            );

            m_picture.set_pixbuf(m_originalPixbuf);
            m_currentFilePath = path;

            m_infoLabel.set_markup(
                "<b>File:</b> " + path.filename().string() +
                " <span color='gray'>(" +
                std::to_string(m_originalPixbuf->get_width()) + "x" +
                std::to_string(m_originalPixbuf->get_height()) +
                ")</span>"
            );

            m_pixbuf.reset();
        } catch (const Glib::Error &ex)
        {
            std::cerr << "Error loading image: " << ex.what() << std::endl;
            auto error_dialog = Gtk::AlertDialog::create("Error loading");
            error_dialog->set_detail(ex.what());
            error_dialog->show(*this);
        }
    }

    void Window::OnSave()
    {
        // If there is no image to save, show an error dialog
        if (!m_pixbuf && !m_originalPixbuf)
        {
            auto dialog = Gtk::AlertDialog::create("No image to save");
            dialog->set_detail("Load an image first.");
            dialog->show(*this);
            return;
        }

        Glib::RefPtr <Gdk::Pixbuf> pixbufToSave = m_pixbuf ? m_pixbuf : m_originalPixbuf;
        if (!pixbufToSave)
            return;

        std::filesystem::path original_path = m_currentFilePath;
        std::string default_name = original_path.filename().string();

        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Save Image");

        // If there is a current file path, set it as the default name
        if (!m_currentFilePath.empty())
            dialog->set_initial_name(default_name);

        dialog->save(*this, [this, pixbufToSave, dialog](const Glib::RefPtr <Gio::AsyncResult> &result) {
            try
            {
                auto file = dialog->save_finish(result);
                std::string filePath = file->get_path();

                std::filesystem::path save_path(filePath);
                std::string ext = save_path.extension().string();

                if (ext.empty())
                    filePath += ".png";

                std::string format = save_path.extension().string();
                if (!format.empty() && format[0] == '.')
                    format = format.substr(1);
                if (format == "jpg")
                    format = "jpeg";
                else if (format == "tif")
                    format = "tiff";

                pixbufToSave->save(filePath, format);

                auto success_dialog = Gtk::AlertDialog::create("Image saved");
                success_dialog->set_detail("The image has been saved successfully.");
                success_dialog->show(*this);
            } catch (const Glib::Error &ex)
            {
                auto error_dialog = Gtk::AlertDialog::create("Error saving image");
                error_dialog->set_detail(ex.what());
                error_dialog->show(*this);
            }
        });
    }

    void Window::OnRotate()
    {
        m_image.Rotate();
        UpdateFromImage();
    }

    void Window::OnFlip()
    {
        m_image.FlipHorizontal();
        UpdateFromImage();
    }

    void Window::OnAbout()
    {
        auto about_dialog = Gtk::AlertDialog::create("Image Editor v1.0");
        about_dialog->set_detail("Created with gtkmm4 and C++\n2026 Edition");
        about_dialog->show(*this);
    }

    void Window::ApplyFilter(Filter::FilterType filterType)
    {
        m_image.ApplyFilter(filterType);
        UpdateFromImage();
        m_picture.queue_draw();
    }

    void Window::SetupMenu()
    {
        insert_action_group("win", m_actionGroup);

        auto menu_model = Gio::Menu::create();
        auto submenu_file = Gio::Menu::create();
        submenu_file->append("Import", "win.import");
        menu_model->append_submenu("File", submenu_file);

        auto menubar = Gtk::make_managed <Gtk::PopoverMenuBar>(menu_model);
        m_vbox.append(*menubar);
    }

    void Window::UpdateFromImage()
    {
        const int width = m_image.GetWidth();
        const int height = m_image.GetHeight();
        const int channels = m_image.GetChannels();

        m_pixbuf = Gdk::Pixbuf::create_from_data(
            reinterpret_cast<const guint8*>(m_image.GetPixelData().data()),
            Gdk::Colorspace::RGB,
            true,
            8,
            width,
            height,
            width * channels
        );

        m_picture.set_pixbuf(m_pixbuf);
        m_picture.queue_draw();
    }

    Glib::RefPtr <Gdk::Pixbuf> Window::PixbufFromImage(const Image& img)
    {
        const int width = img.GetWidth();
        const int height = img.GetHeight();
        const int channels = img.GetChannels();

        if (!m_pixbuf)
        {
            m_pixbuf = Gdk::Pixbuf::create_from_data(
                reinterpret_cast<const guint8*>(img.GetPixelData().data()),
                Gdk::Colorspace::RGB,
                true,
                8,
                width,
                height,
                width * channels
            );
        }
        m_picture.queue_draw();

        return m_pixbuf;
    }
}