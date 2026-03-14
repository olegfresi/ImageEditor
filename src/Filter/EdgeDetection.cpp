#include "../../include/Filters/EdgeDetection.hpp"

#include <algorithm>

namespace Editor::Filter
{
    void EdgeDetection(Image& image, std::span<const Pixel> backup)
    {
        const int width = image.GetWidth();
        const int height = image.GetHeight();
        int currentIndex = 0;

        image.ApplyTransformation([&](Pixel& px) {
            int x = currentIndex % width;
            int y = currentIndex / width;

            if(x > 0 && x < width - 1 && y > 0 && y < height - 1)
            {
                const Pixel& c = backup[y * width + x];

                int r = (c.GetR() * 8);
                int g = (c.GetG() * 8);
                int b = (c.GetB() * 8);

                for(int i = -1; i <= 1; ++i)
                    for(int j = -1; j <= 1; ++j)
                    {
                        if(i == 0 && j == 0)
                            continue;
                        const Pixel& neighbor = backup[(y + i) * width + (x + j)];
                        r -= neighbor.GetR();
                        g -= neighbor.GetG();
                        b -= neighbor.GetB();
                    }

                px.SetPixel(
                        static_cast<uint8_t>(std::clamp(r, 0, 255)),
                        static_cast<uint8_t>(std::clamp(g, 0, 255)),
                        static_cast<uint8_t>(std::clamp(b, 0, 255)),
                        255
                        );
            } else
                px.SetPixel(0, 0, 0, 255);

            currentIndex++;
        });
    }
}