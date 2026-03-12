#include "../../include/Core/Image.hpp"

#include "../../include/Filters/Blur.hpp"
#include "../../include/Math/Convolution.hpp"
#include "../../include/Tools/Profiler.hpp"
#include "../../thirdparty/stb_image.h"

namespace Editor
{
    Image::Image(std::filesystem::path filePath, int width, int height, int channels = 4) :
        m_data(width, height, channels), m_path(std::move(filePath)) {}

    Image::Image(int width, int height, int channels)
    {
        m_data.width = width;
        m_data.height = height;
        m_data.channels = channels;
        m_data.pixels.resize(width * height);
    }

    Image::Image(const Image& other)
    {
        m_data.width = other.m_data.width;
        m_data.height = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels = other.m_data.pixels;
    }

    Image::Image(Image&& other) noexcept
    {
        m_data.width = other.m_data.width;
        m_data.height = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels = other.m_data.pixels;
    }

    Image& Image::operator=(const Image& other)
    {
        if (this == &other)
            return *this;

        m_data.width    = other.m_data.width;
        m_data.height   = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels   = other.m_data.pixels;
        m_path = other.m_path;

        return *this;
    }

    Image& Image::operator=(Image &&other) noexcept
    {
        if (this == &other) return *this;

        m_data.width    = other.m_data.width;
        m_data.height   = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels   = std::move(other.m_data.pixels);
        m_path = std::move(other.m_path);

        other.m_data.width = 0;
        other.m_data.height = 0;
        other.m_data.channels = 0;

        return *this;
    }

    bool Image::LoadImage(const std::filesystem::path& filePath)
    {
        Tools::Profiler profiler("Image::LoadImage");
        int width, height, channels;

        unsigned char* data = stbi_load(
            filePath.string().c_str(),
            &width,
            &height,
            &channels,
            4
        );

        if (!data)
            return false;

        m_data.width = width;
        m_data.height = height;
        m_data.channels = 4;

        const size_t pixelCount = width * height;
        m_data.pixels.resize(pixelCount);

        std::memcpy(m_data.pixels.data(), data, width * height * 4);

        stbi_image_free(data);
        return true;
    }

    void Image::ToGrayScale()
    {
        for (auto& px : m_data.pixels)
        {
            auto gray = static_cast<uint8_t>(
                0.30 * px.GetR() +
                0.59 * px.GetG() +
                0.11 * px.GetB()
            );

            px.SetPixel(gray, gray, gray, px.GetA());
        }
    }

    void Image::Rotate()
    {
        int W = m_data.width;
        int H = m_data.height;

        std::vector<Pixel> newPixels(H * W);

        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
            {
                int newX = H - 1 - y;
                int newY = x;
                newPixels[newY * H + newX] = m_data.pixels[y * W + x];
            }

        m_data.pixels.swap(newPixels);
        std::swap(m_data.width, m_data.height);
    }

    void Image::FlipHorizontal()
    {
        int W = m_data.width;
        int H = m_data.height;

        std::vector<Pixel> newPixels(W * H);

        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
            {
                int newX = W - 1 - x;
                newPixels[y * W + newX] = m_data.pixels[y * W + x];
            }

        m_data.pixels.swap(newPixels);
    }

    void Image::FlipVertical()
    {
        int W = m_data.width;
        int H = m_data.height;
        std::vector<Pixel> newPixels(W * H);

        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
            {
                int newY = H - 1 - y;
                newPixels[newY * W + x] = m_data.pixels[y * W + x];
            }

        m_data.pixels.swap(newPixels);
    }

    void Image::ApplyFilter(Filter::FilterType type)
    {
        switch (type)
        {
            case Filter::FilterType::GrayScale:
                ToGrayScale();
                break;

            case Filter::FilterType::Blur:
                Filter::GaussianBlur(*this, 2.0f);
                break;

            case Filter::FilterType::Emboss:
                break;

            case Filter::FilterType::EdgeDetect:
                break;

            case Filter::FilterType::Invert:
                break;

            case Filter::FilterType::Sharpen:
                break;

        }
    }

    void Image::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (x < 0 || x >= m_data.width || y < 0 || y >= m_data.height)
            return;

        m_data.pixels[y * m_data.width + x].SetPixel(r, g, b, a);
    }
}