#include <giomm/menu.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/droptarget.h>
#include <gdkmm/memorytexture.h>
#include <glibmm/main.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include <glib.h>
#include <ranges>
#include "../../include/Core/Window.hpp"
#include "../../include/Core/FileManager.hpp"
#include "../../include/Core/Utils.hpp"
#include "../../include/Core/Processing.hpp"


namespace Editor
{
    Window::Window(int width, int height, std::string title)
        : m_width{width}, m_height{height}, m_title{std::move(title)}
    {
        set_default_size(m_width, m_height);
        set_title(m_title);
        set_hide_on_close(true);
        set_resizable(true);

        // --- ACTION GROUP SETUP ---
        m_actionGroup = Gio::SimpleActionGroup::create();
        AddAction("import", &Window::OnImport);
        AddAction("save", &Window::OnSave);
        AddAction("rotate", &Window::OnRotate);
        AddAction("flip", &Window::OnFlip);
        AddAction("about", &Window::OnAbout);

        AddActionsToGroupAction();
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
        submenuFilter->append("Invert colors", "win.invert");
        submenuFilter->append("Edge Detect", "win.edge_detect");
        menu->append_submenu("Filter", submenuFilter);

        auto submenuAbout = Gio::Menu::create();
        submenuAbout->append("Info App", "win.about");
        menu->append_submenu("About", submenuAbout);

        auto menubar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu);

        // --- IMAGE DISPLAY SETUP ---
        m_picture.set_hexpand(true);
        m_picture.set_vexpand(true);
        m_picture.set_keep_aspect_ratio(true);
        m_picture.set_can_shrink(true);

        m_picture.set_margin_start(30);
        m_picture.set_margin_end(30);
        m_picture.set_margin_top(5);
        m_picture.set_margin_bottom(5);

        m_infoLabel.set_markup("<span color='gray'> Select an image to start editing </span>");
        m_infoLabel.set_margin_bottom(5);

        // --- IMAGE CONTAINER ---
        m_imageContainer.set_orientation(Gtk::Orientation::VERTICAL);
        m_imageContainer.append(m_picture);
        m_imageContainer.append(m_infoLabel);
        m_imageContainer.set_valign(Gtk::Align::FILL);

        m_scrolled.set_child(m_imageContainer);
        m_scrolled.set_expand(true);
        m_scrolled.set_propagate_natural_height(true);

        // --- BOTTOM CONTROLS BOX ---
        m_controlsBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_controlsBox.set_spacing(20);
        m_controlsBox.set_margin_bottom(10);
        m_controlsBox.set_margin_start(20);
        m_controlsBox.set_margin_end(20);
        m_controlsBox.set_margin_top(0);

        m_controlsBox.set_vexpand(false);
        m_controlsBox.set_valign(Gtk::Align::END);

        // Histogram Frame
        m_histogramFrame.set_label("RGB Histogram");
        m_histogramFrame.set_hexpand(true);
        m_histogramFrame.set_vexpand(false);
        m_histogram.set_size_request(-1, 140);
        m_histogramFrame.set_child(m_histogram);

        // Tone Curve Frame
        m_curveFrame.set_label("Tone Curve");
        m_curveFrame.set_hexpand(false);
        m_curveFrame.set_vexpand(false);
        m_toneCurve.set_size_request(180, 180);
        m_curveFrame.set_child(m_toneCurve);

        m_controlsBox.append(m_histogramFrame);
        m_controlsBox.append(m_curveFrame);

        // --- MAIN VERTICAL BOX ---
        m_vbox.set_spacing(0);
        m_vbox.append(*menubar);
        m_vbox.append(m_scrolled);
        m_vbox.append(m_controlsBox);
        set_child(m_vbox);

        SetupDragAndDrop();

        m_toneCurve.SignalCurveChanged().connect([this]() {
            const auto& lut = m_toneCurve.GetLUT();
            Utils::ApplyLut(m_document->GetImage().GetPixelData(), m_originalPixelsBackup, lut);

            NotifyImageChanged();

            if(m_document)
                m_histogram.SetImage(m_document->GetImage().GetPixelData(), 0, 0, 16);
        });

        m_toneCurve.SignalDragFinished().connect([this]() {
            if(m_document)
            {
                m_histogram.SetImage(m_document->GetImage().GetPixelData(), 0, 0, 1);
            }
        });
    }

    Window::~Window() { m_picture.set_paintable(nullptr); }

    void Window::OnImport()
    {
        auto dialog = Gtk::FileDialog::create();
        dialog->set_title("Open Image");

        dialog->open(*this, [this, dialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
            try
            {
                auto file = dialog->open_finish(result);
                if(!file)
                    return;

                std::filesystem::path path = file->get_path();

                if(m_openDocumentCallback)
                    m_openDocumentCallback(path);
            }
            catch(const Glib::Error& ex)
            {
                auto err = Gtk::AlertDialog::create("Load error");
                err->set_detail(ex.what());
                err->show(*this);
            }
        });
    }

    void Window::LoadDocument(Document* doc)
    {
        m_document = doc;
        auto pixelSpan = m_document->GetImage().GetPixelData();
        m_originalPixelsBackup.assign(pixelSpan.begin(), pixelSpan.end());

        NotifyImageChanged();

        auto initialLum = m_histogram.GetLuminanceHistogram();
        m_toneCurve.SetHistogram(initialLum);

        m_infoLabel.set_markup("<b>File:</b> " + m_document->GetFilePath().filename().string());
    }

    void Window::OnSave()
    {
        if(!m_document)
        {
            auto dialog = Gtk::AlertDialog::create("No document to save");
            dialog->set_detail("Load or create a document first.");
            dialog->show(*this);
            return;
        }

        try
        {
            auto fileDialog = Gtk::FileDialog::create();
            fileDialog->set_title("Save Document As");

            fileDialog->save(*this, [this, fileDialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
                try
                {
                    auto file = fileDialog->save_finish(result);
                    std::filesystem::path path = file->get_path();

                    m_document->SaveAs(path);

                    auto success_dialog = Gtk::AlertDialog::create("Document saved");
                    success_dialog->set_detail("The document has been saved successfully.");
                    success_dialog->show(*this);
                }
                catch(const Glib::Error& ex)
                {
                    auto error_dialog = Gtk::AlertDialog::create("Error saving document");
                    error_dialog->set_detail(ex.what());
                    error_dialog->show(*this);
                }
            });
        }
        catch(const std::exception& ex)
        {
            auto error_dialog = Gtk::AlertDialog::create("Error saving document");
            error_dialog->set_detail(ex.what());
            error_dialog->show(*this);
        }
    }

    void Window::OnRotate()
    {
        Processor::Rotate(m_document->GetImage());
        NotifyImageChanged();
    }

    void Window::OnFlip() // Horizontal
    {
        Processor::FlipVertical(m_document->GetImage());
        NotifyImageChanged();
    }

    void Window::OnAbout()
    {
        auto about_dialog = Gtk::AlertDialog::create("Image Editor v1.0");
        about_dialog->set_detail("Created with gtkmm4 and C++\n2026 Edition");
        about_dialog->show(*this);
    }

    void Window::ExecuteFilter(const std::function<void(Image&)>& filterTask)
    {
        if(!m_document)
            return;

        filterTask(m_document->GetImage());

        auto pixelSpan = m_document->GetImage().GetPixelData();
        m_originalPixelsBackup.assign(pixelSpan.begin(), pixelSpan.end());
        NotifyImageChanged();
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

    void Window::NotifyImageChanged()
    {
        if(!m_document)
            return;

        auto& image = m_document->GetImage();
        std::span<Pixel> pixels = image.GetPixelData();

        auto bytes = Glib::Bytes::create(pixels.data(), pixels.size() * sizeof(Pixel));
        auto texture = Gdk::MemoryTexture::create(
                image.GetWidth(), image.GetHeight(),
                Gdk::MemoryTexture::Format::R8G8B8A8,
                bytes, image.GetWidth() * 4
                );
        m_picture.set_paintable(texture);

        m_histogram.SetImage(pixels, image.GetWidth(), image.GetHeight());
    }

    void Window::UpdateDisplayFromDocument()
    {
        const auto& img = m_document->GetImage();
        auto bytes = Glib::Bytes::create((img.GetPixelData().data()),
                                         img.GetPixelData().size() * sizeof(Pixel)
                );

        auto texture = Gdk::MemoryTexture::create(
                img.GetWidth(),
                img.GetHeight(),
                Gdk::MemoryTexture::Format::R8G8B8A8,
                bytes,
                img.GetWidth() * 4
                );

        m_picture.set_paintable(texture);
    }

    void Window::OnCurveDragFinished()
    {
        const int width = m_document->GetImage().GetWidth();
        const int height = m_document->GetImage().GetHeight();

        m_histogram.SetImage(m_document->GetImage().GetPixelData(), width, height);
        m_histogram.queue_draw();
    }

    void Window::SetOpenDocumentCallback(std::function<void(const std::filesystem::path&)> cb)
    {
        m_openDocumentCallback = std::move(cb);
    }

    void Window::SetDocument(Document* document) { m_document = document; }

    void Window::SetupDragAndDrop()
    {
        const GType uStringType = Glib::Value<Glib::ustring>::value_type();
        auto drop_target = Gtk::DropTarget::create(G_TYPE_INVALID, Gdk::DragAction::COPY);

        // Support multiple formats: file lists, single files, and strings
        drop_target->set_gtypes({GDK_TYPE_FILE_LIST, G_TYPE_FILE, uStringType});

        drop_target->signal_drop().connect(
                sigc::mem_fun(*this, &Window::OnDrop), false
                );

        add_controller(drop_target);
    }

    bool Window::OnDrop(const Glib::ValueBase& value, double, double) const
    {
        if(!m_openDocumentCallback)
            return false;

        const GValue* g_value = value.gobj();
        std::string path_found;

        if(G_VALUE_HOLDS(g_value, GDK_TYPE_FILE_LIST))
        {
            if(auto* file_list = static_cast<GdkFileList*>(g_value_get_boxed(g_value)))
            {
                GSList* files = gdk_file_list_get_files(file_list);
                if(files && files->data)
                    if(gchar* path = g_file_get_path(static_cast<GFile*>(files->data)))
                    {
                        path_found = path;
                        g_free(path);
                    }
            }
        } else if(G_VALUE_HOLDS(g_value, G_TYPE_FILE))
            if(auto* g_file_obj = static_cast<GFile*>(g_value_get_object(g_value)))
                if(gchar* path = g_file_get_path(g_file_obj))
                {
                    path_found = path;
                    g_free(path);
                }


        return (!path_found.empty()) ? HandleFilePath(path_found) : false;
    }

    bool Window::HandleFilePath(const std::string& path_str) const
    {
        if(path_str.empty())
            return false;

        if(m_openDocumentCallback)
        {
            m_openDocumentCallback(std::filesystem::path{path_str});
            return true;
        }

        return false;
    }

    void Window::AddActionsToGroupAction()
    {
        m_actionGroup->add_action("grayscale", [this] {
           ExecuteFilter([](Image& img) {
               Processor::ToGrayScale(img);
           });
       });

        m_actionGroup->add_action("blur", [this] {
            auto dialog = Gtk::make_managed<Gtk::Dialog>("Set Blur Radius", *this, true);
            dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
            dialog->add_button("_Apply", Gtk::ResponseType::OK);

            auto adjustment = Gtk::Adjustment::create(5.0, 0.1, 50.0, 0.1, 1.0);
            auto spin = Gtk::make_managed<Gtk::SpinButton>(adjustment);
            dialog->get_content_area()->append(*spin);

            dialog->signal_response().connect([dialog, spin, this](int response_id) {
                if(response_id == static_cast<int>(Gtk::ResponseType::OK))
                {
                    auto radius = static_cast<float>(spin->get_value());
                    ExecuteFilter([radius](Image& img) {
                        Processor::Blur(img, radius);
                    });
                }
                dialog->set_visible(false);
            });
            dialog->show();
        });

        m_actionGroup->add_action("sharpen", [this] {
            ExecuteFilter([this](Image& img) {
                Processor::Sharpen(img, m_originalPixelsBackup);
            });
        });

        m_actionGroup->add_action("emboss", [this] {
            ExecuteFilter([this](Image& img) {
                Processor::Emboss(img, m_originalPixelsBackup);
            });
        });

        m_actionGroup->add_action("invert", [this] {
            ExecuteFilter([](Image& img) {
                Processor::Invert(img);
            });
        });

        m_actionGroup->add_action("edge_detect", [this] {
            ExecuteFilter([this](Image& img) {
                Processor::EdgeDetect(img, m_originalPixelsBackup);
            });

        });
    }
}