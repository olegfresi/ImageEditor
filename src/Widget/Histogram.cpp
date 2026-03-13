#include "../../include/Widget/Histogram.hpp"
#include <algorithm>


namespace Editor::Widget
{
    HistogramWidget::HistogramWidget()
    {
        set_content_height(200);
        set_draw_func(sigc::mem_fun(*this, &HistogramWidget::DrawHistogram));
    }

    std::array<float, 256> HistogramWidget::GetLuminanceHistogram() const
    {
        std::array<float, 256> lum{};
        float maxv = 0.0f;

        for(int i = 0; i < 256; i++)
        {
            lum[i] = 0.2126f * static_cast<float>(m_r[i]) +
                     0.7152f * static_cast<float>(m_g[i]) +
                     0.0722f * static_cast<float>(m_b[i]);
            maxv = std::max(maxv, lum[i]);
        }

        if(maxv > 0)
            for(auto& v : lum)
                v /= maxv;

        return lum;
    }

    void HistogramWidget::UpdateFromBuffer(std::span<const Pixel> pixels, size_t stride)
    {
        // 1. Reset histograms
        m_r.fill(0);
        m_g.fill(0);
        m_b.fill(0);

        // 2. Safety check: spans handle empty data gracefully
        if (pixels.empty()) return;

        // 3. Setup pointers
        // .data() on a span returns the pointer to the first element
        const Pixel* data = pixels.data();
        const size_t total = pixels.size();

        // Safety check: ensure stride is at least 1 to avoid infinite loop
        size_t actual_stride = (stride < 1) ? 1 : stride;

        const uint8_t* p = reinterpret_cast<const uint8_t*>(data);

        // 4. Computation loop
        // Each pixel is 4 bytes (RGBA). We multiply stride by 4 to jump correctly.
        for(size_t i = 0; i < total * 4; i += 4 * actual_stride)
        {
            m_r[p[i]]++;     // Red
            m_g[p[i + 1]]++; // Green
            m_b[p[i + 2]]++; // Blue
        }

        // 5. UI and Signal update
        queue_draw();
        m_signalHistogramUpdated.emit();
    }

    void HistogramWidget::DrawHistogram(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) const
    {
        uint32_t max_v = 0;

        // 1. ADAPTIVE SMOOTHING FUNCTION
        // To fix the "combing" effect, we increase the smoothing radius
        // This fills the gaps created by the tone curve stretching
        auto get_smoothed = [&](const std::array<uint32_t, 256>& raw, int i) {
            double sum = 0;
            double weight_sum = 0;
            int radius = 4; // Increased radius to fill "comb" gaps

            for(int j = -radius; j <= radius; j++)
            {
                int idx = std::clamp(i + j, 0, 255);
                // Gaussian-like weight: central values count more than edges
                double weight = 1.0 / (1.0 + std::abs(j));
                sum += raw[idx] * weight;
                weight_sum += weight;
            }
            return sum / weight_sum;
        };

        // Find max for normalization (based on smoothed data)
        for(int i = 0; i < 256; i++)
        {
            double r_val = get_smoothed(m_r, i);
            double g_val = get_smoothed(m_g, i);
            double b_val = get_smoothed(m_b, i);
            max_v = std::max({max_v, static_cast<uint32_t>(r_val), static_cast<uint32_t>(g_val), static_cast<uint32_t>(b_val)});
        }

        if(max_v == 0)
            return;

        // Background
        cr->set_source_rgb(0.04, 0.04, 0.04);
        cr->paint();

        auto draw_curved_ch = [&](const std::array<uint32_t, 256>& raw, double r, double g, double b) {
            cr->set_source_rgba(r, g, b, 0.4);
            cr->move_to(0, height);

            double last_x = 0;
            double last_y = height - (get_smoothed(raw, 0) / max_v * height);
            cr->line_to(last_x, last_y);

            // MIDPOINT INTERPOLATION (The secret for smooth Darkroom curves)
            for(int i = 1; i < 256; i++)
            {
                double x = (static_cast<double>(i) / 255.0) * width;
                double y = height - (get_smoothed(raw, i) / max_v * height);

                double mid_x = (last_x + x) / 2.0;
                double mid_y = (last_y + y) / 2.0;

                // Use the control point at (last_x, last_y) to round the corner
                cr->curve_to(last_x, last_y, last_x, last_y, mid_x, mid_y);

                last_x = x;
                last_y = y;
            }

            cr->line_to(width, height);
            cr->close_path();
            cr->fill();
        };

        // Render using additive blending for professional look
        cr->set_operator(Cairo::Context::Operator::ADD);
        draw_curved_ch(m_r, 0.8, 0.0, 0.0);
        draw_curved_ch(m_g, 0.0, 0.8, 0.0);
        draw_curved_ch(m_b, 0.0, 0.0, 0.8);
        cr->set_operator(Cairo::Context::Operator::OVER);
    }

    void HistogramWidget::SetImage(std::span<const Pixel> pixels, int width, int height, int step)
    {
        UpdateFromBuffer(pixels, step);
    }
}