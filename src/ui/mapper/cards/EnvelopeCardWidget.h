#pragma once
#include "SynthCardBase.h"
#include <QUuid>
#include "../../../core/ProjectManager.h"
#include "../../../core/models/AudioNodes.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT EnvelopeCardWidget : public SynthCardBase {
    Q_OBJECT
public:
    explicit EnvelopeCardWidget(ProjectManager* pm, SampleGroup* sg, int oscCount, const QUuid& oscId, QWidget* parent = nullptr);
private:
    ProjectManager* m_pm;
    bool m_isUpdating = true;
};
