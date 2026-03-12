/*
 * Project: ImageEditor
 * File: Histogram.hpp
 * Author: olegfresi
 * Created: 03/03/26 16:20
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
#include <array>
#include <gtkmm/drawingarea.h>
#include "../Core/Pixel.hpp"


namespace Editor::Widget
{
    class HistogramWidget : public Gtk::DrawingArea
    {
    public:

        /**
        * Construct and initialize the Histogram widget.
        *
        * Sets up:
        * - widget drawing area for histogram visualization
        * - draw function callback
        * - histogram data arrays (R, G, B channels with 256 entries each)
        */
        HistogramWidget();

        /**
        * Set the image data to compute the histogram from.
        *
        * Computes histogram values for all RGB channels and triggers
        * a redraw of the widget to display the updated histogram.
        * @param pixels: flat vector of Pixel objects
        * @param width: width of image
        * @param height: height of image
        */
        void SetImage(const std::vector<Pixel>& pixels, int width, int height);

        /**
        * Get the histogram data for the Red channel.
        * @return const reference to array of 256 uint32_t values
        */
        const std::array<uint32_t,256>& GetRed() const   { return m_r; }

        /**
        * Get the histogram data for the Green channel.
        * @return const reference to array of 256 uint32_t values
        */
        const std::array<uint32_t,256>& GetGreen() const { return m_g; }

        /**
        * Get the histogram data for the Blue channel.
        * @return const reference to array of 256 uint32_t values
        */
        const std::array<uint32_t,256>& GetBlue() const  { return m_b; }

        /**
        * Compute and get the luminance histogram from RGB channels.
        * @return array of 256 float values representing luminance distribution
        */
        std::array<float,256> GetLuminanceHistogram() const;

        /**
        * Get the signal emitted when histogram data is updated.
        *
        * This signal is emitted whenever new histogram data is computed
        * from a newly set image, allowing other widgets to react to changes.
        * @return reference to the histogram updated signal
        */
        sigc::signal<void()>& SignalHistogramUpdated() { return m_signalHistogramUpdated; }

        /**
        * Update histogram from a pixel buffer with custom stride.
        *
        * Computes histogram values for all RGB channels from the provided
        * pixel buffer, using the specified stride for row alignment.
        * This is useful for processing buffers with padding or non-contiguous memory layouts.
        * Emits the histogram updated signal after computation.
        *
        * @param pixels: flat vector of Pixel objects
        * @param stride: number of pixels per row (including padding if any)
        */
        void UpdateFromBuffer(const std::vector<Pixel>& pixels, size_t stride);

    private:

        /**
        * Draw the histogram using Cairo context
        * Each channel is drawn as a semi-transparent overlay
        */
        void DrawHistogram(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) const;

        int m_width = 0;
        int m_height = 0;

        std::vector<Pixel> m_pixels{};
        sigc::signal<void()> m_signalHistogramUpdated;

        std::array<uint32_t, 256> m_r{};
        std::array<uint32_t, 256> m_g{};
        std::array<uint32_t, 256> m_b{};
    };
}