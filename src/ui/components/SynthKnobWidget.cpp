#include "SynthKnobWidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDir>
#include <QCoreApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QAction>
#include <QWidgetAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include "RoutingPopupWidget.h"
#include "RoutingPopupWidget.h"
#include "ModSourceHelper.h"
#include "../../core/AppSettings.h"
#include "../../core/ThemePalette.h"

SynthKnobWidget::SynthKnobWidget(QWidget* parent)
    : QWidget(parent), m_value(0.0), m_virtualValue(0.0), m_defaultValue(0.0), m_minimum(-1.0), m_maximum(1.0), 
      m_dragging(false), m_modDragging(false), m_lastX(0), m_lastY(0)
{
    setFixedSize(48, 48); // Slightly bigger for better details
    setCursor(Qt::SizeVerCursor);
    setAcceptDrops(true);
    
    m_baseRenderer.load(QString(":/mixer/knob_base.svg"));
    m_indicatorRenderer.load(QString(":/mixer/knob_indicator.svg"));
}

void SynthKnobWidget::setValue(double val) {
    if (val < m_minimum) val = m_minimum;
    if (val > m_maximum) val = m_maximum;
    if (m_value != val) {
        m_value = val;
        emit valueChanged(m_value);
        update();
    }
}

void SynthKnobWidget::setMinimum(double min) {
    m_minimum = min;
    if (m_value < m_minimum) setValue(m_minimum);
    update();
}

void SynthKnobWidget::setMaximum(double max) {
    m_maximum = max;
    if (m_value > m_maximum) setValue(m_maximum);
    update();
}

void SynthKnobWidget::setModRouting(const ModRouting& routing) {
    m_modRouting = routing;
    emit modRoutingUpdatedExternally(m_modRouting);
    update();
}

void SynthKnobWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int size = qMin(width(), height()) - 4;
    int centerX = width() / 2;
    int centerY = height() / 2;
    
    int arcSize = size - 12;
    int arcX = centerX - arcSize / 2;
    int arcY = centerY - arcSize / 2;
    
    int startAngle = 225 * 16; 
    int spanAngle = -270 * 16; 
    
    QColor bodyCol0 = ThemePalette::color("knob_body_0");
    QColor bodyCol08 = ThemePalette::color("knob_body_08");
    QColor bodyCol1 = ThemePalette::color("knob_body_1");
    
    QRadialGradient knobGradient(centerX, centerY, arcSize / 2 - 2, centerX - arcSize / 6, centerY - arcSize / 6);
    knobGradient.setColorAt(0.0, bodyCol0);
    knobGradient.setColorAt(0.8, bodyCol08);
    knobGradient.setColorAt(1.0, bodyCol1);
    
    painter.setBrush(knobGradient);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(centerX - (arcSize / 2 - 2), centerY - (arcSize / 2 - 2), (arcSize / 2 - 2) * 2, (arcSize / 2 - 2) * 2);
    
    QPen trackPen;
    trackPen.setWidth(2);
    trackPen.setCapStyle(Qt::RoundCap);
    
    QColor trackColor = ThemePalette::color("knob_track");
    if (!isEnabled()) {
        trackColor = ThemePalette::color("knob_track_disabled");
    }
    
    trackPen.setColor(trackColor);
    painter.setPen(trackPen);
    painter.drawArc(arcX, arcY, arcSize, arcSize, startAngle, spanAngle);
    
    double range = m_maximum - m_minimum;
    double normalizedValue = range > 0 ? (m_value - m_minimum) / range : 0;
    int valueSpanAngle = -270 * normalizedValue * 16; 
    
    if (!m_modRouting.source.isEmpty() && m_modRouting.amount != 0.0) {
        QColor modColor = ThemePalette::color("knob_mod");
        QPen modPen;
        modPen.setColor(modColor);
        modPen.setWidth(2);
        modPen.setCapStyle(Qt::RoundCap);
        
        double normalizedAmount = (range != 0.0) ? (m_modRouting.amount / range) : 0.0;
        double modMin = normalizedValue;
        double modMax = normalizedValue + normalizedAmount;
        
        if (m_modRouting.bipolar) {
            modMin = normalizedValue - normalizedAmount;
            modMax = normalizedValue + normalizedAmount;
        }
        
        if (modMin > modMax) std::swap(modMin, modMax);
        modMin = qBound(0.0, modMin, 1.0);
        modMax = qBound(0.0, modMax, 1.0);
        
        int modStartAngle = startAngle + qRound(-270.0 * modMin * 16.0);
        int modSpanAngle = qRound(-270.0 * (modMax - modMin) * 16.0);
        
        painter.setPen(modPen);
        if (modSpanAngle != 0) {
            painter.drawArc(arcX, arcY, arcSize, arcSize, modStartAngle, modSpanAngle);
        }
    }
    
    QPen valuePen;
    valuePen.setWidth(4);
    valuePen.setCapStyle(Qt::RoundCap);
    
    QColor indicatorColor = ThemePalette::color("knob_value_arc", QColor(255, 102, 0));
    if (!isEnabled()) {
        indicatorColor = ThemePalette::color("knob_indicator_disabled");
    }
    
    valuePen.setColor(indicatorColor);
    painter.setPen(valuePen);
    painter.drawArc(arcX, arcY, arcSize, arcSize, startAngle, valueSpanAngle);
}

void SynthKnobWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if ((event->modifiers() & Qt::ControlModifier) && !m_modRouting.source.isEmpty()) {
            m_modDragging = true;
        } else {
            m_dragging = true;
            m_virtualValue = m_value;
        }
        m_lastX = event->pos().x();
        m_lastY = event->pos().y();
        event->accept();
    }
}

void SynthKnobWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging || m_modDragging) {
        int deltaX = event->pos().x() - m_lastX;
        int deltaY = m_lastY - event->pos().y();
        m_lastX = event->pos().x();
        m_lastY = event->pos().y();
        
        // Combine horizontal and vertical deltas
        int delta = deltaX + deltaY;
        
        if (m_modDragging) {
            double step = delta * 0.01;
            if (event->modifiers() & Qt::ShiftModifier) step *= 0.1;
            m_modRouting.amount = qBound(-1.0, m_modRouting.amount + step, 1.0);
            emit modulationChanged(m_modRouting);
            update();
        } else {
            double step = delta / 200.0; // 0.0 to 1.0 normalized delta
            if (event->modifiers() & Qt::ShiftModifier) step *= 0.1; // Fine adjustment
            
            if (m_isLogarithmic && m_minimum > 0 && m_maximum > 0) {
                double currentNorm = (std::log(m_virtualValue) - std::log(m_minimum)) / (std::log(m_maximum) - std::log(m_minimum));
                currentNorm += step;
                currentNorm = qBound(0.0, currentNorm, 1.0);
                
                double displayVal = std::exp(currentNorm * (std::log(m_maximum) - std::log(m_minimum)) + std::log(m_minimum));
                m_virtualValue = displayVal;
                setValue(displayVal);
            } else {
                double range = m_maximum - m_minimum;
                m_virtualValue += step * range;
                double displayVal = m_virtualValue;
                
                // Magnetic snap to 0 if range crosses 0
                if (m_minimum < 0.0 && m_maximum > 0.0 && !(event->modifiers() & Qt::ShiftModifier)) {
                    if (std::abs(displayVal) < (range * 0.03)) {
                        displayVal = 0.0;
                    }
                }
                setValue(displayVal);
            }
        }
        event->accept();
    }
}

void SynthKnobWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        m_modDragging = false;
        event->accept();
    }
}

void SynthKnobWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (event->modifiers() & Qt::ControlModifier && !m_modRouting.source.isEmpty()) {
            m_modRouting.amount = 0.0;
            emit modulationChanged(m_modRouting);
            update();
        } else {
            setValue(m_defaultValue);
        }
        event->accept();
    }
}

void SynthKnobWidget::wheelEvent(QWheelEvent* event) {
    event->ignore(); // Ignore wheel to let the QScrollArea handle the page scrolling
}

void SynthKnobWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("application/x-modsource")) {
        event->acceptProposedAction();
    }
}

void SynthKnobWidget::dropEvent(QDropEvent* event) {
    if (event->mimeData()->hasFormat("application/x-modsource")) {
        QByteArray data = event->mimeData()->data("application/x-modsource");
        QString source = QString::fromUtf8(data);
        if (!source.isEmpty()) {
            m_modRouting.source = source;
            m_modRouting.amount = 0.5; // Default amount upon drag
            emit modulationChanged(m_modRouting);
            update();
        }
        event->acceptProposedAction();
    }
}



void SynthKnobWidget::contextMenuEvent(QContextMenuEvent* event) {
    if (!m_modSourceProvider) return;
    
    QStringList sources = m_modSourceProvider();
    
    ModRouting r = m_modRouting;
    r.destination = m_modDestination; // ensure destination is correctly set

    RoutingPopupWidget* popup = new RoutingPopupWidget(this, sources, r);
    popup->adjustSize();
    
    // Center from the center of the knob, arrow pointing UP
    QPoint globalPos = mapToGlobal(QPoint(width() / 2, height() / 2));
    popup->move(globalPos.x() - popup->width() / 2, globalPos.y() - 8);
    
    popup->show();
    event->accept();
}
