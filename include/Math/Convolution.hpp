/*
 * Project: ImageEditor
 * File: Convolution.hpp
 * Author: olegfresi
 * Created: 23/02/26 18:08
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
#include "Matrix.hpp"

namespace Editor::Math
{

    enum class BorderMode
    {
        Clamp,
        Wrap,
        Mirror
    };

    /**
    * Apply 1D convolution to a 1D signal.
    *
    * Performs a linear convolution of the input signal with the kernel.
    * Constexpr allows compile-time evaluation if inputs are constant.
    *
    * @param input Input signal vector
    * @param kernel Convolution kernel (filter coefficients)
    * @return Convolved output signal
    */
    constexpr std::vector<float> Convolve1D(const std::vector<float>& input, const std::vector<float>& kernel);

    /**
    * Apply 2D convolution to a 2D matrix.
    *
    * Performs a 2D convolution operation commonly used in image filtering.
    * Constexpr allows compile-time evaluation if inputs are constant.
    *
    * @param input Input 2D matrix (image data)
    * @param kernel Convolution kernel (2D filter)
    * @return Convolved output matrix
    */
    constexpr Matrix<float> Convolve2D(const Matrix<float>& input, const Matrix<float>& kernel);

    /**
    * Apply separable 2D convolution efficiently.
    *
    * Decomposes a 2D convolution into two 1D convolutions (horizontal then vertical).
    * Significantly faster than full 2D convolution for separable kernels (e.g., Gaussian).
    * Time complexity reduces from O(k²) to O(2k) where k is kernel size.
    *
    * @param input Input 2D matrix (image data)
    * @param kernelX Horizontal 1D kernel
    * @param kernelY Vertical 1D kernel
    * @return Convolved output matrix
    */
    Matrix<float> ConvolveSeparable( const Matrix<float>& input, const std::vector<float>& kernelX, const std::vector<float>& kernelY);
}