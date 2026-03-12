#include <iostream>
#include <giomm/menu.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/droptarget.h>
#include <gdkmm/memorytexture.h>
#include <glibmm/main.h>
#include "../../include/Core/Window.hpp"
#include "../../include/Filters/Blur.hpp"
#include "../../include/Tools/Profiler.hpp"

namespace Editor
{
    Window::Window(int width, int height, std::string title)
        : m_width{width}, m_height{height}, m_title{std::move(title)}, m_image{"", width, height, 4} {

        set_default_size(m_width, m_height);
        set_title(m_title);
        set_resizable(true);

        // --- ACTION GROUP SETUP ---
        m_actionGroup = Gio::SimpleActionGroup::create();
        AddAction("import", &Window::OnImport);
        AddAction("save", &Window::OnSave);
        AddAction("rotate", &Window::OnRotate);
        AddAction("flip", &Window::OnFlip);
        AddAction("about", &Window::OnAbout);
        AddAction("grayscale", &Window::ApplyFilter, Filter::FilterType::GrayScale);

        m_actionGroup->add_action("blur", [this] { ApplyFilter(Filter::FilterType::Blur); UpdateFromImage(true); });
        m_actionGroup->add_action("sharpen", [this] { ApplyFilter(Filter::FilterType::Sharpen); UpdateFromImage(true); });
        m_actionGroup->add_action("emboss", [this] { ApplyFilter(Filter::FilterType::Emboss); UpdateFromImage(true); });
        m_actionGroup->add_action("edge_detect", [this] { ApplyFilter(Filter::FilterType::EdgeDetect); UpdateFromImage(true); });

        insert_action_group("win", m_actionGroup);

        // --- MENU BAR SETUP ---
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

        // --- UI LAYOUT ---
        m_picture.set_hexpand(true);
        m_picture.set_vexpand(true);
        m_picture.set_keep_aspect_ratio(true);

        m_infoLabel.set_markup("<span color='gray'> Select an image to start editing </span>");

        // --- UI LAYOUT CORRECTION ---
        m_controlsBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_controlsBox.set_spacing(20); // More breathing room between boxes
        m_controlsBox.set_margin_bottom(20);
        m_controlsBox.set_margin_start(20);
        m_controlsBox.set_margin_end(20);

        // Histogram: must expand to fill the left side
        m_histogramFrame.set_label("RGB Histogram");
        m_histogramFrame.set_hexpand(true);
        m_histogramFrame.set_vexpand(true);
        m_histogram.set_size_request(-1, 200);
        m_histogramFrame.set_child(m_histogram);

        // Tone Curve: should have a fixed or proportional size to stay square
        m_curveFrame.set_label("Tone Curve");
        m_curveFrame.set_hexpand(true);
        m_curveFrame.set_vexpand(true);
        m_curveFrame.set_child(m_toneCurve);

        // Ensure the widgets inside the frames fill the space
        m_histogram.set_hexpand(true);
        m_histogram.set_vexpand(true);
        m_toneCurve.set_hexpand(true);
        m_toneCurve.set_vexpand(true);
        m_toneCurve.set_size_request(256, 256);

        // Centering logic to avoid "empty space on the right"
        m_controlsBox.append(m_histogramFrame);
        m_controlsBox.append(m_curveFrame);

        m_imageContainer.set_orientation(Gtk::Orientation::VERTICAL);
        m_imageContainer.append(m_picture);
        m_imageContainer.append(m_infoLabel);

        m_scrolled.set_child(m_imageContainer);
        m_scrolled.set_expand(true);

        m_vbox.set_spacing(5);
        m_vbox.append(*menubar);
        m_vbox.append(m_scrolled);
        m_vbox.append(m_controlsBox);
        set_child(m_vbox);

        // --- DRAG AND DROP ---
        auto drop_target = Gtk::DropTarget::create(Gio::File::get_type(), Gdk::DragAction::COPY);
        drop_target->signal_drop().connect([this](const Glib::ValueBase& value, double, double) -> bool {
            GValue const* g_value = value.gobj();
            if(G_VALUE_HOLDS(g_value, G_TYPE_FILE)) {
                if(gpointer g_file_ptr = g_value_get_object(g_value)) {
                    if(auto file = Glib::wrap(static_cast<GFile*>(g_file_ptr), true)) {
                        LoadImageFromPath(file->get_path());
                        return true;
                    }
                }
            }
            return false;
        }, false);

        add_controller(drop_target);

        m_toneCurve.SignalCurveChanged().connect([this]() {
            const auto& lut = m_toneCurve.GetLUT();
            FastLut(lut);
            // Update RGB Histogram with high stride for smoothness
            m_histogram.UpdateFromBuffer(m_image.GetPixelData(), 64);
            UpdateDisplayOnly();
        });

        m_toneCurve.SignalDragFinished().connect([this]() {
            // Precise histogram update when interaction ends
            m_histogram.SetImage(m_image.GetPixelData(), m_image.GetWidth(), m_image.GetHeight());
        });
    }

    void Window::OnImport()
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->open(*this, [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
            try
            {
                if(auto file = dialog->open_finish(result))
                    LoadImageFromPath(file->get_path());
            } catch(const Glib::Error& ex)
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
            m_originalPixelsBackup = m_image.GetPixelData();
            m_currentFilePath = path;

            // Generate the fixed background histogram for the curve widget
            m_histogram.SetImage(m_originalPixelsBackup, m_image.GetWidth(), m_image.GetHeight());
            auto initialLum = m_histogram.GetLuminanceHistogram();
            m_toneCurve.SetHistogram(initialLum);

            m_infoLabel.set_markup("<b>File:</b> " + path.filename().string());
            UpdateFromImage(false);
        } catch(const Glib::Error& ex) {
            std::cerr << "Load error: " << ex.what() << std::endl;
        }
    }

    void Window::OnSave()
    {
        // If there is no image to save, show an error dialog
        if(!m_pixbuf && !m_originalPixbuf)
        {
            auto dialog = Gtk::AlertDialog::create("No image to save");
            dialog->set_detail("Load an image first.");
            dialog->show(*this);
            return;
        }

        Glib::RefPtr<Gdk::Pixbuf> pixbufToSave = m_pixbuf ? m_pixbuf : m_originalPixbuf;
        if(!pixbufToSave)
            return;

        std::filesystem::path original_path = m_currentFilePath;
        std::string default_name = original_path.filename().string();

        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Save Image");

        // If there is a current file path, set it as the default name
        if(!m_currentFilePath.empty())
            dialog->set_initial_name(default_name);

        dialog->save(*this, [this, pixbufToSave, dialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
            try
            {
                auto file = dialog->save_finish(result);
                std::string filePath = file->get_path();

                std::filesystem::path save_path(filePath);
                std::string ext = save_path.extension().string();

                if(ext.empty())
                    filePath += ".png";

                std::string format = save_path.extension().string();
                if(!format.empty() && format[0] == '.')
                    format = format.substr(1);
                if(format == "jpg")
                    format = "jpeg";
                else if(format == "tif")
                    format = "tiff";

                pixbufToSave->save(filePath, format);

                auto success_dialog = Gtk::AlertDialog::create("Image saved");
                success_dialog->set_detail("The image has been saved successfully.");
                success_dialog->show(*this);
            } catch(const Glib::Error& ex)
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
        UpdateFromImage(false);
    }

    void Window::OnFlip()
    {
        m_image.FlipHorizontal();
        UpdateFromImage(false);
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
        UpdateFromImage(false);
        m_picture.queue_draw();
    }

    void Window::SetupMenu()
    {
        insert_action_group("win", m_actionGroup);

        auto menu_model = Gio::Menu::create();
        auto submenu_file = Gio::Menu::create();
        submenu_file->append("Import", "win.import");
        menu_model->append_submenu("File", submenu_file);

        auto menubar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu_model);
        m_vbox.append(*menubar);
    }

    void Window::UpdateFromImage(bool updateHistogram)
    {
        UpdateDisplayOnly();
        if(updateHistogram) m_histogram.SetImage(m_image.GetPixelData(), m_image.GetWidth(), m_image.GetHeight());
    }

    void Window::UpdateDisplayOnly()
    {
        auto bytes = Glib::Bytes::create(m_image.GetPixelData().data(), m_image.GetPixelData().size() * 4);
        auto texture = Gdk::MemoryTexture::create(
                m_image.GetWidth(), m_image.GetHeight(),
                Gdk::MemoryTexture::Format::R8G8B8A8, bytes, m_image.GetWidth() * 4
        );
        m_picture.set_paintable(texture);
    }

    void Window::OnCurveDragFinished()
    {
        const int width = m_image.GetWidth();
        const int height = m_image.GetHeight();

        m_histogram.SetImage(m_image.GetPixelData(), width, height);
        m_histogram.queue_draw();
    }

    void Window::FastLut(const std::vector<uint8_t>& lut)
    {
        const auto* src = reinterpret_cast<const uint8_t*>(m_originalPixelsBackup.data());
        auto* dst = reinterpret_cast<uint8_t*>(m_image.GetPixelData().data());
        const uint8_t* lut_ptr = lut.data();
        const size_t total_bytes = m_originalPixelsBackup.size() * 4;

        for(size_t i = 0; i < total_bytes; i += 4)
        {
            dst[i]     = lut_ptr[src[i]];
            dst[i + 1] = lut_ptr[src[i + 1]];
            dst[i + 2] = lut_ptr[src[i + 2]];
        }
    }

    void Window::ApplyLUTToTempPixels(const std::vector<uint8_t>& lut)
    {
        std::vector<Pixel> tempPixels = m_originalPixelsBackup;
        for(auto& px : tempPixels)
        {
            uint8_t r = px.GetR();
            uint8_t g = px.GetG();
            uint8_t b = px.GetB();
            px.SetPixel(lut[r], lut[g], lut[b], px.GetA());
        }

        // Update histogram
        m_histogram.SetImage(tempPixels, m_image.GetWidth(), m_image.GetHeight());
        m_histogram.queue_draw();
    }

    Glib::RefPtr<Gdk::Pixbuf> Window::PixbufFromImage(const Image& img)
    {
        const int width = img.GetWidth();
        const int height = img.GetHeight();
        const int channels = img.GetChannels();

        if(!m_pixbuf)
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