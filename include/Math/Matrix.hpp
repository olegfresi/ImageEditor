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
#include <cassert>
#include <vector>


namespace Editor::Math
{
    template<typename T>
    class Matrix
    {
    public:
        Matrix(int width, int height) : m_width(width), m_height(height), m_data(width * height) {}

        T& operator()(int x, int y)
        {
            assert(x <= m_width && y <= m_height);
            return m_data[y * m_width + x];
        }

        const T& operator()(int x, int y) const { return m_data[y * m_width + x]; }

        [[nodiscard]] int GetWidth()  const { return m_width; }
        [[nodiscard]] int GetHeight() const { return m_height; }

    private:
        int m_width;
        int m_height;
        std::vector<T> m_data;
    };
}