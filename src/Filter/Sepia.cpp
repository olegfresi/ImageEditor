#include "../../include/Filters/Sepia.hpp"


namespace Editor::Filter
{
    void Sepia(Image& img)
    {
        auto pixelSpan = img.GetPixelData();
        for(auto& pixel : pixelSpan)
        {
            uint8_t originalRed = pixel.GetR();
            uint8_t originalGreen = pixel.GetG();
            uint8_t originalBlue = pixel.GetB();
            uint8_t originalAlpha = pixel.GetA();

            int newRed = std::min(255, static_cast<int>(originalRed * 0.393 + originalGreen * 0.769 + originalBlue * 0.189));
            int newGreen = std::min(255, static_cast<int>(originalRed * 0.349 + originalGreen * 0.686 + originalBlue * 0.168));
            int newBlue = std::min(255, static_cast<int>(originalRed * 0.272 + originalGreen * 0.534 + originalBlue * 0.131));

            pixel.SetPixel(static_cast<uint8_t>(newRed), static_cast<uint8_t>(newGreen),
                            static_cast<uint8_t>(newBlue), originalAlpha);
        }
    }
}