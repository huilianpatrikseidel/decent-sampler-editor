#pragma once
#include <QWidget>
#include <QSvgRenderer>
#include <QString>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT FaderWidget : public QWidget {
    Q_OBJECT
public:
    FaderWidget(bool isMaster = false, QWidget* parent = nullptr);
    
    double value() const { return m_value; }
    void setValue(double val); // 0.0 to 1.0
    
signals:
    void valueChanged(double newValue);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    
private:
    double m_value; // 0.0 to 1.0
    double m_defaultValue;
    bool m_dragging;
    int m_lastY;
    int m_capHeight;
    QSvgRenderer m_trackRenderer;
    QSvgRenderer m_capRenderer;
    
    int trackTopMargin() const { return m_capHeight / 2; }
    int trackBottomMargin() const { return m_capHeight / 2; }
    int trackLength() const { return height() - trackTopMargin() - trackBottomMargin(); }
    int valueToY() const;
    void updateValueFromY(int y);
};
