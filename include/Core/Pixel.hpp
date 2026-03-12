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

        /**
        * Equality operator.
        *
        * Compares two pixels for equality across all color channels (R, G, B, A).
        *
        * @param other The pixel to compare with
        * @return true if all channels are equal, false otherwise
        */
        bool operator==(const Pixel& other) const;

        /**
        * Inequality operator.
        *
        * Compares two pixels for inequality.
        *
        * @param other The pixel to compare with
        * @return true if any channel differs, false if all channels are equal
        */
        bool operator!=(const Pixel& other) const;

        /**
        * Get red channel value.
        *
        * @return 8-bit red channel (0-255)
        */
        [[nodiscard]] constexpr uint8_t GetR() const { return m_r; }

        /**
        * Get green channel value.
        *
        * @return 8-bit green channel (0-255)
        */
        [[nodiscard]] constexpr uint8_t GetG() const { return m_g; }

        /**
        * Get blue channel value.
        *
        * @return 8-bit blue channel (0-255)
        */
        [[nodiscard]] constexpr uint8_t GetB() const { return m_b; }

        /**
        * Get alpha (transparency) channel value.
        *
        * @return 8-bit alpha channel (0-255, where 0 is transparent and 255 is opaque)
        */
        [[nodiscard]] constexpr uint8_t GetA() const { return m_a; }

        /**
        * Set all pixel color channels.
        *
        * Sets the red, green, blue, and alpha channel values in one operation.
        *
        * @param r Red channel value (0-255)
        * @param g Green channel value (0-255)
        * @param b Blue channel value (0-255)
        * @param a Alpha channel value (0-255)
        */
        void SetPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    private:

        uint8_t m_r{};
        uint8_t m_g{};
        uint8_t m_b{};
        uint8_t m_a{};
    };
}