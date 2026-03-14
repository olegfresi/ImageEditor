#include "../../include/Core/Processing.hpp"
#include "../../include/Filters/Blur.hpp"
#include "../../include/Filters/EdgeDetection.hpp"
#include "../../include/Filters/Sharpen.hpp"
#include "../../include/Filters/Emboss.hpp"
#include "../../include/Filters/ColorInversion.hpp"


namespace Editor
{
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

    void Processor::FlipHorizontal(Image& image)
    {
        int w = image.GetWidth();
        auto original = image.GetPixelData();

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            px = original[y * w + (w - 1 - x)];
        });
    }

    void Processor::FlipVertical(Image& image)
    {
        int w = image.GetWidth();
        int h = image.GetHeight();

        auto original = image.GetPixelData();

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            px = original[(h - 1 - y) * w + x];
        });
    }

    void Processor::Rotate(Image& image)
    {
        int oldW = image.GetWidth();
        int oldH = image.GetHeight();
        auto source = image.GetPixelData();

        image.SetWidth(oldH);
        image.SetHeight(oldW);

        image.ApplyIndexedTransformation([&](Pixel& px, int x, int y) {
            int oldX = y;
            int oldY = oldH - 1 - x;
            px = source[oldY * oldW + oldX];
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

    void Processor::Reset(Image& image, std::span<const Pixel> backup)
    {
        auto target = image.GetPixelData();

        if(target.size() == backup.size())
            std::ranges::copy(backup.begin(), backup.end(), target.begin());
    }
}