#include "MixerView.h"
#include "MixerChannelStrip.h"
#include "../../core/ApplicationController.h"
#include "../../audio/AudioMessage.h"
#include "../../audio/AudioEngine.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QSet>
#include "../../core/ProjectManager.h"

MixerView::MixerView(ApplicationController* appCtrl, QWidget *parent) 
    : QWidget(parent), m_appCtrl(appCtrl), m_pm(appCtrl->getProjectManager()) 
{
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #111; }");
    
    m_channelsContainer = new QWidget();
    m_channelsContainer->setStyleSheet("background-color: #111;");
    m_channelsLayout = new QHBoxLayout(m_channelsContainer);
    m_channelsLayout->setAlignment(Qt::AlignLeft);
    m_channelsLayout->setSpacing(4);
    m_channelsLayout->setContentsMargins(8, 8, 8, 8);
    
    m_scrollArea->setWidget(m_channelsContainer);
    mainLayout->addWidget(m_scrollArea, 1); // 1 = stretch factor so it takes all available space
    
    QFrame* separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("QFrame { border: 2px solid #222; }");
    mainLayout->addWidget(separator);
    
    m_masterStrip = new MixerChannelStrip(m_appCtrl, QUuid(), this);
    mainLayout->addWidget(m_masterStrip);
    
    connect(m_masterStrip, &MixerChannelStrip::masterVolumeChanged, this, [this](double val) {
        AudioMessage msg;
        msg.type = AudioCommandType::SetMasterVolume;
        msg.volume = static_cast<float>(val);
        m_appCtrl->getAudioEngine()->pushCommand(msg);
    });
    
    connect(m_pm, &ProjectManager::nodeAdded, this, &MixerView::updateChannels);
    connect(m_pm, &ProjectManager::nodeRemoved, this, &MixerView::updateChannels);
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (m_strips.contains(id)) {
            m_strips[id]->updateFromNode();
        }
    });
    
    updateChannels();
}

void MixerView::updateChannels() {
    QSet<QUuid> currentIds;
    const auto& nodes = m_pm->getAllNodes();
    for (const auto& pair : nodes) {
        if (pair.second->type == "SampleGroup" || pair.second->type == "Bus") {
            currentIds.insert(pair.first);
        }
    }
    
    auto it = m_strips.begin();
    while (it != m_strips.end()) {
        if (!currentIds.contains(it.key())) {
            m_channelsLayout->removeWidget(it.value());
            delete it.value();
            it = m_strips.erase(it);
        } else {
            ++it;
        }
    }
    
    for (const QUuid& id : currentIds) {
        if (!m_strips.contains(id)) {
            MixerChannelStrip* strip = new MixerChannelStrip(m_appCtrl, id, m_channelsContainer);
            m_channelsLayout->addWidget(strip);
            m_strips.insert(id, strip);
        }
    }
    
    if (m_masterStrip) {
        m_masterStrip->updateFromNode();
    }
}
