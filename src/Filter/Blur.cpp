#include "../../include/Filters/Blur.hpp"
#include "../../include/Core/Utils.hpp"
#include <cmath>


namespace Editor::Filter
{
    std::vector<int> BoxesForGauss(float sigma, int n)
    {
        // Ideal width for the box blur kernel
        float wIdeal = std::sqrt((12.0f * sigma * sigma / static_cast<float>(n)) + 1.0f);
        int wl = static_cast<int>(std::floor(wIdeal));

        // Ensure kernel size is odd
        if(wl % 2 == 0)
            wl--;

        int wu = wl + 2;

        // Calculate how many of each box size to use
        const auto fn = static_cast<float>(n);
        const auto fwl = static_cast<float>(wl);

        float mIdeal = (12.0f * sigma * sigma - fn * fwl * fwl - 4.0f * fn * fwl - 3.0f * fn) / (-4.0f * fwl - 4.0f);
        int m = static_cast<int>(std::round(mIdeal));

        std::vector<int> sizes;
        sizes.reserve(n);
        for(int i = 0; i < n; i++)
            sizes.push_back(i < m ? wl : wu);

        return sizes;
    }

    void GaussianBlur(Image& img, float sigma)
    {
        const int w = img.GetWidth();
        const int h = img.GetHeight();
        auto pixels = img.GetPixelData();

        // 1. BUFFER PREPARATION
        std::vector<FloatRGB> scl(w * h);
        std::vector<FloatRGB> tcl(w * h);

        // 2. PARALLEL DATA CONVERSION (uint8 to float)
        Utils::Parallelize(h, [&](int y) {
            for(int x = 0; x < w; ++x)
            {
                int i = y * w + x;
                scl[i].r = static_cast<float>(pixels[i].GetR());
                scl[i].g = static_cast<float>(pixels[i].GetG());
                scl[i].b = static_cast<float>(pixels[i].GetB());
            }
        });

        // Calculate box sizes for Gaussian approximation
        auto bxs = BoxesForGauss(sigma, 3);

        // 3. PING-PONG BOX BLUR PASSES
        for(int i = 0; i < 3; i++)
        {
            int r = (bxs[i] - 1) / 2;
            if(r <= 0)
                continue;

            const float arr = 1.0f / static_cast<float>(r + r + 1);

            // --- Horizontal Pass (scl -> tcl) ---
            Utils::Parallelize(h, [&](int y) {
                int ti = y * w, li = ti, ri = ti + r;
                FloatRGB fv = scl[ti], lv = scl[ti + w - 1];

                // Initial accumulator sum
                float vr = static_cast<float>(r + 1) * fv.r;
                float vg = static_cast<float>(r + 1) * fv.g;
                float vb = static_cast<float>(r + 1) * fv.b;

                for(int j = 0; j < r; j++)
                {
                    vr += scl[ti + j].r;
                    vg += scl[ti + j].g;
                    vb += scl[ti + j].b;
                }

                // Phase 1: Left edge handling (clamping simulation)
                for(int j = 0; j <= r; j++)
                {
                    vr += scl[ri].r - fv.r;
                    vg += scl[ri].g - fv.g;
                    vb += scl[ri].b - fv.b;
                    tcl[ti++] = {vr * arr, vg * arr, vb * arr};
                    ri++;
                }
                // Phase 2: Core processing (fast path)
                for(int j = r + 1; j < w - r; j++)
                {
                    vr += scl[ri].r - scl[li].r;
                    vg += scl[ri].g - scl[li].g;
                    vb += scl[ri].b - scl[li].b;
                    tcl[ti++] = {vr * arr, vg * arr, vb * arr};
                    ri++;
                    li++;
                }
                // Phase 3: Right edge handling (clamping simulation)
                for(int j = w - r; j < w; j++)
                {
                    vr += lv.r - scl[li].r;
                    vg += lv.g - scl[li].g;
                    vb += lv.b - scl[li].b;
                    tcl[ti++] = {vr * arr, vg * arr, vb * arr};
                    li++;
                }
            });

            // --- Vertical Pass (tcl -> scl) ---
            Utils::Parallelize(w, [&](int x) {
                int ti = x, li = ti, ri = ti + r * w;
                FloatRGB fv = tcl[ti], lv = tcl[ti + w * (h - 1)];

                float vr = static_cast<float>(r + 1) * fv.r;
                float vg = static_cast<float>(r + 1) * fv.g;
                float vb = static_cast<float>(r + 1) * fv.b;

                for(int j = 0; j < r; j++)
                {
                    vr += tcl[ti + j * w].r;
                    vg += tcl[ti + j * w].g;
                    vb += tcl[ti + j * w].b;
                }
                for(int j = 0; j <= r; j++)
                {
                    vr += tcl[ri].r - fv.r;
                    vg += tcl[ri].g - fv.g;
                    vb += tcl[ri].b - fv.b;
                    scl[ti] = {vr * arr, vg * arr, vb * arr};
                    ri += w;
                    ti += w;
                }
                for(int j = r + 1; j < h - r; j++)
                {
                    vr += tcl[ri].r - tcl[li].r;
                    vg += tcl[ri].g - tcl[li].g;
                    vb += tcl[ri].b - tcl[li].b;
                    scl[ti] = {vr * arr, vg * arr, vb * arr};
                    li += w;
                    ri += w;
                    ti += w;
                }
                for(int j = h - r; j < h; j++)
                {
                    vr += lv.r - tcl[li].r;
                    vg += lv.g - tcl[li].g;
                    vb += lv.b - tcl[li].b;
                    scl[ti] = {vr * arr, vg * arr, vb * arr};
                    li += w;
                    ti += w;
                }
            });
        }

        // 4. PARALLEL BACK-CONVERSION (float to uint8)
        Utils::Parallelize(h, [&](int y) {
            for(int x = 0; x < w; ++x)
            {
                int i = y * w + x;
                pixels[i].SetPixel(
                        static_cast<uint8_t>(std::clamp(scl[i].r, 0.0f, 255.0f)),
                        static_cast<uint8_t>(std::clamp(scl[i].g, 0.0f, 255.0f)),
                        static_cast<uint8_t>(std::clamp(scl[i].b, 0.0f, 255.0f)),
                        255
                        );
            }
        });
    }
}