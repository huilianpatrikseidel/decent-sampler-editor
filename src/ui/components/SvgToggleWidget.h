#pragma once
#include <QWidget>
#include <QString>
#include <QSvgRenderer>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT SvgToggleWidget : public QWidget {
    Q_OBJECT
public:
    SvgToggleWidget(const QString& svgOff, const QString& svgOn, QWidget* parent = nullptr);
    
    bool isChecked() const { return m_checked; }
    void setChecked(bool checked);
    
signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    
private:
    bool m_checked;
    QSvgRenderer m_rendererOff;
    QSvgRenderer m_rendererOn;
};
