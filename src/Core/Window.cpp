#include <giomm/menu.h>
#include <gtkmm/popovermenubar.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/stringlist.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/expander.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/cssprovider.h>
#include <gtkmm/droptarget.h>
#include <gdkmm/memorytexture.h>
#include <gdkmm/enums.h>
#include <glibmm/main.h>
#include <gdk/gdk.h>
#include <gtkmm/eventcontrollerkey.h>
#include <gtkmm/overlay.h>
#include <gtkmm/picture.h>
#include <gio/gio.h>
#include <glib.h>
#include <ranges>
#include "../../include/Core/Window.hpp"
#include "../../include/Core/FileManager.hpp"
#include "../../include/Core/Utils.hpp"
#include "../../include/Command/GrayScaleCommand.hpp"
#include "../../include/Command/EmbossCommand.hpp"
#include "../../include/Command/EdgeDetectCommand.hpp"
#include "../../include/Command/SharpenCommand.hpp"
#include "../../include/Command/BlurCommand.hpp"
#include "../../include/Command/ColorInversionCommand.hpp"
#include "../../include/Command/SepiaCommand.hpp"
#include "../../include/Command/FlipCommand.hpp"
#include "../../include/Command/RotateCommand.hpp"
#include "../../include/Command/ToneCurveChangeCommand.hpp"
#include "../../include/Command/ExposureCommand.hpp"
#include "../../include/Command/TextureCommand.hpp"
#include "../../include/Command/BrightnessCommand.hpp"
#include "../../include/Command/ClarityCommand.hpp"
#include "../../include/Command/ContrastCommand.hpp"
#include "../../include/Command/ShadowsCommand.hpp"
#include "../../include/Command/HighlightsCommand.hpp"
#include "../../include/Command/BlackPointCommand.hpp"
#include "../../include/Command/TemperatureCommand.hpp"


namespace Editor
{
    Window::Window(int width, int height, std::string title)
        : m_width{width}, m_height{height}, m_title{std::move(title)}
    {
        // =======================
        // BASIC WINDOW SETUP
        // =======================
        set_default_size(m_width, m_height);
        set_title(m_title);
        set_hide_on_close(true);
        set_resizable(true);

        // =======================
        // ACTION GROUP
        // =======================
        m_actionGroup = Gio::SimpleActionGroup::create();
        AddAction("import", &Window::OnImport);
        AddAction("about", &Window::OnAbout);
        AddActionsToGroupAction();
        insert_action_group("win", m_actionGroup);

        // =======================
        // MENU BAR
        // =======================
        auto menu = Gio::Menu::create();
        auto submenuFile = Gio::Menu::create();
        submenuFile->append("Import", "win.import");
        submenuFile->append("Save", "win.save");
        menu->append_submenu("File", submenuFile);

        auto submenuEdit = Gio::Menu::create();
        submenuEdit->append("Rotate", "win.rotate");
        submenuEdit->append("Flip", "win.flip");
        submenuEdit->append("Undo", "win.undo");
        submenuEdit->append("Redo", "win.redo");
        menu->append_submenu("Edit", submenuEdit);

        auto submenuFilter = Gio::Menu::create();
        submenuFilter->append("GrayScale", "win.grayscale");
        submenuFilter->append("Blur", "win.blur");
        submenuFilter->append("Sharpen", "win.sharpen");
        submenuFilter->append("Emboss", "win.emboss");
        submenuFilter->append("Color Invert", "win.color_invert");
        submenuFilter->append("Sepia", "win.sepia");
        submenuFilter->append("Edge Detect", "win.edge_detect");
        menu->append_submenu("Filter", submenuFilter);

        auto submenuAbout = Gio::Menu::create();
        submenuAbout->append("Info App", "win.about");
        menu->append_submenu("About", submenuAbout);

        auto menubar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu);

        // =======================
        // LEFT SIDE : IMAGE VIEWPORT
        // =======================
        m_picture.set_expand(true);
        m_picture.set_keep_aspect_ratio(true);
        m_picture.set_can_shrink(true);

        auto imageOverlay = Gtk::make_managed<Gtk::Overlay>();
        imageOverlay->set_child(m_picture);

        m_maskPicture.set_keep_aspect_ratio(true);
        m_maskPicture.set_opacity(0.8);
        m_maskPicture.set_can_target(false);
        m_maskPicture.set_can_shrink(true);
        imageOverlay->add_overlay(m_maskPicture);

        imageOverlay->set_hexpand(true);
        imageOverlay->set_vexpand(true);

        m_imageContainer.set_orientation(Gtk::Orientation::VERTICAL);
        m_imageContainer.append(*imageOverlay);
        m_imageContainer.append(m_infoLabel);

        m_imageContainer.set_hexpand(true);
        m_imageContainer.set_vexpand(true);

        m_scrolled.set_child(m_imageContainer);
        m_scrolled.set_expand(true);
        m_scrolled.set_min_content_width(300);

        // =======================
        // RIGHT SIDE : SIDEBAR
        // =======================

        auto sidePanel = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        sidePanel->set_margin(6);


        // =======================
        // HISTOGRAM
        // =======================

        auto histogramBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);

        auto channels = Gtk::StringList::create({
                "RGB",
                "Luminance",
                "Red",
                "Green",
                "Blue"
        });

        auto dropdown = Gtk::make_managed<Gtk::DropDown>(channels);
        dropdown->set_selected(0);

        histogramBox->append(*dropdown);


        // stack contenente gli histogram
        m_histogramStack.set_hexpand(true);

        m_histogramStack.add(m_histogramRGB, "rgb");
        m_histogramStack.add(m_histogramLum, "lum");
        m_histogramStack.add(m_histogramR, "r");
        m_histogramStack.add(m_histogramG, "g");
        m_histogramStack.add(m_histogramB, "b");
        m_histogramRGB.SetChannelMode(Editor::Widget::HistogramWidget::ChannelMode::Rgb);
        m_histogramLum.SetChannelMode(Editor::Widget::HistogramWidget::ChannelMode::Luminance);
        m_histogramR.SetChannelMode(Editor::Widget::HistogramWidget::ChannelMode::Red);
        m_histogramG.SetChannelMode(Editor::Widget::HistogramWidget::ChannelMode::Green);
        m_histogramB.SetChannelMode(Editor::Widget::HistogramWidget::ChannelMode::Blue);

        histogramBox->append(m_histogramStack);

        // dropdown -> stack binding
        dropdown->property_selected().signal_changed().connect(
                [this, dropdown]() {
                    static const char* pages[] = {"rgb", "lum", "r", "g", "b"};
                    m_histogramStack.set_visible_child(pages[dropdown->get_selected()]);

                    if(!m_document)
                        return;

                    if(auto* visibleWidget = dynamic_cast<Editor::Widget::HistogramWidget*>(
                        m_histogramStack.get_visible_child()))
                    {
                        if(m_previewRenderer.GetPixels().empty())
                            visibleWidget->SetImage(m_document->GetImage().GetPixelData(), 32);
                        else
                            visibleWidget->SetImage(m_previewRenderer.GetPixels(), 32);
                    }
                });


        auto histogramPanel = Gtk::make_managed<Gtk::Expander>("Histogram");
        histogramPanel->set_child(*histogramBox);
        histogramPanel->set_expanded(true);

        sidePanel->append(*histogramPanel);


        // =======================
        // TONE CURVE
        // =======================

        m_toneCurve.set_size_request(-1, 180);
        m_toneCurve.set_hexpand(true);

        auto curvePanel = Gtk::make_managed<Gtk::Expander>("Tone Curve");
        curvePanel->set_child(m_toneCurve);
        curvePanel->set_expanded(true);

        sidePanel->append(*curvePanel);


        // =======================
        // BASIC CONTROLS
        // =======================

        auto controlsPanel = Gtk::make_managed<Gtk::Expander>("Basic");
        controlsPanel->set_child(m_controlPanel);
        controlsPanel->set_expanded(true);

        sidePanel->append(*controlsPanel);

        // =======================
        // SCROLL
        // =======================

        auto sideScroll = Gtk::make_managed<Gtk::ScrolledWindow>();
        sideScroll->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
        sideScroll->set_child(*sidePanel);

        constexpr int sidebarWidth = 220;

        sideScroll->set_min_content_width(sidebarWidth);
        sideScroll->set_max_content_width(sidebarWidth);
        // =======================
        // MAIN SPLITTER
        // =======================
        m_mainSplitter.set_orientation(Gtk::Orientation::HORIZONTAL);
        m_mainSplitter.set_start_child(m_scrolled);
        m_mainSplitter.set_end_child(*sideScroll);
        m_mainSplitter.set_position(m_width - sidebarWidth);

        m_mainSplitter.set_resize_start_child(true);
        m_mainSplitter.set_resize_end_child(false);
        m_mainSplitter.set_shrink_end_child(true);

        // =======================
        // FINAL LAYOUT
        // =======================
        m_vbox.set_orientation(Gtk::Orientation::VERTICAL);
        m_vbox.append(*menubar);
        m_vbox.append(m_mainSplitter);

        auto key_controller = Gtk::EventControllerKey::create();

        key_controller->signal_key_pressed().connect([this](guint keyValue, guint, Gdk::ModifierType) {
            if(keyValue == GDK_KEY_m || keyValue == GDK_KEY_M)
            {
                bool is_visible = m_maskPicture.get_visible();
                m_maskPicture.set_visible(!is_visible);
                return true;
            }
            return false;
        }, false);

        add_controller(key_controller);

        m_controlPanel.ParamCommit().connect(sigc::mem_fun(*this, &Window::OnParameterCommit));
        m_controlPanel.ParamChanged().connect(sigc::mem_fun(*this, &Window::OnParameterChanged));

        m_paramSetters = {
                {"exposure", [this](float v) { m_params.exposure = v; }},
                {"brightness", [this](float v) { m_params.brightness = v; }},
                {"contrast", [this](float v) { m_params.contrast = v; }},
                {"highlights", [this](float v) { m_params.highlights = v; }},
                {"shadows", [this](float v) { m_params.shadows = v; }},
                {"black_point", [this](float v) { m_params.black_point = v; }},
                {"temperature", [this](float v) { m_params.temperature = v; }},
                {"clarity", [this](float v) { m_params.clarity = v; }},
                {"texture", [this](float v) { m_params.texture = v; }}
        };

        m_paramCommands = {
                {"exposure", [](Document* doc, float v) { return std::make_unique<Command::ExposureCommand>(doc, v); }},
                {"brightness",
                 [](Document* doc, float v) { return std::make_unique<Command::BrightnessCommand>(doc, v); }},
                {"contrast", [](Document* doc, float v) { return std::make_unique<Command::ContrastCommand>(doc, v); }},
                {"highlights",
                 [](Document* doc, float v) { return std::make_unique<Command::HighlightsCommand>(doc, v); }},
                {"shadows", [](Document* doc, float v) { return std::make_unique<Command::ShadowsCommand>(doc, v); }},
                {"black_point",
                 [](Document* doc, float v) { return std::make_unique<Command::BlackPointCommand>(doc, v); }},
                {"temperature",
                 [](Document* doc, float v) { return std::make_unique<Command::TemperatureCommand>(doc, v); }},
                {"clarity", [](Document* doc, float v) { return std::make_unique<Command::ClarityCommand>(doc, v); }},
                {"texture", [](Document* doc, float v) { return std::make_unique<Command::TextureCommand>(doc, v); }}
        };

        set_child(m_vbox);

        // =======================
        // SIGNALS
        // =======================

        // --- SignalCurveChanged ---
        m_toneCurve.SignalCurveChanged().connect([this]() {
            if(!m_document)
                return;

            if(!m_dragInProgress)
            {
                m_dragInProgress = true;
                auto& image = m_document->GetImage();
                m_startPixels.assign(image.GetPixelData().begin(), image.GetPixelData().end());
                m_startState = m_toneCurve.GetState();
                m_previewRenderer.SetSource(m_startPixels, image.GetWidth(), image.GetHeight());
            }

            if(!m_previewRenderer.ShouldUpdate())
                return;

            m_previewRenderer.UpdateLut(m_toneCurve.GetLUT());

            UpdateImageViewPreview(m_previewRenderer.GetPixels(),
                                   m_previewRenderer.GetWidth(),
                                   m_previewRenderer.GetHeight());

            if(auto* visibleWidget = dynamic_cast<Editor::Widget::HistogramWidget*>(m_histogramStack.
                get_visible_child()))
                visibleWidget->SetImage(m_previewRenderer.GetPixels(), 32);
        });

        // --- SignalDragFinished ---
        m_toneCurve.SignalDragFinished().connect([this]() {
            if(!m_document || !m_dragInProgress)
                return;

            auto endState = m_toneCurve.GetState();
            const auto& lut = m_toneCurve.GetLUT();

            auto finalBuffer = std::make_shared<std::vector<Pixel>>(m_startPixels);

            std::thread([this, finalBuffer, lut, endState]() {
                for(size_t i = 0; i < finalBuffer->size(); i++)
                {
                    const auto& src = (*finalBuffer)[i];
                    (*finalBuffer)[i].SetPixel(lut[src.GetR()], lut[src.GetG()],
                                               lut[src.GetB()], src.GetA());
                }

                Glib::signal_idle().connect_once([this, finalBuffer, endState]() {
                    auto cmd = std::make_unique<Command::ToneCurveCommand>(
                            m_document,
                            m_toneCurve,
                            m_startState,
                            endState,
                            m_startPixels
                            );

                    m_document->ExecuteCommand(std::move(cmd));

                    m_previewRenderer.SetSource(m_document->GetImage().GetPixelData(),
                                                m_document->GetImage().GetWidth(),
                                                m_document->GetImage().GetHeight());

                    UpdateImageView();
                    UpdateVisibleHistogram();

                    m_dragInProgress = false;
                });
            }).detach();
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

                if(openDocumentCallback)
                    openDocumentCallback(path);
            } catch(const Glib::Error& ex)
            {
                auto err = Gtk::AlertDialog::create("Load error");
                err->set_detail(ex.what());
                err->show(*this);
            }
        });
    }

    void Window::LoadDocument(Document* doc)
    {
        if(!doc)
        {
            m_document = nullptr;
            m_picture.set_paintable(nullptr);
            return;
        }

        m_document = doc;

        m_document->SetOnImageChangedCallback([this] {
            UpdateImageView();
        });

        m_document->SetOnCommandStackChangedCallback([this] {
            UpdateUndoRedoState();
        });

        m_params = Utils::AdjustParams{};
        m_controlPanel.ResetAllSliders();

        m_toneCurve.Reset();

        m_dragInProgress = false;
        m_startPixels.clear();

        UpdateImageView();
        UpdateUndoRedoState();
        UpdateAllActionsEnabled();

        m_previewRenderer.SetSource(
            m_document->GetImage().GetPixelData(),
            m_document->GetImage().GetWidth(),
            m_document->GetImage().GetHeight()
        );

        auto initialLum = m_histogramRGB.GetLuminanceHistogram();
        m_toneCurve.SetHistogram(initialLum);

        m_histogramRGB.SetImage(m_document->GetImage().GetPixelData(), 32);
        m_histogramLum.SetImage(m_document->GetImage().GetPixelData(), 32);
        m_histogramR.SetImage(m_document->GetImage().GetPixelData(), 32);
        m_histogramG.SetImage(m_document->GetImage().GetPixelData(), 32);
        m_histogramB.SetImage(m_document->GetImage().GetPixelData(), 32);

        m_infoLabel.set_markup("<b>File:</b> " +
                               m_document->GetFilePath().filename().string());
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
                } catch(const Glib::Error& ex)
                {
                    auto error_dialog = Gtk::AlertDialog::create("Error saving document");
                    error_dialog->set_detail(ex.what());
                    error_dialog->show(*this);
                }
            });
        } catch(const std::exception& ex)
        {
            auto error_dialog = Gtk::AlertDialog::create("Error saving document");
            error_dialog->set_detail(ex.what());
            error_dialog->show(*this);
        }
    }

    void Window::OnAbout()
    {
        auto about_dialog = Gtk::AlertDialog::create("Image Editor v1.0");
        about_dialog->set_detail("Created with gtkmm4 and C++\n2026 Edition");
        about_dialog->show(*this);
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

    void Window::UpdateImageView()
    {
        if(!m_document)
            return;

        auto& image = m_document->GetImage();
        std::span<Pixel> pixels = image.GetPixelData();

        auto bytes = Glib::Bytes::create(pixels.data(), pixels.size() * sizeof(Pixel));
        auto texture = Gdk::MemoryTexture::create(
                image.GetWidth(), image.GetHeight(),
                Gdk::MemoryTexture::Format::R8G8B8A8,
                bytes, image.GetWidth() * 4);
        m_picture.set_paintable(texture);

        if(auto* visibleWidget = dynamic_cast<Editor::Widget::HistogramWidget*>(m_histogramStack.get_visible_child()))
            visibleWidget->SetImage(pixels, 32);

        UpdateVisibleHistogram();
        auto mask = m_analyzer.GenerateClippingMask(pixels, image.GetWidth(), image.GetHeight());
        m_maskPicture.set_pixbuf(mask);
    }

    void Window::UpdateImageViewPreview(const std::span<const Pixel>& previewPixels, int width, int height)
    {
        auto bytes = Glib::Bytes::create(previewPixels.data(),
                                         previewPixels.size() * sizeof(Pixel));
        auto texture = Gdk::MemoryTexture::create(
                width, height,
                Gdk::MemoryTexture::Format::R8G8B8A8,
                bytes, width * 4);

        m_picture.set_paintable(texture);
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

    void Window::OnParameterChanged(const std::string& id, float value)
    {
        if(!m_document)
            return;

        if(m_paramSetters.contains(id))
            m_paramSetters[id](value);

        if(auto lut = PreviewRenderer::BuildLut(id, value))
            m_previewRenderer.UpdateLut(*lut);
        else
        {
            m_previewRenderer.ResetToOriginal();

            std::vector pixels(m_previewRenderer.GetOriginalPixels().begin(), m_previewRenderer.GetOriginalPixels().end());
            Image temp(m_previewRenderer.GetWidth(), m_previewRenderer.GetHeight(), 4, std::move(pixels));
            PreviewRenderer::ApplySingleParameter(temp, id, value);
            m_previewRenderer.SetPreview(temp.GetPixelData());
        }

        UpdateImageViewPreview(m_previewRenderer.GetPixels(), m_previewRenderer.GetWidth(), m_previewRenderer.GetHeight());

        if(auto* visibleWidget = dynamic_cast<Editor::Widget::HistogramWidget*>(m_histogramStack.get_visible_child()))
            visibleWidget->SetImage(m_previewRenderer.GetPixels(), 32);
    }

    void Window::OnParameterCommit(const std::string& id, float value)
    {
        if(!m_document)
            return;

        if(m_paramCommands.contains(id))
        {
            auto cmd = m_paramCommands[id](m_document, value);
            m_document->ExecuteCommand(std::move(cmd));

            auto& img = m_document->GetImage();
            m_previewRenderer.SetSource(img.GetPixelData(), img.GetWidth(), img.GetHeight());

            m_previewRenderer.ResetToOriginal();
            UpdateImageViewPreview(
                    m_previewRenderer.GetPixels(),
                    m_previewRenderer.GetWidth(),
                    m_previewRenderer.GetHeight()
                    );
        }
    }

    void Window::OnCurveDragFinished()
    {
        m_histogramRGB.SetImage(m_document->GetImage().GetPixelData());
        m_histogramRGB.queue_draw();
    }

    void Window::UpdateVisibleHistogram()
    {
        if(auto* visibleWidget = dynamic_cast<Editor::Widget::HistogramWidget*>(m_histogramStack.get_visible_child()))
        {
            auto& image = m_document->GetImage();
            visibleWidget->SetImage(image.GetPixelData(), 32);
        }
    }

    void Window::SetOpenDocumentCallback(std::function<void(const std::filesystem::path&)> cb)
    {
        openDocumentCallback = std::move(cb);
    }

    void Window::SetDocument(Document* document)
    {
        m_document = document;
        m_document->SetOnImageChangedCallback([this] {
            UpdateImageView();
        });

        m_document->SetOnCommandStackChangedCallback([this] {
            UpdateUndoRedoState();
        });

        UpdateUndoRedoState();
        m_previewRenderer.SetSource(m_document->GetImage().GetPixelData(),
                                    m_document->GetImage().GetWidth(),
                                    m_document->GetImage().GetHeight());
    }

    void Window::UpdateUndoRedoState() const
    {
        if(!m_document)
            return;

        auto undoIt = m_actions.find("undo");
        if(undoIt != m_actions.end() && undoIt->second)
            undoIt->second->set_enabled(m_document->CanUndo());

        auto redoIt = m_actions.find("redo");
        if(redoIt != m_actions.end() && redoIt->second)
            redoIt->second->set_enabled(m_document->CanRedo());
    }

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
        if(!openDocumentCallback)
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

        if(openDocumentCallback)
        {
            openDocumentCallback(std::filesystem::path{path_str});
            return true;
        }

        return false;
    }

    void Window::UpdateActionEnabled(const std::string& actionName, const std::function<bool()>& condition)
    {
        auto it = m_actions.find(actionName);
        if(it != m_actions.end() && it->second)
            it->second->set_enabled(condition());
    }

    void Window::UpdateAllActionsEnabled()
    {
        bool hasDoc = (m_document != nullptr);

        for(const auto& [name, action] : m_actions)
        {
            if(name == "import" || name == "about")
                action->set_enabled(true);
            else if(name == "undo")
                action->set_enabled(hasDoc && m_document->CanUndo());
            else if(name == "redo")
                action->set_enabled(hasDoc && m_document->CanRedo());
            else
                action->set_enabled(hasDoc);
        }
    }

    void Window::AddSimpleAction(const std::string& name, const sigc::slot<void(const Glib::VariantBase&)>& callback)
    {
        auto action = Gio::SimpleAction::create(name);
        action->signal_activate().connect(callback);
        m_actionGroup->add_action(action);
        m_actions[name] = action;
    }

    void Window::AddActionsToGroupAction()
    {
        AddSimpleAction("undo", [this](const auto&) {
            if(m_document)
                m_document->Undo();
        });

        AddSimpleAction("redo", [this](const auto&) {
            if(m_document)
                m_document->Redo();
        });

        AddSimpleAction("rotate", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::RotateCommand>(m_document, 90);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("flip_horizontal", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::FlipCommand>(m_document, Command::FlipDirection::Horizontal);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("flip_vertical", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::FlipCommand>(m_document, Command::FlipDirection::Vertical);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("grayscale", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::GrayScaleCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("blur", [this](const auto&) {
            auto dialog = Gtk::make_managed<Gtk::Dialog>("Set Blur Radius", *this, true);
            dialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
            dialog->add_button("_Apply", Gtk::ResponseType::OK);

            auto adjustment = Gtk::Adjustment::create(5.0, 0.1, 50.0, 0.1, 1.0);
            auto spin = Gtk::make_managed<Gtk::SpinButton>(adjustment);
            dialog->get_content_area()->append(*spin);

            dialog->signal_response().connect([dialog, spin, this](int response_id) {
                if(response_id == static_cast<int>(Gtk::ResponseType::OK))
                {
                    if(!m_document)
                        return;

                    auto radius = static_cast<float>(spin->get_value());
                    auto cmd = std::make_unique<Command::BlurCommand>(m_document, radius);
                    m_document->ExecuteCommand(std::move(cmd));
                }

                dialog->set_visible(false);
            });

            dialog->show();
        });

        AddSimpleAction("sharpen", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::SharpenCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("emboss", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::EmbossCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("color_invert", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::ColorInversionCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("edge_detect", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::EdgeDetectCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("sepia", [this](const auto&) {
            if(!m_document)
                return;
            auto cmd = std::make_unique<Command::SepiaCommand>(m_document);
            m_document->ExecuteCommand(std::move(cmd));
        });

        AddSimpleAction("save", [this](const auto&) {
            OnSave();
        });

        UpdateAllActionsEnabled();
    }
}