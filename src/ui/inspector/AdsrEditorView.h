#pragma once
#include <QWidget>
#include "core/models/AudioNodes.h"
#include "core/models/UiComponents.h"
#include "core/models/GraphStructures.h"
#include <QUuid>

class AdsrEditorView : public QWidget {
    Q_OBJECT
public:
    explicit AdsrEditorView(QWidget* parent = nullptr);
    void setAdsr(const ADSR& adsr, const QUuid& sgId);

signals:
    void adsrChanged(const QUuid& sgId, const ADSR& newAdsr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    ADSR m_adsr;
    QUuid m_sgId;
    enum DragNode { None, Attack, Decay, Sustain, Release };
    DragNode m_dragNode = None;
    
    QPointF getPoint(DragNode node) const;
};
