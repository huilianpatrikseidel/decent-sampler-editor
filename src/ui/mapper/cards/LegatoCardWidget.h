#pragma once
#include "SynthCardBase.h"
#include <QUuid>


#include "ui/mapper/MapperLibExport.h"
class SampleGroup;
class SynthKnobWidget;
class QPushButton;
class ProjectManager;

class MAPPERLIB_EXPORT LegatoCardWidget : public SynthCardBase {
    Q_OBJECT
public:
    explicit LegatoCardWidget(ProjectManager* pm, SampleGroup* sg, const QUuid& oscId, QWidget* parent = nullptr);

private:
    ProjectManager* m_pm;
    SampleGroup* m_sg;
    QUuid m_oscId;
    
    QPushButton* m_legatoBtn;
    SynthKnobWidget* m_glideKnob;
};
