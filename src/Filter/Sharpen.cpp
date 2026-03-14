#include "../../include/Filters/Sharpen.hpp"
#include <algorithm>

namespace Editor::Filter
{
    void Sharpen(Image& image, std::span<const Pixel> backup)
    {
        const int width = image.GetWidth();
        const int height = image.GetHeight();
        int currentIndex = 0;

        image.ApplyTransformation([&](Pixel& px) {
            int x = currentIndex % width;
            int y = currentIndex / width;

            if(x > 0 && x < width - 1 && y > 0 && y < height - 1)
            {
                const Pixel& center = backup[y * width + x];
                const Pixel& north = backup[(y - 1) * width + x];
                const Pixel& south = backup[(y + 1) * width + x];
                const Pixel& west = backup[y * width + (x - 1)];
                const Pixel& east = backup[y * width + (x + 1)];

                int r = (center.GetR() * 5) - (north.GetR() + south.GetR() + west.GetR() + east.GetR());
                int g = (center.GetG() * 5) - (north.GetG() + south.GetG() + west.GetG() + east.GetG());
                int b = (center.GetB() * 5) - (north.GetB() + south.GetB() + west.GetB() + east.GetB());

                px.SetPixel(
                        static_cast<uint8_t>(std::clamp(r, 0, 255)),
                        static_cast<uint8_t>(std::clamp(g, 0, 255)),
                        static_cast<uint8_t>(std::clamp(b, 0, 255)),
                        center.GetA()
                        );
            }

            currentIndex++;
        });
    }
}