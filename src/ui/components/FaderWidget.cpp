#include "FaderWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDir>
#include <QCoreApplication>

FaderWidget::FaderWidget(bool isMaster, QWidget* parent)
    : QWidget(parent), m_value(1.0), m_defaultValue(1.0), m_dragging(false), m_capHeight(30)
{
    setFixedSize(30, 200);
    setCursor(Qt::SizeVerCursor);
    
    m_trackRenderer.load(QString(":/mixer/fader_track.svg"));
    if (isMaster) {
        m_capRenderer.load(QString(":/mixer/fader_cap_master.svg"));
    } else {
        m_capRenderer.load(QString(":/mixer/fader_cap.svg"));
    }
}

void FaderWidget::setValue(double val) {
    if (val < 0.0) val = 0.0;
    if (val > 1.0) val = 1.0;
    if (m_value != val) {
        m_value = val;
        emit valueChanged(m_value);
        update();
    }
}

int FaderWidget::valueToY() const {
    // value 1.0 = top, 0.0 = bottom
    return trackTopMargin() + static_cast<int>((1.0 - m_value) * trackLength());
}

void FaderWidget::updateValueFromY(int y) {
    int top = trackTopMargin();
    int bottom = height() - trackBottomMargin();
    if (y < top) y = top;
    if (y > bottom) y = bottom;
    
    double normalized = 1.0 - static_cast<double>(y - top) / trackLength();
    setValue(normalized);
}

void FaderWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    
    // Draw track
    m_trackRenderer.render(&painter, rect());
    
    // Draw cap
    int cy = valueToY();
    QRect capRect(0, cy - m_capHeight / 2, width(), m_capHeight);
    
    // The SVGs are designed as width=40, height=30 but our widget width is 30.
    // Let's adjust cap rect to be slightly wider to overlap the sides
    QRect drawCapRect(-5, cy - m_capHeight / 2, width() + 10, m_capHeight);
    m_capRenderer.render(&painter, drawCapRect);
}

void FaderWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        // Se quisermos pular pro local do click, descomentamos a proxima linha:
        // updateValueFromY(event->pos().y());
        m_lastY = event->pos().y();
        event->accept();
    }
}

void FaderWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging) {
        int dy = m_lastY - event->pos().y();
        m_lastY = event->pos().y();
        
        double step = static_cast<double>(dy) / trackLength();
        if (event->modifiers() & Qt::ShiftModifier) {
            step *= 0.1; // Fine adjustment
        }
        
        setValue(m_value + step);
        event->accept();
    }
}

void FaderWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void FaderWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setValue(m_defaultValue);
        event->accept();
    }
}

void FaderWidget::wheelEvent(QWheelEvent* event) {
    double step = 0.05;
    if (event->angleDelta().y() > 0) {
        setValue(m_value + step);
    } else {
        setValue(m_value - step);
    }
    event->accept();
}
