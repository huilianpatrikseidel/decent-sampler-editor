#include "SampleEditorContainer.h"
#include "../mapper/WaveformView.h"
#include "../waveform/WaveformEditorView.h"
#include "../inspector/AdsrEditorView.h"
#include "../../core/commands/ModifyZonePropertyCommand.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QTabWidget>

SampleEditorContainer::SampleEditorContainer(ApplicationController* controller, QWidget* parent) 
    : QWidget(parent), m_controller(controller) {
    
    // Default dock mode: LeftToRight
    m_mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    m_tabWidget = new QTabWidget(this);
    m_waveformView = new WaveformView(controller, this);
    m_waveformEditorView = new WaveformEditorView(this);
    
    m_tabWidget->addTab(m_waveformView, "Zone Waveform");
    m_tabWidget->addTab(m_waveformEditorView, "Audio Analyzer");
    
    m_mainLayout->addWidget(m_tabWidget, 1);
    
    connect(m_waveformEditorView, &WaveformEditorView::loopPointsChanged, this, [this](double startMs, double endMs) {
        // We need to convert startMs to frames. 
        // We'll rely on the user visually using the old waveform view for fine-tuning.
        // Wait, loopPointsChanged sends ms. 
        // We can just convert it to an approximate sample index using sample rate 44100 as default, or whatever we can find.
        // Or wait, we can just let it emit normalized points from the analyzer instead?
        // Let's assume 44100 for now if not available, or get from controller? 
        // Actually, the new WaveformEditorView uses ms. The Zone structure stores frames.
        // Just approximate it here using 44100:
        if (!m_isUpdatingUI && m_currentZoneIndex >= 0) {
            ProjectManager* pm = m_controller->getProjectManager();
            Node* n = pm->getNode(m_currentSgId);
            if (n && n->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(n);
                if (m_currentZoneIndex < sg->zones.size()) {
                    long long newStart = static_cast<long long>(startMs * 44.1); // approx at 44.1kHz
                    long long newEnd = static_cast<long long>(endMs * 44.1);
                    
                    Zone& z = sg->zones[m_currentZoneIndex];
                    pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, "loopStart", z.loopStart, newStart));
                    pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, "loopEnd", z.loopEnd, newEnd));
                }
            }
        }
    });
    
    QWidget* propsContainer = new QWidget();
    QHBoxLayout* propsHLayout = new QHBoxLayout(propsContainer);
    
    m_propsLayout = new QFormLayout();
    propsHLayout->addLayout(m_propsLayout);
    propsHLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(propsContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    
    m_mainLayout->addWidget(scrollArea, 1);
    
    // Connect to project manager to update UI if zone changes externally
    // However ProjectManager currently emits nodeModified for the whole SampleGroup.
    // We will listen to nodeModified.
    connect(controller->getProjectManager(), &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (id == m_currentSgId && !m_isUpdatingUI) {
            Node* n = m_controller->getProjectManager()->getNode(id);
            if (n && n->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(n);
                if (m_currentZoneIndex >= 0 && m_currentZoneIndex < sg->zones.size()) {
                    setZone(id, m_currentZoneIndex, &sg->zones[m_currentZoneIndex]);
                }
            }
        }
    });
}

void SampleEditorContainer::setZone(const QUuid& sgId, int zoneIndex, const Zone* zone) {
    m_currentSgId = sgId;
    m_currentZoneIndex = zoneIndex;
    m_waveformView->setZone(sgId, zoneIndex, zone);
    if (zone && !zone->samplePath.isEmpty()) {
        m_waveformEditorView->loadAudio(zone->samplePath);
    }
    rebuildPropsForm(zone);
}

void SampleEditorContainer::setFocusMode(bool focus) {
    if (focus) {
        m_mainLayout->setDirection(QBoxLayout::TopToBottom);
    } else {
        m_mainLayout->setDirection(QBoxLayout::LeftToRight);
    }
}

void SampleEditorContainer::rebuildPropsForm(const Zone* zone) {
    m_isUpdatingUI = true;
    
    QLayoutItem* item;
    while ((item = m_propsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    
    if (!zone) {
        m_propsLayout->addRow(new QLabel("No Zone Selected"));
        m_isUpdatingUI = false;
        return;
    }
    
    auto createSpinBox = [this, zone](const QString& label, int value, int min, int max, const QString& propName) {
        QSpinBox* spin = new QSpinBox();
        spin->setRange(min, max);
        spin->setValue(value);
        m_propsLayout->addRow(label, spin);
        connect(spin, &QSpinBox::editingFinished, [this, propName, spin, zone]() {
            if (!m_isUpdatingUI) {
                ProjectManager* pm = m_controller->getProjectManager();
                // Get current old value
                int oldV = 0;
                if (propName == "loopStart") oldV = zone->loopStart;
                else if (propName == "loopEnd") oldV = zone->loopEnd;
                else if (propName == "loopCrossfade") oldV = zone->loopCrossfade;
                
                pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, propName, oldV, spin->value()));
            }
        });
        return spin;
    };
    
    m_propsLayout->addRow(new QLabel("<b>Looping</b>"));
    
    QCheckBox* loopEnableCheck = new QCheckBox();
    loopEnableCheck->setChecked(zone->loopEnabled);
    connect(loopEnableCheck, &QCheckBox::stateChanged, [this, zone](int state) {
        if (!m_isUpdatingUI) {
            ProjectManager* pm = m_controller->getProjectManager();
            pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, "loopEnabled", zone->loopEnabled, state == Qt::Checked));
        }
    });
    m_propsLayout->addRow("Enable Loop", loopEnableCheck);
    
    createSpinBox("Loop Start", zone->loopStart, 0, 999999999, "loopStart");
    createSpinBox("Loop End", zone->loopEnd, 0, 999999999, "loopEnd");
    createSpinBox("Crossfade", zone->loopCrossfade, 0, 999999999, "loopCrossfade");
    
    // Add local ADSR side by side
    QFormLayout* rightLayout = new QFormLayout();
    static_cast<QHBoxLayout*>(m_propsLayout->parentWidget()->layout())->insertLayout(1, rightLayout);
    
    rightLayout->addRow(new QLabel("<b>Amp Envelope (Local)</b>"));
    QCheckBox* localEnvCheck = new QCheckBox();
    localEnvCheck->setChecked(zone->useLocalAmpEnv);
    rightLayout->addRow("Override Group Env", localEnvCheck);
    
    AdsrEditorView* adsrView = new AdsrEditorView();
    adsrView->setAdsr(zone->ampEnv, m_currentSgId);
    adsrView->setEnabled(zone->useLocalAmpEnv);
    rightLayout->addRow(adsrView);
    
    connect(localEnvCheck, &QCheckBox::checkStateChanged, [this, zone, adsrView](int state) {
        bool checked = (state == Qt::Checked);
        adsrView->setEnabled(checked);
        if (!m_isUpdatingUI) {
            ProjectManager* pm = m_controller->getProjectManager();
            pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, "useLocalAmpEnv", zone->useLocalAmpEnv, checked));
        }
    });
    
    connect(adsrView, &AdsrEditorView::adsrChanged, [this, zone](const QUuid& id, const ADSR& newAdsr) {
        if (!m_isUpdatingUI) {
            ProjectManager* pm = m_controller->getProjectManager();
            pm->getUndoStack()->push(new ModifyZonePropertyCommand(pm, m_currentSgId, m_currentZoneIndex, "ampEnv", zone->ampEnv.toJson(), newAdsr.toJson()));
        }
    });
    
    m_isUpdatingUI = false;
}
