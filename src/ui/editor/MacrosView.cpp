#include "MacrosView.h"
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QTimer>
#include <QScrollBar>
#include "../../commands/ModifyPropertyCommand.h"
#include "../components/SynthKnobWidget.h"
#include "../components/ModRoutingWidget.h"
#include "../components/ModSourceHelper.h"

MacrosView::MacrosView(ProjectManager* pm, QWidget* parent) 
    : QWidget(parent), m_pm(pm) {
    
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    setLayout(m_layout);
    
    connect(m_pm->getAudioState(), &AudioStateModel::globalMacrosChanged, this, &MacrosView::updateForm);
    connect(m_pm, &ProjectManager::nodeModified, this, [this](const QUuid& id, const QString& prop) {
        if (m_isUpdating) return;
        if (prop == "routings") {
            updateForm();
        }
    });
    connect(m_pm, &ProjectManager::projectLoaded, this, [this]() {
        m_focusedMacroId = QUuid();
        updateForm();
    });
    
    updateForm();
}

void MacrosView::updateForm() {
    m_isUpdating = true;
    
    QScrollArea* oldScroll = findChild<QScrollArea*>();
    if (oldScroll && oldScroll->verticalScrollBar()) {
        m_savedScrollPos = oldScroll->verticalScrollBar()->value();
    }
    
    while (QLayoutItem* item = m_layout->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    
    QWidget* container = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(container);
    scrollLayout->setContentsMargins(15, 15, 15, 15);
    
    // -- TOP SECTION: MACRO KNOBS --
    QWidget* topSection = new QWidget();
    topSection->setObjectName("NeumorphicCard");
    topSection->setAttribute(Qt::WA_StyledBackground, true);
    QVBoxLayout* topLayout = new QVBoxLayout(topSection);
    
    QLabel* topTitle = new QLabel("<b>Global Macros</b>");
    topTitle->setObjectName("ModuleHeader");
    topLayout->addWidget(topTitle);
    
    QHBoxLayout* knobsLayout = new QHBoxLayout();
    knobsLayout->setSpacing(20);
    
    const auto& macros = m_pm->getAudioState()->getGlobalMacros();
    if (m_focusedMacroId.isNull() && !macros.isEmpty()) {
        m_focusedMacroId = macros.first().id;
    } else if (macros.isEmpty()) {
        m_focusedMacroId = QUuid();
    }
    
    for (const auto& macro : macros) {
        QVBoxLayout* kl = new QVBoxLayout();
        kl->setSpacing(4);
        
        QLabel* nl = new QLabel(macro.name);
        nl->setStyleSheet("font-size: 12px; font-weight: 600;");
        kl->addWidget(nl, 0, Qt::AlignCenter);
        
        SynthKnobWidget* knob = new SynthKnobWidget();
        knob->setFixedSize(48, 48);
        knob->setMinimum(0.0);
        knob->setMaximum(1.0);
        knob->setValue(macro.value);
        
        connect(knob, &SynthKnobWidget::valueChanged, this, [this, macro](double v) {
            if (m_isUpdating) return;
            GlobalMacro updated = macro;
            updated.value = v;
            m_pm->getAudioState()->updateGlobalMacro(macro.id, updated);
            m_pm->setDirty(true);
        });
        
        kl->addWidget(knob, 0, Qt::AlignCenter);
        
        QHBoxLayout* btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(4);
        
        QPushButton* focusBtn = new QPushButton("Focus");
        focusBtn->setCheckable(true);
        if (m_focusedMacroId == macro.id) focusBtn->setChecked(true);
        
        connect(focusBtn, &QPushButton::clicked, this, [this, macro]() {
            m_focusedMacroId = macro.id;
            updateForm();
        });
        
        QPushButton* removeBtn = new QPushButton("X");
        removeBtn->setFixedSize(20, 20);
        removeBtn->setStyleSheet("color: #e81123; border: none; font-weight: bold;");
        connect(removeBtn, &QPushButton::clicked, this, [this, macro]() {
            m_pm->getAudioState()->removeGlobalMacro(macro.id);
        });
        
        btnLayout->addWidget(focusBtn);
        btnLayout->addWidget(removeBtn);
        kl->addLayout(btnLayout);
        
        knobsLayout->addLayout(kl);
    }
    
    QPushButton* addMacroBtn = new QPushButton("+ Add Macro");
    addMacroBtn->setFixedSize(80, 80);
    addMacroBtn->setStyleSheet("background-color: transparent; border: 2px dashed #444; border-radius: 8px; color: #777; font-weight: bold;");
    connect(addMacroBtn, &QPushButton::clicked, this, [this, macros]() {
        GlobalMacro m;
        m.name = QString("Macro %1").arg(macros.size() + 1);
        m_pm->getAudioState()->addGlobalMacro(m);
    });
    knobsLayout->addWidget(addMacroBtn);
    knobsLayout->addStretch();
    
    topLayout->addLayout(knobsLayout);
    scrollLayout->addWidget(topSection);
    
    // -- BOTTOM SECTION: ROUTING MATRIX --
    if (!m_focusedMacroId.isNull()) {
        QString macroSource = "MACRO_" + m_focusedMacroId.toString(QUuid::WithoutBraces);
        
        QLabel* routingTitle = new QLabel(QString("<b>Routing Matrix for Focused Macro</b>"));
        routingTitle->setStyleSheet("font-size: 16px; color: #eee; margin-top: 20px; margin-bottom: 10px;");
        routingTitle->setAlignment(Qt::AlignCenter);
        scrollLayout->addWidget(routingTitle);
        
        QVector<SampleGroup*> rootGroups;
        for (const auto& pair : m_pm->getAllNodes()) {
            if (pair.second->type == "SampleGroup") {
                SampleGroup* sg = static_cast<SampleGroup*>(pair.second.get());
                if (sg->synthParentId.isNull()) rootGroups.append(sg);
            }
        }
        
        std::sort(rootGroups.begin(), rootGroups.end(), [](SampleGroup* a, SampleGroup* b) { return a->name < b->name; });
        
        for (SampleGroup* rootSg : rootGroups) {
            QUuid rootId = rootSg->id;
            
            QVector<SampleGroup*> treeGroups;
            treeGroups.append(rootSg);
            for (const auto& pair : m_pm->getAllNodes()) {
                if (pair.second->type == "SampleGroup") {
                    SampleGroup* child = static_cast<SampleGroup*>(pair.second.get());
                    if (child->synthParentId == rootId) treeGroups.append(child);
                }
            }
            
            // Check if there are any routings for this macro in this tree
            bool hasRoutings = false;
            for (SampleGroup* sg : treeGroups) {
                for (const auto& r : sg->routings) {
                    if (r.source == macroSource) { hasRoutings = true; break; }
                }
            }
            
            QWidget* oscSection = new QWidget();
            oscSection->setObjectName("NeumorphicCard");
            oscSection->setAttribute(Qt::WA_StyledBackground, true);
            QVBoxLayout* oscLayout = new QVBoxLayout(oscSection);
            
            QString displayName = rootSg->name;
            if (displayName.isEmpty()) displayName = rootSg->isSynthContainer ? "Synth" : "Group";
            
            QHBoxLayout* headerLayout = new QHBoxLayout();
            QLabel* tagLabel = new QLabel(rootSg->isSynthContainer ? "SYNTH" : "SAMPLER");
            tagLabel->setStyleSheet(QString("background-color: %1; color: #111; font-weight: bold; font-size: 10px; padding: 2px 6px; border-radius: 4px;")
                .arg(rootSg->isSynthContainer ? "#32dc64" : "#ff9632"));
            headerLayout->addWidget(tagLabel);
            QLabel* oscTitle = new QLabel(displayName);
            oscTitle->setObjectName("ModuleHeader");
            headerLayout->addWidget(oscTitle);
            headerLayout->addStretch();
            oscLayout->addLayout(headerLayout);
            
            QHBoxLayout* slotsLayout = new QHBoxLayout();
            slotsLayout->setSpacing(10);
            
            for (SampleGroup* sg : treeGroups) {
                QUuid sgId = sg->id;
                bool isChild = (sg != rootSg);
                
                auto saveRoutings = [this, sgId](const QVector<ModRouting>& newR) {
                    if (!m_isUpdating) {
                        Node* n = m_pm->getNode(sgId);
                        if (n && n->type == "SampleGroup") {
                            SampleGroup* sg = static_cast<SampleGroup*>(n);
                            QJsonArray oldArr, newArr;
                            for (const auto& r : sg->routings) oldArr.append(r.toJson());
                            for (const auto& r : newR) newArr.append(r.toJson());
                            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, sgId, "routings", oldArr, newArr));
                        }
                    }
                };
                
                for (int i = 0; i < sg->routings.size(); ++i) {
                    const ModRouting& r = sg->routings[i];
                    if (r.source != macroSource) continue;
                    
                    QWidget* slot = new QWidget();
                    slot->setObjectName("NeumorphicInner");
                    slot->setAttribute(Qt::WA_StyledBackground, true);
                    QVBoxLayout* slotLayout = new QVBoxLayout(slot);
                    
                    QStringList baseDestinations = {"Pitch", "Volume", "Pan", "Cutoff", "Resonance", "AmpEnv_Attack", "AmpEnv_Decay", "AmpEnv_Sustain", "AmpEnv_Release"};
                    QStringList destinations;
                    for (const QString& d : baseDestinations) {
                        if (isChild) destinations.append(QString("%1 - %2").arg(d, sg->name));
                        else destinations.append(d);
                    }
                    
                    QList<ModSourceHelper::ModSource> allSources = ModSourceHelper::getAvailableSources(m_pm);
                    QList<ModSourceHelper::ModSource> sources;
                    for (const auto& s : allSources) {
                        if (s.id == macroSource) {
                            sources.append(s);
                            break;
                        }
                    }
                    if (sources.isEmpty()) sources.append({macroSource, "Macro"});
                    ModRoutingWidget* rw = new ModRoutingWidget(r, sources, destinations, false, slot);
                    
                    auto getBaseValue = [sg](const QString& destStr) -> double {
                        QString d = destStr.split(" - ").first();
                        if (d == "Pan") return sg->pan;
                        if (d == "Volume") return sg->volume;
                        if (d == "Pitch") return sg->tuning;
                        if (d == "Cutoff") return sg->filterCutoff;
                        if (d == "Resonance") return sg->filterResonance;
                        if (d == "AmpEnv_Attack") return sg->ampEnv.attack;
                        if (d == "AmpEnv_Decay") return sg->ampEnv.decay;
                        if (d == "AmpEnv_Sustain") return sg->ampEnv.sustain;
                        if (d == "AmpEnv_Release") return sg->ampEnv.release;
                        return 0.0;
                    };
                    rw->setBaseValue(getBaseValue(r.destination));
                    
                    connect(rw, &ModRoutingWidget::routingEditingFinished, this, [sgId, i, saveRoutings, this](const ModRouting& newR) {
                        if (m_isUpdating) return;
                        Node* n = m_pm->getNode(sgId);
                        if (n && n->type == "SampleGroup") {
                            SampleGroup* sg = static_cast<SampleGroup*>(n);
                            auto copy = sg->routings; copy[i] = newR; saveRoutings(copy);
                        }
                    }, Qt::QueuedConnection);
                    
                    connect(rw, &ModRoutingWidget::baseValueEditingFinished, this, [sgId, i, this, rw](double newBase) {
                        if (m_isUpdating) return;
                        Node* n = m_pm->getNode(sgId);
                        if (n && n->type == "SampleGroup") {
                            SampleGroup* sg = static_cast<SampleGroup*>(n);
                            ModRouting r = rw->getRouting();
                            QJsonObject oldState = sg->toJson();
                            if (r.destination == "Pan") sg->pan = newBase;
                            else if (r.destination == "Volume") sg->volume = newBase;
                            else if (r.destination == "Pitch") sg->tuning = newBase;
                            else if (r.destination == "Cutoff") sg->filterCutoff = newBase;
                            else if (r.destination == "Resonance") sg->filterResonance = newBase;
                            else if (r.destination == "AmpEnv_Attack") sg->ampEnv.attack = newBase;
                            else if (r.destination == "AmpEnv_Decay") sg->ampEnv.decay = newBase;
                            else if (r.destination == "AmpEnv_Sustain") sg->ampEnv.sustain = newBase;
                            else if (r.destination == "AmpEnv_Release") sg->ampEnv.release = newBase;
                            m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, sgId, "", oldState, sg->toJson()));
                        }
                    }, Qt::QueuedConnection);
                    
                    connect(rw, &ModRoutingWidget::deleteRequested, this, [sgId, i, saveRoutings, this]() {
                        Node* n = m_pm->getNode(sgId);
                        if (n && n->type == "SampleGroup") {
                            SampleGroup* sg = static_cast<SampleGroup*>(n);
                            auto copy = sg->routings; copy.removeAt(i); saveRoutings(copy);
                        }
                    }, Qt::QueuedConnection);
                    
                    slotLayout->addWidget(rw);
                    slotsLayout->addWidget(slot);
                }
            }
            
            QPushButton* addBtn = new QPushButton("+ Add Routing");
            addBtn->setObjectName("ModAddBtn");
            addBtn->setFixedSize(140, 140);
            connect(addBtn, &QPushButton::clicked, [this, rootId, macroSource](){
                Node* n = m_pm->getNode(rootId);
                if (n && n->type == "SampleGroup") {
                    SampleGroup* sg = static_cast<SampleGroup*>(n);
                    auto copy = sg->routings;
                    ModRouting r; r.source = macroSource; r.destination = "Pitch"; r.amount = 0.0;
                    copy.append(r);
                    QJsonArray oldArr, newArr;
                    for (const auto& routing : sg->routings) oldArr.append(routing.toJson());
                    for (const auto& routing : copy) newArr.append(routing.toJson());
                    m_pm->getUndoStack()->push(new ModifyPropertyCommand(m_pm, rootId, "routings", oldArr, newArr));
                }
            });
            slotsLayout->addWidget(addBtn);
            slotsLayout->addStretch();
            
            oscLayout->addLayout(slotsLayout);
            scrollLayout->addWidget(oscSection);
        }
    }
    
    scrollLayout->addStretch();
    scrollArea->setWidget(container);
    m_layout->addWidget(scrollArea);
    
    m_isUpdating = false;
    QTimer::singleShot(0, this, [this]() {
        QScrollArea* scroll = findChild<QScrollArea*>();
        if (scroll && scroll->verticalScrollBar()) scroll->verticalScrollBar()->setValue(m_savedScrollPos);
    });
}
