#include "GroupEditorView.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include "../../commands/ModifyPropertyCommand.h"
#include "../inspector/AdsrEditorView.h"
#include "../components/EnvelopeGraphWidget.h"
#include "../components/FilterSectionWidget.h"
#include "../components/SynthKnobWidget.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QFormLayout>

GroupEditorView::GroupEditorView(ApplicationController* controller, QWidget* parent) 
    : QWidget(parent), m_controller(controller) {
    m_layout = new QHBoxLayout(this);
    setLayout(m_layout);
    
    connect(m_controller->getProjectManager(), &ProjectManager::nodeModified, this, &GroupEditorView::onNodeModified);
    
    rebuildForm();
}

void GroupEditorView::setGroup(const QUuid& sgId) {
    m_currentSgId = sgId;
    rebuildForm();
}

void GroupEditorView::setFocusMode(bool focus) {
    if (m_focusMode == focus) return;
    m_focusMode = focus;
    rebuildForm();
}

void GroupEditorView::onNodeModified(const QUuid& id, const QString& propertyName) {
    if (id == m_currentSgId && !m_isUpdatingUI) {
        rebuildForm();
    }
}

void GroupEditorView::rebuildForm() {
    m_isUpdatingUI = true;
    
    QLayoutItem* item;
    while ((item = m_layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    
    ProjectManager* pm = m_controller->getProjectManager();
    Node* node = pm->getNode(m_currentSgId);
    
    if (!node || node->type != "SampleGroup") {
        m_isUpdatingUI = false;
        return;
    }
    
    SampleGroup* sg = static_cast<SampleGroup*>(node);
    bool isSynth = sg->isSynthContainer || sg->isOscillator;
    
    // --- Lower Section: Columns ---
    QWidget* columnsWidget = new QWidget();
    QHBoxLayout* columnsLayout = new QHBoxLayout(columnsWidget);
    columnsLayout->setContentsMargins(0, 0, 0, 0);
    
    auto labelWithTooltip = [&](const QString& text, const QString& tooltip = QString()) {
        QLabel* label = new QLabel(text);
        if (!tooltip.isEmpty()) {
            label->setToolTip(tooltip);
            label->setToolTipDuration(10000);
        }
        return label;
    };
    
    // --- Column 1: Settings ---
    QWidget* settingsWidget = new QWidget();
    QFormLayout* settingsLayout = new QFormLayout(settingsWidget);
    settingsLayout->setContentsMargins(0, 0, 10, 0);
    
    if (m_focusMode) {
        settingsLayout->addRow(new QLabel(QString("<b>%1 Settings</b>").arg(sg->name)));
    } else {
        settingsLayout->addRow(new QLabel(QString("<b>%1 Compact Settings</b>").arg(sg->name)));
    }
    
    QDoubleSpinBox* volSpin = new QDoubleSpinBox();
    volSpin->setRange(-96.0, 12.0);
    volSpin->setValue(sg->volume);
    connect(volSpin, &QDoubleSpinBox::editingFinished, [this, sg, volSpin, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "volume", sg->volume, volSpin->value()));
    });
    settingsLayout->addRow(labelWithTooltip("Volume (dB)", "Group output volume in decibels."), volSpin);
    
    QDoubleSpinBox* panSpin = new QDoubleSpinBox();
    panSpin->setRange(-1.0, 1.0);
    panSpin->setValue(sg->pan);
    connect(panSpin, &QDoubleSpinBox::editingFinished, [this, sg, panSpin, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "pan", sg->pan, panSpin->value()));
    });
    settingsLayout->addRow(labelWithTooltip("Pan", "Left / right stereo balance."), panSpin);
    
    QComboBox* triggerCombo = new QComboBox();
    triggerCombo->addItems({"attack", "release", "first", "legato"});
    triggerCombo->setCurrentText(sg->trigger);
    connect(triggerCombo, &QComboBox::currentTextChanged, [this, sg, pm](const QString& text) {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "trigger", sg->trigger, text));
    });
    settingsLayout->addRow(labelWithTooltip("Trigger Event", "When the group begins playback."), triggerCombo);
    
    QComboBox* seqModeCombo = new QComboBox();
    seqModeCombo->addItems({"normal", "round_robin", "random", "true_random"});
    seqModeCombo->setCurrentText(sg->seqMode);
    connect(seqModeCombo, &QComboBox::currentTextChanged, [this, sg, pm](const QString& text) {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "seqMode", sg->seqMode, text));
    });
    settingsLayout->addRow(labelWithTooltip("Playback Mode", "Choose voice selection logic for samples."), seqModeCombo);
    
    QHBoxLayout* legatoLayout = new QHBoxLayout();
    QCheckBox* legatoCheck = new QCheckBox("Enable Legato");
    legatoCheck->setChecked(sg->legatoEnabled);
    connect(legatoCheck, &QCheckBox::checkStateChanged, [this, sg, pm](int state) {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "legatoEnabled", sg->legatoEnabled, state == Qt::Checked));
    });
    
    QDoubleSpinBox* glideSpin = new QDoubleSpinBox();
    glideSpin->setRange(0.0, 5.0);
    glideSpin->setSingleStep(0.05);
    glideSpin->setValue(sg->glideTime);
    connect(glideSpin, &QDoubleSpinBox::editingFinished, [this, sg, glideSpin, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "glideTime", sg->glideTime, glideSpin->value()));
    });
    
    legatoLayout->addWidget(legatoCheck);
    legatoLayout->addWidget(new QLabel("Glide Time (s):"));
    legatoLayout->addWidget(glideSpin);
    settingsLayout->addRow(labelWithTooltip("Portamento", "Legato and glide smoothing."), legatoLayout);
    
    QHBoxLayout* ccLayout = new QHBoxLayout();
    QSpinBox* loCC = new QSpinBox();
    loCC->setRange(0, 127);
    loCC->setValue(sg->loCC64);
    QSpinBox* hiCC = new QSpinBox();
    hiCC->setRange(0, 127);
    hiCC->setValue(sg->hiCC64);
    
    ccLayout->addWidget(new QLabel("Min:"));
    ccLayout->addWidget(loCC);
    ccLayout->addWidget(new QLabel("Max:"));
    ccLayout->addWidget(hiCC);
    
    connect(loCC, &QSpinBox::editingFinished, [this, sg, loCC, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "loCC64", sg->loCC64, loCC->value()));
    });
    connect(hiCC, &QSpinBox::editingFinished, [this, sg, hiCC, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "hiCC64", sg->hiCC64, hiCC->value()));
    });
    
    settingsLayout->addRow(labelWithTooltip("CC64 (Pedal) Range", "MIDI CC64 minimum and maximum values."), ccLayout);
    
    QWidget* voiceWidget = new QWidget();
    QFormLayout* voiceLayout = new QFormLayout(voiceWidget);
    voiceLayout->setContentsMargins(0, 0, 10, 0);
    voiceLayout->addRow(new QLabel("<b>Voice Control</b>"));

    QLineEdit* customTagsEdit = new QLineEdit(sg->customTags);
    connect(customTagsEdit, &QLineEdit::editingFinished, [this, sg, customTagsEdit, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "customTags", sg->customTags, customTagsEdit->text()));
    });
    voiceLayout->addRow("Custom Tags:", customTagsEdit);

    QLineEdit* silencedByEdit = new QLineEdit(sg->silencedByTags);
    connect(silencedByEdit, &QLineEdit::editingFinished, [this, sg, silencedByEdit, pm]() {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "silencedByTags", sg->silencedByTags, silencedByEdit->text()));
    });
    voiceLayout->addRow("Silenced By:", silencedByEdit);

    QComboBox* silencingModeCombo = new QComboBox();
    silencingModeCombo->addItems({"fast", "normal"});
    silencingModeCombo->setCurrentText(sg->silencingMode);
    connect(silencingModeCombo, &QComboBox::currentTextChanged, [this, sg, pm](const QString& text) {
        if (!m_isUpdatingUI) pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "silencingMode", sg->silencingMode, text));
    });
    voiceLayout->addRow("Silencing Mode:", silencingModeCombo);

    QWidget* filterWidgetContainer = new QWidget();
    QVBoxLayout* filterLayout = new QVBoxLayout(filterWidgetContainer);
    filterLayout->setContentsMargins(0, 0, 10, 0);
    filterLayout->addWidget(new QLabel("<b>Filter</b>"));

    FilterSectionWidget* filterWidget = new FilterSectionWidget();
    filterWidget->setFilterType(sg->filterType);
    filterWidget->setCutoff(sg->filterCutoff);
    filterWidget->setResonance(sg->filterResonance);
    filterWidget->setEnvDepth(sg->filterEnvDepth);
    filterWidget->setKeyTrack(sg->filterKeyTrack);
    filterWidget->setEnabled(!isSynth);

    connect(filterWidget, &FilterSectionWidget::filterChanged, this, [this, sg, pm](const QString& type, double cutoff, double resonance, double envDepth, double keyTrack) {
        if (m_isUpdatingUI) return;

        QJsonObject oldJson = sg->toJson();
        SampleGroup newSg = *sg;
        QString internalType = type;
        if (!internalType.isEmpty()) internalType[0] = internalType[0].toUpper();
        newSg.filterType = internalType;
        newSg.filterCutoff = cutoff;
        newSg.filterResonance = resonance;
        newSg.filterEnvDepth = envDepth;
        newSg.filterKeyTrack = keyTrack;

        pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, "", oldJson, newSg.toJson()));
    });
    filterLayout->addWidget(filterWidget);

    auto createEnvTab = [&](const ADSR& env, const QString& propName) {
        QWidget* tab = new QWidget();
        QVBoxLayout* tabLayout = new QVBoxLayout(tab);
        tabLayout->setContentsMargins(15, 15, 15, 15);
        tabLayout->setSpacing(10);

        EnvelopeGraphWidget* envGraph = new EnvelopeGraphWidget();
        envGraph->setParameters(env.attack, env.decay, env.sustain, env.release);
        tabLayout->addWidget(envGraph);

        QWidget* controlsBlock = new QWidget();
        controlsBlock->setObjectName("NeumorphicControls");
        controlsBlock->setAttribute(Qt::WA_StyledBackground, true);
        controlsBlock->setFixedHeight(85);
        QHBoxLayout* knobsLayout = new QHBoxLayout(controlsBlock);
        knobsLayout->setContentsMargins(0, 10, 0, 10);
        knobsLayout->setSpacing(16);

        knobsLayout->addStretch();
        
        struct EnvState { double a, d, s, r; };
        auto currentEnv = std::make_shared<EnvState>(EnvState{env.attack, env.decay, env.sustain, env.release});

        auto addEnvKnob = [&](const QString& label, double min, double max, double value, auto setter) {
            QVBoxLayout* kl = new QVBoxLayout();
            kl->setSpacing(2);
            QLabel* nl = new QLabel(label);
            nl->setStyleSheet("font-size: 12px; font-weight: 600;");
            kl->addWidget(nl, 0, Qt::AlignCenter);

            SynthKnobWidget* knob = new SynthKnobWidget();
            knob->setFixedSize(36, 36);
            knob->setMinimum(min);
            knob->setMaximum(max);
            double def = (label == "Sustain") ? 1.0 : 0.0;
            knob->setDefaultValue(def);
            knob->setValue(value);
            
            kl->addWidget(knob, 0, Qt::AlignCenter);

            QLabel* valueLabel = new QLabel(QString::number(value, 'f', 2));
            valueLabel->setStyleSheet("font-size: 10px; font-family: Consolas, monospace;");
            valueLabel->setAlignment(Qt::AlignCenter);
            kl->addWidget(valueLabel, 0, Qt::AlignCenter);

            connect(knob, &SynthKnobWidget::valueChanged, this, [this, knob, valueLabel, currentEnv, setter, propName, pm, sg, envGraph](double v) {
                if (m_isUpdatingUI) return;
                setter(*currentEnv, v);
                valueLabel->setText(QString::number(v, 'f', 2));
                envGraph->setParameters(currentEnv->a, currentEnv->d, currentEnv->s, currentEnv->r);
                
                QJsonObject oldJson = sg->toJson();
                SampleGroup newSg = *sg;
                if (propName == "ampEnv") {
                    newSg.ampEnv.attack = currentEnv->a;
                    newSg.ampEnv.decay = currentEnv->d;
                    newSg.ampEnv.sustain = currentEnv->s;
                    newSg.ampEnv.release = currentEnv->r;
                } else {
                    newSg.modEnv.attack = currentEnv->a;
                    newSg.modEnv.decay = currentEnv->d;
                    newSg.modEnv.sustain = currentEnv->s;
                    newSg.modEnv.release = currentEnv->r;
                }
                pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, propName, oldJson, newSg.toJson()));
            });

            knobsLayout->addLayout(kl);
            return knob;
        };

        addEnvKnob("Attack", 0.0, 10.0, env.attack, [](EnvState& e, double v){ e.a = v; });
        addEnvKnob("Decay", 0.0, 10.0, env.decay, [](EnvState& e, double v){ e.d = v; });
        addEnvKnob("Sustain", 0.0, 1.0, env.sustain, [](EnvState& e, double v){ e.s = v; });
        addEnvKnob("Release", 0.0, 10.0, env.release, [](EnvState& e, double v){ e.r = v; });

        knobsLayout->addStretch();
        tabLayout->addWidget(controlsBlock);
        
        return tab;
    };

    QWidget* envelopesWidget = new QWidget();
    QVBoxLayout* envWidgetLayout = new QVBoxLayout(envelopesWidget);
    envWidgetLayout->setContentsMargins(0, 0, 0, 0);
    envWidgetLayout->addWidget(new QLabel("<b>Envelopes</b>"));

    QTabWidget* envTabs = new QTabWidget();
    envTabs->addTab(createEnvTab(sg->ampEnv, "ampEnv"), "Amp Env");
    envTabs->addTab(createEnvTab(sg->modEnv, "modEnv"), "Mod Env (Env2)");
    envWidgetLayout->addWidget(envTabs);

    columnsLayout->addWidget(settingsWidget, 1);
    columnsLayout->addWidget(voiceWidget, 1);
    columnsLayout->addWidget(filterWidgetContainer, 1);
    columnsLayout->addWidget(envelopesWidget, 2);
    
    m_layout->addWidget(columnsWidget);
    
    m_isUpdatingUI = false;
}
