#include "MacroPanelWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "../../core/ThemePalette.h"

MacroPanelWidget::MacroPanelWidget(QWidget* parent) : QWidget(parent) {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(20);
    mainLayout->addStretch();
    
    QColor macroColors[] = {
        ThemePalette::color("macro_1", QColor(255, 50, 50)),
        ThemePalette::color("macro_2", QColor(50, 255, 50)),
        ThemePalette::color("macro_3", QColor(50, 150, 255)),
        ThemePalette::color("macro_4", QColor(255, 255, 50))
    };
    
    for (int i = 0; i < 4; ++i) {
        QWidget* col = new QWidget();
        QVBoxLayout* vlay = new QVBoxLayout(col);
        vlay->setAlignment(Qt::AlignCenter);
        
        KnobWidget* knob = new KnobWidget();
        knob->setMinimum(0.0);
        knob->setMaximum(1.0);
        knob->setValue(0.5);
        
        QString sourceId = QString("MACRO%1").arg(i + 1);
        ModSourceWidget* modSource = new ModSourceWidget(sourceId, QString::number(i + 1), macroColors[i]);
        
        QLabel* lbl = new QLabel(QString("Macro %1").arg(i + 1));
        lbl->setAlignment(Qt::AlignCenter);
        
        vlay->addWidget(modSource, 0, Qt::AlignCenter);
        vlay->addWidget(knob, 0, Qt::AlignCenter);
        vlay->addWidget(lbl, 0, Qt::AlignCenter);
        
        connect(knob, &KnobWidget::valueChanged, this, [this, i](double val){
            emit macroValueChanged(i, val);
        });
        
        m_macros.append({knob, modSource});
        mainLayout->addWidget(col);
    }
    
    mainLayout->addStretch(); // push to left
}
