#include "../../include/Core/Utils.hpp"
#include <algorithm>

namespace Editor::Utils
{
    FloatImageRGB ImageToFloatRGB(const Image& img)
    {
        int width = img.GetWidth();
        int height = img.GetHeight();

        FloatImageRGB result(width, height);

        constexpr float inv255 = 1.0f / 255.0f;

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const Pixel& p = img.GetPixel(x, y);

                result.r(x, y) = static_cast<float>(p.GetR()) * inv255;
                result.g(x, y) = static_cast<float>(p.GetG()) * inv255;
                result.b(x, y) = static_cast<float>(p.GetB()) * inv255;
            }

        return result;
    }

    Image FloatToImageRGB(const FloatImageRGB& fimg)
    {
        int width = static_cast<int>(fimg.r.GetWidth());
        int height = static_cast<int>(fimg.r.GetHeight());

        Image result(width, height, 4);

        auto clamp01 = [](float v) {
            return std::clamp(v, 0.0f, 1.0f);
        };

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                auto r = static_cast<uint8_t>(clamp01(fimg.r(x, y)) * 255.0f);
                auto g = static_cast<uint8_t>(clamp01(fimg.g(x, y)) * 255.0f);
                auto b = static_cast<uint8_t>(clamp01(fimg.b(x, y)) * 255.0f);

                result.SetPixel(x, y, r, g, b, 255);
            }

        return result;
    }

    Glib::RefPtr<Gdk::Pixbuf> PixbufFromImage(const Image& img)
    {
        const int width = img.GetWidth();
        const int height = img.GetHeight();

        auto temp_pixbuf = Gdk::Pixbuf::create_from_data(
            reinterpret_cast<const guint8*>(img.GetPixelData().data()),
            Gdk::Colorspace::RGB,
            true,
            8,
            width,
            height,
            width * 4
        );

        return temp_pixbuf->copy();
    }

    void ApplyLut(std::span<Pixel> targetPixels,
                  std::span<const Pixel> sourceBackup,
                  const std::array<uint8_t, 256>& lut)
    {
        if (targetPixels.size() != sourceBackup.size())
            return;

        uint8_t* dst = reinterpret_cast<uint8_t*>(targetPixels.data());
        const uint8_t* src = reinterpret_cast<const uint8_t*>(sourceBackup.data());
        const uint8_t* lut_ptr = lut.data();

        const size_t total_bytes = targetPixels.size() * 4;

        for (size_t i = 0; i < total_bytes; i += 4)
        {
            dst[i]     = lut_ptr[src[i]];     // R
            dst[i + 1] = lut_ptr[src[i + 1]]; // G
            dst[i + 2] = lut_ptr[src[i + 2]]; // B
            // Alpha (i+3) is skipped to preserve transparency
        }
    }
}
