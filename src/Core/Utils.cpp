#include "../../include/Core/Utils.hpp"
#include <algorithm>
#include <thread>
#include <cmath>


namespace Editor::Utils
{
    FloatImageRGB ImageToFloatRGB(const Image& img)
    {
        const int width = img.GetWidth();
        const int height = img.GetHeight();

        FloatImageRGB result(width, height);
        constexpr float inv255 = 1.0f / 255.0f;

        auto pixels = img.GetPixelData();

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const auto& p = pixels[y * width + x];

                result.r(x, y) = static_cast<float>(p.GetR()) * inv255;
                result.g(x, y) = static_cast<float>(p.GetG()) * inv255;
                result.b(x, y) = static_cast<float>(p.GetB()) * inv255;
            }

        return result;
    }

    void UpdateImageFromFloat(Image& dest, const FloatImageRGB& fimg)
    {
        const int width = dest.GetWidth();
        const int height = dest.GetHeight();
        auto pixels = dest.GetPixelData();

        auto parallelLambda = [&](auto workFunc) {
            unsigned int numThreads = std::thread::hardware_concurrency();
            if(numThreads == 0)
                numThreads = 4;

            std::vector<std::thread> threads;
            threads.reserve(static_cast<int>(numThreads));
            for(int t = 0; t < static_cast<int>(numThreads); ++t)
            {
                threads.emplace_back([t, numThreads, height, workFunc]() {
                    int rowsPerThread = height / static_cast<int>(numThreads);
                    int start = t * rowsPerThread;
                    int end = (t == static_cast<int>(numThreads) - 1) ? height : start + rowsPerThread;
                    for(int y = start; y < end; ++y)
                        workFunc(y);
                });
            }
            for(auto& thread : threads)
                thread.join();
        };

        parallelLambda([&](int y) {
            for(int x = 0; x < width; ++x)
            {
                int idx = y * width + x;
                pixels[idx].SetPixel(
                        static_cast<uint8_t>(std::clamp(fimg.r(x, y), 0.0f, 1.0f) * 255.0f),
                        static_cast<uint8_t>(std::clamp(fimg.g(x, y), 0.0f, 1.0f) * 255.0f),
                        static_cast<uint8_t>(std::clamp(fimg.b(x, y), 0.0f, 1.0f) * 255.0f),
                        255
                        );
            }
        });
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
        if(targetPixels.size() != sourceBackup.size())
            return;

        auto* dst = reinterpret_cast<uint8_t*>(targetPixels.data());
        const auto* src = reinterpret_cast<const uint8_t*>(sourceBackup.data());
        const uint8_t* lut_ptr = lut.data();

        const size_t total_bytes = targetPixels.size() * 4;

        for(size_t i = 0; i < total_bytes; i += 4)
        {
            dst[i] = lut_ptr[src[i]]; // R
            dst[i + 1] = lut_ptr[src[i + 1]]; // G
            dst[i + 2] = lut_ptr[src[i + 2]]; // B
            // Alpha (i+3) is skipped to preserve transparency
        }
    }

}