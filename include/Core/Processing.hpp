/*
 * Project: ImageEditor
 * File: ImageProcessing.hpp
 * Author: olegfresi
 * Created: 12/03/26 21:19
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
#include "../../include/Core/Image.hpp"

namespace Editor
{
    class Processor
    {
    public:

        /**
        * Convert an image from RGB color to grayscale.
        *
        * Transforms a color image into grayscale by computing luminance from RGB channels
        *
        * The operation modifies the image in-place, converting all pixels while preserving
        * the alpha channel for transparency information. The resulting image maintains
        * the same dimensions and pixel count as the original.
        *
        * @param image Reference to the Image object to convert.
        */
        static void ToGrayScale(Image& image);

        /**
        * Rotate image 90 degrees clockwise.
        *
        * Modifies the image in-place.
        * Swaps width and height, rearranges pixel data.
        */
        static void Rotate(Image& image);

        /**
        * Flip image horizontally (mirror across vertical axis).
        *
        * Modifies the image in-place.
        * Left-right pixels are reversed.
        */
        static void FlipHorizontal(Image& image);

        /**
        * Flip image vertically (mirror across horizontal axis).
        *
        * Modifies the image in-place.
        * Top-bottom pixels are reversed.
        */
        static void FlipVertical(Image& image);
    };
}