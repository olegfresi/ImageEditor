/*
 * Project: ImageEditor
 * File: Pixel.hpp
 * Author: olegfresi
 * Created: 20/02/26 17:21
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
#include <cstdint>

/* Class for representing raw pixels to work with images*/

namespace Editor
{
    class Pixel
    {
    public:
        Pixel();
        Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        Pixel(uint8_t r, uint8_t g, uint8_t b);

        Pixel(const Pixel&);

        bool operator==(const Pixel &) const;
        bool operator!=(const Pixel &) const;

        constexpr uint8_t GetR() const { return (m_rgba >> 24) & 0xFF; }
        constexpr uint8_t GetG() const { return (m_rgba >> 16) & 0xFF; }
        constexpr uint8_t GetB() const { return (m_rgba >> 16) & 0xFF; }
        constexpr uint8_t GetA() const { return m_rgba & 0xFF; }

        void SetPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        static constexpr uint32_t Pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    private:

        uint32_t m_rgba;
    };
}

/* Color layout of m_rgba variable: 8 bits for each component /*
/*
 *  -------- -------- -------- -------- *
 * |   r    |   g    |    b    |    a   |
 *  ----------------------------------- *
 *    8bit     8bit     8bit      8bit
 */