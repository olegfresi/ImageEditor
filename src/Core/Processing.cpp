#include "../../include/Core/Processing.hpp"


namespace Editor
{
    void Processor::ToGrayScale(Image& image)
    {
        image.ApplyTransformation([](Pixel& px) {
            const uint8_t gray = static_cast<uint8_t>(
                0.299f * px.GetR() +
                0.587f * px.GetG() +
                0.114f * px.GetB()
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
}