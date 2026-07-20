#include "KnobWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDir>
#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include "../../core/ThemePalette.h"

KnobWidget::KnobWidget(QWidget* parent)
    : QWidget(parent), m_value(0.0), m_virtualValue(0.0), m_defaultValue(0.0), m_minimum(-1.0), m_maximum(1.0), 
      m_dragging(false), m_lastY(0)
{
    setFixedSize(48, 48); // Slightly bigger for better details
    setCursor(Qt::SizeVerCursor);
}

void KnobWidget::setValue(double val) {
    if (val < m_minimum) val = m_minimum;
    if (val > m_maximum) val = m_maximum;
    if (m_value != val) {
        m_value = val;
        emit valueChanged(m_value);
        update();
    }
}

void KnobWidget::setMinimum(double min) {
    m_minimum = min;
    if (m_value < m_minimum) setValue(m_minimum);
    update();
}

void KnobWidget::setMaximum(double max) {
    m_maximum = max;
    if (m_value > m_maximum) setValue(m_maximum);
    update();
}

void KnobWidget::setModSource(const QString& source) {
    if (m_modSource != source) {
        m_modSource = source;
        emit modulationChanged(m_modSource, m_modAmount);
        update();
    }
}

void KnobWidget::setModAmount(double amount) {
    if (amount < -1.0) amount = -1.0;
    if (amount > 1.0) amount = 1.0;
    if (m_modAmount != amount) {
        m_modAmount = amount;
        emit modulationChanged(m_modSource, m_modAmount);
        update();
    }
}

void KnobWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int margin = 4;
    QRectF arcRect(margin, margin, width() - margin*2, height() - margin*2);
    
    double range = m_maximum - m_minimum;
    double normalized = range > 0 ? (m_value - m_minimum) / range : 0;
    
    // Qt angles: 0 is 3 o'clock, positive is CCW.
    // 225 degrees is bottom-left. -270 degrees span draws a standard 3/4 circle knob track.
    int startAngle = 225 * 16;
    int spanAngle = -270 * 16;
    
    QPointF center = arcRect.center();
    double radius = arcRect.width() / 2.0;
    
    // Draw knob body (tactile mass)
    painter.setPen(Qt::NoPen);
    QRadialGradient bodyGrad(center, radius);
    bodyGrad.setColorAt(0.0, ThemePalette::color("qss_color_25", QColor(45, 45, 48)));
    bodyGrad.setColorAt(0.8, ThemePalette::color("qss_color_23", QColor(25, 25, 28)));
    bodyGrad.setColorAt(1.0, ThemePalette::color("qss_color_22", QColor(15, 15, 18)));
    painter.setBrush(bodyGrad);
    painter.drawEllipse(center, radius - 2, radius - 2);
    
    // Draw background track
    QPen trackPen(ThemePalette::color("qss_color_35", QColor(40, 40, 42)), 4);
    trackPen.setCapStyle(Qt::RoundCap);
    painter.setPen(trackPen);
    painter.drawArc(arcRect, startAngle, spanAngle);
    
    // Draw value arc
    int valSpan = qRound(-270.0 * normalized * 16.0);
    QPen valPen(ThemePalette::color("qss_color_6", QColor(255, 102, 0)), 4); // Accent Color
    valPen.setCapStyle(Qt::RoundCap);
    painter.setPen(valPen);
    if (valSpan != 0) {
        painter.drawArc(arcRect, startAngle, valSpan);
    }
    
    // Draw modulation ring if active
    if (!m_modSource.isEmpty()) {
        double modVal = m_value + (m_modAmount * range);
        if (modVal < m_minimum) modVal = m_minimum;
        if (modVal > m_maximum) modVal = m_maximum;
        double modNormalized = range > 0 ? (modVal - m_minimum) / range : 0;
        
        int modStart = startAngle + valSpan;
        int modSpan = qRound(-270.0 * (modNormalized - normalized) * 16.0);
        
        QPen modPen(m_modAmount >= 0 ? QColor(0, 180, 255) : QColor(255, 50, 50));
        modPen.setWidth(3);
        modPen.setCapStyle(Qt::RoundCap);
        painter.setPen(modPen);
        
        if (modSpan != 0) {
            painter.drawArc(arcRect, modStart, modSpan);
        }
    }
    
    // Draw small indicator dot at current value
    double currentAngleRad = qDegreesToRadians(225.0 - (270.0 * normalized));
    QPointF indicatorPos(
        center.x() + radius * qCos(currentAngleRad),
        center.y() - radius * qSin(currentAngleRad)
    );
    
    QPointF notchStart(
        center.x() + (radius - 8) * qCos(currentAngleRad),
        center.y() - (radius - 8) * qSin(currentAngleRad)
    );
    
    painter.setPen(QPen(ThemePalette::color("knob_indicator", QColor(255, 102, 0)), 2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(notchStart, indicatorPos);
}

void KnobWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if ((event->modifiers() & Qt::ControlModifier) && !m_modSource.isEmpty()) {
            m_modDragging = true;
        } else {
            m_dragging = true;
            m_virtualValue = m_value;
        }
        m_lastY = event->pos().y();
        event->accept();
    }
}

void KnobWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging || m_modDragging) {
        int deltaY = m_lastY - event->pos().y();
        m_lastY = event->pos().y();
        
        if (m_modDragging) {
            double step = deltaY * 0.01;
            if (event->modifiers() & Qt::ShiftModifier) step *= 0.1;
            setModAmount(m_modAmount + step);
        } else {
            double range = m_maximum - m_minimum;
            double step = (range / 200.0) * deltaY; // 200 pixels for full range
            if (event->modifiers() & Qt::ShiftModifier) step *= 0.1; // Fine adjustment
            
            m_virtualValue += step;
            double displayVal = m_virtualValue;
            
            // Magnetic snap to 0 if range crosses 0
            if (m_minimum < 0.0 && m_maximum > 0.0 && !(event->modifiers() & Qt::ShiftModifier)) {
                if (std::abs(displayVal) < (range * 0.03)) {
                    displayVal = 0.0;
                }
            }
            setValue(displayVal);
        }
        event->accept();
    }
}

void KnobWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_modDragging = false;
        event->accept();
    }
}

void KnobWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier && !m_modSource.isEmpty()) {
            setModAmount(0.0);
        } else {
            setValue(m_defaultValue);
        }
        event->accept();
    }
}

void KnobWidget::wheelEvent(QWheelEvent* event) {
    event->ignore(); // Ignore wheel to let the QScrollArea handle the page scrolling
}

void KnobWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-modsource")) {
        event->acceptProposedAction();
    }
}

void KnobWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-modsource")) {
        QByteArray data = event->mimeData()->data("application/x-modsource");
        QString source = QString::fromUtf8(data);
        if (!source.isEmpty()) {
            m_modSource = source;
            m_modAmount = 0.5; // Default amount upon drag
            emit modulationChanged(m_modSource, m_modAmount);
            update();
        }
        event->acceptProposedAction();
    }
}

void KnobWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (!m_modSource.isEmpty()) {
        QMenu menu(this);
        QAction* removeAct = menu.addAction("Remove Modulation (" + m_modSource + ")");
        if (menu.exec(event->globalPos()) == removeAct) {
            m_modSource.clear();
            m_modAmount = 0.0;
            emit modulationChanged(m_modSource, m_modAmount);
            update();
        }
    }
}
