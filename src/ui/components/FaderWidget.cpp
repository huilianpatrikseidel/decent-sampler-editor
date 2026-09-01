#include "FaderWidget.h"
#include "core/DecibelUtils.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDir>
#include <QCoreApplication>

FaderWidget::FaderWidget(bool isMaster, QWidget* parent)
    : QWidget(parent), m_position(kUnityPosition), m_defaultPosition(kUnityPosition),
      m_dragging(false), m_capHeight(30)
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

// Piecewise taper: the bottom 80% of the travel covers kFloorDb..0 dB, the top 20%
// covers 0..kMaxDb, and the very bottom snaps to silence. Linear-in-dB over each segment
// keeps the useful range readable instead of crowding everything near unity.
double FaderWidget::positionToDb(double pos) {
    if (pos <= 0.0) return DecibelUtils::kSilenceDb;
    if (pos >= kUnityPosition) {
        return (pos - kUnityPosition) / (1.0 - kUnityPosition) * kMaxDb;
    }
    return kFloorDb * (1.0 - pos / kUnityPosition);
}

double FaderWidget::dbToPosition(double db) {
    if (db <= DecibelUtils::kSilenceDb) return 0.0;
    if (db >= 0.0) {
        const double pos = kUnityPosition + (db / kMaxDb) * (1.0 - kUnityPosition);
        return pos > 1.0 ? 1.0 : pos;
    }
    const double pos = kUnityPosition * (1.0 - db / kFloorDb);
    return pos < 0.0 ? 0.0 : pos;
}

double FaderWidget::valueDb() const {
    return positionToDb(m_position);
}

void FaderWidget::setValueDb(double db) {
    setPosition(dbToPosition(db));
}

void FaderWidget::setPosition(double pos) {
    if (pos < 0.0) pos = 0.0;
    if (pos > 1.0) pos = 1.0;
    if (m_position != pos) {
        m_position = pos;
        emit valueChanged(valueDb());
        update();
    }
}

int FaderWidget::valueToY() const {
    // value 1.0 = top, 0.0 = bottom
    return trackTopMargin() + static_cast<int>((1.0 - m_position) * trackLength());
}

void FaderWidget::updateValueFromY(int y) {
    int top = trackTopMargin();
    int bottom = height() - trackBottomMargin();
    if (y < top) y = top;
    if (y > bottom) y = bottom;
    
    const double normalized = 1.0 - static_cast<double>(y - top) / trackLength();
    setPosition(normalized);
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
        
        setPosition(m_position + step);
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
        setPosition(m_defaultPosition);
        event->accept();
    }
}

void FaderWidget::wheelEvent(QWheelEvent* event) {
    const double step = 0.05;
    if (event->angleDelta().y() > 0) {
        setPosition(m_position + step);
    } else {
        setPosition(m_position - step);
    }
    event->accept();
}
