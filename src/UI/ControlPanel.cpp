#include "../../include/UI/ControlPanel.hpp"
#include <gtkmm/adjustment.h>
#include <gtkmm/label.h>
#include <gtkmm/separator.h>
#include <gtkmm/gestureclick.h>


namespace Editor::UI
{
    ControlPanel::ControlPanel() : Box{Gtk::Orientation::VERTICAL, 10}
    {
        set_margin(4);

        AddSlider("exposure", "Exposure (EV)", -4.0f, 4.0f, 0.1f, 0.0f);

        AddSlider("brightness", "Brightness", -100.0f, 100.0f, 1.0f, 0.0f);

        AddSlider("contrast", "Contrast", -100.0f, 100.0f, 1.0f, 0.0f);

        append(*Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL));

        AddSlider("highlights", "Highlights", -100.0f, 100.0f, 1.0f, 0.0f);
        AddSlider("shadows", "Shadows", -100.0f, 100.0f, 1.0f, 0.0f);
        AddSlider("black_point", "Black Point", -50.0f, 50.0f, 1.0f, 0.0f);

        append(*Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL));

        AddSlider("texture", "Texture", -100.0f, 100.0f, 1.0f, 0.0f);
        AddSlider("clarity", "Clarity", -100.0f, 100.0f, 1.0f, 0.0f);

        append(*Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL));

        AddSlider("temperature", "Temperature (K)", -2000.0f, 2000.0f, 50.0f, 0.0f);
    }

    void ControlPanel::AddSlider(const std::string& id,
                             const std::string& label_text,
                             float min, float max,
                             float step, float default_val)
    {
        auto row = Gtk::make_managed<Box>(Gtk::Orientation::VERTICAL, 2);
        row->set_hexpand(true);

        auto label = Gtk::make_managed<Gtk::Label>(label_text);
        label->set_halign(Gtk::Align::START);

        label->set_wrap(true);
        label->set_wrap_mode(Pango::WrapMode::WORD_CHAR);
        label->set_max_width_chars(14);

        row->append(*label);

        auto adjustment = Gtk::Adjustment::create(default_val, min, max, step, step * 10, 0);

        auto slider = Gtk::make_managed<Gtk::Scale>(adjustment, Gtk::Orientation::HORIZONTAL);

        slider->set_hexpand(true);
        slider->set_size_request(-1, -1);
        slider->set_draw_value(false);

        slider->signal_value_changed().connect([this, id, slider]() {
            auto val = static_cast<float>(slider->get_value());
            m_signalParamChanged.emit(id, val);
        });

        auto gesture = Gtk::GestureClick::create();
        gesture->signal_released().connect([this, id, slider](int, double, double) {
            m_signalParamCommit.emit(id, (float)slider->get_value());
        });
        slider->add_controller(gesture);

        row->append(*slider);
        append(*row);

        m_sliders[id] = slider;
    }

    void ControlPanel::ResetAllSliders()
    {
        for(auto& [id, slider] : m_sliders)
            slider->set_value(0.0);
    }
}