#include "../../include/Math/Gaussian.hpp"
#include <cmath>
#include <cassert>

namespace Editor::Math
{
    constexpr std::vector<float> GenerateGaussian1D(float sigma)
    {
        assert(sigma > 0.0f);

        int radius = static_cast<int>(std::ceil(3.0f * sigma));
        int size = 2 * radius + 1;

        std::vector<float> kernel(size);

        float sigma2 = sigma * sigma;
        float normalizationFactor = 1.0f / std::sqrt(2.0f * 3.14159265359f * sigma2);

        float sum = 0.0f;

        for (int i = 0; i < size; ++i)
        {
            int x = i - radius;
            float value = normalizationFactor * std::exp(static_cast<float>(-(x * x)) / (2.0f * sigma2));

            kernel[i] = value;
            sum += value;
        }

        for (float& v : kernel)
            v /= sum;

        return kernel;
    }

    constexpr Matrix<float> GenerateGaussian2D(float sigma)
    {

    }
}