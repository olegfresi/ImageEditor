/*
 * Project: ImageEditor
 * File: ToneCurve.hpp
 * Author: olegfresi
 * Created: 03/03/26 16:56
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
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturedrag.h>
#include <sigc++/sigc++.h>
#include <vector>

namespace Editor::Widget
{
    struct ControlPoint
    {
        double x, y;
        bool operator <(const ControlPoint& other) const { return x < other.x; }
    };

    class ToneCurveWidget : public Gtk::DrawingArea
    {
    public:

        /**
        * Construct and initialize the ToneCurve widget.
        *
        * Sets up:
        * - widget dimensions (256x256)
        * - default curve with linear endpoints (0,0) and (1,1)
        * - curve cache with 256 entries for fast LUT generation
        * - draw function callback
        * - mouse click and drag gesture handlers
        */
        ToneCurveWidget();

        /**
        * Generate 256 entry LUT for image processing.
        *
        * Values are converted from normalized [0,1] to integer range [0,255].
        * @return vector representing integer range
        */
        std::vector<uint8_t> GetLUT() const;

        /**
        * Signal emitted when curve changes.
        * @return a reference to a callback signal
        */
        sigc::signal<void()>& SignalCurveChanged();

        /**
        * Called when mouse drag ends.
        * @return a reference to a callback signal
        */
        sigc::signal<void()>& SignalDragFinished() { return m_signalDragFinished; }

        /**
        * Find a control point near the given coordinates.
        *
        * Searches for an existing control point within a tolerance distance
        * from the specified position.
        * @param x: x-coordinate in normalized [0,1] space
        * @param y: y-coordinate in normalized [0,1] space
        * @return index of the found control point, or -1 if none found
        */
        int FindPoint(double x,double y) const;

        /**
        * Set the histogram data to display in the background.
        *
        * The histogram is used for visualization to show the distribution
        * of pixel values in the image being edited.
        * @param hist: array of 256 float values representing normalized histogram data
        */
        void SetHistogram(const std::array<float,256>& hist);

    private:
        std::vector<ControlPoint> m_points{};
        std::vector<float> m_curveCache{};
        std::array<float,256> m_histogram{};

        int m_selectedIdx = -1;
        bool m_tempPointActive = false;

        double m_dragStartX = 0.0;
        double m_dragStartY = 0.0;

        sigc::signal<void()> m_signalCurveChanged;
        sigc::signal<void()> m_signalDragFinished;

        /**
        * Precompute the tone curve lookup table (LUT).
        *
        * The LUT has 256 entries and maps input intensity [0,255]
        * to normalized output intensity [0,1].
        *
        * This allows fast pixel processing later without recomputing
        * the curve interpolation for each pixel.
        */
        void UpdateCurveCache();

        /**
        * Draw the widget.
        *
        * Renders:
        * - background
        * - grid
        * - tone curve
        * - control points
        */
        void Draw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h) const;

        /**
        * Mouse press handler.
        *
        * Allows selecting an existing control point or creating a new one.
        */
        void OnPressed(double x, double y);

        /**
        * Mouse drag update.
        *
        * Moves the selected control point.
        */
        void OnDragUpdate(double dx, double dy);

        /**
        * Catmull-Rom cubic spline interpolation.
        *
        * Computes a smooth curve through control points using Catmull-Rom spline
        * basis functions. This provides smooth, natural-looking tone curves.
        * @param t: interpolation parameter in range [0,1]
        * @param p0: first control point value
        * @param p1: second control point value
        * @param p2: third control point value
        * @param p3: fourth control point value
        * @return interpolated value at parameter t
        */
        static float CatmullRom(double t, double p0, double p1, double p2, double p3);
    };
}