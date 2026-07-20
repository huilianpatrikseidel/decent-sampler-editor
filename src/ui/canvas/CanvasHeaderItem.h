#pragma once

#include <QGraphicsItem>
#include <QPainter>
#include "core/ProjectManager.h"

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT CanvasHeaderItem : public QGraphicsItem {
public:
    explicit CanvasHeaderItem(ProjectManager* pm, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void setGeometry(const QRectF& rect);

private:
    ProjectManager* m_pm;
    int m_headerHeight;
    QRectF m_rect;
};
