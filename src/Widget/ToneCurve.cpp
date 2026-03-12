#include "../../include/Widget/ToneCurve.hpp"
#include <algorithm>
#include <cmath>

namespace Editor::Widget
{
    ToneCurveWidget::ToneCurveWidget()
    {
        set_size_request(256, 256);

        // Initial state: perfect diagonal
        m_points = {{0.0, 0.0}, {1.0, 1.0}};

        m_curveCache.resize(256);
        UpdateCurveCache();

        // Setup drag gesture for control points
        auto drag = Gtk::GestureDrag::create();
        drag->signal_drag_begin().connect(sigc::mem_fun(*this, &ToneCurveWidget::OnPressed));
        drag->signal_drag_update().connect(sigc::mem_fun(*this, &ToneCurveWidget::OnDragUpdate));
        drag->signal_drag_end().connect([this](double, double) {
            m_selectedIdx = -1;
            m_signalDragFinished.emit();
            queue_draw();
        });
        add_controller(drag);

        set_draw_func(sigc::mem_fun(*this, &ToneCurveWidget::Draw));
    }

    sigc::signal<void()>& ToneCurveWidget::SignalCurveChanged()
    {
        return m_signalCurveChanged;
    }

    std::vector<uint8_t> ToneCurveWidget::GetLUT() const
    {
        std::vector<uint8_t> lut(256);
        for(int i = 0; i < 256; i++)
        {
            lut[i] = static_cast<uint8_t>(std::clamp(m_curveCache[i] * 255.0f, 0.0f, 255.0f));
        }
        return lut;
    }

    int ToneCurveWidget::FindPoint(double x, double y) const
    {
        constexpr double radius = 0.04; // Click tolerance in normalized coordinates
        for(int i = 0; i < static_cast<int>(m_points.size()); i++)
        {
            double dx = m_points[i].x - x;
            double dy = m_points[i].y - y;

            if(std::sqrt(dx * dx + dy * dy) < radius)
                return i;
        }
        return -1;
    }

    float ToneCurveWidget::CatmullRom(double t, double p0, double p1, double p2, double p3)
    {
        double t2 = t * t;
        double t3 = t2 * t;
        return 0.5f * static_cast<float>((2.0 * p1) +
                       (-p0 + p2) * t +
                       (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) * t2 +
                       (-p0 + 3.0 * p1 - 3.0 * p2 + p3) * t3);
    }

    void ToneCurveWidget::OnPressed(double x, double y)
    {
        const double w = get_allocated_width();
        const double h = get_allocated_height();

        double nx = std::clamp(x / w, 0.0, 1.0);
        double ny = std::clamp(1.0 - y / h, 0.0, 1.0);

        int idx = FindPoint(nx, ny);
        if(idx >= 0)
            m_selectedIdx = idx;

        else
        {
            // If click is near the curve, add a new point
            constexpr double tolerance = 0.05;
            int curveIdx = std::clamp(static_cast<int>(nx * 255.0), 0, 255);
            if(std::abs(m_curveCache[curveIdx] - ny) < tolerance)
            {
                m_points.push_back({nx, ny});
                std::ranges::sort(m_points.begin(), m_points.end(), [](auto& a, auto& b){ return a.x < b.x; });
                m_selectedIdx = FindPoint(nx, ny);
            }
        }

        m_dragStartX = nx;
        m_dragStartY = ny;

        UpdateCurveCache();
        queue_draw();
        m_signalCurveChanged.emit();
    }

    void ToneCurveWidget::OnDragUpdate(double dx, double dy)
    {
        if(m_selectedIdx < 0) return;

        double nx = std::clamp(m_dragStartX + dx / get_allocated_width(), 0.0, 1.0);
        double ny = std::clamp(m_dragStartY - dy / get_allocated_height(), 0.0, 1.0);

        // Handle X-axis constraints for intermediate points
        if(m_selectedIdx == 0)
            nx = 0.0;
        else if(m_selectedIdx == static_cast<int>(m_points.size()) - 1) nx = 1.0;
        else
        {
            // Keep points in order
            nx = std::clamp(nx, m_points[m_selectedIdx-1].x + 0.001, m_points[m_selectedIdx+1].x - 0.001);
        }

        m_points[m_selectedIdx] = {nx, ny};
        UpdateCurveCache();
        queue_draw();
        m_signalCurveChanged.emit();
    }

    void ToneCurveWidget::SetHistogram(const std::array<float, 256>& hist)
    {
        m_histogram = hist;
        queue_draw();
    }

    void ToneCurveWidget::UpdateCurveCache()
    {
        if (m_points.size() < 2) return;

        // Perfect linear diagonal if only 2 points
        if (m_points.size() == 2)
        {
            for (int i = 0; i < 256; i++)
                m_curveCache[i] = static_cast<float>(i) / 255.0f;

            return;
        }

        for (int i = 0; i < 256; i++)
        {
            double x = i / 255.0;
            int k = 0;
            while (k < static_cast<int>(m_points.size()) - 2 && x > m_points[k+1].x)
                k++;

            double x1 = m_points[k].x;
            double x2 = m_points[k+1].x;
            double t = (x2 - x1 > 0) ? (x - x1) / (x2 - x1) : 0.0;

            double p1 = m_points[k].y;
            double p2 = m_points[k+1].y;

            // Extrapolate phantom points p0 and p3 to prevent "flopping" at edges
            double p0 = (k > 0) ? m_points[k-1].y : p1 - (p2 - p1);
            double p3 = static_cast<int>(k < (m_points.size() - 2)) ? m_points[k+2].y : p2 + (p2 - p1);

            m_curveCache[i] = std::clamp(CatmullRom(t, p0, p1, p2, p3), 0.0f, 1.0f);
        }
    }

    void ToneCurveWidget::Draw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h) const
    {
        // 1. Background
        cr->set_source_rgb(0.08, 0.08, 0.08);
        cr->paint();

        // 2. Static Background Histogram (Luminance reference)
        if (!m_histogram.empty() && std::ranges::any_of(m_histogram.begin(), m_histogram.end(), [](float v){return v > 0;}))
        {
            cr->set_source_rgba(0.4, 0.4, 0.4, 0.25);
            cr->move_to(0, h);
            for (int i = 0; i < 256; i++)
            {
                double x = (i / 255.0) * w;
                cr->line_to(x, static_cast<float>(h) - (m_histogram[i] * static_cast<float>(h)));
            }
            cr->line_to(w, h);
            cr->fill();
        }

        // 3. Curve Line
        cr->set_source_rgb(0.9, 0.9, 0.9);
        cr->set_line_width(2.0);

        for(int i = 0; i < 256; i++)
        {
            double x = (i / 255.0) * w;
            double y = (1.0 - m_curveCache[i]) * h;

            if(i == 0)
                cr->move_to(x, y);
            else
                cr->line_to(x, y);
        }
        cr->stroke();

        // 4. Control Points (Exclude extreme anchors)
        cr->set_source_rgb(1.0, 0.5, 0.0);
        for(size_t i = 1; i < m_points.size() - 1; i++)
        {
            cr->arc(m_points[i].x * w, (1.0 - m_points[i].y) * h, 4.0, 0, 2 * M_PI);
            cr->fill();
        }
    }
}