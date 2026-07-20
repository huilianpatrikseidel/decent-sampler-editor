#pragma once
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QSvgRenderer>


#include "ui/mixer/MixerLibExport.h"
class MIXERLIB_EXPORT FxSwitchWidget : public QWidget {
    Q_OBJECT
public:
    explicit FxSwitchWidget(const QString& prefix, const QString& label, bool defaultValue, QWidget* parent = nullptr);

    void setChecked(bool checked);
    bool isChecked() const { return m_checked; }

signals:
    void toggled(bool checked);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QString m_prefix;
    QString m_label;
    bool m_checked;
    
    QSvgRenderer* m_onRenderer;
    QSvgRenderer* m_offRenderer;
};
