/*
 * Project: ImageEditor
 * File: Matrix.hpp
 * Author: olegfresi
 * Created: 24/02/26 16:46
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
#include <algorithm>
#include <cassert>
#include <vector>


namespace Editor::Math
{
    template<typename T>
    class Matrix
    {
    public:
        /**
        * Default constructor.
        *
        * Creates an empty matrix with zero dimensions and no allocated data.
        */
        Matrix() = default;

        /**
        * Constructor with dimensions.
        *
        * Allocates a 2D matrix with the specified width and height.
        * Data is stored in row-major order in a contiguous vector.
        *
        * @param width Number of columns
        * @param height Number of rows
        */
        Matrix(int width, int height) : m_width(width), m_height(height), m_data(width * height) {}

        /**
        * Element access by (x, y) coordinates.
        *
        * Accesses the element at column x, row y.
        * Uses asserts to check bounds in debug mode.
        *
        * @param x Column index
        * @param y Row index
        * @return Reference to the element at (x, y)
        */
        T &operator()(size_t x, size_t y)
        {
            assert(x < m_width);
            assert(y < m_height);
            return m_data[y * m_width + x];
        }

        /**
        * Const element access by (x, y) coordinates.
        *
        * Read-only access to the element at column x, row y.
        * Uses asserts to check bounds in debug mode.
        *
        * @param x Column index
        * @param y Row index
        * @return Const reference to the element at (x, y)
        */
        const T &operator()(size_t x, size_t y) const
        {
            assert(x < m_width);
            assert(y < m_height);
            return m_data[y * m_width + x];
        }

        /**
        * Linear index access.
        *
        * Direct access to the underlying flat data storage by linear index.
        *
        * @param i Linear index into the contiguous data array
        * @return Reference to the element at index i
        */
        T &operator[](size_t i) { return m_data[i]; }

        /**
        * Const linear index access.
        *
        * Read-only access to the underlying flat data storage by linear index.
        *
        * @param i Linear index into the contiguous data array
        * @return Const reference to the element at index i
        */
        const T &operator[](size_t i) const { return m_data[i]; }

        /**
        * Get matrix width (number of columns).
        *
        * @return Width of the matrix
        */
        [[nodiscard]] size_t GetWidth() const { return m_width; }

        /**
        * Get matrix height (number of rows).
        *
        * @return Height of the matrix
        */
        [[nodiscard]] size_t GetHeight() const { return m_height; }

        /**
        * Get pointer to underlying data.
        *
        * @return Pointer to the contiguous data array
        */
        T *Data() { return m_data.data(); }

        /**
        * Get const pointer to underlying data.
        *
        * @return Const pointer to the contiguous data array
        */
        const T *Data() const { return m_data.data(); }

        /**
        * Resize the matrix to new dimensions.
        *
        * Reallocates the data array and updates dimensions.
        * Previous contents are lost.
        *
        * @param w New width (columns)
        * @param h New height (rows)
        */
        void Resize(size_t w, size_t h)
        {
            m_width = w;
            m_height = h;
            m_data.resize(w * h);
        }

        /**
        * Fill entire matrix with a single value.
        *
        * Sets all elements to the specified value.
        *
        * @param value The value to fill with
        */
        void Fill(const T &value) { std::fill(m_data.begin(), m_data.end(), value); }

        /**
        * Get pointer to a specific row.
        *
        * Returns a pointer to the first element of the specified row,
        * allowing direct row-wise access.
        *
        * @param y Row index
        * @return Pointer to the first element of row y
        */
        T *Row(size_t y)
        {
            assert(y < m_height);
            return m_data.data() + y * m_width;
        }

        /**
        * Get const pointer to a specific row.
        *
        * Returns a const pointer to the first element of the specified row,
        * allowing read-only row-wise access.
        *
        * @param y Row index
        * @return Const pointer to the first element of row y
        */
        const T *Row(size_t y) const
        {
            assert(y < m_height);
            return m_data.data() + y * m_width;
        }

    private:
        size_t m_width = 0;
        size_t m_height = 0;
        std::vector<T> m_data;
    };
}
