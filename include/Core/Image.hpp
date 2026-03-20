/*
 * Project: ImageEditor
 * File: Image.hpp
 * Author: olegfresi
 * Created: 20/02/26 17:02
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
#include <functional>
#include <vector>
#include <span>
#include "Pixel.hpp"

namespace Editor
{

    using PixelTransformer = std::function<void(Pixel&)>;
    using IndexedPixelTransformer = std::function<void(Pixel&, int, int)>;

    /**
    * Container for raw image data.
    *
    * Stores the pixel data and metadata for an image including dimensions and channel count.
    * Used internally by the Image class to manage image information.
    */
    struct ImageData
    {
        int width{};
        int height{};
        int channels{};
        std::vector<Pixel> pixels{};
    };

    /**
    * Main image class for loading, manipulating, and processing images.
    *
    * Provides methods for image I/O, transformations, filters, and pixel access.
    * Supports copy and move semantics. Images are stored as RGBA pixels internally.
    */
    class Image
    {
    public:
        Image() = default;

        /**
        * Constructor with dimensions.
        *
        * Creates an empty image with specified dimensions.
        * Pixel data is uninitialized.
        *
        * @param width Image width in pixels
        * @param height Image height in pixels
        * @param channels Number of color channels (typically 4 for RGBA)
        */
        Image(int width, int height, int channels);

        /**
        * Constructor with dimensions and pixel data.
        *
        * Creates an image with specified dimensions and initializes pixel data from the provided vector.
        * The pixel vector should contain width * height elements.
        *
        * @param width Image width in pixels
        * @param height Image height in pixels
        * @param channels Number of color channels (typically 4 for RGBA)
        * @param pixels Vector of pixel data to initialize the image with
        */
        Image(int width, int height, int channels, std::vector<Pixel> pixels);

        /**
        * Copy constructor.
        *
        * Creates a deep copy of another image, including all pixel data.
        *
        * @param other The image to copy from
        */
        Image(const Image& other);

        /**
        * Move constructor.
        *
        * Transfers ownership of image data from another image.
        * The source image is left in an empty state.
        *
        * @param other The image to move from (will be invalidated)
        */
        Image(Image&& other) noexcept;

        /**
        * Copy assignment operator.
        *
        * Assigns a deep copy of another image to this image.
        *
        * @param other The image to copy from
        * @return Reference to this image
        */
        Image& operator=(const Image& other);

        /**
        * Move assignment operator.
        *
        * Transfers ownership of image data from another image.
        * The source image is left in an empty state.
        *
        * @param other The image to move from (will be invalidated)
        * @return Reference to this image
        */
        Image& operator=(Image&& other) noexcept;

        /**
        * Set a single pixel color.
        *
        * Writes RGBA values to a specific pixel location.
        *
        * @param x Column index (0 to width-1)
        * @param y Row index (0 to height-1)
        * @param r Red channel value (0-255)
        * @param g Green channel value (0-255)
        * @param b Blue channel value (0-255)
        * @param a Alpha channel value (0-255)
        */
        void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

        /**
        * Get a single pixel color.
        *
        * Reads RGBA values from a specific pixel location.
        *
        * @param x Column index (0 to width-1)
        * @param y Row index (0 to height-1)
        * @return Pixel object containing RGBA values
        */
        [[nodiscard]] Pixel GetPixel(int x, int y) const { return m_data.pixels[y * m_data.width + x]; }

        /**
        * Get image width.
        *
        * @return Width in pixels
        */
        [[nodiscard]] int GetWidth() const noexcept { return m_data.width; }

        /**
        * Get image height.
        *
        * @return Height in pixels
        */
        [[nodiscard]] int GetHeight() const noexcept { return m_data.height; }

        /**
        * Get number of color channels.
        *
        * @return Channel count (typically 4 for RGBA)
        */
        [[nodiscard]] int GetChannels() const noexcept { return m_data.channels; }

        [[nodiscard]] bool Empty() const noexcept { return m_data.pixels.empty(); }

        [[nodiscard]] Pixel& At(int x, int y)
        {
            return m_data.pixels[y * m_data.width + x];
        }

        [[nodiscard]] const Pixel& At(int x, int y) const
        {
            return m_data.pixels[y * m_data.width + x];
        }

        /**
        * Get a read-only view of the image pixel data.
        *
        * Returns a span providing safe, non-owning access to the underlying
        * pixel buffer. This allows for efficient iteration and reading without
        * exposing the internal vector's management functions (like resize or clear).
        *
        * @return A constant span of pixels
        */
        [[nodiscard]] std::span<const Pixel> GetPixelData() const { return m_data.pixels; }

        /**
        * Get a mutable view of the image pixel data.
        *
        * Returns a span that allows modification of individual pixels while
        * preventing changes to the container's structure. This is the preferred
        * way to pass pixel data to external processing functions like LUT applications.
        *
        * @return A mutable span of pixels
        */
        [[nodiscard]] std::span<Pixel> GetPixelData() { return m_data.pixels; }

        /**
        * Set the image width.
        *
        * Updates the width dimension of the image. This should typically be used
        * in conjunction with resizing pixel data to maintain image integrity.
        * Changing width without updating pixel data may cause inconsistencies.
        *
        * @param width New width value in pixels
        */
        void SetWidth(int width) { m_data.width = width; }

        /**
        * Set the image height.
        *
        * Updates the height dimension of the image. This should typically be used
        * in conjunction with resizing pixel data to maintain image integrity.
        * Changing height without updating pixel data may cause inconsistencies.
        *
        * @param height New height value in pixels
        */
        void SetHeight(int height) { m_data.height = height; }

        /**
        * Apply a transformation to every pixel in the image.
        *
        * Iterates through all pixel data and applies the provided transformer
        * function to each pixel. This is an internal iterator that ensures
        * safe access to pixel data without exposing the underlying container.
        * It is best suited for point operations like grayscale or brightness.
        *
        * @param transformer A function or lambda that accepts a Pixel reference
        */
        void ApplyTransformation(const PixelTransformer& transformer);

        /**
        * Apply a coordinate-aware transformation to every pixel in the image.
        *
        * Iterates through all pixel data using a nested row-major loop and
        * provides the current (x, y) coordinates to the transformer function.
        * This is essential for transformations where the new state of a pixel
        * depends on its spatial position, such as rotations, flips, or gradients.
        *
        * @param transformer A function or lambda that accepts a Pixel reference and (x, y) coordinates
        */
        void ApplyIndexedTransformation(const IndexedPixelTransformer& transformer);

    private:
        ImageData m_data;
    };
}