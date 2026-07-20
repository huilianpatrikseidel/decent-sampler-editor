#pragma once
#include <QWidget>
#include <QVector>
#include "../components/KnobWidget.h"
#include "../components/ModSourceWidget.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT MacroPanelWidget : public QWidget {
    Q_OBJECT
public:
    explicit MacroPanelWidget(QWidget* parent = nullptr);

signals:
    void macroValueChanged(int index, double value);

private:
    struct Macro {
        KnobWidget* knob;
        ModSourceWidget* source;
    };
    QVector<Macro> m_macros;
};
