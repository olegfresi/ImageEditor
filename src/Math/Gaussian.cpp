#include "../../include/Math/Gaussian.hpp"
#include <cmath>
#include <numbers>
#include <cassert>

namespace Editor::Math
{
    std::vector<float> GenerateGaussian1D(float sigma)
    {
        assert(sigma > 0.0f);

        /*
            The Gaussian theoretically extends to infinity, but in practice
            values beyond 3*sigma are extremely close to zero.

            Using a radius of 3*sigma captures ~99.7% of the Gaussian energy
            while keeping the kernel small and efficient.
        */
        int radius = static_cast<int>(std::ceil(3.0f * sigma));
        int size   = 2 * radius + 1;

        std::vector<float> kernel(size);

        float sigma2 = sigma * sigma;

        /*
            Precompute constants used in the Gaussian function:

            G(x) = (1 / √(2πσ²)) * exp(-(x²) / (2σ²))

            Precomputing these values avoids repeated divisions
            inside the main loop.
        */
        constexpr auto PI = std::numbers::pi;

        double normalizationFactor = 1.0 / std::sqrt(2.0 * PI * sigma2);
        double invTwoSigma2        = 1.0 / (2.0 * sigma2);

        float sum = 0.0f;

        for (int i = 0; i < size; ++i)
        {
            int x = i - radius;

            /*
                Compute the Gaussian value for position x.
                The kernel is centered at radius.
            */
            double value = normalizationFactor *
                          std::exp(-(static_cast<float>(x * x)) * invTwoSigma2);

            kernel[i] = static_cast<float>(value);
            sum += static_cast<float>(value);
        }

        /*
            Normalize the kernel so that the sum of all weights = 1.

            This guarantees that applying the filter does not change
            the overall brightness of the image.
        */
        for (float& v : kernel)
            v /= sum;

        return kernel;
    }

    Matrix<float> GenerateGaussian2D(float sigma)
    {
        /*
            A 2D Gaussian is separable if G(x,y) = G(x) * G(y)

            Therefore we can build the 2D kernel as the outer
            product of two 1D Gaussian kernels.
        */

        auto kernel1D = GenerateGaussian1D(sigma);
        int size = static_cast<int>(kernel1D.size());
        Matrix<float> kernel(size, size);

        /*
            Compute the outer product of the 1D kernel with itself.

            Each cell becomes: kernel(x,y) = G(x) * G(y)

            The resulting matrix is already normalized because
            the 1D kernel is normalized.
        */
        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
                kernel(x, y) = kernel1D[x] * kernel1D[y];

        return kernel;
    }
}