#include <algorithm>
#include <cassert>
#include <vector>
#include <thread>
#include "../../include/Math/Convolution.hpp"
#include "../../include/Tools/Profiler.hpp"


namespace Editor::Math
{
    constexpr Matrix<float> Convolve2D(const Matrix<float>& input, const Matrix<float>& kernel)
    {
        int width  = input.GetWidth();
        int height = input.GetHeight();

        int kWidth  = kernel.GetWidth();
        int kHeight = kernel.GetHeight();

        assert(kWidth % 2 == 1 && kHeight % 2 == 1);

        Matrix<float> output{width, height};

        int kCenterX = kWidth / 2;
        int kCenterY = kHeight / 2;

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
            {
                float sum = 0.0f;

                for (int j = 0; j < kHeight; ++j)
                {
                    for (int i = 0; i < kWidth; ++i)
                    {
                        int nx = x + i - kCenterX;
                        int ny = y + j - kCenterY;

                        nx = std::clamp(nx, 0, width  - 1);
                        ny = std::clamp(ny, 0, height - 1);

                        sum += input(nx, ny) * kernel(i, j);
                    }
                }

                output(x, y) = sum;
            }

        return output;
    }

    constexpr std::vector<float> Convolve1D(const std::vector<float>& input, const std::vector<float>& kernel)
    {
        int size = static_cast<int>(input.size());
        int kSize = static_cast<int>(kernel.size());

        assert(kSize % 2 == 1);
        std::vector<float> output(size);
        int kCenter = kSize / 2;

        for (int x = 0; x < size; ++x)
        {
            float sum = 0.0f;

            for (int i = 0; i < kSize; ++i)
            {
                int nx = x + i - kCenter;
                nx = std::clamp(nx, 0, size - 1);
                sum += input[nx] * kernel[i];
            }

            output[x] = sum;
        }

        return output;
    }

    Matrix<float> ConvolveSeparable(const Matrix<float>& input, const std::vector<float>& kernelX, const std::vector<float>& kernelY)
    {
        Tools::Profiler p{"Convolve Separable" };
        int width  = input.GetWidth();
        int height = input.GetHeight();

        assert(kernelX.size() % 2 == 1);
        assert(kernelY.size() % 2 == 1);

        Matrix<float> temp{width, height};
        Matrix<float> output{width, height};

        int kCenterX = static_cast<int>(kernelX.size() / 2);
        int kCenterY = static_cast<int>(kernelY.size() / 2);

        unsigned int numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;

        auto parallelLambda = [&](auto workFunc) {
            std::vector<std::thread> threads;
            threads.reserve(numThreads);
            for (unsigned int t = 0; t < numThreads; ++t)
                {
                    threads.emplace_back([t, numThreads, height, workFunc]() {
                        for (size_t y = t; y < height; y += numThreads)
                            workFunc(y);
                });
            }
            for (auto& thread : threads) thread.join();
        };

        parallelLambda([&](int y) {
            for (int x = 0; x < width; ++x)
            {
                float sum = 0.0f;
                for (int k = 0; k < kernelX.size(); ++k)
                {
                    int nx = std::clamp(x + k - kCenterX, 0, width - 1);
                    sum += input(nx, y) * kernelX[k];
                }
                temp(x, y) = sum;
            }
        });

        parallelLambda([&](int y) {
            for (int x = 0; x < width; ++x)
            {
                float sum = 0.0f;
                for (int k = 0; k < kernelY.size(); ++k)
                {
                    int ny = std::clamp(y + k - kCenterY, 0, height - 1);
                    sum += temp(x, ny) * kernelY[k];
                }
                output(x, y) = sum;
            }
        });

        return output;
    }
}