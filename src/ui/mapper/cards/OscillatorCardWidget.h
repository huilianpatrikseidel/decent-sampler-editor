#pragma once
#include "SynthCardBase.h"
#include <QUuid>
#include "../../../core/ProjectManager.h"
#include "../../../core/models/AudioNodes.h"


#include "ui/mapper/MapperLibExport.h"
class MAPPERLIB_EXPORT OscillatorCardWidget : public SynthCardBase {
    Q_OBJECT
public:
    explicit OscillatorCardWidget(ProjectManager* pm, SampleGroup* sg, int oscCount, const QUuid& oscId, QWidget* parent = nullptr);
private:
    ProjectManager* m_pm;
    bool m_isUpdating = true;
    void saveOscParams(const QUuid& oscId, const QString& key, const QVariant& value);
    void saveTuning(const QUuid& oscId, int octave, int semi, double fine);
    void saveGroupProperty(const QUuid& oscId, const QString& key, const QVariant& value);
};
