#include "FxKnobWidget.h"
#include <QPainterPath>
#include <QtMath>
#include <QFont>
#include <QFontMetrics>
#include "../../core/ThemePalette.h"

FxKnobWidget::FxKnobWidget(const QString& prefix, const QString& label, double min, double max, double defaultValue, const QString& suffix, QWidget* parent)
    : QWidget(parent), m_prefix(prefix), m_label(label), m_minimum(min), m_maximum(max), m_defaultValue(defaultValue), m_value(defaultValue), m_suffix(suffix), m_dragging(false), m_lastY(0)
{
    setFixedSize(60, 80);
    setCursor(Qt::SizeVerCursor);
    
    m_baseRenderer = new QSvgRenderer(QString(":/fx/%1_knob_base.svg").arg(m_prefix), this);
    m_capRenderer = new QSvgRenderer(QString(":/fx/%1_knob_cap.svg").arg(m_prefix), this);
}

void FxKnobWidget::setValue(double val) {
    val = qBound(m_minimum, val, m_maximum);
    if (m_value != val) {
        m_value = val;
        update();
        emit valueChanged(m_value);
    }
}

void FxKnobWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastY = event->pos().y();
        event->accept();
    }
}

void FxKnobWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        int deltaY = m_lastY - event->pos().y();
        m_lastY = event->pos().y();
        
        double range = m_maximum - m_minimum;
        double step = (range / 150.0) * deltaY;
        if (event->modifiers() & Qt::ShiftModifier) step *= 0.1;
        
        setValue(m_value + step);
        event->accept();
    }
}

void FxKnobWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void FxKnobWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setValue(m_defaultValue);
        event->accept();
    }
}

void FxKnobWidget::wheelEvent(QWheelEvent* event) {
    double range = m_maximum - m_minimum;
    double step = range / 50.0;
    if (event->angleDelta().y() > 0) setValue(m_value + step);
    else setValue(m_value - step);
    event->accept();
}

void FxKnobWidget::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    
    QRect knobRect(10, 15, 40, 40);
    
    // Draw SVG Base
    if (m_baseRenderer->isValid()) {
        m_baseRenderer->render(&p, knobRect);
    }
    
    // Draw SVG Cap with Rotation
    if (m_capRenderer->isValid()) {
        double normalized = (m_value - m_minimum) / (m_maximum - m_minimum);
        double angle = 225.0 - (normalized * 270.0);
        
        p.translate(knobRect.center());
        p.rotate(-angle + 90);
        
        QRect capRect(-knobRect.width()/2, -knobRect.height()/2, knobRect.width(), knobRect.height());
        m_capRenderer->render(&p, capRect);
        
        p.rotate(angle - 90);
        p.translate(-knobRect.center());
    }
    
    // Draw Label at top
    p.setFont(QFont("Inter", 7, QFont::Bold));
    p.setPen(ThemePalette::color("qss_color_8"));
    p.drawText(QRect(0, 0, 60, 15), Qt::AlignCenter, m_label.toUpper());
    
    // Draw Value at bottom
    p.setFont(QFont("Inter", 8));
    p.setPen(ThemePalette::color("qss_color_4"));
    
    QString valText = QString::number(m_value, 'f', 2) + m_suffix;
    p.drawText(QRect(0, 60, 60, 20), Qt::AlignCenter, valText);
}
