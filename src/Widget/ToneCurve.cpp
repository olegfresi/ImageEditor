#include "../../include/Widget/ToneCurve.hpp"
#include <algorithm>
#include <cmath>

namespace Editor::Widget
{
    ToneCurveWidget::ToneCurveWidget() : m_lutCache{}
    {
        set_size_request(-1, 180);

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

    int ToneCurveWidget::FindPoint(double x, double y) const
    {
        constexpr double radius = 0.04;
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

        if(m_selectedIdx == 0)
            nx = 0.0;
        else if(m_selectedIdx == static_cast<int>(m_points.size()) - 1)
            nx = 1.0;
        else
            nx = std::clamp(nx, m_points[m_selectedIdx-1].x + 0.001, m_points[m_selectedIdx+1].x - 0.001);

        auto oldLut = m_lutCache;

        m_points[m_selectedIdx] = {nx, ny};
        UpdateCurveCache();

        if (m_lutCache != oldLut)
            m_signalCurveChanged.emit();

        queue_draw();
    }

    void ToneCurveWidget::SetHistogram(const std::array<float, 256>& hist)
    {
        m_histogram = hist;
        m_histSurface.reset();

        if (std::ranges::any_of(m_histogram, [](float v){ return v > 0; }))
        {
            const int w = get_allocated_width();
            const int h = get_allocated_height();

            m_histSurface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, w, h);
            auto cr = Cairo::Context::create(m_histSurface);

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

        for (int i = 0; i < 256; i++)
            m_lutCache[i] = static_cast<uint8_t>(std::clamp(m_curveCache[i] * 255.0f, 0.0f, 255.0f));
    }

    void ToneCurveWidget::Draw(const Cairo::RefPtr<Cairo::Context>& cr, int w, int h) const
    {
        // 1. Widget Background
        // Using a dark grey (8% luminance) to provide high contrast for the curve
        cr->set_source_rgb(0.08, 0.08, 0.08);
        cr->paint();

        // 2. Background Histogram Drawing (Cached)
        // We use a pre-rendered surface (m_histSurface) to avoid recalculating
        // 256 geometry points on every mouse movement. This is hardware-accelerated on Apple Silicon.
        if (m_histSurface)
        {
            cr->save();
            cr->set_source(m_histSurface, 0, 0);
            cr->paint();
            cr->restore();
        }

        // 3. Reference Grid (Optional, but useful for visual guidance)
        cr->set_source_rgba(0.2, 0.2, 0.2, 0.5);
        cr->set_line_width(1.0);

        // Draw quarter-division lines (25%, 50%, 75%)
        for(int i = 1; i < 4; ++i) {
            double pos = i * 0.25;
            cr->move_to(pos * w, 0); cr->line_to(pos * w, h);
            cr->move_to(0, pos * h); cr->line_to(w, pos * h);
        }
        cr->stroke();

        // 4. Tone Curve Path Drawing
        cr->save();
        cr->set_source_rgb(0.9, 0.9, 0.9); // Off-white/Light grey for the curve
        cr->set_line_width(2.0);
        cr->set_line_join(Cairo::Context::LineJoin::ROUND);

        for(int i = 0; i < 256; i++)
        {
            // Calculate screen coordinates based on the normalized values in m_curveCache
            double x = (static_cast<double>(i) / 255.0) * w;
            double y = (1.0 - static_cast<double>(m_curveCache[i])) * h;

            if(i == 0)
                cr->move_to(x, y);
            else
                cr->line_to(x, y);
        }
        cr->stroke();
        cr->restore();

        // 5. Control Points Drawing
        cr->save();
        for(size_t i = 0; i < m_points.size(); i++)
        {
            // Apply different colors based on the state of the control point
            if (static_cast<int>(i) == m_selectedIdx)
                cr->set_source_rgb(1.0, 0.8, 0.0); // Bright Yellow for the active/dragged point
            else if (i == 0 || i == m_points.size() - 1)
                cr->set_source_rgb(0.6, 0.6, 0.6); // Grey for the fixed edge anchors (0,0 and 1,1)
            else
                cr->set_source_rgb(1.0, 0.5, 0.0); // Orange for intermediate spline points

            double px = m_points[i].x * w;
            double py = (1.0 - m_points[i].y) * h;

            // Draw the point circle
            cr->arc(px, py, 4.5, 0, 2 * M_PI);
            cr->fill();

            // Draw a white outline (stroke) to make the points pop against the background
            cr->set_source_rgb(1.0, 1.0, 1.0);
            cr->set_line_width(1.0);
            cr->arc(px, py, 4.5, 0, 2 * M_PI);
            cr->stroke();
        }
        cr->restore();
    }

    void ToneCurveWidget::SetLut(const std::array<uint8_t, 256>& lut)
    {
        m_lutCache = lut;
        for(int i = 0; i < 256; ++i)
            m_curveCache[i] = static_cast<float>(lut[i] / 255.0);
        queue_draw();
    }

    ToneCurveState ToneCurveWidget::GetState() const
    {
        return { m_lutCache, m_points };
    }

    void ToneCurveWidget::SetState(const ToneCurveState& state)
    {
        m_lutCache = state.lut;
        m_points = state.points;

        for(int i = 0; i < 256; ++i)
            m_curveCache[i] = static_cast<float>(m_lutCache[i] / 255.0);

        queue_draw();
    }

    void ToneCurveWidget::Reset()
    {
        m_points = {{0.0, 0.0}, {1.0, 1.0}};
        m_selectedIdx = -1;

        UpdateCurveCache();
        queue_draw();
    }
}