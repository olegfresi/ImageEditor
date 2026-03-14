#include "../../include/Filters/ColorInversion.hpp"

namespace Editor::Filter
{
    void InvertColor(Image& image)
    {
        image.ApplyTransformation([](Pixel& px) {
            px.SetPixel(255 - px.GetR(), 255 - px.GetG(), 255 - px.GetB(), px.GetA());
        });
    }
}