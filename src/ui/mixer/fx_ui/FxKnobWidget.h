#pragma once
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QString>
#include <QSvgRenderer>


#include "ui/mixer/MixerLibExport.h"
class MIXERLIB_EXPORT FxKnobWidget : public QWidget {
    Q_OBJECT
public:
    explicit FxKnobWidget(const QString& prefix, const QString& label, double min, double max, double defaultValue, const QString& suffix = "", QWidget* parent = nullptr);

    void setValue(double val);
    double value() const { return m_value; }

signals:
    void valueChanged(double newVal);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QString m_prefix;
    QString m_label;
    QString m_suffix;
    double m_value;
    double m_minimum;
    double m_maximum;
    double m_defaultValue;
    bool m_dragging;
    int m_lastY;
    
    QSvgRenderer* m_baseRenderer;
    QSvgRenderer* m_capRenderer;
};
