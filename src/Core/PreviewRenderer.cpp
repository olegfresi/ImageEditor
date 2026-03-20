#include "../../include/Core/PreviewRenderer.hpp"
#include "Core/Processing.hpp"
#include <glibmm/main.h>
#include <iostream>
#include <thread>


namespace Editor
{
    void PreviewRenderer::SetSource(const std::span<Pixel>& pixels, int w, int h)
    {
        m_source = pixels;
        m_srcW = w;
        m_srcH = h;

        Downscale();
    }

    void PreviewRenderer::Downscale()
    {
        assert(m_source.size() == m_srcW * m_srcH);

        float scale = std::min(static_cast<float>(m_previewImageSize) / static_cast<float>(m_srcW),
                               static_cast<float>(m_previewImageSize) / static_cast<float>(m_srcH));

        m_prevW = std::max(1, static_cast<int>(static_cast<float>(m_srcW) * scale));
        m_prevH = std::max(1, static_cast<int>(static_cast<float>(m_srcH) * scale));

        m_preview.resize(m_prevW * m_prevH);
        m_previewOriginal.resize(m_prevW * m_prevH);

        for(int y = 0; y < m_prevH; y++)
            for(int x = 0; x < m_prevW; x++)
            {
                int srcX = static_cast<int>(static_cast<float>(x) / scale);
                int srcY = static_cast<int>(static_cast<float>(y) / scale);

                const Pixel& p = m_source[srcY * m_srcW + srcX];

                m_preview[y * m_prevW + x] = p;
                m_previewOriginal[y * m_prevW + x] = p;
            }
    }

    void PreviewRenderer::UpdateLut(const std::array<uint8_t, 256>& lut)
    {
        m_lut = lut;
        ApplyLut();
    }

    void PreviewRenderer::ApplyLut()
    {
        for(size_t i = 0; i < m_preview.size(); i++)
        {
            const auto& src = m_previewOriginal[i];
            auto& dst = m_preview[i];

            dst.SetPixel(m_lut[src.GetR()], m_lut[src.GetG()], m_lut[src.GetB()], m_lut[src.GetA()]);
        }
    }

    void PreviewRenderer::CommitFullRes(std::span<const Pixel> fullResSource, const std::array<uint8_t, 256>& lut,
                                        const std::function<void(std::span<const Pixel>, int, int)>& onComplete) const
    {
        auto fullResBuffer = std::make_shared<std::vector<Pixel>>(fullResSource.begin(), fullResSource.end());

        std::thread([fullResBuffer, lut, w = m_srcW, h = m_srcH, onComplete]() {
            for(auto& px : *fullResBuffer)
                px.SetPixel(lut[px.GetR()], lut[px.GetG()], lut[px.GetB()], lut[px.GetA()]);

            Glib::signal_idle().connect_once([fullResBuffer, w, h, onComplete]() {
                onComplete(*fullResBuffer, w, h);
            });
        }).detach();
    }

    void PreviewRenderer::ResetToOriginal() { m_preview = m_previewOriginal; }

    void PreviewRenderer::ApplySingleParameter(Image& img, const std::string& id, float value)
    {
        std::array<uint8_t, 256> lut{};

        if(id == "exposure")
        {
            float factor = std::pow(2.0f, value);
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(static_cast<float>(i) * factor, 0.0f, 255.0f));
        } else if(id == "brightness")
        {
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(static_cast<float>(i) + value * 255.0f, 0.0f, 255.0f));
        } else if(id == "contrast")
        {
            float factor = (259.0f * (value + 255.0f)) / (255.0f * (259.0f - value));
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(factor * (static_cast<float>(i) - 128.0f) + 128.0f, 0.0f,
                                                         255.0f));
        } else if(id == "black_point")
        {
            for(int i = 0; i < 256; i++)
            {
                float norm = (static_cast<float>(i) - value * 255.0f) / (255.0f - value * 255.0f);
                lut[i] = static_cast<uint8_t>(std::clamp(norm * 255.0f, 0.0f, 255.0f));
            }
        } else
        {
            if(id == "highlights")
                Processor::Highlights(img, value);
            else if(id == "shadows")
                Processor::Shadows(img, value);
            else if(id == "clarity")
                Processor::Clarity(img, value);
            else if(id == "texture")
                Processor::Texture(img, value);
            else
                if(id == "temperature")
                    Processor::Temperature(img, value);
            return;
        }

        img.ApplyTransformation([&lut](Pixel& px) {
            px.SetPixel(lut[px.GetR()], lut[px.GetG()], lut[px.GetB()], px.GetA());
        });
    }

    void PreviewRenderer::SetPreview(std::span<const Pixel> pixels)
    {
        assert(pixels.size() == m_preview.size());
        std::ranges::copy(pixels, m_preview.begin());
    }

    bool PreviewRenderer::ShouldUpdate()
    {
        auto now = std::chrono::high_resolution_clock::now();

        if(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count() < 8)
            return false;

        m_lastUpdate = now;
        return true;
    }

    std::optional<std::array<uint8_t, 256>> PreviewRenderer::BuildLut(const std::string& id, float value)
    {
        std::array<uint8_t, 256> lut{};

        if(id == "exposure")
        {
            float factor = std::pow(2.0f, value);
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(static_cast<float>(i) * factor, 0.0f, 255.0f));
        }
        else if(id == "brightness")
        {
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(static_cast<float>(i) + value / 2.0f, 0.0f, 255.0f));
        }
        else if(id == "contrast")
        {
            float factor = std::pow(2.0f, value / 50.0f);
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(
                    (static_cast<float>(i) - 128.0f) * factor + 128.0f, 0.0f, 255.0f));
        }
        else if(id == "black_point")
        {
            if(value <= 0.0f)
                for(int i = 0; i < 256; i++)
                    lut[i] = static_cast<uint8_t>(i);
            else
                for(int i = 0; i < 256; i++)
                {
                    float v = (static_cast<float>(i) - value) * 255.0f / (255.0f - value);
                    lut[i] = static_cast<uint8_t>(std::clamp(v, 0.0f, 255.0f));
                }
        }
        else if(id == "texture")
        {
            float factor = 1.0f + value / 100.0f;
            for(int i = 0; i < 256; i++)
                lut[i] = static_cast<uint8_t>(std::clamp(static_cast<float>(i) * factor, 0.0f, 255.0f));
        }
        else if(id == "clarity")
        {
            float a = value / 100.0f;
            for(int i = 0; i < 256; i++)
            {
                float mid = 128.0f;
                auto f = static_cast<float>(i);
                float factor = 1.0f + a * (1.0f - std::abs(f - mid) / mid);
                lut[i] = static_cast<uint8_t>(std::clamp((f - mid) * factor + mid, 0.0f, 255.0f));
            }
        }
        else if(id == "shadows")
        {
            for(int i = 0; i < 256; i++)
            {
                auto f = static_cast<float>(i);
                float factor = (1.0f - f / 255.0f) * (value / 100.0f);
                lut[i] = static_cast<uint8_t>(std::clamp(f + factor * 100.0f, 0.0f, 255.0f));
            }
        }
        else if(id == "highlights")
        {
            for(int i = 0; i < 256; i++)
            {
                auto f = static_cast<float>(i);
                float factor = (f / 255.0f) * (value / 100.0f);
                lut[i] = static_cast<uint8_t>(std::clamp(f + factor * 100.0f, 0.0f, 255.0f));
            }
        }
        else
            return std::nullopt;

        return lut;
    }
}