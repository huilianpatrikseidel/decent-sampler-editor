#pragma once
#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QPainterPath>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT CurveGraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit CurveGraphWidget(QWidget* parent = nullptr);

    // Parses a Decent Sampler translationTable string (e.g., "0.0,0.0; 0.5,0.8; 1.0,1.0")
    void setTableString(const QString& tableStr);
    QString getTableString() const;

    // Defines the limits for the Y axis
    void setYRange(double minY, double maxY);

signals:
    void tableChanged(const QString& newTableStr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPointF mapToScreen(const QPointF& dataPoint) const;
    QPointF mapToData(const QPoint& screenPoint) const;
    void updatePath();
    
    QVector<QPointF> m_points; // X is always 0.0 to 1.0
    double m_minY = 0.0;
    double m_maxY = 1.0;
    
    int m_dragIndex = -1;
    QPainterPath m_curvePath;
};
