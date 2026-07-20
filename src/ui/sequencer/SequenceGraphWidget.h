#pragma once
#include <QWidget>
#include <QVector>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "../../core/models/ProjectStates.h"

class SequenceGraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit SequenceGraphWidget(QWidget* parent = nullptr);
    
    void setSteps(const QVector<SequenceStep>& steps);
    void setActiveStep(int stepIndex);
    QVector<SequenceStep> getSteps() const;

signals:
    void stepsModified();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    void updateStepFromMouse(const QPoint& pos, Qt::KeyboardModifiers modifiers);
    
    QVector<SequenceStep> m_steps;
    int m_activeStep = -1;
};
