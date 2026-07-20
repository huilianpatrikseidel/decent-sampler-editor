#pragma once
#include <QWidget>
#include "../../core/models/GraphStructures.h"
#include "ModSourceHelper.h"
#include "ui/components/UiComponentsLibExport.h"

class SynthKnobWidget;
class ModRoutingWidget;

class UICOMPONENTSLIB_EXPORT RoutingPopupWidget : public QWidget {
    Q_OBJECT
public:
    RoutingPopupWidget(SynthKnobWidget* knob, const QList<ModSourceHelper::ModSource>& sources, const QVector<ModRouting>& routings);
    
protected:
    void paintEvent(QPaintEvent*) override;
    
private:
    SynthKnobWidget* m_knob;
};
