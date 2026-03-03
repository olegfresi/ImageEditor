/*
 * Project: ImageEditor
 * File: Utils.cpp.hpp
 * Author: olegfresi
 * Created: 01/03/26 11:53
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
#include "../../include/Core/Utils.hpp"

#include <algorithm>

namespace Editor::Utils
{
    constexpr FloatImageRGB ImageToFloatRGB(const Image& img)
    {
        int width  = img.GetWidth();
        int height = img.GetHeight();

        FloatImageRGB result(width, height);

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
            {
                const Pixel& p = img.GetPixel(x, y);
                result.r(x, y) = static_cast<float>(p.GetR()) / 255.0f;
                result.g(x, y) = static_cast<float>(p.GetG()) / 255.0f;
                result.b(x, y) = static_cast<float>(p.GetB()) / 255.0f;
            }

        return result;
    }

    constexpr Image FloatToImageRGB(const FloatImageRGB& fimg)
    {
        int width = fimg.r.GetWidth();
        int height = fimg.r.GetHeight();

        Image result(width, height, 4);

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
            {
                auto clamp01 = [](float v) {
                    return std::clamp(v, 0.0f, 1.0f);
                };

                auto r = static_cast<uint8_t>(clamp01(fimg.r(x, y)) * 255.0f);
                auto g = static_cast<uint8_t>(clamp01(fimg.g(x, y)) * 255.0f);
                auto b = static_cast<uint8_t>(clamp01(fimg.b(x, y)) * 255.0f);

                result.SetPixel(x, y, r, g, b, 255);
            }

        return result;
    }
}
