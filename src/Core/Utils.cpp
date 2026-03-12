#include "../../include/Core/Utils.hpp"
#include <algorithm>

namespace Editor::Utils
{
    FloatImageRGB ImageToFloatRGB(const Image& img)
    {
        int width = img.GetWidth();
        int height = img.GetHeight();

        FloatImageRGB result(width, height);

        /*
            Precompute the reciprocal to avoid a floating point
            division for every pixel. Multiplication is faster
            and easier for the compiler to vectorize.
        */
        constexpr float inv255 = 1.0f / 255.0f;

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                const Pixel& p = img.GetPixel(x, y);

                /*
                    Convert integer color channels [0,255]
                    into normalized floating point values [0,1].

                    This representation is commonly used in image
                    processing because convolution and filtering
                    operate more naturally in floating point space.
                */
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

        /*
            The output image is created with 4 channels (RGBA).
            The alpha channel is always set to 255 (fully opaque).
        */
        Image result(width, height, 4);

        /*
            Helper used to clamp floating point values
            to the valid normalized range [0,1].

            This is necessary because image processing
            operations (blur, sharpening, convolution, etc.)
            can easily generate values outside the valid range.
        */
        auto clamp01 = [](float v) {
            return std::clamp(v, 0.0f, 1.0f);
        };

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                /*
                    Convert normalized float channels back
                    to 8-bit integer color channels.

                    Steps:
                    1. Clamp value to [0,1]
                    2. Scale to [0,255]
                    3. Cast to uint8_t
                */

                auto r = static_cast<uint8_t>(clamp01(fimg.r(x, y)) * 255.0f);
                auto g = static_cast<uint8_t>(clamp01(fimg.g(x, y)) * 255.0f);
                auto b = static_cast<uint8_t>(clamp01(fimg.b(x, y)) * 255.0f);

                /*
                    Alpha is fixed to 255 because FloatImageRGB
                    does not store transparency information.
                */
                result.SetPixel(x, y, r, g, b, 255);
            }

        return result;
    }
}
