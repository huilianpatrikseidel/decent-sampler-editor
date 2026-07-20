#pragma once
#include <QGraphicsObject>
#include <QUuid>
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"
#include "FilmstripRenderer.h"
#include <QPainter>

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT UiComponentItem : public QGraphicsObject {
    Q_OBJECT
public:
    UiComponentItem(const UiComponent* comp, QGraphicsItem* parent = nullptr);
    
    enum { Type = UserType + 2 };
    int type() const override { return Type; }
    
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void paintComponent(QPainter *painter) = 0; // Pure virtual
    
    QUuid getComponentId() const { return m_id; }
    void updateFromComponent(const UiComponent* comp);
    virtual void updateComponentSpecifics(const UiComponent* comp) {}

signals:
    void componentResized(const QUuid& id, double newX, double newY, double newW, double newH);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    enum Handle { None, TopLeft, TopRight, BottomLeft, BottomRight, Left, Right, Top, Bottom };
    Handle m_currentHandle = None;
    bool m_isResizing = false;
    QRectF m_origRect;
    QPointF m_origPos;
    
    Handle getHandleAt(const QPointF& pos) const;
    void updateCursor(Handle handle);
    
protected:
    QUuid m_id;
    QString m_compType;
    QString m_name;
    double m_width;
    double m_height;
};
