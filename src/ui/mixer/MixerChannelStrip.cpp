#include "MixerChannelStrip.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFontMetrics>
#include "../components/KnobWidget.h"
#include "../components/FaderWidget.h"
#include "../components/SvgToggleWidget.h"
#include "../../commands/ModifyPropertyCommand.h"
#include "../../core/ProjectManager.h"
#include "FxRackWidget.h"

#include "../../core/ApplicationController.h"

MixerChannelStrip::MixerChannelStrip(ApplicationController* appCtrl, const QUuid& sgId, QWidget* parent)
    : QWidget(parent), m_appCtrl(appCtrl), m_pm(appCtrl->getProjectManager()), m_sgId(sgId)
{
    setFixedWidth(90);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("MixerChannelStrip { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2a2a2a, stop:1 #1a1a1a); border: 1px solid #111; border-radius: 4px; }");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);
    
    // PAN Section
    QLabel* panLabel = new QLabel("PAN");
    panLabel->setAlignment(Qt::AlignCenter);
    panLabel->setStyleSheet("color: #888; font-size: 9px; font-weight: bold; background: transparent; border: none;");
    layout->addWidget(panLabel);
    
    m_panDial = new KnobWidget();
    m_panDial->setMinimum(-1.0);
    m_panDial->setMaximum(1.0);
    m_panDial->setValue(0);
    layout->addWidget(m_panDial, 0, Qt::AlignHCenter);
    
    m_panValueLabel = new QLabel("C");
    m_panValueLabel->setAlignment(Qt::AlignCenter);
    m_panValueLabel->setStyleSheet("color: #00ffff; font-size: 9px; background: transparent; border: none; font-family: monospace;");
    layout->addWidget(m_panValueLabel);
    
    // FX Rack
    m_fxRackWidget = new FxRackWidget(m_appCtrl, m_sgId);
    layout->addWidget(m_fxRackWidget);
    
    // Mute/Solo
    QHBoxLayout* msLayout = new QHBoxLayout();
    msLayout->setContentsMargins(0, 0, 0, 0);
    msLayout->setSpacing(4);
    
    m_btnMute = new SvgToggleWidget("btn_mute_off.svg", "btn_mute_on.svg");
    m_btnSolo = new SvgToggleWidget("btn_solo_off.svg", "btn_solo_on.svg");
    
    msLayout->addStretch();
    msLayout->addWidget(m_btnMute);
    msLayout->addWidget(m_btnSolo);
    msLayout->addStretch();
    layout->addLayout(msLayout);
    
    // Fader
    m_volumeFader = new FaderWidget(m_sgId.isNull()); // master uses true
    layout->addWidget(m_volumeFader, 1, Qt::AlignHCenter);
    
    m_nameLabel = new QLabel();
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setStyleSheet("background: #0a0a0a; color: #4caf50; border: 1px solid #000; border-radius: 3px; padding: 3px 2px; font-family: monospace; font-size: 10px;");
    layout->addWidget(m_nameLabel);
    
    updateFromNode();
    
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString&) {
        if ((m_sgId.isNull() && id.isNull()) || id == m_sgId) {
            updateFromNode();
        }
    });
    
    connect(m_volumeFader, &FaderWidget::valueChanged, this, [this](double value){
        if (m_updating) return;
        Node* n = m_pm->getNode(m_sgId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            if (sg->volume != value) {
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_sgId, "volume", sg->volume, value));
            }
        } else if (n && n->type == "Bus") {
            BusNode* b = static_cast<BusNode*>(n);
            if (b->volume != value) {
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_sgId, "volume", b->volume, value));
            }
        } else if (m_sgId.isNull()) {
            emit masterVolumeChanged(value);
        }
    });
    
    connect(m_panDial, &KnobWidget::valueChanged, this, [this](double value){
        if (value == 0) m_panValueLabel->setText("C");
        else if (value < 0) m_panValueLabel->setText(QString::number(static_cast<int>(-value * 100)) + " L");
        else m_panValueLabel->setText(QString::number(static_cast<int>(value * 100)) + " R");
        
        if (m_updating) return;
        Node* n = m_pm->getNode(m_sgId);
        if (n && n->type == "SampleGroup") {
            SampleGroup* sg = static_cast<SampleGroup*>(n);
            if (sg->pan != value) {
                m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_sgId, "pan", sg->pan, value));
            }
        }
    });
    
    connect(m_btnMute, &SvgToggleWidget::toggled, this, [this](bool checked){
        if (m_updating) return;
        Node* n = m_pm->getNode(m_sgId);
        if (n && n->muted != checked) {
            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_sgId, "muted", n->muted, checked));
        }
    });
    
    connect(m_btnSolo, &SvgToggleWidget::toggled, this, [this](bool checked){
        if (m_updating) return;
        Node* n = m_pm->getNode(m_sgId);
        if (n && n->soloed != checked) {
            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, m_sgId, "soloed", n->soloed, checked));
        }
    });
}

void MixerChannelStrip::updateFromNode() {
    m_updating = true;
    
    if (m_sgId.isNull()) {
        m_volumeFader->setValue(100);
        m_panDial->setValue(0);
        
        m_nameLabel->setText("Master");
        m_nameLabel->setToolTip("Master Bus");
    } else {
        Node* n = m_pm->getNode(m_sgId);
        if (n) {
            if (n->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(n);
                m_volumeFader->setValue(sg->volume);
                m_panDial->setValue(sg->pan);
                
                if (sg->pan == 0) m_panValueLabel->setText("C");
                else if (sg->pan < 0) m_panValueLabel->setText(QString::number(static_cast<int>(-sg->pan * 100)) + " L");
                else m_panValueLabel->setText(QString::number(static_cast<int>(sg->pan * 100)) + " R");
                
                m_btnMute->setChecked(sg->muted);
                m_btnSolo->setChecked(sg->soloed);
                
                QString name = sg->name.isEmpty() ? "Group" : sg->name;
                m_nameLabel->setText(QFontMetrics(m_nameLabel->font()).elidedText(name, Qt::ElideRight, 70));
                m_nameLabel->setToolTip(name);
            } else if (n->type == "Bus") {
                BusNode* bus = static_cast<BusNode*>(n);
                m_volumeFader->setValue(bus->volume);
                m_panDial->setValue(0); 
                m_panValueLabel->setText("C");
                m_btnMute->setChecked(bus->muted);
                m_btnSolo->setChecked(bus->soloed);
                
                QString name = bus->name.isEmpty() ? "Bus" : bus->name;
                m_nameLabel->setText(QFontMetrics(m_nameLabel->font()).elidedText(name, Qt::ElideRight, 70));
                m_nameLabel->setToolTip(name);
            }
        }
    }
    
    m_fxRackWidget->updateFromNode();
    m_updating = false;
}
