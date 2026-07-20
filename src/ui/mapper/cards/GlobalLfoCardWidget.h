#pragma once
#include "SynthCardBase.h"
#include <QUuid>
#include "../../../core/ProjectManager.h"
#include "../../../core/models/AudioNodes.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT GlobalLfoCardWidget : public SynthCardBase {
    Q_OBJECT
public:
    explicit GlobalLfoCardWidget(ProjectManager* pm, const LFO& lfoData, QWidget* parent = nullptr);
private:
    ProjectManager* m_pm;
    bool m_isUpdating = true;
    void saveLfoParams(const QUuid& lfoId, const QString& key, const QVariant& value);
};
