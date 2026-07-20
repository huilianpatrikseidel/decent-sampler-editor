#pragma once
#include <QGraphicsObject>
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT ZoneItem : public QGraphicsObject {
    Q_OBJECT
public:
    ZoneItem(const Zone* zone, int index, QGraphicsItem* parent = nullptr);

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void updateFromZone(const Zone* zone, double noteWidth, double velocityHeight);

    const Zone* getZone() const { return m_zone; }
    int getIndex() const { return m_index; }
    void getVisualBounds(int& loNote, int& hiNote, int& loVel, int& hiVel) const;
    void applyResizeDelta(double dx, double dy, double dw, double dh);

signals:
    void zoneSelected(int index);
    void zoneModified(int index, int loNote, int hiNote, int loVel, int hiVel);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    const Zone* m_zone;
    int m_index;
    QRectF m_rect;
    double m_noteWidth = 20.0;
    double m_velocityHeight = 4.0;
    
    enum ResizeHandle { None, TopLeft, TopRight, BottomLeft, BottomRight, Left, Right, Top, Bottom };
    ResizeHandle m_resizingMode = None;
    bool m_isHovered = false;
};
