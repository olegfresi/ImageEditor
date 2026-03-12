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
#include <filesystem>
#include <vector>
#include "Pixel.hpp"
#include "../Filters/Filter.hpp"

namespace Editor
{
    /**
    * Container for raw image data.
    *
    * Stores the pixel data and metadata for an image including dimensions and channel count.
    * Used internally by the Image class to manage image information.
    */
    struct ImageData
    {
        int width;
        int height;
        int channels;
        std::vector<Pixel> pixels;
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

        /**
        * Constructor with file path and dimensions.
        *
        * Creates an image with specified dimensions and initializes pixel storage.
        * Optionally loads image data from a file.
        *
        * @param filePath Path to the image file to load
        * @param width Image width in pixels
        * @param height Image height in pixels
        * @param channels Number of color channels (typically 4 for RGBA)
        */
        Image(std::filesystem::path filePath, int width, int height, int channels);

        /**
        * Constructor with dimensions only.
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
        * Load image from file.
        *
        * Reads image data from disk using the stb_image library.
        * Supported formats: PNG, JPG, BMP, TGA, and others.
        * Converts loaded image to RGBA format internally.
        *
        * @param filePath Path to the image file
        * @return true if loading succeeded, false otherwise
        */
        bool LoadImage(const std::filesystem::path& filePath);

        /**
        * Rotate image 90 degrees clockwise.
        *
        * Modifies the image in-place.
        * Swaps width and height, rearranges pixel data.
        */
        void Rotate();

        /**
        * Flip image horizontally (mirror across vertical axis).
        *
        * Modifies the image in-place.
        * Left-right pixels are reversed.
        */
        void FlipHorizontal();

        /**
        * Flip image vertically (mirror across horizontal axis).
        *
        * Modifies the image in-place.
        * Top-bottom pixels are reversed.
        */
        void FlipVertical();

        /**
        * Apply a filter to the image.
        *
        * Applies the specified filter type to modify image appearance.
        * Filter is applied in-place to the current image.
        *
        * @param type The filter type to apply (e.g., blur, edge detection)
        */
        void ApplyFilter(Filter::FilterType type);

        /**
        * Convert image to grayscale.
        *
        * Uses standard luminance formula to convert RGB to single intensity value.
        * Image is modified in-place while preserving alpha channel.
        */
        void ToGrayScale();

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
        [[nodiscard]] int GetWidth() const { return m_data.width; }

        /**
        * Get image height.
        *
        * @return Height in pixels
        */
        [[nodiscard]] int GetHeight() const { return m_data.height; }

        /**
        * Get number of color channels.
        *
        * @return Channel count (typically 4 for RGBA)
        */
        [[nodiscard]] int GetChannels() const { return m_data.channels; }

        /**
        * Get const reference to pixel data.
        *
        * Provides read-only access to the underlying pixel vector.
        * Pixels are stored in row-major order.
        *
        * @return Const reference to the pixel data vector
        */
        [[nodiscard]] const std::vector<Pixel>& GetPixelData() const { return m_data.pixels; }

        /**
        * Get mutable reference to pixel data.
        *
        * Provides read-write access to the underlying pixel vector.
        * Pixels are stored in row-major order.
        * Use with caution to avoid corrupting image state.
        *
        * @return Mutable reference to the pixel data vector
        */
        [[nodiscard]] std::vector<Pixel>& GetPixelData() { return m_data.pixels; }

    private:
        ImageData m_data;
        std::filesystem::path m_path;
    };
}