#include "UiComponentItem.h"
#include <QPainter>
#include <cmath>
#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include "../../core/ThemePalette.h"

UiComponentItem::UiComponentItem(const UiComponent* comp, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_id(comp->id), m_compType(comp->type), m_name(comp->name) {
    
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true); // Required for cursor updates on hover
    
    updateFromComponent(comp);
}

void UiComponentItem::updateFromComponent(const UiComponent* comp) {
    if (comp->id != m_id) return;
    
    m_name = comp->name;
    m_width = comp->width;
    m_height = comp->height;
    setPos(comp->x, comp->y);
    setZValue(comp->zIndex);
    
    updateComponentSpecifics(comp);
    update();
}

QRectF UiComponentItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height).adjusted(-4, -4, 4, 4);
}

void UiComponentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    paintComponent(painter);
    
    if (isSelected()) {
        painter->setPen(QPen(ThemePalette::color("qss_color_6", Qt::yellow), 1, Qt::DashLine));
        painter->drawRect(boundingRect());
        
        // Draw resize handles
        painter->setBrush(ThemePalette::color("qss_color_15", Qt::white));
        painter->setPen(ThemePalette::color("qss_color_8", Qt::black));
        int hs = 6; // handle size
        painter->drawRect(0 - hs/2, 0 - hs/2, hs, hs); // TL
        painter->drawRect(m_width - hs/2, 0 - hs/2, hs, hs); // TR
        painter->drawRect(0 - hs/2, m_height - hs/2, hs, hs); // BL
        painter->drawRect(m_width - hs/2, m_height - hs/2, hs, hs); // BR
        painter->drawRect(m_width/2 - hs/2, 0 - hs/2, hs, hs); // T
        painter->drawRect(m_width/2 - hs/2, m_height - hs/2, hs, hs); // B
        painter->drawRect(0 - hs/2, m_height/2 - hs/2, hs, hs); // L
        painter->drawRect(m_width - hs/2, m_height/2 - hs/2, hs, hs); // R
    }
}

QVariant UiComponentItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionChange && scene() && !m_isResizing) {
        QPointF newPos = value.toPointF();
        return newPos;
    }
    return QGraphicsObject::itemChange(change, value);
}

UiComponentItem::Handle UiComponentItem::getHandleAt(const QPointF& pos) const {
    if (!isSelected()) return None;
    int hs = 8; // hit tolerance
    bool left = pos.x() >= -hs && pos.x() <= hs;
    bool right = pos.x() >= m_width - hs && pos.x() <= m_width + hs;
    bool top = pos.y() >= -hs && pos.y() <= hs;
    bool bottom = pos.y() >= m_height - hs && pos.y() <= m_height + hs;
    bool midX = pos.x() >= m_width/2 - hs && pos.x() <= m_width/2 + hs;
    bool midY = pos.y() >= m_height/2 - hs && pos.y() <= m_height/2 + hs;
    
    if (left && top) return TopLeft;
    if (right && top) return TopRight;
    if (left && bottom) return BottomLeft;
    if (right && bottom) return BottomRight;
    if (left && midY) return Left;
    if (right && midY) return Right;
    if (top && midX) return Top;
    if (bottom && midX) return Bottom;
    return None;
}

void UiComponentItem::updateCursor(Handle handle) {
    switch (handle) {
        case TopLeft: case BottomRight: setCursor(Qt::SizeFDiagCursor); break;
        case TopRight: case BottomLeft: setCursor(Qt::SizeBDiagCursor); break;
        case Left: case Right: setCursor(Qt::SizeHorCursor); break;
        case Top: case Bottom: setCursor(Qt::SizeVerCursor); break;
        default: setCursor(Qt::ArrowCursor); break;
    }
}

void UiComponentItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    if (isSelected()) {
        updateCursor(getHandleAt(event->pos()));
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QGraphicsObject::hoverMoveEvent(event);
}

#include <QGraphicsSceneMouseEvent>
void UiComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Handle h = getHandleAt(event->pos());
    if (h != None) {
        m_currentHandle = h;
        m_isResizing = true;
        m_origRect = boundingRect();
        m_origPos = pos();
        event->accept();
        return;
    }
    m_isResizing = false;
    QGraphicsObject::mousePressEvent(event);
}

void UiComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_isResizing) {
        QPointF pt = event->pos();
        double newX = m_origPos.x(), newY = m_origPos.y();
        double newW = m_origRect.width(), newH = m_origRect.height();
        
        if (m_currentHandle == Left || m_currentHandle == TopLeft || m_currentHandle == BottomLeft) {
            double diff = pt.x();
            newX += diff; newW -= diff;
        }
        if (m_currentHandle == Right || m_currentHandle == TopRight || m_currentHandle == BottomRight) {
            newW = pt.x();
        }
        if (m_currentHandle == Top || m_currentHandle == TopLeft || m_currentHandle == TopRight) {
            double diff = pt.y();
            newY += diff; newH -= diff;
        }
        if (m_currentHandle == Bottom || m_currentHandle == BottomLeft || m_currentHandle == BottomRight) {
            newH = pt.y();
        }
        
        if (newW >= 10 && newH >= 10) {
            prepareGeometryChange();
            m_width = newW;
            m_height = newH;
            setPos(newX, newY);
            update();
        }
        event->accept();
    } else {
        QGraphicsObject::mouseMoveEvent(event);
    }
}

void UiComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_isResizing) {
        m_isResizing = false;
        emit componentResized(m_id, pos().x(), pos().y(), m_width, m_height);
        event->accept();
    } else {
        QGraphicsObject::mouseReleaseEvent(event);
    }
}
