/*
 * Project: ImageEditor
 * File: Utils.hpp
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
#include "Image.hpp"
#include "../Math/Matrix.hpp"

namespace Editor::Utils
{
    /**
    * Floating-point RGB image representation.
    *
    * Separates an image into three separate matrices for red, green, and blue channels.
    * Each channel stores normalized float values [0.0, 1.0] for precise mathematical operations.
    * Useful for image processing algorithms that require higher precision than 8-bit integers.
    */
    struct FloatImageRGB
    {
        Math::Matrix<float> r;
        Math::Matrix<float> g;
        Math::Matrix<float> b;

        /**
        * Constructor with dimensions.
        *
        * Allocates three separate matrices with the specified width and height,
        * one for each color channel.
        *
        * @param width Image width in pixels
        * @param height Image height in pixels
        */
        FloatImageRGB(int width, int height) : r{width, height}, g{width, height}, b{width, height} {}
    };

    /**
    * Convert 8-bit integer RGB image to floating-point representation.
    *
    * Extracts the R, G, B channels from an Image and converts pixel values
    * from integer range [0, 255] to normalized float range [0.0, 1.0].
    * The alpha channel is discarded. Useful before applying mathematical filters.
    *
    * @param img Input image with 8-bit RGBA pixels
    * @return FloatImageRGB structure with separated, normalized channels
    */
    FloatImageRGB ImageToFloatRGB(const Image& img);

    /**
    * Convert floating-point RGB representation to 8-bit integer image.
    *
    * Combines three separate floating-point channel matrices back into a single Image.
    * Converts pixel values from normalized float range [0.0, 1.0] to integer range [0, 255].
    * Values are clamped and rounded appropriately. Alpha channel is set to fully opaque (255).
    *
    * @param fimg Input FloatImageRGB structure with normalized channels
    * @return Image with 8-bit RGBA pixels
    */
    Image FloatToImageRGB(const FloatImageRGB& fimg);
}