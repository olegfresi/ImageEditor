#include "../../include/Core/Processing.hpp"
#include "../../include/Filters/Blur.hpp"
#include "../../include/Filters/EdgeDetection.hpp"
#include "../../include/Filters/Sharpen.hpp"
#include "../../include/Filters/Emboss.hpp"
#include "../../include/Filters/ColorInversion.hpp"
#include "../../include/Filters/Sepia.hpp"
#include <cmath>
#include <iostream>

#include "Core/Utils.hpp"


namespace Editor
{
    void Processor::FlipHorizontal(Image& image)
    {
        int w = image.GetWidth();

        auto pixels = image.GetPixelData();
        std::vector original(pixels.begin(), pixels.end());

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            px = original[y * w + (w - 1 - x)];
        });
    }

    void Processor::FlipVertical(Image& image)
    {
        int w = image.GetWidth();
        int h = image.GetHeight();

        auto pixels = image.GetPixelData();
        std::vector original(pixels.begin(), pixels.end());

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            px = original[(h - 1 - y) * w + x];
        });
    }

    void Processor::Rotate(Image& image, float angleDegrees)
    {
        int oldW = image.GetWidth();
        int oldH = image.GetHeight();

        auto source = image.GetPixelData();
        std::vector sourceCopy(source.begin(), source.end());

        float cx = static_cast<float>(oldW) / 2.0f;
        float cy = static_cast<float>(oldH) / 2.0f;

        float angleRad = static_cast<float>(angleDegrees * 3.14159265358979) / 180.0f;
        float cosA = cosf(angleRad);
        float sinA = sinf(angleRad);

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            float dx = static_cast<float>(x) - cx;
            float dy = static_cast<float>(y) - cy;

            float srcX = cosA * dx + sinA * dy + cx;
            float srcY = -sinA * dx + cosA * dy + cy;

            int ix = static_cast<int>(round(srcX));
            int iy = static_cast<int>(round(srcY));

            if(ix >= 0 && ix < oldW && iy >= 0 && iy < oldH)
                px = sourceCopy[iy * oldW + ix];
            else
                px = Pixel();
        });
    }

    void Processor::ToGrayScale(Image& image)
    {
        image.ApplyTransformation([](Pixel& px) {
            const auto gray = static_cast<uint8_t>(
                0.299f * static_cast<float>(px.GetR()) +
                0.587f * static_cast<float>(px.GetG()) +
                0.114f * static_cast<float>(px.GetB())
            );

            px.SetPixel(gray, gray, gray, px.GetA());
        });
    }

    void Processor::Blur(Image& image, float sigma)
    {
        Filter::GaussianBlur(image, sigma);
    }

    void Processor::Invert(Image& image)
    {
        Filter::InvertColor(image);
    }

    void Processor::EdgeDetect(Image& image, std::span<const Pixel> backup)
    {
        Filter::EdgeDetection(image, backup);
    }

    void Processor::Sharpen(Image& image, std::span<const Pixel> backup)
    {
        Filter::Sharpen(image, backup);
    }

    void Processor::Emboss(Image& image, std::span<const Pixel> backup)
    {
        Filter::Emboss(image, backup);
    }

    void Processor::Sepia(Image& image)
    {
        Filter::Sepia(image);
    }

    void Processor::Brightness(Image& image, float amount)
    {
        image.ApplyTransformation([amount](Pixel& px) {
            px.SetPixel(
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) + amount)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetG()) + amount)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) + amount)),
                    px.GetA()
                    );
        });
    }

    void Processor::Contrast(Image& image, float amount)
    {
        float factor = std::pow(2.0f, amount / 50.0f);

        image.ApplyTransformation([factor](Pixel& px) {
            px.SetPixel(
                Utils::Clamp(static_cast<int>((static_cast<float>(px.GetR()) - 128.0f) * factor + 128.0f)),
                Utils::Clamp(static_cast<int>((static_cast<float>(px.GetG()) - 128.0f) * factor + 128.0f)),
                Utils::Clamp(static_cast<int>((static_cast<float>(px.GetB()) - 128.0f) * factor + 128.0f)),
                px.GetA()
            );
        });
    }

    void Processor::Temperature(Image& image, float amount)
    {
        float shift = amount * 0.3f;

        image.ApplyTransformation([shift](Pixel& px) {
            px.SetPixel(
                Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) + shift)),
                px.GetG(),
                Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) - shift)),
                px.GetA()
            );
        });
    }

    void Processor::Shadows(Image& image, float amount)
    {
        image.ApplyTransformation([amount](Pixel& px) {
            float lum = Utils::Luminance(px);
            float factor = (1.0f - lum / 255.0f) * (amount / 100.0f);

            px.SetPixel(
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) + factor * 100.0f)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetG()) + factor * 100.0f)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) + factor * 100.0f)),
                    px.GetA()
                    );
        });
    }

    void Processor::Exposure(Image& image, float amount)
    {
        float factor = std::pow(2.0f, amount);

        image.ApplyTransformation([factor](Pixel& px) {
            px.SetPixel(
                Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) * factor)),
                Utils::Clamp(static_cast<int>(static_cast<float>(px.GetG()) * factor)),
                Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) * factor)),
                px.GetA()
            );
        });
    }

    void Processor::Highlights(Image& image, float amount)
    {
        image.ApplyTransformation([amount](Pixel& px) {
            float lum = Utils::Luminance(px);
            float factor = (lum / 255.0f) * (amount / 100.0f);

            px.SetPixel(
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) + factor * 100.0f)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetG()) + factor * 100.0f)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) + factor * 100.0f)),
                    px.GetA()
                    );
        });
    }

    void Processor::BlackPoint(Image& image, float amount)
    {
        if(amount <= 0.0f)
            return;

        image.ApplyTransformation([amount](Pixel& px) {
            px.SetPixel(
                    Utils::Clamp(
                            static_cast<int>((static_cast<float>(px.GetR()) - amount) * 255.0f / (255.0f - amount))),
                    Utils::Clamp(
                            static_cast<int>((static_cast<float>(px.GetG()) - amount) * 255.0f / (255.0f - amount))),
                    Utils::Clamp(
                            static_cast<int>((static_cast<float>(px.GetB()) - amount) * 255.0f / (255.0f - amount))),
                    px.GetA()
                    );
        });
    }

    void Processor::Clarity(Image& image, float amount)
    {
        float a = amount / 100.0f;

        image.ApplyTransformation([a](Pixel& px) {
            float lum = Utils::Luminance(px);
            float mid = 128.0f;
            float factor = 1.0f + a * (1.0f - std::abs(lum - mid) / mid);

            px.SetPixel(
                    Utils::Clamp(static_cast<int>((static_cast<float>(px.GetR()) - mid) * factor + mid)),
                    Utils::Clamp(static_cast<int>((static_cast<float>(px.GetG()) - mid) * factor + mid)),
                    Utils::Clamp(static_cast<int>((static_cast<float>(px.GetB()) - mid) * factor + mid)),
                    px.GetA()
                    );
        });
    }

    void Processor::Texture(Image& image, float amount)
    {
        float factor = 1.0f + amount / 100.0f;

        image.ApplyTransformation([factor](Pixel& px) {
            px.SetPixel(
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetR()) * factor)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetG()) * factor)),
                    Utils::Clamp(static_cast<int>(static_cast<float>(px.GetB()) * factor)),
                    px.GetA()
                    );
        });
    }

    void Processor::Reset(Image& image, std::span<const Pixel> backup)
    {
        auto target = image.GetPixelData();

        if(target.size() == backup.size())
            std::ranges::copy(backup.begin(), backup.end(), target.begin());
    }
}