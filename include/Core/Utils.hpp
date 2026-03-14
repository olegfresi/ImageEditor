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
#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <span>
#include <thread>
#include <vector>
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
    * Update image pixels from a floating-point RGB representation.
    *
    * Maps three separate floating-point channel matrices back into the provided Image object.
    * Converts pixel values from a normalized float range [0.0, 1.0] to an 8-bit integer
    * range [0, 255]. Values outside the normalized range are clamped.
    * The alpha channel is set to fully opaque (255).
    *
    * @param dest The destination Image object to be updated
    * @param fimg Input FloatImageRGB structure containing normalized channel data
    */
    void UpdateImageFromFloat(Image& dest, const FloatImageRGB& fimg);

    /**
    * Convert Image object to GTK Pixbuf.
    *
    * Converts the internal Image representation to a Gdk::Pixbuf
    * suitable for GTK display.
    *
    * @param img The Image to convert
    * @return Pixbuf representation of the image
    */
    Glib::RefPtr<Gdk::Pixbuf> PixbufFromImage(const Image& img);

    /**
    * Apply a lookup table transformation to pixel data.
    *
    * Transforms the RGB components of pixels using a 256-entry lookup table.
    * Each color channel value (0-255) is mapped through the LUT to produce
    * the output pixel. The alpha channel remains unchanged to preserve transparency.
    * Commonly used for tone curve adjustments and color corrections.
    *
    * @param targetPixels Output pixel buffer to store transformed pixels
    * @param sourceBackup Input pixel buffer with original pixel data
    * @param lut 256-entry lookup table mapping input intensities (0-255) to output intensities (0-255)
    */
    void ApplyLut(std::span<Pixel> targetPixels,
                  std::span<const Pixel> sourceBackup,
                  const std::array<uint8_t, 256>& lut);

    /**
    * Parallelizes work across multiple hardware threads.
    *
    * Divides the total range of iterations into chunks
    * and processes them in parallel using multiple threads.
    *
    * @param totalRange Number of iterations to perform; the callable is invoked
    *                   with every integer `i` in `[0, totalRange)`.
    * @param work Callable that processes a single index.
    */
    template<typename Func>
    void Parallelize(int totalRange, Func work)
    {
        unsigned int numThreads = std::thread::hardware_concurrency();
        if(numThreads == 0)
            numThreads = 4; // Fallback to 4 threads if hardware detection fails

        std::vector<std::thread> threads;
        threads.reserve(numThreads);

        int chunkSize = totalRange / static_cast<int>(numThreads);

        for(unsigned int t = 0; t < numThreads; ++t)
        {
            int start = static_cast<int>(t) * chunkSize;
            // Ensure the last thread covers the remaining range
            int end = (t == numThreads - 1) ? totalRange : start + chunkSize;

            threads.emplace_back([start, end, work]() {
                for(int i = start; i < end; ++i)
                    work(i);
            });
        }

        // Join all threads to the main execution flow
        for(auto& th : threads)
            th.join();
    }
}
