#include "../../include/Filters/Blur.hpp"
#include "../../include/Math/Convolution.hpp"
#include "../../include/Core/Utils.hpp"
#include <cmath>


namespace Editor::Filter
{
    Image GaussianBlur(const Image& img, float sigma)
    {
        Utils::FloatImageRGB fimg = Utils::ImageToFloatRGB(img);

        auto kernel = GenerateGaussian1D(sigma);

        fimg.r = Math::ConvolveSeparable(fimg.r, kernel, kernel);
        fimg.g = Math::ConvolveSeparable(fimg.g, kernel, kernel);
        fimg.b = Math::ConvolveSeparable(fimg.b, kernel, kernel);

        return Utils::FloatToImageRGB(fimg);
    }

    constexpr std::vector<float> BoxFilter()
    {
        return std::vector{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
                            1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
                            1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f };
    }
}