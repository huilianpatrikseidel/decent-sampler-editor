#pragma once

#include <QGraphicsObject>
#include <QPainter>
#include "core/ProjectManager.h"

#include "../designer/UiDesignerExport.h"

class UIDESIGNER_EXPORT CanvasKeyboardItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit CanvasKeyboardItem(ProjectManager* pm, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    
    void setGeometry(const QRectF& rect);
    
    void setLinearMode(bool active, double keyWidth = 20.0);
    
    int getActiveNote() const { return m_activeMouseNote; }

signals:
    void notePressed(int midiNote, int velocity);
    void noteReleased(int midiNote);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    ProjectManager* m_pm;
    QRectF m_rect;
    int m_padding;
    int m_gap;
    double m_wheelW;
    double m_sideBtnW;
    
    bool m_isLinearMode = false;
    double m_linearKeyWidth = 20.0;
    
    int m_kbStartNote;
    int m_activeMouseNote;
    
    int getMidiNoteAt(const QPointF& pos, int w) const;
};
