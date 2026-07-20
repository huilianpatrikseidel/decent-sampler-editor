#include "LFOGraphWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <cmath>
#include <QPen>
#include <QMouseEvent>

LFOGraphWidget::LFOGraphWidget(QWidget* parent) : QWidget(parent) {
    setObjectName("NeumorphicInner");
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedHeight(100);
    setMinimumWidth(200);
    
    // Animation timer for live LFO feedback
    connect(&m_animTimer, &QTimer::timeout, this, [this]() {
        m_animTime += 0.05 * (m_rate / 10.0 + 0.1);
        if (m_animTime > 1.0) m_animTime -= 1.0;
        update();
    });
    m_animTimer.start(30); // ~33fps
}

void LFOGraphWidget::setParameters(const QString& shape, double rate, double phase) {
    m_shape = shape.toLower();
    m_rate = rate; // Just for visual density
    m_phase = phase;
    update();
}

void LFOGraphWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(0, 5, 0, -5);
    
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 8, 8);
    painter.setClipPath(clipPath);
    
    // Center line
    double midY = rect.top() + rect.height() / 2.0;
    painter.setPen(QPen(QColor(255, 255, 255, 20), 1, Qt::DashLine));
    painter.drawLine(rect.left(), midY, rect.right(), midY);
    
    QPainterPath path;
    int points = rect.width();
    
    // We will draw exactly 2 cycles if rate is "average", or adjust cycles based on rate.
    // For UI simplicity, let's map rate 0..20Hz to 1..5 cycles.
    double cycles = 1.0 + (m_rate / 20.0) * 4.0;
    if (cycles < 1.0) cycles = 1.0;
    if (cycles > 10.0) cycles = 10.0;
    
    bool first = true;
    for (int x = 0; x < points; ++x) {
        // Normalized x from 0 to 1
        double nx = static_cast<double>(x) / points;
        
        // Phase angle in radians (0 to 2PI per cycle)
        double angle = (nx * cycles * 2.0 * M_PI) + (m_phase * 2.0 * M_PI) - (m_animTime * 2.0 * M_PI);
        
        double yVal = 0.0; // from -1 to 1
        
        if (m_shape == "sine") {
            yVal = std::sin(angle);
        } else if (m_shape == "triangle") {
            double normalizedAngle = std::fmod(angle + 100.0 * M_PI, 2.0 * M_PI) / (2.0 * M_PI); // 0 to 1
            if (normalizedAngle < 0.25) {
                yVal = normalizedAngle * 4.0; // 0 to 1
            } else if (normalizedAngle < 0.75) {
                yVal = 1.0 - (normalizedAngle - 0.25) * 4.0; // 1 to -1
            } else {
                yVal = -1.0 + (normalizedAngle - 0.75) * 4.0; // -1 to 0
            }
        } else if (m_shape == "square") {
            double normalizedAngle = std::fmod(angle + 100.0 * M_PI, 2.0 * M_PI) / (2.0 * M_PI);
            yVal = (normalizedAngle < 0.5) ? 1.0 : -1.0;
        } else if (m_shape == "saw" || m_shape == "sawtooth") {
            double normalizedAngle = std::fmod(angle + 100.0 * M_PI, 2.0 * M_PI) / (2.0 * M_PI);
            yVal = 1.0 - (normalizedAngle * 2.0); // 1 to -1
        } else {
            yVal = std::sin(angle); // fallback
        }
        
        double px = rect.left() + x;
        // yVal is -1 to 1. Invert for Y axis (1 is top, -1 is bottom)
        double py = midY - (yVal * (rect.height() / 2.0 - 5.0));
        
        if (first) {
            path.moveTo(px, py);
            first = false;
        } else {
            path.lineTo(px, py);
        }
    }
    
    // Fill slightly FIRST
    QPainterPath fillPath = path;
    fillPath.lineTo(rect.right(), midY);
    fillPath.lineTo(rect.left(), midY);
    fillPath.closeSubpath();
    
    QLinearGradient gradient(0, rect.top(), 0, rect.bottom());
    int fillAlpha = isEnabled() ? 120 : 40;
    gradient.setColorAt(0.0, QColor(180, 80, 255, fillAlpha));
    gradient.setColorAt(0.5, QColor(180, 80, 255, 0));
    gradient.setColorAt(1.0, QColor(180, 80, 255, fillAlpha));
    
    painter.fillPath(fillPath, gradient);
    

    // Draw solid outline
    QPen outlinePen(isEnabled() ? QColor(200, 100, 255) : QColor(200, 100, 255, 80), 2); // Purple for LFO
    outlinePen.setCapStyle(Qt::RoundCap);
    outlinePen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(outlinePen);
    painter.drawPath(path);
}
