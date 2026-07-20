#pragma once
#include "SynthCardBase.h"
#include <QUuid>


#include "ui/mapper/MapperLibExport.h"
class ProjectManager;
struct SampleGroup;
class FilterSectionWidget;

class MAPPERLIB_EXPORT FilterCardWidget : public SynthCardBase {
    Q_OBJECT
public:
    explicit FilterCardWidget(ProjectManager* pm, SampleGroup* sg, const QUuid& oscId, QWidget* parent = nullptr);

private:
    ProjectManager* m_pm;
    QUuid m_oscId;
    FilterSectionWidget* m_filterWidget;
    bool m_isUpdating = false;
};
