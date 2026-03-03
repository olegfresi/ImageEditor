/*
 * Project: ImageEditor
 * File: Profiler.hpp
 * Author: olegfresi
 * Created: 24/02/26 14:19
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
#include <chrono>
#include <string>
#include <iostream>


namespace Editor::Tools
{
    /**
     * @class Profiler
     * @brief RAII-based performance profiling utility.
     *
     * Measures execution time of a code block using RAII (Resource Acquisition Is Initialization).
     * Automatically starts timing on construction and prints the duration on destruction.
     * Useful for quick performance measurements and bottleneck identification.
     */
    class Profiler
    {
    public:
        /**
         * @brief Constructs a profiler and starts timing.
         * @param name Name of the code section being profiled
         *
         * @details Captures the current high-resolution timestamp.
         */
        explicit Profiler(std::string name) : m_name(std::move(name)), m_start{std::chrono::high_resolution_clock::now()} {}

        /**
         * @brief Destructor - stops timing and prints the duration.
         *
         * @details Calculates elapsed time in microseconds and outputs to console.
         */
        ~Profiler()
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();
            std::cout << "Profiling " << m_name << ": " << duration << " microseconds" << std::endl;
        }

    private:
        std::string m_name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}