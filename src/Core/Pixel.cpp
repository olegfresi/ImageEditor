#include "../../include/Core/Pixel.hpp"

namespace Editor
{
    Pixel::Pixel() : Pixel(0, 0, 0, 0) {}
    Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : m_rgba{ Pack(r, g, b, a) } {}
    Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b) : Pixel{r, g, b, 1} {}
    Pixel::Pixel(const Pixel& other) { m_rgba = other.m_rgba; }

    bool Pixel::operator==(const Pixel& other) const { return other.m_rgba == m_rgba; }
    bool Pixel::operator!=(const Pixel& other) const {return !(*this == other); }

    void Pixel::SetPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_rgba = Pack(r, g, b, a);
    }

    constexpr uint32_t Pixel::Pack(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        return (uint32_t(r) << 24) |
               (uint32_t(g) << 16) |
               (uint32_t(b) << 8)  |
                uint32_t(a);
    }
}