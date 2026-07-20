#pragma once
#include <QWidget>
#include <QString>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT ModSourceWidget : public QWidget {
    Q_OBJECT
public:
    explicit ModSourceWidget(const QString& sourceId, const QString& displayName, const QColor& color, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QString m_sourceId;
    QString m_displayName;
    QColor m_color;
    QPoint m_dragStartPosition;
};
