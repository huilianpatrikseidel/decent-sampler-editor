#pragma once
#include <QWidget>
#include <QString>
#include <QTimer>


#include "ui/components/UiComponentsLibExport.h"
class UICOMPONENTSLIB_EXPORT LFOGraphWidget : public QWidget {
    Q_OBJECT
public:
    explicit LFOGraphWidget(QWidget* parent = nullptr);

    void setParameters(const QString& shape, double rate, double phase);

protected:
    void paintEvent(QPaintEvent* event) override;
private:
    QString m_shape = "sine";
    double m_rate = 1.0;
    double m_phase = 0.0;
    
    QTimer m_animTimer;
    double m_animTime = 0.0;
};
