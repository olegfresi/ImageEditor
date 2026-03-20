/*
 * Project: ImageEditor
 * File: ControlPanel.hpp
 * Author: olegfresi
 * Created: 17/03/26 21:27
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
#include <gtkmm/box.h>
#include <gtkmm/scale.h>


namespace Editor::UI
{
    class ControlPanel : public Gtk::Box
    {
    public:
        /**
        * Construct the vertical control panel containing preset sliders.
        *
        * Adds separators and slider rows for exposure, brightness, contrast, local tone,
        * texture/clarity, and temperature adjustments, wiring each to a shared signal.
        */
        ControlPanel();
        ~ControlPanel() override = default;

        void ResetAllSliders();

        /**
        * Expose the signal emitted when any slider value changes.
        *
        * @return Signal that carries the slider identifier and current value.
        */
        sigc::signal<void(const std::string&, float)> ParamChanged() { return m_signalParamChanged;}

        sigc::signal<void(const std::string&, float)>  ParamCommit() { return m_signalParamCommit; }

    private:
        /**
        * Helper that creates a labeled slider row and registers its value change callback.
        *
        * @param id Stable identifier sent on value change.
        * @param label UI label text describing the control.
        * @param min Minimum slider value.
        * @param max Maximum slider value.
        * @param step Slider step granularity.
        * @param default_val Initial slider position.
        */
        void AddSlider(const std::string& id, const std::string& label, float min, float max, float step, float default_val);

        sigc::signal<void(const std::string&, float)> m_signalParamChanged;
        sigc::signal<void(const std::string&, float)> m_signalParamCommit;

        std::map<std::string, Gtk::Scale*> m_sliders;
    };
}
