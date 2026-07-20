#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOption>
#include <QHoverEvent>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT RangeSliderWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(double lowerValue READ lowerValue WRITE setLowerValue NOTIFY rangeChanged)
    Q_PROPERTY(double upperValue READ upperValue WRITE setUpperValue NOTIFY rangeChanged)

public:
    explicit RangeSliderWidget(QWidget* parent = nullptr);

    double minimum() const { return m_minimum; }
    void setMinimum(double min);

    double maximum() const { return m_maximum; }
    void setMaximum(double max);

    double lowerValue() const { return m_lowerValue; }
    void setLowerValue(double val);

    double upperValue() const { return m_upperValue; }
    void setUpperValue(double val);

    void setRange(double min, double max);
    void setValues(double lower, double upper);

signals:
    void rangeChanged(double lower, double upper);
    void rangeEditingFinished(double lower, double upper);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    double m_minimum = 0.0;
    double m_maximum = 100.0;
    double m_lowerValue = 25.0;
    double m_upperValue = 75.0;

    enum Handle { None, Lower, Upper, Center };
    Handle m_activeHandle = None;
    int m_offset = 0;

    QRect getLowerHandleRect() const;
    QRect getUpperHandleRect() const;
    QRect getTrackRect() const;
    double pixelToValue(int x) const;
    int valueToPixel(double val) const;
};
