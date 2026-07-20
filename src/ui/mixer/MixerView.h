#pragma once
#include <QWidget>
#include <QMap>
#include <QUuid>


#include "ui/mixer/MixerLibExport.h"
class ApplicationController;
class ProjectManager;
class QHBoxLayout;
class MixerChannelStrip;
class QScrollArea;

class MIXERLIB_EXPORT MixerView : public QWidget {
    Q_OBJECT
public:
    explicit MixerView(ApplicationController* appCtrl, QWidget *parent = nullptr);
    
public slots:
    void updateChannels();
    
private:
    ApplicationController* m_appCtrl;
    ProjectManager* m_pm;
    QHBoxLayout* m_channelsLayout;
    QWidget* m_channelsContainer;
    QScrollArea* m_scrollArea;
    QMap<QUuid, MixerChannelStrip*> m_strips;
    MixerChannelStrip* m_masterStrip = nullptr;
};
