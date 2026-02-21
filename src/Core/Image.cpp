#include "../../include/Core/Image.hpp"
#include "../../thirdparty/stb_image.h"
#include <iostream>

namespace Editor
{
    Image::Image(std::filesystem::path filePath, int width, int height, int channels = 4)
    {
        m_data.width = width;
        m_data.height = height;
        m_data.channels = channels;
        m_filePath = filePath;
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
        m_data.width = std::move(other.m_data.width);
        m_data.height = std::move(other.m_data.height);
        m_data.channels = std::move(other.m_data.channels);
        m_data.pixels = std::move(other.m_data.pixels);
    }

    bool Image::LoadImage(std::filesystem::path filePath)
    {
        int width, height, channels;

        unsigned char* data = stbi_load(filePath.string().c_str(), &width, &height, &channels, 0);
        if (!data)
        {
            std::cerr << "Errore loading image: " << filePath << std::endl;
            return false;
        }

        m_data.width = width;
        m_data.height = height;
        m_data.channels = channels;

        m_data.pixels.clear();
        m_data.pixels.reserve(width * height);

        for (int i = 0; i < width * height; ++i)
        {
            Pixel px{};
            px.SetPixel(data[i * channels + 0],
                        data[i * channels + 1],
                        data[i * channels + 2],
                        channels == 4 ? data[i * channels + 3] : 255);

            m_data.pixels.push_back(px);
        }

        stbi_image_free(data);
        return true;
    }

    void Image::ToGrayScale()
    {
        for (auto& px : m_data.pixels)
        {
            uint8_t gray = static_cast<uint8_t>(
                0.299 * px.GetR() +
                0.587 * px.GetG() +
                0.114 * px.GetB()
            );

            px.SetPixel(gray, gray, gray, px.GetA());
        }
    }

    void Image::ApplyFilter(FilterType type)
    {
        switch (type)
        {
            case FilterType::GrayScale:
                ToGrayScale();
                break;

            default:
                break;
        }
    }

    void Image::ToRGB()
    {

    }

    void Image::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {

    }

    void Image::Undo()
    {

    }

    void Image::Redo()
    {

    }
}