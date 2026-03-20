/*
 * Project: ImageEditor
 * File: PreviewRenderer.hpp
 * Author: olegfresi
 * Created: 19/03/26 21:25
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
#include "Pixel.hpp"
#include "Utils.hpp"
#include <span>
#include <chrono>
#include <array>
#include <functional>
#include <vector>


namespace Editor
{
    class PreviewRenderer
    {
    public:
        PreviewRenderer() = default;
        ~PreviewRenderer() = default;

        void SetSource(const std::span<Pixel>& pixels, int w, int h);
        void UpdateLut(const std::array<uint8_t, 256>& lut);
        void CommitFullRes(std::span<const Pixel> fullResSource, const std::array<uint8_t, 256>& lut,
                           const std::function<void(std::span<const Pixel>, int, int)>& onComplete) const;
        static void ApplySingleParameter(Image& img, const std::string& id, float value);
        void ResetToOriginal();

        bool ShouldUpdate();
        static std::optional<std::array<uint8_t, 256>> BuildLut(const std::string& id, float value);

        [[nodiscard]] const std::vector<Pixel>& GetOriginalPixels() const { return m_previewOriginal; }
        [[nodiscard]] const std::vector<Pixel>& GetPixels() const noexcept { return m_preview; }
        [[nodiscard]] int GetWidth() const noexcept{ return m_prevW; }
        [[nodiscard]] int GetHeight() const noexcept {return m_prevH; }

        void SetPreview(std::span<const Pixel> pixels);

    private:
        void Downscale();
        void ApplyLut();

    private:
        std::span<const Pixel> m_source;
        std::array<uint8_t, 256> m_lut{};

        std::vector<Pixel> m_preview;
        std::vector<Pixel> m_previewOriginal;

        int m_srcW = 0, m_srcH = 0;
        int m_prevW = 0, m_prevH = 0;
        static constexpr int m_previewImageSize = 2048;

        std::chrono::high_resolution_clock::time_point m_lastUpdate;
    };
}