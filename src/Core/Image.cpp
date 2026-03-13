#include "../../include/Core/Image.hpp"


namespace Editor
{
    Image::Image(int width, int height, int channels = 4) : m_data(width, height, channels) {}

    Image::Image(int width, int height, int channels, std::vector<Pixel> pixels) :
        m_data{width, height, channels, std::move(pixels)} {}

    Image::Image(const Image& other) = default;

    Image::Image(Image&& other) noexcept = default;

    Image& Image::operator=(const Image& other)
    {
        if (this == &other)
            return *this;

        m_data.width    = other.m_data.width;
        m_data.height   = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels   = other.m_data.pixels;

        return *this;
    }

    Image& Image::operator=(Image &&other) noexcept
    {
        if (this == &other)
            return *this;

        m_data.width    = other.m_data.width;
        m_data.height   = other.m_data.height;
        m_data.channels = other.m_data.channels;
        m_data.pixels   = std::move(other.m_data.pixels);

        other.m_data.width = 0;
        other.m_data.height = 0;
        other.m_data.channels = 0;

        return *this;
    }

    void Image::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        if (x < 0 || x >= m_data.width || y < 0 || y >= m_data.height)
            return;

        m_data.pixels[y * m_data.width + x].SetPixel(r, g, b, a);
    }

    void Image::ApplyTransformation(const PixelTransformer& transformer)
    {
        for(auto& px : m_data.pixels)
            transformer(px);
    }

    void Image::ApplyIndexedTransformation(const IndexedPixelTransformer& transformer)
    {
        for (int y = 0; y < m_data.height; ++y)
            for (int x = 0; x < m_data.width; ++x)
                transformer(m_data.pixels[y * m_data.width + x], x, y);
    }
}