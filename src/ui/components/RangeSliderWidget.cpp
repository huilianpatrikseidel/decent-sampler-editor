#include "RangeSliderWidget.h"
#include <QPainterPath>
#include <QEvent>
#include <cmath>

RangeSliderWidget::RangeSliderWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(40);
    setMouseTracking(true);
}

void RangeSliderWidget::setMinimum(double min) {
    if (m_minimum != min) {
        m_minimum = min;
        update();
    }
}

void RangeSliderWidget::setMaximum(double max) {
    if (m_maximum != max) {
        m_maximum = max;
        update();
    }
}

void RangeSliderWidget::setLowerValue(double val) {
    val = std::clamp(val, m_minimum, m_upperValue);
    if (m_lowerValue != val) {
        m_lowerValue = val;
        update();
        emit rangeChanged(m_lowerValue, m_upperValue);
    }
}

void RangeSliderWidget::setUpperValue(double val) {
    val = std::clamp(val, m_lowerValue, m_maximum);
    if (m_upperValue != val) {
        m_upperValue = val;
        update();
        emit rangeChanged(m_lowerValue, m_upperValue);
    }
}

void RangeSliderWidget::setRange(double min, double max) {
    m_minimum = min;
    m_maximum = max;
    update();
}

void RangeSliderWidget::setValues(double lower, double upper) {
    m_lowerValue = std::clamp(lower, m_minimum, m_maximum);
    m_upperValue = std::clamp(upper, m_lowerValue, m_maximum);
    update();
    emit rangeChanged(m_lowerValue, m_upperValue);
}

QRect RangeSliderWidget::getTrackRect() const {
    return QRect(10, height() / 2 - 2, width() - 20, 4);
}

int RangeSliderWidget::valueToPixel(double val) const {
    QRect track = getTrackRect();
    if (m_maximum <= m_minimum) return track.left();
    double ratio = (val - m_minimum) / (m_maximum - m_minimum);
    return track.left() + static_cast<int>(ratio * track.width());
}

double RangeSliderWidget::pixelToValue(int x) const {
    QRect track = getTrackRect();
    if (track.width() == 0) return m_minimum;
    double ratio = static_cast<double>(x - track.left()) / track.width();
    return m_minimum + ratio * (m_maximum - m_minimum);
}

QRect RangeSliderWidget::getLowerHandleRect() const {
    int x = valueToPixel(m_lowerValue);
    return QRect(x - 6, height() / 2 - 8, 12, 16);
}

QRect RangeSliderWidget::getUpperHandleRect() const {
    int x = valueToPixel(m_upperValue);
    return QRect(x - 6, height() / 2 - 8, 12, 16);
}

void RangeSliderWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect track = getTrackRect();

    // Draw background track
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#1a1a1c"));
    painter.drawRoundedRect(track, 2, 2);

    // Draw active span
    int x1 = valueToPixel(m_lowerValue);
    int x2 = valueToPixel(m_upperValue);
    QRect spanRect(x1, track.top(), x2 - x1, track.height());
    
    painter.setBrush(QColor("#ff6600")); // Orange theme
    painter.drawRoundedRect(spanRect, 2, 2);

    // Draw handles
    auto drawHandle = [&](const QRect& r, bool isActive) {
        painter.setPen(QPen(QColor(isActive ? "#ffffff" : "#cccccc"), 1));
        painter.setBrush(QColor("#333333"));
        painter.drawRoundedRect(r, 3, 3);
        
        // Draw grips
        painter.setPen(QColor("#777777"));
        painter.drawLine(r.center().x() - 2, r.top() + 4, r.center().x() - 2, r.bottom() - 4);
        painter.drawLine(r.center().x() + 2, r.top() + 4, r.center().x() + 2, r.bottom() - 4);
    };

    drawHandle(getLowerHandleRect(), m_activeHandle == Lower);
    drawHandle(getUpperHandleRect(), m_activeHandle == Upper);
    
    // Draw text values below handles
    painter.setPen(QColor("#888888"));
    QFont f = painter.font();
    f.setPixelSize(10);
    painter.setFont(f);
    
    QString lowerStr = QString::number(m_lowerValue, 'f', 2);
    QString upperStr = QString::number(m_upperValue, 'f', 2);
    
    painter.drawText(QRect(x1 - 25, height() / 2 + 8, 50, 12), Qt::AlignCenter, lowerStr);
    painter.drawText(QRect(x2 - 25, height() / 2 + 8, 50, 12), Qt::AlignCenter, upperStr);
}

void RangeSliderWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QRect lowerRect = getLowerHandleRect();
        QRect upperRect = getUpperHandleRect();

        // Expand hit area slightly
        lowerRect.adjust(-4, -4, 4, 4);
        upperRect.adjust(-4, -4, 4, 4);

        if (lowerRect.contains(event->pos())) {
            m_activeHandle = Lower;
            m_offset = event->pos().x() - valueToPixel(m_lowerValue);
        } else if (upperRect.contains(event->pos())) {
            m_activeHandle = Upper;
            m_offset = event->pos().x() - valueToPixel(m_upperValue);
        } else {
            int x1 = valueToPixel(m_lowerValue);
            int x2 = valueToPixel(m_upperValue);
            if (event->pos().x() > x1 && event->pos().x() < x2) {
                m_activeHandle = Center;
                m_offset = event->pos().x(); // Used to calculate delta
            } else {
                // Clicked outside span and handles. Snap the closest one.
                double val = pixelToValue(event->pos().x());
                if (std::abs(val - m_lowerValue) < std::abs(val - m_upperValue)) {
                    setLowerValue(val);
                    m_activeHandle = Lower;
                } else {
                    setUpperValue(val);
                    m_activeHandle = Upper;
                }
                m_offset = 0;
            }
        }
        update();
    }
}

void RangeSliderWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_activeHandle == Lower) {
        double val = pixelToValue(event->pos().x() - m_offset);
        setLowerValue(val);
    } else if (m_activeHandle == Upper) {
        double val = pixelToValue(event->pos().x() - m_offset);
        setUpperValue(val);
    } else if (m_activeHandle == Center) {
        double delta = pixelToValue(event->pos().x()) - pixelToValue(m_offset);
        m_offset = event->pos().x();
        
        double range = m_upperValue - m_lowerValue;
        double newLower = m_lowerValue + delta;
        double newUpper = m_upperValue + delta;
        
        if (newLower < m_minimum) {
            newLower = m_minimum;
            newUpper = newLower + range;
        } else if (newUpper > m_maximum) {
            newUpper = m_maximum;
            newLower = newUpper - range;
        }
        
        setValues(newLower, newUpper);
    }
}

void RangeSliderWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_activeHandle = None;
        update();
        emit rangeEditingFinished(m_lowerValue, m_upperValue);
    }
}
