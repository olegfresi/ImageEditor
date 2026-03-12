#include <algorithm>
#include <cassert>
#include <vector>
#include <thread>
#include "../../include/Math/Convolution.hpp"
#include "../../include/Tools/Profiler.hpp"

namespace Editor::Math
{
    constexpr Matrix<float> Convolve2D(const Matrix<float> &input, const Matrix<float> &kernel)
    {
        int width = static_cast<int>(input.GetWidth());
        int height = static_cast<int>(input.GetHeight());

        int kWidth = static_cast<int>(kernel.GetWidth());
        int kHeight = static_cast<int>(kernel.GetHeight());

        assert(kWidth % 2 == 1 && kHeight % 2 == 1);

        Matrix<float> output{width, height};

        int kCenterX = kWidth / 2;
        int kCenterY = kHeight / 2;

        for(int y = 0; y < height; ++y)
            for(int x = 0; x < width; ++x)
            {
                float sum = 0.0f;

                for(int j = 0; j < kHeight; ++j)
                    for(int i = 0; i < kWidth; ++i)
                    {
                        int nx = x + i - kCenterX;
                        int ny = y + j - kCenterY;

                        // Clamp coordinates so that convolution replicates border pixels
                        nx = std::clamp(nx, 0, width - 1);
                        ny = std::clamp(ny, 0, height - 1);

                        sum += input(nx, ny) * kernel(i, j);
                    }

                output(x, y) = sum;
            }

        return output;
    }

    constexpr std::vector<float> Convolve1D(const std::vector<float> &input, const std::vector<float> &kernel)
    {
        int size = static_cast<int>(input.size());
        int kSize = static_cast<int>(kernel.size());

        assert(kSize % 2 == 1);

        std::vector<float> output(size);

        int kCenter = kSize / 2;

        for(int x = 0; x < size; ++x)
        {
            float sum = 0.0f;

            for(int i = 0; i < kSize; ++i)
            {
                int nx = x + i - kCenter;

                // Replicate borders (same strategy used in the 2D convolution)
                nx = std::clamp(nx, 0, size - 1);

                sum += input[nx] * kernel[i];
            }

            output[x] = sum;
        }

        return output;
    }

    Matrix<float> ConvolveSeparable(const Matrix<float> &input, const std::vector<float> &kernelX,
                                    const std::vector<float> &kernelY)
    {
        int width = static_cast<int>(input.GetWidth());
        int height = static_cast<int>(input.GetHeight());

        int kernelXSize = static_cast<int>(kernelX.size());
        int kernelYSize = static_cast<int>(kernelY.size());

        assert(kernelXSize % 2 == 1);
        assert(kernelYSize % 2 == 1);

        Matrix<float> temp{width, height};
        Matrix<float> output{width, height};

        auto kCenterX = kernelXSize / 2;
        auto kCenterY = kernelYSize / 2;

        unsigned int numThreads = std::thread::hardware_concurrency();
        if(numThreads == 0) numThreads = 4;

        /*
            Generic helper used to parallelize row-based work.

            Each thread processes a contiguous block of rows.
            This is better for cache locality than interleaving rows
            (e.g. y += numThreads), because each thread walks through
            memory sequentially.
        */
        auto parallelLambda = [&](auto workFunc) {
            std::vector<std::thread> threads;
            threads.reserve(numThreads);

            for(int t = 0; t < numThreads; ++t)
            {
                threads.emplace_back([t, numThreads, height, workFunc]() {
                    int rowsPerThread = height / static_cast<int>(numThreads);
                    int start = t * rowsPerThread;

                    // Last thread handles the remainder
                    int end = (t == numThreads - 1)
                                  ? height
                                  : start + rowsPerThread;

                    for(int y = start; y < end; ++y)
                        workFunc(y);
                });
            }

            for(auto &thread : threads)
                thread.join();
        };

        /*
            Horizontal convolution pass.

            We split the image into:
            - interior region (no clamp needed)
            - border region (requires clamp)

            Removing clamp from the hot path significantly reduces
            branching and improves vectorization opportunities.
        */
        parallelLambda([&](int y) {
            for(int x = 0; x < width; ++x)
            {
                float sum = 0.0f;

                // Fast path: interior pixels
                if(x >= kCenterX && x < width - kCenterX)
                {
                    for(int k = 0; k < kernelXSize; ++k)
                    {
                        int nx = x + k - kCenterX;
                        sum += input(nx, y) * kernelX[k];
                    }
                }
                else
                {
                    // Border path: clamp coordinates
                    for(int k = 0; k < kernelXSize; ++k)
                    {
                        int nx = std::clamp(x + k - kCenterX, 0, width - 1);
                        sum += input(nx, y) * kernelX[k];
                    }
                }

                temp(x, y) = sum;
            }
        });

        /*
            Vertical convolution pass.
            Same interior/border optimization as the horizontal pass.
        */
        parallelLambda([&](int y) {
            bool interior = (y >= kCenterY && y < height - kCenterY);

            for(int x = 0; x < width; ++x)
            {
                float sum = 0.0f;

                if(interior)
                {
                    // Fast path (no clamp)
                    for(int k = 0; k < kernelYSize; ++k)
                    {
                        int ny = y + k - kCenterY;
                        sum += temp(x, ny) * kernelY[k];
                    }
                }
                else
                {
                    // Border handling
                    for(int k = 0; k < kernelYSize; ++k)
                    {
                        int ny = std::clamp(y + k - kCenterY, 0, height - 1);
                        sum += temp(x, ny) * kernelY[k];
                    }
                }

                output(x, y) = sum;
            }
        });

        return output;
    }
}
