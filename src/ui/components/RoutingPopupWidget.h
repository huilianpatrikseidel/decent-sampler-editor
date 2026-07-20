#pragma once
#include <QWidget>
#include "../../core/models/GraphStructures.h"


#include "ui/components/UiComponentsLibExport.h"
class SynthKnobWidget;
class ModRoutingWidget;

class UICOMPONENTSLIB_EXPORT RoutingPopupWidget : public QWidget {
    Q_OBJECT
public:
    RoutingPopupWidget(SynthKnobWidget* knob, const QStringList& sources, const ModRouting& routing);
    
protected:
    void paintEvent(QPaintEvent*) override;
    
private:
    SynthKnobWidget* m_knob;
};
