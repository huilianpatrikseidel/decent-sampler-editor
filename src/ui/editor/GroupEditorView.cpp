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
#include "../mapper/MacroPanelWidget.h"
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
    
    // --- Top Section: Macros (Synth Only) ---
    QWidget* macrosWidget = nullptr;
    if (isSynth) {
        macrosWidget = new QWidget();
        QVBoxLayout* macrosLayout = new QVBoxLayout(macrosWidget);
        macrosLayout->setContentsMargins(0, 0, 0, 10);
        
        QLabel* macroTitle = new QLabel("<b>Synth Macros</b>");
        macroTitle->setAlignment(Qt::AlignCenter);
        macrosLayout->addWidget(macroTitle);
        
        MacroPanelWidget* macroPanel = new MacroPanelWidget();
        macrosLayout->addWidget(macroPanel);
    }
    
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
        tabLayout->setContentsMargins(0, 0, 0, 0);
        tabLayout->setSpacing(8);

        AdsrEditorView* adsrView = new AdsrEditorView();
        adsrView->setAdsr(env, m_currentSgId);
        tabLayout->addWidget(adsrView);

        QWidget* knobsPanel = new QWidget();
        QHBoxLayout* knobsLayout = new QHBoxLayout(knobsPanel);
        knobsLayout->setContentsMargins(0, 0, 0, 0);
        knobsLayout->setSpacing(10);

        struct KnobDesc { QString label; double min; double max; double value; std::function<void(ADSR&, double)> setter; };
        auto currentEnv = std::make_shared<ADSR>(env);

        auto addEnvKnob = [&](const QString& label, double min, double max, double value, auto setter) {
            QWidget* cell = new QWidget();
            QVBoxLayout* cellLayout = new QVBoxLayout(cell);
            cellLayout->setContentsMargins(0, 0, 0, 0);
            cellLayout->setSpacing(4);
            QLabel* fieldLabel = new QLabel(label);
            fieldLabel->setStyleSheet("font-size: 12px; font-weight: 600;");
            cellLayout->addWidget(fieldLabel, 0, Qt::AlignCenter);

            SynthKnobWidget* knob = new SynthKnobWidget();
            knob->setFixedSize(36, 36);
            knob->setMinimum(min);
            knob->setMaximum(max);
            knob->setValue(value);
            knob->setLogarithmic(label != "Sustain");
            cellLayout->addWidget(knob, 0, Qt::AlignCenter);

            QLabel* valueLabel = new QLabel(QString::number(value, 'f', label == "Sustain" ? 2 : 2));
            valueLabel->setStyleSheet("font-size: 10px; font-family: Consolas, monospace;");
            valueLabel->setAlignment(Qt::AlignCenter);
            cellLayout->addWidget(valueLabel, 0, Qt::AlignCenter);

            connect(knob, &SynthKnobWidget::valueChanged, this, [this, knob, valueLabel, currentEnv, setter, propName, pm, sg, adsrView](double v) {
                if (m_isUpdatingUI) return;
                setter(*currentEnv, v);
                valueLabel->setText(QString::number(v, 'f', qMax(2, knob->isLogarithmic() ? 2 : 2)));
                adsrView->setAdsr(*currentEnv, m_currentSgId);
                QJsonObject oldJson = sg->toJson();
                SampleGroup newSg = *sg;
                if (propName == "ampEnv") newSg.ampEnv = *currentEnv;
                else newSg.modEnv = *currentEnv;
                pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, propName, oldJson, newSg.toJson()));
            });

            knobsLayout->addWidget(cell);
            return knob;
        };

        SynthKnobWidget* attackKnob = addEnvKnob("Attack", 0.0, 5.0, env.attack, [](ADSR& e, double v){ e.attack = v; });
        SynthKnobWidget* decayKnob = addEnvKnob("Decay", 0.0, 5.0, env.decay, [](ADSR& e, double v){ e.decay = v; });
        SynthKnobWidget* sustainKnob = addEnvKnob("Sustain", 0.0, 1.0, env.sustain, [](ADSR& e, double v){ e.sustain = v; });
        SynthKnobWidget* releaseKnob = addEnvKnob("Release", 0.0, 5.0, env.release, [](ADSR& e, double v){ e.release = v; });

        connect(adsrView, &AdsrEditorView::adsrChanged, this, [this, sg, pm, propName, attackKnob, decayKnob, sustainKnob, releaseKnob](const QUuid& id, const ADSR& newAdsr) {
            if (m_isUpdatingUI) return;
            attackKnob->setValue(newAdsr.attack);
            decayKnob->setValue(newAdsr.decay);
            sustainKnob->setValue(newAdsr.sustain);
            releaseKnob->setValue(newAdsr.release);
            QJsonObject oldJson = sg->toJson();
            SampleGroup newSg = *sg;
            if (propName == "ampEnv") newSg.ampEnv = newAdsr;
            else newSg.modEnv = newAdsr;
            pm->getUndoStack()->push(new ModifyPropertyCommand(pm, m_currentSgId, propName, oldJson, newSg.toJson()));
        });

        tabLayout->addWidget(knobsPanel);
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
    
    if (macrosWidget) {
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->addWidget(macrosWidget, 0);
        vLayout->addWidget(columnsWidget, 1);
        m_layout->addLayout(vLayout);
    } else {
        m_layout->addWidget(columnsWidget);
    }
    
    m_isUpdatingUI = false;
}
