#pragma once
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../../core/ProjectManager.h"
#include "../../core/models/AudioNodes.h"


#include "ui/mapper/MapperLibExport.h"
class KnobWidget; // Forward declaration
class WaveformSelectorWidget;
class FilterSectionWidget;

class MAPPERLIB_EXPORT SynthEditorView : public QWidget {
    Q_OBJECT
public:
    explicit SynthEditorView(ProjectManager* pm, QWidget* parent = nullptr);
    void setSampleGroup(const QUuid& sgId);

private slots:
    void onNodeModified(const QUuid& id, const QString& prop);
    void onMacroChanged(int index, double value);

private:
    ProjectManager* m_pm;
    QUuid m_currentGroupId; // This is the synthParentId
    
    QVBoxLayout* m_mainLayout;
    
    void rebuildUi();
    bool m_isUpdating = false;
    int m_lastLfoCount = 0;
};
