#include "ZoneItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QCursor>
#include <QFileInfo>
#include "../../core/ThemePalette.h"

ZoneItem::ZoneItem(const Zone* zone, int index, QGraphicsItem* parent) : QGraphicsObject(parent), m_zone(zone), m_index(index) {
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
}

void ZoneItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    m_isHovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void ZoneItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    QPointF p = event->pos();
    int hs = 6;
    if (p.x() < hs && p.y() < hs) setCursor(Qt::SizeFDiagCursor);
    else if (p.x() > m_rect.width() - hs && p.y() < hs) setCursor(Qt::SizeBDiagCursor);
    else if (p.x() < hs && p.y() > m_rect.height() - hs) setCursor(Qt::SizeBDiagCursor);
    else if (p.x() > m_rect.width() - hs && p.y() > m_rect.height() - hs) setCursor(Qt::SizeFDiagCursor);
    else if (p.x() < hs || p.x() > m_rect.width() - hs) setCursor(Qt::SizeHorCursor);
    else if (p.y() < hs || p.y() > m_rect.height() - hs) setCursor(Qt::SizeVerCursor);
    else setCursor(Qt::ArrowCursor);
    
    QGraphicsObject::hoverMoveEvent(event);
}

void ZoneItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    m_isHovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

void ZoneItem::updateFromZone(const Zone* zone, double noteWidth, double velocityHeight) {
    m_zone = zone;
    m_noteWidth = noteWidth;
    m_velocityHeight = velocityHeight;
    
    double x = zone->loNote * noteWidth;
    double y = (127 - zone->hiVel) * velocityHeight;
    double width = (zone->hiNote - zone->loNote + 1) * noteWidth;
    double height = (zone->hiVel - zone->loVel + 1) * velocityHeight;
    
    prepareGeometryChange();
    m_rect = QRectF(0, 0, width, height);
    setPos(x, y);
}

QRectF ZoneItem::boundingRect() const {
    return m_rect.adjusted(-4, -4, 4, 4);
}

void ZoneItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    // Background gradient
    QLinearGradient grad(0, 0, 0, m_rect.height());
    
    if (isSelected()) {
        grad.setColorAt(0.0, ThemePalette::color("qss_color_21", QColor(130, 230, 130, 210)));
        grad.setColorAt(1.0, ThemePalette::color("qss_color_21", QColor(90, 190, 90, 210)));
        painter->setPen(QPen(ThemePalette::color("qss_color_15", Qt::white), 2));
    } else if (m_isHovered) {
        grad.setColorAt(0.0, ThemePalette::color("qss_color_21", QColor(100, 200, 100, 180)));
        grad.setColorAt(1.0, ThemePalette::color("qss_color_21", QColor(70, 160, 70, 180)));
        painter->setPen(QPen(ThemePalette::color("qss_color_26", QColor(150, 255, 150)), 2));
    } else {
        grad.setColorAt(0.0, ThemePalette::color("qss_color_26", QColor(80, 160, 80, 150)));
        grad.setColorAt(1.0, ThemePalette::color("qss_color_26", QColor(50, 120, 50, 150)));
        painter->setPen(QPen(ThemePalette::color("qss_color_26", QColor(100, 200, 100)), 1));
    }
    
    painter->setBrush(grad);
    double radius = 4.0;
    painter->drawRoundedRect(m_rect, radius, radius);
    
    // Text rendering
    painter->setPen(ThemePalette::color("qss_color_8", Qt::white));
    QFont f = painter->font();
    f.setFamily("Segoe UI");
    f.setPixelSize(11);
    f.setBold(true);
    painter->setFont(f);
    
    QString filename = QFileInfo(m_zone->samplePath).fileName();
    
    // Elide text if too wide
    QFontMetrics fm(f);
    QString elidedText = fm.elidedText(filename, Qt::ElideRight, m_rect.width() - 8);
    
    // Draw at top left of zone with padding
    painter->drawText(m_rect.adjusted(4, 4, -4, -4), Qt::AlignLeft | Qt::AlignTop | Qt::TextSingleLine, elidedText);
    
    // Draw Root Note indicator
    if (m_zone->rootNote >= m_zone->loNote && m_zone->rootNote <= m_zone->hiNote) {
        double rootX = (m_zone->rootNote - m_zone->loNote) * m_noteWidth;
        double centerX = rootX + (m_noteWidth / 2.0);
        
        painter->setPen(QPen(ThemePalette::color("qss_color_35", QColor(100, 200, 255, 200)), 2, Qt::DashLine));
        painter->drawLine(QPointF(centerX, 0), QPointF(centerX, m_rect.height()));
        
        // Draw a small anchor circle at the bottom
        painter->setBrush(ThemePalette::color("qss_color_35", QColor(100, 200, 255)));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(QPointF(centerX, m_rect.height() - 6), 4, 4);
    }
}

QVariant ZoneItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        QPointF newPos = value.toPointF();
        newPos.setX(qRound(newPos.x() / m_noteWidth) * m_noteWidth);
        newPos.setY(qRound(newPos.y() / m_velocityHeight) * m_velocityHeight);
        return newPos;
    }
    if (change == ItemSelectedHasChanged && isSelected()) {
        emit zoneSelected(m_index);
    }
    return QGraphicsObject::itemChange(change, value);
}

void ZoneItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QPointF p = event->pos();
    int hs = 6;
    if (p.x() < hs && p.y() < hs) m_resizingMode = TopLeft;
    else if (p.x() > m_rect.width() - hs && p.y() < hs) m_resizingMode = TopRight;
    else if (p.x() < hs && p.y() > m_rect.height() - hs) m_resizingMode = BottomLeft;
    else if (p.x() > m_rect.width() - hs && p.y() > m_rect.height() - hs) m_resizingMode = BottomRight;
    else if (p.x() < hs) m_resizingMode = Left;
    else if (p.x() > m_rect.width() - hs) m_resizingMode = Right;
    else if (p.y() < hs) m_resizingMode = Top;
    else if (p.y() > m_rect.height() - hs) m_resizingMode = Bottom;
    else m_resizingMode = None;
    
    QGraphicsObject::mousePressEvent(event);
}

void ZoneItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_resizingMode != None) {
        // Snap the current mouse scene position to grid
        QPointF sp = event->scenePos();
        double snappedX = qRound(sp.x() / m_noteWidth) * m_noteWidth;
        double snappedY = qRound(sp.y() / m_velocityHeight) * m_velocityHeight;
        
        double oldX = pos().x();
        double oldY = pos().y();
        double oldW = m_rect.width();
        double oldH = m_rect.height();
        
        double absLeft = oldX;
        double absTop = oldY;
        double absRight = absLeft + oldW;
        double absBottom = absTop + oldH;
        
        if (m_resizingMode == Left || m_resizingMode == TopLeft || m_resizingMode == BottomLeft) {
            absLeft = qMin(snappedX, absRight - m_noteWidth);
        }
        if (m_resizingMode == Right || m_resizingMode == TopRight || m_resizingMode == BottomRight) {
            absRight = qMax(snappedX, absLeft + m_noteWidth);
        }
        if (m_resizingMode == Top || m_resizingMode == TopLeft || m_resizingMode == TopRight) {
            absTop = qMin(snappedY, absBottom - m_velocityHeight);
        }
        if (m_resizingMode == Bottom || m_resizingMode == BottomLeft || m_resizingMode == BottomRight) {
            absBottom = qMax(snappedY, absTop + m_velocityHeight);
        }
        
        double dx = absLeft - oldX;
        double dy = absTop - oldY;
        double dw = (absRight - absLeft) - oldW;
        double dh = (absBottom - absTop) - oldH;

        if (dx != 0 || dy != 0 || dw != 0 || dh != 0) {
            for (auto item : scene()->selectedItems()) {
                if (ZoneItem* zi = dynamic_cast<ZoneItem*>(item)) {
                    if (zi == this) {
                        prepareGeometryChange();
                        m_rect = QRectF(0, 0, absRight - absLeft, absBottom - absTop);
                        setPos(absLeft, absTop);
                    } else {
                        zi->applyResizeDelta(dx, dy, dw, dh);
                    }
                }
            }
        }
    } else {
        QGraphicsObject::mouseMoveEvent(event);
    }
}

void ZoneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsObject::mouseReleaseEvent(event);
    m_resizingMode = None;
}

void ZoneItem::getVisualBounds(int& loNote, int& hiNote, int& loVel, int& hiVel) const {
    loNote = qBound(0, static_cast<int>(pos().x() / m_noteWidth), 127);
    hiNote = qBound(0, static_cast<int>((pos().x() + m_rect.width() - 1) / m_noteWidth), 127);
    hiVel = 127 - qBound(0, static_cast<int>(pos().y() / m_velocityHeight), 127);
    loVel = 127 - qBound(0, static_cast<int>((pos().y() + m_rect.height() - 1) / m_velocityHeight), 127);
    
    if (loNote > hiNote) std::swap(loNote, hiNote);
    if (loVel > hiVel) std::swap(loVel, hiVel);
}

void ZoneItem::applyResizeDelta(double dx, double dy, double dw, double dh) {
    prepareGeometryChange();
    m_rect.setWidth(qMax(m_noteWidth, m_rect.width() + dw));
    m_rect.setHeight(qMax(m_velocityHeight, m_rect.height() + dh));
    
    double newX = pos().x() + dx;
    double newY = pos().y() + dy;
    
    // Clamp slightly to scene to avoid infinite drifting
    if (newX < 0) newX = 0;
    if (newY < 0) newY = 0;
    
    setPos(newX, newY);
}
