#pragma once
#include <QWidget>
#include <QPoint>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT EnvelopeGraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit EnvelopeGraphWidget(QWidget* parent = nullptr);

    void setParameters(double attack, double decay, double sustain, double release);

signals:
    void parametersChanged(double attack, double decay, double sustain, double release);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    double m_attack = 0.5;   // 0 to 10 seconds typical, we can just normalize 0 to 1 for drawing
    double m_decay = 0.5;    // 0 to 10 seconds
    double m_sustain = 1.0;  // 0 to 1 amplitude
    double m_release = 0.5;  // 0 to 10 seconds
    
    // Interaction state
    enum DragState { None, DraggingAttack, DraggingDecay, DraggingSustain, DraggingRelease };
    DragState m_dragState = None;
    
    QPointF getAttackNode() const;
    QPointF getDecayNode() const;
    QPointF getReleaseNode() const;
};
