#pragma once
#include <QWidget>
#include <QUuid>


#include "ui/mixer/MixerLibExport.h"
class ProjectManager;
class ApplicationController;
class FaderWidget;
class KnobWidget;
class QLabel;
class SvgToggleWidget;
class FxRackWidget;

class MIXERLIB_EXPORT MixerChannelStrip : public QWidget {
    Q_OBJECT
public:
    explicit MixerChannelStrip(ApplicationController* appCtrl, const QUuid& sgId, QWidget* parent = nullptr);
    
    void updateFromNode();

signals:
    void masterVolumeChanged(double value);

private:
    ApplicationController* m_appCtrl;
    ProjectManager* m_pm;
    QUuid m_sgId;
    
    FaderWidget* m_volumeFader;
    KnobWidget* m_panDial;
    QLabel* m_panValueLabel;
    QLabel* m_nameLabel;
    
    SvgToggleWidget* m_btnMute;
    SvgToggleWidget* m_btnSolo;
    
    FxRackWidget* m_fxRackWidget;
    
    bool m_updating = false;
};
