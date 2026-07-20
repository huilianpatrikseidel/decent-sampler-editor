#include "CurveGraphWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QStringList>
#include <QtMath>
#include <algorithm>

CurveGraphWidget::CurveGraphWidget(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(100);
    setCursor(Qt::CrossCursor);
    
    // Default to a linear line
    m_points.append(QPointF(0.0, 0.0));
    m_points.append(QPointF(1.0, 1.0));
}

void CurveGraphWidget::setYRange(double minY, double maxY) {
    if (minY < maxY) {
        m_minY = minY;
        m_maxY = maxY;
        updatePath();
        update();
    }
}

void CurveGraphWidget::setTableString(const QString& tableStr) {
    m_points.clear();
    QStringList pairs = tableStr.split(';', Qt::SkipEmptyParts);
    for (const QString& pair : pairs) {
        QStringList vals = pair.split(',', Qt::SkipEmptyParts);
        if (vals.size() == 2) {
            bool okX, okY;
            double x = vals[0].trimmed().toDouble(&okX);
            double y = vals[1].trimmed().toDouble(&okY);
            if (okX && okY) {
                m_points.append(QPointF(x, y));
            }
        }
    }
    
    if (m_points.isEmpty()) {
        m_points.append(QPointF(0.0, 0.0));
        m_points.append(QPointF(1.0, 1.0));
    } else {
        // Sort points by X
        std::sort(m_points.begin(), m_points.end(), [](const QPointF& a, const QPointF& b) {
            return a.x() < b.x();
        });
        
        // Auto-scale Y if needed (expand bounds to fit points)
        double curMinY = m_minY;
        double curMaxY = m_maxY;
        for (const QPointF& p : m_points) {
            if (p.y() < curMinY) curMinY = p.y();
            if (p.y() > curMaxY) curMaxY = p.y();
        }
        if (curMinY != m_minY || curMaxY != m_maxY) {
            // Expand bounds with a little margin so points aren't exactly on edge
            double range = curMaxY - curMinY;
            if (range <= 0) range = 1.0;
            m_minY = curMinY - range * 0.1;
            m_maxY = curMaxY + range * 0.1;
        }
    }
    
    updatePath();
    update();
}

QString CurveGraphWidget::getTableString() const {
    QStringList pairs;
    for (const QPointF& p : m_points) {
        // format up to 4 decimals, omit trailing zeros
        QString xStr = QString::number(p.x(), 'f', 4);
        while(xStr.contains('.') && xStr.endsWith('0')) xStr.chop(1);
        if(xStr.endsWith('.')) xStr.chop(1);
        
        QString yStr = QString::number(p.y(), 'f', 4);
        while(yStr.contains('.') && yStr.endsWith('0')) yStr.chop(1);
        if(yStr.endsWith('.')) yStr.chop(1);
        
        pairs.append(xStr + "," + yStr);
    }
    return pairs.join(";");
}

QPointF CurveGraphWidget::mapToScreen(const QPointF& dataPoint) const {
    QRect r = rect().adjusted(10, 10, -10, -10);
    double rangeY = m_maxY - m_minY;
    if (rangeY <= 0) rangeY = 1.0;
    
    double screenX = r.left() + dataPoint.x() * r.width();
    double normalizedY = (dataPoint.y() - m_minY) / rangeY;
    double screenY = r.bottom() - normalizedY * r.height(); // inverted Y for screen
    
    return QPointF(screenX, screenY);
}

QPointF CurveGraphWidget::mapToData(const QPoint& screenPoint) const {
    QRect r = rect().adjusted(10, 10, -10, -10);
    double rangeY = m_maxY - m_minY;
    if (rangeY <= 0) rangeY = 1.0;
    
    double dataX = static_cast<double>(screenPoint.x() - r.left()) / r.width();
    dataX = qBound(0.0, dataX, 1.0);
    
    double normalizedY = static_cast<double>(r.bottom() - screenPoint.y()) / r.height();
    double dataY = m_minY + normalizedY * rangeY;
    
    return QPointF(dataX, dataY);
}

void CurveGraphWidget::updatePath() {
    m_curvePath = QPainterPath();
    if (m_points.isEmpty()) return;
    
    m_curvePath.moveTo(mapToScreen(m_points.first()));
    for (int i = 1; i < m_points.size(); ++i) {
        m_curvePath.lineTo(mapToScreen(m_points[i]));
    }
}

void CurveGraphWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(30, 30, 32));
    painter.drawRoundedRect(rect(), 6, 6);
    
    QRect r = rect().adjusted(10, 10, -10, -10);
    
    // Grid lines
    painter.setPen(QPen(QColor(60, 60, 64), 1, Qt::DashLine));
    double zeroY = mapToScreen(QPointF(0, 0)).y();
    if (zeroY > r.top() && zeroY < r.bottom()) {
        painter.drawLine(r.left(), zeroY, r.right(), zeroY);
    }
    
    painter.drawLine(r.left(), r.bottom(), r.right(), r.bottom());
    painter.drawLine(r.left(), r.top(), r.left(), r.bottom());

    // Curve Line
    painter.setPen(QPen(QColor(0, 180, 255), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(m_curvePath);

    // Points
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF sc = mapToScreen(m_points[i]);
        if (i == m_dragIndex) {
            painter.setBrush(QColor(255, 255, 255));
            painter.setPen(QPen(QColor(0, 180, 255), 2));
            painter.drawEllipse(sc, 5, 5);
        } else {
            painter.setBrush(QColor(0, 180, 255));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(sc, 4, 4);
        }
    }
}

void CurveGraphWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updatePath();
}

void CurveGraphWidget::mousePressEvent(QMouseEvent* event) {
    m_dragIndex = -1;
    // Find if clicked on existing point
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF sc = mapToScreen(m_points[i]);
        double dist = qHypot(sc.x() - event->pos().x(), sc.y() - event->pos().y());
        if (dist <= 8.0) {
            m_dragIndex = i;
            update();
            return;
        }
    }
    
    // If double clicked, see mouseDoubleClickEvent. 
    // Wait, let's just let double-click handle additions.
}

void CurveGraphWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragIndex >= 0) {
        QPointF dataPt = mapToData(event->pos());
        
        // Constrain X so points don't cross each other
        if (m_dragIndex == 0) {
            dataPt.setX(0.0); // First point always at X=0
        } else if (m_dragIndex == m_points.size() - 1) {
            dataPt.setX(1.0); // Last point always at X=1
        } else {
            double minX = m_points[m_dragIndex - 1].x() + 0.01;
            double maxX = m_points[m_dragIndex + 1].x() - 0.01;
            dataPt.setX(qBound(minX, dataPt.x(), maxX));
        }
        
        m_points[m_dragIndex] = dataPt;
        updatePath();
        update();
        emit tableChanged(getTableString());
    }
}

void CurveGraphWidget::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    if (m_dragIndex >= 0) {
        m_dragIndex = -1;
        update();
    }
}

void CurveGraphWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    // Check if clicked on a point to delete it
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF sc = mapToScreen(m_points[i]);
        double dist = qHypot(sc.x() - event->pos().x(), sc.y() - event->pos().y());
        if (dist <= 8.0) {
            // Don't delete the first or last point
            if (i > 0 && i < m_points.size() - 1) {
                m_points.removeAt(i);
                updatePath();
                update();
                emit tableChanged(getTableString());
            }
            return;
        }
    }
    
    // Otherwise, add a new point
    QPointF newPt = mapToData(event->pos());
    // Insert sorted
    for (int i = 0; i < m_points.size(); ++i) {
        if (m_points[i].x() > newPt.x()) {
            m_points.insert(i, newPt);
            break;
        }
    }
    updatePath();
    update();
    emit tableChanged(getTableString());
}
