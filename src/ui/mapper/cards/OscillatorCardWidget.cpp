#include "OscillatorCardWidget.h"
#include "../../../commands/ModifyPropertyCommand.h"
#include "../../../commands/RemoveNodeCommand.h"
#include "../../components/SynthKnobWidget.h"
#include "../../components/WaveformSelectorWidget.h"
#include "../../components/FilterSectionWidget.h"
#include "../../components/ModSourceHelper.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QComboBox>
#include "../../components/ModSourceHelper.h"

OscillatorCardWidget::OscillatorCardWidget(ProjectManager* pm, SampleGroup* sg, int oscCount, const QUuid& oscId, QWidget* parent)
    : SynthCardBase(QString("Oscillator %1").arg(oscCount), [pm, oscId]() { pm->getUndoStack()->push(new RemoveNodeCommand(pm, oscId)); }, parent),
      m_pm(pm) {
      
    QString initWf = sg->oscParams.waveform;
    if (initWf.length() > 0) initWf[0] = initWf[0].toUpper();
    if (initWf.isEmpty()) initWf = "Sine";
    setTitle(QString("Oscillator %1 - %2").arg(oscCount).arg(initWf));
    
    setEnableToggle(true, sg->oscEnabled);
    connect(this, &SynthCardBase::toggled, this, [this, oscId](bool checked) {
        if (m_isUpdating) return;
        saveGroupProperty(oscId, "oscEnabled", checked);
    });
      
    QVBoxLayout* oscLayout = contentLayout();
    
    auto provider = ModSourceHelper::createProvider(pm);
    
    // ── Waveform Selector (strip of buttons + preview) ──
    WaveformSelectorWidget* waveSelector = new WaveformSelectorWidget();
    waveSelector->setWaveform(sg->oscParams.waveform);
    oscLayout->addWidget(waveSelector);
    
    connect(waveSelector, &WaveformSelectorWidget::waveformChanged, this, [this, oscId](const QString& wf) {
        if (m_isUpdating) return;
        saveOscParams(oscId, "waveform", wf);
    });
    
    // ── Wavetable panel (shown only when waveform == "wavetable") ──
    QWidget* wavetablePanel = new QWidget();
    wavetablePanel->setObjectName("NeumorphicInner");
    wavetablePanel->setAttribute(Qt::WA_StyledBackground, true);
    wavetablePanel->setFixedWidth(160);
    wavetablePanel->setFixedHeight(140);
    QVBoxLayout* wtLayout = new QVBoxLayout(wavetablePanel);
    wtLayout->setContentsMargins(10, 10, 10, 10);
    
    QPushButton* wtFileBtn = new QPushButton();
    if (!sg->oscParams.wavetableFile.isEmpty()) 
        wtFileBtn->setText(QFileInfo(sg->oscParams.wavetableFile).fileName());
    else 
        wtFileBtn->setText("Select Wavetable...");
    wtLayout->addWidget(wtFileBtn);
    
    QHBoxLayout* wtControlsLayout = new QHBoxLayout();
    QVBoxLayout* wtPosLayout = new QVBoxLayout();
    wtPosLayout->addWidget(new QLabel("Position"), 0, Qt::AlignCenter);
    SynthKnobWidget* wtPosKnob = new SynthKnobWidget();
    wtPosKnob->setModSourceProvider(provider);
    wtPosKnob->setFixedSize(40, 40);
    wtPosKnob->setMinimum(0); wtPosKnob->setMaximum(1);
    wtPosKnob->setDefaultValue(0.0);
    wtPosKnob->setValue(sg->oscParams.wavetablePosition);
    wtPosLayout->addWidget(wtPosKnob, 0, Qt::AlignCenter);
    wtControlsLayout->addLayout(wtPosLayout);
    
    QVBoxLayout* wtInterpLayout = new QVBoxLayout();
    wtInterpLayout->addWidget(new QLabel("Interp"), 0, Qt::AlignCenter);
    QPushButton* interpolationToggle = new QPushButton(sg->oscParams.wavetableFrameInterpolation ? "ON" : "OFF");
    interpolationToggle->setCheckable(true);
    interpolationToggle->setChecked(sg->oscParams.wavetableFrameInterpolation);
    interpolationToggle->setObjectName("NeumorphicToggle");
    interpolationToggle->setFixedSize(40, 36);
    wtInterpLayout->addWidget(interpolationToggle, 0, Qt::AlignCenter);
    wtControlsLayout->addLayout(wtInterpLayout);
    wtLayout->addLayout(wtControlsLayout);
    
    oscLayout->addWidget(wavetablePanel);
    wavetablePanel->setVisible(sg->oscParams.waveform == "wavetable");
    
    // ── Pluck panel (shown only when waveform == "pluck") ──
    QWidget* pluckPanel = new QWidget();
    pluckPanel->setObjectName("NeumorphicInner");
    pluckPanel->setAttribute(Qt::WA_StyledBackground, true);
    pluckPanel->setFixedWidth(150);
    pluckPanel->setFixedHeight(140);
    QVBoxLayout* pluckLayout = new QVBoxLayout(pluckPanel);
    pluckLayout->setContentsMargins(10, 10, 10, 10);
    
    QHBoxLayout* pluckTypeLayout = new QHBoxLayout();
    QComboBox* pluckTypeCombo = new QComboBox();
    pluckTypeCombo->addItems({"Karplus-Strong"});
    pluckTypeLayout->addWidget(pluckTypeCombo);
    pluckLayout->addLayout(pluckTypeLayout);
    
    QVBoxLayout* dampingLayout = new QVBoxLayout();
    dampingLayout->addWidget(new QLabel("Damping"), 0, Qt::AlignCenter);
    SynthKnobWidget* dampingKnob = new SynthKnobWidget();
    dampingKnob->setModSourceProvider(provider);
    dampingKnob->setFixedSize(40, 40);
    dampingKnob->setMinimum(0); dampingKnob->setMaximum(1);
    dampingKnob->setDefaultValue(0.5);
    dampingKnob->setValue(sg->oscParams.damping);
    dampingLayout->addWidget(dampingKnob, 0, Qt::AlignCenter);
    pluckLayout->addLayout(dampingLayout);
    
    waveSelector->graphLayout()->addWidget(wavetablePanel, 0); // stretch = 0
    waveSelector->graphLayout()->addWidget(pluckPanel, 0);     // stretch = 0
    
    // Initial visibility and height setup
    bool isWt = (sg->oscParams.waveform == "wavetable");
    bool isPluck = (sg->oscParams.waveform == "pluck");
    wavetablePanel->setVisible(isWt);
    pluckPanel->setVisible(isPluck);
    
    // ── Pitch Controls (Octave, Semi, Fine) ──
    double totalTuning = sg->tuning;
    int octave = static_cast<int>(totalTuning / 12.0);
    int semi = static_cast<int>(totalTuning - octave * 12.0);
    double fine = (totalTuning - octave * 12.0 - semi) * 100.0;
    
    QWidget* pitchSection = new QWidget();
    pitchSection->setObjectName("NeumorphicControls");
    pitchSection->setAttribute(Qt::WA_StyledBackground, true);
    pitchSection->setFixedHeight(85);
    QHBoxLayout* pitchLayout = new QHBoxLayout(pitchSection);
    pitchLayout->setContentsMargins(0, 10, 0, 10);
    
    auto addPitchKnob = [&](const QString& label, double min, double max, double val, bool isInt, const QString& modDest) -> SynthKnobWidget* {
        QVBoxLayout* kl = new QVBoxLayout();
        kl->setSpacing(2);
        QLabel* nl = new QLabel(label);
        nl->setStyleSheet("font-size: 12px;  font-weight: 600;");
        kl->addWidget(nl, 0, Qt::AlignCenter);
        SynthKnobWidget* knob = new SynthKnobWidget();
        knob->setModSourceProvider(provider);
        knob->setFixedSize(36, 36);
        knob->setMinimum(min); knob->setMaximum(max);
        
        double def = 0.0;
        if (label == "Vol") def = 0.0;
        else if (label == "Pan") def = 0.0;
        else if (label == "Oct") def = 0.0;
        else if (label == "Semi") def = 0.0;
        else if (label == "Fine") def = 0.0;
        knob->setDefaultValue(def);
        
        knob->setValue(val);
        kl->addWidget(knob, 0, Qt::AlignCenter);
        
        QLabel* valLabel = new QLabel(isInt ? QString::number(static_cast<int>(val)) : QString::number(val, 'f', 1));
        valLabel->setStyleSheet("font-size: 10px;  font-family: Consolas, monospace;");
        valLabel->setAlignment(Qt::AlignCenter);
        kl->addWidget(valLabel, 0, Qt::AlignCenter);
        
        connect(knob, &SynthKnobWidget::valueChanged, valLabel, [valLabel, isInt](double v) {
            valLabel->setText(isInt ? QString::number(static_cast<int>(v)) : QString::number(v, 'f', 1));
        });
        
        if (!modDest.isEmpty()) {
            knob->setModDestination(modDest);
            for (const ModRouting& r : sg->routings) {
                if (r.destination == modDest || r.destination.startsWith(modDest + " - ")) {
                    knob->setModRouting(r);
                    break;
                }
            }
            
            connect(m_pm, &ProjectManager::nodeModified, knob, [this, knob, oscId, modDest](QUuid id, const QString& prop) {
                if (id == oscId && prop == "routings") {
                    Node* n = m_pm->getNode(oscId);
                    if (n && n->type == "SampleGroup") {
                        SampleGroup* sg = static_cast<SampleGroup*>(n);
                        bool found = false;
                        for (const ModRouting& r : sg->routings) {
                            if (r.destination == modDest || r.destination.startsWith(modDest + " - ")) {
                                knob->setModRouting(r);
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            ModRouting emptyR;
                            emptyR.destination = modDest;
                            knob->setModRouting(emptyR);
                        }
                    }
                }
            });
            
            connect(knob, &SynthKnobWidget::modulationChanged, this, [this, oscId](const ModRouting& newR) {
                if (m_isUpdating) return;
                Node* n = m_pm->getNode(oscId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* sg = static_cast<SampleGroup*>(n);
                    auto copy = sg->routings;
                    bool found = false;
                    for (int i=0; i<copy.size(); ++i) {
                        if (copy[i].destination == newR.destination) {
                            if (newR.source.isEmpty()) {
                                copy.removeAt(i);
                            } else {
                                copy[i] = newR;
                            }
                            found = true;
                            break;
                        }
                    }
                    if (!found && !newR.source.isEmpty()) {
                        copy.append(newR);
                    }
                    
                    QJsonArray oldArr, newArr;
                    for (const auto& r : sg->routings) oldArr.append(r.toJson());
                    for (const auto& r : copy) newArr.append(r.toJson());
                    m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "routings", oldArr, newArr));
                }
            });
        }
        
        pitchLayout->addLayout(kl);
        return knob;
    };
    
    SynthKnobWidget* octaveKnob = addPitchKnob("Oct", -4, 4, octave, true, "");
    SynthKnobWidget* semiKnob = addPitchKnob("Semi", -12, 12, semi, true, "Pitch"); // Only semi handles pitch modulation visually
    SynthKnobWidget* fineKnob = addPitchKnob("Fine", -100, 100, fine, true, "");
    
    QFrame* sep = new QFrame();
    sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet("");
    pitchLayout->addWidget(sep);
    
    SynthKnobWidget* volKnob = addPitchKnob("Vol", -96, 12, sg->volume, false, "Volume");
    volKnob->setBipolarDisplayEnabled(false);
    SynthKnobWidget* panKnob = addPitchKnob("Pan", -1, 1, sg->pan, false, "Pan");
    
    oscLayout->addWidget(pitchSection);
    
    // Connections
    connect(waveSelector, &WaveformSelectorWidget::waveformChanged, this, [this, wavetablePanel, pluckPanel, oscCount](const QString& wf) {
        bool isWt = (wf == "wavetable");
        bool isPluck = (wf == "pluck");
        wavetablePanel->setVisible(isWt);
        pluckPanel->setVisible(isPluck);
        
        QString wfName = wf;
        if (wfName.length() > 0) wfName[0] = wfName[0].toUpper();
        setTitle(QString("Oscillator %1 - %2").arg(oscCount).arg(wfName));
    });
    
    connect(wtFileBtn, &QPushButton::clicked, this, [this, oscId]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Select Wavetable", "", "WAV Files (*.wav);;All Files (*.*)");
        if (!filePath.isEmpty()) {
            saveOscParams(oscId, "wavetableFile", filePath);
        }
    });
    
    connect(interpolationToggle, &QPushButton::toggled, this, [this, oscId, interpolationToggle](bool checked) {
        if (m_isUpdating) return;
        interpolationToggle->setText(checked ? "ON" : "OFF");
        saveOscParams(oscId, "wavetableFrameInterpolation", checked);
    });
    
    connect(wtPosKnob, &SynthKnobWidget::valueChanged, this, [this, oscId](double v) {
        if (m_isUpdating) return;
        saveOscParams(oscId, "wavetablePosition", v);
    });
    
    connect(dampingKnob, &SynthKnobWidget::valueChanged, this, [this, oscId](double v) {
        if (m_isUpdating) return;
        saveOscParams(oscId, "damping", v);
    });
    
    auto onPitchChanged = [this, oscId, octaveKnob, semiKnob, fineKnob]() {
        if (m_isUpdating) return;
        int oct = static_cast<int>(octaveKnob->value());
        int sem = static_cast<int>(semiKnob->value());
        double fin = fineKnob->value();
        saveTuning(oscId, oct, sem, fin);
    };
    connect(octaveKnob, &SynthKnobWidget::valueChanged, this, onPitchChanged);
    connect(semiKnob, &SynthKnobWidget::valueChanged, this, onPitchChanged);
    connect(fineKnob, &SynthKnobWidget::valueChanged, this, onPitchChanged);
    
    connect(volKnob, &SynthKnobWidget::valueChanged, this, [this, oscId](double v) {
        if (m_isUpdating) return;
        saveGroupProperty(oscId, "volume", v);
    });
    connect(panKnob, &SynthKnobWidget::valueChanged, this, [this, oscId](double v) {
        if (m_isUpdating) return;
        saveGroupProperty(oscId, "pan", v);
    });
    
    oscLayout->addStretch();
    m_isUpdating = false;
}

void OscillatorCardWidget::saveOscParams(const QUuid& oscId, const QString& key, const QVariant& value) {
    Node* node = m_pm->getNode(oscId);
    if (node && node->type == "SampleGroup") {
        SampleGroup* sg = static_cast<SampleGroup*>(node);
        QJsonObject oldParams = sg->oscParams.toJson();
        QJsonObject newParams = oldParams;
        newParams[key] = QJsonValue::fromVariant(value);
        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, "oscParams", oldParams, newParams));
    }
}

void OscillatorCardWidget::saveTuning(const QUuid& oscId, int octave, int semi, double fine) {
    double tuning = (octave * 12.0) + semi + (fine / 100.0);
    saveGroupProperty(oscId, "tuning", tuning);
}

void OscillatorCardWidget::saveGroupProperty(const QUuid& oscId, const QString& key, const QVariant& value) {
    Node* node = m_pm->getNode(oscId);
    if (node && node->type == "SampleGroup") {
        SampleGroup* sg = static_cast<SampleGroup*>(node);
        QJsonObject oldJson = sg->toJson();
        QJsonObject newJson = oldJson;
        newJson[key] = QJsonValue::fromVariant(value);
        m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, oscId, key, oldJson, newJson));
    }
}
