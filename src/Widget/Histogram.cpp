#include "../../include/Widget/Histogram.hpp"
#include <algorithm>


namespace Editor::Widget
{
    HistogramWidget::HistogramWidget()
    {
        set_content_height(120);
        set_content_width(140);
        set_draw_func(sigc::mem_fun(*this, &HistogramWidget::DrawHistogram));
    }

    std::array<float, 256> HistogramWidget::GetLuminanceHistogram() const
    {
        std::array<float, 256> lum{};
        float maxValue = 0.0f;

        for(int i = 0; i < 256; i++)
        {
            lum[i] = 0.2126f * static_cast<float>(m_r[i]) +
                     0.7152f * static_cast<float>(m_g[i]) +
                     0.0722f * static_cast<float>(m_b[i]);
            maxValue = std::max(maxValue, lum[i]);
        }

        if(maxValue > 0)
            for(auto& v : lum)
                v /= maxValue;

        return lum;
    }

    void HistogramWidget::UpdateFromBuffer(std::span<const Pixel> pixels, size_t stride)
    {
        m_r.fill(0);
        m_g.fill(0);
        m_b.fill(0);
        m_lum.fill(0);

        m_hasShadowClipping = false;
        m_hasHighlightClipping = false;

        if(pixels.empty())
            return;

        const auto* p = reinterpret_cast<const uint8_t*>(pixels.data());
        size_t total_bytes = pixels.size() * 4;
        size_t actual_stride = (stride < 1) ? 1 : stride;

        for(size_t i = 0; i < total_bytes; i += 4 * actual_stride)
        {
            uint8_t r = p[i];
            uint8_t g = p[i + 1];
            uint8_t b = p[i + 2];

            m_r[r]++;
            m_g[g]++;
            m_b[b]++;

            // Luminance (Y = 0.2126R + 0.7152G + 0.0722B)
            uint32_t y = (218 * r + 732 * g + 74 * b) >> 10;
            y = std::min(y, 255u);
            m_lum[y]++;


            if(y == 0)
                m_hasShadowClipping = true;
            if(y == 255)
                m_hasHighlightClipping = true;
        }

        queue_draw();
    }

    void HistogramWidget::DrawHistogram(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) const
    {
        // 1. DATA SELECTION
        // Identify which data array to use based on the current mode
        const std::array<uint32_t, 256>* current_data = nullptr;
        double r_col = 0.9, g_col = 0.9, b_col = 0.9;

        switch(m_currentMode)
        {
            case ChannelMode::Red:
                current_data = &m_r;
                r_col = 0.9;
                g_col = 0.1;
                b_col = 0.1;
                break;
            case ChannelMode::Green:
                current_data = &m_g;
                r_col = 0.1;
                g_col = 0.9;
                b_col = 0.1;
                break;
            case ChannelMode::Blue:
                current_data = &m_b;
                r_col = 0.1;
                g_col = 0.1;
                b_col = 0.9;
                break;
            case ChannelMode::Luminance:
                current_data = &m_lum;
                r_col = 0.8;
                g_col = 0.8;
                b_col = 0.8;
                break;
            case ChannelMode::Rgb:
                current_data = nullptr;
                break;
        }

        // 2. ADAPTIVE SMOOTHING
        // Reduces "noise" and "gaps" in the histogram for a smoother look
        auto get_smoothed = [&](const std::array<uint32_t, 256>& raw, int i) {
            double sum = 0, weight_sum = 0;
            constexpr int radius = 4;
            for(int j = -radius; j <= radius; j++)
            {
                int idx = std::clamp(i + j, 0, 255);
                double weight = 1.0 / (1.0 + std::abs(j));
                sum += raw[idx] * weight;
                weight_sum += weight;
            }
            return sum / weight_sum;
        };

        // 3. CHANNEL-SPECIFIC NORMALIZATION
        // We calculate a local maximum to ensure the histogram fills the widget height
        double local_max = 0;
        if(m_currentMode == ChannelMode::Rgb)
        {
            for(int i = 0; i < 256; i++)
                local_max = std::max({local_max, get_smoothed(m_r, i), get_smoothed(m_g, i), get_smoothed(m_b, i)});
        } else if(current_data)
        {
            for(int i = 0; i < 256; i++)
                local_max = std::max(local_max, get_smoothed(*current_data, i));
        }

        if(local_max <= 0)
            return;

        // 4. BACKGROUND RENDERING
        cr->set_source_rgb(0.04, 0.04, 0.04);
        cr->paint();

        // 5. DRAWING HELPER LAMBDA
        // Draws the filled curve with midpoint interpolation
        auto draw_curved_ch = [&](const std::array<uint32_t, 256>& raw, double r, double g, double b, double a) {
            cr->set_source_rgba(r, g, b, a);
            cr->move_to(0, height);
            double last_x = 0;
            double last_y = height - (get_smoothed(raw, 0) / local_max * height);

            for(int i = 1; i < 256; i++)
            {
                double x = (static_cast<double>(i) / 255.0) * width;
                double y = height - (get_smoothed(raw, i) / local_max * height);
                double mid_x = (last_x + x) / 2.0;
                double mid_y = (last_y + y) / 2.0;
                cr->curve_to(last_x, last_y, last_x, last_y, mid_x, mid_y);
                last_x = x;
                last_y = y;
            }
            cr->line_to(width, height);
            cr->close_path();
            cr->fill();
        };

        // 6. MAIN CONTENT RENDERING
        if(m_currentMode == ChannelMode::Rgb)
        {
            cr->set_operator(Cairo::Context::Operator::ADD);
            draw_curved_ch(m_r, 0.8, 0.0, 0.0, 0.4);
            draw_curved_ch(m_g, 0.0, 0.8, 0.0, 0.4);
            draw_curved_ch(m_b, 0.0, 0.0, 0.8, 0.4);
        } else if(current_data)
        {
            double alpha = 0.6;
            draw_curved_ch(*current_data, r_col, g_col, b_col, alpha);
        }

        // Switch back to default operator for indicators
        cr->set_operator(Cairo::Context::Operator::OVER);

        // 7. CLIPPING INDICATORS DRAWING
        // Helper to draw a glowing point in the corner
        auto draw_clipping_point = [&](double x, double y, double r, double g, double b, bool active) {
            if(active)
            {
                cr->set_source_rgb(r, g, b);
                cr->arc(x, y, 4, 0, 2 * M_PI);
                cr->fill();
                cr->set_source_rgba(r, g, b, 0.3); // Glow effect
                cr->set_line_width(2.0);
                cr->arc(x, y, 7, 0, 2 * M_PI);
                cr->stroke();
            } else
            {
                cr->set_source_rgb(0.15, 0.15, 0.15); // Dimmed state
                cr->arc(x, y, 3, 0, 2 * M_PI);
                cr->fill();
            }
        };

        constexpr double padding = 15.0;
        // Shadow Indicator (Left) - Uses Cyan
        draw_clipping_point(padding, padding, 0.0, 0.8, 0.9, m_hasShadowClipping);
        // Highlight Indicator (Right) - Uses Red
        draw_clipping_point(width - padding, padding, 1.0, 0.2, 0.2, m_hasHighlightClipping);
    }

    void HistogramWidget::SetImage(std::span<const Pixel> pixels, int step)
    {
        UpdateFromBuffer(pixels, step);
    }
}