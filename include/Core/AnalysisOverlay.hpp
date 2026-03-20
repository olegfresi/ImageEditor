/*
 * Project: ImageEditor
 * File: AnalysisOverlay.hpp
 * Author: olegfresi
 * Created: 17/03/26 18:19
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
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <span>
#include "Pixel.hpp"


namespace Editor::Analysis
{

    class AnalysisOverlay
    {
    public:
        AnalysisOverlay() = default;

        // Generates a mask where clipped highlights are Red and clipped shadows are Blue
        Glib::RefPtr<Gdk::Pixbuf> GenerateClippingMask(std::span<const Pixel> pixels, int width, int height)
        {
            PrepareBuffer(width, height);

            uint8_t* dest = m_buffer->get_pixels();
            int stride = m_buffer->get_rowstride();
            const auto* src = reinterpret_cast<const uint8_t*>(pixels.data());

            for(int y = 0; y < height; ++y)
            {
                for(int x = 0; x < width; ++x)
                {
                    int s_idx = (y * width + x) * 4;
                    int d_idx = y * stride + x * 4;

                    // Calculate luminance (ITU-R BT.709)
                    uint32_t lum = (218 * src[s_idx] + 732 * src[s_idx + 1] + 74 * src[s_idx + 2]) >> 10;

                    if(lum >= 254)
                    {
                        // Highlights blown
                        SetPixel(dest, d_idx, 255, 0, 0, 200); // Semi-transparent Red
                    } else if(lum <= 1)
                    {
                        // Shadows crushed
                        SetPixel(dest, d_idx, 0, 150, 255, 200); // Semi-transparent Cyan/Blue
                    } else
                    {
                        SetPixel(dest, d_idx, 0, 0, 0, 0); // Transparent
                    }
                }
            }
            return m_buffer;
        }

    private:
        Glib::RefPtr<Gdk::Pixbuf> m_buffer;

        void PrepareBuffer(int w, int h)
        {
            if(!m_buffer || m_buffer->get_width() != w || m_buffer->get_height() != h)
                m_buffer = Gdk::Pixbuf::create(Gdk::Colorspace::RGB, true, 8, w, h);
        }

        static void SetPixel(uint8_t* data, int idx, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            data[idx] = r;
            data[idx + 1] = g;
            data[idx + 2] = b;
            data[idx + 3] = a;
        }
    };
}