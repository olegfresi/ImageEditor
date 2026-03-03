#include "../../include/Core/Pixel.hpp"

namespace Editor
{

    bool Pixel::operator==(const Pixel& other) const { return other.m_r == m_r &&
                                                              other.m_g == m_g &&
                                                              other.m_b == m_b &&
                                                              other.m_a == m_a; }

    bool Pixel::operator!=(const Pixel& other) const {return !(*this == other); }

    void Pixel::SetPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_r = r;
        m_g = g;
        m_b = b;
        m_a = a;
    }
}