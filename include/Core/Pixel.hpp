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

class Pixel
{
public:
    Pixel(uint32_t r, uint32_t g, uint32_t b, uint32_t a);
    Pixel(uint32_t r, uint32_t g, uint32_t b);

    Pixel(const Pixel&);

    bool operator==(const Pixel &) const;
    bool operator!=(const Pixel &) const;

    uint32_t GetR();
    uint32_t GetG();
    uint32_t GetB();
    uint32_t GetA();

    void SetPixel(int r, int g, int b, int a);
    void GetPixelAt(uint32_t x, uint32_t y);

private:

    uint32_t m_r;
    uint32_t m_g;
    uint32_t m_b;
    uint32_t m_a;
};