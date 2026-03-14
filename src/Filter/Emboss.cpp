#include "../../include/Filters/Emboss.hpp"
#include <algorithm>


namespace Editor::Filter
{
    void Emboss(Image& image, std::span<const Pixel> backup)
    {
        const int width = image.GetWidth();
        const int height = image.GetHeight();

        int currentIndex = 0;

        image.ApplyTransformation([&](Pixel& px) {
            int x = currentIndex % width;
            int y = currentIndex / width;

            if(x > 0 && x < width - 1 && y > 0 && y < height - 1)
            {
                const Pixel& pNW = backup[(y - 1) * width + (x - 1)];
                const Pixel& pSE = backup[(y + 1) * width + (x + 1)];

                int r = (pSE.GetR() - pNW.GetR()) + 128;
                int g = (pSE.GetG() - pNW.GetG()) + 128;
                int b = (pSE.GetB() - pNW.GetB()) + 128;

                px.SetPixel(
                        static_cast<uint8_t>(std::clamp(r, 0, 255)),
                        static_cast<uint8_t>(std::clamp(g, 0, 255)),
                        static_cast<uint8_t>(std::clamp(b, 0, 255)),
                        255
                        );
            }

            currentIndex++;
        });
    }
}