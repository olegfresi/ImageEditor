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
        * Rotates the image 90 degrees clockwise.
        *
        * Performs a structural transformation by swapping the image width and height.
        * This operation reallocates/reorganizes the internal pixel buffer to match
        * the new dimensions.
        *
        * @param image The Image object to rotate.
        * @param angle The angle of rotation.
        */
        static void Rotate(Image& image, float angle);

        /**
        * Flips the image horizontally (Mirror effect).
        *
        * Reverses the order of pixels along the X-axis. This is useful for correcting
        * mirror-image captures or creating compositional symmetry.
        *
        * @param image The Image object to flip.
        */
        static void FlipHorizontal(Image& image);

        /**
        * Flips the image vertically.
        *
        * Reverses the order of pixels along the Y-axis (top to bottom).
        *
        * @param image The Image object to flip.
        */
        static void FlipVertical(Image& image);

        /**
        * Applies a Gaussian blur to reduce image noise and detail.
        *
        * Utilizes a separable convolution kernel for performance optimization.
        * Processing is performed in floating-point space to ensure high color
        * precision and prevent rounding artifacts during the blurring pass.
        *
        * @param image The Image object to blur.
        * @param sigma The radius used to blur
        */
        static void Blur(Image& image, float sigma);

        /**
        * Inverts the image colors (Negative effect).
        *
        * Calculates the complement of each RGB channel (255 - value).
        * This method uses ApplyTransformation internally for efficient
        * point-processing iteration.
        *
        * @param image The Image object to invert.
        */
        static void Invert(Image& image);

        /**
        * Highlights edges and sudden contrast variations.
        *
        * Applies a Laplacian kernel that zeroes out uniform areas and illuminates
        * pixels where a strong color gradient is detected.
        *
        * @param image The destination Image object where the result is written.
        * @param backup A read-only span of the original pixels used as the data source.
        */
        static void EdgeDetect(Image& image, std::span<const Pixel> backup);

        /**
        * Enhances perceived sharpness by accentuating micro-contrasts.
        *
        * Strengthens the central pixel relative to its immediate neighbors,
        * making details and boundaries appear more defined.
        *
        * @param image The destination Image object where the result is written.
        * @param backup A read-only span of the original pixels used as the data source.
        */
        static void Sharpen(Image& image, std::span<const Pixel> backup);

        /**
        * Creates a 3D relief effect based on light direction.
        *
        * Calculates the intensity difference between opposing diagonal pixels and
        * adds a neutral gray offset. This filter is highly effective when followed
        * by a Tone Curve adjustment.
        *
        * @param image The destination Image object where the result is written.
        * @param backup A read-only span of the original pixels used as the data source.
        */
        static void Emboss(Image& image, std::span<const Pixel> backup);


        /**
        * Apply a sepia tone effect to the image.
        *
        * Transforms the pixel colors to warm sepia tones by reweighting the red,
        * green, and blue channels to mimic the classic photographic look.
        *
        * @param image Image that will receive the sepia conversion.
        */
        static void Sepia(Image& image);

        static void Brightness(Image& image, float amount);

        static void Exposure(Image& image, float amount);

        static void Highlights(Image& image, float amount);

        static void BlackPoint(Image& image, float amount);

        static void Shadows(Image& image, float amount);

        static void Temperature(Image& image, float amount);

        static void Clarity(Image& image, float amount);

        static void Texture(Image& image, float amount);

        static void Contrast(Image& image, float amount);

        /**
        * Restore the image to its original state using a backup buffer.
        *
        * This method performs a deep copy of the pixel data from the provided
        * backup span into the current image buffer. It is used to revert all
        * modifications and restore the image to its initial state.
        *
        * @param image The Image object to be reset
        * @param backup A read-only span containing the original pixel data
        */
        static void Reset(Image& image, std::span<const Pixel> backup);
    };
}
